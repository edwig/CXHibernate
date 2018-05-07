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

//////////////////////////////////////////////////////////////////////////
//
// PRIVATE
//
//////////////////////////////////////////////////////////////////////////

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
