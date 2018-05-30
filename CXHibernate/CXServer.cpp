////////////////////////////////////////////////////////////////////////
//
// File: CXServer.cpp
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
// Last Revision:   27-05-2018
// Version number:  0.5.0
//
#include "stdafx.h"
#include "CXServer.h"
#include "CXClass.h"
#include "CXSession.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// Implementation of the CXServer

CXServer::CXServer(CXSession* p_session
                  ,CString    p_name
                  ,CString    p_webroot
                  ,CString    p_url
                  ,PrefixType p_channelType
                  ,CString    p_targetNamespace
                  ,unsigned   p_maxThreads)
         :m_session(p_session)
         ,WebServiceServer(p_name, p_webroot, p_url, p_channelType, p_targetNamespace, p_maxThreads)
{
}

CXServer::~CXServer()
{
}

// Mapping corresponding to the AddOperation of the WSDL
// Binding ordinals to the service declarations for the On..... members
WEBSERVICE_MAP_BEGIN(CXServer)
  WEBSERVICE(CXSELECT,OnCXSelect)
  WEBSERVICE(CXINSERT,OnCXInsert)
  WEBSERVICE(CXUPDATE,OnCXUpdate)
  WEBSERVICE(CXDELETE,OnCXDelete)
WEBSERVICE_MAP_END

//////////////////////////////////////////////////////////////////////////
//
// HERE ARE THE SERVICE HANDLERS!!
// Derived from the definition above in the WEBSERVICE_MAP
//
//////////////////////////////////////////////////////////////////////////

void
CXServer::OnCXSelect(int p_code,SOAPMessage* p_message)
{
  ASSERT(p_code == CXSELECT);
  UNREFERENCED_PARAMETER(p_code);

  CString errors;
  CString actor("Client");
  SQLFilterSet filters;
  XMLElement* entity = p_message->FindElement("Entity");
  if(entity)
  {
    CString tablename = entity->GetValue();
    CXClass* theClass = m_session->FindClass(tablename);
    CXTable* table = theClass->GetTable();
    if(table)
    {
      filters = FindFilterSet(p_message);
      if(!filters.Empty())
      {
        p_message->Reset();
        try
        {
          CXResultSet set = m_session->Load(tablename,filters);
          for(auto& object : set)
          {
            AddObjectToMessage(p_message,object);
          }
          p_message->SetParameter("Result","OK");
          return;
        }
        catch(StdException* er)
        {
          actor  = "Server";
          errors = "CXSelect search for the object(s) failed: " + er->GetErrorMessage();
          er->Delete();
        }
      }
      else errors = "CXSelect must provide a minimum of 1 filter at least!";
    }
    else errors = "CXSelect entity class name is unknown to the server: " + tablename;
  }
  else errors = "CXSelect action is missing an 'Entity'";

  p_message->Reset();
  p_message->SetFault("Failed",actor,"CXSelect action",errors);
}

void
CXServer::OnCXInsert(int p_code,SOAPMessage* p_message)
{
  ASSERT(p_code == CXINSERT);
  UNREFERENCED_PARAMETER(p_code);

  CString errors;
  CString actor("Client");
  XMLElement* entity = p_message->FindElement("Entity");
  if(entity)
  {
    XMLAttribute* nm = p_message->FindAttribute(entity,"name");
    if(nm)
    {
      CString tablename = nm->m_value;
      CXClass* theClass = m_session->FindClass(tablename);
      CXTable* table = theClass->GetTable();
      if(table)
      {
        try
        {
          CreateCXO creating = theClass->GetCreateCXO();
          CXObject* object = (*creating)();
          object->SetClass(theClass);
          object->DeSerialize(*p_message,entity);
          if(m_session->Insert(object))
          {
            p_message->Reset();
            p_message->SetParameter("Result","OK");
            return;
          }
          else
          {
            actor  = "Server";
            errors = "CXInsert object storage failed";
          }
        }
        catch(StdException* er)
        {
          errors = "CXInsert de-serialization of the object failed: " + er->GetErrorMessage();
          er->Delete();
        }
      }
      else errors = "CXInsert entity class name is unknown to the server: " + tablename;
    }
    else errors = "CXInsert action: Entity is missing a 'name'";
  }
  else errors = "CXInsert action is missing an 'Entity'";

  p_message->Reset();
  p_message->SetFault("Failed",actor,"CXInsert action",errors);
}

