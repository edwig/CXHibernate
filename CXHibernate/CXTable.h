////////////////////////////////////////////////////////////////////////
//
// File: CXTable.h
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
#pragma once
#include "CXHibernate.h"
#include "CXObject.h"
#include "CXSession.h"
#include <SQLMetaInfo.h>
#include <SQLInfoDB.h>
#include <SQLDataSet.h>
#include <vector>

using namespace SQLComponents;
class XMLMessage;
class CXSession;

class CXTable
{
public:
  CXTable(CString p_table);
 ~CXTable();

  // SETTERS

  // Setting a different schema only
  void      SetSchema(CString p_schema);
  // Setting the schema/table/type info
  void      SetSchemaTableType(CString p_schema,CString p_table,CString p_type);

  // GETTERS

  // Names of this object
  CString     GetSchemaName();
  CString     GetTableName();
  CString     GetSchemaTableName();
  CString     GetFullQualifiedTableName();
  CString     GetDMLTableName(SQLInfoDB* p_info);
  // Table types
  bool        GetIsTable();
  bool        GetIsView();
  bool        GetIsTemporary();
  bool        GetIsSynonym();

  // Getting the table info in an immutable way
  MetaTable&      GetTableInfo()   const;
  MColumnMap&     GetColumnInfo()  const;
  MPrimaryMap&    GetPrimaryInfo() const;
  MForeignMap&    GetForeignInfo() const;
  MIndicesMap&    GetIndicesInfo() const;
  MPrivilegeMap&  GetPrivilegesInfo() const;
  WordList        GetPrimaryKeyAsList();
  WordList        GetAttributesAsList();

  // SET INFO BY API
  void      SetInfoTable (MetaTable&     p_metaTable);
  void      AddInfoColumn(MetaColumn&    p_metaColumn);
  void      AddPrimaryKey(MetaPrimary&   p_metaPrimary);
  void      AddForeignKey(MetaForeign&   p_metaForeign);
  void      AddIndex     (MetaIndex&     p_metaIndex);
  void      AddSequence  (MetaSequence&  p_metaSequence);
  void      AddPrivilege (MetaPrivilege& p_metaPrivilege);

  // Get info from ODBC out of the database.
  bool      GetMetaInfoFromDatabase(SQLDatabase& p_database
                                   ,bool p_getForeigns   = false
                                   ,bool p_getIndices    = false
                                   ,bool p_getPrivileges = false);

  // Serialize the info of the table
  bool      SaveMetaInfo(CXSession* p_session,CString p_filename = _T(""));
  bool      LoadMetaInfo(CXSession* p_session,CString p_filename = _T(""));

private:
  // Getting table info from the ODBC database
  void GetTableInfo     (SQLInfoDB* p_info);
  void GetColumnInfo    (SQLInfoDB* p_info);
  void GetPrimaryKeyInfo(SQLInfoDB* p_info);
  void GetForeignKeyInfo(SQLInfoDB* p_info);
  void GetIndexInfo     (SQLInfoDB* p_info);
  void GetPrivilegeInfo (SQLInfoDB* p_info);

  void SaveTableInfo (SOAPMessage& p_msg);
  void SaveColumnInfo(SOAPMessage& p_msg);
  void SavePrimaryKey(SOAPMessage& p_msg);
  void SaveForeignKey(SOAPMessage& p_msg);
  void SaveIndices   (SOAPMessage& p_msg);
  void SavePrivileges(SOAPMessage& p_msg);

  void LoadTableInfo (SOAPMessage& p_msg);
  void LoadColumnInfo(SOAPMessage& p_msg);
  void LoadPrimaryKey(SOAPMessage& p_msg);
  void LoadForeignKey(SOAPMessage& p_msg);
  void LoadIndices   (SOAPMessage& p_msg);
  void LoadPrivileges(SOAPMessage& p_msg);

  // Meta info of this table
  MetaTable     m_table;
  MColumnMap    m_columns;
  MPrimaryMap   m_primary;
  MForeignMap   m_foreigns;
  MIndicesMap   m_indices;
  MetaSequence  m_sequence;
  MPrivilegeMap m_privileges;
};
