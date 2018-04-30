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

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// Create our new object factory
DEFINE_CXO_FACTORY(CXMaster);

// CTOR creating a new master record
CXMaster::CXMaster(CXTable* p_table)
         :CXObject(p_table)
{
}

// Bring the contents of the class to a SOAPMessage or a SQLRecord
void
CXMaster::Serialize(SOAPMessage& p_msg,XMLElement* p_entity,int p_mutation /*= 0*/)
{
}

// Read the contents of an object from a SOAPMessage or a SQLRecord
void
CXMaster::DeSerialize(SOAPMessage& p_msg,XMLElement* p_entity)
{
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
