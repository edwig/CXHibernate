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
// Last Revision:   22-04-2018
// Version number:  0.0.1
//
#include "stdafx.h"
#include "CXSession.h"
#include "CXClass.h"
#include "CXPrimaryHash.h"
#include "CXRole.h"
#include <SQLQuery.h>
#include <SQLTransaction.h>
#include <EnsureFile.h>
#include <SOAPMessage.h>
#include <io.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// CTOR Default session (slave side of the session)
CXSession::CXSession(CString p_sessionKey)
          :m_sessionKey(p_sessionKey)
          ,m_role(CXH_Internet_role)
{
}

// CTOR Filestore session
CXSession::CXSession(CString p_sessionKey,CString p_directory)
          :m_sessionKey(p_sessionKey)
          ,m_baseDirectory(p_directory)
          ,m_role(CXH_Filestore_role)
{
}

// CTOR Master session
CXSession::CXSession(CString p_sessionKey,CString p_database,CString p_user,CString p_password)
          :m_sessionKey(p_sessionKey)
          ,m_role(CXH_Database_role)
{
  m_database = new SQLDatabase();
  if(m_database->Open(p_database,p_user,p_password))
  {
    GetMetaSessionInfo();
  }
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
}

// And closing it again
void
CXSession::CloseSession()
{
  Synchronize();

  if (m_use == SESS_Create)
  {
    TryDropDatabase();
  }
  hibernate.RemoveSession(this);
}

void
CXSession::ChangeRole(CXHRole p_role)
{
  m_role = p_role;
}

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
  if(m_database && m_ownDatabase)
  {
    delete m_database;
  }
  m_database = p_database;
  m_ownDatabase = false;
  m_role = CXH_Database_role;

  GetMetaSessionInfo();
}

