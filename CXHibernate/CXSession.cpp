////////////////////////////////////////////////////////////////////////
//
// File: CXSession.cpp
//
// Copyright (c) 2015-2022 ir. W.E. Huisman
// All rights reserved
//
// Permission is hereby granted, free of charge, to any person obtaining a copy of 
// this software and associated documentation files (the "Software"), 
// to deal in the Software without restriction, including without limitation the rights 
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, 
// and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in all copies 
// or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, 
// INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. 
// IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, 
// WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION 
// WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//
// Last version date: See CXHibernate.h
// Version number:    See CXHibernate.h
//
#include "stdafx.h"
#include "CXSession.h"
#include "CXClass.h"
#include "CXPrimaryHash.h"
#include "CXRole.h"
#include <AutoCritical.h>
#include <SQLQuery.h>
#include <SQLTransaction.h>
#include <SQLAutoDBS.h>
#include <SOAPMessage.h>
#include <HTTPClient.h>
#include <ServiceReporting.h>
#include <io.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////////////
// Static functions for logging of the SQLDatbase

static void CALLBACK CXHLogPrint(void* p_context,LPCTSTR p_text)
{
  hibernate.Log(hibernate.GetLogLevel(),false,p_text);
}

static int CALLBACK CXHLogLevel(void* p_context)
{
  return hibernate.GetLogLevel();
}

//////////////////////////////////////////////////////////////////////////
//
// The CXSesson object
//
//////////////////////////////////////////////////////////////////////////


// CTOR Default session (slave side of the session)
CXSession::CXSession(CString p_sessionKey)
          :m_sessionKey(p_sessionKey)
          ,m_role(CXH_Internet_role)
{
  // Create the cache lock
  InitializeCriticalSection(&m_lock);
}

// CTOR Filestore session
CXSession::CXSession(CString p_sessionKey,CString p_directory)
          :m_sessionKey(p_sessionKey)
          ,m_baseDirectory(p_directory)
          ,m_role(CXH_Filestore_role)
{
  // Create the cache lock
  InitializeCriticalSection(&m_lock);
}

// CTOR Master session
CXSession::CXSession(CString p_sessionKey,CString p_database,CString p_user,CString p_password)
          :m_sessionKey(p_sessionKey)
          ,m_role(CXH_Database_role)
{
  // Create the cache lock
  InitializeCriticalSection(&m_lock);
  SetDatabaseConnection(p_database,p_user,p_password);
}

// DTOR: Free all tables and records
CXSession::~CXSession()
{
  // Destroy the caches
  ClearCache();
  ClearClasses();

  // Destroy database (if any)
  if(m_databasePool && m_ownPool)
  {
    delete m_databasePool;
    m_databasePool = nullptr;
  }

  // Destroy HTTP client
  if(m_client)
  {
    delete m_client;
    m_client = nullptr;
  }

  // Remove our cache lock
  DeleteCriticalSection(&m_lock);
}

// And closing it again
void
CXSession::CloseSession()
{
  hibernate.Log(CXH_LOG_ACTIONS,true,_T("Close session: %s"),m_sessionKey);
  AutoCritSec lock(&m_lock);

  // If create/drop cycle testing, try to drop the database
  if(m_use == SESS_Create)
  {
    TryDropDatabase();
  }
  else
  {
    // If still an open database: synchronize our caches
    if(m_databasePool && GetDatabaseIsOpen())
    {
      Synchronize();
    }
  }

  // Remove our session from the global registration
  hibernate.RemoveSession(this);
}

// Changing the role of our session
void
CXSession::ChangeRole(CXHRole p_role)
{
  hibernate.Log(CXH_LOG_ACTIONS,true,_T("Changing session [%s] role to: %s"),m_sessionKey,CXRoleToString(p_role));
  m_role = p_role;
}

// See if we have an opened database
bool 
CXSession::GetDatabaseIsOpen()
{
  SQLAutoDBS dbs(*m_databasePool,m_dbsConnection);

  if(dbs.Valid())
  {
    return dbs->IsOpen();
  }
  return false;
}

// Alternate database 
void 
CXSession::SetDatabasePool(SQLDatabasePool* p_pool)
{
  AutoCritSec lock(&m_lock);

  if(m_databasePool && m_ownPool)
  {
    delete m_databasePool;
  }
  m_databasePool = p_pool;
  m_ownPool      = false;
  ChangeRole(CXH_Database_role);

  GetMetaSessionInfo();
}

// Getting our database
SQLDatabasePool*
CXSession::GetDatabasePool()
{
  if(m_databasePool)
  {
    // Do we need to create a schema?
    if(m_use == SESS_Create)
    {
      AutoCritSec lock(&m_lock);
      TryCreateDatabase();
    }
    return m_databasePool;
  }
  throw StdException(_T("Cannot connect a database to your CX-Hibernate session!"));
}

// Getting the connection name
CString
CXSession::GetDatabaseConnection()
{
  return m_dbsConnection;
}

// Specify a single database connection in a pool
void
CXSession::SetDatabaseConnection(CString p_datasource,CString p_user,CString p_password)
{
  AutoCritSec lock(&m_lock);

  if(m_databasePool == nullptr)
  {
    m_databasePool = new SQLDatabasePool();
    m_ownPool      = true;
    m_databasePool->RegisterLogContext(hibernate.GetLogLevel(),CXHLogLevel,CXHLogPrint,this);

    m_databasePool->ReadConnections();
  }
  m_databasePool->AddConnection(p_datasource,p_datasource,p_user,p_password,_T(""));

  m_dbsConnection = p_datasource;
  m_dbsCatalog    = p_datasource;
  m_dbsUser       = p_user;
  m_dbsPassword   = p_password;
  // Clearly we want to do database exchanges
  m_role = CXH_Database_role;
}

// Specify default connection to use in the pool
void
CXSession::SetDatabaseConnection(CString p_connectionName)
{
  m_dbsConnection = p_connectionName;
}

void
CXSession::SetDatabaseLogger(LOGLEVEL p_loglevel,LOGPRINT p_logger,void* p_context)
{
  m_printCallback = p_logger;
  m_levelCallback = p_loglevel;
  m_callbkContext = p_context;
}

// Setting an alternate filestore location
void 
CXSession::SetFilestore(CString p_directory)
{
  m_role = CXH_Filestore_role;
  m_baseDirectory = p_directory;
}

// Setting an alternate internet location
void
CXSession::SetInternet(CString p_url)
{
  m_role = CXH_Internet_role;
  m_url = p_url;
}

