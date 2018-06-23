////////////////////////////////////////////////////////////////////////
//
// File: CXHibernate.cpp
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
// Last Revision:   17-06-2018
// Version number:  0.9.0
//
#include "stdafx.h"
#include "CXHibernate.h"
#include "CXSession.h"
#include "CXResourceFile.h"
#include <AutoCritical.h>
#include <WebConfig.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// Our one-and-only hibernate object
CXHibernate hibernate;

// Singleton pointer to the library
static CXHibernate* cxhibernate = nullptr;

// CTOR: Create a singleton CXHibernate object
CXHibernate::CXHibernate()
{
  if(cxhibernate)
  {
    ASSERT("You cannot define the CXHibernate object more than once in an application");
  }
  else
  {
    cxhibernate = this;
  }

  // Start our critical section lock
  InitializeCriticalSection(&m_lock);

  // Initialize application class factories
  CXOInitializeClasses();
}

// DTOR
CXHibernate::~CXHibernate()
{
  // Stop everything
  CloseAllSessions();
  CloseLogfile();
  CXODestroyClasses();

  // Remove our lock
  DeleteCriticalSection(&m_lock);
}

// Getting a (possibly existing) session
CXSession*
CXHibernate::GetSession(CString p_sessionKey /*= ""*/)
{
  // Lock the session mapping
  AutoCritSec lock(&m_lock);

  p_sessionKey.MakeLower();
  // Return a session with this key
  MapSessions::iterator it = m_sessions.find(p_sessionKey);
  if(it != m_sessions.end())
  {
    return it->second;
  }
  // Not found: create a new one
  return CreateSession(p_sessionKey);
}

// Create a new Hibernate session
CXSession*
CXHibernate::CreateSession(CString p_sessionKey /*=""*/,CString p_configFile /*= ""*/)
{
  CXSession* session = nullptr;

  // Init in the correct language
  InitSQLComponents(SQLComponents::LN_ENGLISH);

  // Try to load the default configuration (if any)
  if(m_sessions.empty())
  {
    session = LoadConfiguration(p_sessionKey,p_configFile);
  }

  // Make sure we have a session key
  if(p_sessionKey.IsEmpty())
  {
    p_sessionKey = CreateSessionKey();
  }
  else
  {
    p_sessionKey.MakeLower();
  }

  // If not loaded by the configuration, create an empty one
  if(!session)
  {
    session = new CXSession(p_sessionKey);
    m_sessions.insert(std::make_pair(p_sessionKey,session));
  }

  return session;
}

// Adding a externally created session (but we now own it)
void
CXHibernate::AddSession(CXSession* p_session,CString p_sessionKey)
{
  // Lock the session mapping
  AutoCritSec lock(&m_lock);

  p_sessionKey.MakeLower();
  MapSessions::iterator it = m_sessions.find(p_sessionKey);
  if(it == m_sessions.end())
  {
    m_sessions.insert(std::make_pair(p_sessionKey,p_session));
    return;
  }
  throw StdException("Double registration of session: " + p_sessionKey);
}

// Removing a session, deleting it
// After this action the p_session is an INVALID pointer!
void
CXHibernate::RemoveSession(CXSession* p_session)
{
  // Lock the session mapping
  AutoCritSec lock(&m_lock);

  // Find the session in the session map
  MapSessions::iterator it = m_sessions.begin();
  while(it != m_sessions.end())
  {
    if(it->second == p_session)
    {
      delete p_session;
      m_sessions.erase(it);
      return;
    }
  }
}

// Flushing all data to the database and closing all sessions
void
CXHibernate::CloseAllSessions()
{
  // Lock the session mapping
  AutoCritSec lock(&m_lock);

  while(!m_sessions.empty())
  {
    // Synchronize and remove session
    m_sessions.begin()->second->CloseSession();
  }
  m_sessions.clear();

  // Flushing our logfile
  if(m_logfile)
  {
    m_logfile->Reset();
    m_logfile->SetLogLevel(m_loglevel);
  }
}

