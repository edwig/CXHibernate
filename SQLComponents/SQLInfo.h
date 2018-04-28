////////////////////////////////////////////////////////////////////////
//
// File: SQLInfo.h
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
// Last Revision:   04-02-2018
// Version number:  1.4.5
//
#pragma once
#include "SQLComponents.h"
#include "SQLDatabase.h"
#include "SQLMetaInfo.h"
#include <sql.h>
#include <sqlext.h>
#include <map>
#include <list>

namespace SQLComponents
{

// Finding META types
#define META_CATALOGS      1
#define META_SCHEMAS       2
#define META_TABLES        3

#define MAX_BUFFER       512

typedef struct _TypeInfo
{
  CString     m_type_name;          // Datasource dependent type name. Use for CREATE TABLE
  SQLSMALLINT m_data_type;          // Data type by ODBC as in SQL_<Datatypename>
  SQLINTEGER  m_precision;          // For numbers: precision, otherwise the column size for char/binaries
  CString     m_literal_prefix;     // Prefix in literal string, like {ts' for timestamp
  CString     m_literal_suffix;     // Suffix in literal string, like '}   for timestamp
  CString     m_create_params;      // How to create parameters, like (precision,scale) for decimals
  SQLSMALLINT m_nullable;           // Nullable: SQL_NO_NULLS (0), SQL_NULLABLE (1), SQL_NULLABLE_UNKNOWN (2)
  SQLSMALLINT m_case_sensitive;     // Case sensitive (TRUE,FALSE)
  SQLSMALLINT m_searchable;         // Searchable in where clause. SQL_PRED_NONE (SQL_UNSEARCHABLE), SQL_PRED_CHAR (1), SQL_PRED_BASIC (2), SQL_SEARCHABLE (3)
  SQLSMALLINT m_unsigned;           // Unsigned (TRUE,FALSE)
  SQLSMALLINT m_money;              // Money (FIXED_PRECISION_SCALE) (TRUE,FALSE)
  SQLSMALLINT m_autoincrement;      // Auto incrementable (TRUE,FALSE,NULL for non-numeric)
  CString     m_local_type_name;    // Local type name for display on UI's (not in DDL!)
  SQLSMALLINT m_minimum_scale;      // Minimum scale of datatype. E.g. in seconds of the TIMESTAMP
  SQLSMALLINT m_maximum_scale;      // Maximum scale of datatype, otherwise use column_size
  SQLSMALLINT m_sqlDatatype;        // Driver independent SQL datatype
  SQLSMALLINT m_sqlSubType;         // SQL subtype for TYPE_TIMESTAMP and INTERVAL types
  SQLINTEGER  m_radix;              // Decimal radix (2,10 or NULL)
  SQLSMALLINT m_interval_precision; // Number of decimals in interval precision of leading type
}
TypeInfo;

typedef std::map<CString,TypeInfo*> DataTypeMap;
typedef std::list<CString>          WordList;

class SQLInfo 
{
public:
   SQLInfo(SQLDatabase* p_database);
  ~SQLInfo();

  // Initialize all internal data structures to defaults
  void    Init();
  // (Re)set the connected database (e.g. after re-open)
  void    SetConnection(SQLDatabase* p_database = NULL);

  // Get all static info from the ODBC driver via SQLGetInfo 
  void    GetInfo();

  // Add an ODBC SQL Keyword
  bool    AddSQLWord(CString sql);
  // Extra to be done on getting info 
  virtual void OnGetInfo(HDBC ,int ) {return;};
  // Is it a correct identifier (type 0=table,1=column)
  bool    IsCorrectName(CString p_name,int p_type = 0);
  // Can we start a transaction on the database
  bool    CanStartTransaction();
  // Returns the fact whether an API function is supported
  // by the ODBC driver, regardless of ODBC version
  bool    SupportedFunction(unsigned int api_function);
  // Return the native SQL command from an ODBC-escaped command
  CString NativeSQL(HDBC hdbc,CString& sqlCommand);
  // AT_EXEC data provider needs length beforehand
  bool    NeedLongDataLen();
  // Character name of an SQL_XXX datatype
  CString ODBCDataType(int DataType);
  // Show metadata warning (for interactive mode)
  void    ShowMetaDataWarning(bool p_show);

  // All the version numbers from the driver
  CString GetVersionODBCManager();
  CString GetVersionODBCDriver();
  CString GetVersionODBCStandard();
  CString GetVersionRDBMS();

  // Get information about the primary key of a table
  bool GetPrimaryKeyInfo(CString&    p_tablename
                        ,CString&    p_primary
                        ,MPrimaryMap& p_keymap);

