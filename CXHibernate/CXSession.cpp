////////////////////////////////////////////////////////////////////////
//
// File: CXSession.cpp
//
// Copyright (c) 1998-2018 ir. W.E. Huisman
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
// Last Revision:   17-06-2018
// Version number:  0.9.0
//
#include "stdafx.h"
#include "CXSession.h"
#include "CXClass.h"
#include "CXPrimaryHash.h"
#include "CXRole.h"
#include "CXTransaction.h"
#include <AutoCritical.h>
#include <SQLQuery.h>
#include <SQLTransaction.h>
#include <EnsureFile.h>
#include <SOAPMessage.h>
#include <HTTPClient.h>
#include <io.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////////////
// Static functions for logging of the SQLDatbase

static void CXHLogPrint(void* p_context, const char* p_text)
{
  hibernate.Log(hibernate.GetLogLevel(),false,p_text);
}

static int CXHLogLevel(void* p_context)
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

  m_database = new SQLDatabase();
  if(m_database->Open(p_database,p_user,p_password))
  {
    GetMetaSessionInfo();
  }
  m_database->RegisterLogContext(hibernate.GetLogLevel(),CXHLogLevel,CXHLogPrint,this);
}

// DTOR: Free all tables and records
CXSession::~CXSession()
{
  // Rollback any hanging transaction
  if(m_transaction)
  {
    m_transaction->Rollback();
    delete m_transaction;
    m_transaction = nullptr;
  }

  // Destroy the caches
  ClearCache();
  ClearClasses();

  // Destroy database (if any)
  if(m_database && m_ownDatabase)
  {
    delete m_database;
    m_database = nullptr;
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
  hibernate.Log(CXH_LOG_ACTIONS,true,"Close session: %s",m_sessionKey);

  // If create/drop cycle testing, try to drop the database
  if(m_use == SESS_Create)
  {
    TryDropDatabase();
  }
  else
  {
    // If still an open database: synchronize our caches
    if(m_database && m_database->IsOpen())
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
  hibernate.Log(CXH_LOG_ACTIONS,true,"Changing session [%s] role to: %s",m_sessionKey,CXRoleToString(p_role));
  m_role = p_role;
}

// See if we have an opened database
bool 
CXSession::GetDatabaseIsOpen()
{
  if(m_database)
  {
    return m_database->IsOpen();
  }
  return false;
}

// Alternate database 
void 
CXSession::SetDatabase(SQLDatabase* p_database)
{
  AutoCritSec lock(&m_lock);

  if(m_database && m_ownDatabase)
  {
    delete m_database;
  }
  m_database = p_database;
  m_ownDatabase = false;
  ChangeRole(CXH_Database_role);
  m_database->RegisterLogContext(hibernate.GetLogLevel(),CXHLogLevel,CXHLogPrint,this);

  GetMetaSessionInfo();
}

// Getting our database
SQLDatabase*
CXSession::GetDatabase()
{
  AutoCritSec lock(&m_lock);

  if(m_database)
  {
    return m_database;
  }
  if(!m_dbsCatalog.IsEmpty())
  {
    m_ownDatabase = true;
    m_database = new SQLDatabase();
    m_database->RegisterLogContext(hibernate.GetLogLevel(),CXHLogLevel,CXHLogPrint,this);
    m_database->Open(m_dbsCatalog,m_dbsUser,m_dbsPassword);
    if(m_database->IsOpen())
    {
      m_role = CXH_Database_role;
      GetMetaSessionInfo();

      // Do we need to create a schema?
      if (m_use == SESS_Create)
      {
        TryCreateDatabase();
      }
      return m_database;
    }
  }
  throw StdException("Connect a database to your CX-Hibernate session!");
}

// Specify a database connection
void
CXSession::SetDatabaseCatalog(CString p_datasource)
{
  m_dbsCatalog = p_datasource;
}

void
CXSession::SetDatabaseUsername(CString p_user)
{
  m_dbsUser = p_user;
}

void
CXSession::SetDatabasePassword(CString p_password)
{
  m_dbsPassword = p_password;
}

void
CXSession::SetDatabaseConnection(CString p_datasource,CString p_user,CString p_password)
{
  m_dbsCatalog  = p_datasource;
  m_dbsUser     = p_user;
  m_dbsPassword = p_password;
  // Clearly we want to do database exchanges
  m_role = CXH_Database_role;
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

  hibernate.Log(CXH_LOG_ACTIONS,true,"Adding class [%s] to session [%s] ",name,m_sessionKey);
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
  CString role = p_config.GetElement("session_role");
  m_role = CXStringToRole(role);

  CString use = p_config.GetElement("database_use");
  m_use = CXStringToSessionUse(use);

  // Load all classes
  XMLElement* theclass = p_config.FindElement("class");
  while(theclass)
  {
    CXClass* superclass = nullptr;
    CString    name = p_config.GetElement(theclass,"name");
    CString   super = p_config.GetElement(theclass,"super");
    CString discrim = p_config.GetElement(theclass,"discriminator");

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
      throw StdException("Cannot load class: " + name);
    }
    // Find next class
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
  // Save session parameters
  p_config.AddElement(nullptr,"session_role",XDT_String,CXRoleToString(m_role));
  p_config.AddElement(nullptr,"database_use",XDT_String,CXSessionUseToString(m_use));

  // Save all class info
  for(auto& cl : m_classes)
  {
    cl.second->SaveMetaInfo(p_config,nullptr);
  }
}

//////////////////////////////////////////////////////////////////////////
//
// MUTATIONS
//
//////////////////////////////////////////////////////////////////////////

// Get a master mutation ID, to put actions into one (1) commit
int
CXSession::StartTransaction()
{
  // Lock the caches
  AutoCritSec lock(&m_lock);

  // On re-entry, and still in transaction
  if(m_transaction)
  {
    // Create a sub transaction
    ++m_subtrans;
    return m_mutation;
  }

  // Create transaction and give new ID
  m_transaction = new SQLTransaction(GetDatabase(),"mutation");

  // Reset the sub-transaction
  m_subtrans = 0;
  // This is our mutation ID
  m_mutation = hibernate.GetNewMutation();

  hibernate.Log(CXH_LOG_ACTIONS,true,"Started transaction [%d] for session [%s] ",m_mutation,m_sessionKey);
  return m_mutation;
}

void
CXSession::CommitTransaction()
{
  // Lock the caches
  AutoCritSec lock(&m_lock);

  // Commit of sub-transaction
  if(m_subtrans)
  {
    --m_subtrans;
    return;
  }

  // Same mutation and in-transaction: so commit
  if(m_transaction)
  {
    m_transaction->Commit();
    delete m_transaction;
    m_transaction = nullptr;
  }
  else
  {
    throw StdException("Not in transaction at Commit()");
  }
  hibernate.Log(CXH_LOG_ACTIONS,true,"Commit of transaction [%d] for session [%s] ",m_mutation,m_sessionKey);
}

void
CXSession::RollbackTransaction()
{
  // Lock the caches
  AutoCritSec lock(&m_lock);

  // See if we have a transaction
  if(m_transaction == nullptr)
  {
    throw StdException("Not in transaction at Rollback()");
  }

  // Perform the rollback
  m_transaction->Rollback();
  delete m_transaction;
  m_transaction = nullptr;

  // Sub-transactions also rolled back
  m_subtrans = 0;

  hibernate.Log(CXH_LOG_ACTIONS,true,"Rollback transaction [%d] for session [%s] ",m_mutation,m_sessionKey);
}

bool
CXSession::HasTransaction()
{
  return (m_transaction != nullptr);
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
  hibernate.Log(CXH_LOG_ERRORS,true,"Session [%s] Cannot create object for class: %s",m_sessionKey,p_className);
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
        hibernate.Log(CXH_LOG_ACTIONS,true,"Loading object [%s:%s]",p_className,object->Hashcode());
        if(hibernate.GetLogLevel() >= CXH_LOG_DEBUG)
        {
          object->LogObject();
        }
      }
      return object;
    }
    delete object;
  }
  hibernate.Log(CXH_LOG_ERRORS,true,"Session [%s] Cannot load object for class: %s",m_sessionKey,p_className);
  return nullptr;
}

