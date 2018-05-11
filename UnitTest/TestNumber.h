////////////////////////////////////////////////////////////////////////
//
// File: TestNumber.h
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

class TestNumber : public CXObject
{
public:
  // CTOR of an CXObject
  TestNumber(CXClass* p_className);

  // Serialization of our persistent objects
  DECLARE_CXO_SERIALIZATION

  // GETTERS
  long    GetID()     { return m_id;     };
  int     GetField1() { return m_field1; };
  double  GetField2() { return m_field2; };
  bcd     GetField3() { return m_field3; };

  // SETTERS
  void    SetID(long p_id)          { m_id     = p_id;    };
  void    SetField1(int p_field)    { m_field1 = p_field; };
  void    SetField2(double p_field) { m_field2 = p_field; };
  void    SetField3(bcd p_field)    { m_field3 = p_field; };

protected:

private:
  // Database persistent attributes
  long    m_id;
  int     m_field1;
  double  m_field2;
  bcd     m_field3;
  // Virtual attributes
};

// DECLARE_CXO_FACTORY(TestNumber);

