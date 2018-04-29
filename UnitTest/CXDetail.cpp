#include "stdafx.h"
#include "CXDetail.h"
#include <CXTable.h>
#include <SQLRecord.h>
#include <SOAPMessage.h>

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
CXDetail::Serialize(SOAPMessage& p_message, int p_mutation /*= 0*/)
{
  PreSerialize(p_message);

  XMLElement* entity = p_message.FindElement("Entity");
  p_message.AddElement(entity,"id",         XDT_Integer,m_id);
  p_message.AddElement(entity,"mast_id",    XDT_Integer,m_mast_id);
  p_message.AddElement(entity,"line",       XDT_Integer,m_line);
  p_message.AddElement(entity,"description",XDT_String, m_description);
  p_message.AddElement(entity,"amount",     XDT_Decimal,m_amount.AsDouble());

  PostSerialize(p_message);
}

// Read the contents of an object from a SOAPMessage or a SQLRecord
void
CXDetail::DeSerialize(SOAPMessage& p_message)
{
  PreDeSerialize(p_message);

  XMLElement* entity = p_message.FindElement("Entity");
  if(entity)
  {
    m_id          = p_message.GetElementInteger(entity,"id");
    m_mast_id     = p_message.GetElementInteger(entity,"mast_id");
    m_line        = p_message.GetElementInteger(entity,"line");
    m_description = p_message.GetElement       (entity,"description");
    m_amount      = p_message.GetElementDouble (entity,"amount");
  }
  PostDeSerialize(p_message);
}

void
CXDetail::Serialize(SQLRecord& p_record,int p_mutation /*= 0*/)
{
  PreSerialize(p_record);

  p_record.ModifyField("id",          m_id,         p_mutation);
  p_record.ModifyField("mast_id",     m_mast_id,    p_mutation);
  p_record.ModifyField("line",        m_line,       p_mutation);
  p_record.ModifyField("description", m_description,p_mutation);
  p_record.ModifyField("amount",      m_amount,     p_mutation);

  PostSerialize(p_record);
}

void
CXDetail::DeSerialize(SQLRecord& p_record)
{
  PreDeSerialize(p_record);

  m_id          = (long)    p_record["id"];
  m_mast_id     = (long)    p_record["mast_id"];
  m_line        = (long)    p_record["line"];
  m_description = (CString) p_record["description"];
  m_amount      = (bcd)     p_record["amount"];

  PostDeSerialize(p_record);
}
