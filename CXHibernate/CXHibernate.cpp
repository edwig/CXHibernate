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
// Last Revision:   22-04-2018
// Version number:  0.0.1
//
#include "stdafx.h"
#include "CXHibernate.h"
#include "CXSession.h"
#include <WebConfig.h>

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
}

// DTOR
CXHibernate::~CXHibernate()
{
  CloseAllSessions();
}

// Create a new Hibernate session
CXSession*
CXHibernate::CreateSession()
{
  CXSession* session = new CXSession();
  m_sessions.push_back(session);

  return session;
}

// Adding a externally created session (but we now own it)
void
CXHibernate::AddSession(CXSession* p_session)
{
  MapSessions::iterator it = std::find(m_sessions.begin(),m_sessions.end(),p_session);
  if(it == m_sessions.end())
  {
    m_sessions.push_back(p_session);
  }
}

// Removing a session
void
CXHibernate::RemoveSession(CXSession* p_session)
{
  MapSessions::iterator it = std::find(m_sessions.begin(),m_sessions.end(),p_session);
  if(it != m_sessions.end())
  {
    m_sessions.erase(it);
  }
}

// Flushing all data to the database and closing all sessions
void
CXHibernate::CloseAllSessions()
{
  for(auto& sess : m_sessions)
  {
    sess->Synchronize();
    delete sess;
  }
  m_sessions.clear();
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

// Loading the general configuration XML
CXSession*
CXHibernate::LoadConfiguration(CString p_configFile /*= ""*/)
{
  // Getting our filename
  CString filename(p_configFile);
  if (filename.IsEmpty())
  {
    filename = WebConfig::GetExePath() + "configuration.cxh";
  }

  // Loading our config
  XMLMessage config;
  config.LoadFile(filename);
  if(config.GetInternalError() != XmlError::XE_NoError)
  {
    throw CString("Cannot load hibernate configuration: ") + filename;
  }

  // Loading general attributes
  m_default_catalog = config.GetElement("default_catalog");
  m_default_schema  = config.GetElement("default_schema");
  CString strategy  = config.GetElement("strategy");
  m_strategy = StringToMapStrategy(strategy);

  // Create a session and load from there
  CXSession* session = CreateSession();
  session->LoadConfiguration(config);

  return session;
}

bool
CXHibernate::SaveConfiguration(CXSession* p_session,CString p_configFile /*= ""*/)
{
  // Getting our filename
  CString filename(p_configFile);
  if (filename.IsEmpty())
  {
    filename = WebConfig::GetExePath() + "configuration.cxh";
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
  if (p_strategy.CompareNoCase("sub_tables") == 0) return Strategy_sub_tables;
  throw CString("Unknown mapping strategy: ") + p_strategy;
}

CString
CXHibernate::MapStrategyToString(MapStrategy p_strategy)
{
  switch (p_strategy)
  {
    case Strategy_standalone: return "standalone";
    case Strategy_one_table:  return "one_table";
    case Strategy_sub_tables: return "sub_tables";
  }
  return "standalone";
}
