////////////////////////////////////////////////////////////////////////
//
// File: CXTransaction.h
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
#include "CXSession.h"


//////////////////////////////////////////////////////////////////////////
//
// Autopointer class to a SQLTransaction within a CXSession
// Assures a 'Rollback' when going unplanned off the stack
//
//////////////////////////////////////////////////////////////////////////

class CXTransaction
{
public:
  CXTransaction(CXSession* p_session) : m_session(p_session)
  {
    m_mutation = p_session->StartTransaction();
  };

  ~CXTransaction()
  {
    Rollback();
  }

  void Commit()
  {
    if(m_session)
    {
      m_session->CommitTransaction();
      m_session = nullptr;
    }
  }

  void Rollback()
  {
    if(m_session)
    {
      m_session->RollbackTransaction();
      m_session = nullptr;
    }
  }

  int  Mutation()
  {
    return m_mutation; 
  }

private:
  CXSession* m_session;
  int        m_mutation;
};
