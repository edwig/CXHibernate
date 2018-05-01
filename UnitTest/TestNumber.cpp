////////////////////////////////////////////////////////////////////////
//
// File: TestNumber.cpp
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
#include "TestNumber.h"
#include <CXTable.h>
#include <SQLRecord.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// Create our new object factory
DEFINE_CXO_FACTORY(TestNumber);

// CTOR creating a new master record
TestNumber::TestNumber(CXTable* p_table)
           :CXObject(p_table)
{
}

// Bring the contents of the class to a SOAPMessage or a SQLRecord
void
TestNumber::Serialize(SOAPMessage& p_msg,XMLElement* p_entity)
{
}

// Read the contents of an object from a SOAPMessage or a SQLRecord
void
TestNumber::DeSerialize(SOAPMessage& p_message,XMLElement* p_entity)
{
}

void
TestNumber::Serialize(SQLRecord& p_record,int p_mutation /*= 0*/)
{
  PreSerialize(p_record);

  p_record.ModifyField("id",     m_id,     p_mutation);
  p_record.ModifyField("field1", m_field1, p_mutation);
  p_record.ModifyField("field2", m_field2, p_mutation);
  p_record.ModifyField("field3", m_field3, p_mutation);

  PostSerialize(p_record);
}

void
TestNumber::DeSerialize(SQLRecord& p_record)
{
  PreDeSerialize(p_record);

  m_id     = (long)   p_record["id"];
  m_field1 = (int)    p_record["field1"];
  m_field2 = (double) p_record["field2"];
  m_field3 = (bcd)    p_record["field3"];

  PostDeSerialize(p_record);
}

