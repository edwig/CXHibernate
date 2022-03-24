////////////////////////////////////////////////////////////////////////
//
// File: CXModelWord.cpp
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
#include "CXModelWord.h"
#include <SQLMetaInfo.h>
#include <sqlext.h>

CXModelWord cxmodelWords[] =
{
   // COLUMNS/ATTRIBUTES nullability
   { "not_null",            SQL_NO_NULLS            }
  ,{ "nullable",            SQL_NULLABLE            }
  ,{ "nulls_unknown",       SQL_NULLABLE_UNKNOWN    }
   // CONSTRAINT STATUS
  ,{ "initially_deferred",  SQL_INITIALLY_DEFERRED  }
  ,{ "initially_immediate", SQL_INITIALLY_IMMEDIATE }
  ,{ "not_deferrable",      SQL_NOT_DEFERRABLE      }
  ,{ "immediate",           0                       }
  ,{ "deferred",            1                       }
   // ASSOCIATIONS 
  ,{ "cascade",             SQL_CASCADE             }
  ,{ "no_action",           SQL_NO_ACTION           }
  ,{ "set_null",            SQL_SET_NULL            }
  ,{ "set_default",         SQL_SET_DEFAULT         }
  ,{ "restrict",            SQL_RESTRICT            }
   // Association matching
  ,{ "match_full",          SQL_MATCH_FULL          }
  ,{ "match_partial",       SQL_MATCH_PARTIAL       }
  ,{ "match_simple",        SQL_MATCH_SIMPLE        }
   // INDEX TYPE
  ,{ "table_statistics",    SQL_TABLE_STAT          }
  ,{ "index_clustered",     SQL_INDEX_CLUSTERED     }
  ,{ "index_hashed",        SQL_INDEX_HASHED        }
  ,{ "index_other",         SQL_INDEX_OTHER         }
   // Special attributes
  ,{ "scope_currow",        SQL_SCOPE_CURROW        }
  ,{ "scope_transaction",   SQL_SCOPE_TRANSACTION   }
  ,{ "scope_session",       SQL_SCOPE_SESSION       }
  ,{ "pseudo_transaction",  SQL_PC_PSEUDO           }
  ,{ "nonpseudo_transaction",SQL_PC_NOT_PSEUDO      }
   // PERSISTENT STORED MODULES (PSM)
  ,{ "procedure",           SQL_PT_PROCEDURE        }
  ,{ "function",            SQL_PT_FUNCTION         }
   // RESULT TYPES
  ,{ "param_input",         SQL_PARAM_INPUT         }
  ,{ "param_input_output",  SQL_PARAM_INPUT_OUTPUT  }
  ,{ "param_output",        SQL_PARAM_OUTPUT        }
  ,{ "return_value",        SQL_RETURN_VALUE        }
  ,{ "result_column",       SQL_RESULT_COL          }
};