// Add a class to the session
bool
CXSession::AddClass(CXClass* p_class)
{
  // Lock the caches
  AutoCritSec lock(&m_lock);

  // Getting the table name
  CString name = p_class->GetName();
  name.MakeLower();

  // Add to the map of tables, but only if we not had it previously
  ClassMap::iterator it = m_classes.find(name);
  if(it != m_classes.end())
  {
    return false;
  }

  // Store the class definition
  m_classes.insert(std::make_pair(name,p_class));

  // Create a caching object in the cache
  ObjectCache* objcache = new ObjectCache();
  m_cache.insert(std::make_pair(name,objcache));

  hibernate.Log(CXH_LOG_ACTIONS,true,_T("Adding class [%s] to session [%s] "),name,m_sessionKey);
  return true;
}

// Finding a class
CXClass*
CXSession::FindClass(CString p_name)
{
  // Lock the caches
  AutoCritSec lock(&m_lock);

  p_name.MakeLower();
  ClassMap::iterator it = m_classes.find(p_name);
  if(it != m_classes.end())
  {
    return it->second;
  }
  return nullptr;
}

// Get a map to all classes
ClassMap&
CXSession::GetClasses()
{
  return m_classes;
}

// Loading the general configuration XML
void
CXSession::LoadConfiguration(XMLMessage& p_config)
{
  // Lock the caches
  AutoCritSec lock(&m_lock);

  // Load session parameters
  CString role = p_config.GetElement(_T("session_role"));
  m_role = CXStringToRole(role);

  CString use = p_config.GetElement(_T("database_use"));
  m_use = CXStringToSessionUse(use);

  // Load all classes
  XMLElement* theclass = p_config.FindElement(_T("class"));
  while(theclass)
  {
    CXClass* superclass = nullptr;
    CString    name = p_config.GetElement(theclass,_T("name"));
    CString   super = p_config.GetElement(theclass,_T("super"));
    CString discrim = p_config.GetElement(theclass,_T("discriminator"));

    // Find any superclass
    if(!super.IsEmpty())
    {
      superclass = FindClass(super);
    }
    
    // Create class, superclass & table
    CXClass* newclass = new CXClass(name,discrim,superclass);

    // Perform the rest of the loading of the class
    if(newclass->LoadMetaInfo(this,p_config,theclass))
    {
      AddClass(newclass);
    }
    else
    {
      delete newclass;
      throw StdException(_T("Cannot load class: ") + name);
    }
    // Find next class
    theclass = p_config.GetElementSibling(theclass);
  }

  // Perform late binding of associations
  theclass = p_config.FindElement(_T("class"));
  while(theclass)
  {
    CString  name = p_config.GetElement(theclass, _T("name"));
    name.MakeLower();
    ClassMap::iterator it = m_classes.find(name);
    if(it != m_classes.end())
    {
      CXClass* newclass = it->second;
      if (newclass)
      {
        newclass->LoadMetaInfoAssociations(p_config,theclass,this);
      }
    }
    theclass = p_config.GetElementSibling(theclass);
  }

  // Late linking of subclasses
  for(auto& cl : m_classes)
  {
    cl.second->LinkClasses(this);
  }
}

// Saving the general configuration XML.
void
CXSession::SaveConfiguration(XMLMessage& p_config)
{
  AutoCritSec lock(&m_lock);

  // Save session parameters
  p_config.AddElement(nullptr,_T("session_role"),XDT_String,CXRoleToString(m_role));
  p_config.AddElement(nullptr,_T("database_use"),XDT_String,CXSessionUseToString(m_use));

  // Save all class info
  for(auto& cl : m_classes)
  {
    cl.second->SaveMetaInfo(p_config,nullptr);
  }
}

//////////////////////////////////////////////////////////////////////////
//
// OBJECT INTERFACE
//
//////////////////////////////////////////////////////////////////////////

// OBJECT INTERFACE
CXObject*
CXSession::CreateObject(CString p_className)
{
  CXClass* theClass = FindClass(p_className);
  if(theClass)
  {
    CreateCXO create = theClass->GetCreateCXO();
    CXObject* object = (*create)();
    object->SetClass(theClass);
    return object;
  }
  hibernate.Log(CXH_LOG_ERRORS,true,_T("Session [%s] Cannot create object for class: %s"),m_sessionKey,p_className);
  return nullptr;
}

//////////////////////////////////////////////////////////////////////////
//
// OBJECT INTERFACE
//
//////////////////////////////////////////////////////////////////////////

CXObject*
CXSession::Load(CString p_className,int p_primary)
{
  SQLVariant prim(p_primary);
  VariantSet set;
  set.push_back(&prim);

  return Load(p_className,set);
}

CXObject*
CXSession::Load(CString p_className,CString p_primary)
{
  SQLVariant prim(p_primary);
  VariantSet set;
  set.push_back(&prim);

  return Load(p_className, set);
}

CXObject*
CXSession::Load(CString p_className,SQLVariant* p_primary)
{
  VariantSet set;
  set.push_back(p_primary);

  return Load(p_className,set);
}

CXObject*
CXSession::Load(CString p_className,VariantSet& p_primary)
{
  // Search in cache
  CXObject* object = FindObjectInCache(p_className,p_primary);
  if(object)
  {
    return object;
  }

  // If not found, search in database / SOAP connection
  if(m_role == CXH_Database_role)
  {
    object = FindObjectInDatabase(p_className,p_primary);
  }
  else if(m_role == CXH_Internet_role)
  {
    // Query for object on SOAP connection
    object = FindObjectOnInternet(p_className,p_primary);
  }
  else
  {
    // Query for object in the filestore
    object = FindObjectInFilestore(p_className,p_primary);
  }

  // Place in cache
  if(object && object->IsPersistent())
  {
    if(AddObjectInCache(object))
    {
      // Almost done. Call the trigger (if any)
      // Always called after loading the object. Cannot abort the 'load' action
      CallOnLoad(object);

      if(hibernate.GetLogLevel())
      {
        hibernate.Log(CXH_LOG_ACTIONS,true,_T("Loading object [%s:%s]"),p_className,object->Hashcode());
        if(hibernate.GetLogLevel() >= CXH_LOG_DEBUG)
        {
          object->LogObject();
        }
      }
      return object;
    }
    delete object;
  }
  hibernate.Log(CXH_LOG_ERRORS,true,_T("Session [%s] Cannot load object for class: %s"),m_sessionKey,p_className);
  return nullptr;
}

CXResultSet
CXSession::Load(CString p_tableName,SQLFilter* p_filter, CString p_orderBy /*= _T("")*/)
{
  SQLFilterSet set;
  set.AddFilter(p_filter);

  return Load(p_tableName,set,p_orderBy);
}

