// TestClient.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "CXHibernate.h"
#include "CXSession.h"
#include "CXServer.h"
#include "Master.h"
#include "Detail.h"
#include "TestNumber.h"
#include <conio.h>

CXSession*  g_session = nullptr;
ErrorReport g_errorReport;
int         g_port = 1959;
CString     g_url;

// Start a hibernate session for our default model
void StartSession()
{
  // Init in the correct language
  InitSQLComponents(LN_ENGLISH);

  try
  {
    g_session = hibernate.CreateSession();
    if(g_session)
    {
      // Where we have our filestore
      g_session->SetFilestore(_T("C:\\WWW\\Testing"));

      // Set our internet role
      g_session->SetInternet(g_url);
    }
    else
    {
      _tprintf(_T("ERROR: No Hibernate session started.\n"));
      exit(-3);
    }
  }
  catch (StdException& er)
  {
    AfxMessageBox(er.GetErrorMessage());
    exit(-3);
  }
}

// Close our global CXHibernate session again
void CloseSession()
{
  if(g_session)
  {
    g_session->CloseSession();
    g_session = nullptr;
  }
}

void PrintMaster(Master* p_master)
{
  CString text;
  text.Format(_T("Record ID      : %d"), p_master->GetID());                           _putts(text);
  text.Format(_T("Invoice number : %d"), p_master->GetInvoice());                      _putts(text);
  text.Format(_T("Description    : %s"), p_master->GetDescription().GetString());      _putts(text);
  text.Format(_T("Total amount   : %s"), p_master->GetTotal().AsString().GetString()); _putts(text);
}

void PrintDetail(Detail* p_detail)
{
  CString text;
  text.Format(_T("Record ID      : %d"), p_detail->GetID());                            _putts(text);
  text.Format(_T("Master ID      : %d"), p_detail->GetMasterID());                      _putts(text);
  text.Format(_T("Line number    : %d"), p_detail->GetLine());                          _putts(text);
  text.Format(_T("Description    : %s"), p_detail->GetDescription().GetString());       _putts(text);
  text.Format(_T("Part amount    : %s"), p_detail->GetAmount().AsString().GetString()); _putts(text);
}

void T01_SelectMaster()
{
  _tprintf(_T("%s: Getting a record from the MASTER table\n"),_T(__FUNCTION__));

  try
  {
    Master* master = (Master*)g_session->Load(Master::ClassName(), 2);

    ASSERT(master != nullptr);
    _tprintf(_T("Testing 2th master record\n"));
    PrintMaster(master);
  }
  catch(StdException& er)
  {
    _tprintf(_T("ERROR: %s\n"), er.GetErrorMessage().GetString());
  }
}

void T02_SelectDetails()
{
  _tprintf(_T("%s: Getting all records from the DETAIL table with 'line > 1'\n"),_T(__FUNCTION__));

  try
  {
    Filter filter(_T("line"),OP_Greater,1);

    CXResultSet set = g_session->Load(Detail::ClassName(),&filter);
    for(auto& object : set)
    {
      Detail* detail = reinterpret_cast<Detail*>(object);
      PrintDetail(detail);
    }
  }
  catch (StdException& er)
  {
    _tprintf(_T("ERROR: %s\n"), er.GetErrorMessage().GetString());
  }
}

void T03_UpdateTest()
{
  _tprintf(_T("%s: Updating a record from the MASTER table\n"),_T(__FUNCTION__));

  try
  {
    CXObject* object = g_session->Load(Master::ClassName(), 1);
    Master* master = reinterpret_cast<Master*>(object);

    ASSERT(master != nullptr);
    _tprintf(_T("Updating 1th master record\n"));

    // Keeping the old one (750.00)
    bcd old_total = master->GetTotal();

    // Setting new value
    bcd total(_T("935.12"));
    master->SetTotal(total);
    bool res = g_session->Save(master);
    ASSERT(res == true);

    // Back to the old value
    master->SetTotal(old_total);
    res = g_session->Save(master);
    ASSERT(res == true);

    _tprintf(_T("Record updated\n"));
  }
  catch (StdException& er)
  {
    _tprintf(_T("ERROR: %s\n"), er.GetErrorMessage().GetString());
  }
}

void T04_InsertDelete()
{
  _tprintf(_T("%s: Insert a record into the TEST_NUMBER table.\n"),_T(__FUNCTION__));

  try
  {
    TestNumber* numbers = (TestNumber*)g_session->CreateObject(TestNumber::ClassName());
    // Do Not set the ID field: this will be done by the generator
    numbers->SetField1(42);
    numbers->SetField2(89975.123);
    numbers->SetField3(bcd(_T("300.77")));

    // Insert this object in the database
    bool result = g_session->Insert(numbers);
    ASSERT(result == true);

    // Show id of inserted object
    int id = numbers->GetID();
    _tprintf(_T("Test_number object [%d] inserted.\n"),id);

    // Delete the object again and destroy the derived object!
    result = g_session->Delete(numbers);
    ASSERT(result == true);

    // Test that it is gone
    _tprintf(_T("Test_number record deleted again!\n"));
  }
  catch (StdException& er)
  {
    _tprintf(_T("ERROR: %s\n"), er.GetErrorMessage().GetString());
  }
}

// Wait for key to occur
// so the messages can be send and debugged :-)
void
WaitForKey()
{
  TCHAR buffer[256];
  size_t readIn = 0;
  _cgetts_s(buffer, 256, &readIn);
}

int _tmain(int argc,TCHAR* argv[])
{
  // initialize MFC and print and error on failure
  if(!AfxWinInit(::GetModuleHandle(NULL),NULL,::GetCommandLine(), 0))
  {
    CString command = ::GetCommandLine();
    CString message = _T("Fatal Error in TestClient: MFC initialization failed ") + command;
    _ftprintf(stderr, message + _T("\n"));
    return -3;
  }

  // Test if we should connect to an IIS installation
  if(argc == 2)
  {
    if(_tcsicmp(argv[1], _T("/iis")) == 0)
    {
      ++g_port;
    }
  }
  g_url.Format(_T("http://localhost:%d/CXServer/"),g_port);

  // Print who we are
  _tprintf(_T("CXHibernate test client program\n"));
  _tprintf(_T("===============================\n"));

  // Start our CX-Hibernate session
  StartSession();

  T01_SelectMaster();
  T02_SelectDetails();
  T03_UpdateTest();
  T04_InsertDelete();

  // As a last step, close our Hibernate session
  CloseSession();

  return 0;
}

