////////////////////////////////////////////////////////////////////////
//
// File: CXClass.cpp
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
#include "CXClass.h"
#include <algorithm>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CXClass::CXClass(CString p_name,CXClass* p_super,CreateCXO p_create)
        :m_name(p_name)
        ,m_create(p_create)
        ,m_super(p_super)
{
  // Registering with our super-class
  if(m_super)
  {
    m_super->AddSubClass(this);
  }

  // Find our object factory
  if(p_create == nullptr)
  {
    m_create = hibernate.FindCreateCXO(p_name);
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

// Add an attribute to the class
void
CXClass::AddAttribute(CXAttribute p_attribute)
{
  CString name = p_attribute.GetName();
  name.MakeLower();
  if(FindAttribute(name))
  {
    throw CString("Duplicate attribute cannot be added to class: ") + name;
  }
  // Just to be sure
  p_attribute.SetClass(this);
  // Keep this attribute
  m_attributes.insert(std::make_pair(name,p_attribute));
}

// Find an attribute
CXAttribute* 
CXClass::FindAttribute(CString p_name)
{
  p_name.MakeLower();
  CXAttribMap::iterator it = m_attributes.find(p_name);
  if (it != m_attributes.end())
  {
    return &it->second;
  }
  // Recursively ask our super classes
  if(m_super)
  {
    return m_super->FindAttribute(p_name);
  }
  return nullptr;
}

// Find the generator (if any)
CXAttribute* 
CXClass::FindGenerator()
{
  // Walk the list of attributes
  for(auto& attrib : m_attributes)
  {
    if(attrib.second.GetIsGenerator())
    {
      return &attrib.second;
    }
  }
  // Recursively ask our super classes
  if(m_super)
  {
    return m_super->FindGenerator();
  }
  return nullptr;
}

// Serialize to a configuration XML file
bool
CXClass::SaveMetaInfo(XMLMessage& p_message,XMLElement* p_elem)
{
  XMLElement* theclass = p_message.AddElement(p_elem,"class",XDT_String,"");

  // Class attributes
  p_message.AddElement(theclass,"name",XDT_String,m_name);
  if(m_super)
  {
    p_message.AddElement(theclass,"super",XDT_String,m_super->GetName());
  }
  p_message.AddElement(theclass,"table",XDT_String,m_table->TableName());

  // Add subclass references
  if(!m_subClasses.empty())
  {
    XMLElement* subs = p_message.AddElement(theclass,"subclasses",XDT_String,"");
    for(auto& sub : m_subClasses)
    {
      p_message.AddElement(subs,"subclass",XDT_String,sub->GetName());
    }
  }

  // All attributes of the class
  XMLElement* attribs = p_message.AddElement(theclass,"attributes",XDT_String,"");
  for(auto& attr : m_attributes)
  {
    attr.second.SaveMetaInfo(p_message,attribs);
  }
  return true;
}

//DeSerialize from a XML configuration file
bool
CXClass::LoadMetaInfo(CXSession* p_session,XMLMessage& p_message,XMLElement* p_elem)
{
  XMLElement* name = p_message.FindElement(p_elem,"name");
  if(name)
  {
    // Load name
    m_name = name->GetValue();
    // Load superclass
    XMLElement* super = p_message.FindElement(p_elem,"super");
    if(super)
    {
      m_super = p_session->FindClass(super->GetValue());
    }
    // Load subclasses
    // ??

    // Load attributes
    XMLElement* attribs = p_message.FindElement(p_elem,"attributes");
    if(attribs)
    {
      XMLElement* attrib = p_message.FindElement(attribs,"attribute");
      while(attrib)
      {
        // Create the attribute
        CString name = p_message.GetAttribute(attrib,"name");
        CXAttribute attribute(name);

        // Load the attribute
        attribute.LoadMetaInfo(p_message,attrib);

        // Keep the attribute
        m_attributes.insert(std::make_pair(name,attribute));
        // Next attributes
        attrib = p_message.GetElementSibling(attrib);
      }
      return true;
    }
  }
  return false;
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
