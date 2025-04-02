////////////////////////////////////////////////////////////////////////
//
// File: CXServer.cpp
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
// Last version date: See CXHibernate.h
// Version number:    See CXHibernate.h
//
#include "stdafx.h"
#include "CXServer.h"
#include "CXClass.h"
#include "CXSession.h"
#include <memory>

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

// Register our services for the WSDL and the service mapper
void
CXServer::RegisterOperations()
{
  RegisterSelectOperation();
  RegisterInsertOperation();
  RegisterUpdateOperation();
  RegisterDeleteOperation();
}

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
  CString actor(_T("Client"));
  SQLFilterSet filters;
  XMLAttribute* nameAttribute = nullptr;
  XMLElement* entity = p_message->FindElement(_T("Entity"));
  if(entity)
  {
    nameAttribute = p_message->FindAttribute(entity, _T("name"));
  }
  if(nameAttribute)
  {
    CString classname = nameAttribute->m_value;
    CXClass* theClass = m_session->FindClass(classname);
    CXTable* table = theClass->GetTable();
    if(table)
    {
      FindFilterSet(p_message,filters);
      if(!filters.Empty())
      {
        p_message->Reset();
        try
        {
          CXResultSet set = m_session->Load(classname,filters);
          for(auto& object : set)
          {
            AddObjectToMessage(p_message,object);
          }
          p_message->SetParameter(_T("CXResult"),_T("OK"));
          return;
        }
        catch(StdException& er)
        {
          actor  = _T("Server");
          errors = _T("CXSelect search for the object(s) failed: ") + er.GetErrorMessage();
        }
      }
      else errors = _T("CXSelect must provide a minimum of 1 filter at least!");
    }
    else errors = _T("CXSelect entity class name is unknown to the server: ") + classname;
  }
  else errors = _T("CXSelect action is missing an 'Entity'");

  p_message->Reset();
  p_message->SetFault(_T("Failed"),actor,_T("CXSelect action"),errors);
}

void
CXServer::OnCXInsert(int p_code,SOAPMessage* p_message)
{
  ASSERT(p_code == CXINSERT);
  UNREFERENCED_PARAMETER(p_code);

  CString errors;
  CString actor(_T("Client"));
  XMLElement* entity = p_message->FindElement(_T("Entity"));
  if(entity)
  {
    XMLAttribute* nm = p_message->FindAttribute(entity,_T("name"));
    if(nm)
    {
      CString classname = nm->m_value;
      CXClass* theClass = m_session->FindClass(classname);
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

            // Place resulting object in the message
            entity = p_message->SetParameter(_T("Entity"),_T(""));
            p_message->SetAttribute(entity,_T("name"),classname);
            object->Serialize(*p_message,entity);

            // Add the "OK" sign
            p_message->SetParameter(_T("CXResult"),_T("OK"));
            return;
          }
          else
          {
            actor  = _T("Server");
            errors = _T("CXInsert object storage failed");
          }
        }
        catch(StdException& er)
        {
          errors = _T("CXInsert de-serialization of the object failed: ") + er.GetErrorMessage();
        }
      }
      else errors = _T("CXInsert entity class name is unknown to the server: ") + classname;
    }
    else errors = _T("CXInsert action: Entity is missing a 'name'");
  }
  else errors = _T("CXInsert action is missing an 'Entity'");

  p_message->Reset();
  p_message->SetFault(_T("Failed"),actor,_T("CXInsert action"),errors);
}

