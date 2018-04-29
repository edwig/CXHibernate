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
CXMaster::Serialize(SOAPMessage& p_msg, int p_mutation /*= 0*/)
{
}

// Read the contents of an object from a SOAPMessage or a SQLRecord
void
CXMaster::DeSerialize(SOAPMessage& p_msg)
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