  // GETTING ALL THE TABLES OF A NAME PATTERN
  // GETTING ALL THE INFO FOR ONE TABLE
  // GETTING ALL THE INFO FOR ONE PROCEDURE
  // GETTING ALL META TYPES
protected:
  virtual bool MakeInfoTableTable     (MTableMap&     p_tables,    CString& p_errors,CString p_schema,CString p_tablename,CString p_type);
  virtual bool MakeInfoTableColumns   (MColumnMap&    p_columns,   CString& p_errors,CString p_schema,CString p_tablename,CString p_columnname = "");
  virtual bool MakeInfoTablePrimary   (MPrimaryMap&   p_primaries, CString& p_errors,CString p_schema,CString p_tablename);
  virtual bool MakeInfoPSMProcedures  (MProcedureMap& p_procedures,CString& p_errors,CString p_schema,CString p_procedure);
  virtual bool MakeInfoPSMParameters  (MParameterMap& p_parameters,CString& p_errors,CString p_schema,CString p_procedure);
  virtual bool MakeInfoTableForeign   (MForeignMap&   p_foreigns,  CString& p_errors,CString p_schema,CString p_tablename,bool p_referenced = false);
  virtual bool MakeInfoTableStatistics(MIndicesMap&   p_statistics,CString& p_errors,CString p_schema,CString p_tablename,MPrimaryMap* p_keymap,bool p_all = true);
public:
  virtual bool MakeInfoTableSpecials  (MSpecialsMap&  p_specials,  CString& p_errors,CString p_schema,CString p_tablename);
  virtual bool MakeInfoTablePrivileges(MPrivilegeMap& p_privileges,CString& p_errors,CString p_schema,CString p_tablename);
  virtual bool MakeInfoMetaTypes      (MMetaMap&      p_objects,   CString& p_errors,int p_type);

  // Meta pointer to SQLGet<META> functions
  unsigned char* GetMetaPointer(unsigned char* p_buffer,bool p_meta);

  // Getting datatype info
  TypeInfo* GetTypeInfo(int p_sqlDatatype) const;

  // CONNECTION ATTRIBUTES

  // Retrieve the fact that we are living or dead
  bool    GetConnectionDead();
  // Connection in quiet mode
  int     GetAttributeQuiet();
  // Automatic population of the IPD, so driver supports prepared statements
  int     GetAttributeAutoIPD();
  // ODBC Tracing is on or off
  bool    GetAttributeTracing();
  // The file to which we are tracing
  CString GetAttributeTraceFile();
  // Getting the current catalog name
  CString GetAttributeCatalog();
  // Getting the IP packet size
  int     GetAttributePacketSize();
  // METADATA ID is supported?
  bool    GetAttributeMetadataID();
  // The current transaction level
  int     GetAttributeTransLevel();
  // The translation library path
  CString GetAttributeTranslib();
  // Translation option of the translation library
  int     GetAttributeTransoption();
  // Connection by File DSN
  CString GetAttributeFileDSN();
  CString GetAttributeFileDSNSave();
  // Timeout for the whole connection
  int     GetAttributeConnTimeout();

  // Connection by File DSN
  void    SetAttributeFileDSN    (CString p_fileDSN);
  void    SetAttributeFileDSNSave(CString p_fileDSN);
  // Set the using of ODBC cursors
  bool    SetAttributeOdbcCursors(int p_cursors);
  // Setting the optimal IP Packet size
  bool    SetAttributePacketSize(int p_packet);
   // Setting the ODBC Tracing file
  bool    SetAttributeTraceFile(CString p_traceFile);
  // Set tracing on or off
  bool    SetAttributeTracing(bool p_tracing);
  // Setting the automatic connection timeout (if supported)
  bool    SetAttributeConnTimeout(int p_timeout);
  // Setting the METADATA-ID of the connection
  bool    SetAttributeMetadataID(bool p_metadata);
  // Setting the transaction isolation level
  bool    SetAttributeTransLevel(int p_txnlevel);
  // Setting the transaction library (with or without connection)
  bool    SetAttributeTranslib(CString p_transLib);
  // Setting the translation optioen (with or without connection)
  bool    SetAttributeTransoption(int p_transOption);

  // GETTERS

