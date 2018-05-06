////////////////////////////////////////////////////////////////////////
//
// File: UnitTest.cpp
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
#include "CppUnitTest.h"
#include "CXSession.h"
#include "CXClass.h"
#include "CXMaster.h"
#include "CXDetail.h"
#include "TestNumber.h"
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

namespace UnitTest
{		
	TEST_CLASS(UnitTest)
	{
	public:
		
		TEST_METHOD(T01_SelectMaster)
		{
      Logger::WriteMessage("Getting a record from the MASTER table");

      OpenSession();
      try
      {
        CXObject* object = m_session->SelectObject("master",2);
        CXMaster* master = reinterpret_cast<CXMaster*>(object);

        Assert::IsNotNull(master);

        Logger::WriteMessage("Testing 2th master record");
        PrintMaster(master);
      }
      catch(CString& s)
      {
        Logger::WriteMessage("ERROR: " + s);
        Assert::Fail();
      }
		}

    TEST_METHOD(T02_SelectDetails)
    {
      Logger::WriteMessage("Getting all records from the DETAIL table with 'line > 1'");
      if(OpenSession())
      {
        SQLVariant one((long)1);
        SQLFilterSet filters;
        SQLFilter filter("line", OP_Greater,&one);
        filters.push_back(filter);

        CXResultSet set = m_session->SelectObject("detail",filters);
        for(int ind = 0;ind < set.size(); ++ind)
        {
          CXDetail* detail = reinterpret_cast<CXDetail*>(set[ind]);
          PrintDetail(detail);
        }
      }
      else
      {
        Assert::Fail(L"Database was not opened");
      }
    }

    TEST_METHOD(T03_UpdateTest)
    {
      Logger::WriteMessage("Updating a record from the MASTER table");
      if (OpenSession())
      {
        CXObject* object = m_session->SelectObject("master",1);
        CXMaster* master = reinterpret_cast<CXMaster*>(object);

        Assert::IsNotNull(master);
        Logger::WriteMessage("Updating 1th master record");

        // Keeping the old one (750.00)
        bcd old_total = master->GetTotal();

        // Setting new value
        bcd total("935.12");
        master->SetTotal(total);
        m_session->UpdateObject(master);

        // Back to the old value
        master->SetTotal(old_total);
        m_session->UpdateObject(master);
      }
      else
      {
        Assert::Fail(L"Database was not opened");
      }
    }

    void PrintMaster(CXMaster* p_master)
    {
      CString text;
      text.Format("Record ID      : %d", p_master->GetID());               Logger::WriteMessage(text);
      text.Format("Invoice number : %d", p_master->GetInvoice());          Logger::WriteMessage(text);
      text.Format("Description    : %s", p_master->GetDescription());      Logger::WriteMessage(text);
      text.Format("Total amount   : %s", p_master->GetTotal().AsString()); Logger::WriteMessage(text);
    }

    void PrintDetail(CXDetail* p_detail)
    {
      CString text;
      text.Format("Record ID      : %d", p_detail->GetID());                Logger::WriteMessage(text);
      text.Format("Master ID      : %d", p_detail->GetMasterID());          Logger::WriteMessage(text);
      text.Format("Line number    : %d", p_detail->GetLine());              Logger::WriteMessage(text);
      text.Format("Description    : %s", p_detail->GetDescription());       Logger::WriteMessage(text);
      text.Format("Part amount    : %s", p_detail->GetAmount().AsString()); Logger::WriteMessage(text);
    }

    TEST_METHOD(T04_InsertDelete)
    {
      Logger::WriteMessage("Insert a record into the TEST_NUMBER table");

      OpenSession();

      CXClass* test_numbers = m_session->FindClass("test_number");
      TestNumber* numbers = new TestNumber(test_numbers);
      numbers->SetID(10);
      numbers->SetField1(42);
      numbers->SetField2(89975.123);
      numbers->SetField3("300.77");

      // Insert this object in the database
      m_session->InsertObject(numbers);

      // Test that it is in the database
      int num = TestRecordCount("test_number","id",10);
      Assert::AreEqual(num,1);
      Logger::WriteMessage("Test_number record 10 Inserted!");

      // Delete the object again and destroy the derived object!
      m_session->DeleteObject(numbers);

      // Test that it is gone
      num = TestRecordCount("test_number", "id", 10);
      Assert::AreEqual(num,0);
      Logger::WriteMessage("Test_number record 10 Deleted again!");
    }

    TEST_METHOD(T05_SelectToFilestore)
    {
      Logger::WriteMessage("Getting all records from the DETAIL table and save in FILESTORE");
      if (OpenSession())
      {
        SQLVariant one((long)1);
        SQLFilter filter("line", OP_GreaterEqual, &one);

        CXResultSet set = m_session->SelectObject("detail",&filter);

        m_session->SetBaseDirectory("C:\\WWW\\Testing");
        m_session->ChangeRole(CXHRole::CXH_Filestore_role);

        for(auto& object : set)
        {
          m_session->InsertObject(object);
        }
      }
      else
      {
        Assert::Fail(L"Database was not opened");
      }
    }

    TEST_METHOD(T06_SelectFromFilestore)
    {
      Logger::WriteMessage("Getting a record from the DETAIL table from the FILESTORE");
      if(OpenSession())
      {
        SQLVariant one((long)7);
        m_session->SetBaseDirectory("C:\\WWW\\Testing");

        CXObject* object = m_session->SelectObject("detail",&one);
        CXDetail* detail = reinterpret_cast<CXDetail*>(object);
        Assert::IsNotNull(detail);
        PrintDetail(detail);
      }
      else
      {
        Assert::Fail(L"Database was not opened");
      }
    }

    TEST_METHOD(T07_DeleteFromFilestore)
    {
      Logger::WriteMessage("Delete a record from the DETAIL table from the FILESTORE");
      if (OpenSession())
      {
        m_session->ChangeRole(CXHRole::CXH_Filestore_role);
        SQLVariant one((long)6);

        CXObject* object = m_session->SelectObject("detail", &one);
        CXDetail* detail = reinterpret_cast<CXDetail*>(object);

        Assert::IsNotNull(detail);
        PrintDetail(detail);

        bool res = m_session->DeleteObject(detail);
        Assert::IsTrue(res);
      }
      else
      {
        Assert::Fail(L"Database was not opened");
      }
    }

    TEST_METHOD(T08_WriteTableInfo)
    {
      Logger::WriteMessage("Write meta info of tables");
      if(OpenSession())
      {
        CXTable* master = m_session->FindClass("master")->GetTable();
        CXTable* detail = m_session->FindClass("detail")->GetTable();
        Assert::IsNotNull(master);
        Assert::IsNotNull(detail);

        bool res1 = master->SaveMetaInfo(m_session);
        bool res2 = detail->SaveMetaInfo(m_session);
        Assert::IsTrue(res1);
        Assert::IsTrue(res2);
      }
    }

    TEST_METHOD(T09_LoadTableInfo)
    {
      Logger::WriteMessage("Write meta info of tables");
      if (OpenSession())
      {
        CXTable* master = m_session->FindClass("master")->GetTable();
        CXTable* detail = m_session->FindClass("detail")->GetTable();
        Assert::IsNotNull(master);
        Assert::IsNotNull(detail);

        bool res1 = master->LoadMetaInfo(m_session);
        bool res2 = detail->LoadMetaInfo(m_session);
        Assert::IsTrue(res1);
        Assert::IsTrue(res2);
      }
    }

    // Open a CXHibernate session and add the 'master' and 'detail' tables
    bool OpenSession()
    {
      // Check if already done
      if(m_database.IsOpen())
      {
        return true;
      }

      // Init in the correct language
      InitSQLComponents(LN_ENGLISH);

      try
      {
        m_database.Open("Testing","sysdba","altijd");
        if(m_database.IsOpen())
        {
          hibernate.SetDefaultSchema("sysdba");
          hibernate.SetStrategy(MapStrategy::Strategy_standalone);
          m_session = hibernate.CreateSession();

          m_session->SetBaseDirectory("C:\\WWW\\Testing");
          m_session->SetDatabase(&m_database);

          CXClass* master  = new CXClass("master",     CXO_FACTORY(CXMaster));
          CXClass* detail  = new CXClass("detail",     CXO_FACTORY(CXDetail));
          CXClass* numbers = new CXClass("test_number",CXO_FACTORY(TestNumber));

          // Do the 'lazy' stuff by reading the definition from the database
          // assuming that the definition corresponds with ours
          ReadTableDefinition(master);
          ReadTableDefinition(detail);
          ReadTableDefinition(numbers);

          return true;
        }
        Assert::Fail();
      }
      catch(CString& s)
      {
        Assert::AreEqual(s,"");
      }
      Assert::Fail();
      return false;
    }

    void ReadTableDefinition(CXClass* p_class)
    {
      CXTable* table = p_class->GetTable();
      if(table->GetMetaInfoFromDatabase(m_database,true,true,true))
      {
        m_session->AddClass(p_class);
      }
      else
      {
        Assert::Fail(L"Table structure of table not found");
      }
    }

    int TestRecordCount(CString p_table, CString p_column, int p_value)
    {
      int result = 0;

      if(m_database.IsOpen() == false)
      {
        return -1;
      }
      try
      {
        CString sql;
        sql.Format("SELECT COUNT(*)\n"
                   "  FROM %s\n"
                   " WHERE %s = ?",p_table,p_column);
        SQLQuery query(m_database);
        SQLTransaction trans(&m_database,"count");

        query.DoSQLStatement(sql,p_value);
        if(query.GetRecord())
        {
          result = query[1];
        }
        trans.Commit();
      }
      catch(CString& er)
      {
        Logger::WriteMessage(CString("Failed: ") + er);
      }
      return result;
    }

    CXSession*  m_session { nullptr };
    SQLDatabase m_database;
	};
}