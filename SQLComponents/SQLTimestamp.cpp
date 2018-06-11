////////////////////////////////////////////////////////////////////////
//
// File: SQLTimestamp.cpp
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
// Last Revision:   28-05-2018
// Version number:  1.5.0
//
#include "StdAfx.h"
#include "SQLComponents.h"
#include "SQLTimestamp.h"
#include "SQLTime.h"
#include "SQLDate.h"
#include "SQLInterval.h"
#include "SQLDatabase.h"
#include "SQLLanguage.h"
#include <math.h>
#include <strstream>
#include <ctime>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace SQLComponents
{

SQLTimestamp::SQLTimestamp()
{
  SetNull();
}

// Construct from an exact year,month,day,hour,min,sec
//
SQLTimestamp::SQLTimestamp(int p_year,int p_month,int p_day,
                           int p_hour,int p_min,  int p_sec,
                           int p_fraction /*=0*/)
{
  SetTimestamp(p_year,p_month,p_day,p_hour,p_min,p_sec,p_fraction);
}

// Construct from another timestamp
//
SQLTimestamp::SQLTimestamp(const SQLTimestamp& p_timestamp)
             :m_value    (p_timestamp.m_value)
             ,m_timestamp(p_timestamp.m_timestamp)
             ,m_fraction (p_timestamp.m_fraction)
{
}

// Construct from a date and a time
// Fraction not possible
SQLTimestamp::SQLTimestamp(const SQLDate& p_date, const SQLTime& p_time)
{
  if(p_date.IsNull() || p_time.IsNull())
  {
    SetNull();
  }
  else
  {
    SetTimestamp(p_date.Year(), p_date.Month(), p_date.Day(),
                 p_time.Hour(), p_time.Minute(),p_time.Second());
  }
}

// Construct from a timestamp value
// Used to construct from a stored value 
SQLTimestamp::SQLTimestamp(StampValue p_value,int p_fraction /*=0*/)
             :m_value(p_value)
             ,m_fraction(p_fraction)
{
  Normalise();
}

// Construct from a string (e.g. from a user interface)
//
SQLTimestamp::SQLTimestamp(const CString& p_string)
{
  SetTimestamp(p_string);
}

// Construct from a SQL TIMESTAMP_STRUCT
//
SQLTimestamp::SQLTimestamp(TIMESTAMP_STRUCT p_stamp)
{
  if(p_stamp.year == 0 && p_stamp.month == 0 && p_stamp.day == 0)
  {
    // Most likely a database NULL value
    SetNull();
  }
  else
  {
    SetTimestamp(p_stamp.year,p_stamp.month, p_stamp.day,
                 p_stamp.hour,p_stamp.minute,p_stamp.second,
                 p_stamp.fraction);
  }
}

SQLTimestamp::~SQLTimestamp()
{
}

void
SQLTimestamp::Init(const CString& p_string)
{
  ParseMoment(p_string);
}

void
SQLTimestamp::SetTimestamp(const CString& p_string)
{
  ParseMoment(p_string);
}

// Set (change) the time
void
SQLTimestamp::SetTimestamp(int p_year,int p_month ,int p_day
                          ,int p_hour,int p_minute,int p_second
                          ,int p_fraction /*=0*/)
{
  m_timestamp.m_year   = (short) p_year;
  m_timestamp.m_month  = (char)  p_month;
  m_timestamp.m_day    = (char)  p_day;
  m_timestamp.m_hour   = (char)  p_hour;
  m_timestamp.m_minute = (char)  p_minute;
  m_timestamp.m_second = (char)  p_second;
  m_fraction = p_fraction;

  RecalculateValue();
}

void
SQLTimestamp::SetNull()
{
  m_value = ((StampValue)-(MJD_EPOCH + 1)) * SECONDS_PER_DAY;
  // 1 second before 0 JD (1 januari 4713 BC in the Julian Calendar)
  m_timestamp.m_year   = -4714;
  m_timestamp.m_month  = 12;
  m_timestamp.m_day    = 31;
  m_timestamp.m_hour   = 0;
  m_timestamp.m_minute = 0;
  m_timestamp.m_second = 0;
  m_fraction = 0;
}

bool
SQLTimestamp::IsNull() const
{
  return (m_value == ((StampValue)-(MJD_EPOCH + 1)) * SECONDS_PER_DAY);
}

bool
SQLTimestamp::Valid() const
{
  return (m_value != ((StampValue)-(MJD_EPOCH + 1)) * SECONDS_PER_DAY);
}

void
SQLTimestamp::RecalculateValue()
{
  // Validate timestmap first;
  Validate();

  int year  = m_timestamp.m_year;
  int month = m_timestamp.m_month;
  int day   = m_timestamp.m_day;

  // Check on Gregorian definition of months
  // Check on leapyear and days in the month
  bool leapYear = ((year & 3) == 0) &&
                  ((year % 100) != 0 || (year % 400) == 0);

  long daysInMonth = g_daysInTheMonth[month] - g_daysInTheMonth[month-1] +
                     ((leapYear && day == 29 && month == 2) ? 1 : 0);

  if (day <= 0 || day > daysInMonth)
  {
    SetNull();
    CString error;
    error.Format("Day of the month must be between 1 and %d inclusive.",daysInMonth);
    throw new StdException(error);
  }
  // Calculate the Astronomical Julian Day Number (JD)
  // Method P.D-Smith: Practical Astronomy
  // Page 9: Paragraph 4: Julian day numbers.
  // See alsoo: Robin M. Green: Spherical Astronomy, page 250
  if(month < 3)
  {
    month += 12;
    --year;
  }
  long gregorianA = 0;
  long gregorianB = 0;
  long factorC, factorD;
  if(year > 1582)
  {
    gregorianA = year / 100;
    gregorianB = 2 - gregorianA + (gregorianA / 4);
  }
  factorC = (long) (365.25  * (double)year);
  factorD = (long) (30.6001 * (double)(month + 1));
  // The correction factor (Modified JD) 
  // Falls on 16 november 1858 12:00 hours (noon), 
  // so subtract 679006 (17 november 1858 00:00:00 hour)
  m_value  = gregorianB + factorC + factorD + day - 679006;
  m_value *= SECONDS_PER_DAY;
  m_value += m_timestamp.m_hour   * SECONDS_PER_HOUR   +
             m_timestamp.m_minute * SECONDS_PER_MINUTE +
             m_timestamp.m_second;
}

void
SQLTimestamp::Normalise()
{
  long gregorianA = 0;
  long gregorianB = 0;
  long factorC = 0;
  long factorD = 0;
  long factorE = 0;
  long factorG = 0;

  // Calculate Civil Day from the Modified Juliaanse Day Nummer (MJD)
  // Method P.D-Smith: Practical Astronomy
  // Page 11: Paragraph 5: Converting Julian day number to the calendar date
  // See alsoo Robin M. Green: Spherical Astronomy, page 250 and next

  // Correction factor is MJD (2,400,000.5) + 0.5 (17 nov 1858 instead of 16 nov 12:00 hours)
  double JD = (double)((m_value / SECONDS_PER_DAY) + 2400001);
  if(JD > 2299160)
  {
    gregorianA = (long) ((JD - 1867216.25) / 36524.25);
    gregorianB = (long) (JD + 1 + gregorianA - (gregorianA / 4));
  }
  else
  {
    gregorianB = (long) JD;
  }
  factorC = gregorianB + 1524;
  factorD = (long) (((double)factorC - 122.1) / 365.25);
  factorE = (long)  ((double)factorD * 365.25);
  factorG = (long) (((double)(factorC - factorE)) / 30.6001);
  m_timestamp.m_day   = (char)   (factorC - factorE - (int)((double)factorG * 30.6001));
  m_timestamp.m_month = (char)  ((factorG > 13) ? factorG - 13 : factorG - 1);
  m_timestamp.m_year  = (short) ((m_timestamp.m_month > 2) ? factorD - 4716 : factorD - 4715);

  long seconden        =         m_value  % SECONDS_PER_DAY;
  m_timestamp.m_hour   = (char) (seconden / SECONDS_PER_HOUR);
  int rest             =         seconden % SECONDS_PER_HOUR;
  m_timestamp.m_minute = (char) (rest     / SECONDS_PER_MINUTE);
  m_timestamp.m_second = (char) (rest     % SECONDS_PER_MINUTE);
  // Fraction not set from normalized value
  m_fraction = 0;

  // Validate our results
  Validate();
}

void
SQLTimestamp::Validate()
{
  // Validate by ODBC definition 
  // Timestamp >= { ts '1-1-1 0:0:0'}
  // Timestamp <= { ts '9999-31-12 23:59:61' }

  if (m_timestamp.m_year <= 0 || m_timestamp.m_year >= 10000)
  {
    SetNull();
    throw new StdException("Year must be between 1 and 9999 inclusive.");
  }
  if (m_timestamp.m_month <= 0 || m_timestamp.m_month >= 13)
  {
    SetNull();
    throw new StdException("Month must be between 1 and 12 inclusive.");
  }
  if (m_timestamp.m_hour < 0 || m_timestamp.m_hour >= 24)
  {
    SetNull();
    throw new StdException("Hour must be between 0 and 23 inclusive.");
  }
  if (m_timestamp.m_minute < 0 || m_timestamp.m_minute >= 60)
  {
    SetNull();
    throw new StdException("Minute must be between 0 and 59 inclusive.");
  }
  if (m_timestamp.m_second < 0 || m_timestamp.m_second >= 62)
  {
    SetNull();
    throw new StdException("Number of seconds must be between 0 and 61 inclusive.");
  }
  if (m_fraction < 0 || m_fraction > NANOSECONDS_PER_SEC)
  {
    SetNull();
    throw new StdException("Fraction of seconds must be between 0 and 999,999,999");
  }
}

SQLTimestamp
SQLTimestamp::CurrentTimestamp(bool p_fraction /*=false*/)
{
  // Getting the current date-and-time
  SYSTEMTIME sys;
  ::GetLocalTime(&sys);

  long nanoseconds = 0;
  
  if(p_fraction)
  {
    // Getting high-resolution time in 100-nanosecond resolution
    FILETIME ftime;
    ::GetSystemTimeAsFileTime(&ftime);
    nanoseconds = (ftime.dwLowDateTime % 10000000);
  }
  // Construct timestamp
  SQLTimestamp stamp(sys.wYear,sys.wMonth, sys.wDay
                    ,sys.wHour,sys.wMinute,sys.wSecond
                    ,100 * nanoseconds);
  return stamp;
}

const SQLTimestamp&
SQLTimestamp::FarInTheFuture()
{
  // Largest allowed timestamp by ODBC definition
  static SQLTimestamp future(9999,12,31,23,59,59);
  return future;
}

const SQLTimestamp&
SQLTimestamp::FarInThePast()
{
  // Smallest allowed timestamp by ODBC definition
  // is 1 januari 1 at midnight
  // AND STRANGLY IT IS NOT:
  // January 1st 4713 BC at noon (12:00) is the fundamental Epoch
  // of the Astronomical Emphemeris (Julian Day = 0)
  static SQLTimestamp past(1,1,1,0,0,0);
  return past;
}

int
SQLTimestamp::Year() const
{
  return Valid() ? m_timestamp.m_year : 0;
}

int
SQLTimestamp::Month() const
{
  return Valid() ? m_timestamp.m_month : 0;
}

int
SQLTimestamp::Day() const
{
  return Valid() ? m_timestamp.m_day : 0;
}

int
SQLTimestamp::Hour() const
{
  return Valid() ? m_timestamp.m_hour : 0;
}

int
SQLTimestamp::Minute() const
{
  return Valid() ? m_timestamp.m_minute : 0;
}

int
SQLTimestamp::Second() const
{
  return Valid() ? m_timestamp.m_second : 0;
}

int
SQLTimestamp::Fraction() const
{
  return Valid() ? m_fraction : 0;
}

// Day number of the week
int
SQLTimestamp::WeekDay() const
{
  // Also returns something on NULL
  StampValue val = m_value / SECONDS_PER_DAY;
  return (val + 3) % 7;
}

// Name of the day of the week in a language by your choice
// Returns an empty string for a NULL timestamp
CString
SQLTimestamp::WeekDayName(Language p_lang /*=LN_DEFAULT*/) const
{
  if(Valid())
  {
    if(p_lang == LN_DEFAULT)
    {
      p_lang = g_defaultLanguage;
    }
    if(p_lang >= LN_DUTCH && p_lang <= LN_FRENCH)
    {
      return g_weekdays[p_lang - 1][WeekDay()];
    }
  }
  return "";
}

// Name of the month in a language by your choice
// Returns an emtpy string for a NULL timestamp
CString
SQLTimestamp::MonthName(Language p_lang /*=LN_DEFAULT*/) const
{
  if(Valid())
  {
    long monthNumber = m_timestamp.m_month;
    if (monthNumber >= 1 && monthNumber <= MONTHS_IN_YEAR)
    {
      if(p_lang == LN_DEFAULT)
      {
        p_lang = g_defaultLanguage;
      }
      if(p_lang >= LN_DUTCH && p_lang <= LN_FRENCH)
      {
        return g_monthnames[p_lang - 1][monthNumber - 1];
      }
    }
  }
  return "";
}

SQLTimestamp
SQLTimestamp::AddDays(int p_number) const
{
  if(Valid() && p_number)
  {
    return SQLTimestamp ((StampValue)(m_value + p_number * SECONDS_PER_DAY));
  }
  return *this;
}

SQLTimestamp
SQLTimestamp::AddMonths(int p_number) const
{
  if(Valid() && p_number)
  {
    int newDay   = Day();
    int newMonth = Month();
    int NewYear  = Year();
    int hour     = Hour();
    int min      = Minute();
    int sec      = Second();

    newMonth += p_number;
    if(newMonth > 12)
    {
      NewYear   +=  (newMonth-1) / 12;
      newMonth   = ((newMonth-1) % 12) + 1;
    }
    else if(newMonth < 1)
    {
      NewYear  += (newMonth - 12) / 12;
      newMonth  = 12 + (newMonth % 12);
    }
    int daysInNewMonth = DaysInMonth(NewYear,newMonth);
    if (newDay > daysInNewMonth)
    {
      newDay = daysInNewMonth;
    }
    return SQLTimestamp(NewYear, newMonth, newDay, hour, min, sec);
  }
  return *this;
}

// Returns the days in the month
// or 0 for a NULL timestamp
int
SQLTimestamp::DaysInMonth(int p_year,int p_month) const
{
  if(Valid())
  {
    bool leapYear = ((p_year & 3) == 0) && // if (year & 3) == 0 then the year can be divided
                    ((p_year % 100) != 0 || (p_year % 400) == 0);  // by 4 (binairy).

    return g_daysInTheMonth[p_month] - g_daysInTheMonth[p_month-1] +
                        ((leapYear && p_month == 2) ? 1 : 0);
  }
  return 0;
}

SQLTimestamp
SQLTimestamp::AddYears(int p_number) const
{
  if(Valid() && p_number)
  {
    try
    {
      return SQLTimestamp(Year() + p_number, Month(), Day(), Hour(), Minute(), Second());
    }
    catch(StdException& er)
    {
      if (Month() == 2 && Day() == 29) // Correction for leap year
      {
        // 29-2-1968 plus 1 year becomes 28-2-1969
        return SQLTimestamp(Year() + p_number, Month(), Day() - 1, Hour(), Minute(), Second());
      }
      // Throw on our error (something else went wrong)
      throw new StdException(er.GetErrorMessage());
    }
  }
  return *this;
}

SQLTimestamp
SQLTimestamp::AddHours(int p_number) const
{
  if(Valid() && p_number)
  {
    return SQLTimestamp ((StampValue)(m_value + p_number * SECONDS_PER_HOUR));
  }
  return *this;
}


SQLTimestamp
SQLTimestamp::AddMinutes(int p_number) const
{
  if(Valid() && p_number)
  {
    return SQLTimestamp ((StampValue)(m_value + p_number * SECONDS_PER_MINUTE));
  }
  return *this;
}


SQLTimestamp
SQLTimestamp::AddSeconds(__int64 p_number) const
{
  if(Valid() && p_number)
  {
    return SQLTimestamp ((StampValue)(m_value + p_number));
  }
  return *this;
}

SQLTimestamp
SQLTimestamp::AddFraction(int p_fraction) const
{
  if(Valid() && p_fraction)
  {
    SQLTimestamp stamp(*this);
    if(m_fraction + p_fraction > NANOSECONDS_PER_SEC)
    {
      stamp = stamp.AddSeconds(1);
      stamp.m_fraction = m_fraction + p_fraction - NANOSECONDS_PER_SEC;
    }
    else
    {
      stamp.m_fraction += p_fraction;
    }
    return stamp;
  }
  return *this;
}

int
SQLTimestamp::YearsBetween(const SQLTimestamp& p_stamp) const
{
  if(Valid() && p_stamp.Valid())
  {
    return p_stamp.Year() - Year();
  }
  return 0;
}

int
SQLTimestamp::MonthsBetween(const SQLTimestamp& p_stamp) const
{
  if(Valid() && p_stamp.Valid())
  {
    return ((p_stamp.Year() * 12) + p_stamp.Month()) - ((Year() * 12) + Month());
  }
  return 0;
}

int
SQLTimestamp::DaysBetween(const SQLTimestamp& p_stamp) const
{
  return (int)(SecondsBetween(p_stamp) / SECONDS_PER_DAY);
}

int
SQLTimestamp::HoursBetween(const SQLTimestamp& p_stamp) const
{
  return (int)(SecondsBetween(p_stamp) / SECONDS_PER_HOUR);
}

int
SQLTimestamp::MinutesBetween(const SQLTimestamp& p_stamp) const
{
  return (int)(SecondsBetween(p_stamp) / SECONDS_PER_MINUTE);
}

// Difference between two timestamp
StampValue
SQLTimestamp::SecondsBetween(const SQLTimestamp& p_stamp) const
{
  if(Valid() && p_stamp.Valid())
  {
    StampValue diff = p_stamp.m_value - m_value;
    if(p_stamp.m_value > m_value && m_fraction > p_stamp.m_fraction)
    {
      ++diff;
    }
    else if(p_stamp.m_fraction > m_fraction)
    {
      --diff;
    }
    return diff;
  }
  return 0L;
}

void
SQLTimestamp::ParseMoment(const CString& p_string)
{
  CString string(p_string);
  StampStorage temp;
  CString CurrentDate = "";
  CString Sign		    = "";
  CString ExtraTime   = "";
  int     interval    = 0;

  // Trim spaces from string
  string.Trim();
  // Test for emptiness
  if(p_string.IsEmpty())
  {
    SetNull();
    return;
  }

  // Test if we are properly initialized
  SQLComponentsInitialized();

  SQLDate::SplitStrDate(p_string, CurrentDate, Sign, ExtraTime, interval);

  if(isalpha(CurrentDate.GetAt(0)))
  {
    // Speed optimization. only if alpha chars found parsed 
    if (CurrentDate.CompareNoCase(g_dateNames[g_defaultLanguage][DN_CURRENT]) == 0 ||
        CurrentDate.CompareNoCase(g_dateNames[g_defaultLanguage][DN_NOW])     == 0 ) 
    {
      if (GetVirtualMoment(Sign, ExtraTime, interval, temp))
      {
        SetTimestamp(temp.m_year,temp.m_month,temp.m_day,temp.m_hour,temp.m_minute,temp.m_second);
        return;
      }
    }
    else if (CurrentDate.CompareNoCase(g_dateNames[g_defaultLanguage][DN_TODAY]) == 0)
    { 
      if (GetVirtualMoment(Sign, ExtraTime, interval, temp))
      {
        SetTimestamp(temp.m_year, temp.m_month, temp.m_day, 0, 0, 0);
        return;
      }
    }
    else if (CurrentDate.CompareNoCase(g_dateNames[g_defaultLanguage][DN_FOM]) == 0)
    {
      // FirstOfMonth
      SetTimestamp(CurrentTimestamp().Year()
                  ,CurrentTimestamp().Month()
                  ,1
                  ,0,0,0);
      return;
    }
    else if (CurrentDate.CompareNoCase(g_dateNames[g_defaultLanguage][DN_LOM]) == 0)
    {
      // FirstOfMonth
      SetTimestamp(CurrentTimestamp().Year()
                  ,CurrentTimestamp().Month()
                  ,CurrentTimestamp().DaysInMonth()
                  ,23,59,59);
      return;
    }
    else if (CurrentDate.CompareNoCase(g_dateNames[g_defaultLanguage][DN_FOY]) == 0 ||
             CurrentDate.CompareNoCase(g_dateNames[g_defaultLanguage][DN_CY])  == 0 )
    {
      // FirstOfYear / CurrentYear
      SetTimestamp(CurrentTimestamp().Year(),1,1,0,0,0);
      return;
    }
    else if (CurrentDate.CompareNoCase(g_dateNames[g_defaultLanguage][DN_LOY]) == 0)
    {
      // LastOfYear
      SetTimestamp(CurrentTimestamp().Year(),12,31,23,59,59);
      return;
    }
  }
 
  // Check for a XML string validation
  if(SQLDate::ParseXMLDate(p_string,*this))
  {
    RecalculateValue();
    return;  
  }
  
  // Check for a date with a monthname
  if(ParseNamedDate(p_string))
  {
    return;
  }
  
  // See if we have a date AND a time
  int pos1 = p_string.Find(' ');
  int pos2 = p_string.Find('T');
  if(pos1 > 0 || pos2 > 0)
  {
    // Position comes from 'T' (leftover from XML string)
    // Beware: yy-mm-dd is different than dd-mm-yyyy
    if(pos1 < 0) 
    {
      pos1 = pos2;
    }
    CString dateString = p_string.Left(pos1);
    CString timeString = p_string.Mid(pos1 + 1);

    SQLDate date(dateString);
    SQLTime time(timeString); 
    SetTimestamp(date.Year(),date.Month(), date.Day(),
                 time.Hour(),time.Minute(),time.Second());
    return;
  }
  // See if we have a time only
  if(p_string.Find(':') > 0)
  {
    // It is a timestring for today
    SQLDate date(SQLDate::Today());
    SQLTime time(p_string);
    SetTimestamp(date.Year(),date.Month(), date.Day(),
                 time.Hour(),time.Minute(),time.Second());
    return;
  }
  // It's a date only
  SQLDate date(p_string);
  SQLTime time;
  SetTimestamp(date.Year(),date.Month(), date.Day(),
               time.Hour(),time.Minute(),time.Second());
}

// Named timestamp with short or long monthnames
bool
SQLTimestamp::ParseNamedDate(const CString& p_string)
{
  bool result = false;
  bool alpha  = false;
  for(int ind = 0;ind < p_string.GetLength(); ++ind)
  {
    if(isalpha(p_string.GetAt(ind)))
    {
      alpha = true;
      break;
    }
  }
  if(alpha == false)
  {
    // No named monthname to find
    return result;
  }
  // Find the timestring (if any)
  int pos = p_string.Find(':');
  if(pos > 0)
  {
    int timepos = pos - 2;
    if(p_string.GetAt(pos - 2) == ' ')
    {
      timepos = pos - 1;
    }
    CString timeString = p_string.Mid(timepos);
    CString dateString = p_string.Left(timepos - 1);

    int year  = 0;
    int month = 0;
    int day   = 0;
    if(SQLDate::NamedDate(dateString,year,month,day))
    {
      SQLTime time(timeString); 
      SetTimestamp(year,month,day,time.Hour(),time.Minute(),time.Second());
      result = true;
    }
  }
  else
  {
    // Date string only
    int year  = 0;
    int month = 0;
    int day   = 0;
    if(SQLDate::NamedDate(p_string,year,month,day))
    {
      SQLTime time;
      SetTimestamp(year,month,day,time.Hour(),time.Minute(),time.Second());
      result = true;
    }
  }
  return result;
}

// Returns eg. for all databases: 13-06-1966 19:00:05
CString
SQLTimestamp::AsString(int p_precision /*=0*/) const
{
  CString theStamp;
  if(IsNull() == false)
  {
    theStamp.Format("%02d-%02d-%04d %02d:%02d:%02d"
                    ,Day(),Month(), Year()
                    ,Hour(),Minute(),Second());
    if(m_fraction && p_precision)
    {
      theStamp += PrintFraction(p_precision);
    }
  }
  return theStamp;
}

CString
SQLTimestamp::AsXMLString(int p_precision /*=0*/) const
{
  CString theStamp;
  if(IsNull() == false)
  {
    theStamp.Format("%04d-%02d-%02dT%02d:%02d:%02d"
                    ,Year(),Month(), Day()
                    ,Hour(),Minute(),Second());
    if(m_fraction && p_precision)
    {
      theStamp += PrintFraction(p_precision);
    }
  }
  return theStamp;
}

CString  
SQLTimestamp::AsXMLStringUTC(int p_precision /*=0*/) const
{
  CString theStamp;
  if(IsNull() == false)
  {
    SQLTimestamp stamp(*this);
    TIME_ZONE_INFORMATION tziCurrent;
    ::ZeroMemory(&tziCurrent,sizeof(tziCurrent));
    if(::GetTimeZoneInformation(&tziCurrent) != TIME_ZONE_ID_INVALID)
    {
      stamp = AddMinutes(tziCurrent.Bias);
    }
    theStamp.Format("%04d-%02d-%02dT%02d:%02d:%02d"
                    ,stamp.Year(),stamp.Month(), stamp.Day()
                    ,stamp.Hour(),stamp.Minute(),stamp.Second());
    if(m_fraction && p_precision)
    {
      theStamp += PrintFraction(p_precision);
    }
    // Mark as UTC string
    theStamp += "Z";
  }
  return theStamp;
}

CString
SQLTimestamp::AsReadString(int p_precision /*=0*/) const
{
  CString theStamp;
  if(IsNull() == false)
  {
    theStamp.Format("%02d-%02d-%04d %02d:%02d:%02d"
                   ,Day(), Month(), Year()
                   ,Hour(),Minute(),Second());
    if(m_fraction && p_precision)
    {
      theStamp += PrintFraction(p_precision);
    }
  }
  return theStamp;
}

CString 
SQLTimestamp::AsSQLString(SQLDatabase* p_database) const
{
  if(p_database)
  {
    return p_database->GetTimestampAsString(*this);
  }
  return AsString();
}

SQLDate
SQLTimestamp::AsSQLDate() const
{
  return SQLDate(*this);
}

SQLTime
SQLTimestamp::AsSQLTime() const
{
  return SQLTime(*this);
}

void     
SQLTimestamp::AsTimeStampStruct(TIMESTAMP_STRUCT* p_struct) const
{
  p_struct->year     = m_timestamp.m_year;
  p_struct->month    = m_timestamp.m_month;
  p_struct->day      = m_timestamp.m_day;
  p_struct->hour     = m_timestamp.m_hour;
  p_struct->minute   = m_timestamp.m_minute;
  p_struct->second   = m_timestamp.m_second;
  p_struct->fraction = m_fraction;
}

bool 
SQLTimestamp::GetVirtualMoment(CString Sign
                              ,CString ExtraTime
                              ,int    interval
                              ,StampStorage& temp)
{ 	
  // Test if we are properly initialized
  SQLComponentsInitialized();

  SQLTimestamp mom(CurrentTimestamp());
  if (!Sign.IsEmpty())
  {
    int factor = (Sign == "+") ? 1 : -1;
    if (Sign == "+" || Sign == "-")
    {
      ExtraTime.MakeUpper();
      if (ExtraTime == "" )      
      {
        return false;
      }
      else if (ExtraTime == g_dateNames[g_defaultLanguage][DN_SEC]    || 
               ExtraTime == g_dateNames[g_defaultLanguage][DN_SECONDS] )
      {
        mom = mom.AddSeconds(factor * interval);
      }
      else if (ExtraTime == g_dateNames[g_defaultLanguage][DN_MIN]    || 
               ExtraTime == g_dateNames[g_defaultLanguage][DN_MINUTES] )
      {
        mom = mom.AddMinutes (factor * interval);
      }
      else if (ExtraTime == g_dateNames[g_defaultLanguage][DN_HOUR] || 
               ExtraTime == g_dateNames[g_defaultLanguage][DN_HOURS] )
      {
        mom = mom.AddHours(factor * interval);
      }
      else if (ExtraTime == g_dateNames[g_defaultLanguage][DN_DAY]  || 
               ExtraTime == g_dateNames[g_defaultLanguage][DN_DAYS]  )
      {
        mom = mom.AddDays(factor * interval);
      }
      else if (ExtraTime == g_dateNames[g_defaultLanguage][DN_WEEK] || 
               ExtraTime == g_dateNames[g_defaultLanguage][DN_WEEKS] )
      {
        mom = mom.AddDays(factor * 7 * interval);
      }
      else if (ExtraTime == g_dateNames[g_defaultLanguage][DN_MONTH] || 
               ExtraTime == g_dateNames[g_defaultLanguage][DN_MONTHS] )
      {
        mom = mom.AddMonths(factor * interval);
      }
      else if (ExtraTime == g_dateNames[g_defaultLanguage][DN_YEAR] || 
               ExtraTime == g_dateNames[g_defaultLanguage][DN_YEARS] )
      {
        mom = mom.AddYears(factor * interval);
      }
      else return false;
    }
    else
    {
      return false;
    }
  }
  temp = mom.m_timestamp;
  return true;
}

// Print the fraction to a string
// Maximum resolution by W3C definition is nanoseconds (precision = 9)
// Maximum resolution on a MS-Windows OS is 100 nanoseconds (precision = 7)
CString
SQLTimestamp::PrintFraction(int p_precision) const
{
  CString fract;
  if(m_fraction && p_precision)
  {
    // Standard precision = 3 decimal places
    if(p_precision < 0) p_precision = 3;
    // Maximum precision is nanosecond resolution
    if(p_precision > 9) p_precision = 9;
    double fraction = (double)m_fraction / (double)NANOSECONDS_PER_SEC;
    fract.Format("%.*f",p_precision,fraction);
    // Do not use the prefix '0' (zero)
    return fract.Mid(1);
  }
  return fract;
}

//////////////////////////////////////////////////////////////////////////
//
// OPERATORS
//
//////////////////////////////////////////////////////////////////////////

// Assignment operator, regardless of state

SQLTimestamp&
SQLTimestamp::operator=(const SQLTimestamp& p_moment)
{
  if(&p_moment != this)
  {
    m_value     = p_moment.m_value;
    m_timestamp = p_moment.m_timestamp;
    m_fraction  = p_moment.m_fraction;
  }
  return *this;
}

SQLTimestamp&
SQLTimestamp::operator=(const SQLDate& p_date)
{
  // Check for NULL state
  if(p_date.IsNull())
  {
    SetNull();
  }
  else
  {
    // That day at 00:00:00 midnight (starting of the day)
    SetTimestamp(p_date.Year(),p_date.Month(),p_date.Day(),0,0,0);
  }
  return *this;
}

SQLTimestamp& 
SQLTimestamp::operator=(const SQLTime& p_time)
{
  // Check for NULL state
  if(p_time.IsNull())
  {
    SetNull();
  }
  else
  {
    // Current day on that time
    SQLDate date = SQLDate::Today();
    SetTimestamp(date.Year()
                ,date.Month()
                ,date.Day()
                ,p_time.Hour()
                ,p_time.Minute()
                ,p_time.Second());
  }
  return *this;
}

// Temporal operators
SQLInterval
SQLTimestamp::operator-(const SQLTimestamp& p_timestamp) const
{
  // Check for NULL state
  if(IsNull() || p_timestamp.IsNull())
  {
    SQLInterval intval;
    return intval;
  }
  // Do the calculation
  InterValue value = (m_value - p_timestamp.m_value) * NANOSECONDS_PER_SEC;
  SQLInterval intval(SQL_IS_DAY_TO_SECOND,value);
  return intval;
}

SQLTimestamp
SQLTimestamp::operator+(const SQLInterval& p_interval) const
{
  // Check for NULL state
  if(IsNull() || p_interval.IsNull())
  {
    SQLTimestamp stamp;
    return stamp;
  }
  // Check on correct ordinal interval type
  if(p_interval.GetIsYearMonthType())
  {
    throw new StdException("Cannot add a year-month interval to a timestamp");
  }
  // Do the calculation
  int sign = p_interval.GetIsNegative() ? -1 : 1;
  StampValue value = m_value + (sign * p_interval.AsValue() / NANOSECONDS_PER_SEC);
  int  fraction = m_fraction + (sign * p_interval.AsValue() % NANOSECONDS_PER_SEC);
  SQLTimestamp stamp(value,fraction);
  return stamp;
}

SQLTimestamp  
SQLTimestamp::operator-(const SQLInterval& p_interval) const
{
  // Check for NULL state
  if(IsNull() || p_interval.IsNull())
  {
    SQLTimestamp stamp;
    return stamp;
  }
  // Check on correct ordinal interval type
  if(p_interval.GetIsYearMonthType())
  {
    throw new StdException("Cannot subtract a year-month interval from a timestamp");
  }
  // Do the calculation
  int sign = p_interval.GetIsNegative() ? -1 : 1;
  StampValue value = m_value - (sign * p_interval.AsValue() / NANOSECONDS_PER_SEC);
  int  fraction = m_fraction - (sign * p_interval.AsValue() % NANOSECONDS_PER_SEC);
  SQLTimestamp stamp(value,fraction);
  return stamp;
}

// Comparison operators

bool
SQLTimestamp::operator==(const SQLTimestamp& p_stamp) const
{
  if(IsNull() || p_stamp.IsNull())
  {
    return false;
  }
  return (m_value    == p_stamp.m_value) && 
         (m_fraction == p_stamp.m_fraction);
}

bool
SQLTimestamp::operator!=(const SQLTimestamp& p_stamp) const
{
  if(IsNull() || p_stamp.IsNull())
  {
    return false;
  }
  return (m_value    != p_stamp.m_value) ||
         (m_fraction != p_stamp.m_fraction);
}

bool
SQLTimestamp::operator<(const SQLTimestamp& p_stamp) const
{
  if(IsNull() || p_stamp.IsNull())
  {
    return false;
  }
  if(m_value < p_stamp.m_value)
  {
    return true;
  }
  if(m_value == p_stamp.m_value)
  {
    return m_fraction < p_stamp.m_fraction;
  }
  return false;
}

bool
SQLTimestamp::operator>(const SQLTimestamp& p_stamp) const
{
  if(IsNull() || p_stamp.IsNull())
  {
    return false;
  }
  if(m_value > p_stamp.m_value)
  {
    return true;
  }
  if(m_value == p_stamp.m_value)
  {
    return m_fraction > p_stamp.m_fraction;
  }
  return false;
}

bool
SQLTimestamp::operator<=(const SQLTimestamp& p_stamp) const
{
  if(IsNull() || p_stamp.IsNull())
  {
    return false;
  }
  if(m_value < p_stamp.m_value)
  {
    return true;
  }
  if(m_value == p_stamp.m_value)
  {
    return m_fraction <= p_stamp.m_fraction;
  }
  return false;
}

bool
SQLTimestamp::operator>=(const SQLTimestamp& p_stamp) const
{
  if(IsNull() || p_stamp.IsNull())
  {
    return false;
  }
  if(m_value > p_stamp.m_value)
  {
    return true;
  }
  if(m_value == p_stamp.m_value)
  {
    return m_fraction >= p_stamp.m_fraction;
  }
  return false;
}

// End of namespace
}

