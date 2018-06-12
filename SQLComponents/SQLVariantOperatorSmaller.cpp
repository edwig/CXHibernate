////////////////////////////////////////////////////////////////////////
//
// File: SQLVariantOperatorSmaller.cpp
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
static SQL_OperVarSmallerChar(SQLVariant& p_left,SQLVariant& p_right)
{
  CString leftString,rightString;
  p_left .GetAsString(leftString);
  p_right.GetAsString(rightString);

  return leftString.Compare(rightString) < 0;
}

bool
static SQL_OperGuidSmallerChar(SQLVariant& p_left,SQLVariant& p_right)
{
  CString leftString,rightString;
  p_left .GetAsString(leftString);
  p_right.GetAsString(rightString);

  leftString. TrimLeft ('{');
  leftString. TrimRight('}');
  rightString.TrimLeft ('}');
  rightString.TrimRight('{');

  return leftString.Compare(rightString) < 0;
}

// SIGNED SHORT

bool
static SQL_OperSShortSmallerSShort(SQLVariant& p_left,SQLVariant& p_right)
{
  return p_left.GetAsSShort() < p_right.GetAsSShort();
}

bool
static SQL_OperUShortSmallerSShort(SQLVariant& p_left,SQLVariant& p_right)
{
  return p_left.GetAsUShort() < SQL_ShortToUShort(p_right.GetAsSShort());
}

bool
static SQL_OperSLongSmallerSShort(SQLVariant& p_left,SQLVariant& p_right)
{
  return SQL_SLongToShort(p_left.GetAsSLong()) < p_right.GetAsSShort();
}

bool
static SQL_OperULongSmallerSShort(SQLVariant& p_left,SQLVariant& p_right)
{
  return SQL_ULongToUShort(p_left.GetAsULong()) < SQL_ShortToUShort(p_right.GetAsSShort());
}

bool
static SQL_OperFloatSmallerSShort(SQLVariant& p_left,SQLVariant& p_right)
{
  return SQL_FloatToShort(p_left.GetAsFloat()) < p_right.GetAsSShort();
}

bool
static SQL_OperDoubleSmallerSShort(SQLVariant& p_left,SQLVariant& p_right)
{
  return SQL_DoubleToShort(p_left.GetAsDouble()) < p_right.GetAsSShort();
}

bool
static SQL_OperBitSmallerSShort(SQLVariant& p_left,SQLVariant& p_right)
{
  return p_left.GetAsBit() == 0 && p_right.GetAsSShort() != 0;
}

bool   
static SQL_OperSTinySmallerSShort(SQLVariant& p_left,SQLVariant& p_right)
{
  return p_left.GetAsSTinyInt() < SQL_ShortToTinyInt(p_right.GetAsSShort());
}

bool
static SQL_OperUTinySmallerSShort(SQLVariant& p_left,SQLVariant& p_right)
{
  return p_left.GetAsUTinyInt() < SQL_ShortToUTinyInt(p_right.GetAsSShort());
}

bool
static SQL_OperSBigSmallerSShort(SQLVariant& p_left,SQLVariant& p_right)
{
  return SQL_BIGINTToShort(p_left.GetAsSBigInt()) < p_right.GetAsSShort();
}

bool
static SQL_OperUBigSmallerSShort(SQLVariant& p_left,SQLVariant& p_right)
{
  return SQL_UBIGINTToShort(p_left.GetAsUBigInt()) < p_right.GetAsSShort();
}

bool
static SQL_OperNumSmallerSShort(SQLVariant& p_left,SQLVariant& p_right)
{
  return SQL_SLongToShort(p_left.GetAsBCD().AsLong()) < p_right.GetAsSShort();
}

// UNSIGNED SHORT

bool   
static SQL_OperSShortSmallerUShort(SQLVariant& p_left,SQLVariant& p_right)
{
  return SQL_ShortToUShort(p_left.GetAsSShort()) < p_right.GetAsUShort();
}

bool   
static SQL_OperUShortSmallerUShort(SQLVariant& p_left,SQLVariant& p_right)
{
  return p_left.GetAsUShort() < p_right.GetAsUShort();
}

bool
static SQL_OperSLongSmallerUShort(SQLVariant& p_left,SQLVariant& p_right)
{
  return SQL_SLongToUShort(p_left.GetAsSLong()) < p_right.GetAsUShort();
}

bool
static SQL_OperUlongSmallerUShort(SQLVariant& p_left,SQLVariant& p_right)
{
  return SQL_ULongToUShort(p_left.GetAsULong()) < p_right.GetAsUShort();
}

bool
static SQL_OperFloatSmallerUShort(SQLVariant& p_left,SQLVariant& p_right)
{
  return SQL_FloatToUShort(p_left.GetAsFloat()) < p_right.GetAsUShort();
}

bool
static SQL_OperDoubleSmallerUShort(SQLVariant& p_left,SQLVariant& p_right)
{
  return SQL_DoubleToUShort(p_left.GetAsDouble()) < p_right.GetAsUShort();
}

bool
static SQL_OperBitSmallerUShort(SQLVariant& p_left,SQLVariant& p_right)
{
  return p_left.GetAsBit() == 0 && p_right.GetAsUShort() != 0;
}

bool
static SQL_OperSTinySmallerUShort(SQLVariant& p_left,SQLVariant& p_right)
{
  return p_left.GetAsSTinyInt() < SQL_UShortToTinyInt(p_right.GetAsUShort());
}

bool
static SQL_OperUTinySmallerUShort(SQLVariant& p_left,SQLVariant& p_right)
{
  return p_left.GetAsUTinyInt() < SQL_UShortToUTinyInt(p_right.GetAsUShort());
}

bool
static SQL_OperSBigSmallerUShort(SQLVariant& p_left,SQLVariant& p_right)
{
  return SQL_BIGINTToUShort(p_left.GetAsSBigInt()) < p_right.GetAsUShort();
}

bool
static SQL_OperUBigSmallerUShort(SQLVariant& p_left,SQLVariant& p_right)
{
  return SQL_UBIGINTToUShort(p_left.GetAsUBigInt()) < p_right.GetAsUShort();
}

bool
static SQL_OperNumSmallerUShort(SQLVariant& p_left,SQLVariant& p_right)
{
  return SQL_SLongToUShort(p_left.GetAsBCD().AsLong()) < p_right.GetAsUShort();
}

// SIGNED LONG

