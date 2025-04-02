////////////////////////////////////////////////////////////////////////
//
// File: OneTable.cpp
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
      Logger::WriteMessage(_T("T01_SelectSubject shows first subject in database"));
      OpenSession();

      try
      {
        Subject* sub = (Subject*)m_session->Load(Subject::ClassName(),1);
        PrintSubject(sub);
      }
      catch(StdException& ex)
      {
        Logger::WriteMessage(ex.GetErrorMessage());
        Assert::Fail();
      }
    }

    TEST_METHOD(T02_SelectNaturalPerson)
    {
      Logger::WriteMessage(_T("T02_SelectNaturalPerson shows a real person from the database"));
      OpenSession();

      try
      {
        NaturalPerson* person = (NaturalPerson*)m_session->Load(NaturalPerson::ClassName(),2);
        PrintNaturalPerson(person);
      }
      catch(StdException& ex)
      {
        Logger::WriteMessage(ex.GetErrorMessage());
        Assert::Fail();
      }
    }

    TEST_METHOD(T03_SelectSupplier)
    {
      Logger::WriteMessage(_T("T03_SelectSupplier shows a supplier from the database"));
      OpenSession();

      try
      {
        Supplier* supp = (Supplier*)m_session->Load(Supplier::ClassName(),3);
        PrintSupplier(supp);
      }
      catch(StdException& ex)
      {
        Logger::WriteMessage(ex.GetErrorMessage());
        Assert::Fail();
      }
    }
    
    TEST_METHOD(T04_SelectInfant)
    {
      Logger::WriteMessage(_T("T04_SelectInfant shows a child from the database"));
      OpenSession();

      try
      {
        Infant* infant = (Infant*)m_session->Load(Infant::ClassName(),4);
        PrintInfant(infant);
      }
      catch(StdException& ex)
      {
        Logger::WriteMessage(ex.GetErrorMessage());
        Assert::Fail();
      }
    }

    TEST_METHOD(T05_InsertDeleteNaturalPerson)
    {
      Logger::WriteMessage(_T("T05_InsertNaturalPerson in the database"));

      try
      {
        OpenSession();

        NaturalPerson* person = (NaturalPerson*) m_session->CreateObject(NaturalPerson::ClassName());
        person->SetName(_T("Test person 1"));
        person->SetAdresline1(_T("Downingstreet 10"));
        person->SetAdresline2(_T("London, City of Westminster"));
        person->SetAccount_id(665512);
        person->SetFirstname(_T("Florian-Robert"));
        person->SetDate_of_birth(SQLDate(_T("12-03-1992")));
        person->SetLength(bcd(_T("180")));

        if(m_session->Save(person))
        {
          int id = person->GetId();
          PrintNaturalPerson(person);

          if(m_session->Delete(person))
          {
            Logger::WriteMessage(_T("Person deleted again"));
          }
          else
          {
            CString error;
            error.Format(_T("Person with ID [%d] is ***NOT*** deleted"),id);
            Logger::WriteMessage(error);
            Assert::Fail();
          }
        }
        else
        {
          Logger::WriteMessage(_T("Person ***NOT*** saved"));
          Assert::Fail();
        }
      }
      catch (StdException& ex)
      {
        Logger::WriteMessage(ex.GetErrorMessage());
        Assert::Fail();
      }
    }

    TEST_METHOD(T06_UpdateNaturalPerson)
    {
      Logger::WriteMessage(_T("T06_UpdateNaturalPerson shows updating a person in the database"));

      try
      {
        OpenSession();

        NaturalPerson* person = (NaturalPerson*)m_session->Load(NaturalPerson::ClassName(),2);
        Assert::IsNotNull(person);
        PrintNaturalPerson(person);

        int account = person->GetAccount_id();

        SQLDate birth(_T("15-05-1962"));
        person->SetDate_of_birth(birth);
        person->SetAccount_id(567123);

        m_session->Update(person);
        PrintNaturalPerson(person);

        SQLDate unknown;
        person->SetDate_of_birth(unknown);
        person->SetAccount_id(account);

        m_session->Update(person);
        PrintNaturalPerson(person);
      }
      catch (StdException& ex)
      {
        Logger::WriteMessage(ex.GetErrorMessage());
        Assert::Fail();
      }
    }

    void PrintSubject(Subject* p_subject)
    {
      CString text;
      text.Format(_T("Subject with ID : %d"),p_subject->GetId());         Logger::WriteMessage(text);
      text.Format(_T("Name of subject : %s"),p_subject->GetName());       Logger::WriteMessage(text);
      text.Format(_T("Adresline 1     : %s"),p_subject->GetAdresline1()); Logger::WriteMessage(text);
      text.Format(_T("Adresline 2     : %s"),p_subject->GetAdresline2()); Logger::WriteMessage(text);
      text.Format(_T("Account ID      : %d"),p_subject->GetAccount_id()); Logger::WriteMessage(text);
    }

    void PrintNaturalPerson(NaturalPerson* p_person)
    {
      PrintSubject(p_person);

      CString text;
      text.Format(_T("Firstname       : %s"),p_person->GetFirstname());                Logger::WriteMessage(text);
      text.Format(_T("Date of birth   : %s"),p_person->GetDate_of_birth().AsString()); Logger::WriteMessage(text);
      text.Format(_T("Length in cm    : %s"),p_person->GetLength().AsString());        Logger::WriteMessage(text);
    }

    void PrintSupplier(Supplier* p_supplier)
    {
      PrintSubject(p_supplier);

      CString text;
      text.Format(_T("Delivery address: %s"),p_supplier->GetDeliveryAddress());         Logger::WriteMessage(text);
      text.Format(_T("Balance         : %s"),p_supplier->GetBalance().AsString());      Logger::WriteMessage(text);
      text.Format(_T("Open orders     : %s"),p_supplier->GetOpen_orders().AsString());  Logger::WriteMessage(text);
      text.Format(_T("Open billings   : %s"),p_supplier->GetOpen_billings().AsString());Logger::WriteMessage(text);
    }

    void PrintInfant(Infant* p_infant)
    {
      PrintNaturalPerson(p_infant);

      CString text;
      text.Format(_T("Immunisation    : %s"),p_infant->GetImmunisation()); Logger::WriteMessage(text);
      text.Format(_T("Complications   : %s"),p_infant->GetComplication()); Logger::WriteMessage(text);
      text.Format(_T("Hospital        : %s"),p_infant->GetHospital());     Logger::WriteMessage(text);
    }

    // Open a CXHibernate session by config file
    // Without an external database
    bool OpenSession()
    {
      if(m_session)
      {
        return true;
      }

      try
      {
        m_session = hibernate.CreateSession(_T("x1"),_T("onetable.cfg.xml"));
        if(m_session)
        {
          // We are a database session
          m_session->SetDatabaseConnection(_T("hibtest"),_T("sysdba"),_T("altijd"));

          return true;
        }
      }
      catch(StdException& er)
      {
        Assert::AreEqual(_T(""),er.GetErrorMessage());
      }
      Assert::Fail();
    }
  private:
    CXSession* m_session { nullptr };
  };
}
