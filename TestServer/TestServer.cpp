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
      g_session->SetFilestore(_T("C:\\WWW\\Testing"));

      // We are a database session
      g_session->SetDatabaseConnection(_T("Testing"),_T("sysdba"),_T("altijd"));
    }
    else
    {
      _tprintf(_T("ERROR: No Hibernate session started.\n"));
      exit(-3);
    }
  }
  catch (StdException& er)
  {
    _tprintf(_T("Error while creating Hibernate session: %s\n"),er.GetErrorMessage().GetString());
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
  TCHAR buffer[256];
  size_t readIn = 0;
  _cgetts_s(buffer, 256, &readIn);
}

int _tmain()
{
  // initialize MFC and print and error on failure
  if(!AfxWinInit(::GetModuleHandle(NULL),NULL,::GetCommandLine(), 0))
  {
    CString command = ::GetCommandLine();
    CString message = _T("Fatal Error in TestServer: MFC initialization failed ") + command;
    _ftprintf(stderr, message + _T("\n"));
    return -3;
  }

  // Print who we are
  _tprintf(_T("CXHibernate test server program\n"));
  _tprintf(_T("===============================\n"));

  // Start our CX-Hibernate session
  StartSession();

  // Use this namespace
  CString namesp(_T("http://www.cxhibernate.org/"));
  CString url(_T("http://localhost:1959/CXServer/"));

  // Create a server
  CXServer cxserver(g_session
                   ,_T("CXTestServer")
                   ,_T("C:\\WWW\\")
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
    _tprintf(_T("Could not start the CX-Hibernate test server!\n"));
    _tprintf(_T("Check the URL reservations on your machine!\n"));
    exit(-5);
  }

  _tprintf(_T("\n")
        _T("Server running....\n")
        _T("Waiting to be called by test clients...\n")
        _T("\n"));
  // Wait for key to occur
  WaitForKey();

  // Try stopping the server
  _tprintf(_T("Stopping the server\n"));
  cxserver.Stop();

  // See if the server is indeed in stopped state
  _tprintf(_T("The server is %s\n"), cxserver.IsRunning() ? _T("still running!\n") : _T("stopped.\n"));

  // As a last step, close our Hibernate session
  CloseSession();

  return 0;
}