bool   
static SQL_OperSShortSmallerSLong(SQLVariant& p_left,SQLVariant& p_right)
{
  return p_left.GetAsSShort() < SQL_SLongToShort(p_right.GetAsSLong());
}

bool
static SQL_OperUShortSmallerSLong(SQLVariant& p_left,SQLVariant& p_right)
{
  return p_left.GetAsUShort() < SQL_SLongToUShort(p_right.GetAsSLong());
}

bool
static SQL_OperSLongSmallerSLong(SQLVariant& p_left,SQLVariant& p_right)
{
  return p_left.GetAsSLong() < p_right.GetAsSLong();
}

bool
static SQL_OperULongSmallerSLong(SQLVariant& p_left,SQLVariant& p_right)
{
  return p_left.GetAsULong() < SQL_LongToULong(p_right.GetAsSLong());
}

bool
static SQL_OperFloatSmallerSLong(SQLVariant& p_left,SQLVariant& p_right)
{
  return SQL_FloatToLong(p_left.GetAsFloat()) < p_right.GetAsSLong();
}

bool
static SQL_OperDoubleESmallerSLong(SQLVariant& p_left,SQLVariant& p_right)
{
  return SQL_DoubleToLong(p_left.GetAsDouble()) < p_right.GetAsSLong();
}

bool
static SQL_OperBitSmallerSLong(SQLVariant& p_left,SQLVariant& p_right)
{
  return p_left.GetAsBit() == 0 && p_right.GetAsSLong() != 0;
}

bool
static SQL_OperSTinySmallerSLong(SQLVariant& p_left,SQLVariant& p_right)
{
  return p_left.GetAsSTinyInt() < SQL_SLongToTinyInt(p_right.GetAsSLong());
}

bool
static SQL_OperUTinySmallerSLong(SQLVariant& p_left,SQLVariant& p_right)
{
  return p_left.GetAsUTinyInt() < SQL_SLongToUTinyInt(p_right.GetAsSLong());
}

bool
static SQL_OperSBigSmallerSLong(SQLVariant& p_left,SQLVariant& p_right)
{
  return SQL_BIGINTToLong(p_left.GetAsSBigInt()) < p_right.GetAsSLong();
}

bool
static SQL_OperUBigSmallerSLong(SQLVariant& p_left,SQLVariant& p_right)
{
  return SQL_UBIGINTToLong(p_left.GetAsUBigInt()) < p_right.GetAsSLong();
}

bool
static SQL_OperNumSmallerSLong(SQLVariant& p_left,SQLVariant& p_right)
{
  return p_left.GetAsBCD().AsLong() < p_right.GetAsSLong();
}

// UNSIGNED LONG

bool   
static SQL_OperSShortSmallerULong(SQLVariant& p_left,SQLVariant& p_right)
{
  return SQL_ShortToUShort(p_left.GetAsSShort()) < SQL_ULongToUShort(p_right.GetAsULong());
}

bool
static SQL_OperUShortSmallerULong(SQLVariant& p_left,SQLVariant& p_right)
{
  return p_left.GetAsUShort() < SQL_ULongToUShort(p_right.GetAsULong());
}

bool
static SQL_OperSLongSmallerULong(SQLVariant& p_left,SQLVariant& p_right)
{
  return SQL_LongToULong(p_left.GetAsSLong()) < p_right.GetAsULong();
}

bool
static SQL_OperULongSmallerULong(SQLVariant& p_left,SQLVariant& p_right)
{
  return p_left.GetAsULong() < p_right.GetAsULong();
}

bool
static SQL_OperFloatSmallerULong(SQLVariant& p_left,SQLVariant& p_right)
{
  return SQL_FloatToULong(p_left.GetAsFloat()) < p_right.GetAsULong();
}

bool
static SQL_OperDoubleSmallerULong(SQLVariant& p_left,SQLVariant& p_right)
{
  return SQL_DoubleToULong(p_left.GetAsDouble()) < p_right.GetAsULong();
}

bool
static SQL_OperBitSmallerULong(SQLVariant& p_left,SQLVariant& p_right)
{
  return p_left.GetAsBit() == 0 && p_right.GetAsULong() != 0;
}

bool
static SQL_OperSTinySmallerULong(SQLVariant& p_left,SQLVariant& p_right)
{
  return SQL_TinyIntToUTinyInt(p_left.GetAsSTinyInt()) < SQL_ULongToUTinyInt(p_right.GetAsULong());
}

bool
static SQL_OperUTinySmallerULong(SQLVariant& p_left,SQLVariant& p_right)
{
  return p_left.GetAsUTinyInt() < SQL_ULongToUTinyInt(p_right.GetAsULong());
}

bool
static SQL_OperSBigSmallerULong(SQLVariant& p_left,SQLVariant& p_right)
{
  return SQL_BIGINTToULong(p_left.GetAsSBigInt()) < p_right.GetAsULong();
}

bool
static SQL_OperUBigSmallerULong(SQLVariant& p_left,SQLVariant& p_right)
{
  return SQL_UBIGINTToULong(p_left.GetAsUBigInt()) < p_right.GetAsULong();
}

bool
static SQL_OperNumSmallerULong(SQLVariant& p_left,SQLVariant& p_right)
{
  return SQL_BIGINTToULong(p_left.GetAsBCD().AsInt64()) < p_right.GetAsULong();
}

// FLOAT

bool   
static SQL_OperSShortSmallerFloat(SQLVariant& p_left,SQLVariant& p_right)
{
  return p_left.GetAsSShort() < SQL_FloatToShort(p_right.GetAsFloat());
}

bool   
static SQL_OperUShortSmallerFloat(SQLVariant& p_left,SQLVariant& p_right)
{
  return p_left.GetAsUShort() < SQL_FloatToUShort(p_right.GetAsFloat());
}

bool
static SQL_OperSLongSmallerFloat(SQLVariant& p_left,SQLVariant& p_right)
{
  return p_left.GetAsSLong() < SQL_FloatToLong(p_right.GetAsFloat());
}

bool
static SQL_OperULongSmallerFloat(SQLVariant& p_left,SQLVariant& p_right)
{
  return p_left.GetAsULong() < SQL_FloatToULong(p_right.GetAsFloat());
}

bool
static SQL_OperFloatSmallerFloat(SQLVariant& p_left,SQLVariant& p_right)
{
  return p_left.GetAsFloat() < p_right.GetAsFloat();
}

bool
static SQL_OperDoubleSmallerFloat(SQLVariant& p_left,SQLVariant& p_right)
{
  return SQL_DoubleToFloat(p_left.GetAsDouble()) < p_right.GetAsFloat();
}