  SQLSMALLINT   GetDatabaseFileUsage();
  // mappings
  WordList&     GetODBCKeywords();
  WordList&     GetRDBMSKeywords();
  DataTypeMap&  GetDataTypeMap();
  CString       GetDriverName();
  // type conversions
  SQLUINTEGER   GetConversionFunctions();
  SQLINTEGER    GetConvertBigint();
  SQLINTEGER    GetConvertBinary();
  SQLINTEGER    GetConvertBit();
  SQLINTEGER    GetConvertChar();
  SQLINTEGER    GetConvertDate();
  SQLINTEGER    GetConvertDecimal();
  SQLINTEGER    GetConvertDouble();
  SQLINTEGER    GetConvertFloat();
  SQLINTEGER    GetConvertInteger();
  SQLINTEGER    GetConvertIYM();
  SQLINTEGER    GetConvertIDT();
  SQLINTEGER    GetConvertLVarBinary();
  SQLINTEGER    GetConvertLVarchar();
  SQLINTEGER    GetConvertNumeric();
  SQLINTEGER    GetConvertReal();
  SQLINTEGER    GetConvertSmallint();
  SQLINTEGER    GetConvertTime();
  SQLINTEGER    GetConvertTimestamp();
  SQLINTEGER    GetConvertTinyint();
  SQLINTEGER    GetConvertVarBinary();
  SQLINTEGER    GetConvertVarchar();
  // conformance
  SQLUINTEGER   GetSQLConformance();
  SQLUINTEGER   GetODBCConformance();
  SQLUINTEGER   GetCLIConformance();
  CString       GetCLIYear();
  // Support for functions
  SQLUINTEGER   GetAggregationFunctions();
  SQLUINTEGER   GetNumericFunctions();
  SQLUINTEGER   GetStringFunctions();
  SQLUINTEGER   GetStringFunctions3();
  SQLUINTEGER   GetDateTimeFunctions();
  SQLUINTEGER   GetTimestampAddFunctions();
  SQLUINTEGER   GetTimestampDiffFunctions();
  SQLUINTEGER   GetSystemFunctions();
  // Support for SQL DDL
  SQLUINTEGER   GetAlterDomainFunctions();
  SQLUINTEGER   GetAlterTableFunctions();
  SQLUINTEGER   GetCreateSchemaFunctions();
  SQLUINTEGER   GetCreateDomainFunctions();
  SQLUINTEGER   GetCreateTableFunctions();
  SQLUINTEGER   GetCreateViewFunctions();
  SQLUINTEGER   GetDeleteRuleFunctions();
  SQLUINTEGER   GetDropSchemaFunctions();
  SQLUINTEGER   GetDropDomainFunctions();
  SQLUINTEGER   GetDropTableFunctions();
  SQLUINTEGER   GetDropViewFunctions();
  // Options
  SQLUINTEGER   GetGrantOptions();
  SQLUINTEGER   GetIndexOptions1();
  SQLUINTEGER   GetIndexOptions2();
  SQLUINTEGER   GetInsertOptions();
  SQLUINTEGER   GetRevokeOptions();
  SQLUINTEGER   GetRowConstructorOptions();
  SQLUINTEGER   GetValueExpressionOptions();
  SQLSMALLINT   GetTableCorrelationOptions();
  SQLUINTEGER   GetOuterJoinOptions();
  SQLUINTEGER   GetOuterJoin1992Options();
  SQLUINTEGER   GetPredicateOptions();
  SQLUINTEGER   GetPredicateSubqueryOptions();
  SQLUINTEGER   GetGroupByOptions();
  SQLUINTEGER   GetUnionOptions();
  SQLUINTEGER   GetUpdateOptions();
  SQLSMALLINT   GetIdentifierCase();
  SQLSMALLINT   GetCatalogLocation();
  SQLUINTEGER   GetCatalogNameUsage();
  SQLUINTEGER   GetSchemaNameUsage();
  SQLSMALLINT   GetNULLCollation();
  SQLSMALLINT   GetNULLConcatBehaviour();
  SQLSMALLINT   GetColumnsCanBeNULL();
  SQLUINTEGER   GetSupportedDateTimeLiterals();
  SQLSMALLINT   GetTransactionCapabilities();
  SQLINTEGER    GetDefaultTransactionLevel();
  SQLSMALLINT   GetCursorCommitBehaviour();
  SQLSMALLINT   GetCursorRollbackBehaviour();
  SQLINTEGER    GetInformationSchemaViews();
  SQLUINTEGER   GetGetDataExtensions();
  // Ordinal maximum
  SQLSMALLINT   GetMaxActiveEnvironments();
  SQLSMALLINT   GetMaxColumnsInIndex();
  SQLSMALLINT   GetMaxColumnsInSelect();
  SQLSMALLINT   GetMaxColumnsInGroupBy();
  SQLSMALLINT   GetMaxColumnsInOrderBy();
  SQLSMALLINT   GetMaxTablesInSelect();
  SQLSMALLINT   GetMaxCatalogNameLength();
  SQLSMALLINT   GetMaxSchemaNameLength();
  SQLSMALLINT   GetMaxTableNameLength();
  SQLSMALLINT   GetMaxColumnNameLength();
  SQLSMALLINT   GetMaxColumnsInTable();
  // Booleans
  bool          GetColumnAliasAllowed();
  bool          GetOrderByInSelectAllowed();
  bool          GetExpressionsInOrderByAllowed();
  bool          GetSupportsCatalogs();
  bool          GetTransactionIntegrity();
  bool          GetAccessibleTables();
  bool          GetAccessibleProcedures();
  bool          GetSupportsV3Functions();
  // Strings
  CString       GetCatalogTerm();
  CString       GetSchemaTerm();
  CString       GetTableTerm();
  CString       GetProcedureTerm();
  CString       GetCatalogNameSeparator();
  CString       GetSpecialCharacters();
  CString       GetLikeEscapeCharacter();
  CString       GetIdentifierQuoteCharacter();
  CString       GetCollationSequence();
  // Function arrays
  SQLUSMALLINT* GetFunctionArrayV2();
  SQLUSMALLINT* GetFunctionArrayV3();

  // Get the catalog.schema.table from a user string
  void    GetObjectName(CString pattern,CString& p_catalog,CString& p_schema,CString& p_table);
  
private:
  // SQLDatabase has access to attribute methods
  friend SQLDatabase;

