#include "stdafx.h"
#include "CXClass.h"
#include <algorithm>

CXClass::CXClass(CString p_name,CreateCXO p_create,CXClass* p_super)
        :m_name(p_name)
        ,m_create(p_create)
        ,m_super(p_super)
{
  // Registering with our super-class
  if(m_super)
  {
    m_super->AddSubClass(this);
  }

  // Getting a database table
  if(m_super && hibernate.GetStrategy() == Strategy_one_table)
  {
    m_table = m_super->GetTable();
  }
  else
  {
    m_table = new CXTable(p_name);
  }
}

CXClass::~CXClass()
{
  if(m_table)
  {
    delete m_table;
    m_table = nullptr;
  }
}

// The name of the game
CString
CXClass::GetName()
{
  return m_name;
}

// Object factory of this class
CreateCXO
CXClass::GetCreateCXO()
{
  return m_create;
}

// Getting our underlying database table
CXTable*
CXClass::GetTable()
{
  return m_table;
}

//////////////////////////////////////////////////////////////////////////
//
// PROTECTED
//
//////////////////////////////////////////////////////////////////////////

// Adding a sub-class
void
CXClass::AddSubClass(CXClass* p_subclass)
{
  SubClasses::iterator it = std::find(m_subClasses.begin(),m_subClasses.end(),p_subclass);
  if(it == m_subClasses.end())
  {
    m_subClasses.push_back(p_subclass);
  }
}