bool
static SQL_OperBitSmallerFloat(SQLVariant& p_left,SQLVariant& p_right)
{
  return p_left.GetAsBit() == 0 && p_right.GetAsFloat() != 0.0;
}

bool
static SQL_OperSTinySmallerFloat(SQLVariant& p_left,SQLVariant& p_right)
{
  return p_left.GetAsSTinyInt() < SQL_FloatToTinyInt(p_right.GetAsFloat());
}

bool
static SQL_OperUTinySmallerFloat(SQLVariant& p_left,SQLVariant& p_right)
{
  return p_left.GetAsUTinyInt() < SQL_FloatToUTinyInt(p_right.GetAsFloat());
}

bool
static SQL_OperSBigSmallerFloat(SQLVariant& p_left,SQLVariant& p_right)
{
  return p_left.GetAsSBigInt() < SQL_FloatToBIGINT(p_right.GetAsFloat());
}

bool
static SQL_OperUBigSmallerFloat(SQLVariant& p_left,SQLVariant& p_right)
{
  return p_left.GetAsUBigInt() < SQL_FloatToUBIGINT(p_right.GetAsFloat());
}

bool
static SQL_OperNumSmallerFloat(SQLVariant& p_left,SQLVariant& p_right)
{
  return p_left.GetAsBCD().AsDouble() < (double) p_right.GetAsFloat();
}

// DOUBLE

bool
static SQL_OperSShortSmallerDouble(SQLVariant& p_left,SQLVariant& p_right)
{
  return p_left.GetAsSShort() < SQL_DoubleToShort(p_right.GetAsDouble());
}

bool
static SQL_OperUShortSmallerDouble(SQLVariant& p_left,SQLVariant& p_right)
{
  return p_left.GetAsUShort() < SQL_DoubleToUShort(p_right.GetAsDouble());
}

bool
static SQL_OperSLongSmallerDouble(SQLVariant& p_left,SQLVariant& p_right)
{
  return p_left.GetAsSLong() < SQL_DoubleToLong(p_right.GetAsDouble());
}

bool
static SQL_OperULongSmallerDouble(SQLVariant& p_left,SQLVariant& p_right)
{
  return p_left.GetAsULong() < SQL_DoubleToULong(p_right.GetAsDouble());
}

bool
static SQL_OperFloatSmallerDouble(SQLVariant& p_left,SQLVariant& p_right)
{
  return p_left.GetAsFloat() < SQL_DoubleToFloat(p_right.GetAsDouble());
}

bool
static SQL_OperDoubleSmallerDouble(SQLVariant& p_left,SQLVariant& p_right)
{
  return p_left.GetAsDouble() < p_right.GetAsDouble();
}

bool
static SQL_OperBitSmallerDouble(SQLVariant& p_left,SQLVariant& p_right)
{
  return p_left.GetAsBit() == 0 && p_right.GetAsDouble() != 0.0;
}

bool
static SQL_OperSTinySmallerDouble(SQLVariant& p_left,SQLVariant& p_right)
{
  return p_left.GetAsSTinyInt() < SQL_DoubleToTinyInt(p_right.GetAsDouble());
}

bool
static SQL_OperUTinySmallerDouble(SQLVariant& p_left,SQLVariant& p_right)
{
  return p_left.GetAsUTinyInt() < SQL_DoubleToUTinyInt(p_right.GetAsDouble());
}

bool
static SQL_OperSBigSmallerDouble(SQLVariant& p_left,SQLVariant& p_right)
{
  return p_left.GetAsSBigInt() < SQL_DoubleToBIGINT(p_right.GetAsDouble());
}

bool
static SQL_OperUBigSmallerDouble(SQLVariant& p_left,SQLVariant& p_right)
{
  return p_left.GetAsUBigInt() < SQL_DoubleToUBIGINT(p_right.GetAsDouble());
}

bool
static SQL_OperNumSmallerDouble(SQLVariant& p_left,SQLVariant& p_right)
{
  return p_left.GetAsBCD().AsDouble() < p_right.GetAsDouble();
}

// BIT

bool   
static SQL_OperSShortSmallerBit(SQLVariant& p_left,SQLVariant& p_right)
{
  return p_left.GetAsSShort() == 0 && p_right.GetAsBit() != 0;
}

bool
static SQL_OperUShortSmallerBit(SQLVariant& p_left,SQLVariant& p_right)
{
  return p_left.GetAsUShort() == 0 && p_right.GetAsBit() != 0;
}

bool   
static SQL_OperSLongSmallerBit(SQLVariant& p_left,SQLVariant& p_right)
{
  return p_left.GetAsSLong() == 0 && p_right.GetAsBit() != 0;
}

bool
static SQL_OperULongSmallerBit(SQLVariant& p_left,SQLVariant& p_right)
{
  return p_left.GetAsULong() == 0 && p_right.GetAsBit() != 0;
}

bool
static SQL_OperFloatSmallerBit(SQLVariant& p_left,SQLVariant& p_right)
{
  return p_left.GetAsFloat() == 0.0 && p_right.GetAsBit() != 0;
}

bool
static SQL_OperDoubleSmallerBit(SQLVariant& p_left,SQLVariant& p_right)
{
  return p_left.GetAsDouble() == 0.0 && p_right.GetAsBit() != 0;
}

bool
static SQL_OperBitSmallerBit(SQLVariant& p_left,SQLVariant& p_right)
{
  return p_left.GetAsBit() < p_right.GetAsBit();
}

bool
static SQL_OperSTinySmallerBit(SQLVariant& p_left,SQLVariant& p_right)
{
  return p_left.GetAsSTinyInt() == 0 && p_right.GetAsBit() != 0;
}

bool
static SQL_OperUTinySmallerBit(SQLVariant& p_left,SQLVariant& p_right)
{
  return p_left.GetAsUTinyInt() == 0 && p_right.GetAsBit() != 0;
}

bool
static SQL_OperSBigSmallerBit(SQLVariant& p_left,SQLVariant& p_right)
{
  return p_left.GetAsSBigInt() == 0 && p_right.GetAsBit() != 0;
}

bool
static SQL_OperUBigSmallerBit(SQLVariant& p_left,SQLVariant& p_right)
{
  return p_left.GetAsUBigInt() == 0 && p_right.GetAsBit() != 0;
}

bool
static SQL_OperNumSmallerBit(SQLVariant& p_left,SQLVariant& p_right)
{
  return p_left.GetAsBCD().IsNull() && p_right.GetAsBit() != 0;
}