CXResultSet
CXSession::Load(CString p_className,SQLFilterSet& p_filters, CString p_orderBy /*= _T("")*/)
{
  CXResultSet set;

  if(m_role == CXH_Database_role)
  {
    set = SelectObjectsFromDatabase(p_className,p_filters,p_orderBy);
  }
  else if(m_role == CXH_Internet_role)
  {
    set = SelectObjectsFromInternet(p_className,p_filters,p_orderBy);
  }
  else // CXH_Filestore_role
  {
    set = SelectObjectsFromFilestore(p_className,p_filters,p_orderBy);
  }

  // Call OnLoad for all objects in the set
  // Always called after loading the entire set. Cannot abort the 'load' action
  for(auto& object : set)
  {
    CallOnLoad(object);
  }

  // Log the object as one of the set
  if(hibernate.GetLogLevel())
  {
    for(auto& object : set)
    {
      hibernate.Log(CXH_LOG_ACTIONS,true,_T("Loading object [%s:%s]"),p_className,object->Hashcode());
      if(hibernate.GetLogLevel() >= CXH_LOG_DEBUG)
      {
        object->LogObject();
      }
    }
  }
  return set;
}

void
CXSession::CheckReadOnly(CXObject* p_object)
{
  if(p_object->GetReadOnly())
  {
    XString error;
    error.Format(_T("Session [%s] Cannot save read-only object: %s"),m_sessionKey,p_object->Hashcode());
    hibernate.Log(CXH_LOG_ERRORS,false,error);
    throw StdException(error);
  }
}

bool
CXSession::Save(CXObject* p_object)
{
  CheckReadOnly(p_object);
  if(p_object->IsTransient())
  {
    return Insert(p_object);
  }
  else
  {
    return Update(p_object);
  }
}

// Update an object
bool
CXSession::Update(CXObject* p_object,SQLDatabase* p_dbs /*=nullptr*/)
{
  bool result = false;

  CheckReadOnly(p_object);

  // Call OnUpdate trigger for the object
  bool canUpdate = CallOnUpdate(p_object);

  // Only update if trigger allows so
  if(canUpdate)
  {
    if(m_role == CXH_Database_role)
    {
      result = UpdateObjectInDatabase(p_object,p_dbs);
    }
    else if(m_role == CXH_Internet_role)
    {
      result = UpdateObjectInInternet(p_object);
    }
    else // CXH_Filestore_role
    {
      result = UpdateObjectInFilestore(p_object);
    }
  }

  // Log the object as updated
  if(result && hibernate.GetLogLevel())
  {
    CString action = canUpdate ? _T("Update object") : _T("CANNOT UPDATE");
    hibernate.Log(CXH_LOG_ACTIONS, true, _T("%s [%s:%s]"),action,p_object->ClassName(),p_object->Hashcode());
    if (hibernate.GetLogLevel() >= CXH_LOG_DEBUG)
    {
      p_object->LogObject();
    }
  }
  return result;
}

bool
CXSession::Insert(CXObject* p_object)
{
  bool result = false;

  CheckReadOnly(p_object);

  // Call the OnInsert trigger
  bool canInsert = CallOnInsert(p_object);

  // Only insert if triggers allows us so
  if(canInsert)
  {
    if(m_role == CXH_Database_role)
    {
      result = InsertObjectInDatabase(p_object);
    }
    else if(m_role == CXH_Internet_role)
    {
      // Insert as a SOAP Message
      result = InsertObjectInInternet(p_object);
    }
    else // CXH_Filestore_role
    {
      result = InsertObjectInFilestore(p_object);
    }
  }

  // Add object to the cache
  if(result && p_object->IsPersistent())
  {
    result = AddObjectInCache(p_object);

    // Log the object as inserted
    if(hibernate.GetLogLevel())
    {
      CString action = canInsert ? _T("Inserted object") : _T("CANNOT INSERT object");
      hibernate.Log(CXH_LOG_ACTIONS,true,_T("%s [%s:%s]"),action,p_object->ClassName(),p_object->Hashcode());
      if(hibernate.GetLogLevel() >= CXH_LOG_DEBUG)
      {
        p_object->LogObject();
      }
    }
  }
  return result;
}

bool
CXSession::Delete(CXObject* p_object)
{
  bool result = false;

  CheckReadOnly(p_object);

  // Call OnDelete trigger
  bool canDelete = CallOnDelete(p_object);

  // Log the object as deleted, before it's gone
  if(hibernate.GetLogLevel())
  {
    CString action = canDelete ? _T("Object to delete") : _T("CANNOT DELETE object");
    hibernate.Log(CXH_LOG_ACTIONS,true,_T("%s [%s:%s]"),action,p_object->ClassName(),p_object->Hashcode());
    if(hibernate.GetLogLevel() >= CXH_LOG_DEBUG)
    {
      p_object->LogObject();
    }
  }

  // Only delete if trigger allows us to
  if(canDelete)
  {
    if(m_role == CXH_Database_role)
    {
      result = DeleteObjectInDatabase(p_object);
    }
    else if(m_role == CXH_Internet_role)
    {
      result = DeleteObjectInInternet(p_object);
    }
    else // CXH_Filestore_role
    {
      result = DeleteObjectInFilestore(p_object);
    }
    hibernate.Log(CXH_LOG_ACTIONS,true,_T("Delete succeeded: %s"),result ? _T("yes") : _T("no"));
  }
  return result;
}

// Remove object from the result cache without any database/internet actions
bool
CXSession::RemoveObject(CXObject* p_object)
{
  return RemoveObjectFromCache(p_object);
}

bool
CXSession::RemoveObjects(CXResultSet& p_resultSet)
{
  AutoCritSec lock(&m_lock);

  size_t size = p_resultSet.size();
  for(auto& object : p_resultSet)
  {
    if(RemoveObject(object))
    {
      --size;
    }
  }
  return (size == 0);
}

// Flush all objects and dataset for the class
bool
CXSession::Flush(CString p_className,bool p_save /*=false*/)\
{
  AutoCritSec lock(&m_lock);

  // Synchronize first
  if(p_save)
  {
    Synchronize(p_className);
  }

  // Find the ObjectCache
  p_className.MakeLower();
  CXCache::iterator it = m_cache.find(p_className);
  if(it == m_cache.end())
  {
    return false;
  }

  // Remove the objects and the cache
  ObjectCache* objects = it->second;
  for(auto& object : *objects)
  {
    RemoveObject(object.second);
  }
  m_cache.erase(it);

  // Close the dataset
  CXClass* theClass = FindClass(p_className);
  if(theClass)
  {
    theClass->GetDataSet()->Close();
  }
  return true;
}

