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
// Last Revision:   22-04-2018
// Version number:  0.0.1
//
#include "stdafx.h"
#include "CXObject.h"
#include "CXTable.h"
#include <SQLRecord.h>
#include <SOAPMessage.h>


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// XTOR: Standard empty object
CXObject::CXObject()
{
}

// XTOR: As a object from a table
CXObject::CXObject(CXTable* p_table)
         :m_table(p_table)
{
}

// DTOR: Destruct the object
CXObject::~CXObject()
{
  // Destroy the primary key value (if any)
  for(auto& key : m_primaryKey)
  {
    delete key;
  }
  m_primaryKey.clear();
}

// Getting or setting the Primary key of the object

// Set a part of the primary key. Silently enlarges the set of values
// Makes a **COPY** of the SQLVariant value
void
CXObject::SetPrimaryKey(SQLVariant* p_keyValue, int p_part)
{
  if(m_primaryKey.size() > p_part)
  {
    delete m_primaryKey[p_part];
  }
  else while(m_primaryKey.size() <= p_part)
  {
    m_primaryKey.push_back(nullptr);
  }
  m_primaryKey[p_part] = new SQLVariant(p_keyValue);
}

// Get a part of the primary key. Fails in silence!
SQLVariant* 
CXObject::GetPrimaryKey(int p_part)
{
  if(m_primaryKey.size() > p_part)
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

bool
CXObject::IsTransient()
{
  return m_primaryKey.empty();
}

bool
CXObject::IsPersistent()
{
  return !m_primaryKey.empty();
}

// Getting the table of this object
CXTable*
CXObject::BelongsToTable()
{
  return m_table;
}

// Defined by the underlying database record
SQLRecord*
CXObject::GetDatabaseRecord()
{
  return m_record;
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
  int index = 0;
  WordList keys = m_table->GetPrimaryKeyAsList();
  for(auto& key : keys)
  {
    key.MakeLower();
    SQLVariant* val = m_primaryKey[index++];
    XmlDataType typ = ODBCToXmlDataType(val->GetDataType());

    p_message.AddElement(p_entity,key,typ|XDT_Type,val->GetAsChar());
  }
}

void
CXObject::PreSerialize(SQLRecord& p_record)
{
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

// Todo when we create an object from a SQLRecord

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

//////////////////////////////////////////////////////////////////////////
//
// PRIVATE
//
//////////////////////////////////////////////////////////////////////////

// Fill in the primary key of the object
void 
CXObject::FillPrimaryKey(SQLRecord& p_record)
{
  if(!m_primaryKey.empty())
  {
    return;
  }

  // Walk the list of primary key columns
  WordList keys = m_table->GetPrimaryKeyAsList();
  for (auto& key : keys)
  {
    SQLVariant* var = new SQLVariant(p_record.GetField(key));
    m_primaryKey.push_back(var);
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
  WordList keys = m_table->GetPrimaryKeyAsList();
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
