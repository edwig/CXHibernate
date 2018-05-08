#pragma once

//////////////////////////////////////////////////////////////////////////
//
// OBJECT FACTORY MACRO's
// Define your own factory for a derived class of CXOBject and 
// create an object of your own subtype
//
//////////////////////////////////////////////////////////////////////////

class CXClass;
class CXObject;

// Prototype for use in method/function declarations
typedef CXObject* (CALLBACK *CreateCXO)(CXClass* p_class);

// Macro for calling a method with this factory as a parameter
#define CXO_FACTORY(classname) CreateCXObject##classname
// Declaring a factory in your derived class
#define DECLARE_CXO_FACTORY(classname) CXObject* CreateCXObject##classname(CXClass* p_class)
// Defining the factory in your class implementation
#define DEFINE_CXO_FACTORY(classname)  CXObject* CreateCXObject##classname(CXClass* p_class)\
                                       {\
                                         return new classname(p_class);\
                                       }

#define CXO_CONSTRUCTOR(classname)     classname::classname(CXClass* p_className)\
                                       :CXObject(p_className)\
                                       {}


#define   CXO_DBS_SERIALIZE(c_type,property,column,XDT) p_record.ModifyField(column,property,p_mutation)
#define CXO_DBS_DESERIALIZE(c_type,property,column,XDT) property = (c_type) p_record[ column ]
#define   CXO_XML_SERIALIZE(c_type,property,column,XDT) AddElement(p_message,p_entity,column,XDT | XDT_Type,property)
#define CXO_XML_DESERIALIZE(c_type,property,column,XDT) property = GetElement##c_type(p_message,p_entity,column)

#define BEGIN_DBS_SERIALIZE(Classname)   void Classname::Serialize(SQLRecord& p_record,int p_mutation)\
                                         {PreSerialize(p_record);
#define END_DBS_SERIALIZE                PostSerialize(p_record);}

#define BEGIN_DBS_DESERIALIZE(Classname) void Classname::DeSerialize(SQLRecord& p_record)\
                                         {PreDeSerialize(p_record);
#define END_DBS_DESERIALIZE              PostDeSerialize(p_record);}

#define BEGIN_XML_SERIALIZE(Classname)   void Classname::Serialize(SOAPMessage& p_message,XMLElement* p_entity)\
                                         {PreSerialize(p_message,p_entity);
#define END_XML_SERIALIZE                PostSerialize(p_message,p_entity);}
#define BEGIN_XML_DESERIALIZE(Classname) void Classname::DeSerialize(SOAPMessage& p_message,XMLElement* p_entity)\
                                         {PreDeSerialize(p_message,p_entity);
#define END_XML_DESERIALIZE              PostDeSerialize(p_message,p_entity);}


// Bring the contents of the class to a SOAPMessage or a SQLRecord and vice versa
#define DECLARE_CXO_SERIALIZATION virtual void Serialize(SOAPMessage& p_message,XMLElement* p_entity);  \
                                  virtual void Serialize(SQLRecord&   p_record,int p_mutation = 0);     \
                                  virtual void DeSerialize(SOAPMessage& p_message,XMLElement* p_entity);\
                                  virtual void DeSerialize(SQLRecord&   p_record);

//////////////////////////////////////////////////////////////////////////
//
// INLINE FUNCTIONS TO SERIALIZE A DATATYPE FROM/TO A SOAP Message
//
//////////////////////////////////////////////////////////////////////////

#include <SOAPMessage.h>

inline long GetElementlong(SOAPMessage& p_message, XMLElement* p_entity,CString p_column)
{
  return atoi(p_message.GetElement(p_entity, p_column));
}

inline CString GetElementCString(SOAPMessage& p_message, XMLElement* p_entity,CString p_column)
{
  return p_message.GetElement(p_entity, p_column);
}

inline bcd GetElementbcd(SOAPMessage& p_message, XMLElement* p_entity,CString p_column)
{
  bcd num = p_message.GetElement(p_entity, p_column);
  return num;
}


//////////////////////////////////////////////////////////////////////////
//
// ADDING TO A SOAP MESSAGE
//
//////////////////////////////////////////////////////////////////////////


inline XMLElement* AddElement(SOAPMessage& p_message,XMLElement* p_base,CString p_name,XmlDataType p_type,CString p_value)
{
  return p_message.AddElement(p_base,p_name,p_type,p_value);
}

inline XMLElement* AddElement(SOAPMessage& p_message,XMLElement* p_base,CString p_name,XmlDataType p_type,int p_value)
{
  CString value;
  value.Format("%d",p_value);
  return p_message.AddElement(p_base,p_name,p_type,value);
}

inline XMLElement* AddElement(SOAPMessage& p_message,XMLElement* p_base,CString p_name,XmlDataType p_type,bcd p_value)
{
  CString value = p_value.AsString();
  return p_message.AddElement(p_base,p_name,p_type,value);
}