// Complete cache synchronize with the database, saving all results
bool
CXSession::Synchronize()
{
  // See if we do a database role
  if(m_role != CXH_Database_role)
  {
    return false;
  }

  AutoCritSec lock(&m_lock);

  // Getting a new mutation
  SQLAutoDBS dbs(*m_databasePool,m_dbsConnection);
  SQLTransaction trans(dbs,_T("synchronize"));
  dbs->RegisterLogContext(hibernate.GetLogLevel(),m_levelCallback,m_printCallback,m_callbkContext);

  // Walk our object cache
  for(auto& objcache : m_cache)
  {
    ObjectCache& objects = *objcache.second;
    for(auto& obj : objects)
    {
      CXObject*  object = obj.second;
      if(!object->GetReadOnly())
      {
        SQLRecord* record = object->GetDatabaseRecord();
        // Only relevant status is 'Updated'. Cannot be otherwise!
        if(record && (record->GetStatus() & SQL_Record_Updated))
        {
          SerializeDiscriminator(object,record);
          object->Serialize(*record);
          if(Update(object) == false)
          {
            // Implicit rollback transaction
            return false;
          }
        }
      }
    }
  }

  // Commit in the database
  trans.Commit();
  return true;
}

bool
CXSession::Synchronize(CString p_className)
{
  // See if we do a database role
  if(m_role != CXH_Database_role)
  {
    return false;
  }

  AutoCritSec lock(&m_lock);

  // Getting a new mutation
  SQLAutoDBS dbs(*m_databasePool,m_dbsConnection);
  SQLTransaction trans(dbs,_T("synchronize"));

  // Walk our object cache
  p_className.MakeLower();
  CXCache::iterator ch = m_cache.find(p_className);

  if(ch != m_cache.end())
  {
    ObjectCache& objects = *(ch->second);
    for(auto& obj : objects)
    {
      CXObject* object = obj.second;
      if(!object->GetReadOnly())
      {
        SQLRecord* record = object->GetDatabaseRecord();
        // Only relevant status is 'Updated'. Cannot be otherwise!
        if(record && (record->GetStatus() & SQL_Record_Updated))
        {
          SerializeDiscriminator(object,record);
          object->Serialize(*record);
          if(Update(object) == false)
          {
            // Implicit rollback transaction
            return false;
          }
        }
      }
    }
  }

  // Commit in the database
  trans.Commit();
  return true;
}

// Create a filestore name for a table
CString
CXSession::CreateFilestoreName(CXTable* p_table)
{
  CString tablename = p_table->GetFullQualifiedTableName();
  tablename.Replace(_T("."), _T("_"));

  CString fileName = m_baseDirectory;
  if (fileName.Right(1) != _T('\\'))
  {
    fileName += _T("\\");
  }

  WinFile dir(fileName);
  if(!dir.CreateDirectory())
  {
    return _T("");
  }

  // Add our table name
  fileName += tablename;
  fileName += _T(".xml");

  return fileName;
}

// Create a filestore name for an object
CString
CXSession::CreateFilestoreName(CXTable* p_table, VariantSet& p_primary)
{
  CString tablename = p_table->GetFullQualifiedTableName();
  tablename.Replace(_T("."), _T("_"));

  CString fileName = m_baseDirectory;
  if (fileName.Right(1) != _T('\\'))
  {
    fileName += _T("\\");
  }
  fileName += tablename;
  fileName += _T("\\");

  WinFile dir(fileName);
  if(!dir.CreateDirectory())
  {
    return _T("");
  }

  CString hash = CXPrimaryHash(p_primary);
  hash.Replace(_T("\002"), _T("_"));

  fileName += _T("Object_");
  fileName += hash;
  fileName += _T(".xml");

  return fileName;
}

// Saving a SOAP XML message on the file system
bool
CXSession::SaveSOAPMessage(SOAPMessage& p_message, CString p_fileName)
{
  return p_message.SaveFile(p_fileName);
}

// FOLLOW AN ASSOCIATION

// Getting the result of an association
CXResultSet
CXSession::FollowAssociation(CXObject* p_object,CString p_toClass,int p_value,CString p_associationName /*= ""*/)
{
  SQLVariant prim(p_value);
  VariantSet primary;
  primary.push_back(&prim);

  return FollowAssociation(p_object,p_toClass,primary,p_associationName);
}

CXResultSet
CXSession::FollowAssociation(CXObject* p_object,CString p_toClass,SQLVariant* p_value,CString p_associationName /*= ""*/)
{
  VariantSet primary;
  primary.push_back(p_value);

  return FollowAssociation(p_object,p_toClass,primary,p_associationName);
}

CXResultSet
CXSession::FollowAssociation(CXObject* p_object,CString p_toClass,VariantSet& p_value,CString p_associationName /*= ""*/)
{
  CXResultSet  set;
  SQLFilterSet filters;
  CXObject*     object = nullptr;
  CXClass*   fromClass = p_object->GetClass();
  CXAssociation* assoc = fromClass->FindAssociation(p_toClass,p_associationName);

  if(assoc == nullptr)
  {
    throw StdException(_T("Association not found to class: ") + p_toClass);
  }
  switch(assoc->m_assocType)
  {
    case ASSOC_MANY_TO_ONE: object = Load(p_toClass,p_value);
                            if(object)
                            {
                              set.push_back(object);
                            }
                            return set;
    case ASSOC_ONE_TO_MANY: fromClass->BuildFilter(assoc->m_attributes,p_value,filters);
                            return Load(p_toClass,filters);
    case ASSOC_MANY_TO_MANY:throw StdException(_T("Association type many-to-many not yet implemented!"));
    default:                break;
  }
  throw StdException(_T("Association type not found for association: ") + assoc->m_constraintName);
}

//////////////////////////////////////////////////////////////////////////
//
// PRIVATE
//
//////////////////////////////////////////////////////////////////////////

// Clear the cache of all objects of table <p_table>
// if no parameter given, clear the complete cache
void 
CXSession::ClearCache(CString p_className /*= ""*/)
{
  hibernate.Log(CXH_LOG_ACTIONS,true,_T("Clear object cache of session: %s"),m_sessionKey);

  if(m_cache.empty())
  {
    return;
  }

  // Lock the caches
  AutoCritSec lock(&m_lock);

  CXCache::iterator it = m_cache.begin();
  while (it != m_cache.end())
  {
    if (p_className.IsEmpty() || p_className.CompareNoCase(it->first) == 0)
    {
      for(auto& object : *it->second)
      {
        delete object.second;
      }
      it->second->clear();
      delete it->second;
      it = m_cache.erase(it);
    }
    else
    {
      ++it;
    }
  }
}

