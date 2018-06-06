////////////////////////////////////////////////////////////////////////
//
// File: CXObject.cpp
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
// Last Revision:   27-05-2018
// Version number:  0.5.0
//
#include "stdafx.h"
#include "CXObject.h"
#include "CXTable.h"
#include "CXClass.h"
#include "CXPrimaryHash.h"
#include <SQLRecord.h>
#include <SOAPMessage.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// XTOR: As a object from a table
CXObject::CXObject()
{
}

// DTOR: Destruct the object
CXObject::~CXObject()
{
  ResetPrimaryKey();
}

// First mandatory action for an object
void
CXObject::SetClass(CXClass* p_class)
{
  // Can only be called once!
  // So an object cannot be changed from class.
  if(m_class == nullptr)
  {
    m_class = p_class;
  }
}

// Getting the class of this object
CXClass*
CXObject::GetClass()
{
  return m_class;
}

// See if the object has the same business key as the other
// Can also be used to sort the objects on
int
CXObject::Compare(CXObject* p_other)
{
  // See if it is the same object
  if(this == p_other)
  {
    return 0;
  }
  VariantSet::iterator pkey = m_primaryKey.begin();
  VariantSet::iterator okey = p_other->m_primaryKey.begin();
  while(pkey != m_primaryKey.end() && okey != p_other->m_primaryKey.end())
  {
    if(*pkey < *okey) return -1;
    if(*pkey > *okey) return  1;
    // Next key
    ++pkey;
    ++okey;
  }
  return 0;
}

// The standard hash-code for object caches
CString 
CXObject::Hashcode()
{
  return CXPrimaryHash(m_primaryKey);
}

// Getting or setting the Primary key of the object

// Set a part of the primary key. Silently enlarges the set of values
// Makes a **COPY** of the SQLVariant value
void
CXObject::SetPrimaryKey(SQLVariant* p_keyValue, int p_part)
{
  if((int)m_primaryKey.size() > p_part)
  {
    delete m_primaryKey[p_part];
  }
  else while((int)m_primaryKey.size() <= p_part)
  {
    m_primaryKey.push_back(nullptr);
  }
  m_primaryKey[p_part] = new SQLVariant(p_keyValue);
}

// Get a part of the primary key. Fails in silence!
SQLVariant* 
CXObject::GetPrimaryKey(int p_part)
{
  if((int)m_primaryKey.size() > p_part)
  {
    return m_primaryKey[p_part];
  }
  return nullptr;
}

// Getting the complete primary key
VariantSet& 
CXObject::GetPrimaryKey()
{
  return m_primaryKey;
}

// Destroy the primary key value (if any)
void
CXObject::ResetPrimaryKey()
{
  for (auto& key : m_primaryKey)
  {
    delete key;
  }
  m_primaryKey.clear();
}

bool
CXObject::IsTransient()
{
  // See if no primary key yet
  if(m_primaryKey.empty())
  {
    return true;
  }

  // Walk the chain of primary keys
  // To see if the primary is (even partially) filled
  for(auto& key : m_primaryKey)
  {
    if(!key->IsEmpty())
    {
      return false;
    }
  }
  // All keys are NULL or ZERO
  return true;
}

bool
CXObject::IsPersistent()
{
  // See if no primary key yet
  if(m_primaryKey.empty())
  {
    return false;
  }

  // Walk the chain of primary keys
  for(auto& key : m_primaryKey)
  {
    if(key->IsEmpty())
    {
      return false;
    }
  }
  // All keys filled (not NULL and not ZERO)
  return true;
}

// Defined by the underlying database record
SQLRecord*
CXObject::GetDatabaseRecord()
{
  return m_record;
}

void
CXObject::MakeTransient()
{
  // Forget the database record
  m_record = nullptr;
  // Forget the primary key
  ResetPrimaryKey();
}

//////////////////////////////////////////////////////////////////////////
//
// PROTECTED INTERFACE
//
//////////////////////////////////////////////////////////////////////////

// We can fill in the primary key here
void 
CXObject::PreSerialize(SOAPMessage& p_message,XMLElement* p_entity)
{
}

void
CXObject::PreSerialize(SQLRecord& p_record)
{
  FillPrimaryKey(p_record);
}

