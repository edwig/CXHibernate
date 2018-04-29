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

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// CTOR Default session (slave side of the session)
CXSession::CXSession()
          :m_master(false)
          ,m_database(nullptr)
          ,m_mutation(0)
{
}

// CTOR Master session
CXSession::CXSession(CString p_database,CString p_user,CString p_password)
          :m_mutation(0)
          ,m_ownDatabase(true)
{
  m_database = new SQLDatabase();
  if(m_database->Open(p_database,p_user,p_password))
  {
    m_master = true;
  }
}

// DTOR: Free all tables and records
CXSession::~CXSession()
{
  // Destroy the cache
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
CXSession::SetMaster(bool p_master)
{
  m_master = p_master;
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
  if(m_master)
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

//////////////////////////////////////////////////////////////////////////
//
// QUERY INTERFACE
//
//////////////////////////////////////////////////////////////////////////

CXObject*
CXSession::SelectObject(CString p_tableName,SQLVariant* p_primary,CreateCXO p_create)
{
  VariantSet set;
  set.push_back(p_primary);

  return SelectObject(p_tableName,set,p_create);
}

CXObject*
CXSession::SelectObject(CString p_table,VariantSet& p_primary,CreateCXO p_create)
{
  // Search in cache
  CXObject* object = FindObjectInCache(p_table,p_primary);
  if(object)
  {
    return object;
  }

  // If not found, search in database / SOAP connection
  if(m_master)
  {
    object = FindObjectInDatabase(p_table,p_primary,p_create);
  }
  else
  {
    // Query for object on SOAP connection
    object = FindObjectOnInternet(p_table,p_primary,p_create);
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
CXSession::SelectObject(CString p_tableName,SQLFilter* p_filter,CreateCXO p_create)
{
  SQLFilterSet set;
  set.push_back(*p_filter);

  return SelectObject(p_tableName,set,p_create);
}

CXResultSet
CXSession::SelectObject(CString p_table,SQLFilterSet& p_filters,CreateCXO p_create)
{
  CXResultSet set;

  p_table.MakeLower();
  ClearCache(p_table);

  // Create a caching object in the cache
  TableCache* tcache = new TableCache();
  m_cache.insert(std::make_pair(p_table, tcache));

  if(m_master)
  {
    SelectObjectsFromDatabase(p_table,p_filters,p_create);
  }
  else
  {
    SelectObjectsFromInternet(p_table,p_filters,p_create);
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
CXSession::UpdateObject(CXObject* p_object)
{
  CXTable* table = p_object->BelongsToTable();

  if(m_master)
  {
    return UpdateObjectInDatabase(table,p_object);
  }
  else
  {
    // Save as a SOAP message
    return false;
  }
  return true;
}

bool
CXSession::InsertObject(CXObject* p_object)
{
  CXTable* table = p_object->BelongsToTable();

  if(m_master)
  {
    InsertObjectInDatabase(table,p_object);
  }
  else
  {
    // Insert as a SOAP Message
    return false;
  }
  // Add object to the cache
  if(p_object->IsPersistent())
  {
    AddObjectInCache(p_object,p_object->GetPrimaryKey());
  }
  return true;
}

bool
CXSession::DeleteObject(CXObject* p_object)
{
  return false;
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
CXSession::FindObjectInDatabase(CString p_table,VariantSet& p_primary,CreateCXO p_create)
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
  CXObject* object = (*p_create)(table);

  // De-serialize the SQL Record to an CXObject derived object
  object->DeSerialize(*record);

  return object;
}

// Try to find an object via the SOAP interface
CXObject*
CXSession::FindObjectOnInternet(CString p_table,VariantSet& p_primary,CreateCXO p_create)
{
  return nullptr;
}

void
CXSession::SelectObjectsFromDatabase(CString p_table,SQLFilterSet& p_filters,CreateCXO p_create)
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
      CXObject* object = (*p_create)(table);

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
CXSession::SelectObjectsFromInternet(CString p_table,SQLFilterSet& p_filters,CreateCXO p_create)
{

}

bool
CXSession::UpdateObjectInDatabase(CXTable* p_table,CXObject* p_object)
{
  SQLRecord* record = p_object->GetDatabaseRecord();
  SQLDataSet*  dset = p_table->GetDataSet();

  // New mutation ID for this update action
  ++m_mutation;

  // Serialize object to database record
  p_object->Serialize(*record, m_mutation);
  return dset->Synchronize(m_mutation);
}

// Insert a new object in the database
bool
CXSession::InsertObjectInDatabase(CXTable* p_table,CXObject* p_object)
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
  ++m_mutation;

  // Now serialize our object with the 'real' values
  p_object->Serialize(*record,m_mutation);
  // Set the record to 'insert-only'
  record->Inserted();

  // Go save the record
  return dset->Synchronize(m_mutation);
}
