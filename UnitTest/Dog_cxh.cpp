// (De-)Serializing factories for class: Dog
// Generated by CX-Hibernate cfg2cpp tool
// 
#include "stdafx.h"
#include "Dog.h"
#include <SQLRecord.h>
#include <SOAPMessage.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BEGIN_XML_SERIALIZE(Dog,Animal)
  CXO_XML_SERIALIZE(m_subrace    ,_T("subrace"));
  CXO_XML_SERIALIZE(m_walksPerDay,_T("walksPerDay"));
  CXO_XML_SERIALIZE(m_hunting    ,_T("hunting"));
  CXO_XML_SERIALIZE(m_waterdog   ,_T("waterdog"));
END_XML_SERIALIZE

BEGIN_XML_DESERIALIZE(Dog,Animal)
  CXO_XML_DESERIALIZE(m_subrace     ,_T("subrace"));
  CXO_XML_DESERIALIZE(m_walksPerDay ,_T("walksPerDay"));
  CXO_XML_DESERIALIZE(m_hunting     ,_T("hunting"));
  CXO_XML_DESERIALIZE(m_waterdog    ,_T("waterdog"));
END_XML_DESERIALIZE

BEGIN_DBS_SERIALIZE(Dog,Animal)
  CXO_DBS_SERIALIZE(m_subrace    ,_T("subrace"));
  CXO_DBS_SERIALIZE(m_walksPerDay,_T("walksPerDay"));
  CXO_DBS_SERIALIZE(m_hunting    ,_T("hunting"));
  CXO_DBS_SERIALIZE(m_waterdog   ,_T("waterdog"));
END_DBS_SERIALIZE

BEGIN_DBS_DESERIALIZE(Dog,Animal)
  CXO_DBS_DESERIALIZE(m_subrace    ,_T("subrace"));
  CXO_DBS_DESERIALIZE(m_walksPerDay,_T("walksPerDay"));
  CXO_DBS_DESERIALIZE(m_hunting    ,_T("hunting"));
  CXO_DBS_DESERIALIZE(m_waterdog   ,_T("waterdog"));
END_DBS_DESERIALIZE

BEGIN_DESERIALIZE_GENERATOR(Dog)
  CXO_DBS_DESERIALIZE(m_id, _T("id"));
END_DESERIALIZE_GENERATOR

// Static factory to create a new object if this class
DEFINE_CXO_FACTORY(Dog);