// SIGNED TINYINT

bool
static SQL_OperSShortSmallerSTiny(SQLVariant& p_left,SQLVariant& p_right)
{
  return SQL_ShortToTinyInt(p_left.GetAsSShort()) < p_right.GetAsSTinyInt();
}

bool
static SQL_OperUShortSmallerSTiny(SQLVariant& p_left,SQLVariant& p_right)
{
  return SQL_UShortToTinyInt(p_left.GetAsUShort()) < p_right.GetAsSTinyInt();
}

bool
static SQL_OperSLongSmallerSTiny(SQLVariant& p_left,SQLVariant& p_right)
{
  return SQL_SLongToTinyInt(p_left.GetAsSLong()) < p_right.GetAsSTinyInt();
}

bool
static SQL_OperULongSmallerSTiny(SQLVariant& p_left,SQLVariant& p_right)
{
  return SQL_ULongToTinyInt(p_left.GetAsULong()) < p_right.GetAsSTinyInt();
}

bool
static SQL_OperFloatSmallerSTiny(SQLVariant& p_left,SQLVariant& p_right)
{
  return SQL_FloatToTinyInt(p_left.GetAsFloat()) < p_right.GetAsSTinyInt();
}

bool
static SQL_OperDoubleSmallerSTiny(SQLVariant& p_left,SQLVariant& p_right)
{
  return SQL_DoubleToTinyInt(p_left.GetAsDouble()) < p_right.GetAsSTinyInt();
}

bool
static SQL_OperBitSmallerSTiny(SQLVariant& p_left,SQLVariant& p_right)
{
  return p_left.GetAsBit() == 0 && p_right.GetAsSTinyInt() != 0;
}

bool
static SQL_OperSTinySmallerSTiny(SQLVariant& p_left,SQLVariant& p_right)
{
  return p_left.GetAsSTinyInt() < p_right.GetAsSTinyInt();
}

bool
static SQL_OperUTinySmallerSTiny(SQLVariant& p_left,SQLVariant& p_right)
{
  return p_left.GetAsUTinyInt() < SQL_TinyIntToUTinyInt(p_right.GetAsSTinyInt());
}

bool
static SQL_OperSBigSmallerSTiny(SQLVariant& p_left,SQLVariant& p_right)
{
  return SQL_SBIGINTToTinyInt(p_left.GetAsSBigInt()) < p_right.GetAsSTinyInt();
}

bool
static SQL_OperUBigSmallerSTiny(SQLVariant& p_left,SQLVariant& p_right)
{
  return SQL_UBIGINTToTinyInt(p_left.GetAsUBigInt()) < p_right.GetAsSTinyInt();
}

bool
static SQL_OperNumSmallerSTiny(SQLVariant& p_left,SQLVariant& p_right)
{
  return SQL_SLongToTinyInt(p_left.GetAsBCD().AsLong()) < p_right.GetAsSTinyInt();
}

// UNSIGNED TINYINT

bool
static SQL_OperSShortSmallerUTiny(SQLVariant& p_left,SQLVariant& p_right)
{
  return SQL_ShortToUTinyInt(p_left.GetAsSShort()) < p_right.GetAsUTinyInt();
}

bool
static SQL_OperUShortSmallerUTiny(SQLVariant& p_left,SQLVariant& p_right)
{
  return SQL_UShortToUTinyInt(p_left.GetAsUShort()) < p_right.GetAsUTinyInt();
}

bool
static SQL_OperSLongSmallerUTiny(SQLVariant& p_left,SQLVariant& p_right)
{
  return SQL_SLongToUTinyInt(p_left.GetAsSLong()) < p_right.GetAsUTinyInt();
}

bool
static SQL_OperULongSmallerUTiny(SQLVariant& p_left,SQLVariant& p_right)
{
  return SQL_ULongToUTinyInt(p_left.GetAsULong()) < p_right.GetAsUTinyInt();
}

bool
static SQL_OperFloatSmallerUTiny(SQLVariant& p_left,SQLVariant& p_right)
{
  return SQL_FloatToUTinyInt(p_left.GetAsFloat()) < p_right.GetAsUTinyInt();
}

bool
static SQL_OperDoubleSmallerUTiny(SQLVariant& p_left,SQLVariant& p_right)
{
  return SQL_DoubleToUTinyInt(p_left.GetAsDouble()) < p_right.GetAsUTinyInt();
}

bool
static SQL_OperBitSmallerUTiny(SQLVariant& p_left,SQLVariant& p_right)
{
  return p_left.GetAsBit() == 0 && p_right.GetAsUTinyInt() != 0;
}

bool
static SQL_OperSTinySmallerUTiny(SQLVariant& p_left,SQLVariant& p_right)
{
  return SQL_TinyIntToUTinyInt(p_left.GetAsSTinyInt()) < p_right.GetAsUTinyInt();
}

bool
static SQL_OperUTinySmallerUTiny(SQLVariant& p_left,SQLVariant& p_right)
{
  return p_left.GetAsUTinyInt() < p_right.GetAsUTinyInt();
}

bool
static SQL_OperSBigSmallerUTiny(SQLVariant& p_left,SQLVariant& p_right)
{
  return SQL_SBIGINTToUTinyInt(p_left.GetAsSBigInt()) < p_right.GetAsUTinyInt();
}

bool
static SQL_OperUBigSmallerUTiny(SQLVariant& p_left,SQLVariant& p_right)
{
  return SQL_UBIGINTToUTinyInt(p_left.GetAsUBigInt()) < p_right.GetAsUTinyInt();
}

bool
static SQL_OperNumSmallerUTiny(SQLVariant& p_left,SQLVariant& p_right)
{
  return SQL_SLongToUTinyInt(p_left.GetAsBCD().AsLong()) < p_right.GetAsUTinyInt();
}

// SIGNED BIGINT

bool
static SQL_OperSShortSmallerSBig(SQLVariant& p_left,SQLVariant& p_right)
{
  return p_left.GetAsSShort() < SQL_BIGINTToShort(p_right.GetAsSBigInt());
}

bool
static SQL_OperUShortSmallerSBig(SQLVariant& p_left,SQLVariant& p_right)
{
  return p_left.GetAsUShort() < SQL_BIGINTToUShort(p_right.GetAsSBigInt());
}

