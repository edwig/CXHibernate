////////////////////////////////////////////////////////////////////////
//
// File: TEST_Standalone.cpp
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
#include "CppUnitTest.h"
#include "CXSession.h"
#include "CXClass.h"
#include "Master.h"
#include "Detail.h"
#include "TestNumber.h"
#include <SQLAutoDBS.h>
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
	TEST_CLASS(StandAlone)
	{
	public:
    ~StandAlone()
    {
      hibernate.CloseAllSessions();
    }
		
		TEST_METHOD(T01_SelectMaster)
		{
      Logger::WriteMessage(_T("Getting a record from the MASTER table"));

      try
      {
        OpenSession();

        Master* master = (Master*) m_session->Load(Master::ClassName(),2);

        Assert::IsNotNull(master);
        Logger::WriteMessage(_T("Testing 2th master record"));
        PrintMaster(master);
      }
      catch(StdException& er)
      {
        Logger::WriteMessage(_T("ERROR: ") + er.GetErrorMessage());
        Assert::Fail();
      }
		}

    TEST_METHOD(T02_SelectDetails)
    {
      Logger::WriteMessage(_T("Getting all records from the DETAIL table with 'line > 1'"));
      
      OpenSession();

      Filter* filter = new Filter(_T("line"),OP_Greater,1);

      CXResultSet set = m_session->Load(Detail::ClassName(),filter);
      for(int ind = 0;ind < (int)set.size(); ++ind)
      {
        Detail* detail = reinterpret_cast<Detail*>(set[ind]);
        PrintDetail(detail);
      }
    }

    TEST_METHOD(T03_UpdateTest)
    {
      Logger::WriteMessage(_T("Updating a record from the MASTER table"));

      OpenSession();

      CXObject* object = m_session->Load(Master::ClassName(),1);
      Master* master = reinterpret_cast<Master*>(object);

      Assert::IsNotNull(master);
      Logger::WriteMessage(_T("Updating 1th master record"));

      // Keeping the old one (750.00)
      bcd old_total = master->GetTotal();

      // Setting new value
      bcd total(_T("935.12"));
      master->SetTotal(total);
      bool res = m_session->Save(master);
      Assert::IsTrue(res);

      CString value = TestRecordValue(_T("master"),_T("id"),1,_T("total"));
      Assert::AreEqual(value,_T("935.12"));

      // Back to the old value
      master->SetTotal(old_total);
      res = m_session->Save(master);
      Assert::IsTrue(res);

      value = TestRecordValue(_T("master"),_T("id"),1,_T("total"));
      Assert::AreEqual(value.GetString(),old_total.AsString().GetString());
    }

    void PrintMaster(Master* p_master)
    {
      CString text;
      text.Format(_T("Record ID      : %d"), p_master->GetID());               Logger::WriteMessage(text);
      text.Format(_T("Invoice number : %d"), p_master->GetInvoice());          Logger::WriteMessage(text);
      text.Format(_T("Description    : %s"), p_master->GetDescription());      Logger::WriteMessage(text);
      text.Format(_T("Total amount   : %s"), p_master->GetTotal().AsString()); Logger::WriteMessage(text);
    }

    void PrintDetail(Detail* p_detail)
    {
      CString text;
      text.Format(_T("Record ID      : %d"), p_detail->GetID());                Logger::WriteMessage(text);
      text.Format(_T("Master ID      : %d"), p_detail->GetMasterID());          Logger::WriteMessage(text);
      text.Format(_T("Line number    : %d"), p_detail->GetLine());              Logger::WriteMessage(text);
      text.Format(_T("Description    : %s"), p_detail->GetDescription());       Logger::WriteMessage(text);
      text.Format(_T("Part amount    : %s"), p_detail->GetAmount().AsString()); Logger::WriteMessage(text);
    }

    TEST_METHOD(T04_InsertDelete)
    {
      Logger::WriteMessage(_T("Insert a record into the TEST_NUMBER table"));

      OpenSession();

      TestNumber* numbers = (TestNumber*) m_session->CreateObject(TestNumber::ClassName());
      // Do Not set the ID field: this will be done by the generator
      numbers->SetField1(42);
      numbers->SetField2(89975.123);
      numbers->SetField3(bcd(_T("300.77")));

      // Insert this object in the database
      m_session->Insert(numbers);

      int id = numbers->GetID();

      // Test that it is in the database
      int num = TestRecordCount(_T("test_number"),_T("id"),numbers->GetID());
      Assert::AreEqual(num,1);
      Logger::WriteMessage(_T("Test_number record inserted!"));

      // Delete the object again and destroy the derived object!
      m_session->Delete(numbers);

      // Test that it is gone
      num = TestRecordCount(_T("test_number"), _T("id"), id);
      Assert::AreEqual(num,0);
      Logger::WriteMessage(_T("Test_number record deleted again!"));
    }

    TEST_METHOD(T05_SelectToFilestore)
    {
      Logger::WriteMessage(_T("Getting all records from the DETAIL table and save in FILESTORE"));
      
      OpenSession();

      var one((long)1);
      Filter* filter = new Filter(_T("line"), OP_GreaterEqual, &one);

      CXResultSet set = m_session->Load(_T("detail"),filter);

      m_session->SetFilestore(_T("C:\\WWW\\Testing"));
      m_session->ChangeRole(CXHRole::CXH_Filestore_role);

      for(auto& object : set)
      {
        m_session->Insert(object);
      }
    }

    TEST_METHOD(T06_SelectFromFilestore)
    {
      Logger::WriteMessage(_T("Getting a record from the DETAIL table from the FILESTORE"));
      
      OpenSession();

      var one(7);
      m_session->SetFilestore(_T("C:\\WWW\\Testing"));

      CXObject* object = m_session->Load(Detail::ClassName(),&one);
      Detail* detail = reinterpret_cast<Detail*>(object);
      Assert::IsNotNull(detail);
      PrintDetail(detail);
    }

    TEST_METHOD(T07_DeleteFromFilestore)
    {
      Logger::WriteMessage(_T("Delete a record from the DETAIL table from the FILESTORE"));

      OpenSession();

      m_session->ChangeRole(CXHRole::CXH_Filestore_role);

      CXObject* object = m_session->Load(Detail::ClassName(),6);
      Detail* detail = reinterpret_cast<Detail*>(object);

      Assert::IsNotNull(detail);
      PrintDetail(detail);

      bool res = m_session->Delete(detail);
      Assert::IsTrue(res);
    }

    TEST_METHOD(T08_WriteTableInfo)
    {
      Logger::WriteMessage(_T("Write meta info of tables"));

      OpenSession();
      
      CXTable* master = m_session->FindClass(Master::ClassName())->GetTable();
      CXTable* detail = m_session->FindClass(Detail::ClassName())->GetTable();
      CXTable* number = m_session->FindClass(TestNumber::ClassName())->GetTable();
      Assert::IsNotNull(master);
      Assert::IsNotNull(detail);
      Assert::IsNotNull(number);

      bool res1 = master->SaveMetaInfo(m_session);
      bool res2 = detail->SaveMetaInfo(m_session);
      bool res3 = number->SaveMetaInfo(m_session);
      Assert::IsTrue(res1);
      Assert::IsTrue(res2);
      Assert::IsTrue(res3);
    }

    TEST_METHOD(T09_LoadTableInfo)
    {
      Logger::WriteMessage(_T("Write meta info of tables"));

      OpenSession();

      CXTable* master = m_session->FindClass(Master::ClassName())->GetTable();
      CXTable* detail = m_session->FindClass(Detail::ClassName())->GetTable();
      Assert::IsNotNull(master);
      Assert::IsNotNull(detail);

      bool res1 = master->LoadMetaInfo(m_session);
      bool res2 = detail->LoadMetaInfo(m_session);
      Assert::IsTrue(res1);
      Assert::IsTrue(res2);
    }

    TEST_METHOD(T10_FollowAssocToDetails)
    {
      Logger::WriteMessage(_T("Testing following an association to our details"));
      try
      {
        OpenSession();

        Master* master = (Master*)m_session->Load(Master::ClassName(),2);
        Assert::IsNotNull(master);
        Logger::WriteMessage(_T("Testing 1th master record and it's details"));
        PrintMaster(master);

     // The following two lines are identical
     // CXResultSet set = m_session->FollowAssociation(Master::ClassName(),Detail::ClassName(),master->GetID());
     // CXResultSet set = master->GetDetailsOfMaster(m_session);
        CXResultSet set = m_session->FollowAssociation(master,Detail::ClassName(),master->GetID());
        for(auto& object : set)
        {
          Detail* detail = (Detail*)object;
          CString message;
          message.Format(_T("Detail: %d Master: %d : Detail line: %d Amount: %s")
                         ,detail->GetID()
                         ,master->GetID()
                         ,detail->GetLine()
                         ,detail->GetAmount().AsString().GetString());
          Logger::WriteMessage(message);
        }
      }
      catch (StdException& er)
      {
        Logger::WriteMessage(_T("ERROR: ") + er.GetErrorMessage());
        Assert::Fail();
      }
    }

    TEST_METHOD(T11_FollowAssocToMaster)
    {
      Logger::WriteMessage(_T("Testing following an association to our master record"));

      try
      {
        OpenSession();

        Detail* detail = (Detail*) m_session->Load(Detail::ClassName(),7);
        Assert::IsNotNull(detail);

        Master* master = detail->GetMasterOfDetail(m_session);
        Assert::IsNotNull(master);

        CString message1;
        message1.Format(_T("Detail record [%d] of master [%d]. Description [%s] Amount: %s")
                        ,detail->GetID()
                        ,master->GetID()
                        ,detail->GetDescription().GetString()
                        ,detail->GetAmount().AsString().GetString());
        Logger::WriteMessage(message1);
        Logger::WriteMessage(_T("This is our master:"));
        PrintMaster(master);
      }
      catch (StdException& er)
      {
        Logger::WriteMessage(_T("ERROR: ") + er.GetErrorMessage());
        Assert::Fail();
      }
    }

    // Open a CXHibernate session and add the 'master' and 'detail' tables
    bool OpenSessionFULL()
    {
      try
      {
        // Init in the correct language, before 'CreateSession' is called!!
        InitSQLComponents(LN_ENGLISH);

        m_session->SetDatabaseConnection(_T("hibtest"),_T("sysdba"),_T("altijd"));
        if(m_session->GetDatabaseIsOpen())
        {
          hibernate.SetDefaultSchema(_T("sysdba"));
          hibernate.SetStrategy(MapStrategy::Strategy_standalone);
          m_session = hibernate.CreateSession();

          m_session->SetFilestore(_T("C:\\WWW\\Testing"));

          if(m_session->FindClass(Master::ClassName()) == nullptr)
          {
            // If no configuration loaded, define it
            CXClass* master  = new CXClass(Master::ClassName());
            CXClass* detail  = new CXClass(Detail::ClassName());
            CXClass* numbers = new CXClass(TestNumber::ClassName());

            // Test with different table name
            numbers->GetTable()->SetSchemaTableType(_T(""),_T("test_number"),_T("TABLE"));

            // Program the class structure (instead of configuration.cxh)
            DefineMaster(master);
            DefineDetail(detail);
            DefineNumbers(numbers);

            // Do the 'lazy' stuff by reading the definition from the database
            // assuming that the definition corresponds with ours
            ReadTableDefinition(master);
            ReadTableDefinition(detail);
            ReadTableDefinition(numbers);

            // hibernate.SaveConfiguration(m_session);
          }
          return true;
        }
        Assert::Fail();
      }
      catch(StdException& er)
      {
        Logger::WriteMessage(er.GetErrorMessage());
      }
      Assert::Fail();
      return false;
    }

    void DefineMaster(CXClass* p_class)
    {
      p_class->AddAttribute(new CXAttribute(_T("int"),   _T("id"),0,false,true));
      p_class->AddAttribute(new CXAttribute(_T("int"),   _T("invoice")));
      p_class->AddAttribute(new CXAttribute(_T("string"),_T("description"),250));
      p_class->AddAttribute(new CXAttribute(_T("bcd"),   _T("total")));

      CXIdentity primary;
      primary.m_constraintName = _T("pk_master");
      primary.m_attributes.push_back(p_class->FindAttribute(_T("id")));
      p_class->AddIdentity(primary);
    }

    void DefineDetail(CXClass* p_class)
    {
      p_class->AddAttribute(new CXAttribute(_T("int"),   _T("id"),     0,false,true));
      p_class->AddAttribute(new CXAttribute(_T("int"),   _T("mast_id"),0,false,false,true));
      p_class->AddAttribute(new CXAttribute(_T("int"),   _T("line")));
      p_class->AddAttribute(new CXAttribute(_T("string"),_T("description"),250));
      p_class->AddAttribute(new CXAttribute(_T("bcd"),   _T("amount")));

      CXIdentity primary;
      primary.m_constraintName = _T("pk_detail");
      primary.m_attributes.push_back(p_class->FindAttribute(_T("id")));
      p_class->AddIdentity(primary);
    }

    void DefineNumbers(CXClass* p_class)
    {
      p_class->AddAttribute(new CXAttribute(_T("int"),   _T("id"),0,true,true));
      p_class->AddAttribute(new CXAttribute(_T("int"),   _T("field1")));
      p_class->AddAttribute(new CXAttribute(_T("double"),_T("field2")));
      p_class->AddAttribute(new CXAttribute(_T("bcd"),   _T("field3")));

      CXIdentity primary;
      primary.m_constraintName = _T("pk_test_numbers");
      primary.m_attributes.push_back(p_class->FindAttribute(_T("id")));
      p_class->AddIdentity(primary);
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
          m_session = hibernate.CreateSession();
          if(m_session)
          {
            // Where we have our filestore
            m_session->SetFilestore(_T("C:\\WWW\\Testing"));

            // We are a database session
            m_session->SetDatabaseConnection(_T("hibtest"),_T("sysdba"),_T("altijd"));

            return true;
          }
      }
      catch(StdException& er)
      {
        Assert::AreEqual(_T(""),er.GetErrorMessage());
      }
      return OpenSessionFULL();
    }

    void ReadTableDefinition(CXClass* p_class)
    {
      SQLAutoDBS database(*m_session->GetDatabasePool(),m_session->GetDatabaseConnection());
      CXTable* table = p_class->GetTable();
      if(table->GetMetaInfoFromDatabase(*database,true,true,true) == false)
      {
        Assert::Fail(L"Table structure of table not found and loaded!");
      }
      m_session->AddClass(p_class);
    }

    int TestRecordCount(CString p_table, CString p_column, int p_value)
    {
      int result = 0;

      SQLAutoDBS database(*m_session->GetDatabasePool(),m_session->GetDatabaseConnection());
      if(database->IsOpen() == false)
      {
        return -1;
      }
      try
      {
        CString sql;
        sql.Format(_T("SELECT COUNT(*)\n")
                   _T("  FROM %s\n")
                   _T(" WHERE %s = ?"),p_table,p_column);
        SQLQuery query(database);
        SQLTransaction trans(database,_T("count"));

        query.DoSQLStatement(sql,p_value);
        if(query.GetRecord())
        {
          result = query[1];
        }
        trans.Commit();
      }
      catch(StdException& er)
      {
        Logger::WriteMessage(CString(_T("Failed: ")) + er.GetErrorMessage());
      }
      return result;
    }

    CString TestRecordValue(CString p_table,CString p_key,int p_value,CString p_column)
    {
      CString result;

      SQLAutoDBS database(*m_session->GetDatabasePool(),m_session->GetDatabaseConnection());
      if(database->IsOpen() == false)
      {
        return _T("");
      }
      try
      {
        CString sql;
        sql.Format(_T("SELECT %s\n")
                   _T("  FROM %s\n")
                   _T(" WHERE %s = ?"),p_column,p_table,p_key);
        SQLQuery query(database);
        SQLTransaction trans(database,_T("value"));

        query.DoSQLStatement(sql,p_value);
        if(query.GetRecord())
        {
          result = query[1].GetAsChar();
        }
        trans.Commit();
      }
      catch(StdException& er)
      {
        Logger::WriteMessage(CString(_T("Failed: ")) + er.GetErrorMessage());
      }
      return result;
    }
    CXSession*  m_session { nullptr };
	};
}
