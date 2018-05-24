#include "stdafx.h"
#include "CXSessionUse.h"

CXHSessionUse 
CXStringToSessionUse(CString p_use)
{
  if(p_use == "use")    return SESS_Use;
  if(p_use == "none")   return SESS_None;
  if(p_use == "create") return SESS_Create;
  return SESS_None;
}

CString 
CXSessionUseToString(CXHSessionUse p_use)
{
  switch(p_use)
  {
    case SESS_Use:    return "use";
    case SESS_None:   return "none";
    case SESS_Create: return "create";
  }
  return "none";
}