// Getting our database
SQLDatabase*
CXSession::GetDatabase()
{
  if(m_database)
  {
    return m_database;
  }
  if(!m_dbsCatalog.IsEmpty())
  {
    m_database = new SQLDatabase();
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
  return nullptr;
}

// Specifiy a database connection
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

// Add a table to the session
bool
CXSession::AddClass(CXClass* p_class)
{
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

  return true;
}

// Finding a class
CXClass*
CXSession::FindClass(CString p_name)
{
  p_name.MakeLower();
  ClassMap::iterator it = m_classes.find(p_name);
  if(it != m_classes.end())
  {
    return it->second;
  }
  return nullptr;
}

// Loading the general configuration XML
void
CXSession::LoadConfiguration(XMLMessage& p_config)
{
  // Load session parameters
  CString role = p_config.GetElement("session_role");
  m_role = CXStringToRole(role);

  CString use = p_config.GetElement("database_use");
  m_use = CXStringToSessionUse(use);

  // Load all classes
  XMLElement* theclass = p_config.FindElement("class");
  while(theclass)
  {
    CString  name = p_config.GetElement(theclass,"name");
    CXClass* newclass = new CXClass(name);

    if(newclass->LoadMetaInfo(this,p_config,theclass))
    {
      AddClass(newclass);
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
CXSession::GetMutationID(bool p_transaction /*= false*/)
{
  // On re-entry, and still in transaction
  if(p_transaction && m_transaction)
  {
    // Create a sub transaction
    ++m_subtrans;
    return m_mutation;
  }

  // Should now be without a transaction
  if(m_transaction)
  {
    m_transaction->Rollback();
    delete m_transaction;
    m_transaction = nullptr;
  }

  // Create transaction and give new ID
  if(p_transaction)
  {
    m_transaction = new SQLTransaction(GetDatabase(),"mutation");
  }
  // Reset the sub-transaction
  m_subtrans = 0;
  // This is our mutation ID
  return ++m_mutation;
}

void
CXSession::CommitMutation(int p_mutationID)
{
  // Verify that it's still the same mutation
  if(p_mutationID != m_mutation)
  {
    return;
  }

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
}

void
CXSession::RollbackMutation(int p_mutationID)
{
  // See if we have a transaction
  if(m_transaction == nullptr)
  {
    return;
  }

  m_transaction->Rollback();
  delete m_transaction;
  m_transaction = nullptr;
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
  SQLVariant prim = (long)p_primary;
  VariantSet set;
  set.push_back(&prim);

  return Load(p_className,set);
}

CXObject*
CXSession::Load(CString p_tableName,SQLVariant* p_primary)
{
  VariantSet set;
  set.push_back(p_primary);

  return Load(p_tableName,set);
}

CXObject*
CXSession::Load(CString p_table,VariantSet& p_primary)
{
  // Search in cache
  CXObject* object = FindObjectInCache(p_table,p_primary);
  if(object)
  {
    return object;
  }

  // If not found, search in database / SOAP connection
  if(m_role == CXH_Database_role)
  {
    object = FindObjectInDatabase(p_table,p_primary);
  }
  else if(m_role == CXH_Internet_role)
  {
    // Query for object on SOAP connection
    object = FindObjectOnInternet(p_table,p_primary);
  }
  else
  {
    // Query for object in the filestore
    object = FindObjectInFilestore(p_table,p_primary);
  }

  // Place in cache
  if(object && object->IsPersistent())
  {
    if(AddObjectInCache(object,p_primary))
    {
      return object;
    }
    delete object;
  }
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
    SelectObjectsFromDatabase(p_className,p_filters);
  }
  else if(m_role == CXH_Internet_role)
  {
    SelectObjectsFromInternet(p_className,p_filters);
  }
  else // CXH_Filestore_role
  {
    SelectObjectsFromFilestore(p_className,p_filters);
  }

  // Getting the result set
  CXCache::iterator it = m_cache.find(p_className);
  if(it != m_cache.end())
  {
    for(auto& obj : *it->second)
    {
      set.push_back(obj.second);
    }
  }

  return set;
}

bool
CXSession::Save(CXObject* p_object,int p_mutationID /*= 0*/)
{
  if(p_object->IsTransient())
  {
    return Insert(p_object,p_mutationID);
  }
  else
  {
    return Update(p_object,p_mutationID);
  }
}

// Update an object
bool
CXSession::Update(CXObject* p_object,int p_mutationID /*= 0*/)
{
  if(m_role == CXH_Database_role)
  {
    return UpdateObjectInDatabase(p_object,p_mutationID);
  }
  else if(m_role == CXH_Internet_role)
  {
    // Save as a SOAP message
    // UpdateObjectInInternet(table,p_object);
    return false;
  }
  else // CXH_Filestore_role
  {
    UpdateObjectInFilestore(p_object);
  }
  return true;
}

bool
CXSession::Insert(CXObject* p_object,int p_mutationID /*= 0*/)
{
  if(m_role == CXH_Database_role)
  {
    InsertObjectInDatabase(p_object,p_mutationID);
  }
  else if(m_role == CXH_Internet_role)
  {
    // Insert as a SOAP Message
    // InsertObjectInInternet(table,p_object);
    return false;
  }
  else // CXH_Filestore_role
  {
    InsertObjectInFilestore(p_object,p_mutationID);
  }
  // Add object to the cache
  if(p_object->IsPersistent())
  {
    return AddObjectInCache(p_object,p_object->GetPrimaryKey());
  }
  return false;
}

bool
CXSession::Delete(CXObject* p_object,int p_mutationID /*= 0*/)
{
  if(m_role == CXH_Database_role)
  {
    DeleteObjectInDatabase(p_object,p_mutationID);
  }
  else if(m_role == CXH_Internet_role)
  {
    // DeleteObjectInInternet(table,p_object);
    return false;
  }
  else // CXH_Filestore_role
  {
    DeleteObjectInFilestore(p_object,p_mutationID);
  }

  return RemoveObjectFromCache(p_object,p_object->GetPrimaryKey());
}

// Remove object from the result cache without any database/internet actions
bool
CXSession::RemoveObject(CXObject* p_object)
{
  return RemoveObjectFromCache(p_object,p_object->GetPrimaryKey());
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
  int mutationID = GetMutationID(true);

  // Walk our object cache
  for(auto& objcache : m_cache)
  {
    ObjectCache& objects = *objcache.second;
    for(auto& obj : objects)
    {
      CXObject*  object = obj.second;
      SQLRecord* record = object->GetDatabaseRecord();
      if(record && (record->GetStatus() & SQL_Record_Updated))
      {
        object->Serialize(*record,mutationID);
        if(Update(object,mutationID) == false)
        {
          RollbackMutation(mutationID);
          return false;
        }
      }
    }
  }

  // Commit in the database
  CommitMutation(mutationID);
  return true;
}

// Create a filestore name for a table
CString
CXSession::CreateFilestoreName(CXTable* p_table)
{
  CString tablename = p_table->FullQualifiedTableName();
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
  CString tablename = p_table->FullQualifiedTableName();
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
  if(m_cache.empty())
  {
    return;
  }

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
    catch(StdException* er)
    {
      // Error in silence ?
      er->Delete();
    }
  }
}

// Add an object to the cache
bool
CXSession::AddObjectInCache(CXObject* p_object, VariantSet& p_primary)
{
  CString hash = CXPrimaryHash(p_primary);
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
  }
  return false;
}

// And remove again from the cache
bool
CXSession::RemoveObjectFromCache(CXObject* p_object, VariantSet& p_primary)
{
  CString hash = CXPrimaryHash(p_primary);
  CString tableName = p_object->GetClass()->GetTable()->TableName();
  tableName.MakeLower();

  CXCache::iterator it = m_cache.find(tableName);
  if (it != m_cache.end())
  {
    ObjectCache* tcache = it->second;
    ObjectCache::iterator tit = tcache->find(hash);
    if (tit != tcache->end())
    {
      // Destroy the CXObject derived object
      delete tit->second;
      tcache->erase(tit);
      return true;
    }
  }
  return false;
}

// Create a filters set for a DataSet
bool
CXSession::CreateFilterSet(CXTable* p_table,VariantSet& p_primary,SQLFilterSet& p_filters)
{
  // Check if we have a primary key, or an empty key
  if(p_primary.empty())
  {
    return true;
  }

  // Check if number of columns of the primary key matches the number of values
  WordList list = p_table->GetPrimaryKeyAsList();
  if(list.size() != p_primary.size())
  {
    return false;
  }

  // Walk the list of columns / values
  int ind = 0;
  for(auto& column : list)
  {
    SQLFilter* filter = new SQLFilter(column,SQLOperator::OP_Equal,p_primary[ind++]);
    p_filters.AddFilter(filter);
  }

  return true;
}

// Try to find an object in the cache
// It's a double map lookup (table, object)
CXObject*
CXSession::FindObjectInCache(CString p_tableName,VariantSet& p_primary)
{
  CString hash = CXPrimaryHash(p_primary);
  p_tableName.MakeLower();

  CXCache::iterator it = m_cache.find(p_tableName);
  if (it != m_cache.end())
  {
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
  // Find the CXTable object
  p_className.MakeLower();
  ClassMap::iterator it = m_classes.find(p_className);
  if(it == m_classes.end())
  {
    return nullptr;
  }
  CXClass* theClass = it->second;
  CXTable* table = theClass->GetTable();

  // See if we have a data set
  SQLDataSet* dset = table->GetDataSet();
  if(dset == nullptr)
  {
    return nullptr;
  }

  // Connect our database
  dset->SetDatabase(GetDatabase());

  // Create correct query
  theClass->BuildDefaultSelectQuery(GetDatabase()->GetSQLInfoDB());

  SQLFilterSet fset;
  if(CreateFilterSet(table,p_primary,fset))
  {
    dset->SetFilters(&fset);

    // Open our dataset (and search)
    if(dset->IsOpen() == false)
    {
      dset->Open();
    }
    else
    {
      dset->Append();
    }
    dset->SetFilters(nullptr);
  }
  else return nullptr;

  // See if we have a record
  SQLRecord* record = dset->FindObjectRecord(p_primary);
  if(record == nullptr)
  {
    return nullptr;
  }

  // Create our object by the creation factory
  CreateCXO create = theClass->GetCreateCXO();
  CXObject* object = (*create)();
  object->SetClass(theClass);

  // De-serialize the SQL Record to an CXObject derived object
  object->DeSerialize(*record);

  return object;
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
    return false;
  }

  // Test if we can access this file
  if(_access(filename, 0) == 0)
  {
    // Try to delete it
    SOAPMessage p_message;
    if(p_message.LoadFile(filename))
    {
      // Find our object (just one entity in the message)
      XMLElement* entity = p_message.FindElement("Entity");
      if(entity)
      {
        // Create our object by the creation factory
        CreateCXO create = theClass->GetCreateCXO();
        CXObject* object = (*create)();
        object->SetClass(theClass);

        // Fill in our object from the message 
        object->DeSerialize(p_message,entity);

        return object;
      }
    }
  }
  return false;
}


// Try to find an object via the SOAP interface
CXObject*
CXSession::FindObjectOnInternet(CString p_table,VariantSet& p_primary)
{
  return nullptr;
}

void
CXSession::SelectObjectsFromDatabase(CString p_className,SQLFilterSet& p_filters)
{
  // Find the CXTable object
  p_className.MakeLower();
  ClassMap::iterator it = m_classes.find(p_className);
  if (it == m_classes.end())
  {
    return;
  }
  CXClass* theClass = it->second;
  CXTable* table = theClass->GetTable();

  // See if we have a data set
  SQLDataSet* dset = table->GetDataSet();
  if (dset == nullptr)
  {
    return;
  }

  // Connect our database
  dset->SetDatabase(GetDatabase());

  // Close dataset if it was opened
  if(dset->IsOpen())
  {
    dset->Close();
  }

  // Propagate our filters
  dset->SetFilters(&p_filters);
  // Set our query
  CString query = CString("SELECT * FROM ") + table->DMLTableName(GetDatabase()->GetSQLInfoDB());
  dset->SetQuery(query);

  // NOW GO OPEN our dataset
  if(dset->Open())
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
      if (object->IsPersistent())
      {
        AddObjectInCache(object,object->GetPrimaryKey());
      }

      // Getting the next record
      ++recnum;
    }
  }
  dset->SetFilters(nullptr);
}

