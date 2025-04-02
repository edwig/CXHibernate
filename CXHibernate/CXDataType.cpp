////////////////////////////////////////////////////////////////////////
//
// File: CXDataType.cpp
//
// Copyright (c) 2015-2022 ir. W.E. Huisman
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
// Last version date: See CXHibernate.h
// Version number:    See CXHibernate.h
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
  { _T("CString"),      SQL_C_CHAR     }
 ,{ _T("int"),          SQL_C_LONG     }
 ,{ _T("int"),          SQL_C_SLONG    }
 ,{ _T("bool"),         SQL_C_BIT      }
 ,{ _T("bcd"),          SQL_C_NUMERIC  }
 ,{ _T("uint"),         SQL_C_ULONG    }
 ,{ _T("short"),        SQL_C_SHORT    }
 ,{ _T("short"),        SQL_C_SSHORT   }
 ,{ _T("ushort"),       SQL_C_USHORT   }
 ,{ _T("long"),         SQL_C_LONG     }
 ,{ _T("ulong"),        SQL_C_ULONG    }
 ,{ _T("char"),         SQL_C_TINYINT  }
 ,{ _T("char"),         SQL_C_STINYINT }
 ,{ _T("uchar"),        SQL_C_UTINYINT }
 ,{ _T("bigint"),       SQL_C_SBIGINT  }
 ,{ _T("ubigint"),      SQL_C_UBIGINT  }
 ,{ _T("float"),        SQL_C_FLOAT    }
 ,{ _T("double"),       SQL_C_DOUBLE   }
 ,{ _T("SQLDate"),      SQL_C_DATE            }
 ,{ _T("SQLDate"),      SQL_C_TYPE_DATE       }
 ,{ _T("SQLTime"),      SQL_C_TIME            }
 ,{ _T("SQLTime"),      SQL_C_TYPE_TIME       }
 ,{ _T("SQLTimestamp"), SQL_C_TIMESTAMP       }
 ,{ _T("SQLTimestamp"), SQL_C_TYPE_TIMESTAMP  }
 ,{ _T("SQLGuid"),      SQL_C_GUID            }
 ,{ _T("SQLVariant"),   0                     }
 ,{ _T("SQLInterval"),  SQL_C_INTERVAL_DAY_TO_SECOND    }
 ,{ _T("SQLInterval"),  SQL_C_INTERVAL_YEAR_TO_MONTH    }
 ,{ _T("SQLInterval"),  SQL_C_INTERVAL_HOUR_TO_SECOND   }
 ,{ _T("SQLInterval"),  SQL_C_INTERVAL_HOUR_TO_MINUTE   }
 ,{ _T("SQLInterval"),  SQL_C_INTERVAL_DAY_TO_MINUTE    }
 ,{ _T("SQLInterval"),  SQL_C_INTERVAL_DAY_TO_HOUR      }
 ,{ _T("SQLInterval"),  SQL_C_INTERVAL_MINUTE_TO_SECOND }
 ,{ _T("SQLInterval"),  SQL_C_INTERVAL_YEAR             }
 ,{ _T("SQLInterval"),  SQL_C_INTERVAL_MONTH            }
 ,{ _T("SQLInterval"),  SQL_C_INTERVAL_DAY              }
 ,{ _T("SQLInterval"),  SQL_C_INTERVAL_HOUR             }
 ,{ _T("SQLInterval"),  SQL_C_INTERVAL_MINUTE           }
 ,{ _T("SQLInterval"),  SQL_C_INTERVAL_SECOND           }
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
  return _T("var");
}

CXType configtypes[] =
{
  { _T("string"),       SQL_C_CHAR      }
 ,{ _T("int"),          SQL_C_LONG      }
 ,{ _T("int"),          SQL_C_SLONG     }
 ,{ _T("bool"),         SQL_C_BIT       }
 ,{ _T("bcd"),          SQL_C_NUMERIC   }
 ,{ _T("uint"),         SQL_C_ULONG     }
 ,{ _T("short"),        SQL_C_SHORT     }
 ,{ _T("short"),        SQL_C_SSHORT    }
 ,{ _T("ushort"),       SQL_C_USHORT    }
 ,{ _T("long"),         SQL_C_LONG      }
 ,{ _T("ulong"),        SQL_C_ULONG     }
 ,{ _T("tinyint"),      SQL_C_TINYINT   }
 ,{ _T("tinyint"),      SQL_C_STINYINT  }
 ,{ _T("utinyint"),     SQL_C_UTINYINT  }
 ,{ _T("bigint"),       SQL_C_SBIGINT   }
 ,{ _T("ubigint"),      SQL_C_UBIGINT   }
 ,{ _T("float"),        SQL_C_FLOAT     }
 ,{ _T("double"),       SQL_C_DOUBLE    }
 ,{ _T("date"),         SQL_C_DATE      }
 ,{ _T("time"),         SQL_C_TIME      }
 ,{ _T("timestamp"),    SQL_C_TIMESTAMP }
 ,{ _T("guid"),         SQL_C_GUID      }
 ,{ _T("var"),          0               }
 ,{ _T("interval_day_to_second"),     SQL_C_INTERVAL_DAY_TO_SECOND    }
 ,{ _T("interval_year_to_month"),     SQL_C_INTERVAL_YEAR_TO_MONTH    }
 ,{ _T("interval_hour_to_second"),    SQL_C_INTERVAL_HOUR_TO_SECOND   }
 ,{ _T("interval_hour_to_minute"),    SQL_C_INTERVAL_HOUR_TO_MINUTE   }
 ,{ _T("interval_day_to_minute"),     SQL_C_INTERVAL_DAY_TO_MINUTE    }
 ,{ _T("interval_day_to_hour"),       SQL_C_INTERVAL_DAY_TO_HOUR      }
 ,{ _T("interval_minute_to_second"),  SQL_C_INTERVAL_MINUTE_TO_SECOND }
 ,{ _T("interval_year"),              SQL_C_INTERVAL_YEAR             }
 ,{ _T("interval_month"),             SQL_C_INTERVAL_MONTH            }
 ,{ _T("interval_day"),               SQL_C_INTERVAL_DAY              }
 ,{ _T("interval_hour"),              SQL_C_INTERVAL_HOUR             }
 ,{ _T("interval_minute"),            SQL_C_INTERVAL_MINUTE           }
 ,{ _T("interval_second"),            SQL_C_INTERVAL_SECOND           }
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
  return _T("var");
}
