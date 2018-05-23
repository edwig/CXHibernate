////////////////////////////////////////////////////////////////////////
//
// File: SQLError.h
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
// Last Revision:   04-02-2018
// Version number:  1.4.5
//
#pragma once

namespace SQLComponents
{

typedef struct _sql_state
{
  CString m_code;         // Standard SQLSTATE code (5 chars)
  CString m_error;        // Short error text
  CString m_explanation;  // Longer explanation of what went wrong
}
SQLState;

// Array of known sqlstates
extern SQLState sql_states[];

class SQLError
{
public:
  SQLError();
 ~SQLError();
 
  SQLState* GetSQLState(CString p_sqlState);
  CString   GetSQLStateFormatted(CString p_sqlState);
  SQLState* GetLastSQLState();
private:
  SQLState* m_lastState;  
};

inline SQLState*
SQLError::GetLastSQLState()
{ 
  return m_lastState; 
}

// End of namespace
}