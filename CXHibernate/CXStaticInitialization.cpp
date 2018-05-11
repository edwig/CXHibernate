#include "stdafx.h"
#include "CXStaticInitialization.h"
#include "CXHibernate.h"

static unsigned cxpf = 0;    // number of destructors we need to call  
PF pfx[MAX_CLASSES];    // pointers to destructors.  

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

