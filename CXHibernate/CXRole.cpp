#include "stdafx.h"
#include "CXRole.h"


CXHRole 
CXStringToRole(CString p_role)
{
  if(p_role.CompareNoCase("database_role")  == 0) return CXH_Database_role;
  if(p_role.CompareNoCase("filestore_role") == 0) return CXH_Filestore_role;
  if(p_role.CompareNoCase("internet_role")  == 0) return CXH_Internet_role;
  return (CXHRole) 0;
}

CString 
CXRoleToString(CXHRole p_role)
{
  switch (p_role)
  {
    case CXH_Database_role:  return "database_role";
    case CXH_Filestore_role: return "filestore_role";
    case CXH_Internet_role:  return "internet_role";
  }
  return "";
}
