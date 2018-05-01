////////////////////////////////////////////////////////////////////////
//
// File: CXServer.h
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
#pragma once
#include <WebServiceServer.h>
#include <SQLFilter.h>

class CXSession;
class CXObject;
class SOAPMessage;

// Ordinals for the services
#define CXSELECT 1
#define CXINSERT 2
#define CXUPDATE 3
#define CXDELETE 4

class CXServer : public WebServiceServer
{
public:
  CXServer(CString    p_name
          ,CString    p_webroot
          ,CString    p_url
          ,PrefixType p_channelType
          ,CString    p_targetNamespace
          ,unsigned   p_maxThreads);
  virtual ~CXServer();
protected:
  WEBSERVICE_MAP; // Using a WEBSERVICE mapping

  // Declare all our webservice call names
  // which will translate in the On.... methods
  WEBSERVICE_DECLARE(OnCXSelect)
  WEBSERVICE_DECLARE(OnCXInsert)
  WEBSERVICE_DECLARE(OnCXUpdate)
  WEBSERVICE_DECLARE(OnCXDelete)
private:
  // Find our filter set in the SOAP message
  SQLFilterSet FindFilterSet(SOAPMessage* p_message);
  // Add an object to the answer of the SOAP message
  void         AddObjectToMessage(SOAPMessage* p_message,CXObject* object);
  // Translate a string from the message to an operator
  SQLOperator  StringToSQLOperator(CString p_oper);

  // Our CXHibernate session
  CXSession* m_session { nullptr };
};
