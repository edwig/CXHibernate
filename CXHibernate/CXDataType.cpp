////////////////////////////////////////////////////////////////////////
//
// File: CXDataType.cpp
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
// Last Revision:   22-04-2018
// Version number:  0.0.1
//
#include "stdafx.h"
#include "CXDataType.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// All datatypes to ODBC types
CXType cxtypes[] =
{
  { "CString",      SQL_C_CHAR     }
 ,{ "int",          SQL_C_LONG     }
 ,{ "int",          SQL_C_SLONG    }
 ,{ "bool",         SQL_C_BIT      }
 ,{ "bcd",          SQL_C_NUMERIC  }
 ,{ "uint",         SQL_C_ULONG    }
 ,{ "short",        SQL_C_SHORT    }
 ,{ "short",        SQL_C_SSHORT   }
 ,{ "ushort",       SQL_C_USHORT   }
 ,{ "long",         SQL_C_LONG     }
 ,{ "ulong",        SQL_C_ULONG    }
 ,{ "char",         SQL_C_TINYINT  }
 ,{ "char",         SQL_C_STINYINT }
 ,{ "uchar",        SQL_C_UTINYINT }
 ,{ "bigint",       SQL_C_SBIGINT  }
 ,{ "ubigint",      SQL_C_UBIGINT  }
 ,{ "float",        SQL_C_FLOAT    }
 ,{ "double",       SQL_C_DOUBLE   }
 ,{ "SQLDate",      SQL_C_DATE            }
 ,{ "SQLDate",      SQL_C_TYPE_DATE       }
 ,{ "SQLTime",      SQL_C_TIME            }
 ,{ "SQLTime",      SQL_C_TYPE_TIME       }
 ,{ "SQLTimestamp", SQL_C_TIMESTAMP       }
 ,{ "SQLTimestamp", SQL_C_TYPE_TIMESTAMP  }
 ,{ "SQLGuid",      SQL_C_GUID            }
 ,{ "SQLVariant",   0                     }
 ,{ "SQLInterval",  SQL_C_INTERVAL_DAY_TO_SECOND    }
 ,{ "SQLInterval",  SQL_C_INTERVAL_YEAR_TO_MONTH    }
 ,{ "SQLInterval",  SQL_C_INTERVAL_HOUR_TO_SECOND   }
 ,{ "SQLInterval",  SQL_C_INTERVAL_HOUR_TO_MINUTE   }
 ,{ "SQLInterval",  SQL_C_INTERVAL_DAY_TO_MINUTE    }
 ,{ "SQLInterval",  SQL_C_INTERVAL_DAY_TO_HOUR      }
 ,{ "SQLInterval",  SQL_C_INTERVAL_MINUTE_TO_SECOND }
 ,{ "SQLInterval",  SQL_C_INTERVAL_YEAR             }
 ,{ "SQLInterval",  SQL_C_INTERVAL_MONTH            }
 ,{ "SQLInterval",  SQL_C_INTERVAL_DAY              }
 ,{ "SQLInterval",  SQL_C_INTERVAL_HOUR             }
 ,{ "SQLInterval",  SQL_C_INTERVAL_MINUTE           }
 ,{ "SQLInterval",  SQL_C_INTERVAL_SECOND           }
};

// Types names are Case-Sensitive!!
int 
CXStringToDatatType(CString p_datatype)
{
  CXType* types = cxtypes;

  for(int ind = 0;ind < sizeof(cxtypes) / sizeof(CXType); ++ind)
  {
    if(p_datatype.Compare(types->m_name) == 0)
    {
      return types->m_type;
    }
    ++types;
  }
  return 0;
}

// Convert ODBC datatype to first matching CXDataType name
// Default is the 'var' typedef for SQLVariant
CString
CXDataTypeToString(int p_datatype)
{
  CXType* types = cxtypes;

  for(int ind = 0;ind < sizeof(cxtypes) / sizeof(CXType); ++ind)
  {
    if(p_datatype == types->m_type)
    {
      return types->m_name;
    }
    ++types;
  }
  return "var";
}

CXType configtypes[] =
{
  { "string",       SQL_C_CHAR      }
 ,{ "int",          SQL_C_LONG      }
 ,{ "int",          SQL_C_SLONG     }
 ,{ "bool",         SQL_C_BIT       }
 ,{ "bcd",          SQL_C_NUMERIC   }
 ,{ "uint",         SQL_C_ULONG     }
 ,{ "short",        SQL_C_SHORT     }
 ,{ "short",        SQL_C_SSHORT    }
 ,{ "ushort",       SQL_C_USHORT    }
 ,{ "long",         SQL_C_LONG      }
 ,{ "ulong",        SQL_C_ULONG     }
 ,{ "tinyint",      SQL_C_TINYINT   }
 ,{ "tinyint",      SQL_C_STINYINT  }
 ,{ "utinyint",     SQL_C_UTINYINT  }
 ,{ "bigint",       SQL_C_SBIGINT   }
 ,{ "ubigint",      SQL_C_UBIGINT   }
 ,{ "float",        SQL_C_FLOAT     }
 ,{ "double",       SQL_C_DOUBLE    }
 ,{ "date",         SQL_C_DATE      }
 ,{ "time",         SQL_C_TIME      }
 ,{ "timestamp",    SQL_C_TIMESTAMP }
 ,{ "guid",         SQL_C_GUID      }
 ,{ "var",          0               }
 ,{ "interval_day_to_second",     SQL_C_INTERVAL_DAY_TO_SECOND    }
 ,{ "interval_year_to_month",     SQL_C_INTERVAL_YEAR_TO_MONTH    }
 ,{ "interval_hour_to_second",    SQL_C_INTERVAL_HOUR_TO_SECOND   }
 ,{ "interval_hour_to_minute",    SQL_C_INTERVAL_HOUR_TO_MINUTE   }
 ,{ "interval_day_to_minute",     SQL_C_INTERVAL_DAY_TO_MINUTE    }
 ,{ "interval_day_to_hour",       SQL_C_INTERVAL_DAY_TO_HOUR      }
 ,{ "interval_minute_to_second",  SQL_C_INTERVAL_MINUTE_TO_SECOND }
 ,{ "interval_year",              SQL_C_INTERVAL_YEAR             }
 ,{ "interval_month",             SQL_C_INTERVAL_MONTH            }
 ,{ "interval_day",               SQL_C_INTERVAL_DAY              }
 ,{ "interval_hour",              SQL_C_INTERVAL_HOUR             }
 ,{ "interval_minute",            SQL_C_INTERVAL_MINUTE           }
 ,{ "interval_second",            SQL_C_INTERVAL_SECOND           }
};

int 
CXConfigToDataType(CString p_datatype)
{
  CXType* types = configtypes;

  for(int ind = 0;ind < sizeof(cxtypes) / sizeof(CXType); ++ind)
  {
    if(p_datatype.Compare(types->m_name) == 0)
    {
      return types->m_type;
    }
    ++types;
  }
  return 0;
}

CString
CXDataTypeToConfig(int p_datatype)
{
  CXType* types = configtypes;

  for(int ind = 0;ind < sizeof(cxtypes) / sizeof(CXType); ++ind)
  {
    if(p_datatype == types->m_type)
    {
      return types->m_name;
    }
    ++types;
  }
  return "var";
}