  // Get the supported ODBC functions (2.x/3.x)
  void SupportedODBCFunctions();
  // Make a statement handle for METADATA query
  bool GetStatement(bool p_metadataID = true);
  // Close the internal statement handle
  void CloseStatement();
  // Get a string from GetInfo with extra security checks for overflow
  CString GetInfoString(SQLUSMALLINT info);
  // Get a 32 bit integer value from GetInfo
  unsigned int GetInfoInteger(SQLUSMALLINT info);
  // Get a 16 bit integer value from GetInfo
  unsigned short GetInfoShortInteger(SQLUSMALLINT info);

  // Getting a general INTEGER connection attribute
  int     GetAttributeInteger(LPCTSTR description,SQLINTEGER attrib);
  // Getting a general STRING connection attribute
  CString GetAttributeString(CString description,SQLINTEGER attrib);
  // Setting an INTEGER attribute
  bool    SetAttributeInteger(CString description,SQLINTEGER  attrib,SQLUINTEGER value);
  // Setting a STRING attribute
  bool    SetAttributeString(CString description,SQLINTEGER attrib,SQLCHAR* value);

protected:
  // Reprint the catalog.schema.table combination
  CString MakeObjectName(SQLCHAR* search_catalog
                        ,SQLCHAR* search_schema
                        ,SQLCHAR* search_table
                        ,SQLCHAR* search_type);
  void    ReadingDataTypes();
  void    InfoMessageBox(CString p_message,UINT p_type = MB_OK);

protected:
  bool           m_initDone;           // Already read in?
  SQLDatabase*   m_database;           // Database for which we provide the info
  HDBC           m_hdbc;               // Database handle (if open)
  HSTMT          m_hstmt;              // Statement handle for info of tables/procedures
  RETCODE        m_retCode;            // Generic return code from ::SQL function

  WordList    m_ODBCKeywords;          // Stationary ODBC keywords
  WordList    m_RDBMSkeywords;         // Keywords  reported by the RDBMS
  DataTypeMap m_dataTypes;             // Datatypes reported by the RDBMS

  // CONFORMANCE TO THE SQL-LANGUAGE
  CString      m_cli_year;             // Year of X/Open CLI standard
  SQLUINTEGER  m_sql_conformance;      // Overall SQL  Conformance
  SQLUINTEGER  m_odbc_conformance;     // Overall ODBC Conformance
  SQLUINTEGER  m_cli_conformance;      // Overall CLI  Conformance
  SQLUINTEGER  m_aggr_functions;       // Supports  MIN/MAX/SUM/COUNT/AVG/DISTINCT
  SQLUINTEGER  m_alterDomain;          // Supported ALTER  DOMAIN variants
  SQLUINTEGER  m_alterTable;           // Supported ALTER  TABLE  variants
  SQLUINTEGER  m_createDomain;         // Supported CREATE DOMAIN variants
  SQLUINTEGER  m_createSchema;         // Supported CREATE SCHEMA variants
  SQLUINTEGER  m_createTable;          // Supported CREATE TABLE  variants
  SQLUINTEGER  m_createView;           // Supported CREATE VIEW   variants
  SQLUINTEGER  m_dropDomain;           // Supported DROP   DOMAIN variants
  SQLUINTEGER  m_dropSchema;           // Supported DROP   SCHEMA variants
  SQLUINTEGER  m_dropTable;            // Supported DROP   TABLE  variants
  SQLUINTEGER  m_dropView;             // Supported DROP   VIEW   variants
  SQLUINTEGER  m_grantOptions;         // Supported GRANT         variants
  SQLUINTEGER  m_insertOptions;        // Supported INSERT        variants
  SQLUINTEGER  m_revokeOptions;        // Supported REVOKE        variants
  SQLUINTEGER  m_indexOptions1;        // Supported CREATE/DROP INDEX
  SQLUINTEGER  m_indexOptions2;        // Supported ASC/DESC on INDEX
  SQLUINTEGER  m_predicates;           // Supported WHERE predicates
  SQLUINTEGER  m_groupBy;              // Supported GROUP BY behaviour
  SQLUINTEGER  m_unions;               // Supported UNION variants
  SQLUINTEGER  m_rowConstructors;      // Supported row constructors in select
  SQLSMALLINT  m_maxColSelect;         // Supported max columns in select
  SQLSMALLINT  m_maxTabSelect;         // Supported max tables  in FROM
  SQLSMALLINT  m_maxColGroupBy;        // Supported max columns in group by
  SQLSMALLINT  m_maxColOrderBy;        // Supported max columns in order by
  SQLSMALLINT  m_maxColTable;          // Supported max columns in a table
  SQLSMALLINT  m_maxColIndex;          // Supported max columns in a index
  SQLUINTEGER  m_funcNumeric;          // Supported numeric   functions
  SQLUINTEGER  m_funcString;           // Supported string    functions ODBC 1.x/2.x
  SQLUINTEGER  m_funcString3;          // Supported string    functions ODBC 3.x
  SQLUINTEGER  m_funcSystem;           // Supported system    functions
  SQLUINTEGER  m_funcDateTime;         // Supported date/time functions
  SQLUINTEGER  m_funcTimestampadd;     // Supported TIMESTAMPADD  fields
  SQLUINTEGER  m_funcTimestampdiff;    // Supported TIMESTAMPDIFF fields
  bool         m_orderByInSelect;      // Order by must be in the select
  bool         m_columnAliases;        // Supports "columnname AS alias"
  bool         m_exprInOrderBy;        // Supports expressions in the ORDER BY
  SQLSMALLINT  m_correlationNames;     // Correlation of table aliases
  SQLUINTEGER  m_oj_cap;               // Outer join capabilities in WHERE
  SQLUINTEGER  m_oj_cap92;             // Outer join capabilities in FROM
  SQLUINTEGER  m_valExpressions;       // Value Expressions
  SQLUINTEGER  m_predSubqueries;       // Predicates that support subqueries
  SQLUINTEGER  m_deleteRules;          // Supported DELETE rules
  SQLUINTEGER  m_updateRules;          // Supported UPDATE rules

