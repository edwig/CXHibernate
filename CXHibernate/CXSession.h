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
// Last Revision:   27-05-2018
// Version number:  0.5.0
//
#pragma once
#include "CXTable.h"
#include "CXObject.h"
#include "CXRole.h"
#include "CXSessionUse.h"
#include <SQLDatabase.h>
#include <SQLDataSet.h>
#include <SQLMetaInfo.h>
#include <map>

class CXClass;
using namespace SQLComponents;

using ClassMap    = std::map<CString,CXClass*>;
using ObjectCache = std::map<CString,CXObject*>;
using CXCache     = std::map<CString,ObjectCache*>;

class CXSession
{
public:
  // Construct as a internet slave
  CXSession(CString p_sessionKey);
  // Construct as a filestore handler
  CXSession(CString p_sessionKey,CString p_directory);
  // Construct as a database master handler
  CXSession(CString p_sessionKey,CString p_database,CString p_user,CString p_password);
  // DTOR
 ~CXSession();

  // And closing it again
  void          CloseSession();
  // Changing our role (database, filestore, internet)
  void          ChangeRole(CXHRole p_role);
  // Specifiy a database connection
  void          SetDatabaseCatalog(CString p_datasource);
  void          SetDatabaseUsername(CString p_user);
  void          SetDatabasePassword(CString p_password);
  void          SetDatabaseConnection(CString p_datasource,CString p_user,CString p_password);
  // Alternate database 
  void          SetDatabase(SQLDatabase* p_database);
  // Getting our database
  SQLDatabase*  GetDatabase();
  // Setting an alternate filestore location
  void          SetFilestore(CString p_directory);
  // Add a class to the session
  bool          AddClass(CXClass* p_table);
  // Finding a class
  CXClass*      FindClass(CString p_name);
  // Find if the database is correctly opened
  bool          GetDatabaseIsOpen();
  // Loading the general configuration XML.
  void          LoadConfiguration(XMLMessage& p_config);
  // Saving the general configuration XML.
  void          SaveConfiguration(XMLMessage& p_config);

  // Get a master mutation ID, to put actions into one (1) commit
  int           StartTransaction();
  void          CommitTransaction();
  void          RollbackTransaction();
  bool          HasTransaction();

  // SOAP INTERFACE
  // Create a filestore name for a table
  CString       CreateFilestoreName(CXTable* p_table);
  // Create a filestore name for an object
  CString       CreateFilestoreName(CXTable* p_table, VariantSet& p_primary);
  // Save a SOAPMessage on the filesystem
  bool          SaveSOAPMessage(SOAPMessage& p_message, CString p_fileName);

  // OBJECT INTERFACE

  // Create an object prior to inserting it
  CXObject*     CreateObject(CString p_className);
  // Primary operations
  CXObject*     Load  (CString p_className,int           p_primary);
  CXObject*     Load  (CString p_className,SQLVariant*   p_primary);
  CXObject*     Load  (CString p_className,VariantSet&   p_primary);
  CXResultSet   Load  (CString p_className,SQLFilter*    p_filter);
  CXResultSet   Load  (CString p_className,SQLFilterSet& p_filters);
  bool          Save  (CXObject* p_object);
  bool          Update(CXObject* p_object);
  bool          Insert(CXObject* p_object);
  bool          Delete(CXObject* p_object);
  // Remove object from the result cache without any database/internet actions
  bool          RemoveObject(CXObject* p_object);
  // Complete cache synchronize with the database, saving all results
  bool          Synchronize();

private:
  // Getting meta-session info from our database
  void          GetMetaSessionInfo();
  // Create a new database and drop it again
  void          TryCreateDatabase();
  void          TryDropDatabase();
  // Clear the table cache
  void          ClearCache  (CString p_className = "");
  void          ClearClasses(CString p_className = "");
  // Add an object to the cache
  bool          AddObjectInCache(CXObject* p_object,VariantSet& p_primary);
  // And remove again from the cache
  bool          RemoveObjectFromCache(CXObject* p_object, VariantSet& p_primary);
  // Create a filters set for a DataSet
  bool          CreateFilterSet(CXTable* p_table,VariantSet& p_primary,SQLFilterSet& p_filters);

  // Try to find an object in the cache
  CXObject*     FindObjectInCache    (CString p_className,VariantSet& p_primary);
  // Try to find an object in the database
  CXObject*     FindObjectInDatabase (CString p_className,VariantSet& p_primary);
  // Try to find an object in the filestore
  CXObject*     FindObjectInFilestore(CString p_className,VariantSet& p_primary);
  // Try to find an object via the SOAP interface
  CXObject*     FindObjectOnInternet (CString p_className,VariantSet& p_primary);

  // SELECT objects
  void          SelectObjectsFromDatabase (CString p_className,SQLFilterSet& p_filters);
  void          SelectObjectsFromFilestore(CString p_className,SQLFilterSet& p_filters);
  void          SelectObjectsFromInternet (CString p_className,SQLFilterSet& p_filters);
  // DML operations in the database
  bool          UpdateObjectInDatabase (CXObject* p_object);
  bool          InsertObjectInDatabase (CXObject* p_object);
  bool          DeleteObjectInDatabase (CXObject* p_object);
  // DML operations in the filestore
  bool          UpdateObjectInFilestore(CXObject* p_object);
  bool          InsertObjectInFilestore(CXObject* p_object);
  bool          DeleteObjectInFilestore(CXObject* p_object);
  // DML operations on the internet
  bool          UpdateObjectInInternet (CXObject* p_object);
  bool          InsertObjectInInternet (CXObject* p_object);
  bool          DeleteObjectInInternet (CXObject* p_object);

  CString       m_sessionKey;                  // As known by CXHibernate
  CXHRole       m_role { CXH_Database_role};   // Master/Slave role of the session
  CXHSessionUse m_use;                         // How to use our database
  CString       m_baseDirectory;               // Base directory for filestore role
  bool          m_ownDatabase   { false   };   // We own / destroy this database
  SQLDatabase*  m_database      { nullptr };   // Currently using database connection
  CString       m_dbsCatalog;                  // Database to connect to
  CString       m_dbsUser;                     // Database user to connect
  CString       m_dbsPassword;                 // Password of our database user
  ClassMap      m_classes;                     // All classes definitions that we know of
  CXCache       m_cache;                       // All cached objects of all known tables
  int           m_mutation      { 0       };   // Mutation id
  MetaSession   m_metaInfo;                    // Database meta-session info
  SQLTransaction* m_transaction { nullptr };   // Enveloping transaction for all updates
  int             m_subtrans    { 0       };   // Sub-transaction
};
