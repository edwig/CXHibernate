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
#include "CXHibernate.h"
#include "CXTable.h"
#include <SOAPMessage.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// CTOR: Standard creation of a table
CXTable::CXTable(CString p_table)
{
  m_table.m_catalog    = hibernate.GetDefaultCatalog();
  m_table.m_schema     = hibernate.GetDefaultSchema();
  m_table.m_table      = p_table;
  m_table.m_objectType = OBJECT_TYPE_TABLE;
}

// DTOR: Destruct table
CXTable::~CXTable()
{
  if(m_dataSet)
  {
    delete m_dataSet;
    m_dataSet = nullptr;
  }
}

// Setting a different schema only
void
CXTable::SetSchema(CString p_schema)
{
  m_table.m_schema = p_schema;
}

// Setting the schema/table/type info
void
CXTable::SetSchemaTableType(CString p_schema,CString p_table,CString p_type)
{
  m_table.m_schema     = p_schema;
  m_table.m_table      = p_table;
  m_table.m_objectType = p_type;
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
  if(m_dataSet == nullptr)
  {
    m_dataSet = new SQLDataSet();

    // Fill in the dataset
    m_dataSet->SetPrimaryTable(SchemaName(),TableName());
    WordList list = GetPrimaryKeyAsList();
    m_dataSet->SetPrimaryKeyColumn(list);
  }
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
  name += TableName();
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
  name += TableName();

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
  name += TableName();

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
CXTable::AddPrimaryKey(MetaPrimary& p_metaPrimary)
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
CXTable::AddSequence(MetaSequence& p_metaSequence)
{
  m_sequence = p_metaSequence;
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

// Serialize the info of the table
bool
CXTable::SaveMetaInfo(CXSession* p_session,CString p_filename)
{
  // Getting a XML SOAPMessage
  CString namesp(DEFAULT_NAMESPACE);
  CString action("TableMetaInfo");
  SOAPMessage msg(namesp,action);

  // Saving table info to the XML message
  SaveTableInfo(msg);
  SaveColumnInfo(msg);
  SavePrimaryKey(msg);
  SaveForeignKey(msg);
  SaveIndices(msg);
  SavePrivileges(msg);

  // Storing the info on the file system
  CString filename = p_filename.IsEmpty() ? p_session->CreateFilestoreName(this) : p_filename;
  return p_session->SaveSOAPMessage(msg,filename);
}

bool
CXTable::LoadMetaInfo(CXSession* p_session,CString p_filename)
{
  SOAPMessage msg;
  CString filename = p_filename.IsEmpty() ? p_session->CreateFilestoreName(this) : p_filename;

  // Load our table description and see if everything went well
  if((msg.LoadFile(filename) == false) ||
     (msg.GetInternalError() != XmlError::XE_NoError))
  {
    return false;
  }

  // Forget previous info
  m_columns.clear();
  m_primary.clear();
  m_foreigns.clear();
  m_indices.clear();
  m_privileges.clear();

  // Loading info from the XML message
  LoadTableInfo(msg);
  LoadColumnInfo(msg);
  LoadPrimaryKey(msg);
  LoadForeignKey(msg);
  LoadIndices(msg);
  LoadPrivileges(msg);

  return true;
}

//////////////////////////////////////////////////////////////////////////
//
// PRIVATE
//
//////////////////////////////////////////////////////////////////////////

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

//////////////////////////////////////////////////////////////////////////

void 
CXTable::SaveTableInfo(SOAPMessage& p_msg)
{
  XMLElement* elem = p_msg.SetParameter("TableInfo","");

  p_msg.AddElement(elem,"Schema",    XDT_String |XDT_Type,m_table.m_schema);
  p_msg.AddElement(elem,"Table",     XDT_String |XDT_Type,m_table.m_table);
  p_msg.AddElement(elem,"ObjectType",XDT_String |XDT_Type,m_table.m_objectType);
  p_msg.AddElement(elem,"Remarks",   XDT_String |XDT_Type,m_table.m_remarks);
  p_msg.AddElement(elem,"FullName",  XDT_String |XDT_Type,m_table.m_fullName);
  p_msg.AddElement(elem,"Tablespace",XDT_String |XDT_Type,m_table.m_tablespace);
  p_msg.AddElement(elem,"Temporary", XDT_Boolean|XDT_Type,m_table.m_temporary);
}

void 
CXTable::SaveColumnInfo(SOAPMessage& p_msg)
{
  XMLElement* columns = p_msg.SetParameter("Columns", "");

  for(auto& col : m_columns)
  {
    XMLElement* column = p_msg.AddElement(columns,"Column",XDT_String,"");

    p_msg.AddElement(column, "Position",      XDT_Integer|XDT_Type,col.m_position);
    p_msg.AddElement(column, "ColumnName",    XDT_String |XDT_Type,col.m_column);
    p_msg.AddElement(column, "ODBC_Type",     XDT_Integer|XDT_Type,col.m_datatype);
    p_msg.AddElement(column, "RDBMS_Type",    XDT_String |XDT_Type,col.m_typename);
    p_msg.AddElement(column, "ColumnSize",    XDT_Integer|XDT_Type,col.m_columnSize);
    p_msg.AddElement(column, "BufferLength",  XDT_Integer|XDT_Type,col.m_bufferLength);
    p_msg.AddElement(column, "DecimalDigits", XDT_Integer|XDT_Type,col.m_decimalDigits);
    p_msg.AddElement(column, "Radix",         XDT_Integer|XDT_Type,col.m_numRadix);
    p_msg.AddElement(column, "Nullable",      XDT_Integer|XDT_Type,col.m_nullable);
    p_msg.AddElement(column, "Remarks",       XDT_String |XDT_Type,col.m_remarks);
    p_msg.AddElement(column, "DefaultValue",  XDT_String |XDT_Type,col.m_default);
    p_msg.AddElement(column, "ODBC_Type3",    XDT_Integer|XDT_Type,col.m_datatype3);
    p_msg.AddElement(column, "SubType",       XDT_Integer|XDT_Type,col.m_sub_datatype);
    p_msg.AddElement(column, "OctetLength",   XDT_Integer|XDT_Type,col.m_octet_length);
  }
}

void 
CXTable::SavePrimaryKey(SOAPMessage& p_msg)
{
  XMLElement* elem = p_msg.SetParameter("PrimaryKey", "");

  bool first = true;
  for(auto& pkey : m_primary)
  {
    if(first)
    {
      p_msg.AddElement(elem,"ConstraintName",   XDT_String |XDT_Type,pkey.m_constraintName);
      p_msg.AddElement(elem,"Deferrable",       XDT_Integer|XDT_Type,pkey.m_deferrable);
      p_msg.AddElement(elem,"InitiallyDeferred",XDT_Integer|XDT_Type,pkey.m_initiallyDeferred);
    }
    first = false;

    XMLElement* col = p_msg.AddElement(elem,"Column",  XDT_String,"");
    p_msg.AddElement(col,"Position",XDT_Integer|XDT_Type,pkey.m_columnPosition);
    p_msg.AddElement(col,"Name",    XDT_String |XDT_Type,pkey.m_columnName);
  }
}

void 
CXTable::SaveForeignKey(SOAPMessage& p_msg)
{
  XMLElement* foreign = nullptr;
  XMLElement* columns = nullptr;
  XMLElement* pkcols  = nullptr;

  XMLElement* elem = p_msg.SetParameter("ForeignKeys","");

  for(auto& fkey : m_foreigns)
  {
    if(fkey.m_keySequence == 1)
    {
      foreign = p_msg.AddElement(elem,"ForeignKey",XDT_String,"");

      // Foreign key part
      p_msg.AddElement(foreign,"Constraint",XDT_String |XDT_Type,fkey.m_foreignConstraint);
      p_msg.AddElement(foreign,"UpdateRule",XDT_Integer|XDT_Type,fkey.m_updateRule);
      p_msg.AddElement(foreign,"DeleteRule",XDT_Integer|XDT_Type,fkey.m_deleteRule);
      p_msg.AddElement(foreign,"Deferrable",XDT_Integer|XDT_Type,fkey.m_deferrable);
      p_msg.AddElement(foreign,"Match",     XDT_Integer|XDT_Type,fkey.m_match);
      p_msg.AddElement(foreign,"InitiallyDeferred",XDT_Integer|XDT_Type,fkey.m_initiallyDeferred);
      p_msg.AddElement(foreign,"Enabled",   XDT_Integer|XDT_Type,fkey.m_enabled);

      // Prepare for foreign-key columns
      columns = p_msg.AddElement(foreign,"Columns",XDT_String,"");

      // Primary key part
      XMLElement* prim = p_msg.AddElement(foreign,"Primary",XDT_String,"");
      p_msg.AddElement(prim,"Schema",    XDT_String|XDT_Type,fkey.m_pkSchemaName);
      p_msg.AddElement(prim,"Table",     XDT_String|XDT_Type,fkey.m_pkTableName);

      // Prepare for primary-key columns
      pkcols = p_msg.AddElement(prim,"Columns",XDT_String,"");
    }

    // For all columns
    p_msg.AddElement(columns,"Column",XDT_String|XDT_Type,fkey.m_fkColumnName);
    p_msg.AddElement(pkcols ,"Column",XDT_String|XDT_Type,fkey.m_pkColumnName);
  }
}

void 
CXTable::SaveIndices(SOAPMessage& p_msg)
{
  XMLElement* index   = nullptr;
  XMLElement* columns = nullptr;
  XMLElement* elem    = p_msg.SetParameter("Indices","");

  for(auto& ind : m_indices)
  {
    if(ind.m_position == 1)
    {
      index = p_msg.AddElement(elem,"Index",XDT_String,"");
      p_msg.AddElement(index,"Name",     XDT_String |XDT_Type,ind.m_indexName);
      p_msg.AddElement(index,"Unique",   XDT_Boolean|XDT_Type,ind.m_unique);
      p_msg.AddElement(index,"Ascending",XDT_Boolean|XDT_Type,ind.m_ascending == "A");
      p_msg.AddElement(index,"Filter",   XDT_String |XDT_Type,ind.m_filter);

      columns = p_msg.AddElement(index,"Columns",XDT_String,"");
    }
    // Add all columns
    p_msg.AddElement(columns,"Column",XDT_String|XDT_Type,ind.m_columnName);
  }
}

void 
CXTable::SavePrivileges(SOAPMessage& p_msg)
{
  XMLElement* privs = p_msg.SetParameter("Privileges","");

  for(auto& priv : m_privileges)
  {
    XMLElement* pri = p_msg.AddElement(privs,"Privilege",XDT_String,"");

    p_msg.AddElement(pri,"Grantor",  XDT_String |XDT_Type,priv.m_grantor);
    p_msg.AddElement(pri,"Grantee",  XDT_String |XDT_Type,priv.m_grantee);
    p_msg.AddElement(pri,"Right",    XDT_String |XDT_Type,priv.m_privilege);
    p_msg.AddElement(pri,"Grantable",XDT_Boolean|XDT_Type,priv.m_grantable);
  }
}

void 
CXTable::LoadTableInfo(SOAPMessage& p_msg)
{
  XMLElement* elem = p_msg.FindElement("TableInfo");

  if(elem)
  {
    m_table.m_schema      = p_msg.GetElement(elem,"Schema");
    m_table.m_table       = p_msg.GetElement(elem, "Table");
    m_table.m_objectType  = p_msg.GetElement(elem, "ObjectType");
    m_table.m_remarks     = p_msg.GetElement(elem, "Remarks");
    m_table.m_fullName    = p_msg.GetElement(elem, "FullName");
    m_table.m_tablespace  = p_msg.GetElement(elem, "Tablespace");
    m_table.m_temporary   = p_msg.GetElementBoolean(elem, "Temporary");
  }
}

void 
CXTable::LoadColumnInfo(SOAPMessage& p_msg)
{
  XMLElement* columns = p_msg.FindElement("Columns");
  XMLElement* column  = p_msg.FindElement(columns,"Column",false);

  while(column)
  {
    MetaColumn col;

    col.m_position      = p_msg.GetElementInteger(column,"Position");
    col.m_column        = p_msg.GetElement       (column,"ColumnName");
    col.m_datatype      = p_msg.GetElementInteger(column,"ODBC_Type");
    col.m_typename      = p_msg.GetElement       (column,"RDBMS_Type");
    col.m_columnSize    = p_msg.GetElementInteger(column,"ColumnSize");
    col.m_bufferLength  = p_msg.GetElementInteger(column,"BufferLength");
    col.m_decimalDigits = p_msg.GetElementInteger(column,"DecimalDigits");
    col.m_numRadix      = p_msg.GetElementInteger(column,"Radix");
    col.m_nullable      = p_msg.GetElementInteger(column,"Nullable");
    col.m_remarks       = p_msg.GetElement       (column,"Remarks");
    col.m_default       = p_msg.GetElement       (column, "DefaultValue");
    col.m_datatype3     = p_msg.GetElementInteger(column,"ODCB_Type3");
    col.m_sub_datatype  = p_msg.GetElementInteger(column,"SubType");
    col.m_octet_length  = p_msg.GetElementInteger(column,"OctetLength");

    // Keep the column info
    m_columns.push_back(col);

    // Find next column
    column = p_msg.GetElementSibling(column);
  }
}

void 
CXTable::LoadPrimaryKey(SOAPMessage& p_msg)
{
  XMLElement*  elem = p_msg.FindElement("PrimaryKey");
  MetaPrimary* fkey = nullptr;

  bool first = true;
  if(elem)
  {
    MetaPrimary prim;

    if(first)
    {
      prim.m_constraintName    = p_msg.GetElement       (elem,"ConstraintName");
      prim.m_deferrable        = p_msg.GetElementInteger(elem,"Deferrable");
      prim.m_initiallyDeferred = p_msg.GetElementInteger(elem,"InitiallyDeferred");
    }
    else
    {
      prim.m_constraintName    = fkey->m_constraintName;
      prim.m_deferrable        = fkey->m_deferrable;
      prim.m_initiallyDeferred = fkey->m_initiallyDeferred;
    }

    XMLElement* column = p_msg.FindElement(elem,"Column",false);
    while(column)
    {
      prim.m_columnPosition = p_msg.GetElementInteger(column, "Position");
      prim.m_columnName     = p_msg.GetElement(column,"Name");

      // Keep the primary key (column) info
      m_primary.push_back(prim);
      if(first)
      {
        fkey = &(m_primary.front());
      }
      first = false;

      // Find next column
      column = p_msg.GetElementSibling(column);
    }
  }
}

void 
CXTable::LoadForeignKey(SOAPMessage& p_msg)
{
  XMLElement* keys = p_msg.FindElement("ForeignKeys");
  if(!keys) return;

  XMLElement* columns = nullptr;
  XMLElement* pkcols  = nullptr;

  CString lastkey;
  int sequence = 1;

  XMLElement* foreign = p_msg.FindElement(keys,"ForeignKey",false);
  while(foreign)
  {
    MetaForeign fkey;

    fkey.m_keySequence        = sequence;
    fkey.m_foreignConstraint  = p_msg.GetElement       (foreign,"Constraint");
    fkey.m_updateRule         = p_msg.GetElementInteger(foreign,"UpdateRule");
    fkey.m_deleteRule         = p_msg.GetElementInteger(foreign,"DeleteRule");
    fkey.m_deferrable         = p_msg.GetElementInteger(foreign,"Deferrable");
    fkey.m_match              = p_msg.GetElementInteger(foreign,"Match");
    fkey.m_initiallyDeferred  = p_msg.GetElementInteger(foreign,"InitiallyDeferred");

    columns = p_msg.FindElement(foreign,"Columns",false);
    columns = p_msg.FindElement(columns,"Column", false);

    pkcols  = p_msg.FindElement(foreign,"Primary",false);

    fkey.m_pkSchemaName = p_msg.GetElement(pkcols,"Schema");
    fkey.m_pkTableName  = p_msg.GetElement(pkcols,"Table");

    pkcols  = p_msg.FindElement(pkcols,"Columns",false);
    pkcols  = p_msg.FindElement(pkcols,"Column", false);

    fkey.m_fkColumnName = columns->GetValue();
    fkey.m_pkColumnName = pkcols ->GetValue();

    // Keep foreign key (column)
    m_foreigns.push_back(fkey);

    // Second and next columns
    columns = p_msg.GetElementSibling(columns);
    pkcols  = p_msg.GetElementSibling(pkcols);

    while(columns && pkcols)
    {
      MetaForeign key = fkey;

      fkey.m_keySequence  = ++sequence;
      fkey.m_fkColumnName = columns->GetValue();
      fkey.m_pkColumnName = pkcols ->GetValue();

      // Keep foreign key (column)
      m_foreigns.push_back(key);

      // Next column
      columns = p_msg.GetElementSibling(columns);
      pkcols  = p_msg.GetElementSibling(pkcols);
    }

    // Reset the sequence
    sequence = 1;
    // Next column in foreign key
    foreign = p_msg.GetElementSibling(foreign);
  }
}

void 
CXTable::LoadIndices(SOAPMessage& p_msg)
{
  XMLElement* indices = p_msg.FindElement("Indices");
  XMLElement* index   = p_msg.FindElement(indices,"Index");
  int position = 1;
  while(index)
  {
    MetaIndex ind;

    ind.m_indexName = p_msg.GetElement       (index,"Name");
    ind.m_unique    = p_msg.GetElementBoolean(index,"Unique");
    ind.m_ascending = p_msg.GetElementBoolean(index,"Ascending") ? "A" : "D";
    ind.m_filter    = p_msg.GetElement       (index,"Filter");
    ind.m_position  = position;

    XMLElement* columns = p_msg.FindElement(index,"Columns",false);
    XMLElement* column  = p_msg.FindElement(columns,"Column",false);
    ind.m_columnName = column->GetValue();

    m_indices.push_back(ind);
    column = p_msg.GetElementSibling(column);

    while(column)
    {
      MetaIndex ind2 = ind;

      ind2.m_position   = ++position;
      ind2.m_columnName = column->GetValue();

      m_indices.push_back(ind2);

      // Getting the next column
      column = p_msg.GetElementSibling(column);
    }

    // Reset the column position
    position = 1;
    // Next index
    index = p_msg.GetElementSibling(index);
  }
}

void 
CXTable::LoadPrivileges(SOAPMessage& p_msg)
{
  XMLElement* privs = p_msg.FindElement("Privileges");
  XMLElement* pri   = p_msg.FindElement(privs,"Privilege");

  while(pri)
  {
    MetaPrivilege priv;

    priv.m_grantor   = p_msg.GetElement(pri,"Grantor");
    priv.m_grantee   = p_msg.GetElement(pri,"Grantee");
    priv.m_privilege = p_msg.GetElement(pri,"Right");
    priv.m_grantable = p_msg.GetElementBoolean(pri,"Grantable");

    m_privileges.push_back(priv);

    // Next privilege
    pri = p_msg.GetElementSibling(pri);
  }

}