// ORM Mapping strategy can only be changed when no running sessions
// are in order on the global level
bool
CXHibernate::SetStrategy(MapStrategy p_strategy)
{
  if(m_sessions.empty())
  {
    m_strategy = p_strategy;
    return true;
  }
  return false;
}

void
CXHibernate::SetDefaultCatalog(CString p_catalog)
{
  m_default_catalog = p_catalog;
}

void
CXHibernate::SetDefaultSchema(CString p_schema)
{
  m_default_schema = p_schema;
}

MapStrategy   
CXHibernate::GetStrategy()
{
  return m_strategy;
}

CString
CXHibernate::GetDefaultCatalog()
{
  return m_default_catalog;
}

CString
CXHibernate::GetDefaultSchema()
{
  return m_default_schema;
}

// Toolset: incomplete model
void
CXHibernate::SetIncomplete(bool p_incomplete)
{
  m_incomplete = p_incomplete;
}

// Setting a default configuration file (can be called once!)
void
CXHibernate::SetConfiguration(CString p_configFile)
{
  if(m_configFile.IsEmpty())
  {
    m_configFile = p_configFile;
  }
}

CString
CXHibernate::GetConfiguration()
{
  if(m_configFile.IsEmpty())
  {
    m_configFile = HIBERNATE_CONFIG_FILE;
  }
  return m_configFile;
}

// Register a create function for a class name
void
CXHibernate::RegisterCreateCXO(CString p_name,CreateCXO p_create)
{
  // Lock the factory mapping
  AutoCritSec lock(&m_lock);

  // Does not need to check on duplicates
  // Code could otherwise not compile !!!
  p_name.MakeLower();
  m_createCXO.insert(std::make_pair(p_name,p_create));
}

// Find a create function for a class name
CreateCXO
CXHibernate::FindCreateCXO(CString p_name)
{
  // Lock the factory mapping
  AutoCritSec lock(&m_lock);

  p_name.MakeLower();
  MapCreate::iterator it = m_createCXO.find(p_name);
  if(it != m_createCXO.end())
  {
    return it->second;
  }
  // If we are not a tool with an incomplete class system
  // we must check that we find a creation factory
  // otherwise the programmer must implement a new class!!
  if(!m_incomplete)
  {
    throw StdException("Class definition error: CreateCXO Function not found for class: " + p_name);
  }
  return nullptr;
}

// Log a message to the hibernate logfile
void
CXHibernate::Log(int p_level,bool p_format,const char* p_text,...)
{
  // See if we have a logfile and must log
  if((m_logfile == nullptr) || (p_level > m_loglevel))
  {
    return;
  }

  // Format our log text
  CString text;
  if(p_format)
  {
    va_list  varargs;
    va_start(varargs,p_text);
    text.AppendFormatV(p_text,varargs);
    va_end(varargs);
  }
  else
  {
    text = p_text;
  }
  // Stow it
  m_logfile->AnalysisLog("Hibernate",LogType::LOG_INFO,false,text);
}

// Getting the loglevel
int
CXHibernate::GetLogLevel()
{
  return m_loglevel;
}

LogAnalysis*
CXHibernate::GetLogAnalysis()
{
  return m_logfile;
}

// Loading the general configuration XML
CXSession*
CXHibernate::LoadConfiguration(CString p_sessionKey,CString p_configFile /*= ""*/)
{
  // Getting our filename
  CString filename(p_configFile);
  if(filename.IsEmpty())
  {
    filename = GetConfiguration();
  }
  if(filename.Find('\\') < 0)
  {
    filename = WebConfig::GetExePath() + filename;
  }

  // Loading our config
  XMLMessage config;
  if(config.LoadFile(filename) == false)
  {
    // If no override on the file system
    // try our resources of the compiled application
    CString configuration = CXHReadResourceData(IDR_HIBERNATE_CONFIG,"CFG");
    if(configuration.IsEmpty())
    {
      // No configuration loaded
      return nullptr;
    }
    config.ParseMessage(configuration);
  }

  // Ill-formed XML configuration file
  if(config.GetInternalError() != XmlError::XE_NoError)
  {
    throw StdException((int)config.GetInternalError(),config.GetInternalErrorString());
  }

  // Be sure we have a session key
  if(p_sessionKey.IsEmpty())
  {
    p_sessionKey = CreateSessionKey();
  }

  // Loading general attributes
  m_default_catalog = config.GetElement("default_catalog");
  m_default_schema  = config.GetElement("default_schema");
  CString strategy  = config.GetElement("strategy");
  m_strategy = StringToMapStrategy(strategy);

  // Logging from here
  CString logfile =      config.GetElement("logfile");
  int    loglevel = atoi(config.GetElement("loglevel"));
  StartLogging(logfile,loglevel);

  // Create a session and load from there
  CXSession* session = new CXSession(p_sessionKey);
  session->LoadConfiguration(config);

  // Add our session in the session map
  AddSession(session,p_sessionKey);
  return session;
}

