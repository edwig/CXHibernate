////////////////////////////////////////////////////////////////////////
//
// File: CXHibernate.h
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
#pragma once
#include "CXObjectFactory.h"
#include "CXStaticInitialization.h"
#include <vector>
#include <map>

#define HIBERNATE_VERSION _T("1.4.1")
#define HIBERNATE_DATE    _T("10-04-2025")

#define CXH_LOG_NOTHING       0   // Logging is OFF
#define CXH_LOG_ERRORS        1   // Only error logging
#define CXH_LOG_ACTIONS       2   // Log standard actions
#define CXH_LOG_DEBUG         3   // Log as much as possible
#define CXH_LOG_SQL           6   // Log all SQL actions
#define CXH_LOG_MAX           6   // Maximum log level

// ORM Mapping strategy classes -> tables
typedef enum _map_strategy
{
   Strategy_standalone   // No sub/super classes. 1 table = 1 class
  ,Strategy_one_table    // 1 table for super    + all sub-classes
  ,Strategy_sub_table    // 1 table for super    + 1 table per sub-class
  ,Strategy_classtable   // 1 table for subclass + all its supers
}
MapStrategy;

#define HIBERNATE_CONFIG_FILE _T("hibernate.cfg.xml")

// Maximum number of classes that can be initialized
// by the object factories. De- or increase at your own leisure.
const unsigned int MAX_CLASSES = 1000;

class LogAnalysis;
class CXSession;
using MapSessions = std::map<CString,CXSession*>;
using MapCreate   = std::map<CString,CreateCXO>;

class CXHibernate
{
public:
  CXHibernate();
 ~CXHibernate();

  // FUNCTIONS

  // Log a message to the hibernate logfile
  void         Log(int p_level, bool p_format,LPCTSTR p_text, ...);
  // Loading and saving the general configuration XML
  CXSession*   LoadConfiguration(CString p_sessionKey, CString p_configFile = _T(""));
  bool         SaveConfiguration(CXSession* p_session, CString p_configFile = _T(""));

  // Getting a (possibly existing) session
  CXSession*   GetSession(CString p_sessionKey = _T(""));
  // Create a new Hibernate session
  CXSession*   CreateSession(CString p_sessionKey = _T(""),CString p_configFile = _T(""));
  // Adding a externally created session (but we now own it). You **MUST** supply a session key
  void         AddSession(CXSession* p_session, CString p_sessionKey);
  // Removing a session
  void         RemoveSession(CXSession* p_session);
  // Flushing all data to the database and closing all sessions
  void         CloseAllSessions();
  // Register a create function for a class name
  void         RegisterCreateCXO(CString p_name, CreateCXO p_create);
  // Find a create function for a class name
  CreateCXO    FindCreateCXO(CString p_name);

  // SETTERS

  // Setting a hibernate mapping strategy
  bool          SetStrategy(MapStrategy p_strategy);
  // Setting a default catalog for the execution time
  void          SetDefaultCatalog(CString p_catalog);
  // Setting a default database schema for the execution time
  void          SetDefaultSchema(CString p_schema);
  // Toolset: incomplete model
  void          SetIncomplete(bool p_incomplete);
  // Setting a default configuration file (can be called once!)
  void          SetConfiguration(CString p_configFile);
  // Setting a different logfile instead of our own
  void          SetLogfile(LogAnalysis* p_logfile);
  // Setting a different loglevel
  void          SetLogLevel(int p_level);

  // GETTERS

  MapStrategy   GetStrategy();
  CString       GetDefaultCatalog();
  CString       GetDefaultSchema();
  int           GetLogLevel();
  LogAnalysis*  GetLogAnalysis();
  CString       GetConfiguration();
  int           GetNewMutation();

private:
  // Translate strategy names
  MapStrategy   StringToMapStrategy(CString p_strategy);
  CString       MapStrategyToString(MapStrategy p_strategy);
  // Create an opaque session key
  CString       CreateSessionKey();
  // Start and stop our logging
  void          StartLogging(CString p_logfile, int p_level);
  void          CloseLogfile();

  // Relational-Object-Mapping strategy
  MapStrategy   m_strategy { Strategy_standalone };
  // Mapping to all open sessions
  MapSessions   m_sessions;
  // Mapping with all create object functions
  MapCreate     m_createCXO;
  // Configuration file
  CString       m_configFile;
  // Current session key
  int           m_key      { 1 };
  // Current transaction
  long          m_mutation { 0 };
  // Logging for Hibernate
  int           m_loglevel { CXH_LOG_NOTHING };
  LogAnalysis*  m_logfile  { nullptr };
  bool          m_logOwner { false };
  // DEFAULTS
  CString       m_default_catalog;      // Default catalog
  CString       m_default_schema;       // Default schema of all tables
  // Incomplete mode: Only for tools and partly running models
  bool          m_incomplete { false };
  // Multi-threaded session lock
  CRITICAL_SECTION m_lock;
};

// Singleton pointer to the library
extern CXHibernate hibernate;

// Selecting the right library to link with automatically
// So we do not need to worry about which library to use in the linker settings
#ifdef _UNICODE
#if defined _M_IX86
#define CXH_PLATFORM "Ux86"
#else
#define CXH_PLATFORM "Ux64"
#endif

#else // _UNICODE

#if defined _M_IX86
#define CXH_PLATFORM "x86"
#else
#define CXH_PLATFORM "x64"
#endif
#endif

#if defined _DEBUG
#define CXH_CONFIGURATION "D"
#else
#define CXH_CONFIGURATION "R"
#endif 

#ifndef CXH_COMPONENTS_NOAUTOLINK
#pragma comment(lib,"CXHibernate_"                        CXH_PLATFORM CXH_CONFIGURATION ".lib")
#pragma message("Automatically linking with CXHibernate_" CXH_PLATFORM CXH_CONFIGURATION ".lib")
#endif 
