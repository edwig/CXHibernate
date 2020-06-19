////////////////////////////////////////////////////////////////////////
//
// File: SQLVariantFormat.h
//
// Copyright (c) 1998-2020 ir. W.E. Huisman
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
// Version number: See SQLComponents.h
//
#pragma once
#include "SQLVariant.h"
#include "SQLDatabase.h"

namespace SQLComponents
{

// User status
#define USER_DISPLAY		   1
#define NUMBER_BUFFER_SIZE 100

// string format number and money format functions
extern bool g_locale_valutaInit;
extern char g_locale_decimalSep [];
extern char g_locale_thousandSep[];
extern char g_locale_strCurrency[];
extern int  g_locale_decimalSepLen;
extern int  g_locale_thousandSepLen;
extern int  g_locale_strCurrencyLen;

// Initialize the above global locale variables!
void InitValutaString();

class SQLVariantFormat
{
public:
  SQLVariantFormat(SQLVariant* p_variant);
  SQLVariantFormat(SQLVariant& p_variant);
  ~SQLVariantFormat();
  void        Reset();
  void        ResetValue();
  void        ReFormat();
  // All words begin with a capital
  void        StringInitCapital();
  // First letter of first word starts with a capital
  void        StringStartCapital();
  // Set current date and time
  void        SetCurrentDate();
  // Format the date
  int         FormatDate(CString p_pattern);
  // Is a constant or a numeric / IsConstanteOfNummer
  bool        IsConstantOrNumber(char p_seperator = '.');
  // Converting european values to system values
  int         StrValutaNLOmzetten(CString& p_string,bool p_enkelValuta);
  // Converting american $ values to dutch values
  int         StrValutaAMOmzetten(CString& p_string,bool p_enkelValuta);
  // Value of a string as a double
  double      StringDoubleValue();
  // Format according to user interface
  int         FormatNumber(CString p_format,bool p_currency);
  // Do math on dates
  int         DateCalculate(char p_operator,CString p_argument);

  CString     FormatVariantForSQL(SQLDatabase* p_database);
  void        SetVariant(SQLVariant* p_variant);
  void        SetFormat(CString p_format);
  void        SetUserStatus(int p_status);
  SQLVariant* GetVariant();
  int         GetUserStatus();
  CString&    GetFormat();
private:
  // Internal formatting of a number by a template
  int         FormatNumberTemplate(char *Getal,const char *strNumFormat,int p_buflen);
  // Scan naar '23-09-2012' of naar '2012-09-23' // let op "09/23/2012"  wordt niet ondersteund
  bool        GetDateFromStringVariant(SQLVariant* p_variant,CString p_format,DATE_STRUCT* p_date);
  bool        GetTimeFromStringVariant(SQLVariant* p_variant,CString p_format,TIME_STRUCT* p_date);

  // Is string a (formatted) windows number?
  bool        IsWinNumber(const CString p_string,char* p_decSeperator,char* p_thouSeperator,char* p_valuta,CString* p_newNumber = NULL);


  SQLVariant* m_variant;
  bool        m_owner;
  int         m_userStatus;
  CString     m_format;
};

inline void
SQLVariantFormat::SetVariant(SQLVariant* p_variant)
{
  if(m_owner && m_variant)
  {
    delete m_variant;
    m_variant = NULL;
  }
  if(p_variant)
  {
    m_variant = new SQLVariant(p_variant);
    m_owner   = true;
  }
}

inline SQLVariant*
SQLVariantFormat::GetVariant()
{
  return m_variant;
}

inline void
SQLVariantFormat::SetUserStatus(int p_status)
{
  m_userStatus = p_status;
}

inline int
SQLVariantFormat::GetUserStatus()
{
  return m_userStatus;
}

inline void
SQLVariantFormat::SetFormat(CString p_format)
{
  m_format = p_format;
}

inline CString&
SQLVariantFormat::GetFormat()
{
  return m_format;
}

// End of namespace
}