bool
static SQL_OperSLongSmallerSBig(SQLVariant& p_left,SQLVariant& p_right)
{
  return p_left.GetAsSLong() < SQL_BIGINTToLong(p_right.GetAsSBigInt());
}

bool
static SQL_OperULongSmallerSBig(SQLVariant& p_left,SQLVariant& p_right)
{
  return p_left.GetAsULong() < SQL_BIGINTToULong(p_right.GetAsSBigInt());
}

bool
static SQL_OperFloatSmallerSBig(SQLVariant& p_left,SQLVariant& p_right)
{
  return p_left.GetAsFloat() < SQL_BIGINTToFloat(p_right.GetAsSBigInt());
}

bool
static SQL_OperDoubleSmallerSBig(SQLVariant& p_left,SQLVariant& p_right)
{
  return p_left.GetAsDouble() < SQL_BIGINTToDouble(p_right.GetAsSBigInt());
}

bool
static SQL_OperBitSmallerSBig(SQLVariant& p_left,SQLVariant& p_right)
{
  return p_left.GetAsBit() == 0 && p_right.GetAsSBigInt() != 0;
}

bool
static SQL_OperSTinySmallerSBig(SQLVariant& p_left,SQLVariant& p_right)
{
  return p_left.GetAsSTinyInt() < SQL_SBIGINTToTinyInt(p_right.GetAsSBigInt());
}

bool
static SQL_OperUTinySmallerSBig(SQLVariant& p_left,SQLVariant& p_right)
{
  return p_left.GetAsUTinyInt() < SQL_SBIGINTToUTinyInt(p_right.GetAsSBigInt());
}

bool
static SQL_OperSBigSmallerSBig(SQLVariant& p_left,SQLVariant& p_right)
{
  return p_left.GetAsSBigInt() < p_right.GetAsSBigInt();
}

bool
static SQL_OperUBigSmallerSBig(SQLVariant& p_left,SQLVariant& p_right)
{
  return p_left.GetAsUBigInt() < SQL_SBIGINTToUBIGINT(p_right.GetAsSBigInt());
}

bool   
static SQL_OperNumSmallerSBig(SQLVariant& p_left,SQLVariant& p_right)
{
  return p_left.GetAsBCD().AsInt64() < p_right.GetAsSBigInt();
}

// UNSIGNED BIGINT

bool
static SQL_OperSShortSmallerUBig(SQLVariant& p_left,SQLVariant& p_right)
{
  return p_left.GetAsSShort() < SQL_UBIGINTToShort(p_right.GetAsUBigInt());
}

bool
static SQL_OperUShortSmallerUBig(SQLVariant& p_left,SQLVariant& p_right)
{
  return p_left.GetAsUShort() < SQL_UBIGINTToUShort(p_right.GetAsUBigInt());
}

bool
static SQL_OperSLongSmallerUBig(SQLVariant& p_left,SQLVariant& p_right)
{
  return p_left.GetAsSLong() < SQL_UBIGINTToLong(p_right.GetAsUBigInt());
}

bool
static SQL_OperULongSmallerUBig(SQLVariant& p_left,SQLVariant& p_right)
{
  return p_left.GetAsULong() < SQL_UBIGINTToULong(p_right.GetAsUBigInt());
}

bool
static SQL_OperFloatSmallerUBig(SQLVariant& p_left,SQLVariant& p_right)
{
  return p_left.GetAsFloat() < SQL_UBIGINTToFloat(p_right.GetAsUBigInt());
}

bool
static SQL_OperDoubleSmallerUBig(SQLVariant& p_left,SQLVariant& p_right)
{
  return p_left.GetAsDouble() < SQL_UBIGINTToDouble(p_right.GetAsUBigInt());
}

bool
static SQL_OperBitSmallerUBig(SQLVariant& p_left,SQLVariant& p_right)
{
  return p_left.GetAsBit() == 0 && p_right.GetAsUBigInt() != 0;
}

bool
static SQL_OperSTinySmallerUBig(SQLVariant& p_left,SQLVariant& p_right)
{
  return p_left.GetAsUTinyInt() < SQL_UBIGINTToUTinyInt(p_right.GetAsUBigInt());
}

bool
static SQL_OperUTinySmallerUBig(SQLVariant& p_left,SQLVariant& p_right)
{
  return p_left.GetAsUTinyInt() < SQL_UBIGINTToUTinyInt(p_right.GetAsUBigInt());
}

bool
static SQL_OperSBigSmallerUBig(SQLVariant& p_left,SQLVariant& p_right)
{
  return SQL_SBIGINTToUBIGINT(p_left.GetAsSBigInt()) < p_right.GetAsUBigInt();
}

bool
static SQL_OperUBigSmallerUBig(SQLVariant& p_left,SQLVariant& p_right)
{
  return p_left.GetAsUBigInt() < p_right.GetAsUBigInt();
}

bool
static SQL_OperNumSmallerUBig(SQLVariant& p_left,SQLVariant& p_right)
{
  return p_left.GetAsBCD().AsUInt64() < p_right.GetAsUBigInt();
}

// NUMERIC

bool
static SQL_OperSShortSmallerNum(SQLVariant& p_left,SQLVariant& p_right)
{
  return p_left.GetAsSShort() < SQL_SLongToShort(p_right.GetAsBCD().AsLong());
}

bool
static SQL_OperUShortSmallerNum(SQLVariant& p_left,SQLVariant& p_right)
{
  return p_left.GetAsUShort() < SQL_SLongToUShort(p_right.GetAsBCD().AsLong());
}

bool
static SQL_OperSLongSmallerNum(SQLVariant& p_left,SQLVariant& p_right)
{
  return p_left.GetAsSLong() < p_right.GetAsBCD().AsLong();
}

bool
static SQL_OperULongSmallerNum(SQLVariant& p_left,SQLVariant& p_right)
{
  return p_left.GetAsULong() < SQL_BIGINTToULong(p_right.GetAsBCD().AsInt64());
}

bool
static SQL_OperFloatSmallerNum(SQLVariant& p_left,SQLVariant& p_right)
{
  return p_left.GetAsFloat() < (float) p_right.GetAsBCD().AsDouble();
}

bool
static SQL_OperDoubleSmallerNum(SQLVariant& p_left,SQLVariant& p_right)
{
  return p_left.GetAsDouble() < p_right.GetAsBCD().AsDouble();
}

bool
static SQL_OperBitSmallerNum(SQLVariant& p_left,SQLVariant& p_right)
{
  return p_left.GetAsBit() == 0 && !p_right.GetAsBCD().IsNull();
}

