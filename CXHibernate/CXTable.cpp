////////////////////////////////////////////////////////////////////////
//
// File: CXTable.cpp
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
#include "CXTable.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// CTOR: Empty table object
CXTable::CXTable()
        :m_dataSet(nullptr)
{
}

// CTOR: Standard creation of a table
CXTable::CXTable(CString p_schema, CString p_table)
        :m_dataSet(nullptr)
{
  m_table.m_schema     = p_schema;
  m_table.m_table      = p_table;
  m_table.m_objectType = OBJECT_TYPE_TABLE;
}

// DTOR: Destruct table
CXTable::~CXTable()
{
  if(m_dataSet)
  {
    delete m_dataSet;
  }
}

// Master side must apply a SQLDataSet
void
CXTable::SetDataSet(SQLDataSet* p_dataset)
{
  m_dataSet = p_dataset;
}

SQLDataSet*
CXTable::GetDataSet()
{
  return m_dataSet;
}

CString
CXTable::SchemaName()
{
  return m_table.m_schema;
}

CString   
CXTable::TableName()
{
  return m_table.m_table;
}

CString
CXTable::SchemaTableName()
{
  CString name;

  if(!m_table.m_schema.IsEmpty())
  {
    name += m_table.m_schema + ".";
  }
  name += m_table.m_table;
  return name;
}

CString   
CXTable::FullQualifiedTableName()
{
  CString name;

  if(!m_table.m_catalog.IsEmpty())
  {
    name = m_table.m_catalog + ".";
  }
  if (!m_table.m_schema.IsEmpty())
  {
    name += m_table.m_schema + ".";
  }
  name += m_table.m_table;

  return name;
}

// Create a table name for a DML (SELECT, INSERT, UPDATE, DELETE) statement
// This will construct different names on different database platforms
// Firebird : tablename
// Oracle   : schema.tablename[@catalog]
// SQLServer: catalog.schema.tablename
// MySQL    : schema.tablename
CString   
CXTable::DMLTableName(SQLInfoDB* p_info)
{
  CString name;
  CString catalog;
  p_info->GetInfo();

  if(p_info->GetSupportsCatalogs())
  {
    if(p_info->GetCatalogNameUsage() & SQL_CU_DML_STATEMENTS)
    {
      catalog = m_table.m_catalog;
    }
  }
  int catalogLocation = p_info->GetCatalogLocation();
  if((catalogLocation == SQL_CL_START) && !catalog.IsEmpty())
  {
    name = catalog + p_info->GetCatalogNameSeparator();
  }

  if(p_info->GetSchemaNameUsage() & SQL_SU_DML_STATEMENTS)
  {
    if (!m_table.m_schema.IsEmpty())
    {
      name += m_table.m_schema;
      name += ".";
    }
  }

  // Now comes the table name
  name += m_table.m_table;

  // Catalog at the end of the name
  if((catalogLocation == SQL_CL_END) && !catalog.IsEmpty())
  {
    name += p_info->GetCatalogNameSeparator();
    name += catalog;
  }

  return name;
}


bool
CXTable::GetIsTable()
{
  return (m_table.m_objectType.Compare(OBJECT_TYPE_TABLE) == 0);
}

bool
CXTable::GetIsView()
{
  return (m_table.m_objectType.Compare(OBJECT_TYPE_VIEW) == 0);
}

bool
CXTable::GetIsTemporary()
{
  return (m_table.m_objectType.Compare(OBJECT_TYPE_GLOBALTEMP) == 0 ||
          m_table.m_objectType.Compare(OBJECT_TYPE_LOCALTEMP)  == 0 );
}

bool
CXTable::GetIsSynonym()
{
  return (m_table.m_objectType.Compare(OBJECT_TYPE_SYNONYM) == 0 ||
          m_table.m_objectType.Compare(OBJECT_TYPE_ALIAS)   == 0 );
}

void
CXTable::SetInfoTable(MetaTable& p_metaTable)
{
  m_table = p_metaTable;
}

void
CXTable::AddInfoColumn(MetaColumn& p_metaColumn)
{
  m_columns.push_back(p_metaColumn);
}

void
CXTable::SetPrimaryKey(MetaPrimary& p_metaPrimary)
{
  m_primary.push_back(p_metaPrimary);
}

void
CXTable::AddForeignKey(MetaForeign& p_metaForeign)
{
  m_foreigns.push_back(p_metaForeign);
}

void
CXTable::AddIndex(MetaIndex& p_metaIndex)
{
  m_indices.push_back(p_metaIndex);
}

void
CXTable::AddPrivilege(MetaPrivilege& p_metaPrivilege)
{
  m_privileges.push_back(p_metaPrivilege);
}