  // How an object is named. Standard is m_catalogName.m_schemaName.m_tableName
  bool         m_supportsCatalogs;      // Database supports catalogs
  CString      m_catalogName;           // empty if catalogs ar not supported
  CString      m_schemaName;            // Standard is 'owner'
  CString      m_tableName;             // Standard is 'table'
  CString      m_procedureName;         // Standard is 'procedure'
  SQLUSMALLINT m_catalogLocation;       // Before schema or after tablename
  SQLUINTEGER  m_catalogUsage;          // Where catalog names can be used
  SQLUINTEGER  m_schemaUsage;           // WHere schema names can be used
  CString      m_catalogNameSeparator;  // Separator for the catalog name
  SQLSMALLINT  m_identifierCase;        // Case-specific of names in the catalog

  // RDBMS Implementation specifics
  SQLSMALLINT  m_fileUsage;             // Type of database storage!
  SQLSMALLINT  m_maxCatalogName;
  SQLSMALLINT  m_maxSchemaName;
  SQLSMALLINT  m_maxTableName;
  SQLSMALLINT  m_maxColumnName;
  bool         m_accesibleProcedures;   // User can execute all returned names
  bool         m_accesibleTables;       // User can select from all returned names
  SQLSMALLINT  m_txn_cap;               // Capable of transactions
  SQLINTEGER   m_txnLevel;              // Current transacation level
  SQLSMALLINT  m_cursor_commit;
  SQLSMALLINT  m_cursor_rollback;
  SQLSMALLINT  m_activeEnvironments;    // Maximum of environments (catalogs?)
  CString      m_specialChars;
  CString      m_likeEscape;
  SQLUINTEGER  m_datetimeLiterals;      // Supported date/time/interval literals
  CString      m_identifierQuote;       // RDBMS identifier quote character
  CString      m_collationSequence;     // RDBMS default collation sequence
  SQLINTEGER   m_defaultTransaction;    // RDBMS default transaction level
  SQLINTEGER   m_schemaViews;           // INFORMATION_SCHEMA support
  SQLSMALLINT  m_concatBehaviour;       // NULL concat behaviour
  CString      m_userName;              // User name in database
  SQLSMALLINT  m_nullCollation;         // How NULL's are sorted
  SQLSMALLINT  m_nullableColumns;       // Columns can be nullable
  bool         m_integrity;             // Integrity support
  bool         m_needLongDataLen;       // Need length for SQL_AT_EXEC 
  SQLINTEGER   m_packetSize;            // Optimization of the IP packet size
  SQLINTEGER   m_connTimeout;           // Connection timeout
  SQLINTEGER   m_odbcCursors;           // Using ODBC Cursors
  CString      m_transLib;              // Translation library
  SQLINTEGER   m_transOption;           // Translation option

  // DATATYPE conversions
  SQLUINTEGER  m_conversionFuncs;       // Supported conversion functions
  SQLINTEGER   m_convertBigint;         // Convert BIGINT to ...
  SQLINTEGER   m_convertBinary;         // Convert BINARY to ...
  SQLINTEGER   m_convertBit;            // Convert BIT to ...
  SQLINTEGER   m_convertChar;           // Convert CHAR to ...
  SQLINTEGER   m_convertDate;           // Convert DATE to ...
  SQLINTEGER   m_convertDecimal;        // Convert DECIMAL to ...
  SQLINTEGER   m_convertDouble;         // Convert DOUBLE to ...
  SQLINTEGER   m_convertFloat;          // Convert FLOAT to ...
  SQLINTEGER   m_convertInteger;        // Convert INTEGER to ...
  SQLINTEGER   m_convertIYM;            // Convert INTERVAL year to month to ...
  SQLINTEGER   m_convertIDT;            // Convert INTERVAL day to second to ...
  SQLINTEGER   m_convertLVarBinary;     // Convert LONGVARBINARY to ...
  SQLINTEGER   m_convertLVarchar;       // Convert LONGVARCHAR to ...
  SQLINTEGER   m_convertNumeric;        // Convert NUMERIC to ...
  SQLINTEGER   m_convertReal;           // Convert REAL to ...
  SQLINTEGER   m_convertSmallint;       // Convert SMALLINT to ...
  SQLINTEGER   m_convertTime;           // Convert TIME to ...
  SQLINTEGER   m_convertTimestamp;      // Convert TIMESTAMP to ...
  SQLINTEGER   m_convertTinyint;        // Convert TINYINT to ...
  SQLINTEGER   m_convertVarBinary;      // Convert VARBINARY to ...
  SQLINTEGER   m_convertVarchar;        // Convert VARCHAR to ...