bool
CXHibernate::SaveConfiguration(CXSession* p_session,CString p_configFile /*= ""*/)
{
  // Getting our filename
  CString filename(p_configFile);
  if (filename.IsEmpty())
  {
    filename = WebConfig::GetExePath() + HIBERNATE_CONFIG_FILE;
  }

  // Create an XMLMessage
  XMLMessage config;
  config.SetRootNodeName("hibernate");

  // Setting the general attributes
  config.SetElement("default_catalog",m_default_catalog);
  config.SetElement("default_schema", m_default_schema);
  config.SetElement("strategy", MapStrategyToString(m_strategy));

  // Saving our session
  p_session->SaveConfiguration(config);

  // And save it to file
  return config.SaveFile(filename);
}

// Returing an ever increasing transaction mutation number
int
CXHibernate::GetNewMutation()
{
  return _InterlockedIncrement(&m_mutation);
}

//////////////////////////////////////////////////////////////////////////
//
// PRIVATE
//
//////////////////////////////////////////////////////////////////////////

// Translate strategy names
MapStrategy
CXHibernate::StringToMapStrategy(CString p_strategy)
{
  if (p_strategy.CompareNoCase("standalone") == 0) return Strategy_standalone;
  if (p_strategy.CompareNoCase("one_table")  == 0) return Strategy_one_table;
  if (p_strategy.CompareNoCase("sub_table")  == 0) return Strategy_sub_table;
  if (p_strategy.CompareNoCase("classtable") == 0) return Strategy_classtable;
  throw StdException("Unknown mapping strategy: " + p_strategy);
}

CString
CXHibernate::MapStrategyToString(MapStrategy p_strategy)
{
  switch (p_strategy)
  {
    case Strategy_standalone: return "standalone";
    case Strategy_one_table:  return "one_table";
    case Strategy_sub_table:  return "sub_table";
    case Strategy_classtable: return "classtable";
  }
  return "standalone";
}

// Create an opaque CX-Session key
CString
CXHibernate::CreateSessionKey()
{
  // Beware: Session keys MUST be lowercase at all times
  CString key;
  key.Format("cxs%d",m_key++);
  return key;
}

// Start our logging
void
CXHibernate::StartLogging(CString p_logfile,int p_level)
{
  // See if already done
  if(m_logfile)
  {
    return;
  }

  // Check boundaries and store the loglevel
  if(p_level < CXH_LOG_NOTHING) p_level = CXH_LOG_NOTHING;
  if(p_level > CXH_LOG_MAX)     p_level = CXH_LOG_MAX;
  m_loglevel = p_level;

  // Try to create a logfile
  if(!p_logfile.IsEmpty() && p_level > CXH_LOG_NOTHING)
  {
    // Standard rotation logfile 
    // Default 1000 lines or 30 seconds flushing borderline
    m_logfile = new LogAnalysis("CXHibernate");
    m_logfile->SetLogFilename(p_logfile);
    m_logfile->SetLogLevel(p_level);
    m_logfile->SetLogRotation(true);
    m_logfile->AnalysisLog(__FUNCTION__,LogType::LOG_INFO,false,"CX-Hibernate started");
  }
}

// Close our logfile
void
CXHibernate::CloseLogfile()
{
  if(m_logfile)
  {
    m_logfile->Reset();

    delete m_logfile;
    m_logfile  = nullptr;
    m_loglevel = CXH_LOG_NOTHING;
  }
}
