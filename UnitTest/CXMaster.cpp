////////////////////////////////////////////////////////////////////////
//
// File: CXMaster.cpp
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
#include "CXMaster.h"
#include <CXTable.h>
#include <SQLRecord.h>
#include <SOAPMessage.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// Create our new object factory
DEFINE_CXO_FACTORY(CXMaster);

// CTOR creating a new master record
CXMaster::CXMaster(CXClass* p_className)
         :CXObject(p_className)
{
}

// Bring the contents of the class to a SOAPMessage or a SQLRecord
void
CXMaster::Serialize(SOAPMessage& p_message,XMLElement* p_entity)
{
  PreSerialize(p_message, p_entity);

  p_message.AddElement(p_entity, "id",          XDT_Integer | XDT_Type, m_id);
  p_message.AddElement(p_entity, "invoice",     XDT_Integer | XDT_Type, m_invoice);
  p_message.AddElement(p_entity, "description", XDT_String  | XDT_Type, m_description);
  p_message.AddElement(p_entity, "total",       XDT_Decimal | XDT_Type, m_total.AsDouble());

  PostSerialize(p_message, p_entity);
}

// Read the contents of an object from a SOAPMessage or a SQLRecord
void
CXMaster::DeSerialize(SOAPMessage& p_message,XMLElement* p_entity)
{
  PreDeSerialize(p_message,p_entity);

  m_id          = p_message.GetElementInteger(p_entity,"id");
  m_invoice     = p_message.GetElementInteger(p_entity,"invoice");
  m_description = p_message.GetElement       (p_entity,"description");
  m_total       = p_message.GetElementDouble (p_entity,"total");

  PostDeSerialize(p_message,p_entity);
}

void
CXMaster::Serialize(SQLRecord& p_record,int p_mutation /*= 0*/)
{
  PreSerialize(p_record);

  p_record.ModifyField("id",          m_id,         p_mutation);
  p_record.ModifyField("invoice",     m_invoice,    p_mutation);
  p_record.ModifyField("description", m_description,p_mutation);
  p_record.ModifyField("total",       m_total,      p_mutation);

  PostSerialize(p_record);
}

void
CXMaster::DeSerialize(SQLRecord& p_record)
{
  PreDeSerialize(p_record);

  m_id          = (long)    p_record["id"];
  m_invoice     = (long)    p_record["invoice"];
  m_description = (CString) p_record["description"];
  m_total       = (bcd)     p_record["total"];

  PostDeSerialize(p_record);
}

// Setters
void
CXMaster::SetTotal(bcd p_bcd)
{
  m_total = p_bcd;
}