bool
static SQL_OperSTinySmallerNum(SQLVariant& p_left,SQLVariant& p_right)
{
  return p_left.GetAsSTinyInt() < SQL_SLongToTinyInt(p_right.GetAsBCD().AsLong());
}

bool
static SQL_OperUTinySmallerNum(SQLVariant& p_left,SQLVariant& p_right)
{
  return p_left.GetAsUTinyInt() < SQL_SLongToUTinyInt(p_right.GetAsBCD().AsLong());
}

bool
static SQL_OperSBigSmallerNum(SQLVariant& p_left,SQLVariant& p_right)
{
  return p_left.GetAsSBigInt() < p_right.GetAsBCD().AsInt64();
}

bool
static SQL_OperUBigSmallerNum(SQLVariant& p_left,SQLVariant& p_right)
{
  return p_left.GetAsUBigInt() < p_right.GetAsBCD().AsUInt64();
}

bool
static SQL_OperNumSmallerNum(SQLVariant& p_left,SQLVariant& p_right)
{
  return p_left.GetAsBCD() < p_right.GetAsBCD();
}

// TYPE == GUID

bool
static SQL_OperGuidSmallerGuid(SQLVariant& p_left,SQLVariant& p_right)
{
  SQLGUID* left  = p_left.GetAsGUID();
  SQLGUID* right = p_right.GetAsGUID();

  return memcmp(left,right,sizeof(SQLGUID)) < 0;
}

// TYPE == BINARY

bool
static SQL_OperBinarySmallerBinary(SQLVariant& p_left,SQLVariant& p_right)
{
  void* left   = p_left.GetAsBinary();
  void* right  = p_right.GetAsBinary();
  // Take longest of two binaries in comparison
  long  length = p_left.GetBinaryLength() < p_right.GetBinaryLength() ? p_left.GetBinaryLength() : p_right.GetBinaryLength();

  return memcmp(left,right,length) < 0;
}

// TYPE == DATE

bool
static SQL_OperDateSmallerDate(SQLVariant& p_left,SQLVariant& p_right)
{
  // Relies on the < operator of SQLDate
  return p_left.GetAsSQLDate() < p_right.GetAsSQLDate();
}

bool
static SQL_OperStampSmallerDate(SQLVariant& p_left,SQLVariant& p_right)
{
  SQLDate date = p_left.GetAsSQLTimestamp().AsSQLDate();
  return date < p_right.GetAsSQLDate();
}

// TYPE == TIME

bool
static SQL_OperTimeSmallerTime(SQLVariant& p_left,SQLVariant& p_right)
{
  // Relies on the < operator of SQLTime
  return p_left.GetAsSQLTime() < p_right.GetAsSQLTime();
}

bool
static SQL_OperStampSmallerTime(SQLVariant& p_left,SQLVariant& p_right)
{
  SQLTime time = p_left.GetAsSQLTimestamp().AsSQLTime();
  return time < p_right.GetAsSQLTime();
}

// TYPE == TIMESTAMP

bool
static SQL_OperStampSmallerStamp(SQLVariant& p_left,SQLVariant& p_right)
{
  // Relies on the < operator of SQLTimestamp
  return p_left.GetAsSQLTimestamp() < p_right.GetAsSQLTimestamp();
}

// TYPE == INTERVAL_YEAR_MONTH

bool
static SQL_OperIntYMSmallerIntYM(SQLVariant& p_left,SQLVariant& p_right)
{
  // Relies on the calculation of months in SQLInterval
  return p_left.GetAsSQLInterval().AsDatabaseDouble() < p_right.GetAsSQLInterval().AsDatabaseDouble();
}

// TYPE == INTERVAL_DAY_SECOND

bool
static SQL_OperIntDSSmallerIntDS(SQLVariant& p_left,SQLVariant& p_right)
{
  // Relies on the calculation of seconds in SQLInterval
  return p_left.GetAsSQLInterval().AsDatabaseDouble() < p_right.GetAsSQLInterval().AsDatabaseDouble();
}

// OPERATOR ARRAY

