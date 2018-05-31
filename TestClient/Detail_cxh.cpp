////////////////////////////////////////////////////////////////////////
//
// File: Detail_cxh.cpp
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
#include "stdafx.h"
#include "Detail.h"
#include <SQLRecord.h>
#include <SOAPMessage.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////////////

// Bring the contents of the class to a SOAPMessage 
BEGIN_XML_SERIALIZE(Detail)
    CXO_XML_SERIALIZE(int,    m_id,         "id",         XDT_Integer);
    CXO_XML_SERIALIZE(int,    m_mast_id,    "mast_id",    XDT_Integer);
    CXO_XML_SERIALIZE(int,    m_line,       "line",       XDT_Integer);
    CXO_XML_SERIALIZE(CString,m_description,"description",XDT_String);
    CXO_XML_SERIALIZE(bcd,    m_amount,     "amount",     XDT_Decimal);
END_XML_SERIALIZE

// Read the contents of an object from a SOAPMessage
BEGIN_XML_DESERIALIZE(Detail)
  CXO_XML_DESERIALIZE(int,    m_id,         "id",         XDT_Integer);
  CXO_XML_DESERIALIZE(int,    m_mast_id,    "mast_id",    XDT_Integer);
  CXO_XML_DESERIALIZE(int,    m_line,       "line",       XDT_Integer);
  CXO_XML_DESERIALIZE(CString,m_description,"description",XDT_String);
  CXO_XML_DESERIALIZE(bcd,    m_amount,     "amount",     XDT_Decimal);
END_XML_DESERIALIZE

//////////////////////////////////////////////////////////////////////////

BEGIN_DBS_SERIALIZE(Detail)
    CXO_DBS_SERIALIZE(int,    m_id,         "id",         XDT_Integer);
    CXO_DBS_SERIALIZE(int,    m_mast_id,    "mast_id",    XDT_Integer);
    CXO_DBS_SERIALIZE(int,    m_line,       "line",       XDT_Integer);
    CXO_DBS_SERIALIZE(CString,m_description,"description",XDT_String);
    CXO_DBS_SERIALIZE(bcd,    m_amount,     "amount",     XDT_Decimal);
END_DBS_SERIALIZE


BEGIN_DBS_DESERIALIZE(Detail)
  CXO_DBS_DESERIALIZE(int,    m_id,         "id",         XDT_Integer);
  CXO_DBS_DESERIALIZE(int,    m_mast_id,    "mast_id",    XDT_Integer);
  CXO_DBS_DESERIALIZE(int,    m_line,       "line",       XDT_Integer);
  CXO_DBS_DESERIALIZE(CString,m_description,"description",XDT_String);
  CXO_DBS_DESERIALIZE(bcd,    m_amount,     "amount",     XDT_Decimal);
END_DBS_DESERIALIZE

// Static factory to create a new object if this class
DEFINE_CXO_FACTORY(Detail);
