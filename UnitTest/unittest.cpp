#include "stdafx.h"
#include "CppUnitTest.h"
#include <CXSession.h>
#include <SQLComponents.h>
#include <SQLVariant.h>
#include <SQLQuery.h>
#include "CXMaster.h"
#include "CXDetail.h"
#include "TestNumber.h"

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
      if(OpenSession())
      {
        SQLVariant var((long)2);
        VariantSet set;
        set.push_back(&var);

        CXObject* object = m_session.SelectObject("master",set,CXO_FACTORY(CXMaster));
        CXMaster* master = reinterpret_cast<CXMaster*>(object);

        Assert::IsNotNull(master);
        Logger::WriteMessage("Testing 2th master record");

        PrintMaster(master);
      }
      else
      {
        Assert::Fail(L"Database was not opened");
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

        CXResultSet set = m_session.SelectObject("detail",filters,CXO_FACTORY(CXDetail));
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
        SQLVariant var((long)1);
        VariantSet set;
        set.push_back(&var);

        CXObject* object = m_session.SelectObject("master", set, CXO_FACTORY(CXMaster));
        CXMaster* master = reinterpret_cast<CXMaster*>(object);

        Assert::IsNotNull(master);
        Logger::WriteMessage("Updating 1th master record");

        // Keeping the old one (750.00)
        bcd old_total = master->GetTotal();

        // Setting new value
        bcd total("935.12");
        master->SetTotal(total);
        m_session.UpdateObject(master);

        // Back to the old value
        master->SetTotal(old_total);
        m_session.UpdateObject(master);
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

      if(OpenSession())
      {
        CXTable* test_numbers = m_session.FindTable("test_number");
        TestNumber* numbers = new TestNumber(test_numbers);
        numbers->SetID(10);
        numbers->SetField1(42);
        numbers->SetField2(89975.123);
        numbers->SetField3("300.77");

        // Insert this object in the database
        m_session.InsertObject(numbers);

        // Test that it is in the database
        int num = TestRecordCount("test_number","id",10);
        Assert::AreEqual(num,1);
        Logger::WriteMessage("Test_number record 10 Inserted!");

        // Delete the object again and destroy the derived object!
        m_session.DeleteObject(numbers);

        // Test that it is gone
        num = TestRecordCount("test_number", "id", 10);
        Assert::AreEqual(num,0);
        Logger::WriteMessage("Test_number record 10 Deleted again!");
      }
      else
      {
        Assert::Fail(L"Database was not opened");
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
          m_session.SetDatabase(&m_database);
          m_session.SetMaster(true);

          CXTable* master  = new CXTable("", "master");
          CXTable* detail  = new CXTable("", "detail");
          CXTable* numbers = new CXTable("", "test_number");
          if(master->GetMetaInfoFromDatabase(m_database))
          {
            m_session.AddTable(master);
          }
          else
          {
            Assert::Fail(L"Table structure of 'master' table not found");
          }
          if(detail->GetMetaInfoFromDatabase(m_database))
          {
            m_session.AddTable(detail);
          }
          else
          {
            Assert::Fail(L"Table structure of 'detail' table not found");
          }
          if(numbers->GetMetaInfoFromDatabase(m_database))
          {
            m_session.AddTable(numbers);
          }
          else
          {
            Assert::Fail(L"Table structure of 'test_number' table not found");
          }
          return true;
        }
        return false;
      }
      catch(CString& s)
      {
        Assert::AreEqual(s,"");
      }
      return false;
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
        CString sql("SELECT COUNT(*) FROM ");
        sql += p_table;
        sql += " WHERE ";
        sql += p_column;
        sql.AppendFormat(" = %d", p_value);
        SQLQuery query(m_database);
        SQLTransaction trans(&m_database,"count");

        query.DoSQLStatement(sql);
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

    CXSession   m_session;
    SQLDatabase m_database;
	};
}