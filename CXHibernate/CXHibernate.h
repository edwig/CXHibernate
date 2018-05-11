////////////////////////////////////////////////////////////////////////
//
// File: CXHibernate.h
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
#include "CXObjectFactory.h"
#include "CXStaticInitialization.h"
#include <vector>
#include <map>

// ORM Mapping strategy classes -> tables
typedef enum _map_strategy
{
   Strategy_standalone   // No sub/super classes. 1 table = 1 class
  ,Strategy_one_table    // 1 table for super + all sub-classes
  ,Strategy_sub_tables   // 1 table for super + 1 table per sub-class
//,Strategy_unions       // 1 table for subclass + all its supers
}
MapStrategy;

// Maximum number of classes that can be initialized
const unsigned int MAX_CLASSES = 1000;

class CXSession;
using MapSessions = std::vector<CXSession*>;
using MapCreate   = std::map<CString,CreateCXO>;

class CXHibernate
{
public:
  CXHibernate();
 ~CXHibernate();

  // FUNCTIONS

  // Loading and saving the general configuration XML
  CXSession*   LoadConfiguration(CString p_configFile = "");
  bool         SaveConfiguration(CXSession* p_session,CString p_configFile = "");
  // Create a new Hibernate session
  CXSession*   CreateSession();
  // Adding a externally created session (but we now own it)
  void         AddSession(CXSession* p_session);
  // Removing a session
  void         RemoveSession(CXSession* p_session);
  // Flushing all data to the database and closing all sessions
  void         CloseAllSessions();
  // Register a create function for a class name
  void         RegisterCreateCXO(CString p_name,CreateCXO p_create);
  // Find a create function for a class name
  CreateCXO    FindCreateCXO(CString p_name);

  // SETTERS

  // Setting a hibernate mapping strategy
  bool          SetStrategy(MapStrategy p_strategy);
  // Setting a default catalog for the execution time
  void          SetDefaultCatalog(CString p_catalog);
  // Setting a default database schema for the execution time
  void          SetDefaultSchema (CString p_schema);

  // GETTERS

  MapStrategy   GetStrategy();
  CString       GetDefaultCatalog();
  CString       GetDefaultSchema();

private:
  // Translate strategy names
  MapStrategy   StringToMapStrategy(CString p_strategy);
  CString       MapStrategyToString(MapStrategy p_strategy);

  // Relational-Object-Mapping strategy
  MapStrategy   m_strategy { Strategy_standalone };
  // Mapping to all open sessions
  MapSessions   m_sessions;
  // Mapping met alle create object functions
  MapCreate     m_createCXO;

  // DEFAULTS
  CString       m_default_catalog;      // Default catalog
  CString       m_default_schema;       // Default schema of all tables
};

// Singleton pointer to the library
extern CXHibernate hibernate;