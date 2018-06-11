////////////////////////////////////////////////////////////////////////
//
// File: SQLStatement.cpp
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
// Last Revision:   28-05-2018
// Version number:  1.5.0
//
#include "stdafx.h"
#include "SQLComponents.h"
#include "SQLStatement.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace SQLComponents
{

SQLStatement::SQLStatement()
              :m_number(0)
              ,m_minNumber(0)
              ,m_maxNumber(0)
              ,m_customer(false)
{
}

SQLStatement::SQLStatement(int p_number,char* p_statement,int p_minimum,int p_maximum)
              :m_number(p_number)
              ,m_statement(p_statement)
              ,m_minNumber(p_minimum)
              ,m_maxNumber(p_maximum)
              ,m_customer(false)
{
}

// DTOR
SQLStatement::~SQLStatement()
{
}

bool
SQLStatement::CheckRecords(int p_records)
{
  CString fout;
  if(m_minNumber > 0 && p_records < m_minNumber)
  {
    fout.Format("SQL %d has only %d record(s) (too few) minimum = %d",m_number,p_records,m_minNumber);
		return true;
  }
  if(m_maxNumber > 0 && p_records > m_maxNumber)
  {
    fout.Format("SQL %d has %d records(s) (too much) maximum = %i",m_number,p_records,m_maxNumber);
    return true;
  }
  return false;
}

void
SQLStatement::AddToStatement(CString& p_statement)
{
  m_statement += p_statement;
}

// End of namespace
}
