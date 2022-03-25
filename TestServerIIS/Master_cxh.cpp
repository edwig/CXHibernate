////////////////////////////////////////////////////////////////////////
//
// File: CXMaster.cpp
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
// Last Revision:   22-04-2018
// Version number:  0.0.1
//
#include "stdafx.h"
#include "Master.h"
#include <CXTable.h>
#include <SQLRecord.h>
#include <SOAPMessage.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// Bring the contents of the class to a SOAPMessage 
BEGIN_XML_SERIALIZE(Master,CXObject)
    CXO_XML_SERIALIZE(m_id,         "id");
    CXO_XML_SERIALIZE(m_invoice,    "invoice");
    CXO_XML_SERIALIZE(m_description,"description");
    CXO_XML_SERIALIZE(m_total,      "total");
END_XML_SERIALIZE

// Read the contents of an object from a SOAPMessage or a SQLRecord
BEGIN_XML_DESERIALIZE(Master,CXObject)
  CXO_XML_DESERIALIZE(m_id,         "id");
  CXO_XML_DESERIALIZE(m_invoice,    "invoice");
  CXO_XML_DESERIALIZE(m_description,"description");
  CXO_XML_DESERIALIZE(m_total,      "total");
END_XML_DESERIALIZE

BEGIN_DBS_SERIALIZE(Master,CXObject)
    CXO_DBS_SERIALIZE(m_id,         "id");
    CXO_DBS_SERIALIZE(m_invoice,    "invoice");
    CXO_DBS_SERIALIZE(m_description,"description");
    CXO_DBS_SERIALIZE(m_total,      "total");
END_DBS_SERIALIZE

BEGIN_DBS_DESERIALIZE(Master,CXObject)
  CXO_DBS_DESERIALIZE(m_id,         "id");
  CXO_DBS_DESERIALIZE(m_invoice,    "invoice");
  CXO_DBS_DESERIALIZE(m_description,"description");
  CXO_DBS_DESERIALIZE(m_total,      "total");
END_DBS_DESERIALIZE

BEGIN_DESERIALIZE_GENERATOR(Master)
  CXO_DBS_DESERIALIZE(m_id, "id");
END_DESERIALIZE_GENERATOR

//////////////////////////////////////////////////////////////////////////

// Create our new object factory
DEFINE_CXO_FACTORY(Master);