static CompareFunctionArray OperatorSmaller[CT_LAST][CT_LAST] =
{
                         // CT_CHAR                  CT_SSHORT                    CT_USHORT                    CT_SLONG                     CT_ULONG                    CT_FLOAT                    CT_DOUBLE                    CT_BIT                    CT_STINYINT                 CT_UTINYINT                 CT_SBIGINT                 CT_UBIGINT                 CT_NUMERIC                CT_GUID                  CT_BINARY                    CT_DATE                   CT_TIME                   CT_TIMESTAMP               CT_INTERVAL_YM             CT_INTERVAL_DS
                         // ------------------------ ---------------------------- ---------------------------- ---------------------------- --------------------------- --------------------------- ---------------------------- ------------------------- --------------------------- --------------------------- -------------------------- -------------------------- ------------------------- ------------------------ ---------------------------- ------------------------- ------------------------- -------------------------- -------------------------- ----------------------------
  /* CT_CHAR        */   {  &SQL_OperVarSmallerChar ,&SQL_OperVarSmallerChar     ,&SQL_OperVarSmallerChar     ,&SQL_OperVarSmallerChar     ,&SQL_OperVarSmallerChar    ,&SQL_OperVarSmallerChar    ,&SQL_OperVarSmallerChar     ,&SQL_OperVarSmallerChar  ,&SQL_OperVarSmallerChar    ,&SQL_OperVarSmallerChar    ,&SQL_OperVarSmallerChar   ,&SQL_OperVarSmallerChar   ,&SQL_OperVarSmallerChar  ,&SQL_OperVarSmallerChar ,nullptr                     ,&SQL_OperVarSmallerChar  ,&SQL_OperVarSmallerChar  ,&SQL_OperVarSmallerChar   ,&SQL_OperVarSmallerChar   ,&SQL_OperVarSmallerChar    }
  /* CT_SSHORT      */  ,{  &SQL_OperVarSmallerChar ,&SQL_OperSShortSmallerSShort,&SQL_OperSShortSmallerUShort,&SQL_OperSShortSmallerSLong ,&SQL_OperSShortSmallerULong,&SQL_OperSShortSmallerFloat,&SQL_OperSShortSmallerDouble,&SQL_OperSShortSmallerBit,&SQL_OperSShortSmallerSTiny,&SQL_OperSShortSmallerUTiny,&SQL_OperSShortSmallerSBig,&SQL_OperSShortSmallerUBig,&SQL_OperSShortSmallerNum,nullptr                 ,nullptr                     ,nullptr                  ,nullptr                  ,nullptr                   ,nullptr                   ,nullptr                    }
  /* CT_USHORT      */  ,{  &SQL_OperVarSmallerChar ,&SQL_OperUShortSmallerSShort,&SQL_OperUShortSmallerUShort,&SQL_OperUShortSmallerSLong ,&SQL_OperUShortSmallerULong,&SQL_OperUShortSmallerFloat,&SQL_OperUShortSmallerDouble,&SQL_OperUShortSmallerBit,&SQL_OperUShortSmallerSTiny,&SQL_OperUShortSmallerUTiny,&SQL_OperUShortSmallerSBig,&SQL_OperUShortSmallerUBig,&SQL_OperUShortSmallerNum,nullptr                 ,nullptr                     ,nullptr                  ,nullptr                  ,nullptr                   ,nullptr                   ,nullptr                    }
  /* CT_SLONG       */  ,{  &SQL_OperVarSmallerChar ,&SQL_OperSLongSmallerSShort ,&SQL_OperSLongSmallerUShort ,&SQL_OperSLongSmallerSLong  ,&SQL_OperSLongSmallerULong ,&SQL_OperSLongSmallerFloat ,&SQL_OperSLongSmallerDouble ,&SQL_OperSLongSmallerBit ,&SQL_OperSLongSmallerSTiny ,&SQL_OperSLongSmallerUTiny ,&SQL_OperSLongSmallerSBig ,&SQL_OperSLongSmallerUBig ,&SQL_OperSLongSmallerNum ,nullptr                 ,nullptr                     ,nullptr                  ,nullptr                  ,nullptr                   ,nullptr                   ,nullptr                    }
  /* CT_ULONG       */  ,{  &SQL_OperVarSmallerChar ,&SQL_OperULongSmallerSShort ,&SQL_OperUlongSmallerUShort ,&SQL_OperULongSmallerSLong  ,&SQL_OperULongSmallerULong ,&SQL_OperULongSmallerFloat ,&SQL_OperULongSmallerDouble ,&SQL_OperULongSmallerBit ,&SQL_OperULongSmallerSTiny ,&SQL_OperULongSmallerUTiny ,&SQL_OperULongSmallerSBig ,&SQL_OperULongSmallerUBig ,&SQL_OperULongSmallerNum ,nullptr                 ,nullptr                     ,nullptr                  ,nullptr                  ,nullptr                   ,nullptr                   ,nullptr                    }
  /* CT_FLOAT       */  ,{  &SQL_OperVarSmallerChar ,&SQL_OperFloatSmallerSShort ,&SQL_OperFloatSmallerUShort ,&SQL_OperFloatSmallerSLong  ,&SQL_OperFloatSmallerULong ,&SQL_OperFloatSmallerFloat ,&SQL_OperFloatSmallerDouble ,&SQL_OperFloatSmallerBit ,&SQL_OperFloatSmallerSTiny ,&SQL_OperFloatSmallerUTiny ,&SQL_OperFloatSmallerSBig ,&SQL_OperFloatSmallerUBig ,&SQL_OperFloatSmallerNum ,nullptr                 ,nullptr                     ,nullptr                  ,nullptr                  ,nullptr                   ,nullptr                   ,nullptr                    }
  /* CT_DOUBLE      */  ,{  &SQL_OperVarSmallerChar ,&SQL_OperDoubleSmallerSShort,&SQL_OperDoubleSmallerUShort,&SQL_OperDoubleESmallerSLong,&SQL_OperDoubleSmallerULong,&SQL_OperDoubleSmallerFloat,&SQL_OperDoubleSmallerDouble,&SQL_OperDoubleSmallerBit,&SQL_OperDoubleSmallerSTiny,&SQL_OperDoubleSmallerUTiny,&SQL_OperDoubleSmallerSBig,&SQL_OperDoubleSmallerUBig,&SQL_OperDoubleSmallerNum,nullptr                 ,nullptr                     ,nullptr                  ,nullptr                  ,nullptr                   ,nullptr                   ,nullptr                    }
  /* CT_BIT         */  ,{  &SQL_OperVarSmallerChar ,&SQL_OperBitSmallerSShort   ,&SQL_OperBitSmallerUShort   ,&SQL_OperBitSmallerSLong    ,&SQL_OperBitSmallerULong   ,&SQL_OperBitSmallerFloat   ,&SQL_OperBitSmallerDouble   ,&SQL_OperBitSmallerBit   ,&SQL_OperBitSmallerSTiny   ,&SQL_OperBitSmallerUTiny   ,&SQL_OperBitSmallerSBig   ,&SQL_OperBitSmallerUBig   ,&SQL_OperBitSmallerNum   ,nullptr                 ,nullptr                     ,nullptr                  ,nullptr                  ,nullptr                   ,nullptr                   ,nullptr                    }
  /* CT_STINYINT    */  ,{  &SQL_OperVarSmallerChar ,&SQL_OperSTinySmallerSShort ,&SQL_OperSTinySmallerUShort ,&SQL_OperSTinySmallerSLong  ,&SQL_OperSTinySmallerULong ,&SQL_OperSTinySmallerFloat ,&SQL_OperSTinySmallerDouble ,&SQL_OperSTinySmallerBit ,&SQL_OperSTinySmallerSTiny ,&SQL_OperSTinySmallerUTiny ,&SQL_OperSTinySmallerSBig ,&SQL_OperSTinySmallerUBig ,&SQL_OperSTinySmallerNum ,nullptr                 ,nullptr                     ,nullptr                  ,nullptr                  ,nullptr                   ,nullptr                   ,nullptr                    }
  /* CT_UTINYINT    */  ,{  &SQL_OperVarSmallerChar ,&SQL_OperUTinySmallerSShort ,&SQL_OperUTinySmallerUShort ,&SQL_OperUTinySmallerSLong  ,&SQL_OperUTinySmallerULong ,&SQL_OperUTinySmallerFloat ,&SQL_OperUTinySmallerDouble ,&SQL_OperUTinySmallerBit ,&SQL_OperUTinySmallerSTiny ,&SQL_OperUTinySmallerUTiny ,&SQL_OperUTinySmallerSBig ,&SQL_OperUTinySmallerUBig ,&SQL_OperUTinySmallerNum ,nullptr                 ,nullptr                     ,nullptr                  ,nullptr                  ,nullptr                   ,nullptr                   ,nullptr                    }
  /* CT_SBIGINT     */  ,{  &SQL_OperVarSmallerChar ,&SQL_OperSBigSmallerSShort  ,&SQL_OperSBigSmallerUShort  ,&SQL_OperSBigSmallerSLong   ,&SQL_OperSBigSmallerULong  ,&SQL_OperSBigSmallerFloat  ,&SQL_OperSBigSmallerDouble  ,&SQL_OperSBigSmallerBit  ,&SQL_OperSBigSmallerSTiny  ,&SQL_OperSBigSmallerUTiny  ,&SQL_OperSBigSmallerSBig  ,&SQL_OperSBigSmallerUBig  ,&SQL_OperSBigSmallerNum  ,nullptr                 ,nullptr                     ,nullptr                  ,nullptr                  ,nullptr                   ,nullptr                   ,nullptr                    }
  /* CT_UBIGINT     */  ,{  &SQL_OperVarSmallerChar ,&SQL_OperUBigSmallerSShort  ,&SQL_OperUBigSmallerUShort  ,&SQL_OperUBigSmallerSLong   ,&SQL_OperUBigSmallerULong  ,&SQL_OperUBigSmallerFloat  ,&SQL_OperUBigSmallerDouble  ,&SQL_OperUBigSmallerBit  ,&SQL_OperUBigSmallerSTiny  ,&SQL_OperUBigSmallerUTiny  ,&SQL_OperUBigSmallerSBig  ,&SQL_OperUBigSmallerUBig  ,&SQL_OperUBigSmallerNum  ,nullptr                 ,nullptr                     ,nullptr                  ,nullptr                  ,nullptr                   ,nullptr                   ,nullptr                    }
  /* CT_NUMERIC     */  ,{  &SQL_OperVarSmallerChar ,&SQL_OperNumSmallerSShort   ,&SQL_OperNumSmallerUShort   ,&SQL_OperNumSmallerSLong    ,&SQL_OperNumSmallerULong   ,&SQL_OperNumSmallerFloat   ,&SQL_OperNumSmallerDouble   ,&SQL_OperNumSmallerBit   ,&SQL_OperNumSmallerSTiny   ,&SQL_OperNumSmallerUTiny   ,&SQL_OperNumSmallerSBig   ,&SQL_OperNumSmallerUBig   ,&SQL_OperNumSmallerNum   ,nullptr                 ,nullptr                     ,nullptr                  ,nullptr                  ,nullptr                   ,nullptr                   ,nullptr                    }
  /* CT_GUID        */  ,{  &SQL_OperGuidSmallerChar,nullptr                     ,nullptr                     ,nullptr                     ,nullptr                    ,nullptr                    ,nullptr                     ,nullptr                  ,nullptr                    ,nullptr                    ,nullptr                   ,nullptr                   ,nullptr                  ,&SQL_OperGuidSmallerGuid,nullptr                     ,nullptr                  ,nullptr                  ,nullptr                   ,nullptr                   ,nullptr                    }
  /* CT_BINARY      */  ,{  nullptr                 ,nullptr                     ,nullptr                     ,nullptr                     ,nullptr                    ,nullptr                    ,nullptr                     ,nullptr                  ,nullptr                    ,nullptr                    ,nullptr                   ,nullptr                   ,nullptr                  ,nullptr                 ,&SQL_OperBinarySmallerBinary,nullptr                  ,nullptr                  ,nullptr                   ,nullptr                   ,nullptr                    }
  /* CT_DATE        */  ,{  &SQL_OperVarSmallerChar ,nullptr                     ,nullptr                     ,nullptr                     ,nullptr                    ,nullptr                    ,nullptr                     ,nullptr                  ,nullptr                    ,nullptr                    ,nullptr                   ,nullptr                   ,nullptr                  ,nullptr                 ,nullptr                     ,&SQL_OperDateSmallerDate ,nullptr                  ,nullptr                   ,nullptr                   ,nullptr                    }
  /* CT_TIME        */  ,{  &SQL_OperVarSmallerChar ,nullptr                     ,nullptr                     ,nullptr                     ,nullptr                    ,nullptr                    ,nullptr                     ,nullptr                  ,nullptr                    ,nullptr                    ,nullptr                   ,nullptr                   ,nullptr                  ,nullptr                 ,nullptr                     ,nullptr                  ,&SQL_OperTimeSmallerTime ,nullptr                   ,nullptr                   ,nullptr                    }
  /* CT_TIMESTAMP   */  ,{  &SQL_OperVarSmallerChar ,nullptr                     ,nullptr                     ,nullptr                     ,nullptr                    ,nullptr                    ,nullptr                     ,nullptr                  ,nullptr                    ,nullptr                    ,nullptr                   ,nullptr                   ,nullptr                  ,nullptr                 ,nullptr                     ,&SQL_OperStampSmallerDate,&SQL_OperStampSmallerTime,&SQL_OperStampSmallerStamp,nullptr                   ,nullptr                    }
  /* CT_INTERVAL_YM */  ,{  &SQL_OperVarSmallerChar ,nullptr                     ,nullptr                     ,nullptr                     ,nullptr                    ,nullptr                    ,nullptr                     ,nullptr                  ,nullptr                    ,nullptr                    ,nullptr                   ,nullptr                   ,nullptr                  ,nullptr                 ,nullptr                     ,nullptr                  ,nullptr                  ,nullptr                   ,&SQL_OperIntYMSmallerIntYM,nullptr                    }
  /* CT_INTERVAL_DS */  ,{  &SQL_OperVarSmallerChar ,nullptr                     ,nullptr                     ,nullptr                     ,nullptr                    ,nullptr                    ,nullptr                     ,nullptr                  ,nullptr                    ,nullptr                    ,nullptr                   ,nullptr                   ,nullptr                  ,nullptr                 ,nullptr                     ,nullptr                  ,nullptr                  ,nullptr                   ,nullptr                   ,&SQL_OperIntDSSmallerIntDS }
};

// Smaller operator for SQLVariant
bool
SQLVariant::operator<(SQLVariant& p_right)
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
  OperatorCompare function = OperatorSmaller[left][right].function;
  if(function)
  {
    return (*function)(*this,p_right);
  }
  // No compare function found
  // Data types are not comparable
  CString leftType  = FindDatatype(m_datatype);
  CString rightType = FindDatatype(p_right.m_datatype);
  CString error;
  error.Format("Cannot do the Smaller operator on (%s < %s)",leftType.GetString(),rightType.GetString());
  throw StdException(error);
}

// End of namespace
}
