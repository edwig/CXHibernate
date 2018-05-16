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