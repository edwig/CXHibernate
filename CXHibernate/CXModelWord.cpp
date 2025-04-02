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
   { _T("not_null"),            SQL_NO_NULLS            }
  ,{ _T("nullable"),            SQL_NULLABLE            }
  ,{ _T("nulls_unknown"),       SQL_NULLABLE_UNKNOWN    }
   // CONSTRAINT STATUS
  ,{ _T("initially_deferred"),  SQL_INITIALLY_DEFERRED  }
  ,{ _T("initially_immediate"), SQL_INITIALLY_IMMEDIATE }
  ,{ _T("not_deferrable"),      SQL_NOT_DEFERRABLE      }
  ,{ _T("immediate"),           0                       }
  ,{ _T("deferred"),            1                       }
   // ASSOCIATIONS 
  ,{ _T("cascade"),             SQL_CASCADE             }
  ,{ _T("no_action"),           SQL_NO_ACTION           }
  ,{ _T("set_null"),            SQL_SET_NULL            }
  ,{ _T("set_default"),         SQL_SET_DEFAULT         }
  ,{ _T("restrict"),            SQL_RESTRICT            }
   // Association matching
  ,{ _T("match_full"),          SQL_MATCH_FULL          }
  ,{ _T("match_partial"),       SQL_MATCH_PARTIAL       }
  ,{ _T("match_simple"),        SQL_MATCH_SIMPLE        }
   // INDEX TYPE
  ,{ _T("table_statistics"),    SQL_TABLE_STAT          }
  ,{ _T("index_clustered"),     SQL_INDEX_CLUSTERED     }
  ,{ _T("index_hashed"),        SQL_INDEX_HASHED        }
  ,{ _T("index_other"),         SQL_INDEX_OTHER         }
   // Special attributes
  ,{ _T("scope_currow"),        SQL_SCOPE_CURROW        }
  ,{ _T("scope_transaction"),   SQL_SCOPE_TRANSACTION   }
  ,{ _T("scope_session"),       SQL_SCOPE_SESSION       }
  ,{ _T("pseudo_transaction"),  SQL_PC_PSEUDO           }
  ,{ _T("nonpseudo_transaction"),SQL_PC_NOT_PSEUDO      }
   // PERSISTENT STORED MODULES (PSM)
  ,{ _T("procedure"),           SQL_PT_PROCEDURE        }
  ,{ _T("function"),            SQL_PT_FUNCTION         }
   // RESULT TYPES
  ,{ _T("param_input"),         SQL_PARAM_INPUT         }
  ,{ _T("param_input_output"),  SQL_PARAM_INPUT_OUTPUT  }
  ,{ _T("param_output"),        SQL_PARAM_OUTPUT        }
  ,{ _T("return_value"),        SQL_RETURN_VALUE        }
  ,{ _T("result_column"),       SQL_RESULT_COL          }
};