void
CXObject::PostSerialize(SOAPMessage& p_message,XMLElement* p_entity)
{
  FillPrimaryKey(p_message,p_entity);
}

void
CXObject::PostSerialize(SQLRecord& p_record)
{
  m_record = &p_record;
  FillPrimaryKey(p_record);
}

void
CXObject::PreDeSerialize(SOAPMessage& p_message,XMLElement* p_entity)
{
  FillPrimaryKey(p_message,p_entity);
}

void
CXObject::PostDeSerialize(SOAPMessage& p_msg,XMLElement* p_entity)
{
}

// Remember the record where we where created from
void
CXObject::PreDeSerialize(SQLRecord& p_record)
{
  m_record = &p_record;
  FillPrimaryKey(p_record);
}

// Getting the primary key info
void
CXObject::PostDeSerialize(SQLRecord& p_record)
{
}

// Does nothing here!!
void CXObject::Serialize  (SOAPMessage& p_message,XMLElement* p_entity) {}
void CXObject::Serialize  (SQLRecord&   p_record,int p_mutation) {}
void CXObject::DeSerialize(SOAPMessage& p_message,XMLElement* p_entity) {}
void CXObject::DeSerialize(SQLRecord&   p_record) {}

// Logging of this object (called by CXSession only!)
void 
CXObject::LogObject()
{
  // See if we can log this object
  if(m_class == nullptr || m_record == nullptr)
  {
    return;
  }

  if(m_class->GetSuperClass())
  {
    LogClassAttributes(m_class->GetSuperClass());
  }
  LogClassAttributes(m_class);

  SubClasses& subs = m_class->GetSubClasses();
  for(auto& cl : subs)
  {
    LogClassAttributes(cl);
  }
}

//////////////////////////////////////////////////////////////////////////
//
// PRIVATE
//
//////////////////////////////////////////////////////////////////////////

// Super class of object does NOT have a specific class name
CString
CXObject::ClassName()
{
  if(m_class)
  {
    return m_class->GetName();
  }
  return "<UNKNOWN>";
}

// Fill in the primary key of the object
void 
CXObject::FillPrimaryKey(SQLRecord& p_record)
{
  if(!m_primaryKey.empty())
  {
    return;
  }

  // Walk the list of primary key columns
  WordList keys = m_class->GetTable()->GetPrimaryKeyAsList();
  for (auto& key : keys)
  {
    SQLVariant* field = p_record.GetField(key);
    if(field)
    {
      SQLVariant* value = new SQLVariant(field);
      m_primaryKey.push_back(value);
    }
  }
}

void
CXObject::FillPrimaryKey(SOAPMessage& p_message, XMLElement* p_entity)
{
  // See if already filled in
  if(!m_primaryKey.empty())
  {
    return;
  }

  // Walk the list of primary key columns
  WordList keys = m_class->GetTable()->GetPrimaryKeyAsList();
  for(auto& key : keys)
  {
    key.MakeLower();
    XMLElement* keyelem = p_message.FindElement(p_entity,key,false);
    if(keyelem)
    {
      XMLAttribute* attrib = p_message.FindAttribute(keyelem,"type");
      if(attrib)
      {
        XmlDataType xmltype = StringToXmlDataType(attrib->m_value);
        int odbcType = XmlDataTypeToODBC(xmltype);

        if(odbcType)
        {
          SQLVariant* var = new SQLVariant();
          var->SetData(odbcType,keyelem->GetValue());
          m_primaryKey.push_back(var);
        }
      }
    }
  }
}

// Log all attributes of a class (if any)
void 
CXObject::LogClassAttributes(CXClass* p_class)
{
  hibernate.Log(CXH_LOG_DEBUG,true,"Attributes of class: %s",p_class->GetName());

  int index = 0;
  CXAttribute* attribute = p_class->FindAttribute(index);
  while (attribute)
  {
    var* value = m_record->GetField(attribute->GetName());
    if(value)
    {
      hibernate.Log(CXH_LOG_DEBUG,true,"%d: %-32s:%s",index,attribute->GetName(),value->GetAsChar());
    }
    // Next attribute
    attribute = p_class->FindAttribute(++index);
  }
}