// Clear the class definition cache
void
CXSession::ClearClasses(CString p_className /*= ""*/)
{
  hibernate.Log(CXH_LOG_ACTIONS,true,_T("Clear class definition cache of session: %s"),m_sessionKey);

  // Lock the caches
  AutoCritSec lock(&m_lock);

  ClassMap::iterator it = m_classes.begin();
  while(it != m_classes.end())
  {
    if(p_className.IsEmpty() || p_className.CompareNoCase(it->first) == 0)
    {
      delete it->second;
      it = m_classes.erase(it);
    }
    else
    {
      ++it;
    }
  }
}

// Getting meta-session info from our database
void
CXSession::GetMetaSessionInfo()
{
  if(m_databasePool)
  {
    try
    {
      SQLAutoDBS dbs(*m_databasePool,m_dbsConnection);
      dbs->RegisterLogContext(hibernate.GetLogLevel(),m_levelCallback,m_printCallback,m_callbkContext);
      CString sql = dbs->GetSQLInfoDB()->GetSESSIONMyself();
      SQLQuery query(dbs);
      SQLTransaction trans(dbs,_T("session"));

      query.DoSQLStatement(sql);

      if(query.GetRecord())
      {
        m_metaInfo.m_session     = (CString) query[1];
        m_metaInfo.m_user        = (CString) query[2];
        m_metaInfo.m_terminal    = (CString) query[3];
        m_metaInfo.m_logonMoment = (CString) query[4];
        m_metaInfo.m_remoteIP    = (CString) query[5];
        m_metaInfo.m_processName = (CString) query[6];
        m_metaInfo.m_processID   = (CString) query[7];
      }
      trans.Commit();
    }
    catch(StdException& er)
    {
      // Error in silence ?
      SvcReportErrorEvent(0,true,_T(__FUNCTION__),_T("Error getting database session info: %s"),er.GetErrorMessage());
      UNREFERENCED_PARAMETER(er);
    }
  }
  if(hibernate.GetLogLevel())
  {
    hibernate.Log(CXH_LOG_DEBUG,true,_T("Hibernate session : %s"),m_sessionKey);
    hibernate.Log(CXH_LOG_DEBUG,true,_T("Database session  : %s"),m_metaInfo.m_session);
    hibernate.Log(CXH_LOG_DEBUG,true,_T("Database user     : %s"),m_metaInfo.m_user);
    hibernate.Log(CXH_LOG_DEBUG,true,_T("Database terminal : %s"),m_metaInfo.m_terminal);
    hibernate.Log(CXH_LOG_DEBUG,true,_T("Database logon    : %s"),m_metaInfo.m_logonMoment);
    hibernate.Log(CXH_LOG_DEBUG,true,_T("Database address  : %s"),m_metaInfo.m_remoteIP);
    hibernate.Log(CXH_LOG_DEBUG,true,_T("Processname in dbs: %s"),m_metaInfo.m_processName);
    hibernate.Log(CXH_LOG_DEBUG,true,_T("Database ID       : %s"),m_metaInfo.m_processID);
  }
}

// Getting a HTTP client object
HTTPClient*
CXSession::GetHTTPClient()
{
  if(m_client)
  {
    return m_client;
  }
  m_client = new HTTPClient();
  return m_client;
}


// Try to add an object to the cache
// Return values are:
// true  : Success: object added to the cache
// false : Failure: object was already present in the cache
// throw : Failure: Cache for the class of this object not found!!
bool
CXSession::AddObjectInCache(CXObject* p_object)
{
  // Lock the caches
  AutoCritSec lock(&m_lock);

  CString hash = p_object->Hashcode();
  CString className = p_object->GetClass()->GetName();
  className.MakeLower();

  CXCache::iterator it = m_cache.find(className);
  if(it == m_cache.end())
  {
    // Create a caching object in the cache
    ObjectCache* objcache = new ObjectCache();
    m_cache.insert(std::make_pair(className,objcache));
    it = m_cache.find(className);
  }
  if (it != m_cache.end())
  {
    ObjectCache::iterator tit = it->second->find(hash);
    if(tit == it->second->end())
    {
      it->second->insert(std::make_pair(hash,p_object));
      return true;
    }
    // Object already in the cache. Do not cache again!
    p_object->SetReadOnly(true);
    return false;
  }
  throw StdException(_T("Cache not found for class: ") + className);
}

// And remove again from the cache
bool
CXSession::RemoveObjectFromCache(CXObject* p_object)
{
  // Lock the caches
  AutoCritSec lock(&m_lock);
  bool removedFromCache = false;

  // Try to remove the primary record from the hibernate cache
  if(p_object->GetReadOnly() == false)
  {
    CString hash = p_object->Hashcode();
    CString className = p_object->GetClass()->GetName();
    className.MakeLower();

    CXCache::iterator it = m_cache.find(className);
    if(it != m_cache.end())
    {
      ObjectCache* tcache = it->second;
      ObjectCache::iterator tit = tcache->find(hash);
      if(tit != tcache->end())
      {
        // Destroy the CXObject derived object
        tcache->erase(tit);
        removedFromCache = true;
      }
    }
  }
  // Try to remove the object from the dataset
  // Also works for read-only objects!
  CXClass* cxclass = p_object->GetClass();
  bool removed = cxclass->GetDataSet()->ForgetRecord(p_object->GetDatabaseRecord(),true);

  if(removed || removedFromCache)
  {
    delete p_object;
    return true;
  }
  return false;
}

// Try to find an object in the cache
// It's a double map lookup (table, object)
CXObject*
CXSession::FindObjectInCache(CString p_className,VariantSet& p_primary)
{
  // Lock the caches
  AutoCritSec lock(&m_lock);

  p_className.MakeLower();
  CXCache::iterator it = m_cache.find(p_className);
  if (it != m_cache.end())
  {
    CString  hash;
    CXClass* theClass = FindClass(p_className);
    if(theClass && theClass->GetCalcHashcode())
    {
      hash = (*theClass->GetCalcHashcode())(p_primary);
    }
    else
    {
      hash = CXPrimaryHash(p_primary);
    }

    ObjectCache::iterator tit = it->second->find(hash);
    if (tit != it->second->end())
    {
      return tit->second;
    }
  }
  return nullptr;
}

// Try to find an object in the database
CXObject*
CXSession::FindObjectInDatabase(CString p_className,VariantSet& p_primary)
{
  // Find the class of the object
  CXClass* theClass = FindClass(p_className);

  SQLAutoDBS dbs(*GetDatabasePool(),GetDatabaseConnection());
  dbs->RegisterLogContext(hibernate.GetLogLevel(),m_levelCallback,m_printCallback,m_callbkContext);

  SQLRecord* record = theClass->SelectObjectInDatabase(dbs,nullptr,p_primary);
  if(record)
  {
    // Create our object by the creation factory
    CreateCXO create = theClass->GetCreateCXO();
    CXObject* object = (*create)();
    object->SetClass(theClass);

    // De-serialize the SQL Record to an CXObject derived object
    object->DeSerialize(*record);

    return object;
  }
  return nullptr;
}

