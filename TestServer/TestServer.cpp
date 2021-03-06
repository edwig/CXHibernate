// TestServer.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "CXHibernate.h"
#include "CXSession.h"
#include "CXServer.h"
#include <conio.h>

CXSession* g_session = nullptr;
ErrorReport g_errorReport;

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

      // We are a database session
      g_session->SetDatabaseConnection("Testing","sysdba","altijd");
    }
    else
    {
      printf("ERROR: No Hibernate session started.\n");
      exit(-3);
    }
  }
  catch (StdException& er)
  {
    printf("Error while creating Hibernate session: %s\n",er.GetErrorMessage().GetString());
    exit(-3);
  }
}

// Close our global CXHibernate session again
void CloseSession()
{
  if(g_session)
  {
    // Closing and destroying the session
    g_session->CloseSession();
    g_session = nullptr;
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
    CString message = "Fatal Error in TestServer: MFC initialization failed " + command;
    fprintf(stderr, message + "\n");
    return -3;
  }

  // Print who we are
  printf("CXHibernate test server program\n");
  printf("===============================\n");

  // Start our CX-Hibernate session
  StartSession();

  // Use this namespace
  CString namesp("http://www.cxhibernate.org/");
  CString url("http://localhost:1959/CXServer/");

  // Create a server
  CXServer cxserver(g_session
                   ,"CXTestServer"
                   ,"C:\\WWW\\"
                   ,url
                   ,PrefixType::URLPRE_Strong
                   ,namesp
                   ,10);

  // Connect OUR logfile
  cxserver.SetLogAnalysis(hibernate.GetLogAnalysis());
  cxserver.SetLogLevel(hibernate.GetLogLevel());

  // Connect an error reporting object
  cxserver.SetErrorReport(&g_errorReport);

  // No WSDL (yet)
  cxserver.RegisterOperations();
  cxserver.SetGenerateWsdl(false);

  // Start the service. 
  // Rely on the start for creating an HTTPServer, a HTTPSite and a HTTPHandlerPost
  // to service the CX-webservice interface
  if(cxserver.RunService() == false)
  {
    printf("Could not start the CX-Hibernate test server!\n");
    printf("Check the URL reservations on your machine!\n");
    exit(-5);
  }

  printf("\n"
        "Server running....\n"
        "Waiting to be called by test clients...\n"
        "\n");
  // Wait for key to occur
  WaitForKey();

  // Try stopping the server
  printf("Stopping the server\n");
  cxserver.Stop();

  // See if the server is indeed in stopped state
  printf("The server is %s\n", cxserver.IsRunning() ? "still running!\n" : "stopped.\n");

  // As a last step, close our Hibernate session
  CloseSession();

  return 0;
}

