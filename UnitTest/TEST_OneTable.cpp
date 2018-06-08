////////////////////////////////////////////////////////////////////////
//
// File: OneTable.cpp
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
#include "stdafx.h"
#include <CppUnitTest.h>
#include <CXSession.h>
#include <CXClass.h>
#include "Subject.h"
#include "NaturalPerson.h"
#include "Supplier.h"
#include "Infant.h"
#include <SQLComponents.h>
#include <SQLVariant.h>
#include <SQLQuery.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace SQLComponents;

namespace HibernateTest
{
  TEST_CLASS(OneTableStrategy)
  {
  public:
    ~OneTableStrategy()
    {
      hibernate.CloseAllSessions();
    }

    TEST_METHOD(T01_SelectSubject)
    {
      Logger::WriteMessage("T01_SelectSubject shows first subject in database");
      OpenSession();

      try
      {
        Subject* sub = (Subject*)m_session->Load(Subject::ClassName(),1);
        PrintSubject(sub);
      }
      catch(StdException* ex)
      {
        Logger::WriteMessage(ex->GetErrorMessage());
        ex->Delete();
      }
    }

    TEST_METHOD(T02_SelectNaturalPerson)
    {
      Logger::WriteMessage("T02_SelectNaturalPerson shows a real person from the database");
      OpenSession();

      try
      {
        NaturalPerson* person = (NaturalPerson*)m_session->Load(NaturalPerson::ClassName(),2);
        PrintNaturalPerson(person);
      }
      catch(StdException* ex)
      {
        Logger::WriteMessage(ex->GetErrorMessage());
        ex->Delete();
      }
    }

    TEST_METHOD(T03_SelectSupplier)
    {
      Logger::WriteMessage("T03_SelectSupplier shows a supplier from the database");
      OpenSession();

      try
      {
        Supplier* supp = (Supplier*)m_session->Load(Supplier::ClassName(),3);
        PrintSupplier(supp);
      }
      catch(StdException* ex)
      {
        Logger::WriteMessage(ex->GetErrorMessage());
        ex->Delete();
      }
    }
    
    TEST_METHOD(T04_SelectInfant)
    {
      Logger::WriteMessage("T04_SelectInfant shows a child from the database");
      OpenSession();

      try
      {
        Infant* infant = (Infant*)m_session->Load(Infant::ClassName(),4);
        PrintInfant(infant);
      }
      catch(StdException* ex)
      {
        Logger::WriteMessage(ex->GetErrorMessage());
        ex->Delete();
      }
    }

    void PrintSubject(Subject* p_subject)
    {
      CString text;
      text.Format("Subject with ID : %d",p_subject->GetId());         Logger::WriteMessage(text);
      text.Format("Name of subject : %s",p_subject->GetName());       Logger::WriteMessage(text);
      text.Format("Adresline 1     : %s",p_subject->GetAdresline1()); Logger::WriteMessage(text);
      text.Format("Adresline 2     : %s",p_subject->GetAdresline2()); Logger::WriteMessage(text);
      text.Format("Account ID      : %d",p_subject->GetAccount_id()); Logger::WriteMessage(text);
    }

    void PrintNaturalPerson(NaturalPerson* p_person)
    {
      PrintSubject(p_person);

      CString text;
      text.Format("Firstname       : %s",p_person->GetFirstname());                Logger::WriteMessage(text);
      text.Format("Date of birth   : %s",p_person->GetDate_of_birth().AsString()); Logger::WriteMessage(text);
      text.Format("Length in cm    : %s",p_person->GetLength().AsString());        Logger::WriteMessage(text);
    }

    void PrintSupplier(Supplier* p_supplier)
    {
      PrintSubject(p_supplier);

      CString text;
      text.Format("Delivery address: %s",p_supplier->GetDeliveryAddress());         Logger::WriteMessage(text);
      text.Format("Balance         : %s",p_supplier->GetBalance().AsString());      Logger::WriteMessage(text);
      text.Format("Open orders     : %s",p_supplier->GetOpen_orders().AsString());  Logger::WriteMessage(text);
      text.Format("Open billings   : %s",p_supplier->GetOpen_billings().AsString());Logger::WriteMessage(text);
    }

    void PrintInfant(Infant* p_infant)
    {
      PrintNaturalPerson(p_infant);

      CString text;
      text.Format("Immunisation    : %s",p_infant->GetImmunisation()); Logger::WriteMessage(text);
      text.Format("Complications   : %s",p_infant->GetComplication()); Logger::WriteMessage(text);
      text.Format("Hospital        : %s",p_infant->GetHospital());     Logger::WriteMessage(text);
    }

    // Open a CXHibernate session by config file
    // Without an external database
    bool OpenSession()
    {
      if(m_session)
      {
        return true;
      }

      // Init in the correct language
      InitSQLComponents(LN_ENGLISH);

      try
      {
        m_session = hibernate.CreateSession("x1","onetable.cfg.xml");
        if(m_session)
        {
          // We are a database session
          m_session->SetDatabaseConnection("hibtest","sysdba","altijd");

          return true;
        }
      }
      catch(StdException* er)
      {
        Assert::AreEqual("",er->GetErrorMessage());
        er->Delete();
      }
      Assert::Fail();
    }
  private:
    CXSession* m_session { nullptr };
  };
}
