// (De-)Serializing factories for class: Animal
// Generated by CX-Hibernate cfg2cpp tool
// 
#include "stdafx.h"
#include "Animal.h"
#include <SQLRecord.h>
#include <SOAPMessage.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BEGIN_XML_SERIALIZE(Animal,CXObject)
  CXO_XML_SERIALIZE(m_id              ,_T("id"));
  CXO_XML_SERIALIZE(m_animalName      ,_T("animalName"));
  CXO_XML_SERIALIZE(m_has_claws       ,_T("has_claws"));
  CXO_XML_SERIALIZE(m_has_hair        ,_T("has_hair"));
  CXO_XML_SERIALIZE(m_has_wings       ,_T("has_wings"));
  CXO_XML_SERIALIZE(m_numberOfLegs    ,_T("numberOfLegs"));
END_XML_SERIALIZE

BEGIN_XML_DESERIALIZE(Animal,CXObject)
  CXO_XML_DESERIALIZE(m_id            ,_T("id"));
  CXO_XML_DESERIALIZE(m_animalName    ,_T("animalName"));
  CXO_XML_DESERIALIZE(m_has_claws     ,_T("has_claws"));
  CXO_XML_DESERIALIZE(m_has_hair      ,_T("has_hair"));
  CXO_XML_DESERIALIZE(m_has_wings     ,_T("has_wings"));
  CXO_XML_DESERIALIZE(m_numberOfLegs  ,_T("numberOfLegs"));
END_XML_DESERIALIZE

BEGIN_DBS_SERIALIZE(Animal,CXObject)
  CXO_DBS_SERIALIZE(m_id              ,_T("id"));
  CXO_DBS_SERIALIZE(m_animalName      ,_T("animalName"));
  CXO_DBS_SERIALIZE(m_has_claws       ,_T("has_claws"));
  CXO_DBS_SERIALIZE(m_has_hair        ,_T("has_hair"));
  CXO_DBS_SERIALIZE(m_has_wings       ,_T("has_wings"));
  CXO_DBS_SERIALIZE(m_numberOfLegs    ,_T("numberOfLegs"));
END_DBS_SERIALIZE

BEGIN_DBS_DESERIALIZE(Animal,CXObject)
  CXO_DBS_DESERIALIZE(m_id            ,_T("id"));
  CXO_DBS_DESERIALIZE(m_animalName    ,_T("animalName"));
  CXO_DBS_DESERIALIZE(m_has_claws     ,_T("has_claws"));
  CXO_DBS_DESERIALIZE(m_has_hair      ,_T("has_hair"));
  CXO_DBS_DESERIALIZE(m_has_wings     ,_T("has_wings"));
  CXO_DBS_DESERIALIZE(m_numberOfLegs  ,_T("numberOfLegs"));
END_DBS_DESERIALIZE

BEGIN_DESERIALIZE_GENERATOR(Animal)
  CXO_DBS_DESERIALIZE(m_id,_T("id"));
END_DESERIALIZE_GENERATOR

// Static factory to create a new object if this class
DEFINE_CXO_FACTORY(Animal);