void
CXSession::SelectObjectsFromFilestore(CString p_className,SQLFilterSet& p_filters)
{
  throw new StdException("Cannot select multiple objects from the filestore");
}

void
CXSession::SelectObjectsFromInternet(CString p_className,SQLFilterSet& p_filters)
{

}

bool
CXSession::UpdateObjectInDatabase(CXObject* p_object,int p_mutationID /*= 0*/)
{
  CXClass* theClass = p_object->GetClass();
  CXTable*    table = theClass->GetTable();
  SQLRecord* record = p_object->GetDatabaseRecord();
  SQLDataSet*  dset = table->GetDataSet();

  // Connect our database
  dset->SetDatabase(GetDatabase());

  // New mutation ID for this update action
  if(p_mutationID == 0)
  {
    p_mutationID = ++m_mutation;
  }

  // Serialize object to database record
  p_object->Serialize(*record,p_mutationID);
  return dset->Synchronize(p_mutationID);
}

// Insert a new object in the database
bool
CXSession::InsertObjectInDatabase(CXObject* p_object,int p_mutationID /*= 0*/)
{
  CXClass*   theClass = p_object->GetClass();
  CXTable*      table = theClass->GetTable();
  SQLDataSet*    dset = table->GetDataSet();
  SQLRecord*   record = dset->InsertRecord();
  MColumnMap& columns = table->GetColumnInfo();
  SQLVariant zero;

  // Connect our database
  dset->SetDatabase(GetDatabase());

  // See if dataset is empty
  if(dset->GetNumberOfRecords() == 1 && dset->GetNumberOfFields() == 0)
  {
    for (auto& column : columns)
    {
      dset->InsertField(column.m_column, &zero);
    }
  }

  // Set the values on the record
  for(int ind = 0;ind < dset->GetNumberOfFields();++ind)
  {
    record->SetField(ind,&zero);
  }

  // New mutation ID for this update action
  if(p_mutationID == 0)
  {
    p_mutationID = ++m_mutation;
  }

  // Now serialize our object with the 'real' values
  p_object->Serialize(*record, p_mutationID);
  // Set the record to 'insert-only'
  record->Inserted();

  // Check if we must generate our primary key
  CXAttribute* gen = theClass->FindGenerator();
  if(gen && p_object->IsTransient())
  {
    // -1: not found, 0 -> (n-1) is the field number of the generator
    int generator = dset->GetFieldNumber(gen->GetName());
    record->SetGenerator(generator);
  }

  // Go save the record
  bool saved = dset->Synchronize(p_mutationID);
  if(saved)
  {
    // Re-sync the primary key
    p_object->ResetPrimaryKey();
    p_object->DeSerialize(*record);
  }
  return saved;
}

