#include "stdafx.h"
#include "XMLDataType.h"

const char* xml_datatypes[] =
{
  ""                          // 0
 ,"string"                    // 1  XDT_String            SQL_C_CHAR
 ,"integer"                   // 2  XDT_Integer           SQL_C_SLONG
 ,"boolean"                   // 3  XDT_Boolean           SQL_C_BOOLEAN
 ,"double"                    // 4  XDT_Double            SQL_C_DOUBLE
 ,"base64Binary"              // 5  XDT_Base64Binary
 ,"dateTime"                  // 6  XDT_DateTime          SQL_C_TIMESTAMP
 ,"anyURI"                    // 7  XDT_AnyURI
 ,"date"                      // 8  XDT_Date              SQL_C_DATE
 ,"dateTimeStamp"             // 9  XDT_DateTimeStamp     SQL_C_TIMESTAMP
 ,"decimal"                   // 10 XDT_Decimal           SQL_C_NUMERIC
 ,"long"                      // 11 XDT_Long              SQL_C_SLONG
 ,"int"                       // 12 XDT_Int               SQL_C_SLONG
 ,"short"                     // 13 XDT_Short
 ,"byte"                      // 14 XDT_Byte
 ,"nonNegativeInteger"        // 15 XDT_NonNegativeInteger
};

// #define XDT_PositiveInteger     0x00000016
// #define XDT_UnsignedLong        0x00000017
// #define XDT_UnsignedInt         0x00000018
// #define XDT_UnsignedShort       0x00000019
// #define XDT_UnsignedByte        0x00000020
// #define XDT_NonPositiveInteger  0x00000021
// #define XDT_NegativeInteger     0x00000022
// #define XDT_Duration            0x00000023
// #define XDT_DayTimeDuration     0x00000024
// #define XDT_YearMonthDuration   0x00000025
// #define XDT_Float               0x00000026
// #define XDT_GregDay             0x00000027
// #define XDT_GregMonth           0x00000028
// #define XDT_GregMonthDay        0x00000029
// #define XDT_GregYear            0x00000030
// #define XDT_GregYearMonth       0x00000031
// #define XDT_HexBinary           0x00000032
// #define XDT_NOTATION            0x00000033
// #define XDT_QName               0x00000034
// #define XDT_NormalizedString    0x00000035
// #define XDT_Token               0x00000036
// #define XDT_Language            0x00000037
// #define XDT_Name                0x00000038
// #define XDT_NCName              0x00000039
// #define XDT_ENTITY              0x00000040
// #define XDT_ID                  0x00000041
// #define XDT_IDREF               0x00000042
// #define XDT_NMTOKEN             0x00000043
// #define XDT_Time                0x00000044
// #define XDT_ENTITIES            0x00000045
// #define XDT_IDREFS              0x00000046
// #define XDT_NMTOKENS            0x00000047