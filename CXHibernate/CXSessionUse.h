#pragma once

typedef enum _cxh_session_use
{
  SESS_Use        // Use database connection, must conform to the config file
 ,SESS_None       // Do not use a database connection (filestore / internet)
 ,SESS_Create     // Create a new (test) database
}
CXHSessionUse;


CXHSessionUse CXStringToSessionUse(CString p_use);

CString CXSessionUseToString(CXHSessionUse p_useo);
