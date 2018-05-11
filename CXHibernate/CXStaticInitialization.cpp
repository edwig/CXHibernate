#include "stdafx.h"
#include "CXStaticInitialization.h"
#include "CXHibernate.h"

static int cxpf = 0;    // number of destructors we need to call  
PF pfx[MAX_CLASSES];    // pointers to destructors.  

// Register the destructor of an object
int cxoexit(PF pf)
{
  pfx[cxpf++] = pf;
  return 0;
}

#pragma section(".mine$a", read)  
__declspec(allocate(".mine$a")) const PF InitSegStart = (PF)1;

#pragma section(".mine$z",read)  
__declspec(allocate(".mine$z")) const PF InitSegEnd = (PF)1;

// The comparison for 0 is important.  
// For now, each section is 256 bytes. When they  
// are merged, they are padded with zeros. You  
// can't depend on the section being 256 bytes, but  
// you can depend on it being padded with zeros.  

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

void CXODestroyClasses()
{
  while(cxpf > 0)
  {
    --cxpf;
    (pfx[cxpf])();
  }
}

