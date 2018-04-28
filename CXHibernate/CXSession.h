////////////////////////////////////////////////////////////////////////
//
// File: CXSession.h
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
#include "CXTable.h"
#include "CXObject.h"
#include <SQLDatabase.h>
#include <SQLDataSet.h>
#include <map>

using namespace SQLComponents;

using TableMap   = std::map<CString,CXTable*>;
using TableCache = std::map<CString,CXObject*>;
using CXCache    = std::map<CString,TableCache*>;

class CXSession
{
public:
  CXSession();
  CXSession(CString p_database,CString p_user,CString p_password);
 ~CXSession();

  // Set master (database) or slave (Distant SOAP) role of the session
  void SetMaster(bool p_master);

  // Alternate database 
  void SetDatabase(SQLDatabase* p_database);

  // Add a table to the session
  bool AddTable(CXTable* p_table,CString p_name = "");

  // Finding a table
  CXTable*      FindTable(CString p_name);

  // GETTERS

  // Find if the database is correctly opened
  bool          GetDatabaseIsOpen();
 
  // QUERY INTERFACE
  CXObject*     SelectObject(CString p_tableName,SQLVariant*   p_primary,CreateCXO p_create);
  CXObject*     SelectObject(CString p_tableName,VariantSet&   p_primary,CreateCXO p_create);
  CXResultSet   SelectObject(CString p_tableName,SQLFilter*    p_filter ,CreateCXO p_create);
  CXResultSet   SelectObject(CString p_tableName,SQLFilterSet& p_filters,CreateCXO p_create);
  bool          UpdateObject(CXObject* p_object);
  bool          InsertObject(CXObject* p_object);
  bool          DeleteObject(CXObject* p_object);
  // Remove object from the result cache
  bool          RemoveObject(CXObject* p_object);

private:
  // Clear the table cache
  void          ClearCache(CString p_table = "");
  void          ClearTables(CString p_table = "");
  // Add an object to the cache
  bool          AddObjectInCache(CXObject* p_object,VariantSet& p_primary);
  // Create a filters set for a DataSet
  bool          CreateFilterSet(CXTable* p_table,VariantSet& p_primary,SQLFilterSet& p_filters);

  // Try to find an object in the cache
  CXObject*     FindObjectInCache   (CString p_tableName,VariantSet& p_primary);
  // Try to find an object in the database
  CXObject*     FindObjectInDatabase(CString p_table,VariantSet& p_primary,CreateCXO p_create);
  // Try to find an object via the SOAP interface
  CXObject*     FindObjectOnInternet(CString p_table,VariantSet& p_primary,CreateCXO p_create);

  void          SelectObjectsFromDatabase(CString p_table,SQLFilterSet& p_filters,CreateCXO p_create);
  void          SelectObjectsFromInternet(CString p_table,SQLFilterSet& p_filters,CreateCXO p_create);

  bool          m_master;      // Master/Slave role of the session
  bool          m_ownDatabase; // We own / destroy this database
  SQLDatabase*  m_database;    // Currently using database connection
  TableMap      m_tables;      // All table metadata definitions that we know of
  CXCache       m_cache;       // All cached objects of all known tables
  int           m_mutation;    // Mutation id
};
