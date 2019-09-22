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
      g_session->SetFilestore("C:\\WWW\\Testing");

      // Set our internet role
      g_session->SetInternet(g_url);
    }
    else
    {
      printf("ERROR: No Hibernate session started.\n");
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
  text.Format("Record ID      : %d", p_master->GetID());               puts(text);
  text.Format("Invoice number : %d", p_master->GetInvoice());          puts(text);
  text.Format("Description    : %s", p_master->GetDescription());      puts(text);
  text.Format("Total amount   : %s", p_master->GetTotal().AsString()); puts(text);
}

void PrintDetail(Detail* p_detail)
{
  CString text;
  text.Format("Record ID      : %d", p_detail->GetID());                puts(text);
  text.Format("Master ID      : %d", p_detail->GetMasterID());          puts(text);
  text.Format("Line number    : %d", p_detail->GetLine());              puts(text);
  text.Format("Description    : %s", p_detail->GetDescription());       puts(text);
  text.Format("Part amount    : %s", p_detail->GetAmount().AsString()); puts(text);
}

void T01_SelectMaster()
{
  printf("%s: Getting a record from the MASTER table\n",__FUNCTION__);

  try
  {
    Master* master = (Master*)g_session->Load(Master::ClassName(), 2);

    ASSERT(master != nullptr);
    printf("Testing 2th master record\n");
    PrintMaster(master);
  }
  catch(StdException& er)
  {
    printf("ERROR: %s\n", er.GetErrorMessage().GetString());
  }
}

void T02_SelectDetails()
{
  printf("%s: Getting all records from the DETAIL table with 'line > 1'\n",__FUNCTION__);

  try
  {
    Filter filter("line",OP_Greater,1);

    CXResultSet set = g_session->Load(Detail::ClassName(),&filter);
    for(auto& object : set)
    {
      Detail* detail = reinterpret_cast<Detail*>(object);
      PrintDetail(detail);
    }
  }
  catch (StdException& er)
  {
    printf("ERROR: %s\n", er.GetErrorMessage().GetString());
  }
}

void T03_UpdateTest()
{
  printf("%s: Updating a record from the MASTER table\n",__FUNCTION__);

  try
  {
    CXObject* object = g_session->Load(Master::ClassName(), 1);
    Master* master = reinterpret_cast<Master*>(object);

    ASSERT(master != nullptr);
    printf("Updating 1th master record\n");

    // Keeping the old one (750.00)
    bcd old_total = master->GetTotal();

    // Setting new value
    bcd total("935.12");
    master->SetTotal(total);
    bool res = g_session->Save(master);
    ASSERT(res == true);

    // Back to the old value
    master->SetTotal(old_total);
    res = g_session->Save(master);
    ASSERT(res == true);

    printf("Record updated\n");
  }
  catch (StdException& er)
  {
    printf("ERROR: %s\n", er.GetErrorMessage().GetString());
  }
}

void T04_InsertDelete()
{
  printf("%s: Insert a record into the TEST_NUMBER table.\n",__FUNCTION__);

  try
  {
    TestNumber* numbers = (TestNumber*)g_session->CreateObject(TestNumber::ClassName());
    // Do Not set the ID field: this will be done by the generator
    numbers->SetField1(42);
    numbers->SetField2(89975.123);
    numbers->SetField3("300.77");

    // Insert this object in the database
    bool result = g_session->Insert(numbers);
    ASSERT(result == true);

    // Show id of inserted object
    int id = numbers->GetID();
    printf("Test_number object [%d] inserted.\n",id);

    // Delete the object again and destroy the derived object!
    result = g_session->Delete(numbers);
    ASSERT(result = true);

    // Test that it is gone
    printf("Test_number record deleted again!\n");
  }
  catch (StdException& er)
  {
    printf("ERROR: %s\n", er.GetErrorMessage().GetString());
  }
}

// Wait for key to occur
// so the messages can be send and debugged :-)
void
WaitForKey()
{
  char buffer[256];
  size_t readIn = 0;
  _cgets_s(buffer, 256, &readIn);
}

int main(int argc,char* argv[])
{
  // initialize MFC and print and error on failure
  if(!AfxWinInit(::GetModuleHandle(NULL),NULL,::GetCommandLine(), 0))
  {
    CString command = ::GetCommandLine();
    CString message = "Fatal Error in TestClient: MFC initialization failed " + command;
    fprintf(stderr, message + "\n");
    return -3;
  }

  // Test if we should connect to an IIS installation
  if(argc == 2)
  {
    if(_stricmp(argv[1], "/iis") == 0)
    {
      ++g_port;
    }
  }
  g_url.Format("http://localhost:%d/CXServer/",g_port);

  // Print who we are
  printf("CXHibernate test client program\n");
  printf("===============================\n");

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

