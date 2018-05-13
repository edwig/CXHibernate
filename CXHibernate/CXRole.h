#pragma once

// Still more roles to come
typedef enum _cxh_roles
{
  CXH_Database_role = 1
 ,CXH_Filestore_role
 ,CXH_Internet_role
}
CXHRole;

CXHRole CXStringToRole(CString p_role);

CString CXRoleToString(CXHRole p_role);
