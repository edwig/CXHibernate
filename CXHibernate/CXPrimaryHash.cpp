////////////////////////////////////////////////////////////////////////
//
// File: CXPrimaryHash.cpp
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
// Last Revision:   17-06-2018
// Version number:  0.9.0
//
#include "stdafx.h"
#include "CXPrimaryHash.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// A 'primary-key-hash' is the translation of all fields in to strings
// but fields are separated by the ASCII SOT (Start-Of-Text)
// This function is used to store and find the objects in the object caches
//
CString CXPrimaryHash(VariantSet& p_primary)
{
  CString hash;
  CString one;
  bool firstseen = false;

  for(auto& var : p_primary)
  {
    var->GetAsString(one);
    if(firstseen)
    {
      hash += '\002'; // Add ASCI Start-Of-Text (STX)
    }
    firstseen = true;

    hash += one;
  }
  return hash;
}
