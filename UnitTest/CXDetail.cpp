#include "stdafx.h"
#include "CXDetail.h"
#include <CXTable.h>
#include <SQLRecord.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

DEFINE_CXO_FACTORY(CXDetail);

CXDetail::CXDetail(CXTable* p_table)
         :CXObject(p_table)
{

}

// Bring the contents of the class to a SOAPMessage or a SQLRecord
void
CXDetail::Serialize(SOAPMessage& p_msg, int p_mutation /*= 0*/)
{
}

// Read the contents of an object from a SOAPMessage or a SQLRecord
void
CXDetail::DeSerialize(SOAPMessage& p_msg)
{
}

void
CXDetail::Serialize(SQLRecord& p_record,int p_mutation /*= 0*/)
{
  CXObject::PreSerialize(p_record);

  p_record.ModifyField("id",          m_id,         p_mutation);
  p_record.ModifyField("mast_id",     m_mast_id,    p_mutation);
  p_record.ModifyField("line",        m_line,       p_mutation);
  p_record.ModifyField("description", m_description,p_mutation);
  p_record.ModifyField("amount",      m_amount,     p_mutation);

  CXObject::PostSerialize(p_record);
}

void
CXDetail::DeSerialize(SQLRecord& p_record)
{
  CXObject::PreDeSerialize(p_record);

  m_id          = (long)    p_record["id"];
  m_mast_id     = (long)    p_record["mast_id"];
  m_line        = (long)    p_record["line"];
  m_description = (CString) p_record["description"];
  m_amount      = (bcd)     p_record["amount"];

  CXObject::PostDeSerialize(p_record);
}
