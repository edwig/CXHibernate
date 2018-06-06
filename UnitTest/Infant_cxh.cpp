// (De-)Serializing factories for class: Infant
// Generated by CX-Hibernate cfg2cpp tool
// 
#include "stdafx.h"
#include "Infant.h"
#include <SQLRecord.h>
#include <SOAPMessage.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BEGIN_XML_SERIALIZE(Infant,NaturalPerson)
  CXO_XML_SERIALIZE(CString     ,m_immunisation    ,"immunisation"    ,XDT_String);
  CXO_XML_SERIALIZE(CString     ,m_complication    ,"complication"    ,XDT_String);
  CXO_XML_SERIALIZE(CString     ,m_hospital        ,"hospital"        ,XDT_String);
END_XML_SERIALIZE

BEGIN_XML_DESERIALIZE(Infant,NaturalPerson)
  CXO_XML_DESERIALIZE(CString     ,m_immunisation    ,"immunisation"    ,XDT_String);
  CXO_XML_DESERIALIZE(CString     ,m_complication    ,"complication"    ,XDT_String);
  CXO_XML_DESERIALIZE(CString     ,m_hospital        ,"hospital"        ,XDT_String);
END_XML_DESERIALIZE

BEGIN_DBS_SERIALIZE(Infant,NaturalPerson)
  CXO_DBS_SERIALIZE(CString     ,m_immunisation    ,"immunisation"    ,XDT_String);
  CXO_DBS_SERIALIZE(CString     ,m_complication    ,"complication"    ,XDT_String);
  CXO_DBS_SERIALIZE(CString     ,m_hospital        ,"hospital"        ,XDT_String);
END_DBS_SERIALIZE

BEGIN_DBS_DESERIALIZE(Infant,NaturalPerson)
  CXO_DBS_DESERIALIZE(CString     ,m_immunisation    ,"immunisation"    ,XDT_String);
  CXO_DBS_DESERIALIZE(CString     ,m_complication    ,"complication"    ,XDT_String);
  CXO_DBS_DESERIALIZE(CString     ,m_hospital        ,"hospital"        ,XDT_String);
END_DBS_DESERIALIZE

// Static factory to create a new object if this class
DEFINE_CXO_FACTORY(Infant);