// Try to find an object in the filestore
CXObject*
CXSession::FindObjectInFilestore(CString p_className,VariantSet& p_primary)
{
  CXClass* theClass = FindClass(p_className);
  CXTable* table = theClass->GetTable();

  // See if we can create a directory and filename
  CString filename = CreateFilestoreName(table,p_primary);
  if(filename.IsEmpty())
  {
    return nullptr;
  }

  // Test if we can access this file
  if(_taccess(filename, 0) == 0)
  {
    // Try to load the object
    SOAPMessage p_message;
    if(p_message.LoadFile(filename))
    {
      // Find our object (just one entity in the message)
      XMLElement* entity = p_message.FindElement(_T("Entity"));
      if(entity)
      {
        return LoadObjectFromXML(p_message,entity,theClass);
      }
    }
  }
  return nullptr;
}

// Try to find an object via the SOAP interface
CXObject*
CXSession::FindObjectOnInternet(CString p_className,VariantSet& p_primary)
{
  CXClass* theClass = FindClass(p_className);

  // Create a SOAP message for select
  CString namesp(DEFAULT_NAMESPACE);
  CString action(_T("CXH_Select"));
  SOAPMessage msg(namesp,action,SoapVersion::SOAP_12);
  XMLElement* entity = msg.SetParameter(_T("Entity"), _T(""));
  msg.SetAttribute(entity,_T("name"),p_className); 
  
  // Load filters in message
  theClass->BuildPrimaryKeyFilter(msg,entity,p_primary);

  // Send to client
  msg.SetURL(m_url);
  if(GetHTTPClient()->Send(&msg))
  {
    // Find our object (just one entity in the message)
    XMLElement* entity = msg.FindElement(_T("Entity"));
    if(entity)
    {
      return LoadObjectFromXML(msg,entity,theClass);
    }
  }
  CString httpError;
  CString error;
  error.Format(_T("Could not reach CXServer on: %s\n%s\n")
                ,m_url.GetString()
                ,msg.GetFault().GetString());
  GetHTTPClient()->GetError(&httpError);
  error += httpError;
  throw StdException(error);
}

CXObject*
CXSession::LoadObjectFromXML(SOAPMessage& p_message,XMLElement* p_entity,CXClass* p_class)
{
  // Create our object by the creation factory
  CreateCXO create = p_class->GetCreateCXO();
  CXObject* object = (*create)();
  object->SetClass(p_class);

  // Fill in our object from the message 
  object->DeSerialize(p_message,p_entity);

  // Can only log the object from the message!
  if (hibernate.GetLogLevel() >= CXH_LOG_DEBUG)
  {
    hibernate.Log(CXH_LOG_DEBUG, false,p_message.GetBodyPart());
  }
  return object;
}

CXResultSet
CXSession::SelectObjectsFromDatabase(CString p_className,SQLFilterSet& p_filters, CString p_orderBy /*= _T("")*/)
{
  CXResultSet set;

  // Find the CXTable object
  p_className.MakeLower();
  ClassMap::iterator it = m_classes.find(p_className);
  if (it == m_classes.end())
  {
    return set;
  }
  CXClass* theClass = it->second;
  CXTable* table = theClass->GetTable();

  // See if we have a data set
  SQLDataSet* dset = theClass->GetDataSet();
  if (dset == nullptr)
  {
    return set;
  }

  // Connect our database
  SQLAutoDBS dbs(*GetDatabasePool(),GetDatabaseConnection());
  dbs->RegisterLogContext(hibernate.GetLogLevel(),m_levelCallback,m_printCallback,m_callbkContext);
  dset->SetDatabase(dbs);

  // Create correct query
  theClass->BuildDefaultSelectQuery(dset,dbs->GetSQLInfoDB(),p_orderBy);

  // Propagate our filters
  dset->SetFilters(&p_filters);

  // NOW GO OPEN our dataset
  bool selected = false;
  int  startreading = dset->GetNumberOfRecords();
  if(dset->IsOpen())
  {
    dset->SetKeepDuplicates(true);
    selected = dset->Append();
  }
  else
  {
    selected = dset->Open();
  }
  if(selected)
  {
    int recnum = startreading;
    while(recnum >= 0)
    {
      SQLRecord* record = dset->GetRecord(recnum);
      if (record == nullptr)
      {
        break;
      }

      // Create our object by the creation factory
      CreateCXO create = theClass->GetCreateCXO();
      CXObject* object = (*create)();
      object->SetClass(theClass);

      // De-serialize the SQL Record to an CXObject derived object
      object->DeSerialize(*record);

      // Add object to the cache
      if(object->IsPersistent())
      {
        AddObjectInCache(object);
        // Keep in the return set
        set.push_back(object);
      }
      else
      {
        // LOG
        delete object;
      }

      // Getting the next record
      ++recnum;
    }
  }
  dset->SetFilters(nullptr);

  return set;
}

CXResultSet
CXSession::SelectObjectsFromFilestore(CString p_className,SQLFilterSet& p_filters,CString p_orderBy /*= _T("")*/)
{
  throw StdException(_T("Cannot select multiple objects from the filestore"));
}

CXResultSet
CXSession::SelectObjectsFromInternet(CString p_className,SQLFilterSet& p_filters,CString p_orderBy /*= _T("")*/)
{
  CXResultSet set;
  CXClass* theClass = FindClass(p_className);

  // Create a SOAP message for select
  CString namesp(DEFAULT_NAMESPACE);
  CString action(_T("CXH_Select"));
  SOAPMessage msg(namesp,action,SoapVersion::SOAP_12);
  XMLElement* entity = msg.SetParameter(_T("Entity"), _T(""));
  msg.SetAttribute(entity,_T("name"),p_className); 
  
  // Load filters in message
  BuildFilter(msg,entity,p_filters);

  // Send to client
  msg.SetURL(m_url);
  if(GetHTTPClient()->Send(&msg))
  {
    // Find our object (just one entity in the message)
    XMLElement* entity = msg.FindElement(_T("Entity"));
    while(entity)
    {
      CXObject* object = LoadObjectFromXML(msg,entity,theClass);

      // Add object to the cache
      if(object->IsPersistent())
      {
        if(AddObjectInCache(object) == false)
        {
          // LOG!
          delete object;
        }
        set.push_back(object);
      }
      else
      {
        // LOG!
        delete object;
      }
      // Getting next entity
      entity = msg.GetElementSibling(entity);
      if(entity->GetName().CompareNoCase(_T("Entity")))
      {
        break;
      }
    }
    return set;
  }
  CString httpError;
  CString error;
  error.Format(_T("Could not reach CXServer on: %s\n%s\n")
                ,m_url.GetString()
                ,msg.GetFault().GetString());
  GetHTTPClient()->GetError(&httpError);
  error += httpError;
  throw StdException(error);
}

