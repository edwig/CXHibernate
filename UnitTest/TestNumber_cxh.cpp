////////////////////////////////////////////////////////////////////////
//
// File: TestNumber.cpp
//
// Copyright (c) 2015-2022 ir. W.E. Huisman
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
#include <SOAPMessage.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// Bring the contents of the class to a SOAPMessage or a SQLRecord
void
TestNumber::Serialize(SOAPMessage& p_message, XMLElement* p_entity)
{
  PreSerialize(p_message, p_entity);

  CXObject::Serialize(p_message,p_entity);

  CXObject::SetMsgElement(p_message,p_entity,_T("id"),    m_id);
  CXObject::SetMsgElement(p_message,p_entity,_T("field1"),m_field1);
  CXObject::SetMsgElement(p_message,p_entity,_T("field2"),m_field2);
  CXObject::SetMsgElement(p_message,p_entity,_T("field3"),m_field3);

  PostSerialize(p_message, p_entity);
}

// Read the contents of an object from a SOAPMessage or a SQLRecord
void
TestNumber::DeSerialize(SOAPMessage& p_message, XMLElement* p_entity)
{
  PreDeSerialize(p_message, p_entity);

  CXObject::DeSerialize(p_message,p_entity);

  CXObject::GetMsgElement(p_message,p_entity,_T("id"),m_id);
  CXObject::GetMsgElement(p_message,p_entity,_T("field1"),m_field1);
  CXObject::GetMsgElement(p_message,p_entity,_T("field2"),m_field2);
  CXObject::GetMsgElement(p_message,p_entity,_T("field3"),m_field3);

  PostDeSerialize(p_message, p_entity);
}

void
TestNumber::Serialize(SQLRecord& p_record, int p_mutation /*= 0*/)
{
  PreSerialize(p_record);

  CXObject::Serialize(p_record,p_mutation);

  p_record.ModifyField(_T("id"),     m_id,     p_mutation);
  p_record.ModifyField(_T("field1"), m_field1, p_mutation);
  p_record.ModifyField(_T("field2"), m_field2, p_mutation);
  p_record.ModifyField(_T("field3"), m_field3, p_mutation);

  PostSerialize(p_record);
}

void
TestNumber::DeSerialize(SQLRecord& p_record)
{
  PreDeSerialize(p_record);

  CXObject::DeSerialize(p_record);

  CXObject::GetRecordField(p_record,_T("id"),    m_id);
  CXObject::GetRecordField(p_record,_T("field1"),m_field1);
  CXObject::GetRecordField(p_record,_T("field2"),m_field2);
  CXObject::GetRecordField(p_record,_T("field3"),m_field3);

  PostDeSerialize(p_record);
}

void
TestNumber::DeSerializeGenerator(SQLRecord& p_record)
{
  // Re-Sync the primary key & the record
  PreDeSerialize(p_record);
  // Re-sync the generator column
  CXObject::GetRecordField(p_record,_T("id"),m_id);
}

// Create our new object factory
// DEFINE_CXO_FACTORY(TestNumber);

CXObject* CreateCXObjectTestNumber(void)
{
  return new TestNumber();
}

CString
TestNumber::ClassName()
{
  return _T("TestNumber");
}

#pragma  section(".ccxo$m",read)
#pragma init_seg(".ccxo$m",cxoexit)
class CXORegTestNumber
{
public:
  CXORegTestNumber()
  {
    hibernate.RegisterCreateCXO(_T("TestNumber"),CreateCXObjectTestNumber);
  }
  ~CXORegTestNumber() {};
};

CXORegTestNumber _registerDetail;


