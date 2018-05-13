////////////////////////////////////////////////////////////////////////
//
// File: CXStaticInitialization.cpp
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
#include "CXStaticInitialization.h"
#include "CXHibernate.h"

static unsigned cxpf = 0;    // number of destructors we need to call  
static PF pfx[MAX_CLASSES];  // pointers to destructors.  

// Register the destructor of an object
// Used by the #pragma init_seg construction
int cxoexit(PF pf)
{
  if(cxpf < MAX_CLASSES)
  {
    pfx[cxpf++] = pf;
  }
  return 0;
}

#pragma     section(".ccxo$a", read)  
__declspec(allocate(".ccxo$a")) const PF InitSegStart = (PF)1;

#pragma     section(".ccxo$z",read)  
__declspec(allocate(".ccxo$z")) const PF InitSegEnd = (PF)1;

// The comparison for 0 is important.  
// For now, each section is 256 bytes. When they  
// are merged, they are padded with zeros. You  
// can't depend on the section being 256 bytes, but  
// you can depend on it being padded with zeros.  

// Call all constructors of the static classes
// in the .ccxo segment
void CXOInitializeClasses()
{
  const PF *func = &InitSegStart;
  for(++func; func < &InitSegEnd; ++func)
  {
    if(*func)
    {
      (*func)();
    }
  }
}

// Call all the destructors in the static classes
// in the .ccxo segment
void CXODestroyClasses()
{
  while(cxpf > 0)
  {
    --cxpf;
    (pfx[cxpf])();
  }
}
