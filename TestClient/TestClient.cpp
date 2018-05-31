// TestClient.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "CXHibernate.h"
#include "CXSession.h"
#include "CXServer.h"
#include "Master.h"
#include <conio.h>

CXSession* g_session = nullptr;
ErrorReport g_errorReport;
CString url("http://localhost:1959/CXServer/");

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
      g_session->SetInternet(url);
    }
    else
    {
      printf("ERROR: No Hibernate session started.\n");
      exit(-3);
    }
  }
  catch (StdException* er)
  {
    AfxMessageBox(er->GetErrorMessage());
    er->Delete();
    exit(-3);
  }
}

// Close our global CXHibernate session again
void CloseSession()
{
  if(g_session)
  {
    g_session->CloseSession();
    delete g_session;
  }
  g_session = nullptr;
}

void PrintMaster(Master* p_master)
{
  CString text;
  text.Format("Record ID      : %d\n", p_master->GetID());               puts(text);
  text.Format("Invoice number : %d\n", p_master->GetInvoice());          puts(text);
  text.Format("Description    : %s\n", p_master->GetDescription());      puts(text);
  text.Format("Total amount   : %s\n", p_master->GetTotal().AsString()); puts(text);
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
  catch(StdException* er)
  {
    printf("ERROR: %s\n", er->GetErrorMessage().GetString());
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

int main()
{
  // initialize MFC and print and error on failure
  if(!AfxWinInit(::GetModuleHandle(NULL),NULL,::GetCommandLine(), 0))
  {
    CString command = ::GetCommandLine();
    CString message = "Fatal Error in TestClient: MFC initialization failed " + command;
    fprintf(stderr, message + "\n");
    return -3;
  }

  // Print who we are
  printf("CXHibernate test client program\n");
  printf("===============================\n");

  // Start our CX-Hibernate session
  StartSession();

  T01_SelectMaster();

  // As a last step, close our Hibernate session
  CloseSession();

  return 0;
}

