////////////////////////////////////////////////////////////////////////
//
// File: CXAttribute.cpp
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
#include "CXAttribute.h"
#include "CXClass.h"

CXAttribute::CXAttribute(CString p_name)
            :m_name(p_name)
{
}

CXAttribute::CXAttribute(int p_datatype,CString p_name)
            :m_datatype(p_datatype)
            ,m_name(p_name)
{
}

CXAttribute::~CXAttribute()
{
}

// GETTERS

CXClass*
CXAttribute::GetClass()
{
  return m_class;
}

CString
CXAttribute::GetName()
{
  return m_name;
}

int
CXAttribute::GetDataType()
{
  return m_datatype;
}

int
CXAttribute::GetMaxLength()
{
  return m_maxlength;
}

CString
CXAttribute::GetDatabaseColumn()
{
  if(!m_dbs_column.IsEmpty())
  {
    return m_dbs_column;
  }
  return m_name;
}

bool
CXAttribute::GetIsGenerator()
{
  return m_generator;
}

bool
CXAttribute::GetIsPrimary()
{
  return m_isPrimary;
}

bool
CXAttribute::GetIsForeign()
{
  return m_isForeign;
}

bool
CXAttribute::GetHasNotNull()
{
  return m_notNull;
}

// SETTERS

void
CXAttribute::SetClass(CXClass* p_class)
{
  m_class = p_class;
}

void
CXAttribute::SetDataType(int p_datatype)
{
  m_datatype = p_datatype;
}

void
CXAttribute::SetDataType(CString p_datatype)
{
  m_datatype = CXStringToDatatType(p_datatype);
}

void
CXAttribute::SetMaxLength(int p_length)
{
  m_maxlength = p_length;
}

void
CXAttribute::SetGenerator(bool p_generator)
{
  m_generator = p_generator;
}

void
CXAttribute::SetIsPrimary(bool p_primary)
{
  m_isPrimary = p_primary;
}

void
CXAttribute::SetIsForeign(bool p_foreign)
{
  m_isForeign = p_foreign;
}

void
CXAttribute::SetDatabaseColumn(CString p_column)
{
  m_dbs_column = p_column;
}

void
CXAttribute::SetHasNotNull(bool p_notNull)
{
  m_notNull = p_notNull;
}


// Serialize to a configuration XML file
bool
CXAttribute::SaveMetaInfo(XMLMessage* p_message,XMLElement* p_elem)
{
  XMLElement* attrib = p_message->AddElement(p_elem,"attribute",XDT_String,"");
  CString datatype = CXDataTypeToString(m_datatype);

  p_message->SetAttribute(attrib,"name",    m_name);
  p_message->SetAttribute(attrib,"datatype",datatype);

  if(m_maxlength) p_message->SetAttribute(attrib,"maxlength",m_maxlength);
  if(m_generator) p_message->SetAttribute(attrib,"generator",m_generator);
  if(m_isPrimary) p_message->SetAttribute(attrib,"primary",  m_isPrimary);
  if(m_isForeign) p_message->SetAttribute(attrib,"foreign",  m_isForeign);
  if(m_notNull)   p_message->SetAttribute(attrib,"not-null", m_notNull);
  if(!m_dbs_column.IsEmpty())
  {
    p_message->SetAttribute(attrib,"dbs_column",m_dbs_column);
  }
  if(!m_default.IsEmpty())
  {
    p_message->SetAttribute(attrib,"default",m_default);
  }
  return true;
}

// DeSerialize from a XML configuration file
bool
CXAttribute::LoadMetaInfo(XMLMessage& p_message,XMLElement* p_elem)
{
  CString type = p_message.GetAttribute(p_elem,"datatype");
  m_datatype = CXStringToDatatType(type);

  m_maxlength  = p_message.GetAttributeInteger(p_elem,"maxlength");
  m_generator  = p_message.GetAttributeBoolean(p_elem,"generator");
  m_isPrimary  = p_message.GetAttributeBoolean(p_elem,"primary");
  m_isForeign  = p_message.GetAttributeBoolean(p_elem,"foreign");
  m_notNull    = p_message.GetAttributeBoolean(p_elem,"not-null");
  m_dbs_column = p_message.GetAttribute       (p_elem,"dbs_column");
  m_notNull    = p_message.GetAttribute       (p_elem,"default");

  return false;
}
