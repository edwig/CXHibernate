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
#include "CXPrimaryHash.h"
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
CXSession::CXSession()
          :m_role(CXH_Internet_role)
{
}

// CTOR Filestore session
CXSession::CXSession(CString p_directory)
          :m_baseDirectory(p_directory)
          ,m_role(CXH_Filestore_role)
{
}

// CTOR Master session
CXSession::CXSession(CString p_database,CString p_user,CString p_password)
          :m_role(CXH_Database_role)
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
  ClearTables();

  // Destroy database (if any)
  if(m_database && m_ownDatabase)
  {
    delete m_database;
    m_database = nullptr;
  }
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

// Setting an alternate filestore location
void 
CXSession::SetBaseDirectory(CString p_directory)
{
  m_role = CXH_Filestore_role;
  m_baseDirectory = p_directory;
}

// Add a table to the session
bool
CXSession::AddTable(CXTable* p_table,CString p_name /*=""*/)
{
  // Getting the table name
  CString table(p_name);
  if(table.IsEmpty())
  {
    table = p_table->TableName();
  }
  table.MakeLower();

  // Add to the map of tables, but only if we not had it previously
  TableMap::iterator it = m_tables.find(table);
  if(it != m_tables.end())
  {
    return false;
  }

  // Store the table definition
  m_tables.insert(std::make_pair(table,p_table));

  // Create a caching object in the cache
  TableCache* tcache = new TableCache();
  m_cache.insert(std::make_pair(table, tcache));

  // On the master side, we need a SQLDataSet for the database
  if(m_role == CXH_Database_role)
  {
    SQLDataSet* dset = new SQLDataSet();

    // Fill in the dataset
    dset->SetDatabase(m_database);
    dset->SetPrimaryTable(p_table->SchemaName(),p_table->TableName());
    WordList list = p_table->GetPrimaryKeyAsList();
    dset->SetPrimaryKeyColumn(list);

    // Default query
    CString query = CString("SELECT * FROM ") + p_table->DMLTableName(m_database->GetSQLInfoDB());
    dset->SetQuery(query);

    // Transfer to the CXTable object
    p_table->SetDataSet(dset);
  }
  return true;
}

// Finding a table
CXTable*
CXSession::FindTable(CString p_name)
{
  p_name.MakeLower();
  TableMap::iterator it = m_tables.find(p_name);
  if(it != m_tables.end())
  {
    return it->second;
  }
  return nullptr;
}

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
    m_transaction = new SQLTransaction(m_database,"mutation");
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
// QUERY INTERFACE
//
//////////////////////////////////////////////////////////////////////////

CXObject*
CXSession::SelectObject(CString p_tableName,SQLVariant* p_primary)
{
  VariantSet set;
  set.push_back(p_primary);

  return SelectObject(p_tableName,set);
}

CXObject*
CXSession::SelectObject(CString p_table,VariantSet& p_primary)
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
CXSession::SelectObject(CString p_tableName,SQLFilter* p_filter)
{
  SQLFilterSet set;
  set.push_back(*p_filter);

  return SelectObject(p_tableName,set);
}

CXResultSet
CXSession::SelectObject(CString p_table,SQLFilterSet& p_filters)
{
  CXResultSet set;

  p_table.MakeLower();
  ClearCache(p_table);

  // Create a caching object in the cache
  TableCache* tcache = new TableCache();
  m_cache.insert(std::make_pair(p_table, tcache));

  if(m_role == CXH_Database_role)
  {
    SelectObjectsFromDatabase(p_table,p_filters);
  }
  else if(m_role == CXH_Internet_role)
  {
    SelectObjectsFromInternet(p_table,p_filters);
  }
  else // CXH_Filestore_role
  {
    SelectObjectsFromFilestore(p_table,p_filters);
  }

  // Getting the result set
  CXCache::iterator it = m_cache.find(p_table);
  if(it != m_cache.end())
  {
    for(auto& obj : *it->second)
    {
      set.push_back(obj.second);
    }
  }

  return set;
}

// Update an object
bool
CXSession::UpdateObject(CXObject* p_object,int p_mutationID /*= 0*/)
{
  CXTable* table = p_object->BelongsToTable();

  if(m_role == CXH_Database_role)
  {
    return UpdateObjectInDatabase(table,p_object,p_mutationID);
  }
  else if(m_role == CXH_Internet_role)
  {
    // Save as a SOAP message
    // UpdateObjectInInternet(table,p_object);
    return false;
  }
  else // CXH_Filestore_role
  {
    UpdateObjectInFilestore(table,p_object);
  }
  return true;
}