bool
CXSession::UpdateObjectInDatabase(CXObject* p_object,SQLDatabase* p_dbs /*=nullptr*/)
{
  CXClass* theClass = p_object->GetClass();
  if(theClass == nullptr)
  {
    throw StdException(_T("Missing class on UPDATE of an object. Did you tinkle with the PrimaryKey?"));
  }

  bool result(false);
  if(p_dbs)
  {
    result = theClass->UpdateObjectInDatabase(p_dbs,nullptr,p_object,0);
  }
  else
  {
    // New mutation ID for this update action
    SQLAutoDBS dbs(*GetDatabasePool(),GetDatabaseConnection());
    dbs->RegisterLogContext(hibernate.GetLogLevel(),m_levelCallback,m_printCallback,m_callbkContext);

    SQLTransaction trans(dbs,_T("update"));

    result = theClass->UpdateObjectInDatabase(dbs,nullptr,p_object,0);
    if(result)
    {
      // Commit our transaction in the database
      trans.Commit();
    }
  }
  return result;
}

bool
CXSession::UpdateObjectInInternet(CXObject* p_object)
{
  // Create a SOAP message for select
  CString namesp(DEFAULT_NAMESPACE);
  CString action(_T("CXH_Update"));
  SOAPMessage msg(namesp, action, SoapVersion::SOAP_12);
  XMLElement* entity = msg.SetParameter(_T("Entity"), _T(""));
  msg.SetAttribute(entity,_T("name"),p_object->ClassName());

  // Now serialize our object into this message
  SerializeDiscriminator(p_object,msg,entity);
  p_object->Serialize(msg,entity);

  // Send to client
  msg.SetURL(m_url);
  if(GetHTTPClient()->Send(&msg))
  {
    CString result = msg.GetParameter(_T("CXResult"));
    if(result == _T("OK"))
    {
      return true;
    }
  }
  CString httpError;
  CString error;
  error.Format(_T("Could not reach CXServer on: %s\n%s\n")
               ,m_url.GetString()
               ,msg.GetFault().GetString());
  GetHTTPClient()->GetError(&httpError);
  error += httpError;
  throw StdException(error);
}

// Insert a new object in the database
bool
CXSession::InsertObjectInDatabase(CXObject* p_object)
{
  // Get class and check original constructor
  CXClass* theClass = p_object->GetClass();
  if(theClass == nullptr)
  {
    throw StdException(_T("Object without a connected class. Did you call CXSession::CreateObject(<classname>)??"));
  }

  // New mutation ID for this update action
  SQLAutoDBS dbs(*GetDatabasePool(),GetDatabaseConnection());
  dbs->RegisterLogContext(hibernate.GetLogLevel(),m_levelCallback,m_printCallback,m_callbkContext);
  SQLTransaction trans(dbs,_T("insert"));

  bool saved = theClass->InsertObjectInDatabase(dbs,nullptr,p_object,0);
  if(saved)
  {
    // Commit in the database
    trans.Commit();
  }
  return saved;
}

bool
CXSession::InsertObjectInInternet(CXObject* p_object)
{
  // Create a SOAP message for select
  CString namesp(DEFAULT_NAMESPACE);
  CString action(_T("CXH_Insert"));
  SOAPMessage msg(namesp, action, SoapVersion::SOAP_12);
  XMLElement* entity = msg.SetParameter(_T("Entity"), _T(""));
  msg.SetAttribute(entity,_T("name"),p_object->ClassName());

  // Now serialize our object into this message
  SerializeDiscriminator(p_object,msg,entity);
  p_object->Serialize(msg,entity);

  // Send to client
  msg.SetURL(m_url);
  if(GetHTTPClient()->Send(&msg))
  {
    CString result = msg.GetParameter(_T("CXResult"));
    if(result == _T("OK"))
    {
      // De-Serialize the result (gives us the new primary key)
      // And possibly changed attributes
      XMLElement* entity = msg.FindElement(_T("Entity"));
      p_object->DeSerialize(msg,entity);
      return true;
    }
  }
  CString httpError;
  CString error;
  error.Format(_T("Could not reach CXServer on: %s\n%s\n")
               ,m_url.GetString()
               ,msg.GetFault().GetString());
  GetHTTPClient()->GetError(&httpError);
  error += httpError;
  throw StdException(error);
}

bool
CXSession::DeleteObjectInDatabase(CXObject* p_object)
{
  // Finding our object's class
  CXClass* theClass = p_object->GetClass();
  if(theClass == nullptr)
  {
    throw StdException(_T("No class definition while deleting an object"));
  }
  // If object was **NOT** in the database, removal did work out OK!
  if(p_object->IsPersistent() == false)
  {
    return true;
  }

  // New mutation ID for this update action
  SQLAutoDBS dbs(*GetDatabasePool(),GetDatabaseConnection());
  dbs->RegisterLogContext(hibernate.GetLogLevel(),m_levelCallback,m_printCallback,m_callbkContext);
  SQLTransaction trans(dbs,_T("delete"));

  // Go delete the record
  bool deleted = theClass->DeleteObjectInDatabase(dbs,nullptr,p_object,0);
  if(deleted)
  {
    // Commit in the database first
    trans.Commit();

    // Remove the object from the cache, and make it transient
    if(RemoveObjectFromCache(p_object) == false)
    {
      // Could not be removed / deleted: so make it transient
      p_object->MakeTransient();
    }
  }
  return deleted;
}

// DML operations in the filestore
bool
CXSession::UpdateObjectInFilestore(CXObject* p_object)
{
  CXClass* theClass = p_object->GetClass();
  CXTable* table = theClass->GetTable();

  // See if we can create a directory and filename
  CString filename = CreateFilestoreName(table,p_object->GetPrimaryKey());
  if (filename.IsEmpty())
  {
    return false;
  }

  // Create a SOAP message object
  SOAPMessage msg(DEFAULT_NAMESPACE, _T("Object"));

  // Adding an entity with name attributes
  XMLElement* entity = msg.SetParameter(_T("Entity"), _T(""));
  if(!table->GetSchemaName().IsEmpty())
  {
    msg.SetAttribute(entity,_T("schema"),table->GetSchemaName());
  }
  msg.SetAttribute(entity,_T("name"),table->GetTableName());

  // Serialize our object
  SerializeDiscriminator(p_object,msg,entity);
  p_object->Serialize(msg,entity);

  // Test if we can access this file
  if(_taccess(filename, 0) == 0)
  {
    // Save to the file: overwriting the file
    return SaveSOAPMessage(msg,filename);
  }
  // File did not exist, non-existing objects cannot be updated!
  return false;
}

