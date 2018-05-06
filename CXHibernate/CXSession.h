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
#include <SQLMetaInfo.h>
#include <map>

class CXClass;
using namespace SQLComponents;

using ClassMap    = std::map<CString,CXClass*>;
using ObjectCache = std::map<CString,CXObject*>;
using CXCache     = std::map<CString,ObjectCache*>;

typedef enum _cxh_roles
{
  CXH_Database_role
 ,CXH_Filestore_role
 ,CXH_Internet_role
}
CXHRole;

class CXSession
{
public:
  // Construct as a internet slave
  CXSession();
  // Construct as a filestore handler
  CXSession(CString p_directory);
  // Construct as a database master handler
  CXSession(CString p_database,CString p_user,CString p_password);
  // DTOR
 ~CXSession();

  // Changing our role (database, filestore, internet)
  void          ChangeRole(CXHRole p_role);
  // Alternate database 
  void          SetDatabase(SQLDatabase* p_database);
  // Setting an alternate filestore location
  void          SetBaseDirectory(CString p_directory);
  // Add a class to the session
  bool          AddClass(CXClass* p_table);
  // Finding a class
  CXClass*      FindClass(CString p_name);
  // Find if the database is correctly opened
  bool          GetDatabaseIsOpen();
  // Get a master mutation ID, to put actions into one (1) commit
  int           GetMutationID(bool p_transaction = false);
  void          CommitMutation(int p_mutationID);
  void          RollbackMutation(int p_mutationID);

  // SOAP INTERFACE
  // Create a filestore name for a table
  CString       CreateFilestoreName(CXTable* p_table);
  // Create a filestore name for an object
  CString       CreateFilestoreName(CXTable* p_table, VariantSet& p_primary);
  // Save a SOAPMessage on the filesystem
  bool          SaveSOAPMessage(SOAPMessage& p_message, CString p_fileName);

  // QUERY INTERFACE
  CXObject*     SelectObject(CString p_className,SQLVariant*   p_primary);
  CXObject*     SelectObject(CString p_className,VariantSet&   p_primary);
  CXResultSet   SelectObject(CString p_className,SQLFilter*    p_filter);
  CXResultSet   SelectObject(CString p_className,SQLFilterSet& p_filters);
  bool          UpdateObject(CXObject* p_object,int p_mutationID = 0);
  bool          InsertObject(CXObject* p_object,int p_mutationID = 0);
  bool          DeleteObject(CXObject* p_object,int p_mutationID = 0);
  // Remove object from the result cache without any database/internet actions
  bool          RemoveObject(CXObject* p_object);
  // Complete cache synchronize with the database, saving all results
  bool          Synchronize();

private:
  // Getting meta-session info from our database
  void          GetMetaSessionInfo();
  // Clear the table cache
  void          ClearCache  (CString p_table = "");
  void          ClearClasses(CString p_table = "");
  // Add an object to the cache
  bool          AddObjectInCache(CXObject* p_object,VariantSet& p_primary);
  // And remove again from the cache
  bool          RemoveObjectFromCache(CXObject* p_object, VariantSet& p_primary);
  // Create a filters set for a DataSet
  bool          CreateFilterSet(CXTable* p_table,VariantSet& p_primary,SQLFilterSet& p_filters);

  // Try to find an object in the cache
  CXObject*     FindObjectInCache   (CString p_tableName,VariantSet& p_primary);
  // Try to find an object in the database
  CXObject*     FindObjectInDatabase (CString p_table,VariantSet& p_primary);
  // Try to find an object in the filestore
  CXObject*     FindObjectInFilestore(CString p_table,VariantSet& p_primary);
  // Try to find an object via the SOAP interface
  CXObject*     FindObjectOnInternet (CString p_table,VariantSet& p_primary);

  // SELECT objects
  void          SelectObjectsFromDatabase (CString p_table,SQLFilterSet& p_filters);
  void          SelectObjectsFromFilestore(CString p_table,SQLFilterSet& p_filters);
  void          SelectObjectsFromInternet (CString p_table,SQLFilterSet& p_filters);
  // DML operations in the database
  bool          UpdateObjectInDatabase (CXTable* p_table,CXObject* p_object,int p_mutationID = 0);
  bool          InsertObjectInDatabase (CXTable* p_table,CXObject* p_object,int p_mutationID = 0);
  bool          DeleteObjectInDatabase (CXTable* p_table,CXObject* p_object,int p_mutationID = 0);
  // DML operations in the filestore
  bool          UpdateObjectInFilestore(CXTable* p_table,CXObject* p_object,int p_mutationID = 0);
  bool          InsertObjectInFilestore(CXTable* p_table,CXObject* p_object,int p_mutationID = 0);
  bool          DeleteObjectInFilestore(CXTable* p_table,CXObject* p_object,int p_mutationID = 0);
  // DML operations on the internet
  bool          UpdateObjectInInternet (CXTable* p_table,CXObject* p_object,int p_mutationID = 0);
  bool          InsertObjectInInternet (CXTable* p_table,CXObject* p_object,int p_mutationID = 0);
  bool          DeleteObjectInInternet (CXTable* p_table,CXObject* p_object,int p_mutationID = 0);

  CXHRole       m_role { CXH_Database_role};   // Master/Slave role of the session
  bool          m_ownDatabase   { false   };   // We own / destroy this database
  CString       m_baseDirectory;               // Base directory for filestore role
  SQLDatabase*  m_database      { nullptr };   // Currently using database connection
  ClassMap      m_classes;                     // All classes definitions that we know of
  CXCache       m_cache;                       // All cached objects of all known tables
  int           m_mutation      { 0       };   // Mutation id
  MetaSession   m_metaInfo;                    // Database meta-session info
  SQLTransaction* m_transaction { nullptr };   // Enveloping transaction for all updates
  int             m_subtrans    { 0       };   // Sub-transaction
};