void
CXServer::OnCXUpdate(int p_code,SOAPMessage* p_message)
{
  ASSERT(p_code == CXUPDATE);
  UNREFERENCED_PARAMETER(p_code);

  CString errors;
  CString actor(_T("Client"));
  XMLElement* entity = p_message->FindElement(_T("Entity"));
  if(entity)
  {
    XMLAttribute* nm = p_message->FindAttribute(entity,_T("name"));
    if(nm)
    {
      CString classname = nm->m_value;
      CXClass* theClass = m_session->FindClass(classname);
      CXTable* table = theClass->GetTable();
      if(table)
      {
        try
        {
          // Creating a temporary object to discover our primary key
          CreateCXO creating = theClass->GetCreateCXO();
          std::unique_ptr<CXObject> object((*creating)());
          object->SetClass(theClass);
          object->DeSerialize(*p_message,entity);
          // Create copy on the stack of the primary key
          VariantSet primary = object->GetPrimaryKey();

          CXObject* realObject = m_session->Load(classname,primary);
          if(realObject)
          {
            // Now de-serialize AGAIN, but now in the correct object
            realObject->DeSerialize(*p_message,entity);
            // Go update in the database
            if(m_session->Update(realObject))
            {
              p_message->Reset();
              p_message->SetParameter(_T("CXResult"),_T("OK"));
              return;
            }
            else
            {
              actor  = _T("Server");
              errors = _T("CXUpdate object storage failed");
            }
          }
          else errors = _T("CXUpdate object to be updated not found!");
        }
        catch(StdException& er)
        {
          errors = _T("CXUpdate de-serialization of the object failed: ") + er.GetErrorMessage();
        }
      }
      else errors = _T("CXUpdate entity class name is unknown to the server: ") + classname;
    }
    else errors = _T("CXUpdate action: Entity is missing a 'name'");
  }
  else errors = _T("CXUpdate action is missing an 'Entity'");

  p_message->Reset();
  p_message->SetFault(_T("Failed"),actor,_T("CXUpdate action"),errors);
}

void
CXServer::OnCXDelete(int p_code,SOAPMessage* p_message)
{
  ASSERT(p_code == CXDELETE);
  UNREFERENCED_PARAMETER(p_code);

  CString errors;
  CString actor(_T("Client"));
  XMLElement* entity = p_message->FindElement(_T("Entity"));
  if(entity)
  {
    XMLAttribute* nm = p_message->FindAttribute(entity,_T("name"));
    if(nm)
    {
      CString classname = nm->m_value;
      CXClass* theClass = m_session->FindClass(classname);
      CXTable* table = theClass->GetTable();
      if(table)
      {
        try
        {
          // Creating a temporary object to discover our primary key
          CreateCXO creating = theClass->GetCreateCXO();
          std::unique_ptr<CXObject> object((*creating)());
          object->SetClass(theClass);
          object->DeSerialize(*p_message,entity);
          // Create copy on the stack of the primary key
          VariantSet primary = object->GetPrimaryKey();

          CXObject* realObject = m_session->Load(classname,primary);
          if(realObject)
          {
            if(m_session->Delete(realObject))
            {
              p_message->Reset();
              p_message->SetParameter(_T("CXResult"),_T("OK"));
              return;
            }
            else
            {
              actor  = _T("Server");
              errors = _T("CXDelete object storage failed");
            }
          }
          else errors = _T("CXDelete object to be deleted not found!");
        }
        catch(StdException& er)
        {
          errors = _T("CXDelete de-serialization of the object failed: ") + er.GetErrorMessage();
        }
      }
      else errors = _T("CXDelete entity class name is unknown to the server: ") + classname;
    }
    else errors = _T("CXDelete action: Entity is missing a 'name'");
  }
  else errors = _T("CXDelete action is missing an 'Entity'");

  p_message->Reset();
  p_message->SetFault(_T("Failed"),actor,_T("CXDelete action"),errors);
}

//////////////////////////////////////////////////////////////////////////
//
// PRIVATE
//
//////////////////////////////////////////////////////////////////////////

// Find our filter set in the SOAP message
void
CXServer::FindFilterSet(SOAPMessage* p_message,SQLFilterSet& p_filters)
{
  XMLElement* fselem = p_message->FindElement(_T("Filters"));
  if(fselem == nullptr)
  {
    return;
  }

  XMLElement* filter = p_message->FindElement(fselem,_T("Filter"));
  if(filter == nullptr)
  {
    return;
  }

  // Iterate through all the filters
  while(filter)
  {
    XMLElement* column = p_message->FindElement(filter,_T("Column"),  false);
    XMLElement* operat = p_message->FindElement(filter,_T("Operator"),false);

    if(column && operat)
    {
      SQLOperator oper = StringToSQLOperator(operat->GetValue());
      if(oper == OP_NOP)
      {
        return;
      }

      // Create extra filter
      SQLFilter filt(column->GetValue(),oper);

      XMLElement* value = p_message->FindElement(filter,_T("Value"),false);
      while(value)
      {
        SQLVariant var(value->GetValue());
        filt.AddValue(&var);
        // Next value in the filter
        value = p_message->GetElementSibling(value);
      }
      // Keep the filter
      p_filters.AddFilter(&filt);
    }
    // Next filter
    filter = p_message->GetElementSibling(filter);
  }
}

