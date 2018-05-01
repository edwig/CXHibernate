////////////////////////////////////////////////////////////////////////
//
// File: CXTable.h
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
#pragma once
#include <SQLMetaInfo.h>
#include <SQLInfoDB.h>
#include <SQLDataSet.h>

using namespace SQLComponents;

class CXTable
{
public:
  CXTable();
  CXTable(CString p_schema,CString p_table);
 ~CXTable();

  // Master side must apply a SQLDataSet
  void      SetDataSet(SQLDataSet* p_dataset);

  // Names of this object
  CString   SchemaName();
  CString   TableName();
  CString   SchemaTableName();
  CString   FullQualifiedTableName();
  CString   DMLTableName(SQLInfoDB* p_info);

  // GETTERS

  // Table types
  bool        GetIsTable();
  bool        GetIsView();
  bool        GetIsTemporary();
  bool        GetIsSynonym();
  // Primary data set
  SQLDataSet* GetDataSet();

  // Getting the table info in an immutable way
  MetaTable&      GetTableInfo()   const;
  MColumnMap&     GetColumnInfo()  const;
  MPrimaryMap&    GetPrimaryInfo() const;
  MForeignMap&    GetForeignInfo() const;
  MIndicesMap&    GetIndicesInfo() const;
  MPrivilegeMap&  GetPrivilegesInfo() const;
  WordList        GetPrimaryKeyAsList();

  // SET INFO BY API
  void      SetInfoTable (MetaTable&     p_metaTable);
  void      AddInfoColumn(MetaColumn&    p_metaColumn);
  void      AddPrimaryKey(MetaPrimary&   p_metaPrimary);
  void      AddForeignKey(MetaForeign&   p_metaForeign);
  void      AddIndex     (MetaIndex&     p_metaIndex);
  void      AddPrivilege (MetaPrivilege& p_metaPrivilege);

  // Get info from ODBC out of the database.
  bool      GetMetaInfoFromDatabase(SQLDatabase& p_database
                                   ,bool p_getForeigns   = false
                                   ,bool p_getIndices    = false
                                   ,bool p_getPrivileges = false);
private:
  // Getting table info from the ODBC database
  void GetTableInfo     (SQLInfoDB* p_info);
  void GetColumnInfo    (SQLInfoDB* p_info);
  void GetPrimaryKeyInfo(SQLInfoDB* p_info);
  void GetForeignKeyInfo(SQLInfoDB* p_info);
  void GetIndexInfo     (SQLInfoDB* p_info);
  void GetPrivilegeInfo (SQLInfoDB* p_info);

  // Meta info of this table
  MetaTable     m_table;
  MColumnMap    m_columns;
  MPrimaryMap   m_primary;
  MForeignMap   m_foreigns;
  MIndicesMap   m_indices;
  MPrivilegeMap m_privileges;

  // Standard data set in a master connection
  SQLDataSet*   m_dataSet;
};