// Get info from ODBC out of the database.
// Can throw a CString error!
bool
CXTable::GetMetaInfoFromDatabase(SQLDatabase& p_database
                                ,bool p_getForeigns   /*= false*/
                                ,bool p_getIndices    /*= false*/
                                ,bool p_getPrivileges /*= false*/)
{
  // See if we have something to go on...
  if(m_table.m_table.IsEmpty())
  {
    return false;
  }

  // Getting the info object
  SQLInfoDB* info = p_database.GetSQLInfoDB();

  // Get mandatory info always
  GetTableInfo(info);
  GetColumnInfo(info);
  GetPrimaryKeyInfo(info);

  // Get optional info
  if(p_getForeigns)
  {
    GetForeignKeyInfo(info);
  }
  if(p_getIndices)
  {
    GetIndexInfo(info);
  }
  if(p_getPrivileges)
  {
    GetPrivilegeInfo(info);
  }
  return true;
}

void 
CXTable::GetTableInfo(SQLInfoDB* p_info)
{
  CString   errors;
  MTableMap tables;

  // Find table info
  if(!p_info->MakeInfoTableTable(tables,errors,m_table.m_schema,m_table.m_table))
  {
    throw CString("Cannot find table: ") + FullQualifiedTableName() + " : " + errors;
  }

  // Some engines get a synonym AND a table/view record
  // The table record always comes last.
  if(!tables.empty())
  {
    m_table = tables.back();
  }
}

void 
CXTable::GetColumnInfo(SQLInfoDB* p_info)
{
  CString errors;

  // Find column info
  m_columns.clear();
  if(!p_info->MakeInfoTableColumns(m_columns,errors,m_table.m_schema,m_table.m_table))
  {
    throw CString("Cannot find columns for table: ") + FullQualifiedTableName() + " : " + errors;
  }
}

void 
CXTable::GetPrimaryKeyInfo(SQLInfoDB* p_info)
{
  CString errors;

  // Find primary column info
  m_primary.clear();
  p_info->MakeInfoTablePrimary(m_primary,errors,m_table.m_schema,m_table.m_table);
  if(!errors.IsEmpty())
  {
    throw CString("Cannot find the primary key for table: ") + FullQualifiedTableName() + " : " + errors;
  }
}

void 
CXTable::GetForeignKeyInfo(SQLInfoDB* p_info)
{
  CString errors;

  // Find column info
  m_foreigns.clear();
  p_info->MakeInfoTableForeign(m_foreigns,errors,m_table.m_schema,m_table.m_table);
  if (!errors.IsEmpty())
  {
    throw CString("Cannot find the foreign keys for table: ") + FullQualifiedTableName() + " : " + errors;
  }
}

void 
CXTable::GetIndexInfo(SQLInfoDB* p_info)
{
  CString errors;

  // Find column info
  m_indices.clear();
  p_info->MakeInfoTableStatistics(m_indices,errors,m_table.m_schema,m_table.m_table,nullptr);
  if (!errors.IsEmpty())
  {
    throw CString("Cannot find indices for table: ") + FullQualifiedTableName() + " : " + errors;
  }
}

void 
CXTable::GetPrivilegeInfo(SQLInfoDB* p_info)
{
  CString errors;

  // Find column info
  m_privileges.clear();
  p_info->MakeInfoTablePrivileges(m_privileges,errors,m_table.m_schema,m_table.m_table);
  if(!errors.IsEmpty())
  {
    throw CString("Cannot find the privileges for table: ") + FullQualifiedTableName() + " : " + errors;
  }
}

// Getting the mappings

MetaTable&
CXTable::GetTableInfo() const
{
  return const_cast<MetaTable&>(m_table);
}

MColumnMap&
CXTable::GetColumnInfo() const
{
  return const_cast<MColumnMap&>(m_columns);
}

MPrimaryMap&
CXTable::GetPrimaryInfo() const
{
  return const_cast<MPrimaryMap&>(m_primary);
}

MForeignMap&
CXTable::GetForeignInfo() const
{
  return const_cast<MForeignMap&>(m_foreigns);
}

MIndicesMap&
CXTable::GetIndicesInfo() const
{
  return const_cast<MIndicesMap&>(m_indices);
}

MPrivilegeMap&
CXTable::GetPrivilegesInfo() const
{
  return const_cast<MPrivilegeMap&>(m_privileges);
}

WordList
CXTable::GetPrimaryKeyAsList()
{
  WordList list;

  for (auto& prim : m_primary)
  {
    list.push_back(prim.m_columnName);
  }
  return list;
}