CXResultSet
CXSession::Load(CString p_tableName,SQLFilter* p_filter)
{
  SQLFilterSet set;
  set.AddFilter(p_filter);

  return Load(p_tableName,set);
}

CXResultSet
CXSession::Load(CString p_className,SQLFilterSet& p_filters)
{
  CXResultSet set;

  p_className.MakeLower();
  ClearCache(p_className);

  // Create a caching object in the cache
  ObjectCache* objcache = new ObjectCache();
  m_cache.insert(std::make_pair(p_className,objcache));

  if(m_role == CXH_Database_role)
  {
    set = SelectObjectsFromDatabase(p_className,p_filters);
  }
  else if(m_role == CXH_Internet_role)
  {
    set = SelectObjectsFromInternet(p_className,p_filters);
  }
  else // CXH_Filestore_role
  {
    set = SelectObjectsFromFilestore(p_className,p_filters);
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
      hibernate.Log(CXH_LOG_ACTIONS,true,"Loading object [%s:%s]",p_className,object->Hashcode());
      if(hibernate.GetLogLevel() >= CXH_LOG_DEBUG)
      {
        object->LogObject();
      }
    }
  }
  return set;
}

bool
CXSession::Save(CXObject* p_object)
{
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
CXSession::Update(CXObject* p_object)
{
  bool result = false;

  // Call OnUpdate trigger for the object
  bool canUpdate = CallOnUpdate(p_object);

  // Only update if trigger allows so
  if(canUpdate)
  {
    if(m_role == CXH_Database_role)
    {
      result = UpdateObjectInDatabase(p_object);
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
    CString action = canUpdate ? "Update object" : "CANNOT UPDATE";
    hibernate.Log(CXH_LOG_ACTIONS, true, "%s [%s:%s]",action,p_object->ClassName(),p_object->Hashcode());
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
      CString action = canInsert ? "Inserted object" : "CANNOT INSERT object";
      hibernate.Log(CXH_LOG_ACTIONS,true,"%s [%s:%s]",action,p_object->ClassName(),p_object->Hashcode());
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

  // Call OnDelete trigger
  bool canDelete = CallOnDelete(p_object);

  // Log the object as deleted, before it's gone
  if(hibernate.GetLogLevel())
  {
    CString action = canDelete ? "Object to delete" : "CANNOT DELETE object";
    hibernate.Log(CXH_LOG_ACTIONS,true,"%s [%s:%s]",action,p_object->ClassName(),p_object->Hashcode());
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
    hibernate.Log(CXH_LOG_ACTIONS,true,"Delete succeeded: %s",result ? "yes" : "no");
  }
  return result;
}

// Remove object from the result cache without any database/internet actions
bool
CXSession::RemoveObject(CXObject* p_object)
{
  return RemoveObjectFromCache(p_object);
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

  // Getting a new mutation
  CXTransaction trans(this);

  // Walk our object cache
  for(auto& objcache : m_cache)
  {
    ObjectCache& objects = *objcache.second;
    for(auto& obj : objects)
    {
      CXObject*  object = obj.second;
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

  // Commit in the database
  trans.Commit();
  return true;
}

// Create a filestore name for a table
CString
CXSession::CreateFilestoreName(CXTable* p_table)
{
  CString tablename = p_table->GetFullQualifiedTableName();
  tablename.Replace(".", "_");

  CString fileName = m_baseDirectory;
  if (fileName.Right(1) != '\\')
  {
    fileName += "\\";
  }

  EnsureFile ensure(fileName);
  if (ensure.CheckCreateDirectory())
  {
    return "";
  }

  // Add our table name
  fileName += tablename;
  fileName += ".xml";

  return fileName;
}

// Create a filestore name for an object
CString
CXSession::CreateFilestoreName(CXTable* p_table, VariantSet& p_primary)
{
  CString tablename = p_table->GetFullQualifiedTableName();
  tablename.Replace(".", "_");

  CString fileName = m_baseDirectory;
  if (fileName.Right(1) != '\\')
  {
    fileName += "\\";
  }
  fileName += tablename;
  fileName += "\\";

  EnsureFile ensure(fileName);
  if (ensure.CheckCreateDirectory())
  {
    return "";
  }

  CString hash = CXPrimaryHash(p_primary);
  hash.Replace("\002", "_");

  fileName += "Object_";
  fileName += hash;
  fileName += ".xml";

  return fileName;
}

// Saving a SOAP XML message on the file system
bool
CXSession::SaveSOAPMessage(SOAPMessage& p_message, CString p_fileName)
{
  bool result = false;
  FILE* file = nullptr;
  if (fopen_s(&file, p_fileName, "w") == 0 && file)
  {
    CString contents = p_message.GetSoapMessage();
    if (fwrite(contents.GetString(), contents.GetLength(), 1, file) == 1)
    {
      result = true;
    }
    // Close and flush the file
    fclose(file);
  }
  return result;
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
    throw StdException("Association not found to class: " + p_toClass);
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
    case ASSOC_MANY_TO_MANY:throw StdException("Association type many-to-many not yet implemented!");
    default:                break;
  }
  throw StdException("Association type not found for association: " + assoc->m_constraintName);
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
  hibernate.Log(CXH_LOG_ACTIONS,true,"Clear object cache of session: %s",m_sessionKey);

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
  hibernate.Log(CXH_LOG_ACTIONS,true,"Clear class definition cache of session: %s",m_sessionKey);

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
  if(m_database)
  {
    try
    {
      CString sql = m_database->GetSQLInfoDB()->GetSESSIONMyself();
      SQLQuery query(m_database);
      SQLTransaction trans(m_database,"session");

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
      UNREFERENCED_PARAMETER(er);
    }
  }
  if(hibernate.GetLogLevel())
  {
    hibernate.Log(CXH_LOG_DEBUG,true,"Hibernate session : %s",m_sessionKey);
    hibernate.Log(CXH_LOG_DEBUG,true,"Database session  : %s",m_metaInfo.m_session);
    hibernate.Log(CXH_LOG_DEBUG,true,"Database user     : %s",m_metaInfo.m_user);
    hibernate.Log(CXH_LOG_DEBUG,true,"Database terminal : %s",m_metaInfo.m_terminal);
    hibernate.Log(CXH_LOG_DEBUG,true,"Database logon    : %s",m_metaInfo.m_logonMoment);
    hibernate.Log(CXH_LOG_DEBUG,true,"Database address  : %s",m_metaInfo.m_remoteIP);
    hibernate.Log(CXH_LOG_DEBUG,true,"Processname in dbs: %s",m_metaInfo.m_processName);
    hibernate.Log(CXH_LOG_DEBUG,true,"Database ID       : %s",m_metaInfo.m_processID);
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
  if (it != m_cache.end())
  {
    ObjectCache::iterator tit = it->second->find(hash);
    if(tit == it->second->end())
    {
      it->second->insert(std::make_pair(hash,p_object));
      return true;
    }
    // Object already in the cache. Do not cache again!
    return false;
  }
  throw StdException("Cache not found for class: " + className);
}

// And remove again from the cache
bool
CXSession::RemoveObjectFromCache(CXObject* p_object)
{
  // Lock the caches
  AutoCritSec lock(&m_lock);

  CString hash = p_object->Hashcode();
  CString tableName = p_object->GetClass()->GetTable()->GetTableName();
  tableName.MakeLower();

  CXCache::iterator it = m_cache.find(tableName);
  if (it != m_cache.end())
  {
    ObjectCache* tcache = it->second;
    ObjectCache::iterator tit = tcache->find(hash);
    if(tit != tcache->end())
    {
      // Destroy the CXObject derived object
      delete tit->second;
      tcache->erase(tit);
      return true;
    }
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

  SQLRecord* record = theClass->SelectObjectInDatabase(GetDatabase(),nullptr,p_primary);
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
  if(_access(filename, 0) == 0)
  {
    // Try to load the object
    SOAPMessage p_message;
    if(p_message.LoadFile(filename))
    {
      // Find our object (just one entity in the message)
      XMLElement* entity = p_message.FindElement("Entity");
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
  CString action("CXH_Select");
  SOAPMessage msg(namesp,action,SoapVersion::SOAP_12);
  XMLElement* entity = msg.SetParameter("Entity", "");
  msg.SetAttribute(entity,"name",p_className); 
  
  // Load filters in message
  theClass->BuildPrimaryKeyFilter(msg,entity,p_primary);

  // Send to client
  msg.SetURL(m_url);
  if(GetHTTPClient()->Send(&msg))
  {
    // Find our object (just one entity in the message)
    XMLElement* entity = msg.FindElement("Entity");
    if(entity)
    {
      return LoadObjectFromXML(msg,entity,theClass);
    }
  }
  CString httpError;
  CString error;
  error.Format("Could not reach CXServer on: %s\n%s\n"
                ,m_url
                ,msg.GetFault());
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
CXSession::SelectObjectsFromDatabase(CString p_className,SQLFilterSet& p_filters)
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
  dset->SetDatabase(GetDatabase());

  // Propagate our filters
  dset->SetFilters(&p_filters);

  // Create correct query
  theClass->BuildDefaultSelectQuery(dset,GetDatabase()->GetSQLInfoDB());

  // NOW GO OPEN our dataset
  bool selected = false;
  if(dset->IsOpen())
  {
    selected = dset->Append();
  }
  else
  {
    selected = dset->Open();
  }
  if(selected)
  {
    int recnum = dset->First();
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
        if(AddObjectInCache(object) == false)
        {
          // LOG!
          delete object;
        }
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
CXSession::SelectObjectsFromFilestore(CString p_className,SQLFilterSet& p_filters)
{
  throw StdException("Cannot select multiple objects from the filestore");
}

CXResultSet
CXSession::SelectObjectsFromInternet(CString p_className,SQLFilterSet& p_filters)
{
  CXResultSet set;
  CXClass* theClass = FindClass(p_className);

  // Create a SOAP message for select
  CString namesp(DEFAULT_NAMESPACE);
  CString action("CXH_Select");
  SOAPMessage msg(namesp,action,SoapVersion::SOAP_12);
  XMLElement* entity = msg.SetParameter("Entity", "");
  msg.SetAttribute(entity,"name",p_className); 
  
  // Load filters in message
  BuildFilter(msg,entity,p_filters);

  // Send to client
  msg.SetURL(m_url);
  if(GetHTTPClient()->Send(&msg))
  {
    // Find our object (just one entity in the message)
    XMLElement* entity = msg.FindElement("Entity");
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
      if(entity->GetName().CompareNoCase("Entity"))
      {
        break;
      }
    }
    return set;
  }
  CString httpError;
  CString error;
  error.Format("Could not reach CXServer on: %s\n%s\n"
                ,m_url
                ,msg.GetFault());
  GetHTTPClient()->GetError(&httpError);
  error += httpError;
  throw StdException(error);
}

bool
CXSession::UpdateObjectInDatabase(CXObject* p_object)
{
  CXClass* theClass = p_object->GetClass();
  if(theClass == nullptr)
  {
    throw StdException("Missing class on UPDATE of an object. Did you tinkle with the PrimaryKey?");
  }

  // New mutation ID for this update action
  CXTransaction trans(this);

  bool result = theClass->UpdateObjectInDatabase(GetDatabase(),nullptr,p_object,m_mutation);
  if(result)
  {
    // Commit our transaction in the database
    trans.Commit();
  }
  return result;
}

bool
CXSession::UpdateObjectInInternet(CXObject* p_object)
{
  // Create a SOAP message for select
  CString namesp(DEFAULT_NAMESPACE);
  CString action("CXH_Update");
  SOAPMessage msg(namesp, action, SoapVersion::SOAP_12);
  XMLElement* entity = msg.SetParameter("Entity", "");
  msg.SetAttribute(entity,"name",p_object->ClassName());

  // Now serialize our object into this message
  SerializeDiscriminator(p_object,msg,entity);
  p_object->Serialize(msg,entity);

  // Send to client
  msg.SetURL(m_url);
  if(GetHTTPClient()->Send(&msg))
  {
    CString result = msg.GetParameter("CXResult");
    if(result == "OK")
    {
      return true;
    }
  }
  CString httpError;
  CString error;
  error.Format("Could not reach CXServer on: %s\n%s\n"
               ,m_url
               ,msg.GetFault());
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
    throw StdException("Object without a connected class. Did you call CXSession::CreateObject(<classname>)??");
  }

  // New mutation ID for this update action
  CXTransaction trans(this);

  bool saved = theClass->InsertObjectInDatabase(GetDatabase(),nullptr,p_object,m_mutation);
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
  CString action("CXH_Insert");
  SOAPMessage msg(namesp, action, SoapVersion::SOAP_12);
  XMLElement* entity = msg.SetParameter("Entity", "");
  msg.SetAttribute(entity,"name",p_object->ClassName());

  // Now serialize our object into this message
  SerializeDiscriminator(p_object,msg,entity);
  p_object->Serialize(msg,entity);

  // Send to client
  msg.SetURL(m_url);
  if(GetHTTPClient()->Send(&msg))
  {
    CString result = msg.GetParameter("CXResult");
    if(result == "OK")
    {
      // De-Serialize the result (gives us the new primary key)
      // And possibly changed attributes
      XMLElement* entity = msg.FindElement("Entity");
      p_object->DeSerialize(msg,entity);
      return true;
    }
  }
  CString httpError;
  CString error;
  error.Format("Could not reach CXServer on: %s\n%s\n"
               ,m_url
               ,msg.GetFault());
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
    throw StdException("No class definition while deleting an object");
  }
  // If object was **NOT** in the database, removal did work out OK!
  if(p_object->IsPersistent() == false)
  {
    return true;
  }

  // New mutation ID for this update action
  CXTransaction trans(this);

  // Go delete the record
  bool deleted = theClass->DeleteObjectInDatabase(GetDatabase(),nullptr,p_object,m_mutation);
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
  SOAPMessage msg(DEFAULT_NAMESPACE, "Object");

  // Adding an entity with name attributes
  XMLElement* entity = msg.SetParameter("Entity", "");
  if(!table->GetSchemaName().IsEmpty())
  {
    msg.SetAttribute(entity,"schema",table->GetSchemaName());
  }
  msg.SetAttribute(entity,"name",table->GetTableName());

  // Serialize our object
  SerializeDiscriminator(p_object,msg,entity);
  p_object->Serialize(msg,entity);

  // Test if we can access this file
  if(_access(filename, 0) == 0)
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
  CString action("Object");
  SOAPMessage msg(namesp,action,SoapVersion::SOAP_12);
  XMLElement* entity = msg.SetParameter("Entity","");
  msg.SetAttribute(entity,"name",table->GetTableName());

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
  if(_access(filename, 0) == 0)
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
  CString action("CXH_Delete");
  SOAPMessage msg(namesp,action,SoapVersion::SOAP_12);
  XMLElement* entity = msg.SetParameter("Entity", "");
  msg.SetAttribute(entity,"name",p_object->ClassName());

  // Now serialize our object into this message
  p_object->Serialize(msg,entity);

  // Send to client
  msg.SetURL(m_url);
  if (GetHTTPClient()->Send(&msg))
  {
    CString result = msg.GetParameter("CXResult");
    if (result == "OK")
    {
      // Object is deleted. Remove from the caches
      RemoveObject(p_object);
      return true;
    }
  }
  CString httpError;
  CString error;
  error.Format("Could not reach CXServer on: %s\n%s\n"
              ,m_url
              ,msg.GetFault());
  GetHTTPClient()->GetError(&httpError);
  error += httpError;
  throw StdException(error);
}

//////////////////////////////////////////////////////////////////////////  

void
CXSession::BuildFilter(SOAPMessage& p_message, XMLElement* p_entity, SQLFilterSet& p_filters)
{
  // All filters under this node
  XMLElement* filters = p_message.AddElement(p_entity,"Filters",XDT_String,"");

  // Prepare for iteration
  for(auto& filter : p_filters.GetFilters())
  {
    int     xmltype = XDT_String | ODBCToXmlDataType(filter->GetValue()->GetDataType());
    CString column  = filter->GetField();
    CString operat  = SQLOperatorToString(filter->GetOperator());
    CString value;
    filter->GetValue()->GetAsString(value);

    XMLElement* filter = p_message.AddElement(filters,"Filter",XDT_String,"");
    p_message.AddElement(filter, "Column",   XDT_String, column);
    p_message.AddElement(filter, "Operator", XDT_String, operat);
    p_message.AddElement(filter, "Value",    xmltype,    value);
  }
}

//////////////////////////////////////////////////////////////////////////

void
CXSession::SerializeDiscriminator(CXObject* p_object,SQLRecord* p_record)
{
  if(hibernate.GetStrategy() != MapStrategy::Strategy_standalone)
  {
    SQLVariant disc(p_object->GetDiscriminator());
    p_record->SetField("discriminator",&disc,m_mutation);
  }
}

void
CXSession::SerializeDiscriminator(CXObject* p_object,SOAPMessage& p_message,XMLElement* p_entity)
{
  if(hibernate.GetStrategy() != MapStrategy::Strategy_standalone)
  {
    p_message.AddElement(p_entity,"discriminator",XDT_String|XDT_Type,p_object->GetDiscriminator());
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
  	hibernate.Log(LOGLEVEL_ERROR,false,"Error while calling OnLoad trigger for object");
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
    hibernate.Log(LOGLEVEL_ERROR,false,"Error while calling OnInsert trigger for object");
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
    hibernate.Log(LOGLEVEL_ERROR,false,"Error while calling OnUpdate trigger for object");
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
    hibernate.Log(LOGLEVEL_ERROR,false,"Error while calling OnDelete trigger for object");
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
  hibernate.Log(CXH_LOG_ACTIONS,false,"Try to create database schema");

  for(auto& cl : m_classes)
  {
    // cl.second->TryCreateTable()
  }
}

void
CXSession::TryDropDatabase()
{
  hibernate.Log(CXH_LOG_ACTIONS,false,"Try to drop database schema");

  for (auto& cl : m_classes)
  {
    // cl.second->TryDropTable()
  }
}
