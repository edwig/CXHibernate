////////////////////////////////////////////////////////////////////////
//
// File: Detail_cxh.cpp
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
BEGIN_XML_SERIALIZE(Detail,CXObject)
    CXO_XML_SERIALIZE(m_id,         _T("id"));
    CXO_XML_SERIALIZE(m_mast_id,    _T("mast_id"));
    CXO_XML_SERIALIZE(m_line,       _T("line"));
    CXO_XML_SERIALIZE(m_description,_T("description"));
    CXO_XML_SERIALIZE(m_amount,     _T("amount"));
END_XML_SERIALIZE

// Read the contents of an object from a SOAPMessage
BEGIN_XML_DESERIALIZE(Detail,CXObject)
  CXO_XML_DESERIALIZE(m_id,         _T("id"));
  CXO_XML_DESERIALIZE(m_mast_id,    _T("mast_id"));
  CXO_XML_DESERIALIZE(m_line,       _T("line"));
  CXO_XML_DESERIALIZE(m_description,_T("description"));
  CXO_XML_DESERIALIZE(m_amount,     _T("amount"));
END_XML_DESERIALIZE

//////////////////////////////////////////////////////////////////////////

BEGIN_DBS_SERIALIZE(Detail,CXObject)
    CXO_DBS_SERIALIZE(m_id,         _T("id"));
    CXO_DBS_SERIALIZE(m_mast_id,    _T("mast_id"));
    CXO_DBS_SERIALIZE(m_line,       _T("line"));
    CXO_DBS_SERIALIZE(m_description,_T("description"));
    CXO_DBS_SERIALIZE(m_amount,     _T("amount"));
END_DBS_SERIALIZE


BEGIN_DBS_DESERIALIZE(Detail,CXObject)
  CXO_DBS_DESERIALIZE(m_id,         _T("id"));
  CXO_DBS_DESERIALIZE(m_mast_id,    _T("mast_id"));
  CXO_DBS_DESERIALIZE(m_line,       _T("line"));
  CXO_DBS_DESERIALIZE(m_description,_T("description"));
  CXO_DBS_DESERIALIZE(m_amount,     _T("amount"));
END_DBS_DESERIALIZE

BEGIN_DESERIALIZE_GENERATOR(Detail)
  CXO_DBS_DESERIALIZE(m_id, _T("id"));
END_DESERIALIZE_GENERATOR

// Static factory to create a new object if this class
DEFINE_CXO_FACTORY(Detail);
