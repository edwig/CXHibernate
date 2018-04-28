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
#include <vector>
#include <SQLDataSet.h>

using namespace SQLComponents;

class SOAPMessage;
class CXTable;

class CXObject
{
public:
  CXObject();
  CXObject(CXTable* p_table);
  virtual ~CXObject();

  // Factory to create a new object
  // virtual CXObject* CreateCXObject();

  // Bring the contents of the class to a SOAPMessage or a SQLRecord
  virtual void Serialize(SOAPMessage& p_msg,int p_mutation = 0) = 0;
  virtual void Serialize(SQLRecord&   p_rec,int p_mutation = 0) = 0;

  // Read the contents of an object from a SOAPMessage or a SQLRecord
  virtual void DeSerialize(SOAPMessage& p_msg) = 0;
  virtual void DeSerialize(SQLRecord&   p_rec) = 0;

  // Getting or setting the Primary key of the object
  void        SetPrimaryKey(SQLVariant* p_keyValue,int p_part);
  SQLVariant* GetPrimaryKey(int p_part);

  // Object status
  bool        IsTransient();
  bool        IsPersistent();

  // Getting the table of this object
  CXTable*    BelongsToTable();

  // Defined by the underlying database record
  SQLRecord*  GetDatabaseRecord();

  // Getting the complete primary key
  VariantSet& GetPrimaryKey();

protected:

  // Bring the contents of the class to a SOAPMessage or a SQLRecord
  virtual void PreSerialize (SOAPMessage& p_msg);
  virtual void PreSerialize (SQLRecord&   p_rec);
  virtual void PostSerialize(SOAPMessage& p_msg);
  virtual void PostSerialize(SQLRecord&   p_rec);

  // Read the contents of an object from a SOAPMessage or a SQLRecord
  virtual void PreDeSerialize (SOAPMessage& p_msg);
  virtual void PreDeSerialize (SQLRecord&   p_rec);
  virtual void PostDeSerialize(SOAPMessage& p_msg);
  virtual void PostDeSerialize(SQLRecord&   p_rec);

  // All derived classes **must** implement this factory
//   virtual CXObject* CreateNewObject(const SOAPMessage* p_msg) = 0;
//   virtual CXObject* CreateNewObject(const SQLRecord*   p_rec) = 0;

  // Table where this object belongs to
  CXTable*   m_table { nullptr };

  // Record of this object in the database
  SQLRecord* m_record { nullptr };

  // Primary key value of the object
  VariantSet m_primaryKey;

private:
  // Fill in the primary key of the object
  void FillPrimaryKey(SQLRecord& p_record);
};

//////////////////////////////////////////////////////////////////////////
//
// ResultSets come back from database select operations
//
//////////////////////////////////////////////////////////////////////////

using CXResultSet = std::vector<CXObject*>;

//////////////////////////////////////////////////////////////////////////
//
// Main class for an object factory
// Define your own factory for a derived class of CXOBject and 
// create an object of your own subtype
//
//////////////////////////////////////////////////////////////////////////

// Prototype for use in method/function declarations
typedef CXObject* (CALLBACK *CreateCXO)(CXTable* p_table);

// Macro for calling a method with this factory as a parameter
#define CXO_FACTORY(classname) CreateCXObject##classname
// Declaring a factory in your derived class
#define DECLARE_CXO_FACTORY(classname) CXObject* CreateCXObject##classname(CXTable* p_table)
// Defining the factory in your class implementation
#define DEFINE_CXO_FACTORY(classname) CXObject* CreateCXObject##classname(CXTable* p_table)\
                                      {\
                                        return new classname(p_table);\
                                      }
