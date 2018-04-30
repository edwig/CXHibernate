////////////////////////////////////////////////////////////////////////
//
// File: CXDetail.h
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
#include <CXObject.h>
#include <bcd.h>

class CXTable;

class CXDetail : public CXObject
{
public:
  // CTOR of an CXObject
  CXDetail(CXTable* p_table);

  // Bring the contents of the class to a SOAPMessage or a SQLRecord
  virtual void Serialize(SOAPMessage& p_message,XMLElement* p_entity,int p_mutation = 0);
  virtual void Serialize(SQLRecord&   p_record,                      int p_mutation = 0);

  // Read the contents of an object from a SOAPMessage or a SQLRecord
  virtual void DeSerialize(SOAPMessage& p_message,XMLElement* p_entity);
  virtual void DeSerialize(SQLRecord&   p_record);

  // GETTERS
  int     GetID()            { return m_id;          };
  int     GetMasterID()      { return m_mast_id;     };
  int     GetLine()          { return m_line;        };
  CString GetDescription()   { return m_description; };
  bcd     GetAmount()        { return m_amount;      };

protected:

private:
  // Database persistent attributes
  long    m_id      { 0 };
  long    m_mast_id { 0 };
  long    m_line    { 0 };
  CString m_description;
  bcd     m_amount;
  // Virtual attributes
};

DECLARE_CXO_FACTORY(CXDetail);
