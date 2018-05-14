////////////////////////////////////////////////////////////////////////
//
// File: CXClass.h
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
#include "CXHibernate.h"
#include "CXAttribute.h"
#include "CXTable.h"
#include "CXObject.h"
#include <vector>

using SubClasses = std::vector<CXClass*>;

class CXClass
{
public:
  CXClass(CString p_name,CXClass* p_super = nullptr,CreateCXO p_create = nullptr);
 ~CXClass();

  // The name of the game
  CString     GetName();
  // Object factory of this table
  CreateCXO   GetCreateCXO();
  // Getting our underlying database table
  CXTable*    GetTable();

  // Add an attribute to the class
  void        AddAttribute(CXAttribute* p_attribute);
  // Find an attribute
  CXAttribute* FindAttribute(CString p_name);
  // Find the generator (if any)
  CXAttribute* FindGenerator();

  // Serialize to a configuration XML file
  bool        SaveMetaInfo(XMLMessage& p_message,XMLElement* p_elem);
  // DeSerialize from a XML configuration file
  bool        LoadMetaInfo(CXSession* p_session,XMLMessage& p_message,XMLElement* p_elem);

  // Build default SELECT query
  bool        BuildDefaultSelectQuery(SQLInfoDB* p_info);

protected:
  // Adding a sub-class
  void        AddSubClass(CXClass* p_subclass);
  // Fill in our underlying table
  void        FillTableInfoFromClassInfo();

private:
  // The name of our class
  CString       m_name;
  // In case of a sub-class, this is our super-class
  CXClass*      m_super { nullptr };
  // In case of a super-class, these are our sub-classes
  SubClasses    m_subClasses;
  // Our CXObject factory function
  CreateCXO     m_create { nullptr };
  // Underlying database table (in current mapping strategy)
  CXTable*      m_table;
  // All attributes of this class
  CXAttribMap   m_attributes;
  CXPrimaryKey  m_primary;
};
