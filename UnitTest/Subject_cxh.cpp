// (De-)Serializing factories for class: Subject
// Generated by CX-Hibernate cfg2cpp tool
// 
#include "stdafx.h"
#include "Subject.h"
#include <SQLRecord.h>
#include <SOAPMessage.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BEGIN_XML_SERIALIZE(Subject,CXObject)
  CXO_XML_SERIALIZE(int         ,m_id              ,"id"              ,XDT_Integer);
  CXO_XML_SERIALIZE(CString     ,m_name            ,"name"            ,XDT_String);
  CXO_XML_SERIALIZE(CString     ,m_adresline1      ,"adresline1"      ,XDT_String);
  CXO_XML_SERIALIZE(CString     ,m_adresline2      ,"adresline2"      ,XDT_String);
  CXO_XML_SERIALIZE(int         ,m_account_id      ,"account_id"      ,XDT_Integer);
END_XML_SERIALIZE

BEGIN_XML_DESERIALIZE(Subject,CXObject)
  CXO_XML_DESERIALIZE(int         ,m_id              ,"id"              ,XDT_Integer);
  CXO_XML_DESERIALIZE(CString     ,m_name            ,"name"            ,XDT_String);
  CXO_XML_DESERIALIZE(CString     ,m_adresline1      ,"adresline1"      ,XDT_String);
  CXO_XML_DESERIALIZE(CString     ,m_adresline2      ,"adresline2"      ,XDT_String);
  CXO_XML_DESERIALIZE(int         ,m_account_id      ,"account_id"      ,XDT_Integer);
END_XML_DESERIALIZE

BEGIN_DBS_SERIALIZE(Subject,CXObject)
  CXO_DBS_SERIALIZE(int         ,m_id              ,"id"              ,XDT_Integer);
  CXO_DBS_SERIALIZE(CString     ,m_name            ,"name"            ,XDT_String);
  CXO_DBS_SERIALIZE(CString     ,m_adresline1      ,"adresline1"      ,XDT_String);
  CXO_DBS_SERIALIZE(CString     ,m_adresline2      ,"adresline2"      ,XDT_String);
  CXO_DBS_SERIALIZE(int         ,m_account_id      ,"account_id"      ,XDT_Integer);
END_DBS_SERIALIZE

BEGIN_DBS_DESERIALIZE(Subject,CXObject)
  CXO_DBS_DESERIALIZE(int         ,m_id              ,"id"              ,XDT_Integer);
  CXO_DBS_DESERIALIZE(CString     ,m_name            ,"name"            ,XDT_String);
  CXO_DBS_DESERIALIZE(CString     ,m_adresline1      ,"adresline1"      ,XDT_String);
  CXO_DBS_DESERIALIZE(CString     ,m_adresline2      ,"adresline2"      ,XDT_String);
  CXO_DBS_DESERIALIZE(int         ,m_account_id      ,"account_id"      ,XDT_Integer);
END_DBS_DESERIALIZE

// Static factory to create a new object if this class
DEFINE_CXO_FACTORY(Subject);
