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
  CXO_XML_SERIALIZE(int         ,m_id              ,"id"              ,XDT_Integer);
  CXO_XML_SERIALIZE(CString     ,m_animalName      ,"animalName"      ,XDT_String);
  CXO_XML_SERIALIZE(bool        ,m_has_claws       ,"has_claws"       ,XDT_Boolean);
  CXO_XML_SERIALIZE(bool        ,m_has_hair        ,"has_hair"        ,XDT_Boolean);
  CXO_XML_SERIALIZE(bool        ,m_has_wings       ,"has_wings"       ,XDT_Boolean);
  CXO_XML_SERIALIZE(int         ,m_numberOfLegs    ,"numberOfLegs"    ,XDT_Integer);
  CXO_XML_SERIALIZE(CString     ,m_discriminator   ,"discriminator"   ,XDT_String);
END_XML_SERIALIZE

BEGIN_XML_DESERIALIZE(Animal,CXObject)
  CXO_XML_DESERIALIZE(int         ,m_id              ,"id"              ,XDT_Integer);
  CXO_XML_DESERIALIZE(CString     ,m_animalName      ,"animalName"      ,XDT_String);
  CXO_XML_DESERIALIZE(bool        ,m_has_claws       ,"has_claws"       ,XDT_Boolean);
  CXO_XML_DESERIALIZE(bool        ,m_has_hair        ,"has_hair"        ,XDT_Boolean);
  CXO_XML_DESERIALIZE(bool        ,m_has_wings       ,"has_wings"       ,XDT_Boolean);
  CXO_XML_DESERIALIZE(int         ,m_numberOfLegs    ,"numberOfLegs"    ,XDT_Integer);
  CXO_XML_DESERIALIZE(CString     ,m_discriminator   ,"discriminator"   ,XDT_String);
END_XML_DESERIALIZE

BEGIN_DBS_SERIALIZE(Animal,CXObject)
  CXO_DBS_SERIALIZE(int         ,m_id              ,"id"              ,XDT_Integer);
  CXO_DBS_SERIALIZE(CString     ,m_animalName      ,"animalName"      ,XDT_String);
  CXO_DBS_SERIALIZE(bool        ,m_has_claws       ,"has_claws"       ,XDT_Boolean);
  CXO_DBS_SERIALIZE(bool        ,m_has_hair        ,"has_hair"        ,XDT_Boolean);
  CXO_DBS_SERIALIZE(bool        ,m_has_wings       ,"has_wings"       ,XDT_Boolean);
  CXO_DBS_SERIALIZE(int         ,m_numberOfLegs    ,"numberOfLegs"    ,XDT_Integer);
  CXO_DBS_SERIALIZE(CString     ,m_discriminator   ,"discriminator"   ,XDT_String);
END_DBS_SERIALIZE

BEGIN_DBS_DESERIALIZE(Animal,CXObject)
  CXO_DBS_DESERIALIZE(int         ,m_id              ,"id"              ,XDT_Integer);
  CXO_DBS_DESERIALIZE(CString     ,m_animalName      ,"animalName"      ,XDT_String);
  CXO_DBS_DESERIALIZE(bool        ,m_has_claws       ,"has_claws"       ,XDT_Boolean);
  CXO_DBS_DESERIALIZE(bool        ,m_has_hair        ,"has_hair"        ,XDT_Boolean);
  CXO_DBS_DESERIALIZE(bool        ,m_has_wings       ,"has_wings"       ,XDT_Boolean);
  CXO_DBS_DESERIALIZE(int         ,m_numberOfLegs    ,"numberOfLegs"    ,XDT_Integer);
  CXO_DBS_DESERIALIZE(CString     ,m_discriminator   ,"discriminator"   ,XDT_String);
END_DBS_DESERIALIZE

// Static factory to create a new object if this class
DEFINE_CXO_FACTORY(Animal);

