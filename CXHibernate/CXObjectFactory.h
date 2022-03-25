////////////////////////////////////////////////////////////////////////
//
// File: CXObjectFactory.h
//
// Copyright (c) 2015-2022 ir. W.E. Huisman
// All rights reserved
//
// Permission is hereby granted, free of charge, to any person obtaining a copy of 
// this software and associated documentation files (the "Software"), 
// to deal in the Software without restriction, including without limitation the rights 
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, 
// and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in all copies 
// or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, 
// INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. 
// IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, 
// WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION 
// WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//
// Last version date: See CXHibernate.h
// Version number:    See CXHibernate.h
//
#pragma once
#include "CXStaticInitialization.h"
#include <bcd.h>

//////////////////////////////////////////////////////////////////////////
//
// OBJECT FACTORY MACRO's
// Define your own factory for a derived class of CXOBject and 
// create an object of your own subtype
//
//////////////////////////////////////////////////////////////////////////

class CXObject;

// Prototype for use in method/function declarations
typedef CXObject* (*CreateCXO)(void);

// Defining the factory in your class implementation
#define DEFINE_CXO_FACTORY(classname)  CXObject* CreateCXObject##classname()\
                                       {\
                                         return new classname();\
                                       }\
CString classname::ClassName()\
{\
  return #classname;\
}\
__pragma(section (".ccxo$m",read))\
__pragma(init_seg(".ccxo$m",cxoexit))\
class CXOReg##classname\
{\
public:\
  CXOReg##classname()\
  {\
    hibernate.RegisterCreateCXO(#classname, CreateCXObject##classname);\
  }\
  ~CXOReg##classname() {};\
};\
CXOReg##classname _register##classname;

//////////////////////////////////////////////////////////////////////////
//
// OBJECT SERIALIZATION AND DE-SERIALIZATION
//
//////////////////////////////////////////////////////////////////////////

// Bring the contents of the class to a SOAPMessage or a SQLRecord and vice versa
#define DECLARE_CXO_SERIALIZATION virtual void   Serialize(SOAPMessage& p_message,XMLElement* p_entity);  \
                                  virtual void   Serialize(SQLRecord&   p_record,int p_mutation = 0);     \
                                  virtual void DeSerialize(SOAPMessage& p_message,XMLElement* p_entity);\
                                  virtual void DeSerialize(SQLRecord&   p_record);\
                                  virtual void DeSerializeGenerator(SQLRecord& p_record);\
                                  static  CString ClassName()


// Serialization and de-serialization of your class object
#define   CXO_DBS_SERIALIZE(property,column) p_record.ModifyField(column,property,p_mutation)
#define CXO_DBS_DESERIALIZE(property,column) CXObject::GetRecordField(p_record,column,property);
#define   CXO_XML_SERIALIZE(property,column) CXObject::SetMsgElement(p_message,p_entity,column,property);
#define CXO_XML_DESERIALIZE(property,column) CXObject::GetMsgElement(p_message,p_entity,column,property);

#define BEGIN_DBS_SERIALIZE(Classname,super)   void Classname::Serialize(SQLRecord& p_record,int p_mutation)\
                                               {PreSerialize(p_record);\
                                                super::Serialize(p_record,p_mutation);
#define END_DBS_SERIALIZE                       PostSerialize(p_record);}

#define BEGIN_DBS_DESERIALIZE(Classname,super) void Classname::DeSerialize(SQLRecord& p_record)\
                                               {PreDeSerialize(p_record);\
                                                super::DeSerialize(p_record);
#define END_DBS_DESERIALIZE                     PostDeSerialize(p_record);}

#define BEGIN_XML_SERIALIZE(Classname,super)   void Classname::Serialize(SOAPMessage& p_message,XMLElement* p_entity)\
                                               {PreSerialize(p_message,p_entity);\
                                                super::Serialize(p_message,p_entity);
#define END_XML_SERIALIZE                       PostSerialize(p_message,p_entity);}
#define BEGIN_XML_DESERIALIZE(Classname,super) void Classname::DeSerialize(SOAPMessage& p_message,XMLElement* p_entity)\
                                               {PreDeSerialize(p_message,p_entity);\
                                                super::DeSerialize(p_message,p_entity);
#define END_XML_DESERIALIZE                     PostDeSerialize(p_message,p_entity);}

#define BEGIN_DESERIALIZE_GENERATOR(Classname) void Classname::DeSerializeGenerator(SQLRecord& p_record)\
                                               {PreDeSerialize(p_record);
#define END_DESERIALIZE_GENERATOR              }


//////////////////////////////////////////////////////////////////////////
//
// INLINE FUNCTIONS TO SERIALIZE A DATATYPE FROM/TO A SOAP Message
//
//////////////////////////////////////////////////////////////////////////

#include "CXDataType.h"
#include <SOAPMessage.h>
#include <SQLVariant.h>
#include <SQLDate.h>
#include <SQLGuid.h>

using SQLComponents::SQLGuid;
using SQLComponents::SQLDate;
using SQLComponents::SQLTime; 
using SQLComponents::SQLTimestamp;
using SQLComponents::SQLInterval;
using SQLComponents::SQLVariant;

inline SQLVariant GetElementSQLVariant(SOAPMessage& p_message,XMLElement* p_entity,CString p_column)
{
  return SQLVariant(p_message.GetElement(p_entity,p_column));
}

inline int GetElementint(SOAPMessage& p_message,XMLElement* p_entity,CString p_column)
{
  return atoi(p_message.GetElement(p_entity,p_column));
}

inline long GetElementlong(SOAPMessage& p_message, XMLElement* p_entity,CString p_column)
{
  return atoi(p_message.GetElement(p_entity, p_column));
}

inline short GetElementshort(SOAPMessage& p_message,XMLElement* p_entity,CString p_column)
{
  return atoi(p_message.GetElement(p_entity,p_column));
}

inline ushort GetElementushort(SOAPMessage& p_message,XMLElement* p_entity,CString p_column)
{
  return atoi(p_message.GetElement(p_entity,p_column));
}

inline ulong GetElementulong(SOAPMessage& p_message,XMLElement* p_entity,CString p_column)
{
  return static_cast<unsigned long>(atoll(p_message.GetElement(p_entity,p_column)));
}

inline bigint GetElementbigint(SOAPMessage& p_message,XMLElement* p_entity,CString p_column)
{
  return _atoi64(p_message.GetElement(p_entity,p_column));
}

inline ubigint GetElementubigint(SOAPMessage& p_message,XMLElement* p_entity,CString p_column)
{
  return _atoi64(p_message.GetElement(p_entity,p_column));
}

inline float GetElementfloat(SOAPMessage& p_message,XMLElement* p_entity,CString p_column)
{
  return static_cast<float>(atof(p_message.GetElement(p_entity,p_column)));
}

inline double GetElementdouble(SOAPMessage& p_message,XMLElement* p_entity,CString p_column)
{
  return atof(p_message.GetElement(p_entity,p_column));
}

inline bool GetElementbool(SOAPMessage& p_message,XMLElement* p_entity,CString p_column)
{
  CString value(p_message.GetElement(p_entity,p_column));
  if(value.CompareNoCase("true")  == 0) return true;
  if(value.CompareNoCase("false") == 0) return false;
  // In case of '0' or '1'
  return atoi(value);
}

inline CString GetElementCString(SOAPMessage& p_message, XMLElement* p_entity,CString p_column)
{
  return p_message.GetElement(p_entity, p_column);
}

inline char GetElementchar(SOAPMessage& p_message,XMLElement* p_entity,CString p_column)
{
  CString value(p_message.GetElement(p_entity,p_column));
  if(value.GetLength())
  {
    return value.GetAt(0);
  }
  return 0;
}

inline uchar GetElementuchar(SOAPMessage& p_message,XMLElement* p_entity,CString p_column)
{
  CString value(p_message.GetElement(p_entity,p_column));
  if(value.GetLength())
  {
    return value.GetAt(0);
  }
  return 0;
}

inline bcd GetElementbcd(SOAPMessage& p_message, XMLElement* p_entity,CString p_column)
{
  bcd num(p_message.GetElement(p_entity, p_column));
  return num;
}

inline SQLDate GetElementSQLDate(SOAPMessage& p_message,XMLElement* p_entity,CString p_column)
{
  CString value(p_message.GetElement(p_entity,p_column));
  return SQLDate(value);
}

inline SQLTime GetElementSQLTime(SOAPMessage& p_message,XMLElement* p_entity,CString p_column)
{
  CString value(p_message.GetElement(p_entity,p_column));
  return SQLTime(value);
}

inline SQLTimestamp GetElementSQLTimestamp(SOAPMessage& p_message,XMLElement* p_entity,CString p_column)
{
  CString value(p_message.GetElement(p_entity,p_column));
  return SQLTimestamp(value);
}

inline SQLInterval GetElementSQLInterval(SOAPMessage& p_message,XMLElement* p_entity,CString p_column)
{
  CString value(p_message.GetElement(p_entity,p_column));
  return SQLInterval(value);
}

inline SQLGuid GetElementSQLGuid(SOAPMessage& p_message,XMLElement* p_entity,CString p_column)
{
  CString value(p_message.GetElement(p_entity,p_column));
  return SQLGuid(value);
}

//////////////////////////////////////////////////////////////////////////
//
// ADDING TO A SOAP MESSAGE
//
//////////////////////////////////////////////////////////////////////////

inline XMLElement* AddElement(SOAPMessage& p_message,XMLElement* p_base,CString p_name,XmlDataType p_type,SQLVariant& p_value)
{
  CString value;
  p_value.GetAsString(value);
  return p_message.AddElement(p_base,p_name,p_type,value);
}

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

inline XMLElement* AddElement(SOAPMessage& p_message,XMLElement* p_base,CString p_name,XmlDataType p_type,short p_value)
{
  CString value;
  value.Format("%d",p_value);
  return p_message.AddElement(p_base,p_name,p_type,value);
}

inline XMLElement* AddElement(SOAPMessage& p_message,XMLElement* p_base,CString p_name,XmlDataType p_type,unsigned short p_value)
{
  CString value;
  value.Format("%d",p_value);
  return p_message.AddElement(p_base,p_name,p_type,value);
}

inline XMLElement* AddElement(SOAPMessage& p_message,XMLElement* p_base,CString p_name,XmlDataType p_type,unsigned long p_value)
{
  CString value;
  value.Format("%ld",p_value);
  return p_message.AddElement(p_base,p_name,p_type,value);
}

inline XMLElement* AddElement(SOAPMessage& p_message,XMLElement* p_base,CString p_name,XmlDataType p_type,bigint p_value)
{
  CString value;
  value.Format("%ld",p_value);
  return p_message.AddElement(p_base,p_name,p_type,value);
}

inline XMLElement* AddElement(SOAPMessage& p_message,XMLElement* p_base,CString p_name,XmlDataType p_type,ubigint p_value)
{
  CString value;
  value.Format("%ld",p_value);
  return p_message.AddElement(p_base,p_name,p_type,value);
}

inline XMLElement* AddElement(SOAPMessage& p_message,XMLElement* p_base,CString p_name,XmlDataType p_type,float p_value)
{
  CString value;
  value.Format("%.8f",p_value);
  value = value.TrimRight('0');
  return p_message.AddElement(p_base,p_name,p_type,value);
}

inline XMLElement* AddElement(SOAPMessage& p_message,XMLElement* p_base,CString p_name,XmlDataType p_type,double p_value)
{
  return p_message.AddElement(p_base,p_name,p_type,p_value);
}

inline XMLElement* AddElement(SOAPMessage& p_message,XMLElement* p_base,CString p_name,XmlDataType p_type,bool p_value)
{
  return p_message.AddElement(p_base,p_name,p_type,p_value);
}

inline XMLElement* AddElement(SOAPMessage& p_message,XMLElement* p_base,CString p_name,XmlDataType p_type,char p_value)
{
  CString value(p_value);
  return p_message.AddElement(p_base,p_name,p_type,value);
}

inline XMLElement* AddElement(SOAPMessage& p_message,XMLElement* p_base,CString p_name,XmlDataType p_type,unsigned char p_value)
{
  CString value;
  value.SetAt(0,((unsigned short)p_value));
  return p_message.AddElement(p_base,p_name,p_type,value);
}

inline XMLElement* AddElement(SOAPMessage& p_message,XMLElement* p_base,CString p_name,XmlDataType p_type,bcd p_value)
{
  CString value(p_value.AsString());
  return p_message.AddElement(p_base,p_name,p_type,value);
}

inline XMLElement* AddElement(SOAPMessage& p_message,XMLElement* p_base,CString p_name,XmlDataType p_type,SQLDate& p_value)
{
  CString value(p_value.AsString());
  return p_message.AddElement(p_base,p_name,p_type,value);
}

inline XMLElement* AddElement(SOAPMessage& p_message,XMLElement* p_base,CString p_name,XmlDataType p_type,SQLTime& p_value)
{
  CString value(p_value.AsString());
  return p_message.AddElement(p_base,p_name,p_type,value);
}

inline XMLElement* AddElement(SOAPMessage& p_message,XMLElement* p_base,CString p_name,XmlDataType p_type,SQLTimestamp& p_value)
{
  CString value(p_value.AsXMLString());
  return p_message.AddElement(p_base,p_name,p_type,value);
}

inline XMLElement* AddElement(SOAPMessage& p_message,XMLElement* p_base,CString p_name,XmlDataType p_type,SQLInterval& p_value)
{
  CString value(p_value.AsXMLString());
  return p_message.AddElement(p_base,p_name,p_type,value);
}

inline XMLElement* AddElement(SOAPMessage& p_message,XMLElement* p_base,CString p_name,XmlDataType p_type,SQLGuid& p_value)
{
  CString value(p_value.AsString());
  return p_message.AddElement(p_base,p_name,p_type,value);
}