  // VERSION NUMBERS of the various layers
  CString      m_manager_version;      // ODBC Driver manager version (from MS-Windows)
  CString      m_odbc_driver_version;  // ODBC Driver version
  CString      m_odbc_version;         // ODBC Standard version
  CString      m_rdbms_version;        // RDBMS Version
  // The ODBC Driver
  CString      m_driverName;           // Name of the driver DLL
  SQLUINTEGER  m_getdata_extensions;   // Driver SQLGetData extensions
  // File DSN
  CString      m_fileDSN;
  CString      m_fileDSNSave;

  // Work arounds
  bool         m_metadataID;
  bool         m_METADATA_ID_unsupported;
  bool         m_METADATA_ID_errorseen;
  // ODBC supported functions
  bool         m_functions_use_3;
  SQLUSMALLINT m_ODBCFunctions_2[100];   // ODBC 1.x and 2.x standard is 100 int's
  SQLUSMALLINT m_ODBCFunctions_3[SQL_API_ODBC3_ALL_FUNCTIONS_SIZE]; // ODBC 3.x standard = 4000 bits

  // How to use META types
  bool         m_canFindCatalogs;
  bool         m_canFindSchemas;
  bool         m_canFindTypes;
};
 
inline CString 
SQLInfo::GetVersionODBCManager()
{
  return m_manager_version;
}

inline CString 
SQLInfo::GetVersionODBCDriver()
{
  return m_odbc_driver_version;
}

inline CString 
SQLInfo::GetVersionODBCStandard()
{
  return m_odbc_version;
}

inline CString 
SQLInfo::GetVersionRDBMS()
{
  return m_rdbms_version;
}

inline bool 
SQLInfo::NeedLongDataLen()
{
  return m_needLongDataLen;
}

inline void
SQLInfo::ShowMetaDataWarning(bool p_show)
{
  m_METADATA_ID_errorseen = !p_show;
}

// Connection by File DSN
inline CString 
SQLInfo::GetAttributeFileDSN()
{
  return m_fileDSN;
}

inline CString 
SQLInfo::GetAttributeFileDSNSave()
{
  return m_fileDSNSave;
}

inline void
SQLInfo::SetAttributeFileDSN(CString p_fileDSN)
{
  m_fileDSN = p_fileDSN;
}

inline void
SQLInfo::SetAttributeFileDSNSave(CString p_fileDSN)
{
  m_fileDSNSave = p_fileDSN;
}

inline WordList& 
SQLInfo::GetODBCKeywords()
{
  return m_ODBCKeywords;
}

inline WordList&
SQLInfo::GetRDBMSKeywords()
{
  return m_RDBMSkeywords;
}

inline DataTypeMap&  
SQLInfo::GetDataTypeMap()
{
  return m_dataTypes;
}

inline CString
SQLInfo::GetDriverName()
{
  return m_driverName;
}

inline SQLUINTEGER   
SQLInfo::GetConversionFunctions()
{
  return m_conversionFuncs;
}

inline SQLINTEGER    
SQLInfo::GetConvertBigint()
{
  return m_convertBigint;
}

inline SQLINTEGER    
SQLInfo::GetConvertBinary()
{
  return m_convertBinary;
}

inline SQLINTEGER    
SQLInfo::GetConvertBit()
{
  return m_convertBit;
}

inline SQLINTEGER    
SQLInfo::GetConvertChar()
{
  return m_convertChar;
}

inline SQLINTEGER    
SQLInfo::GetConvertDate()
{
  return m_convertDate;
}

inline SQLINTEGER    
SQLInfo::GetConvertDecimal()
{
  return m_convertDecimal;
}

inline SQLINTEGER    
SQLInfo::GetConvertDouble()
{
  return m_convertDouble;
}

inline SQLINTEGER    
SQLInfo::GetConvertFloat()
{
  return m_convertFloat;
}

inline SQLINTEGER    
SQLInfo::GetConvertInteger()
{
  return m_convertInteger;
}

inline SQLINTEGER    
SQLInfo::GetConvertIYM()
{
  return m_convertIYM;
}

inline SQLINTEGER    
SQLInfo::GetConvertIDT()
{
  return m_convertIDT;
}

inline SQLINTEGER    
SQLInfo::GetConvertLVarBinary()
{
  return m_convertVarBinary;
}

inline SQLINTEGER    
SQLInfo::GetConvertLVarchar()
{
  return m_convertLVarchar;
}

inline SQLINTEGER    
SQLInfo::GetConvertNumeric()
{
  return m_convertNumeric;
}

inline SQLINTEGER    
SQLInfo::GetConvertReal()
{
  return m_convertReal;
}

inline SQLINTEGER    
SQLInfo::GetConvertSmallint()
{
  return m_convertSmallint;
}

inline SQLINTEGER    
SQLInfo::GetConvertTime()
{
  return m_convertTime;
}

inline SQLINTEGER    
SQLInfo::GetConvertTimestamp()
{
  return m_convertTimestamp;
}

inline SQLINTEGER    
SQLInfo::GetConvertTinyint()
{
  return m_convertTinyint;
}

inline SQLINTEGER    
SQLInfo::GetConvertVarBinary()
{
  return m_convertVarBinary;
}

inline SQLINTEGER    
SQLInfo::GetConvertVarchar()
{
  return m_convertVarchar;
}

inline SQLUINTEGER   
SQLInfo::GetSQLConformance()
{
  return m_sql_conformance;
}

inline SQLUINTEGER   
SQLInfo::GetODBCConformance()
{
  return m_odbc_conformance;
}

inline SQLUINTEGER   
SQLInfo::GetCLIConformance()
{
  return m_cli_conformance;
}

inline CString
SQLInfo::GetCLIYear()
{
  return m_cli_year;
}

inline SQLUINTEGER   
SQLInfo::GetAggregationFunctions()
{
  return m_aggr_functions;
}

inline SQLUINTEGER   
SQLInfo::GetNumericFunctions()
{
  return m_funcNumeric;
}

inline SQLUINTEGER   
SQLInfo::GetStringFunctions()
{
  return m_funcString;
}

inline SQLUINTEGER
SQLInfo::GetStringFunctions3()
{
  return m_funcString3;
}

inline SQLUINTEGER   
SQLInfo::GetDateTimeFunctions()
{
  return m_funcDateTime;
}

inline SQLUINTEGER   
SQLInfo::GetTimestampAddFunctions()
{
  return m_funcTimestampadd;
}

inline SQLUINTEGER   
SQLInfo::GetTimestampDiffFunctions()
{
  return m_funcTimestampdiff;
}

inline SQLUINTEGER   
SQLInfo::GetSystemFunctions()
{
  return m_funcSystem;
}

inline SQLUINTEGER   
SQLInfo::GetAlterDomainFunctions()
{
  return m_alterDomain;
}

inline SQLUINTEGER   
SQLInfo::GetAlterTableFunctions()
{
  return m_alterTable;
}

inline SQLUINTEGER   
SQLInfo::GetCreateSchemaFunctions()
{
  return m_createSchema;
}

inline SQLUINTEGER   
SQLInfo::GetCreateDomainFunctions()
{
  return m_createDomain;
}

inline SQLUINTEGER   
SQLInfo::GetCreateTableFunctions()
{
  return m_createTable;
}

inline SQLUINTEGER   
SQLInfo::GetCreateViewFunctions()
{
  return m_createView;
}

inline SQLUINTEGER   
SQLInfo::GetDeleteRuleFunctions()
{
  return m_deleteRules;
}

inline SQLUINTEGER   
SQLInfo::GetDropSchemaFunctions()
{
  return m_dropSchema;
}

inline SQLUINTEGER   
SQLInfo::GetDropDomainFunctions()
{
  return m_dropDomain;
}

inline SQLUINTEGER   
SQLInfo::GetDropTableFunctions()
{
  return m_dropTable;
}

inline SQLUINTEGER   
SQLInfo::GetDropViewFunctions()
{
  return m_dropView;
}

inline SQLUINTEGER   
SQLInfo::GetGrantOptions()
{
  return m_grantOptions;
}

inline SQLUINTEGER   
SQLInfo::GetIndexOptions1()
{
  return m_indexOptions1;
}

inline SQLUINTEGER   
SQLInfo::GetIndexOptions2()
{
  return m_indexOptions2;
}

inline SQLUINTEGER   
SQLInfo::GetInsertOptions()
{
  return m_insertOptions;
}

inline SQLUINTEGER   
SQLInfo::GetRevokeOptions()
{
  return m_revokeOptions;
}

inline SQLSMALLINT   
SQLInfo::GetMaxColumnsInIndex()
{
  return m_maxColIndex;
}

inline SQLSMALLINT   
SQLInfo::GetMaxColumnsInSelect()
{
  return m_maxColSelect;
}

inline SQLSMALLINT   
SQLInfo::GetMaxTablesInSelect()
{
  return m_maxTabSelect;
}

inline bool
SQLInfo::GetColumnAliasAllowed()
{
  return m_columnAliases;
}

inline SQLUINTEGER   
SQLInfo::GetRowConstructorOptions()
{
  return m_rowConstructors;
}

inline SQLUINTEGER   
SQLInfo::GetValueExpressionOptions()
{
  return m_valExpressions;
}

inline SQLSMALLINT   
SQLInfo::GetTableCorrelationOptions()
{
  return m_correlationNames;
}

inline SQLUINTEGER   
SQLInfo::GetOuterJoinOptions()
{
  return m_oj_cap;
}

inline SQLUINTEGER   
SQLInfo::GetOuterJoin1992Options()
{
  return m_oj_cap92;
}

inline SQLUINTEGER   
SQLInfo::GetPredicateOptions()
{
  return m_predicates;
}

inline SQLUINTEGER   
SQLInfo::GetPredicateSubqueryOptions()
{
  return m_predSubqueries;
}

inline SQLUINTEGER   
SQLInfo::GetGroupByOptions()
{
  return m_groupBy;
}

inline SQLSMALLINT   
SQLInfo::GetMaxColumnsInGroupBy()
{
  return m_maxColGroupBy;
}

inline bool
SQLInfo::GetOrderByInSelectAllowed()
{
  return m_orderByInSelect;
}

inline bool
SQLInfo::GetExpressionsInOrderByAllowed()
{
  return m_exprInOrderBy;
}

inline SQLSMALLINT   
SQLInfo::GetMaxColumnsInOrderBy()
{
  return m_maxColOrderBy;
}

inline SQLUINTEGER   
SQLInfo::GetUnionOptions()
{
  return m_unions;
}

inline SQLUINTEGER   
SQLInfo::GetUpdateOptions()
{
  return m_updateRules;
}

inline SQLSMALLINT   
SQLInfo::GetDatabaseFileUsage()
{
  return m_fileUsage;
}

inline bool
SQLInfo::GetSupportsCatalogs()
{
  return m_supportsCatalogs;
}

inline CString       
SQLInfo::GetCatalogTerm()
{
  return m_catalogName;
}

inline CString       
SQLInfo::GetSchemaTerm()
{
  return m_schemaName;
}

inline CString       
SQLInfo::GetTableTerm()
{
  return m_tableName;
}

inline CString       
SQLInfo::GetProcedureTerm()
{
  return m_procedureName;
}

inline SQLSMALLINT   
SQLInfo::GetIdentifierCase()
{
  return m_identifierCase;
}

inline SQLSMALLINT   
SQLInfo::GetCatalogLocation()
{
  return m_catalogLocation;
}

inline CString
SQLInfo::GetCatalogNameSeparator()
{
  return m_catalogNameSeparator;
}

inline SQLUINTEGER   
SQLInfo::GetCatalogNameUsage()
{
  return m_catalogUsage;
}

inline SQLUINTEGER
SQLInfo::GetSchemaNameUsage()
{
  return m_schemaUsage;
}

inline SQLSMALLINT   
SQLInfo::GetMaxCatalogNameLength()
{
  return m_maxCatalogName;
}

inline SQLSMALLINT   
SQLInfo::GetMaxSchemaNameLength()
{
  return m_maxSchemaName;
}

inline SQLSMALLINT   
SQLInfo::GetMaxTableNameLength()
{
  return m_maxTableName;
}

inline SQLSMALLINT   
SQLInfo::GetMaxColumnNameLength()
{
  return m_maxColumnName;
}

inline SQLSMALLINT   
SQLInfo::GetMaxColumnsInTable()
{
  return m_maxColTable;
}

inline CString       
SQLInfo::GetSpecialCharacters()
{
  return m_specialChars;
}

inline CString       
SQLInfo::GetLikeEscapeCharacter()
{
  return m_likeEscape;
}

inline CString       
SQLInfo::GetIdentifierQuoteCharacter()
{
  return m_identifierQuote;
}

inline CString       
SQLInfo::GetCollationSequence()
{
  return m_collationSequence;
}

inline SQLSMALLINT   
SQLInfo::GetNULLCollation()
{
  return m_nullCollation;
}

inline SQLSMALLINT   
SQLInfo::GetNULLConcatBehaviour()
{
  return m_concatBehaviour;
}

inline SQLSMALLINT   
SQLInfo::GetColumnsCanBeNULL()
{
  return m_nullableColumns;
}

inline SQLUINTEGER   
SQLInfo::GetSupportedDateTimeLiterals()
{
  return m_datetimeLiterals;
}

inline SQLSMALLINT   
SQLInfo::GetMaxActiveEnvironments()
{
  return m_activeEnvironments;
}

inline bool
SQLInfo::GetTransactionIntegrity()
{
  return m_integrity;
}

inline SQLSMALLINT   
SQLInfo::GetTransactionCapabilities()
{
  return m_txn_cap;
}

inline SQLINTEGER    
SQLInfo::GetDefaultTransactionLevel()
{
  return m_defaultTransaction;
}

inline SQLSMALLINT   
SQLInfo::GetCursorCommitBehaviour()
{
  return m_cursor_commit;
}

inline SQLSMALLINT   
SQLInfo::GetCursorRollbackBehaviour()
{
  return m_cursor_rollback;
}

inline bool
SQLInfo::GetAccessibleTables()
{
  return m_accesibleTables;
}

inline bool
SQLInfo::GetAccessibleProcedures()
{
  return m_accesibleProcedures;
}

inline SQLINTEGER    
SQLInfo::GetInformationSchemaViews()
{
  return m_schemaViews;
}

inline SQLUINTEGER   
SQLInfo::GetGetDataExtensions()
{
  return m_getdata_extensions;
}

inline bool
SQLInfo::GetSupportsV3Functions()
{
  return m_functions_use_3;
}

inline SQLUSMALLINT* 
SQLInfo::GetFunctionArrayV2()
{
  return m_ODBCFunctions_2;
}

inline SQLUSMALLINT* 
SQLInfo::GetFunctionArrayV3()
{
  return m_ODBCFunctions_3;
}

// End of namespace
}