bool
CXSession::InsertObjectInFilestore(CXObject* p_object)
{
  CXClass* theClass = p_object->GetClass();
  CXTable* table = theClass->GetTable();

  // See if we can create a directory and filename
  CString filename = CreateFilestoreName(table,p_object->GetPrimaryKey());
  if(filename.IsEmpty())
  {
    return false;
  }

  // Create a SOAP message object
  CString namesp(DEFAULT_NAMESPACE);
  CString action(_T("Object"));
  SOAPMessage msg(namesp,action,SoapVersion::SOAP_12);
  XMLElement* entity = msg.SetParameter(_T("Entity"),_T(""));
  msg.SetAttribute(entity,_T("name"),table->GetTableName());

  // Serialize our object
  SerializeDiscriminator(p_object,msg,entity);
  p_object->Serialize(msg,entity);

  // Save to the file: overwriting the file
  return SaveSOAPMessage(msg,filename);
}

bool
CXSession::DeleteObjectInFilestore(CXObject* p_object)
{
  CXClass* theClass = p_object->GetClass();
  CXTable* table = theClass->GetTable();

  // See if we can create a directory and filename
  CString filename = CreateFilestoreName(table,p_object->GetPrimaryKey());
  if(filename.IsEmpty())
  {
    return false;
  }

  // Test if we can access this file
  if(_taccess(filename, 0) == 0)
  {
    // Try to delete it
    if(DeleteFile(filename))
    {
      return RemoveObjectFromCache(p_object);
    }
  }
  return false;
}

bool
CXSession::DeleteObjectInInternet(CXObject* p_object)
{
  // Create a SOAP message for select
  CString namesp(DEFAULT_NAMESPACE);
  CString action(_T("CXH_Delete"));
  SOAPMessage msg(namesp,action,SoapVersion::SOAP_12);
  XMLElement* entity = msg.SetParameter(_T("Entity"), _T(""));
  msg.SetAttribute(entity,_T("name"),p_object->ClassName());

  // Now serialize our object into this message
  p_object->Serialize(msg,entity);

  // Send to client
  msg.SetURL(m_url);
  if (GetHTTPClient()->Send(&msg))
  {
    CString result = msg.GetParameter(_T("CXResult"));
    if (result == _T("OK"))
    {
      // Object is deleted. Remove from the caches
      RemoveObject(p_object);
      return true;
    }
  }
  CString httpError;
  CString error;
  error.Format(_T("Could not reach CXServer on: %s\n%s\n")
              ,m_url.GetString()
              ,msg.GetFault().GetString());
  GetHTTPClient()->GetError(&httpError);
  error += httpError;
  throw StdException(error);
}

//////////////////////////////////////////////////////////////////////////  

void
CXSession::BuildFilter(SOAPMessage& p_message, XMLElement* p_entity, SQLFilterSet& p_filters)
{
  // All filters under this node
  XMLElement* filters = p_message.AddElement(p_entity,_T("Filters"),XDT_String,_T(""));

  // Prepare for iteration
  for(auto& filter : p_filters.GetFilters())
  {
    int     xmltype = XDT_String | ODBCToXmlDataType(filter->GetValue()->GetDataType());
    CString column  = filter->GetField();
    CString operat  = SQLOperatorToString(filter->GetOperator());
    CString value;
    filter->GetValue()->GetAsString(value);

    XMLElement* filter = p_message.AddElement(filters,_T("Filter"),XDT_String,_T(""));
    p_message.AddElement(filter, _T("Column"),   XDT_String, column);
    p_message.AddElement(filter, _T("Operator"), XDT_String, operat);
    p_message.AddElement(filter, _T("Value"),    xmltype,    value);
  }
}

//////////////////////////////////////////////////////////////////////////

void
CXSession::SerializeDiscriminator(CXObject* p_object,SQLRecord* p_record)
{
  if(hibernate.GetStrategy() != MapStrategy::Strategy_standalone)
  {
    SQLVariant disc(p_object->GetDiscriminator());
    p_record->SetField(_T("discriminator"),&disc,0);
  }
}

void
CXSession::SerializeDiscriminator(CXObject* p_object,SOAPMessage& p_message,XMLElement* p_entity)
{
  if(hibernate.GetStrategy() != MapStrategy::Strategy_standalone)
  {
    p_message.AddElement(p_entity,_T("discriminator"),XDT_String|XDT_Type,p_object->GetDiscriminator());
  }
}

//////////////////////////////////////////////////////////////////////////
//
// FIRING TRIGGERS
//
//////////////////////////////////////////////////////////////////////////

void
CXSession::CallOnLoad(CXObject* p_object)
{
  try
  {
    p_object->OnLoad();
  }
  catch(StdException& ex)
  {
  	hibernate.Log(LOGLEVEL_ERROR,false,_T("Error while calling OnLoad trigger for object"));
    throw ex;
  }
}

bool
CXSession::CallOnInsert(CXObject* p_object)
{
  bool result = false;
  try
  {
    result = p_object->OnInsert();
  }
  catch(StdException& ex)
  {
    hibernate.Log(LOGLEVEL_ERROR,false,_T("Error while calling OnInsert trigger for object"));
    throw ex;
  }
  return result;
}

bool
CXSession::CallOnUpdate(CXObject* p_object)
{
  bool result = false;
  try
  {
    result = p_object->OnUpdate();
  }
  catch(StdException& ex)
  {
    hibernate.Log(LOGLEVEL_ERROR,false,_T("Error while calling OnUpdate trigger for object"));
    throw ex;
  }
  return result;
}

bool
CXSession::CallOnDelete(CXObject* p_object)
{
  bool result = false;
  try
  {
    result = p_object->OnDelete();
  }
  catch(StdException& ex)
  {
    hibernate.Log(LOGLEVEL_ERROR,false,_T("Error while calling OnDelete trigger for object"));
    throw ex;
  }
  return result;
}

//////////////////////////////////////////////////////////////////////////
//
// TRY TO CREATE A NEW (TEST) DATABASE FOR THIS SESSION
//
//////////////////////////////////////////////////////////////////////////

void
CXSession::TryCreateDatabase()
{
  hibernate.Log(CXH_LOG_ACTIONS,false,_T("Try to create database schema"));

  for(auto& cl : m_classes)
  {
    // cl.second->TryCreateTable()
  }
  m_use = CXHSessionUse::SESS_Use;
}

void
CXSession::TryDropDatabase()
{
  hibernate.Log(CXH_LOG_ACTIONS,false,_T("Try to drop database schema"));

  for (auto& cl : m_classes)
  {
    // cl.second->TryDropTable()
  }
}
