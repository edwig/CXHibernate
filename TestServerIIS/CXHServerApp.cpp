/////////////////////////////////////////////////////////////////////////////////
//
// SourceFile: CXHServerApp.cpp
//
// Marlin Server: Internet server/client
// 
// Copyright (c) 2015-2022 ir. W.E. Huisman
// All rights reserved
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files(the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions :
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//
#include "stdafx.h"
#include "CXHServerApp.h"
#include "CXSession.h"
#include "MarlinModule.h"
#include "SQLComponents.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// Total number of errors registered while testing
int g_errors = 0;

using namespace SQLComponents;

const char* APPLICATION_NAME     = "HibernateTestServerIIS";
const char* PRODUCT_NAME         = "CXHibernate";
const char* PRODUCT_DISPLAY_NAME = "CXHibernate Test Server";
const char* PRODUCT_COPYRIGHT    = "CXHibernate (c) 2019";
const char* PRODUCT_VERSION      = "1.1.0";
const char* PRODUCT_MESSAGES_DLL = "CXHibernateMessages.dll";
const char* PRODUCT_SITE         = "/CXHibernate/";

// The app Factory
CXHServerAppFactory CXHAppFactory;

// This is the ServerApp of the IIS server variant (running in W3SVC)

//////////////////////////////////////////////////////////////////////////
//
// The test server app
//
//////////////////////////////////////////////////////////////////////////

CXHServerApp::CXHServerApp(IHttpServer* p_iis,CString p_webroot,CString p_appName)
             :ServerApp(p_iis,p_webroot,p_appName)
{
}

CXHServerApp::~CXHServerApp()
{
}

void
CXHServerApp::InitInstance()
{
  // First always call the main class 
  // Must init for the HTTPServer and other objects
  ServerApp::InitInstance();

  CString name     = "CXHibernate";
  CString contract = "http://www.cxhibernate.org/";
  CString url("http://localhost:1960/CXServer/");

  StartSession();

  m_hibernateServer = new CXServer(m_session,m_applicationName,m_webroot,url,PrefixType::URLPRE_Strong,contract,10);
  m_hibernateServer->SetHTTPServer(m_httpServer);
  m_hibernateServer->SetLogAnalysis(m_logfile);
  m_hibernateServer->SetLogLevel(m_logLevel);
  m_hibernateServer->RegisterOperations();
  m_hibernateServer->SetGenerateWsdl(false);

  if(m_hibernateServer)
  {
    m_httpServer->RegisterService(m_hibernateServer);
  }

  // Try running the service
  if (m_hibernateServer->RunService())
  {
    qprintf("WebServiceServer [%s] is now running OK\n",m_applicationName);
    qprintf("Running contract      : %s\n",             contract);
    qprintf("For WSDL download use : %s%s.wsdl\n",      url, name);
    qprintf("For interface page use: %s%s%s\n",         url, name,m_hibernateServer->GetServicePostfix());
    qprintf("\n");

    // Instance is now running
    m_running = true;
  }
  else
  {
    xerror();
    qprintf("ERROR Starting WebServiceServer for: %s\n", contract.GetString());
    qprintf("ERROR Reported by the server: %s\n",        m_hibernateServer->GetErrorMessage().GetString());
  }
}

bool
CXHServerApp::LoadSite(IISSiteConfig& p_config)
{
  // Already done in the InitInstance
  return true;
}

void
CXHServerApp::ExitInstance()
{
  if (m_running)
  {
    // Report all tests
    ReportAfterTesting();

    // Stopped running
    m_running = false;
  }

  // Always call the ExitInstance of the main class last
  // Will destroy the HTTPServer and all other objects
  ServerApp::ExitInstance();
}

// Start a hibernate session for our default model
void 
CXHServerApp::StartSession()
{
  // Init in the correct language
  InitSQLComponents(Language::LN_ENGLISH);

  try
  {
    m_session = hibernate.CreateSession();
    if (m_session)
    {
      // Where we have our filestore
      m_session->SetFilestore("C:\\WWW\\Testing");

      // We are a database session
      m_session->SetDatabaseConnection("Testing", "sysdba", "altijd");
    }
    else
    {
      xerror();
      qprintf("ERROR: No Hibernate session started.\n");
      exit(-3);
    }
  }
  catch (StdException& er)
  {
    xerror();
    qprintf("Error while creating Hibernate session: %s\n", er.GetErrorMessage());
    exit(-3);
  }
}



bool
CXHServerApp::CorrectlyStarted()
{
  if(ServerApp::CorrectlyStarted() == false)
  {
    qprintf("ServerApp incorrectly started. Review your program logic");
    return false;
  }
  return true;
}

void
CXHServerApp::ReportAfterTesting()
{
  // SUMMARY OF ALL THE TEST
  // ---- "---------------------------------------------- - ------
  qprintf("TOTAL number of errors after all tests are run : %d", g_errors);
}

//////////////////////////////////////////////////////////////////////////
//
// SERVICE ROUTINES FOR TESTING
//
//////////////////////////////////////////////////////////////////////////

bool doDetails = false;

// Increment the total global number of errors while testing
void xerror()
{
  ++g_errors;
}

// Suppressed printing. Only print when doDetails = true
// Any testing module can turn doDetails to 'on' or 'off'
void xprintf(const char* p_format, ...)
{
  if (doDetails)
  {
    va_list varargs;
    va_start(varargs, p_format);
    CString string;
    string.FormatV(p_format, varargs);
    va_end(varargs);

    string.TrimRight("\n");

    g_analysisLog->AnalysisLog("Testing details", LogType::LOG_INFO, false, string);
  }
}

// Printing to the logfile for test results
// "String to the logfile"   -> Will be printed to logfile including terminating newline
// "Another string <+>"      -> Will be printed WITHOUT terminating newline
void qprintf(const char* p_format, ...)
{
  static CString stringRegister;

  // Print variadic arguments
  va_list varargs;
  va_start(varargs, p_format);
  CString string;
  string.FormatV(p_format, varargs);
  va_end(varargs);

  // See if we must just register the string
  string.TrimRight("\n");
  if (string.Right(3) == "<+>")
  {
    stringRegister += string.Left(string.GetLength() - 3);
    return;
  }

  // Print the result to the logfile as INFO
  string = stringRegister + string;
  g_analysisLog->AnalysisLog("Testing", LogType::LOG_INFO, false, string);
  stringRegister.Empty();
}

//////////////////////////////////////////////////////////////////////////
//
// SERVER APP FACTORY
// Override to create your own "ServerApp derived class object"
//
//////////////////////////////////////////////////////////////////////////

CXHServerAppFactory::CXHServerAppFactory()
{
  if(appFactory)
  {
    TRACE("You can only have ONE singleton ServerAppFactory in your program logic");
    ASSERT(FALSE);
  }
  else
  {
    appFactory = (ServerAppFactory*) this;
  }
}

CXHServerApp*
CXHServerAppFactory::CreateServerApp(IHttpServer* p_iis
                                    ,const char*  p_webroot
                                    ,const char*  p_appName)
{
  return new CXHServerApp(p_iis, p_webroot, p_appName);
}

