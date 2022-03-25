// (De-)Serializing factories for class: NaturalPerson
// Generated by CX-Hibernate cfg2cpp tool
// 
#include "stdafx.h"
#include "NaturalPerson.h"
#include <SQLRecord.h>
#include <SOAPMessage.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BEGIN_XML_SERIALIZE(NaturalPerson,Subject)
  CXO_XML_SERIALIZE(m_firstname       ,"firstname");
  CXO_XML_SERIALIZE(m_date_of_birth   ,"date_of_birth");
  CXO_XML_SERIALIZE(m_length          ,"length");
END_XML_SERIALIZE

BEGIN_XML_DESERIALIZE(NaturalPerson,Subject)
  CXO_XML_DESERIALIZE(m_firstname       ,"firstname");
  CXO_XML_DESERIALIZE(m_date_of_birth   ,"date_of_birth");
  CXO_XML_DESERIALIZE(m_length          ,"length");
END_XML_DESERIALIZE

BEGIN_DBS_SERIALIZE(NaturalPerson,Subject)
  CXO_DBS_SERIALIZE(m_firstname       ,"firstname");
  CXO_DBS_SERIALIZE(m_date_of_birth   ,"date_of_birth");
  CXO_DBS_SERIALIZE(m_length          ,"length");
END_DBS_SERIALIZE

BEGIN_DBS_DESERIALIZE(NaturalPerson,Subject)
  CXO_DBS_DESERIALIZE(m_firstname       ,"firstname");
  CXO_DBS_DESERIALIZE(m_date_of_birth   ,"date_of_birth");
  CXO_DBS_DESERIALIZE(m_length          ,"length");
END_DBS_DESERIALIZE

BEGIN_DESERIALIZE_GENERATOR(NaturalPerson)
  CXO_DBS_DESERIALIZE(m_id, "id");
END_DESERIALIZE_GENERATOR

// Static factory to create a new object if this class
DEFINE_CXO_FACTORY(NaturalPerson);

