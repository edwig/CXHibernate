#pragma once
#include "CXHibernate.h"
#include "CXTable.h"
#include "CXObject.h"
#include <vector>

using SubClasses = std::vector<CXClass*>;

class CXClass
{
public:
  CXClass(CString p_name,CreateCXO p_create,CXClass* p_super = nullptr);
 ~CXClass();

  // The name of the game
  CString     GetName();
  // Object factory of this table
  CreateCXO   GetCreateCXO();
  // Getting our underlying database table
  CXTable*    GetTable();

protected:
  // Adding a sub-class
  void        AddSubClass(CXClass* p_subclass);


private:
  // The name of our class
  CString       m_name;
  // In case of a sub-class, this is our super-class
  CXClass*      m_super { nullptr };
  // In case of a super-class, these are our sub-classes
  SubClasses    m_subClasses;
  // Our CXObject factory function
  CreateCXO     m_create { nullptr };
  // Underlying database table
  CXTable*      m_table;
};
