// CXHibernate.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "CXHibernate.h"

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

MapStrategy   
CXHibernate::GetStrategy()
{
  return m_strategy;
}