bool
CXSession::InsertObject(CXObject* p_object,int p_mutationID /*= 0*/)
{
  CXTable* table = p_object->BelongsToTable();

  if(m_role == CXH_Database_role)
  {
    InsertObjectInDatabase(table,p_object,p_mutationID);
  }
  else if(m_role == CXH_Internet_role)
  {
    // Insert as a SOAP Message
    // InsertObjectInInternet(table,p_object);
    return false;
  }
  else // CXH_Filestore_role
  {
    InsertObjectInFilestore(table,p_object,p_mutationID);
  }
  // Add object to the cache
  if(p_object->IsPersistent())
  {
    return AddObjectInCache(p_object,p_object->GetPrimaryKey());
  }
  return false;
}

bool
CXSession::DeleteObject(CXObject* p_object,int p_mutationID /*= 0*/)
{
  CXTable* table = p_object->BelongsToTable();

  if(m_role == CXH_Database_role)
  {
    DeleteObjectInDatabase(table,p_object,p_mutationID);
  }
  else if(m_role == CXH_Internet_role)
  {
    // DeleteObjectInInternet(table,p_object);
    return false;
  }
  else // CXH_Filestore_role
  {
    DeleteObjectInFilestore(table,p_object,p_mutationID);
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
  for(auto& tcache : m_cache)
  {
    TableCache& objects = *tcache.second;
    for(auto& obj : objects)
    {
      CXObject*  object = obj.second;
      SQLRecord* record = object->GetDatabaseRecord();
      object->Serialize(*record,mutationID);
      if(UpdateObject(object,mutationID) == false)
      {
        RollbackMutation(mutationID);
        return false;
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

  fileName += "Object_";

  fileName += CXPrimaryHash(p_primary);
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
CXSession::ClearCache(CString p_table /*= ""*/)
{
  if(m_cache.empty())
  {
    return;
  }

  CXCache::iterator it = m_cache.begin();
  while (it != m_cache.end())
  {
    if (p_table.IsEmpty() || p_table.CompareNoCase(it->first) == 0)
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

// Clear the table definition cache
void
CXSession::ClearTables(CString p_table /*= ""*/)
{
  TableMap::iterator it = m_tables.begin();
  while(it != m_tables.end())
  {
    if(p_table.IsEmpty() || p_table.CompareNoCase(it->first) == 0)
    {
      delete it->second;
      it = m_tables.erase(it);
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
    catch(...)
    {
      // Gaat in stilte fout
    }
  }
}

// Add an object to the cache
bool
CXSession::AddObjectInCache(CXObject* p_object, VariantSet& p_primary)
{
  CString hash = CXPrimaryHash(p_primary);
  CString tableName = p_object->BelongsToTable()->TableName();
  tableName.MakeLower();

  CXCache::iterator it = m_cache.find(tableName);
  if (it != m_cache.end())
  {
    TableCache::iterator tit = it->second->find(hash);
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
  CString tableName = p_object->BelongsToTable()->TableName();
  tableName.MakeLower();

  CXCache::iterator it = m_cache.find(tableName);
  if (it != m_cache.end())
  {
    TableCache* tcache = it->second;
    TableCache::iterator tit = tcache->find(hash);
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
    SQLFilter filter(column,SQLOperator::OP_Equal,p_primary[ind++]);
    p_filters.push_back(filter);
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
    TableCache::iterator tit = it->second->find(hash);
    if (tit != it->second->end())
    {
      return tit->second;
    }
  }
  return nullptr;
}

// Try to find an object in the database
CXObject*
CXSession::FindObjectInDatabase(CString p_table,VariantSet& p_primary)
{
  // Find the CXTable object
  p_table.MakeLower();
  TableMap::iterator it = m_tables.find(p_table);
  if(it == m_tables.end())
  {
    return nullptr;
  }
  CXTable* table = it->second;

  // See if we have a data set
  SQLDataSet* dset = table->GetDataSet();
  if(dset == nullptr)
  {
    return nullptr;
  }

  SQLFilterSet fset;
  if(CreateFilterSet(table,p_primary,fset))
  {
    dset->SetFilters(fset);

    // Open our dataset (and search)
    if(dset->IsOpen() == false)
    {
      dset->Open();
    }
    else
    {
      dset->Append();
    }
  }
  else return nullptr;

  // See if we have a record
  SQLRecord* record = dset->FindObjectRecord(p_primary);
  if(record == nullptr)
  {
    return nullptr;
  }

  // Create our object by the creation factory
  CreateCXO create = table->GetCreateCXO();
  CXObject* object = (*create)(table);

  // De-serialize the SQL Record to an CXObject derived object
  object->DeSerialize(*record);

  return object;
}

// Try to find an object in the filestore
CXObject*
CXSession::FindObjectInFilestore(CString p_table,VariantSet& p_primary)
{
  CXTable* table = FindTable(p_table);

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
        CreateCXO create = table->GetCreateCXO();
        CXObject* object = (*create)(table);

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
CXSession::SelectObjectsFromDatabase(CString p_table,SQLFilterSet& p_filters)
{
  // Find the CXTable object
  p_table.MakeLower();
  TableMap::iterator it = m_tables.find(p_table);
  if (it == m_tables.end())
  {
    return;
  }
  CXTable* table = it->second;

  // See if we have a data set
  SQLDataSet* dset = table->GetDataSet();
  if (dset == nullptr)
  {
    return;
  }

  // Close dataset if it was opened
  if(dset->IsOpen())
  {
    dset->Close();
  }

  // Propagate our filters
  dset->SetFilters(p_filters);
  // Set our query
  CString query = CString("SELECT * FROM ") + table->DMLTableName(m_database->GetSQLInfoDB());
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
      CreateCXO create = table->GetCreateCXO();
      CXObject* object = (*create)(table);

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
}

void
CXSession::SelectObjectsFromFilestore(CString p_table,SQLFilterSet& p_filters)
{
  throw CString("Cannot select multiple objects from the filestore");
}

void
CXSession::SelectObjectsFromInternet(CString p_table,SQLFilterSet& p_filters)
{

}

bool
CXSession::UpdateObjectInDatabase(CXTable* p_table,CXObject* p_object,int p_mutationID /*= 0*/)
{
  SQLRecord* record = p_object->GetDatabaseRecord();
  SQLDataSet*  dset = p_table->GetDataSet();

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
CXSession::InsertObjectInDatabase(CXTable* p_table,CXObject* p_object,int p_mutationID /*= 0*/)
{
  SQLDataSet*    dset = p_table->GetDataSet();
  SQLRecord*   record = dset->InsertRecord();
  MColumnMap& columns = p_table->GetColumnInfo();
  SQLVariant zero;

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
  p_object->Serialize(*record,p_mutationID);
  // Set the record to 'insert-only'
  record->Inserted();

  // Go save the record
  return dset->Synchronize(p_mutationID);
}

bool
CXSession::DeleteObjectInDatabase(CXTable* p_table,CXObject* p_object,int p_mutationID /*= 0*/)
{
  // Getting the database record
  SQLDataSet*  dset = p_table->GetDataSet();
  SQLRecord* record = p_object->GetDatabaseRecord();
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
  return dset->Synchronize(p_mutationID);
}

// DML operations in the filestore
bool
CXSession::UpdateObjectInFilestore(CXTable* p_table, CXObject* p_object, int p_mutationID /*= 0*/)
{
  // See if we can create a directory and filename
  CString filename = CreateFilestoreName(p_table,p_object->GetPrimaryKey());
  if (filename.IsEmpty())
  {
    return false;
  }

  // Create a SOAP message object
  SOAPMessage msg(DEFAULT_NAMESPACE, "Object");

  // Adding an entity with name attributes
  XMLElement* entity = msg.SetParameter("Entity", "");
  if(!p_table->SchemaName().IsEmpty())
  {
    msg.SetAttribute(entity,"schema",p_table->SchemaName());
  }
  msg.SetAttribute(entity,"name",p_table->TableName());

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
CXSession::InsertObjectInFilestore(CXTable* p_table,CXObject* p_object,int p_mutationID /*= 0*/)
{
  // See if we can create a directory and filename
  CString filename = CreateFilestoreName(p_table,p_object->GetPrimaryKey());
  if(filename.IsEmpty())
  {
    return false;
  }

  // Create a SOAP message object
  CString namesp(DEFAULT_NAMESPACE);
  CString action("Object");
  SOAPMessage msg(namesp,action,SoapVersion::SOAP_12);
  XMLElement* entity = msg.SetParameter("Entity","");
  msg.SetAttribute(entity,"name",p_table->TableName());

  // Serialize our object
  p_object->Serialize(msg,entity);

  // Save to the file: overwriting the file
  return SaveSOAPMessage(msg,filename);
}

bool
CXSession::DeleteObjectInFilestore(CXTable* p_table, CXObject* p_object, int p_mutationID /*= 0*/)
{
  // See if we can create a directory and filename
  CString filename = CreateFilestoreName(p_table,p_object->GetPrimaryKey());
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
