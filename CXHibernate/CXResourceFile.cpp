////////////////////////////////////////////////////////////////////////
//
// File: CXResourceFile.cpp
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
#include "CXResourceFile.h"
#include "StdException.h"


//////////////////////////////////////////////////////////////////////////
//
// Configuration files can be compiled into the resource of your app
// so you do not have to re-distribute the files with the *.exe
//

// In the "application.rc" file
/////////////////////////////////////////////////////////////////////////////
//
// CFG
//
// IDR_RESOURCENAME           CFG                     "tablename.hbm.xml"

// In the "resource.h" file of your application
//////////////////////////////////////////////////////////////////////////
// #define IDR_RESOURCENAME   80
//

static char g_staticAddress;

// Load a compiled text file from our resources
CString
CXHReadResourceData(int p_resId,CString type)
{
  HMODULE module = NULL;
  GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | 
                    GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT
                   ,static_cast<LPCTSTR>(&g_staticAddress)
                   ,&module);


  // Getting the resource handle
  HRSRC list = FindResource(module,MAKEINTRESOURCE(p_resId),type);
  if (list == 0)
  {
    CString error;
    error.Format("Cannot find the resource [%d] of type [%s]", p_resId, type);
    throw new StdException(error);
  }

  // Load the resource
  HGLOBAL glist = LoadResource(module, list);
  if (glist == 0)
  {
    CString error;
    error.Format("Cannot load the resource [%d] of type [%s]", p_resId, type);
    throw error;
  }

  // Getting the size
  DWORD dwSize = SizeofResource(module, list);
  if (dwSize == 0)
  {
    return CString();
  }

  // Lock resource for a pointer
  char const* ptr = (char*)LockResource(glist);

  // Copy as a string
  CString result(ptr, dwSize);

  // unlock the resource
  UnlockResource(glist);

  // Ready!
  return result;
}