void
CXServer::OnCXUpdate(int p_code,SOAPMessage* p_message)
{
  ASSERT(p_code == CXUPDATE);
  UNREFERENCED_PARAMETER(p_code);

  CString errors;
  CString actor("Client");
  XMLElement* entity = p_message->FindElement("Entity");
  if(entity)
  {
    XMLAttribute* nm = p_message->FindAttribute(entity,"name");
    if(nm)
    {
      CString tablename = nm->m_value;
      CXClass* theClass = m_session->FindClass(tablename);
      CXTable* table = theClass->GetTable();
      if(table)
      {
        try
        {
          // Creating a temporary object to discover our primary key
          CreateCXO creating = theClass->GetCreateCXO();
          CXObject* object = (*creating)();
          object->SetClass(theClass);
          object->DeSerialize(*p_message,entity);
          // Create copy on the stack of the primary key
          VariantSet primary = object->GetPrimaryKey();

          CXObject* realObject = m_session->Load(tablename,primary);
          if(realObject)
          {
            // Now de-serialize AGAIN, but now in the correct object
            realObject->DeSerialize(*p_message,entity);
            // Go update in the database
            if(m_session->Update(realObject))
            {
              p_message->Reset();
              p_message->SetParameter("Result","OK");
              return;
            }
            else
            {
              actor  = "Server";
              errors = "CXUpdate object storage failed";
            }
          }
          else errors = "CXUpdate object to be updated not found!";

          delete object;
        }
        catch(StdException* er)
        {
          errors = "CXUpdate de-serialization of the object failed: " + er->GetErrorMessage();
          er->Delete();
        }
      }
      else errors = "CXUpdate entity class name is unknown to the server: " + tablename;
    }
    else errors = "CXUpdate action: Entity is missing a 'name'";
  }
  else errors = "CXUpdate action is missing an 'Entity'";

  p_message->Reset();
  p_message->SetFault("Failed",actor,"CXUpdate action",errors);
}

void
CXServer::OnCXDelete(int p_code,SOAPMessage* p_message)
{
  ASSERT(p_code == CXDELETE);
  UNREFERENCED_PARAMETER(p_code);

  CString errors;
  CString actor("Client");
  XMLElement* entity = p_message->FindElement("Entity");
  if(entity)
  {
    XMLAttribute* nm = p_message->FindAttribute(entity,"name");
    if(nm)
    {
      CString tablename = nm->m_value;
      CXClass* theClass = m_session->FindClass(tablename);
      CXTable* table = theClass->GetTable();
      if(table)
      {
        try
        {
          // Creating a temporary object to discover our primary key
          CreateCXO creating = theClass->GetCreateCXO();
          CXObject* object = (*creating)();
          object->SetClass(theClass);
          object->DeSerialize(*p_message,entity);
          // Create copy on the stack of the primary key
          VariantSet primary = object->GetPrimaryKey();

          CXObject* realObject = m_session->Load(tablename,primary);
          if(realObject)
          {
            if(m_session->Delete(realObject))
            {
              p_message->Reset();
              p_message->SetParameter("Result","OK");
              return;
            }
            else
            {
              actor  = "Server";
              errors = "CXDelete object storage failed";
            }
          }
          else errors = "CXDelete object to be deleted not found!";

          delete object;
        }
        catch(StdException* er)
        {
          errors = "CXDelete de-serialization of the object failed: " + er->GetErrorMessage();
          er->Delete();
        }
      }
      else errors = "CXDelete entity class name is unknown to the server: " + tablename;
    }
    else errors = "CXDelete action: Entity is missing a 'name'";
  }
  else errors = "CXDelete action is missing an 'Entity'";

  p_message->Reset();
  p_message->SetFault("Failed",actor,"CXDelete action",errors);
}

//////////////////////////////////////////////////////////////////////////
//
// PRIVATE
//
//////////////////////////////////////////////////////////////////////////

// Find our filter set in the SOAP message
SQLFilterSet 
CXServer::FindFilterSet(SOAPMessage* p_message)
{
  SQLFilterSet filters;

  XMLElement* fselem = p_message->FindElement("Filters");
  if(fselem == nullptr)
  {
    return filters;
  }

  XMLElement* filter = p_message->FindElement(fselem,"Filter");
  if(filter == nullptr)
  {
    return filters;
  }

  // Iterate through all the filters
  while(filter)
  {
    XMLElement* column = p_message->FindElement(filter,"Column",  false);
    XMLElement* operat = p_message->FindElement(filter,"Operator",false);

    if(column && operat)
    {
      SQLOperator oper = StringToSQLOperator(operat->GetValue());
      if(oper == OP_NOP)
      {
        return SQLFilterSet();
      }

      // Create extra filter
      SQLFilter* filt = new SQLFilter(column->GetValue(),oper);

      XMLElement* value = p_message->FindElement(filter,"Value",false);
      while(value)
      {
        SQLVariant* var = new SQLVariant(value->GetValue());
        filt->AddValue(var);
        // Next value in the filter
        value = p_message->GetElementSibling(value);
      }
      // Keep the filter
      filters.AddFilter(filt);
    }
    // Next filter
    filter = p_message->GetElementSibling(filter);
  }
  return filters;
}

// Add an object to the answer of the SOAP message
void
CXServer::AddObjectToMessage(SOAPMessage* p_message,CXObject* object)
{
  // Add a new entity node
  XMLElement* entity = p_message->SetParameter("Entity","");
  p_message->SetAttribute(entity,"name",object->GetClass()->GetTable()->TableName());

  // Serialize our object to this message on this node
  object->Serialize(*p_message,entity);
}
