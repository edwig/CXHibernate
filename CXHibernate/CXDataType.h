////////////////////////////////////////////////////////////////////////
//
// File: CXDataType.h
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
#include <sqlext.h>

// One word shorthands for elementary datatypes of C++
using uint    = unsigned int;
using ushort  = unsigned short;
using ulong   = unsigned long;
using uchar   = unsigned char;
using bigint  = __int64;
using ubigint = unsigned __int64;

// Definition array of all CX data types
typedef struct _cxtypes
{
  const char* m_name;
  const int   m_type;
}
CXType;

int CXStringToDatatType(CString p_datatype);

CString CXDataTypeToString(int p_datatype);

int CXConfigToDataType(CString p_datatype);

CString CXDataTypeToConfig(int p_datatype);