// Add an object to the answer of the SOAP message
void
CXServer::AddObjectToMessage(SOAPMessage* p_message,CXObject* object)
{
  // Add a new entity node
  XMLElement* param  = p_message->GetParameterObjectNode();
  XMLElement* entity = p_message->AddElement(param,_T("Entity"),XDT_String,_T(""));
  p_message->SetAttribute(entity,_T("name"),object->GetClass()->GetTable()->GetTableName());

  // Serialize our object to this message on this node
  object->Serialize(*p_message,entity);
}

//////////////////////////////////////////////////////////////////////////
//
// Register all operations
//
//////////////////////////////////////////////////////////////////////////

void
CXServer::RegisterSelectOperation()
{
  CString request(_T("CXH_Select"));
  CString response = request + _T("Response");

  SOAPMessage input (m_targetNamespace,request);
  SOAPMessage output(m_targetNamespace,response);

  // Entity with a filter
  XMLElement* entity  = input.AddElement(NULL,   _T("Entity"), WSDL_Mandatory|XDT_String,_T(""));
  XMLElement* filters = input.AddElement(entity, _T("Filters"),WSDL_Mandatory|WSDL_OneMany|XDT_String,_T(""));
  XMLElement* filter  = input.AddElement(filters,_T("Filter"), WSDL_Mandatory|XDT_String,_T(""));
  // Filter must contain these
  input.AddElement(filter,_T("Column"),  WSDL_Mandatory|XDT_String,_T("string"));
  input.AddElement(filter,_T("Operator"),WSDL_Mandatory|XDT_String,_T("string"));
  input.AddElement(filter,_T("Value"),   WSDL_Optional |XDT_String,_T("string"));

  // Returns an entity (Unspecified)
  output.AddElement(NULL,_T("Entity"),   WSDL_Optional |XDT_String,_T("string"));

  AddOperation(CXSELECT,request,&input,&output);
}

void
CXServer::RegisterInsertOperation()
{
  CString request(_T("CXH_Insert"));
  CString response = request + _T("Response");

  SOAPMessage input (m_targetNamespace,request);
  SOAPMessage output(m_targetNamespace,response);

  // Entity (Unspecified
  input .AddElement(NULL,_T("Entity"),WSDL_Mandatory|XDT_String,_T(""));
  // Returns an entity (Unspecified)
  output.AddElement(NULL,_T("Entity"),WSDL_Optional |XDT_String,_T("string"));

  AddOperation(CXINSERT,request,&input,&output);
}

void
CXServer::RegisterUpdateOperation()
{
  CString request(_T("CXH_Update"));
  CString response = request + _T("Response");

  SOAPMessage input (m_targetNamespace,request);
  SOAPMessage output(m_targetNamespace,response);

  // Entity (Unspecified
  input .AddElement(NULL,_T("Entity"),WSDL_Mandatory|XDT_String,_T(""));
  // Returns an entity (Unspecified)
  output.AddElement(NULL,_T("Entity"),WSDL_Optional |XDT_String,_T("string"));

  AddOperation(CXUPDATE,request,&input,&output);
}

void
CXServer::RegisterDeleteOperation()
{
  CString request(_T("CXH_Delete"));
  CString response = request + _T("Response");

  SOAPMessage input (m_targetNamespace,request);
  SOAPMessage output(m_targetNamespace,response);

  // Entity (Unspecified
  input .AddElement(NULL,_T("Entity"),WSDL_Mandatory|XDT_String,_T(""));
  // Returns an entity (Unspecified)
  output.AddElement(NULL,_T("Entity"),WSDL_Optional |XDT_String,_T("string"));

  AddOperation(CXDELETE,request,&input,&output);
}
