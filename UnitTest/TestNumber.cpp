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
TestNumber::Serialize(SOAPMessage& p_msg,int p_mutation /*= 0*/)
{
}

// Read the contents of an object from a SOAPMessage or a SQLRecord
void
TestNumber::DeSerialize(SOAPMessage& p_msg)
{
}

void
TestNumber::Serialize(SQLRecord& p_record,int p_mutation /*= 0*/)
{
  CXObject::PreSerialize(p_record);

  p_record.ModifyField("id",     m_id,     p_mutation);
  p_record.ModifyField("field1", m_field1, p_mutation);
  p_record.ModifyField("field2", m_field2, p_mutation);
  p_record.ModifyField("field3", m_field3, p_mutation);

  CXObject::PostSerialize(p_record);
}

void
TestNumber::DeSerialize(SQLRecord& p_record)
{
  CXObject::PreDeSerialize(p_record);

  m_id     = (long)   p_record["id"];
  m_field1 = (int)    p_record["field1"];
  m_field2 = (double) p_record["field2"];
  m_field3 = (bcd)    p_record["field3"];

  CXObject::PostDeSerialize(p_record);
}

