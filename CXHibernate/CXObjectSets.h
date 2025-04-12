////////////////////////////////////////////////////////////////////////
//
// File: CXObjectSets.h
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
#include "CXObject.h"
#include "CXObjectFactory.h"
#include "CXSession.h"

//////////////////////////////////////////////////////////////////////////
//
// ResultSets come back from database select operations
//
//////////////////////////////////////////////////////////////////////////

// Result set that automatically removes the duplicate read objects
// from the class table SQLDataSet after use
class AutoCXResultSet
{
public:
  AutoCXResultSet(CXSession* p_session,CXResultSet p_set)
  {
    m_session = p_session;
    m_set = p_set;
  }
  ~AutoCXResultSet()
  {
    for(auto& object : m_set)
    {
      // ONLY REMOVE THE DUPLICATE RECORDS !!
      // (Do not call "RemoveOjbects")
      if(object->GetReadOnly())
      {
        m_session->RemoveObject(object);
      }
    }
  }

  CXResultSet& get()
  {
    return m_set;
  }

  CXSession* m_session;
  CXResultSet m_set;
};

//////////////////////////////////////////////////////////////////////////
//
// Auto class to reset the record in the object
//
class AutoObjectRecord
{
public:
  AutoObjectRecord(CXObject* p_object,SQLRecord* p_record)
  {
    m_object = p_object;
    m_record = p_object->TempReplaceRecord(p_record);
  }
  ~AutoObjectRecord()
  {
    m_object->TempReplaceRecord(m_record);
  }
private:
  CXObject* m_object;
  SQLRecord* m_record;
};
