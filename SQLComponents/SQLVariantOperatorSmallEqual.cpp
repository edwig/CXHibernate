////////////////////////////////////////////////////////////////////////
//
// File: SQLVariantOperatorSmallEQ.cpp
//
// Copyright (c) 1998-2021 ir. W.E. Huisman
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
#include "stdafx.h"
#include "SQLComponents.h"
#include "SQLVariant.h"
#include "SQLVariantTrim.h"
#include "SQLVariantOperator.h"
#include "SQLDate.h"
#include "bcd.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace SQLComponents
{

//////////////////////////////////////////////////////////////////////////
//
// ALL EQUALITY OPERATOR FUNCTIONS
//
//////////////////////////////////////////////////////////////////////////

bool
static SQL_OperVarSmallEQChar(SQLVariant& p_left,SQLVariant& p_right)
{
  CString leftString,rightString;
  p_left .GetAsString(leftString);
  p_right.GetAsString(rightString);

  return leftString.Compare(rightString) <= 0;
}

bool
static SQL_OperGuidSmallEQChar(SQLVariant& p_left,SQLVariant& p_right)
{
  CString leftString,rightString;
  p_left .GetAsString(leftString);
  p_right.GetAsString(rightString);

  leftString. TrimLeft ('{');
  leftString. TrimRight('}');
  rightString.TrimLeft ('}');
  rightString.TrimRight('{');

  return leftString.Compare(rightString) <= 0;
}

// SIGNED SHORT

bool
static SQL_OperSShortSmallEQSShort(SQLVariant& p_left,SQLVariant& p_right)
{
  return p_left.GetAsSShort() <= p_right.GetAsSShort();
}

bool
static SQL_OperUShortSmallEQSShort(SQLVariant& p_left,SQLVariant& p_right)
{
  return p_left.GetAsUShort() <= SQL_ShortToUShort(p_right.GetAsSShort());
}

bool
static SQL_OperSLongSmallEQSShort(SQLVariant& p_left,SQLVariant& p_right)
{
  return SQL_SLongToShort(p_left.GetAsSLong()) <= p_right.GetAsSShort();
}

bool
static SQL_OperULongSmallEQSShort(SQLVariant& p_left,SQLVariant& p_right)
{
  return SQL_ULongToUShort(p_left.GetAsULong()) <= SQL_ShortToUShort(p_right.GetAsSShort());
}

bool
static SQL_OperFloatSmallEQSShort(SQLVariant& p_left,SQLVariant& p_right)
{
  return SQL_FloatToShort(p_left.GetAsFloat()) <= p_right.GetAsSShort();
}

bool
static SQL_OperDoubleSmallEQSShort(SQLVariant& p_left,SQLVariant& p_right)
{
  return SQL_DoubleToShort(p_left.GetAsDouble()) <= p_right.GetAsSShort();
}

bool
static SQL_OperBitSmallEQSShort(SQLVariant& /*p_left*/,SQLVariant& p_right)
{
  return p_right.GetAsSShort() != 0;
}

bool   
static SQL_OperSTinySmallEQSShort(SQLVariant& p_left,SQLVariant& p_right)
{
  return p_left.GetAsSTinyInt() <= SQL_ShortToTinyInt(p_right.GetAsSShort());
}

bool
static SQL_OperUTinySmallEQSShort(SQLVariant& p_left,SQLVariant& p_right)
{
  return p_left.GetAsUTinyInt() <= SQL_ShortToUTinyInt(p_right.GetAsSShort());
}

bool
static SQL_OperSBigSmallEQSShort(SQLVariant& p_left,SQLVariant& p_right)
{
  return SQL_BIGINTToShort(p_left.GetAsSBigInt()) <= p_right.GetAsSShort();
}

bool
static SQL_OperUBigSmallEQSShort(SQLVariant& p_left,SQLVariant& p_right)
{
  return SQL_UBIGINTToShort(p_left.GetAsUBigInt()) <= p_right.GetAsSShort();
}

bool
static SQL_OperNumSmallEQSShort(SQLVariant& p_left,SQLVariant& p_right)
{
  return SQL_SLongToShort(p_left.GetAsBCD().AsLong()) <= p_right.GetAsSShort();
}

// UNSIGNED SHORT

bool   
static SQL_OperSShortSmallEQUShort(SQLVariant& p_left,SQLVariant& p_right)
{
  return SQL_ShortToUShort(p_left.GetAsSShort()) <= p_right.GetAsUShort();
}

bool   
static SQL_OperUShortSmallEQUShort(SQLVariant& p_left,SQLVariant& p_right)
{
  return p_left.GetAsUShort() <= p_right.GetAsUShort();
}

bool
static SQL_OperSLongSmallEQUShort(SQLVariant& p_left,SQLVariant& p_right)
{
  return SQL_SLongToUShort(p_left.GetAsSLong()) <= p_right.GetAsUShort();
}

bool
static SQL_OperUlongSmallEQUShort(SQLVariant& p_left,SQLVariant& p_right)
{
  return SQL_ULongToUShort(p_left.GetAsULong()) <= p_right.GetAsUShort();
}

bool
static SQL_OperFloatSmallEQUShort(SQLVariant& p_left,SQLVariant& p_right)
{
  return SQL_FloatToUShort(p_left.GetAsFloat()) <= p_right.GetAsUShort();
}

bool
static SQL_OperDoubleSmallEQUShort(SQLVariant& p_left,SQLVariant& p_right)
{
  return SQL_DoubleToUShort(p_left.GetAsDouble()) <= p_right.GetAsUShort();
}

bool
static SQL_OperBitSmallEQUShort(SQLVariant& /*p_left*/,SQLVariant& p_right)
{
  return p_right.GetAsUShort() != 0;
}

bool
static SQL_OperSTinySmallEQUShort(SQLVariant& p_left,SQLVariant& p_right)
{
  return p_left.GetAsSTinyInt() <= SQL_UShortToTinyInt(p_right.GetAsUShort());
}

bool
static SQL_OperUTinySmallEQUShort(SQLVariant& p_left,SQLVariant& p_right)
{
  return p_left.GetAsUTinyInt() <= SQL_UShortToUTinyInt(p_right.GetAsUShort());
}

bool
static SQL_OperSBigSmallEQUShort(SQLVariant& p_left,SQLVariant& p_right)
{
  return SQL_BIGINTToUShort(p_left.GetAsSBigInt()) <= p_right.GetAsUShort();
}

bool
static SQL_OperUBigSmallEQUShort(SQLVariant& p_left,SQLVariant& p_right)
{
  return SQL_UBIGINTToUShort(p_left.GetAsUBigInt()) <= p_right.GetAsUShort();
}

bool   
static SQL_OperNumSmallEQUShort(SQLVariant& p_left,SQLVariant& p_right)
{
  return SQL_SLongToUShort(p_left.GetAsBCD().AsLong()) <= p_right.GetAsUShort();
}

// SIGNED LONG

bool   
static SQL_OperSShortSmallEQSLong(SQLVariant& p_left,SQLVariant& p_right)
{
  return p_left.GetAsSShort() <= SQL_SLongToShort(p_right.GetAsSLong());
}

bool
static SQL_OperUShortSmallEQSLong(SQLVariant& p_left,SQLVariant& p_right)
{
  return p_left.GetAsUShort() <= SQL_SLongToUShort(p_right.GetAsSLong());
}

bool
static SQL_OperSLongSmallEQSLong(SQLVariant& p_left,SQLVariant& p_right)
{
  return p_left.GetAsSLong() <= p_right.GetAsSLong();
}

bool
static SQL_OperULongSmallEQSLong(SQLVariant& p_left,SQLVariant& p_right)
{
  return p_left.GetAsULong() <= SQL_LongToULong(p_right.GetAsSLong());
}

bool
static SQL_OperFloatSmallEQSLong(SQLVariant& p_left,SQLVariant& p_right)
{
  return SQL_FloatToLong(p_left.GetAsFloat()) <= p_right.GetAsSLong();
}

bool
static SQL_OperDoubleESmallEQSLong(SQLVariant& p_left,SQLVariant& p_right)
{
  return SQL_DoubleToLong(p_left.GetAsDouble()) <= p_right.GetAsSLong();
}

bool
static SQL_OperBitSmallEQSLong(SQLVariant& /*p_left*/,SQLVariant& p_right)
{
  return p_right.GetAsSLong() != 0;
}

bool
static SQL_OperSTinySmallEQSLong(SQLVariant& p_left,SQLVariant& p_right)
{
  return p_left.GetAsSTinyInt() <= SQL_SLongToTinyInt(p_right.GetAsSLong());
}

bool
static SQL_OperUTinySmallEQSLong(SQLVariant& p_left,SQLVariant& p_right)
{
  return p_left.GetAsUTinyInt() <= SQL_SLongToUTinyInt(p_right.GetAsSLong());
}

bool
static SQL_OperSBigSmallEQSLong(SQLVariant& p_left,SQLVariant& p_right)
{
  return SQL_BIGINTToLong(p_left.GetAsSBigInt()) <= p_right.GetAsSLong();
}

bool
static SQL_OperUBigSmallEQSLong(SQLVariant& p_left,SQLVariant& p_right)
{
  return SQL_UBIGINTToLong(p_left.GetAsUBigInt()) <= p_right.GetAsSLong();
}

bool
static SQL_OperNumSmallEQSLong(SQLVariant& p_left,SQLVariant& p_right)
{
  return p_left.GetAsBCD().AsLong() <= p_right.GetAsSLong();
}

// UNSIGNED LONG

bool   
static SQL_OperSShortSmallEQULong(SQLVariant& p_left,SQLVariant& p_right)
{
  return SQL_ShortToUShort(p_left.GetAsSShort()) <= SQL_ULongToUShort(p_right.GetAsULong());
}

bool
static SQL_OperUShortSmallEQULong(SQLVariant& p_left,SQLVariant& p_right)
{
  return p_left.GetAsUShort() <= SQL_ULongToUShort(p_right.GetAsULong());
}

bool
static SQL_OperSLongSmallEQULong(SQLVariant& p_left,SQLVariant& p_right)
{
  return SQL_LongToULong(p_left.GetAsSLong()) <= p_right.GetAsULong();
}

bool
static SQL_OperULongSmallEQULong(SQLVariant& p_left,SQLVariant& p_right)
{
  return p_left.GetAsULong() <= p_right.GetAsULong();
}

bool
static SQL_OperFloatSmallEQULong(SQLVariant& p_left,SQLVariant& p_right)
{
  return SQL_FloatToULong(p_left.GetAsFloat()) <= p_right.GetAsULong();
}

bool
static SQL_OperDoubleSmallEQULong(SQLVariant& p_left,SQLVariant& p_right)
{
  return SQL_DoubleToULong(p_left.GetAsDouble()) <= p_right.GetAsULong();
}

bool
static SQL_OperBitSmallEQULong(SQLVariant& /*p_left*/,SQLVariant& p_right)
{
  return p_right.GetAsULong() != 0;
}

bool
static SQL_OperSTinySmallEQULong(SQLVariant& p_left,SQLVariant& p_right)
{
  return SQL_TinyIntToUTinyInt(p_left.GetAsSTinyInt()) <= SQL_ULongToUTinyInt(p_right.GetAsULong());
}

bool
static SQL_OperUTinySmallEQULong(SQLVariant& p_left,SQLVariant& p_right)
{
  return p_left.GetAsUTinyInt() <= SQL_ULongToUTinyInt(p_right.GetAsULong());
}

bool
static SQL_OperSBigSmallEQULong(SQLVariant& p_left,SQLVariant& p_right)
{
  return SQL_BIGINTToULong(p_left.GetAsSBigInt()) <= p_right.GetAsULong();
}

bool
static SQL_OperUBigSmallEQULong(SQLVariant& p_left,SQLVariant& p_right)
{
  return SQL_UBIGINTToULong(p_left.GetAsUBigInt()) <= p_right.GetAsULong();
}

bool
static SQL_OperNumSmallEQULong(SQLVariant& p_left,SQLVariant& p_right)
{
  return SQL_BIGINTToULong(p_left.GetAsBCD().AsInt64()) <= p_right.GetAsULong();
}

// FLOAT

bool   
static SQL_OperSShortSmallEQFloat(SQLVariant& p_left,SQLVariant& p_right)
{
  return p_left.GetAsSShort() <= SQL_FloatToShort(p_right.GetAsFloat());
}

bool   
static SQL_OperUShortSmallEQFloat(SQLVariant& p_left,SQLVariant& p_right)
{
  return p_left.GetAsUShort() <= SQL_FloatToUShort(p_right.GetAsFloat());
}

bool
static SQL_OperSLongSmallEQFloat(SQLVariant& p_left,SQLVariant& p_right)
{
  return p_left.GetAsSLong() <= SQL_FloatToLong(p_right.GetAsFloat());
}

bool
static SQL_OperULongSmallEQFloat(SQLVariant& p_left,SQLVariant& p_right)
{
  return p_left.GetAsULong() <= SQL_FloatToULong(p_right.GetAsFloat());
}

bool
static SQL_OperFloatSmallEQFloat(SQLVariant& p_left,SQLVariant& p_right)
{
  return p_left.GetAsFloat() <= p_right.GetAsFloat();
}

bool
static SQL_OperDoubleSmallEQFloat(SQLVariant& p_left,SQLVariant& p_right)
{
  return SQL_DoubleToFloat(p_left.GetAsDouble()) <= p_right.GetAsFloat();
}

bool
static SQL_OperBitSmallEQFloat(SQLVariant& /*p_left*/,SQLVariant& p_right)
{
  return p_right.GetAsFloat() < -FLT_EPSILON || FLT_EPSILON < p_right.GetAsFloat();
}

bool
static SQL_OperSTinySmallEQFloat(SQLVariant& p_left,SQLVariant& p_right)
{
  return p_left.GetAsSTinyInt() <= SQL_FloatToTinyInt(p_right.GetAsFloat());
}

bool
static SQL_OperUTinySmallEQFloat(SQLVariant& p_left,SQLVariant& p_right)
{
  return p_left.GetAsUTinyInt() <= SQL_FloatToUTinyInt(p_right.GetAsFloat());
}

bool
static SQL_OperSBigSmallEQFloat(SQLVariant& p_left,SQLVariant& p_right)
{
  return p_left.GetAsSBigInt() <= SQL_FloatToBIGINT(p_right.GetAsFloat());
}

bool
static SQL_OperUBigSmallEQFloat(SQLVariant& p_left,SQLVariant& p_right)
{
  return p_left.GetAsUBigInt() <= SQL_FloatToUBIGINT(p_right.GetAsFloat());
}

bool
static SQL_OperNumSmallEQFloat(SQLVariant& p_left,SQLVariant& p_right)
{
  return p_left.GetAsBCD().AsDouble() <= (double) p_right.GetAsFloat();
}

// DOUBLE

bool
static SQL_OperSShortSmallEQDouble(SQLVariant& p_left,SQLVariant& p_right)
{
  return p_left.GetAsSShort() <= SQL_DoubleToShort(p_right.GetAsDouble());
}

bool
static SQL_OperUShortSmallEQDouble(SQLVariant& p_left,SQLVariant& p_right)
{
  return p_left.GetAsUShort() <= SQL_DoubleToUShort(p_right.GetAsDouble());
}

bool
static SQL_OperSLongSmallEQDouble(SQLVariant& p_left,SQLVariant& p_right)
{
  return p_left.GetAsSLong() <= SQL_DoubleToLong(p_right.GetAsDouble());
}

bool
static SQL_OperULongSmallEQDouble(SQLVariant& p_left,SQLVariant& p_right)
{
  return p_left.GetAsULong() <= SQL_DoubleToULong(p_right.GetAsDouble());
}

bool
static SQL_OperFloatSmallEQDouble(SQLVariant& p_left,SQLVariant& p_right)
{
  return p_left.GetAsFloat() <= SQL_DoubleToFloat(p_right.GetAsDouble());
}

bool
static SQL_OperDoubleSmallEQDouble(SQLVariant& p_left,SQLVariant& p_right)
{
  return p_left.GetAsDouble() <= p_right.GetAsDouble();
}

bool
static SQL_OperBitSmallEQDouble(SQLVariant& /*p_left*/,SQLVariant& p_right)
{
  return p_right.GetAsDouble() < -DBL_EPSILON || DBL_EPSILON < p_right.GetAsDouble();
}

bool
static SQL_OperSTinySmallEQDouble(SQLVariant& p_left,SQLVariant& p_right)
{
  return p_left.GetAsSTinyInt() <= SQL_DoubleToTinyInt(p_right.GetAsDouble());
}

bool
static SQL_OperUTinySmallEQDouble(SQLVariant& p_left,SQLVariant& p_right)
{
  return p_left.GetAsUTinyInt() <= SQL_DoubleToUTinyInt(p_right.GetAsDouble());
}

bool
static SQL_OperSBigSmallEQDouble(SQLVariant& p_left,SQLVariant& p_right)
{
  return p_left.GetAsSBigInt() <= SQL_DoubleToBIGINT(p_right.GetAsDouble());
}

bool
static SQL_OperUBigSmallEQDouble(SQLVariant& p_left,SQLVariant& p_right)
{
  return p_left.GetAsUBigInt() <= SQL_DoubleToUBIGINT(p_right.GetAsDouble());
}

bool
static SQL_OperNumSmallEQDouble(SQLVariant& p_left,SQLVariant& p_right)
{
  return p_left.GetAsBCD().AsDouble() <= p_right.GetAsDouble();
}

// BIT

bool   
static SQL_OperSShortSmallEQBit(SQLVariant& /*p_left*/,SQLVariant& p_right)
{
  return p_right.GetAsBit() != 0;
}

bool
static SQL_OperUShortSmallEQBit(SQLVariant& /*p_left*/,SQLVariant& p_right)
{
  return p_right.GetAsBit() != 0;
}

bool   
static SQL_OperSLongSmallEQBit(SQLVariant& /*p_left*/,SQLVariant& p_right)
{
  return p_right.GetAsBit() != 0;
}

bool
static SQL_OperULongSmallEQBit(SQLVariant& /*p_left*/,SQLVariant& p_right)
{
  return p_right.GetAsBit() != 0;
}

bool
static SQL_OperFloatSmallEQBit(SQLVariant& /*p_left*/,SQLVariant& p_right)
{
  return p_right.GetAsBit() != 0;
}

bool
static SQL_OperDoubleSmallEQBit(SQLVariant& /*p_left*/,SQLVariant& p_right)
{
  return p_right.GetAsBit() != 0;
}

bool
static SQL_OperBitSmallEQBit(SQLVariant& /*p_left*/,SQLVariant& p_right)
{
  return p_right.GetAsBit() != 0;
}

bool
static SQL_OperSTinySmallEQBit(SQLVariant& /*p_left*/,SQLVariant& p_right)
{
  return p_right.GetAsBit() != 0;
}

bool
static SQL_OperUTinySmallEQBit(SQLVariant& /*p_left*/,SQLVariant& p_right)
{
  return p_right.GetAsBit() != 0;
}

bool
static SQL_OperSBigSmallEQBit(SQLVariant& /*p_left*/,SQLVariant& p_right)
{
  return p_right.GetAsBit() != 0;
}

bool
static SQL_OperUBigSmallEQBit(SQLVariant& /*p_left*/,SQLVariant& p_right)
{
  return p_right.GetAsBit() != 0;
}

bool
static SQL_OperNumSmallEQBit(SQLVariant& /*p_left*/,SQLVariant& p_right)
{
  return p_right.GetAsBit() != 0;
}

// SIGNED TINYINT

bool
static SQL_OperSShortSmallEQSTiny(SQLVariant& p_left,SQLVariant& p_right)
{
  return SQL_ShortToTinyInt(p_left.GetAsSShort()) <= p_right.GetAsSTinyInt();
}

bool
static SQL_OperUShortSmallEQSTiny(SQLVariant& p_left,SQLVariant& p_right)
{
  return SQL_UShortToTinyInt(p_left.GetAsUShort()) <= p_right.GetAsSTinyInt();
}

bool
static SQL_OperSLongSmallEQSTiny(SQLVariant& p_left,SQLVariant& p_right)
{
  return SQL_SLongToTinyInt(p_left.GetAsSLong()) <= p_right.GetAsSTinyInt();
}

bool
static SQL_OperULongSmallEQSTiny(SQLVariant& p_left,SQLVariant& p_right)
{
  return SQL_ULongToTinyInt(p_left.GetAsULong()) <= p_right.GetAsSTinyInt();
}

bool
static SQL_OperFloatSmallEQSTiny(SQLVariant& p_left,SQLVariant& p_right)
{
  return SQL_FloatToTinyInt(p_left.GetAsFloat()) <= p_right.GetAsSTinyInt();
}

bool
static SQL_OperDoubleSmallEQSTiny(SQLVariant& p_left,SQLVariant& p_right)
{
  return SQL_DoubleToTinyInt(p_left.GetAsDouble()) <= p_right.GetAsSTinyInt();
}

bool
static SQL_OperBitSmallEQSTiny(SQLVariant& /*p_left*/,SQLVariant& p_right)
{
  return p_right.GetAsSTinyInt() != 0;
}

bool
static SQL_OperSTinySmallEQSTiny(SQLVariant& p_left,SQLVariant& p_right)
{
  return p_left.GetAsSTinyInt() <= p_right.GetAsSTinyInt();
}

bool
static SQL_OperUTinySmallEQSTiny(SQLVariant& p_left,SQLVariant& p_right)
{
  return p_left.GetAsUTinyInt() <= SQL_TinyIntToUTinyInt(p_right.GetAsSTinyInt());
}

bool
static SQL_OperSBigSmallEQSTiny(SQLVariant& p_left,SQLVariant& p_right)
{
  return SQL_SBIGINTToTinyInt(p_left.GetAsSBigInt()) <= p_right.GetAsSTinyInt();
}

bool
static SQL_OperUBigSmallEQSTiny(SQLVariant& p_left,SQLVariant& p_right)
{
  return SQL_UBIGINTToTinyInt(p_left.GetAsUBigInt()) <= p_right.GetAsSTinyInt();
}

bool
static SQL_OperNumSmallEQSTiny(SQLVariant& p_left,SQLVariant& p_right)
{
  return SQL_SLongToTinyInt(p_left.GetAsBCD().AsLong()) <= p_right.GetAsSTinyInt();
}

// UNSIGNED TINYINT

bool
static SQL_OperSShortSmallEQUTiny(SQLVariant& p_left,SQLVariant& p_right)
{
  return SQL_ShortToUTinyInt(p_left.GetAsSShort()) <= p_right.GetAsUTinyInt();
}

bool
static SQL_OperUShortSmallEQUTiny(SQLVariant& p_left,SQLVariant& p_right)
{
  return SQL_UShortToUTinyInt(p_left.GetAsUShort()) <= p_right.GetAsUTinyInt();
}

bool
static SQL_OperSLongSmallEQUTiny(SQLVariant& p_left,SQLVariant& p_right)
{
  return SQL_SLongToUTinyInt(p_left.GetAsSLong()) <= p_right.GetAsUTinyInt();
}

bool
static SQL_OperULongSmallEQUTiny(SQLVariant& p_left,SQLVariant& p_right)
{
  return SQL_ULongToUTinyInt(p_left.GetAsULong()) <= p_right.GetAsUTinyInt();
}

bool
static SQL_OperFloatSmallEQUTiny(SQLVariant& p_left,SQLVariant& p_right)
{
  return SQL_FloatToUTinyInt(p_left.GetAsFloat()) <= p_right.GetAsUTinyInt();
}

bool
static SQL_OperDoubleSmallEQUTiny(SQLVariant& p_left,SQLVariant& p_right)
{
  return SQL_DoubleToUTinyInt(p_left.GetAsDouble()) <= p_right.GetAsUTinyInt();
}

bool
static SQL_OperBitSmallEQUTiny(SQLVariant& /*p_left*/,SQLVariant& p_right)
{
  return p_right.GetAsUTinyInt() != 0;
}

bool
static SQL_OperSTinySmallEQUTiny(SQLVariant& p_left,SQLVariant& p_right)
{
  return SQL_TinyIntToUTinyInt(p_left.GetAsSTinyInt()) <= p_right.GetAsUTinyInt();
}

bool
static SQL_OperUTinySmallEQUTiny(SQLVariant& p_left,SQLVariant& p_right)
{
  return p_left.GetAsUTinyInt() <= p_right.GetAsUTinyInt();
}

bool
static SQL_OperSBigSmallEQUTiny(SQLVariant& p_left,SQLVariant& p_right)
{
  return SQL_SBIGINTToUTinyInt(p_left.GetAsSBigInt()) <= p_right.GetAsUTinyInt();
}

bool
static SQL_OperUBigSmallEQUTiny(SQLVariant& p_left,SQLVariant& p_right)
{
  return SQL_UBIGINTToUTinyInt(p_left.GetAsUBigInt()) <= p_right.GetAsUTinyInt();
}

bool
static SQL_OperNumSmallEQUTiny(SQLVariant& p_left,SQLVariant& p_right)
{
  return SQL_SLongToUTinyInt(p_left.GetAsBCD().AsLong()) <= p_right.GetAsUTinyInt();
}

// SIGNED BIGINT

bool
static SQL_OperSShortSmallEQSBig(SQLVariant& p_left,SQLVariant& p_right)
{
  return p_left.GetAsSShort() <= SQL_BIGINTToShort(p_right.GetAsSBigInt());
}

bool
static SQL_OperUShortSmallEQSBig(SQLVariant& p_left,SQLVariant& p_right)
{
  return p_left.GetAsUShort() <= SQL_BIGINTToUShort(p_right.GetAsSBigInt());
}

bool
static SQL_OperSLongSmallEQSBig(SQLVariant& p_left,SQLVariant& p_right)
{
  return p_left.GetAsSLong() <= SQL_BIGINTToLong(p_right.GetAsSBigInt());
}

bool
static SQL_OperULongSmallEQSBig(SQLVariant& p_left,SQLVariant& p_right)
{
  return p_left.GetAsULong() <= SQL_BIGINTToULong(p_right.GetAsSBigInt());
}

bool
static SQL_OperFloatSmallEQSBig(SQLVariant& p_left,SQLVariant& p_right)
{
  return p_left.GetAsFloat() <= SQL_BIGINTToFloat(p_right.GetAsSBigInt());
}

bool
static SQL_OperDoubleSmallEQSBig(SQLVariant& p_left,SQLVariant& p_right)
{
  return p_left.GetAsDouble() <= SQL_BIGINTToDouble(p_right.GetAsSBigInt());
}

bool
static SQL_OperBitSmallEQSBig(SQLVariant& /*p_left*/,SQLVariant& p_right)
{
  return p_right.GetAsSBigInt() != 0;
}

bool
static SQL_OperSTinySmallEQSBig(SQLVariant& p_left,SQLVariant& p_right)
{
  return p_left.GetAsSTinyInt() <= SQL_SBIGINTToTinyInt(p_right.GetAsSBigInt());
}

bool
static SQL_OperUTinySmallEQSBig(SQLVariant& p_left,SQLVariant& p_right)
{
  return p_left.GetAsUTinyInt() <= SQL_SBIGINTToUTinyInt(p_right.GetAsSBigInt());
}

bool
static SQL_OperSBigSmallEQSBig(SQLVariant& p_left,SQLVariant& p_right)
{
  return p_left.GetAsSBigInt() <= p_right.GetAsSBigInt();
}

bool
static SQL_OperUBigSmallEQSBig(SQLVariant& p_left,SQLVariant& p_right)
{
  return p_left.GetAsUBigInt() <= SQL_SBIGINTToUBIGINT(p_right.GetAsSBigInt());
}

bool   
static SQL_OperNumSmallEQSBig(SQLVariant& p_left,SQLVariant& p_right)
{
  return p_left.GetAsBCD().AsInt64() <= p_right.GetAsSBigInt();
}

// UNSIGNED BIGINT

bool
static SQL_OperSShortSmallEQUBig(SQLVariant& p_left,SQLVariant& p_right)
{
  return p_left.GetAsSShort() <= SQL_UBIGINTToShort(p_right.GetAsUBigInt());
}

bool
static SQL_OperUShortSmallEQUBig(SQLVariant& p_left,SQLVariant& p_right)
{
  return p_left.GetAsUShort() <= SQL_UBIGINTToUShort(p_right.GetAsUBigInt());
}

bool
static SQL_OperSLongSmallEQUBig(SQLVariant& p_left,SQLVariant& p_right)
{
  return p_left.GetAsSLong() <= SQL_UBIGINTToLong(p_right.GetAsUBigInt());
}

bool
static SQL_OperULongSmallEQUBig(SQLVariant& p_left,SQLVariant& p_right)
{
  return p_left.GetAsULong() <= SQL_UBIGINTToULong(p_right.GetAsUBigInt());
}

bool
static SQL_OperFloatSmallEQUBig(SQLVariant& p_left,SQLVariant& p_right)
{
  return p_left.GetAsFloat() <= SQL_UBIGINTToFloat(p_right.GetAsUBigInt());
}

bool
static SQL_OperDoubleSmallEQUBig(SQLVariant& p_left,SQLVariant& p_right)
{
  return p_left.GetAsDouble() <= SQL_UBIGINTToDouble(p_right.GetAsUBigInt());
}

bool
static SQL_OperBitSmallEQUBig(SQLVariant& /*p_left*/,SQLVariant& p_right)
{
  return p_right.GetAsUBigInt() != 0;
}

bool
static SQL_OperSTinySmallEQUBig(SQLVariant& p_left,SQLVariant& p_right)
{
  return p_left.GetAsUTinyInt() <= SQL_UBIGINTToUTinyInt(p_right.GetAsUBigInt());
}

bool
static SQL_OperUTinySmallEQUBig(SQLVariant& p_left,SQLVariant& p_right)
{
  return p_left.GetAsUTinyInt() <= SQL_UBIGINTToUTinyInt(p_right.GetAsUBigInt());
}

bool
static SQL_OperSBigSmallEQUBig(SQLVariant& p_left,SQLVariant& p_right)
{
  return SQL_SBIGINTToUBIGINT(p_left.GetAsSBigInt()) <= p_right.GetAsUBigInt();
}

bool
static SQL_OperUBigSmallEQUBig(SQLVariant& p_left,SQLVariant& p_right)
{
  return p_left.GetAsUBigInt() <= p_right.GetAsUBigInt();
}

bool
static SQL_OperNumSmallEQUBig(SQLVariant& p_left,SQLVariant& p_right)
{
  return p_left.GetAsBCD().AsUInt64() <= p_right.GetAsUBigInt();
}

// NUMERIC

bool
static SQL_OperSShortSmallEQNum(SQLVariant& p_left,SQLVariant& p_right)
{
  return p_left.GetAsSShort() <= SQL_SLongToShort(p_right.GetAsBCD().AsLong());
}

bool
static SQL_OperUShortSmallEQNum(SQLVariant& p_left,SQLVariant& p_right)
{
  return p_left.GetAsUShort() <= SQL_SLongToUShort(p_right.GetAsBCD().AsLong());
}

bool
static SQL_OperSLongSmallEQNum(SQLVariant& p_left,SQLVariant& p_right)
{
  return p_left.GetAsSLong() <= p_right.GetAsBCD().AsLong();
}

bool
static SQL_OperULongSmallEQNum(SQLVariant& p_left,SQLVariant& p_right)
{
  return p_left.GetAsULong() <= SQL_BIGINTToULong(p_right.GetAsBCD().AsInt64());
}

bool
static SQL_OperFloatSmallEQNum(SQLVariant& p_left,SQLVariant& p_right)
{
  return p_left.GetAsFloat() <= (float) p_right.GetAsBCD().AsDouble();
}

bool
static SQL_OperDoubleSmallEQNum(SQLVariant& p_left,SQLVariant& p_right)
{
  return p_left.GetAsDouble() <= p_right.GetAsBCD().AsDouble();
}

bool
static SQL_OperBitSmallEQNum(SQLVariant& /*p_left*/,SQLVariant& p_right)
{
  return !p_right.GetAsBCD().IsNull();
}

bool
static SQL_OperSTinySmallEQNum(SQLVariant& p_left,SQLVariant& p_right)
{
  return p_left.GetAsSTinyInt() <= SQL_SLongToTinyInt(p_right.GetAsBCD().AsLong());
}

bool
static SQL_OperUTinySmallEQNum(SQLVariant& p_left,SQLVariant& p_right)
{
  return p_left.GetAsUTinyInt() <= SQL_SLongToUTinyInt(p_right.GetAsBCD().AsLong());
}

bool
static SQL_OperSBigSmallEQNum(SQLVariant& p_left,SQLVariant& p_right)
{
  return p_left.GetAsSBigInt() <= p_right.GetAsBCD().AsInt64();
}

bool
static SQL_OperUBigSmallEQNum(SQLVariant& p_left,SQLVariant& p_right)
{
  return p_left.GetAsUBigInt() <= p_right.GetAsBCD().AsUInt64();
}

bool
static SQL_OperNumSmallEQNum(SQLVariant& p_left,SQLVariant& p_right)
{
  return p_left.GetAsBCD() <= p_right.GetAsBCD();
}

// TYPE == GUID

bool
static SQL_OperGuidSmallEQGuid(SQLVariant& p_left,SQLVariant& p_right)
{
  SQLGUID* left  = p_left.GetAsGUID();
  SQLGUID* right = p_right.GetAsGUID();

  return memcmp(left,right,sizeof(SQLGUID)) <= 0;
}

// TYPE == BINARY

bool
static SQL_OperBinarySmallEQBinary(SQLVariant& p_left,SQLVariant& p_right)
{
  void* left   = p_left.GetAsBinary();
  void* right  = p_right.GetAsBinary();
  // Take longest of two binaries in comparison
  long  length = p_left.GetBinaryLength() < p_right.GetBinaryLength() ? p_left.GetBinaryLength() : p_right.GetBinaryLength();

  return memcmp(left,right,length) <= 0;
}

// TYPE == DATE

bool
static SQL_OperDateSmallEQDate(SQLVariant& p_left,SQLVariant& p_right)
{
  // Relies on the < operator of SQLDate
  return p_left.GetAsSQLDate() <= p_right.GetAsSQLDate();
}

bool
static SQL_OperStampSmallEQDate(SQLVariant& p_left,SQLVariant& p_right)
{
  SQLDate date = p_left.GetAsSQLTimestamp().AsSQLDate();
  return date <= p_right.GetAsSQLDate();
}

// TYPE == TIME

bool
static SQL_OperTimeSmallEQTime(SQLVariant& p_left,SQLVariant& p_right)
{
  // Relies on the < operator of SQLTime
  return p_left.GetAsSQLTime() <= p_right.GetAsSQLTime();
}

bool
static SQL_OperStampSmallEQTime(SQLVariant& p_left,SQLVariant& p_right)
{
  SQLTime time = p_left.GetAsSQLTimestamp().AsSQLTime();
  return time <= p_right.GetAsSQLTime();
}

// TYPE == TIMESTAMP

bool
static SQL_OperStampSmallEQStamp(SQLVariant& p_left,SQLVariant& p_right)
{
  // Relies on the < operator of SQLTimestamp
  return p_left.GetAsSQLTimestamp() <= p_right.GetAsSQLTimestamp();
}

// TYPE == INTERVAL_YEAR_MONTH

bool
static SQL_OperIntYMSmallEQIntYM(SQLVariant& p_left,SQLVariant& p_right)
{
  // Relies on the calculation of months in SQLInterval
  return p_left.GetAsSQLInterval().AsDatabaseDouble() <= p_right.GetAsSQLInterval().AsDatabaseDouble();
}

// TYPE == INTERVAL_DAY_SECOND

bool
static SQL_OperIntDSSmallEQIntDS(SQLVariant& p_left,SQLVariant& p_right)
{
  // Relies on the calculation of seconds in SQLInterval
  return p_left.GetAsSQLInterval().AsDatabaseDouble() <= p_right.GetAsSQLInterval().AsDatabaseDouble();
}

// OPERATOR ARRAY

static CompareFunctionArray OperatorSmallerEqual[CT_LAST][CT_LAST] =
{
                         // CT_CHAR                  CT_SSHORT                    CT_USHORT                    CT_SLONG                     CT_ULONG                    CT_FLOAT                    CT_DOUBLE                    CT_BIT                    CT_STINYINT                 CT_UTINYINT                 CT_SBIGINT                 CT_UBIGINT                 CT_NUMERIC                CT_GUID                  CT_BINARY                    CT_DATE                   CT_TIME                   CT_TIMESTAMP               CT_INTERVAL_YM             CT_INTERVAL_DS
                         // ------------------------ ---------------------------- ---------------------------- ---------------------------- --------------------------- --------------------------- ---------------------------- ------------------------- --------------------------- --------------------------- -------------------------- -------------------------- ------------------------- ------------------------ ---------------------------- ------------------------- ------------------------- -------------------------- -------------------------- ----------------------------
  /* CT_CHAR        */   {  &SQL_OperVarSmallEQChar ,&SQL_OperVarSmallEQChar     ,&SQL_OperVarSmallEQChar     ,&SQL_OperVarSmallEQChar     ,&SQL_OperVarSmallEQChar    ,&SQL_OperVarSmallEQChar    ,&SQL_OperVarSmallEQChar     ,&SQL_OperVarSmallEQChar  ,&SQL_OperVarSmallEQChar    ,&SQL_OperVarSmallEQChar    ,&SQL_OperVarSmallEQChar   ,&SQL_OperVarSmallEQChar   ,&SQL_OperVarSmallEQChar  ,&SQL_OperVarSmallEQChar ,nullptr                     ,&SQL_OperVarSmallEQChar  ,&SQL_OperVarSmallEQChar  ,&SQL_OperVarSmallEQChar   ,&SQL_OperVarSmallEQChar   ,&SQL_OperVarSmallEQChar    }
  /* CT_SSHORT      */  ,{  &SQL_OperVarSmallEQChar ,&SQL_OperSShortSmallEQSShort,&SQL_OperSShortSmallEQUShort,&SQL_OperSShortSmallEQSLong ,&SQL_OperSShortSmallEQULong,&SQL_OperSShortSmallEQFloat,&SQL_OperSShortSmallEQDouble,&SQL_OperSShortSmallEQBit,&SQL_OperSShortSmallEQSTiny,&SQL_OperSShortSmallEQUTiny,&SQL_OperSShortSmallEQSBig,&SQL_OperSShortSmallEQUBig,&SQL_OperSShortSmallEQNum,nullptr                 ,nullptr                     ,nullptr                  ,nullptr                  ,nullptr                   ,nullptr                   ,nullptr                    }
  /* CT_USHORT      */  ,{  &SQL_OperVarSmallEQChar ,&SQL_OperUShortSmallEQSShort,&SQL_OperUShortSmallEQUShort,&SQL_OperUShortSmallEQSLong ,&SQL_OperUShortSmallEQULong,&SQL_OperUShortSmallEQFloat,&SQL_OperUShortSmallEQDouble,&SQL_OperUShortSmallEQBit,&SQL_OperUShortSmallEQSTiny,&SQL_OperUShortSmallEQUTiny,&SQL_OperUShortSmallEQSBig,&SQL_OperUShortSmallEQUBig,&SQL_OperUShortSmallEQNum,nullptr                 ,nullptr                     ,nullptr                  ,nullptr                  ,nullptr                   ,nullptr                   ,nullptr                    }
  /* CT_SLONG       */  ,{  &SQL_OperVarSmallEQChar ,&SQL_OperSLongSmallEQSShort ,&SQL_OperSLongSmallEQUShort ,&SQL_OperSLongSmallEQSLong  ,&SQL_OperSLongSmallEQULong ,&SQL_OperSLongSmallEQFloat ,&SQL_OperSLongSmallEQDouble ,&SQL_OperSLongSmallEQBit ,&SQL_OperSLongSmallEQSTiny ,&SQL_OperSLongSmallEQUTiny ,&SQL_OperSLongSmallEQSBig ,&SQL_OperSLongSmallEQUBig ,&SQL_OperSLongSmallEQNum ,nullptr                 ,nullptr                     ,nullptr                  ,nullptr                  ,nullptr                   ,nullptr                   ,nullptr                    }
  /* CT_ULONG       */  ,{  &SQL_OperVarSmallEQChar ,&SQL_OperULongSmallEQSShort ,&SQL_OperUlongSmallEQUShort ,&SQL_OperULongSmallEQSLong  ,&SQL_OperULongSmallEQULong ,&SQL_OperULongSmallEQFloat ,&SQL_OperULongSmallEQDouble ,&SQL_OperULongSmallEQBit ,&SQL_OperULongSmallEQSTiny ,&SQL_OperULongSmallEQUTiny ,&SQL_OperULongSmallEQSBig ,&SQL_OperULongSmallEQUBig ,&SQL_OperULongSmallEQNum ,nullptr                 ,nullptr                     ,nullptr                  ,nullptr                  ,nullptr                   ,nullptr                   ,nullptr                    }
  /* CT_FLOAT       */  ,{  &SQL_OperVarSmallEQChar ,&SQL_OperFloatSmallEQSShort ,&SQL_OperFloatSmallEQUShort ,&SQL_OperFloatSmallEQSLong  ,&SQL_OperFloatSmallEQULong ,&SQL_OperFloatSmallEQFloat ,&SQL_OperFloatSmallEQDouble ,&SQL_OperFloatSmallEQBit ,&SQL_OperFloatSmallEQSTiny ,&SQL_OperFloatSmallEQUTiny ,&SQL_OperFloatSmallEQSBig ,&SQL_OperFloatSmallEQUBig ,&SQL_OperFloatSmallEQNum ,nullptr                 ,nullptr                     ,nullptr                  ,nullptr                  ,nullptr                   ,nullptr                   ,nullptr                    }
  /* CT_DOUBLE      */  ,{  &SQL_OperVarSmallEQChar ,&SQL_OperDoubleSmallEQSShort,&SQL_OperDoubleSmallEQUShort,&SQL_OperDoubleESmallEQSLong,&SQL_OperDoubleSmallEQULong,&SQL_OperDoubleSmallEQFloat,&SQL_OperDoubleSmallEQDouble,&SQL_OperDoubleSmallEQBit,&SQL_OperDoubleSmallEQSTiny,&SQL_OperDoubleSmallEQUTiny,&SQL_OperDoubleSmallEQSBig,&SQL_OperDoubleSmallEQUBig,&SQL_OperDoubleSmallEQNum,nullptr                 ,nullptr                     ,nullptr                  ,nullptr                  ,nullptr                   ,nullptr                   ,nullptr                    }
  /* CT_BIT         */  ,{  &SQL_OperVarSmallEQChar ,&SQL_OperBitSmallEQSShort   ,&SQL_OperBitSmallEQUShort   ,&SQL_OperBitSmallEQSLong    ,&SQL_OperBitSmallEQULong   ,&SQL_OperBitSmallEQFloat   ,&SQL_OperBitSmallEQDouble   ,&SQL_OperBitSmallEQBit   ,&SQL_OperBitSmallEQSTiny   ,&SQL_OperBitSmallEQUTiny   ,&SQL_OperBitSmallEQSBig   ,&SQL_OperBitSmallEQUBig   ,&SQL_OperBitSmallEQNum   ,nullptr                 ,nullptr                     ,nullptr                  ,nullptr                  ,nullptr                   ,nullptr                   ,nullptr                    }
  /* CT_STINYINT    */  ,{  &SQL_OperVarSmallEQChar ,&SQL_OperSTinySmallEQSShort ,&SQL_OperSTinySmallEQUShort ,&SQL_OperSTinySmallEQSLong  ,&SQL_OperSTinySmallEQULong ,&SQL_OperSTinySmallEQFloat ,&SQL_OperSTinySmallEQDouble ,&SQL_OperSTinySmallEQBit ,&SQL_OperSTinySmallEQSTiny ,&SQL_OperSTinySmallEQUTiny ,&SQL_OperSTinySmallEQSBig ,&SQL_OperSTinySmallEQUBig ,&SQL_OperSTinySmallEQNum ,nullptr                 ,nullptr                     ,nullptr                  ,nullptr                  ,nullptr                   ,nullptr                   ,nullptr                    }
  /* CT_UTINYINT    */  ,{  &SQL_OperVarSmallEQChar ,&SQL_OperUTinySmallEQSShort ,&SQL_OperUTinySmallEQUShort ,&SQL_OperUTinySmallEQSLong  ,&SQL_OperUTinySmallEQULong ,&SQL_OperUTinySmallEQFloat ,&SQL_OperUTinySmallEQDouble ,&SQL_OperUTinySmallEQBit ,&SQL_OperUTinySmallEQSTiny ,&SQL_OperUTinySmallEQUTiny ,&SQL_OperUTinySmallEQSBig ,&SQL_OperUTinySmallEQUBig ,&SQL_OperUTinySmallEQNum ,nullptr                 ,nullptr                     ,nullptr                  ,nullptr                  ,nullptr                   ,nullptr                   ,nullptr                    }
  /* CT_SBIGINT     */  ,{  &SQL_OperVarSmallEQChar ,&SQL_OperSBigSmallEQSShort  ,&SQL_OperSBigSmallEQUShort  ,&SQL_OperSBigSmallEQSLong   ,&SQL_OperSBigSmallEQULong  ,&SQL_OperSBigSmallEQFloat  ,&SQL_OperSBigSmallEQDouble  ,&SQL_OperSBigSmallEQBit  ,&SQL_OperSBigSmallEQSTiny  ,&SQL_OperSBigSmallEQUTiny  ,&SQL_OperSBigSmallEQSBig  ,&SQL_OperSBigSmallEQUBig  ,&SQL_OperSBigSmallEQNum  ,nullptr                 ,nullptr                     ,nullptr                  ,nullptr                  ,nullptr                   ,nullptr                   ,nullptr                    }
  /* CT_UBIGINT     */  ,{  &SQL_OperVarSmallEQChar ,&SQL_OperUBigSmallEQSShort  ,&SQL_OperUBigSmallEQUShort  ,&SQL_OperUBigSmallEQSLong   ,&SQL_OperUBigSmallEQULong  ,&SQL_OperUBigSmallEQFloat  ,&SQL_OperUBigSmallEQDouble  ,&SQL_OperUBigSmallEQBit  ,&SQL_OperUBigSmallEQSTiny  ,&SQL_OperUBigSmallEQUTiny  ,&SQL_OperUBigSmallEQSBig  ,&SQL_OperUBigSmallEQUBig  ,&SQL_OperUBigSmallEQNum  ,nullptr                 ,nullptr                     ,nullptr                  ,nullptr                  ,nullptr                   ,nullptr                   ,nullptr                    }
  /* CT_NUMERIC     */  ,{  &SQL_OperVarSmallEQChar ,&SQL_OperNumSmallEQSShort   ,&SQL_OperNumSmallEQUShort   ,&SQL_OperNumSmallEQSLong    ,&SQL_OperNumSmallEQULong   ,&SQL_OperNumSmallEQFloat   ,&SQL_OperNumSmallEQDouble   ,&SQL_OperNumSmallEQBit   ,&SQL_OperNumSmallEQSTiny   ,&SQL_OperNumSmallEQUTiny   ,&SQL_OperNumSmallEQSBig   ,&SQL_OperNumSmallEQUBig   ,&SQL_OperNumSmallEQNum   ,nullptr                 ,nullptr                     ,nullptr                  ,nullptr                  ,nullptr                   ,nullptr                   ,nullptr                    }
  /* CT_GUID        */  ,{  &SQL_OperGuidSmallEQChar,nullptr                     ,nullptr                     ,nullptr                     ,nullptr                    ,nullptr                    ,nullptr                     ,nullptr                  ,nullptr                    ,nullptr                    ,nullptr                   ,nullptr                   ,nullptr                  ,&SQL_OperGuidSmallEQGuid,nullptr                     ,nullptr                  ,nullptr                  ,nullptr                   ,nullptr                   ,nullptr                    }
  /* CT_BINARY      */  ,{  nullptr                 ,nullptr                     ,nullptr                     ,nullptr                     ,nullptr                    ,nullptr                    ,nullptr                     ,nullptr                  ,nullptr                    ,nullptr                    ,nullptr                   ,nullptr                   ,nullptr                  ,nullptr                 ,&SQL_OperBinarySmallEQBinary,nullptr                  ,nullptr                  ,nullptr                   ,nullptr                   ,nullptr                    }
  /* CT_DATE        */  ,{  &SQL_OperVarSmallEQChar ,nullptr                     ,nullptr                     ,nullptr                     ,nullptr                    ,nullptr                    ,nullptr                     ,nullptr                  ,nullptr                    ,nullptr                    ,nullptr                   ,nullptr                   ,nullptr                  ,nullptr                 ,nullptr                     ,&SQL_OperDateSmallEQDate ,nullptr                  ,nullptr                   ,nullptr                   ,nullptr                    }
  /* CT_TIME        */  ,{  &SQL_OperVarSmallEQChar ,nullptr                     ,nullptr                     ,nullptr                     ,nullptr                    ,nullptr                    ,nullptr                     ,nullptr                  ,nullptr                    ,nullptr                    ,nullptr                   ,nullptr                   ,nullptr                  ,nullptr                 ,nullptr                     ,nullptr                  ,&SQL_OperTimeSmallEQTime ,nullptr                   ,nullptr                   ,nullptr                    }
  /* CT_TIMESTAMP   */  ,{  &SQL_OperVarSmallEQChar ,nullptr                     ,nullptr                     ,nullptr                     ,nullptr                    ,nullptr                    ,nullptr                     ,nullptr                  ,nullptr                    ,nullptr                    ,nullptr                   ,nullptr                   ,nullptr                  ,nullptr                 ,nullptr                     ,&SQL_OperStampSmallEQDate,&SQL_OperStampSmallEQTime,&SQL_OperStampSmallEQStamp,nullptr                   ,nullptr                    }
  /* CT_INTERVAL_YM */  ,{  &SQL_OperVarSmallEQChar ,nullptr                     ,nullptr                     ,nullptr                     ,nullptr                    ,nullptr                    ,nullptr                     ,nullptr                  ,nullptr                    ,nullptr                    ,nullptr                   ,nullptr                   ,nullptr                  ,nullptr                 ,nullptr                     ,nullptr                  ,nullptr                  ,nullptr                   ,&SQL_OperIntYMSmallEQIntYM,nullptr                    }
  /* CT_INTERVAL_DS */  ,{  &SQL_OperVarSmallEQChar ,nullptr                     ,nullptr                     ,nullptr                     ,nullptr                    ,nullptr                    ,nullptr                     ,nullptr                  ,nullptr                    ,nullptr                    ,nullptr                   ,nullptr                   ,nullptr                  ,nullptr                 ,nullptr                     ,nullptr                  ,nullptr                  ,nullptr                   ,nullptr                   ,&SQL_OperIntDSSmallEQIntDS }
};

// Smaller-Equal operator for SQLVariant
bool
SQLVariant::operator<=(SQLVariant& p_right)
{
  // If one of both is NULL, the result is false
  if(IsNULL() || p_right.IsNULL())
  {
    return false;
  }

  // Getting the concise type
  SQLConciseType left  = SQLTypeToConciseType(m_datatype);
  SQLConciseType right = SQLTypeToConciseType(p_right.m_datatype);

  // Check whether both datatypes are valid
  if(left == CT_LAST || right == CT_LAST)
  {
    return false;
  }

  // Find our comparison function
  OperatorCompare function = OperatorSmallerEqual[left][right].function;
  if(function)
  {
    return (*function)(*this,p_right);
  }
  // No compare function found
  // Data types are not comparable
  CString leftType  = FindDatatype(m_datatype);
  CString rightType = FindDatatype(p_right.m_datatype);
  CString error;
  error.Format("Cannot do the smaller-equal operator on (%s <= %s)",leftType.GetString(),rightType.GetString());
  throw StdException(error);
}

// End of namespace
}
