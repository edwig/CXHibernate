// CXHibernate.cpp : Defines the entry point for the console application.
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