bool
CXSession::DeleteObjectInDatabase(CXObject* p_object,int p_mutationID /*= 0*/)
{
  // Getting the database record
  CXClass* theClass = p_object->GetClass();
  CXTable*    table = theClass->GetTable();
  SQLDataSet*  dset = table->GetDataSet();
  SQLRecord* record = p_object->GetDatabaseRecord();

  // Connect our database
  dset->SetDatabase(GetDatabase());

  if(record == nullptr || dset == nullptr)
  {
    return false;
  }

  // If object was **NOT** in the database, removal did work ok
  if(p_object->IsPersistent() == false)
  {
    return true;
  }

  // Set the record to the delete status
  record->Delete();

  // New mutation ID for this update action
  if(p_mutationID == 0)
  {
    p_mutationID = ++m_mutation;
  }

  // BEWARE: We need not "Serialize" our object
  // We take the assumption that the primary key is "immutable"

  // Go delete the record
  bool deleted = dset->Synchronize(p_mutationID);
  if (deleted)
  {
    p_object->MakeTransient();
  }
  return deleted;
}

// DML operations in the filestore
bool
CXSession::UpdateObjectInFilestore(CXObject* p_object,int p_mutationID /*= 0*/)
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
  if(!table->SchemaName().IsEmpty())
  {
    msg.SetAttribute(entity,"schema",table->SchemaName());
  }
  msg.SetAttribute(entity,"name",table->TableName());

  // Serialize our object
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
CXSession::InsertObjectInFilestore(CXObject* p_object,int p_mutationID /*= 0*/)
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
  msg.SetAttribute(entity,"name",table->TableName());

  // Serialize our object
  p_object->Serialize(msg,entity);

  // Save to the file: overwriting the file
  return SaveSOAPMessage(msg,filename);
}

bool
CXSession::DeleteObjectInFilestore(CXObject* p_object,int p_mutationID /*= 0*/)
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
      return true;
    }
  }
  return false;
}

//////////////////////////////////////////////////////////////////////////
//
// TRY TO CREATE A NEW (TEST) DATABASE FOR THIS SESSION
//
//////////////////////////////////////////////////////////////////////////

void
CXSession::TryCreateDatabase()
{
  for(auto& cl : m_classes)
  {
    // cl.second->TryCreateTable()
  }
}

void
CXSession::TryDropDatabase()
{
  for (auto& cl : m_classes)
  {
    // cl.second->TryDropTable()
  }
}
