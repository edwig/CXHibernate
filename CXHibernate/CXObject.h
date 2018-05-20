////////////////////////////////////////////////////////////////////////
//
// File: CXObject.h
//
// Copyright (c) 1998-2018 ir. W.E. Huisman
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
// Last Revision:   22-04-2018
// Version number:  0.0.1
//
#pragma once
#include "CXHibernate.h"
#include <vector>
#include <SQLDataSet.h>

using namespace SQLComponents;

class SOAPMessage;
class XMLElement;
class CXTable;
class CXClass;

class CXObject
{
public:
  CXObject();
  virtual ~CXObject();

  // Setting the class is a mandatory action
  void SetClass(CXClass* p_class);

  // Bring the contents of the class to a SOAPMessage or a SQLRecord
  virtual void Serialize(SOAPMessage& p_message,XMLElement* p_entity) = 0;
  virtual void Serialize(SQLRecord&   p_record, int p_mutation = 0) = 0;

  // Read the contents of an object from a SOAPMessage or a SQLRecord
  virtual void DeSerialize(SOAPMessage& p_message,XMLElement* p_entity) = 0;
  virtual void DeSerialize(SQLRecord&   p_record) = 0;

  // Overridable functionality in the framework
  virtual int  Compare(CXObject* p_other);
  virtual CString Hashcode();

  // Getting or setting the Primary key of the object
  void         SetPrimaryKey(SQLVariant* p_keyValue,int p_part);
  SQLVariant*  GetPrimaryKey(int p_part);
  void         ResetPrimaryKey();

  // Object status
  bool         IsTransient();
  bool         IsPersistent();
  void         MakeTransient();

  // Getting the class of this object
  CXClass*     GetClass();

  // Defined by the underlying database record
  SQLRecord*   GetDatabaseRecord();

  // Getting the complete primary key
  VariantSet&  GetPrimaryKey();

protected:

  // Bring the contents of the class to a SOAPMessage or a SQLRecord
  virtual void PreSerialize (SOAPMessage& p_msg,XMLElement* p_entity);
  virtual void PreSerialize (SQLRecord&   p_rec);
  virtual void PostSerialize(SOAPMessage& p_msg,XMLElement* p_entity);
  virtual void PostSerialize(SQLRecord&   p_rec);

  // Read the contents of an object from a SOAPMessage or a SQLRecord
  virtual void PreDeSerialize (SOAPMessage& p_msg,XMLElement* p_entity);
  virtual void PreDeSerialize (SQLRecord&   p_rec);
  virtual void PostDeSerialize(SOAPMessage& p_msg,XMLElement* p_entity);
  virtual void PostDeSerialize(SQLRecord&   p_rec);

  // Table where this object belongs to
  CXClass*   m_class  { nullptr };

  // Record of this object in the database
  SQLRecord* m_record { nullptr };

  // Primary key value of the object
  VariantSet m_primaryKey;

private:
  CString ClassName();
  // Fill in the primary key of the object
  void FillPrimaryKey(SOAPMessage& p_message, XMLElement* p_entity);
  void FillPrimaryKey(SQLRecord&   p_record);
};

//////////////////////////////////////////////////////////////////////////
//
// ResultSets come back from database select operations
//
//////////////////////////////////////////////////////////////////////////

using CXResultSet = std::vector<CXObject*>;

#include "CXObjectFactory.h"

