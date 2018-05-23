////////////////////////////////////////////////////////////////////////
//
// File: SQLInfoInformix.cpp
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
#include "stdafx.h"
#include "SQLComponents.h"
#include "SQLInfoInformix.h"
#include "SQLQuery.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace SQLComponents
{
  
// Constructor. 
SQLInfoInformix::SQLInfoInformix(SQLDatabase* p_database)
                :SQLInfoDB(p_database)
{
}

// Destructor. Does nothing
SQLInfoInformix::~SQLInfoInformix()
{
}

//////////////////////////////////////////////////////////////////////////
//
// GENERALS (Strings & Booleans) 
//
//////////////////////////////////////////////////////////////////////////

// Get the database type
// DatabaseType GetDatabaseType() const;
DatabaseType
SQLInfoInformix::GetRDBMSDatabaseType() const
{
  return RDBMS_INFORMIX;
}

// The name of the database vendor
CString
SQLInfoInformix::GetRDBMSVendorName() const
{
  // The name of the database vendor
  return "IBM-Informix";
}

// Get the physical database name
CString
SQLInfoInformix::GetRDBMSPhysicalDatabaseName() const
{
  // Oops. We have an INFORMIX CLI bug
  // Versions of the INFORMIX CLI above 2.5 reflect in the connection string
  // only the name of the service of the DNS and not the 'real' database
  // That is why we read the database name from the sysmaster database
  // Beware that this only works for and above INFORMIX 7.x.y
  CString query = "SELECT scs_currdb\n"
                  "  FROM sysmaster:syssqlcurses\n"
                  " WHERE scs_sessionid = DBINFO('sessionid')";
  SQLQuery qry(m_database);
  qry.DoSQLStatement(query);
  if(qry.GetRecord())
  {
    return qry.GetColumn(1)->GetAsChar();
  }
  return CString("");
}

// System catalog is stored in uppercase in the database?
bool
SQLInfoInformix::GetRDBMSIsCatalogUpper() const
{
  return false;
}

// System catalog supports full ISO schemas (same tables per schema)
bool
SQLInfoInformix::GetRDBMSUnderstandsSchemas() const
{
  return false;
}

// Supports database/ODBCdriver comments in SQL
bool
SQLInfoInformix::GetRDBMSSupportsComments() const
{
  // Does not work anymore from and above version 3.34 of KIT 2.70-TC3
  return false;
}

// Database can defer constraints until the end of a transaction
bool
SQLInfoInformix::GetRDBMSSupportsDeferredConstraints() const
{
  // SET CONSTRAINTS DEFERRED is supported
  return true;
}

// Database has ORDER BY with an expression, e.g. ORDER BY UPPER(columnname)
bool
SQLInfoInformix::GetRDBMSSupportsOrderByExpression() const
{
  return false;
}

// Supports the ODBC escape sequence {[?=] CALL procedure (?,?,?)}
bool
SQLInfoInformix::GetRDBMSSupportsODBCCallEscapes() const
{
  return true;
}

// If the database does not support the datatype TIME, it can be implemented as a DECIMAL
bool
SQLInfoInformix::GetRDBMSSupportsDatatypeTime() const
{
  // Time can be implemented as TIME
  return true;
}

// If the database does not support the datatype INTERVAL, it can be implemented as a DECIMAL
bool
SQLInfoInformix::GetRDBMSSupportsDatatypeInterval() const
{
  // Interval supported
  return true;
}

// Gets the maximum length of an SQL statement
unsigned long 
SQLInfoInformix::GetRDBMSMaxStatementLength() const
{
  // The current limit of the INFORMIX ODBC driver is MAX_UINT16
  // We limit somewhat lower, just to be on the sure side
  return 64000;
}

// Database must commit DDL commands in a transaction
bool
SQLInfoInformix::GetRDBMSMustCommitDDL() const
{
  return false;
}

// KEYWORDS

// Keyword for the current date and time
CString
SQLInfoInformix::GetKEYWORDCurrentTimestamp() const
{
  return "current";
}

// String for the current date
CString
SQLInfoInformix::GetKEYWORDCurrentDate() const
{
  return "current_date";
}

// Get the concatenation operator
CString
SQLInfoInformix::GetKEYWORDConcatanationOperator() const
{
  return "||";
}

// Get quote character for strings
CString
SQLInfoInformix::GetKEYWORDQuoteCharacter() const
{
  return "\'";
}

// Get default NULL for parameter list input
CString
SQLInfoInformix::GetKEYWORDParameterDefaultNULL() const
{
  // Standard, no definition defines the NULL state
  return "";
}

// Parameter is for INPUT and OUTPUT in parameter list
CString
SQLInfoInformix::GetKEYWORDParameterINOUT() const
{
  return "";
}

// Parameter is for OUTPUT only in parameter list
CString
SQLInfoInformix::GetKEYWORDParameterOUT() const
{
  return "";
}

// Get datatype of the IDENTITY primary key in a Network database
CString
SQLInfoInformix::GetKEYWORDNetworkPrimaryKeyType() const
{
  return "serial";
}

// Get datatype for timestamp (year to second)
CString
SQLInfoInformix::GetKEYWORDTypeTimestamp() const
{
  return "datetime year to second";
}

// Prefix for a parameter in a stored procedure
CString
SQLInfoInformix::GetKEYWORDParameterPrefix() const
{
  return "";
}

// Get select part to add new record identity to a table
// Can be special column like 'OID' or a sequence select
CString
SQLInfoInformix::GetKEYWORDIdentityString(CString& /*p_tablename*/,CString /*p_postfix*/ /*= "_seq"*/) const
{
  // Insert 0 for a SERIAL column
  return "0";
}

// Gets the UPPER function
CString
SQLInfoInformix::GetKEYWORDUpper(CString& p_expression) const
{
  return "UPPER(" + p_expression + ")";
}

// Gets the construction for 1 minute ago
CString
SQLInfoInformix::GetKEYWORDInterval1MinuteAgo() const
{
  return "(CURRENT - INTERVAL (1) MINUTE TO MINUTE)";
}

// Gets the Not-NULL-Value statement of the database
CString
SQLInfoInformix::GetKEYWORDStatementNVL(CString& p_test,CString& p_isnull) const
{
  return CString("NVL(") + p_test + "," + p_isnull + ")";
}

// Gets the construction for inline generating a key within an INSERT statement
CString
SQLInfoInformix::GetSQLNewSerial(CString /*p_table*/, CString /*p_sequence*/) const
{
  // Insert a zero in an SERIAL column
  return "0";
}

// Gets the construction / select for generating a new serial identity
CString 
SQLInfoInformix::GetSQLGenerateSerial(CString p_table) const
{
  // Is generated by the database type SERIAL
  return "0";
}

// Gets the construction / select for the resulting effective generated serial
CString 
SQLInfoInformix::GetSQLEffectiveSerial(CString p_identity) const
{
  return "SELECT DBINFO('sqlca.sqlerrd1')\n"
         "  FROM systables WHERE tabid = 1";
}

// Gets the sub transaction commands
CString
SQLInfoInformix::GetSQLStartSubTransaction(CString p_savepointName) const
{
  return CString("SAVEPOINT ") + p_savepointName;
}

CString
SQLInfoInformix::GetSQLCommitSubTransaction(CString p_savepointName) const
{
  return CString("COMMIT TRANSACTION ") + p_savepointName;
}

CString
SQLInfoInformix::GetSQLRollbackSubTransaction(CString p_savepointName) const
{
  return CString("ROLLBACK TO SAVEPOINT ") + p_savepointName;
}

// FROM-Part for a query to select only 1 (one) record / or empty!
CString
SQLInfoInformix::GetSQLFromDualClause() const
{
  // Systables entry in systables is guaranteed to always be there!
  return " FROM systables WHERE tabid = 1";
}

// Get SQL to lock  a table 
CString
SQLInfoInformix::GetSQLLockTable(CString /*p_schema*/, CString p_tablename, bool p_exclusive) const
{
  CString query = "LOCK TABLE " + p_tablename + " IN ";
  query += p_exclusive ? "EXCLUSIVE" : "SHARE";
  query += " MODE";
  return query;
}

// Get query to optimize the table statistics
CString
SQLInfoInformix::GetSQLOptimizeTable(CString /*p_schema*/,CString p_tablename) const
{
  CString optim;
  optim = "UPDATE STATISTICS LOW  FOR TABLE " + p_tablename + " DROP DISTRIBUTIONS;\n"
          "<@>\n"
          "UPDATE STATISTICS HIGH FOR TABLE " + p_tablename + ";\n";
  return optim;
}

//////////////////////////////////////////////////////////////////////////
//
// SQL STRINGS
//
//////////////////////////////////////////////////////////////////////////

// Makes a SQL string from a given string, with all the right quotes
CString
SQLInfoInformix::GetSQLString(const CString& p_string) const
{
  CString str = p_string;
  str.Replace("'","''");
  CString kwoot = GetKEYWORDQuoteCharacter();
  return kwoot + str + kwoot;
}

// Get date string in engine format
CString
SQLInfoInformix::GetSQLDateString(int p_year,int p_month,int p_day) const
{
  // Informix used to be depended on the DBFORMAT parameter
  // This form is independent of it's setting!
  CString dateString;
  dateString.Format("DATETIME(%04d-%02d-%02d) YEAR TO DAY",p_year,p_month,p_day);
  return dateString;
}

// Get time string in database engine format
CString
SQLInfoInformix::GetSQLTimeString(int p_hour,int p_minute,int p_second) const
{
  CString retval;
  retval.Format("DATETIME (%02d:%02d:%02d) HOUR TO SECOND",p_hour,p_minute,p_second);
  return retval;
}

// Get date-time string in database engine format
CString
SQLInfoInformix::GetSQLDateTimeString(int p_year,int p_month,int p_day,int p_hour,int p_minute,int p_second) const
{
  // Informix used to be depended on the DBFORMAT parameter
  // This form is independent of it's setting!
  CString string;
  string.Format("DATETIME(%04d-%02d-%02d %02d:%02d:%02d) YEAR TO SECOND"
                ,p_day,p_month,p_year
                ,p_hour,p_minute,p_second);
  return string;
}

// Get date-time bound parameter string in database format
CString
SQLInfoInformix::GetSQLDateTimeBoundString() const
{
  return "TO_DATE(?,'%d-%m-%Y %H:%M:%S')";
}

// Stripped data for the parameter binding
CString
SQLInfoInformix::GetSQLDateTimeStrippedString(int p_year,int p_month,int p_day,int p_hour,int p_minute,int p_second) const
{
  CString string;
  string.Format("%04d-%02d-%02d %02d:%02d:%02d"
                ,p_day,p_month,p_year
                ,p_hour,p_minute,p_second);
  return string;
}

//////////////////////////////////////////////////////////////////////////
//
// CATALOG
// o GetCATALOG<Object[s]><Function>
//   Objects
//   - Table
//   - Column
//   - Index
//   - PrimaryKey
//   - ForeignKey
//   - Trigger
//   - TemporaryTable 
//   - View
//   - Sequence
//  Functions per object type
//   - Exists
//   - List
//   - Attributes
//   - Create
//   - Alter  (where possible)
//   - Rename (where possible)
//   - Drop
//
//////////////////////////////////////////////////////////////////////////

// Get SQL to check if a table already exists in the database
CString
SQLInfoInformix::GetCATALOGTableExists(CString /*p_schema*/,CString p_tablename) const
{
  p_tablename.MakeLower();
  CString query = "SELECT count(*)\n"
                  "  FROM systables\n"
                  " WHERE tabname = '" + p_tablename + "'";
  return query;
}

CString
SQLInfoInformix::GetCATALOGTablesList(CString p_schema,CString p_pattern) const
{
  return GetCATALOGTableAttributes(p_schema,p_pattern);
}

CString
SQLInfoInformix::GetCATALOGTableAttributes(CString /*p_schema*/,CString /*p_tablename*/) const
{
  // Getting the temp table status
  return "";
}

CString
SQLInfoInformix::GetCATALOGTableSynonyms(CString p_schema,CString p_tablename) const
{
  p_schema.MakeLower();
  p_tablename.MakeLower();
  CString sql = "SELECT Trim(DBINFO('dbname')) AS synonym_catalog\n"
                "      ,Trim(tab.owner)   AS synonym_schema\n"
                "      ,Trim(tab.tabname) AS synonym_table\n"
                "      ,'SYNONYM'         AS object_type\n"
                "      ,''                AS remarks\n"
                "      ,Trim(tab.owner) || '.' || Trim(tab.tabname) AS fullname\n"
                "      ,''                AS storage\n"
                "      ,0                 AS temporary\n"
                "  FROM systables tab\n"
                " WHERE tabtype = 'S'\n";
  if(!p_schema.IsEmpty())
  {
    sql += "   AND owner = '" + p_schema + "'\n";
  }
  if(!p_tablename.IsEmpty())
  {
    sql += "   and tabname ";
    sql += p_tablename.Find('%') >= 0 ? "LIKE '" : "= '";
    sql += p_tablename + "'\n";
  }
  sql += " ORDER BY 1,2,3";
  return sql;
}

CString
SQLInfoInformix::GetCATALOGTableCatalog(CString /*p_schema*/,CString /*p_tablename*/) const
{
  return "";
}

CString
SQLInfoInformix::GetCATALOGTableCreate(MetaTable& /*p_table*/,MetaColumn& /*p_column*/) const
{
  return "";
}

// Rename a database table 
CString
SQLInfoInformix::GetCATALOGTableRename(CString /*p_schema*/,CString p_tablename,CString p_newname) const
{
  // Beware: 'TABLE' keyword in the statement
  CString sql("RENAME TABLE " + p_tablename + " TO " + p_newname);
  return sql;
}

CString
SQLInfoInformix::GetCATALOGTableDrop(CString /*p_schema*/,CString p_tablename) const
{
  return "DROP TABLE " + p_tablename;
}

//////////////////////////////////////////////////////////////////////////
// ALL TEMPORARY TABLE FUNCTIONS

CString 
SQLInfoInformix::GetCATALOGTemptableCreate(CString /*p_schema*/,CString p_tablename,CString p_select) const
{
  return p_select + " INTO TEMP " + p_tablename + " WITH NO LOG";
}

CString 
SQLInfoInformix::GetCATALOGTemptableIntoTemp(CString /*p_schema*/,CString p_tablename,CString p_select) const
{
  return p_select + " INTO TEMP " + p_tablename;
}

CString 
SQLInfoInformix::GetCATALOGTemptableDrop(CString /*p_schema*/,CString p_tablename) const
{
  return "DROP TABLE " + p_tablename;
}

//////////////////////////////////////////////////////////////////////////
// ALL COLUMN FUNCTIONS

CString 
SQLInfoInformix::GetCATALOGColumnExists(CString p_schema,CString p_tablename,CString p_columnname) const
{
  p_tablename.MakeLower();
  p_columnname.MakeLower();
  CString query = "SELECT count(*)\n"
                  "  FROM systables  tab\n"
                  "      ,syscolumns col\n"
                  " WHERE tab.tabid = col.tabid\n"
                  "   AND tab.tabname = '" + p_tablename  + "'\n"
                  "   AND col.colname = '" + p_columnname + "'";
  return query;
}

CString 
SQLInfoInformix::GetCATALOGColumnList(CString p_schema,CString p_tablename) const
{
  return "";
//   CString sql = GetCATALOGColumnAttributes(p_schema,p_tablename,"");
// 
//   int pos = sql.ReverseFind('\n');
//   sql = sql.Mid(pos + 1) + " ORDER BY col.colno";
// 
//   return sql;
}

CString 
SQLInfoInformix::GetCATALOGColumnAttributes(CString /*p_schema*/,CString p_tablename,CString p_columnname) const
{
  return "";
//   p_tablename.MakeLower();
//   p_columnname.MakeLower();
//   CString select = "SELECT col.colname\n"       // 1 -> Name
//                    "      ,col.colno\n"         // 2 -> Position
//                    "      ,col.coltype\n"       // 3 -> datatype
//                    "      ,col.collength\n"     // 4 -> Length
//                    "      ,def.default\n"       // 5 -> Default
//                    "  FROM systables   tab\n"
//                    "      ,syscolumns  col\n"
//                    "      ,sysdefaults def\n"
//                    " WHERE col.tabid   = tab.tabid\n"
//                    "   AND def.tabid   = tab.tabid\n"
//                    "   AND def.colno   = col.colno\n"
//                    "   AND tab.tabname = '" + p_tablename  + "'\n"
//                    "   AND col.colname = '" + p_columnname + "'";
//                    // Beware: columnname filter must be last for previous method!
//   return select;
}

CString 
SQLInfoInformix::GetCATALOGColumnCreate(MetaColumn& p_column) const
{
  CString sql = "ALTER TABLE "  + p_column.m_table  + "\n";
                "  ADD COLUMN " + p_column.m_column + " " + p_column.m_typename;
  p_column.GetPrecisionAndScale(sql);
  p_column.GetNullable(sql);
  // m_default  not used
  // m_position not used
  // m_length   not used
  // m_remarks  not used
  return sql;
}

CString 
SQLInfoInformix::GetCATALOGColumnAlter(MetaColumn& p_column) const
{
  // The MODIFY keyword is a-typical
  CString sql = "ALTER  TABLE  " + p_column.m_table  + "\n";
                "MODIFY COLUMN " + p_column.m_column + " " + p_column.m_typename;
  p_column.GetPrecisionAndScale(sql);
  p_column.GetNullable(sql);
  p_column.GetDefault(sql);
  // m_position not used
  // m_length   not used
  // m_remarks  not used
  return sql;
}

CString 
SQLInfoInformix::GetCATALOGColumnRename(CString /*p_schema*/,CString p_tablename,CString p_columnname,CString p_newname,CString /*p_datatype*/) const
{
  CString sql("ALTER  TABLE  " + p_tablename + "\n"
              "RENAME " + p_columnname + " TO " + p_newname + "\n");
  return sql;
}

CString
SQLInfoInformix::GetCATALOGColumnDrop(CString p_schema,CString p_tablename,CString p_columnname) const
{
  CString sql("ALTER TABLE "  + p_tablename + "\n"
              " DROP COLUMN " + p_columnname);
  return sql;
}

//////////////////////////////////////////////////////////////////////////
// ALL INDICES FUNCTIONS

// All index functions
CString
SQLInfoInformix::GetCATALOGIndexExists(CString p_schema,CString p_tablename,CString p_indexname) const
{
  return "";
}

CString
SQLInfoInformix::GetCATALOGIndexList(CString p_schema,CString p_tablename)   const
{
  CString query;
  p_tablename.MakeLower();

  // Reads all current indices in the database in a list
  // So we can figure out if an index need to be generated
  for(int ind = 1; ind <= INFORMIX_IDS_MAX_INDEX_COLUMNS; ++ind)
  {
    if(!query.IsEmpty())
    {
      query += "\nUNION ALL\n";
    }
    query.AppendFormat("SELECT idx.idxname AS index_name\n"
                       "      ,col.colname\n"
                       "      ,%d   AS index_column\n"
                       "      ,CASE WHEN idx.idxtype = 'D' THEN 0\n"
                       "            WHEN idx.idxtype = 'U' THEN 1\n"
                       "            ELSE 0 END AS is_unique\n"
                       "      ,CASE WHEN idx.part%d < 0 THEN 1\n"
                       "            ELSE 0 END AS descending\n"
                       "  FROM sysindexes idx\n"
                       "      ,systables  tab\n"
                       "      ,syscolumns col\n"
                       " WHERE tab.tabid = idx.tabid\n"
                       "   AND tab.tabname = '%s'\n"
                       "   AND col.tabid   = idx.tabid\n"
                       "   AND col.colno   = abs(idx.part%d)\n"
                       "   AND idx.idxname[1] != ' '"
                       ,ind
                       ,ind
                       ,p_tablename.GetString()
                       ,ind);
  }
  return query;
}

CString
SQLInfoInformix::GetCATALOGIndexAttributes(CString p_schema,CString p_tablename,CString p_indexname) const
{
  return "";
}

CString
SQLInfoInformix::GetCATALOGIndexCreate(MIndicesMap& p_indices) const
{
  // Get SQL to create an index for a table
  // CREATE [UNIQUE] INDEX [<schema>.]indexname ON [<schema>.]tablename(column [ASC|DESC] [,...]);
  CString query;
  for(auto& index : p_indices)
  {
    if(index.m_position == 1)
    {
      // New index
      query = "CREATE ";
      if(index.m_unique)
      {
        query += "UNIQUE ";
      }
      query += "INDEX ";
      if(!index.m_schemaName.IsEmpty())
      {
        query += index.m_schemaName + ".";
      }
      query += index.m_indexName;
      query += " ON ";
      if(!index.m_schemaName.IsEmpty())
      {
        query += index.m_schemaName + ".";
      }
      query += index.m_tableName;
      query += "(";
    }
    else
    {
      query += ",";
    }
    query += index.m_columnName;
    if(index.m_ascending != "A")
    {
      query += " DESC";
    }
  }
  query += ")";
  return query;
}

CString
SQLInfoInformix::GetCATALOGIndexDrop(CString /*p_schema*/,CString /*p_tablename*/,CString p_indexname) const
{
  CString sql = "DROP INDEX " + p_indexname;
  return sql;
}

// Get extra filter expression for an index column
CString
SQLInfoInformix::GetCATALOGIndexFilter(MetaIndex& /*p_index*/) const
{
  return "";
}

//////////////////////////////////////////////////////////////////////////
// ALL PRIMARY KEY FUNCTIONS

CString
SQLInfoInformix::GetCATALOGPrimaryExists(CString /*p_schema*/,CString p_tablename) const
{
  p_tablename.MakeLower();
  CString query = "SELECT COUNT(*)\n"
                  "  FROM systables      tab\n"
                  "      ,sysconstraints con\n"
                  " WHERE tab.tabid      = con.tabid\n"
                  "   AND tab.tabname    = '" + p_tablename + "'\n"
                  "   AND con.constrtype = 'P'";
  return query;
}

CString
SQLInfoInformix::GetCATALOGPrimaryAttributes(CString p_schema,CString p_tablename) const
{
//   p_tablename.MakeLower();
//   CString sql = "SELECT constrname\n"
//                 "      ,'' as indexname\n"
//                 "      ,'Y' as deferrable\n"
//                 "      ,'N' as initially_deferred\n"
//                 "  FROM systables      tab\n"
//                 "      ,sysconstraints con\n"
//                 " WHERE con.constrtype = 'P'\n"
//                 "   AND con.tabid      = tab.tabid\n"
//                 "   AND tab.tabname    = '" + p_tablename + "'";
//   return sql;
  return "";
}

CString
SQLInfoInformix::GetCATALOGPrimaryCreate(MPrimaryMap& p_primaries) const
{
  CString query("ALTER TABLE ");
  CString constraintName;

  for(auto& prim : p_primaries)
  {
    if(prim.m_columnPosition == 1)
    {
      if(!prim.m_schema.IsEmpty())
      {
        query += prim.m_schema + ".";
      }
      query += prim.m_table + "\n";
      query += "  ADD CONSTRAINT PRIMARY KEY (";
    }
    else
    {
      query += ",";
    }
    query += prim.m_columnName;
    constraintName = prim.m_constraintName;
  }
  query += ")\n";
  query += "      CONSTRAINT " + constraintName;

  return query;
}

CString
SQLInfoInformix::GetCATALOGPrimaryDrop(CString /*p_schema*/,CString p_tablename,CString p_constraintname) const
{
  CString sql("ALTER TABLE " + p_tablename + "\n"
              " DROP CONSTRAINT " + p_constraintname);
  return sql;
}

//////////////////////////////////////////////////////////////////////////
// ALL FOREIGN KEY FUNCTIONS

CString
SQLInfoInformix::GetCATALOGForeignExists(CString /*p_schema*/,CString p_tablename,CString p_constraintname) const
{
  p_tablename.MakeLower();
  p_constraintname.MakeLower();

  CString sql;
  sql.Format("SELECT COUNT(*)\n"
             "  FROM sysconstraints con\n"
             "      ,systables      tab\n"
             " WHERE tab.tabid      = con.tabid\n"
             "   AND con.constrtype = 'R'\n"
             "   AND tab.tablename  = '%s'\n"
             "   AND con.constrname = '%s'"
            ,p_tablename.GetString()
            ,p_constraintname.GetString());
  return sql;
}

// Get all attributes in order of MetaForeign for ALL FK constraints
CString
SQLInfoInformix::GetCATALOGForeignList(CString p_schema,CString p_tablename,int p_maxColumns /*=SQLINFO_MAX_COLUMNS*/) const
{
  return GetCATALOGForeignAttributes(p_schema,p_tablename,"",p_maxColumns);
}

// Get all attributes in order of MetaForeign for 1 FK constraint
CString
SQLInfoInformix::GetCATALOGForeignAttributes(CString p_schema
                                            ,CString p_tablename
                                            ,CString p_constraint
                                            ,bool    p_referenced /*= false*/
                                            ,int     p_maxColumns /*=SQLINFO_MAX_COLUMNS*/) const
{
  CString query;
  p_schema.MakeLower();
  p_tablename.MakeLower();
  p_constraint.MakeLower();

  for(int ind = 1;ind <= p_maxColumns; ++ind)
  {
    CString part;
    part.Format("SELECT trim(DBINFO('dbname')) AS primary_catalog_name\n"
                "      ,trim(pri.owner)        AS primary_schema_name\n"
                "      ,trim(pri.tabname)      AS primary_table_name\n"
                "      ,trim(DBINFO('dbname')) AS foreign_catalog_name\n"
                "      ,trim(tab.owner)        AS foreign_schema_name\n"
                "      ,trim(tab.tabname)      AS foreign_table_name\n"
                "      ,trim(pcn.constrname)   AS primary_key_constraint\n"
                "      ,trim(con.constrname)   AS foreign_key_constraint\n"
                "      ,%d               AS key_sequence\n"
                "      ,trim(pcl.colname)      AS primary_column_name\n"
                "      ,trim(col.colname)      AS foreign_column_name\n"
                "      ,CASE WHEN ref.updrule = 'R' THEN 1\n"
                "            WHEN ref.updrule = 'C' THEN 0\n"
                "            WHEN ref.updrule = 'N' THEN 2\n"
                "            WHEN ref.updrule = 'D' THEN 4\n"
                "            ELSE 0\n"
                "       END  AS update_rule\n"
                "      ,CASE WHEN ref.delrule = 'R' THEN 1\n"
                "            WHEN ref.delrule = 'C' THEN 0\n"
                "            WHEN ref.delrule = 'N' THEN 2\n"
                "            WHEN ref.delrule = 'D' THEN 4\n"
                "            ELSE 0\n"
                "       END  AS delete_rule\n"
                "      ,1    AS deferrable\n"
                "      ,CASE WHEN ref.matchtype = 'N' THEN 0\n"
                "            WHEN ref.matchtype = 'P' THEN 1\n"
                "            ELSE 0\n"
                "       END  AS match_option\n"
                "      ,0    AS initially_deferred\n"
                "      ,1    AS enabled\n"
                "  FROM sysconstraints con\n"
                "      ,systables      tab\n"
                "      ,syscolumns     col\n"
                "      ,sysreferences  ref\n"
                "      ,systables      pri\n"
                "      ,sysindexes     idx\n"
                "      ,sysconstraints pcn\n"
                "      ,sysindexes     pix\n"
                "      ,syscolumns     pcl\n"
                " WHERE tab.tabid      = con.tabid\n"
                "   AND con.constrid   = ref.constrid\n"
                "   AND ref.ptabid     = pri.tabid\n"
                "   AND con.idxname    = idx.idxname\n"
                "   AND col.tabid      = tab.tabid\n"
                "   AND col.colno      = idx.part%d\n"
                "   AND pcn.tabid      = pri.tabid\n"
                "   AND pix.idxname    = pcn.idxname\n"
                "   AND pcl.tabid      = pri.tabid\n"
                "   AND pcl.colno      = pix.part%d\n"
                "   and con.constrtype = 'R'\n"
                "   AND pcn.constrtype = 'P'\n"
               ,ind
               ,ind
               ,ind);
    if(!p_schema.IsEmpty())
    {
      if(p_referenced)
      {
        part += "   AND pri.owner      = '" + p_schema + "'\n";
      }
      else
      {
      part += "    AND tab.owner = '" + p_schema + "'\n";
    }
    }
    if(!p_tablename.IsEmpty())
    {
      if(p_referenced)
      {
        part += "   AND pri.tabname    = '" + p_tablename + "'\n";
      }
      else
      {
      part += "   AND tab.tabname    = '" + p_tablename + "'\n";
    }
    }
    if(!p_constraint.IsEmpty())
    {
      if(p_referenced)
      {
        part += "    AND pcn.constrname = '" + p_constraint + "'\n";
      }
      else
      {
      part += "    AND con.constrname = '" + p_constraint + "'\n";
    }
    }
    // Add to the query
    if(!query.IsEmpty())
    {
      query += "\nUNION ALL\n\n";
    }
    query += part;
  }

  // Add ordering upto column number
  query += " ORDER BY 1,2,3,4,5,6,7,8,9";

  return query;
}

CString
SQLInfoInformix::GetCATALOGForeignCreate(MForeignMap& p_foreigns) const
{
  // Get first record
  MetaForeign& foreign = p_foreigns.front();

  // Construct the correct tablename
  CString table(foreign.m_fkTableName);
  CString primary(foreign.m_pkTableName);
  if(!foreign.m_fkSchemaName.IsEmpty())
  {
    table = foreign.m_fkSchemaName + "." + table;
  }
  if(!foreign.m_pkSchemaName.IsEmpty())
  {
    primary = foreign.m_pkSchemaName + "." + primary;
  }

  // The base foreign key command
  CString query = "ALTER TABLE " + table + "\n"
                  "  ADD CONSTRAINT FOREIGN KEY (";

  // Add the foreign key columns
  bool extra = false;
  for(auto& key : p_foreigns)
  {
    if(extra) query += ",";
    query += key.m_fkColumnName;
    extra  = true;
  }

  // Add references primary table
  query += ")\n      REFERENCES " + primary + "(";

  // Add the primary key columns
  extra = false;
  for(auto& key : p_foreigns)
  {
    if(extra) query += ",";
    query += key.m_pkColumnName;
    extra  = true;
  }
  query += ")\n";
  query += "      CONSTRAINT " + foreign.m_foreignConstraint;

  // Add all relevant options
  switch(foreign.m_deferrable)
  {
    case SQL_INITIALLY_DEFERRED:  query += "\n      NOVALIDATE"; break;
    default:                      break;
  }
  switch(foreign.m_deleteRule)
  {
    case SQL_CASCADE: query += "\n      ON DELETE CASCADE"; break;
    default:          // In essence: ON DELETE RESTRICT, but that's already the default
                      break;
  }
  return query;
}

CString
SQLInfoInformix::GetCATALOGForeignAlter(MForeignMap& p_original, MForeignMap& p_requested) const
{
  // Make sure we have both
  if (p_original.empty() || p_requested.empty())
  {
    return "";
  }

  MetaForeign& original = p_original.front();
  MetaForeign& requested = p_requested.front();

  // Construct the correct tablename (NO schema)
  CString table(original.m_fkTableName);

  // The base foreign key command
  CString query = "ALTER TABLE " + table + "\n"
                  "  SET CONSTRAINTS " + original.m_foreignConstraint + "\n";

  // Add all relevant options
  if (original.m_initiallyDeferred != requested.m_initiallyDeferred)
  {
    query += requested.m_initiallyDeferred ? "DEFERRED" : "IMMEDIATE";
  }
  else if (original.m_enabled != requested.m_enabled)
  {
    query += requested.m_enabled ? "ENABLED" : "DISABLED";
  }
  return query;
}

CString
SQLInfoInformix::GetCATALOGForeignDrop(CString /*p_schema*/,CString p_tablename,CString p_constraintname) const
{
  CString sql("ALTER TABLE " + p_tablename + "\n"
              " DROP CONSTRAINT " + p_constraintname);
  return sql;
}

//////////////////////////////////////////////////////////////////////////
// ALL TRIGGER FUNCTIONS

CString
SQLInfoInformix::GetCATALOGTriggerExists(CString p_schema, CString p_tablename, CString p_triggername) const
{
  return "";
}

CString
SQLInfoInformix::GetCATALOGTriggerList(CString p_schema, CString p_tablename) const
{
  return GetCATALOGTriggerAttributes(p_schema,p_tablename,"");
}

CString
SQLInfoInformix::GetCATALOGTriggerAttributes(CString p_schema, CString p_tablename, CString p_triggername) const
{
  p_schema.MakeLower();
  p_tablename.MakeLower();
  p_triggername.MakeLower();
  CString sql;
  sql = "SELECT Trim(DBINFO('dbname')) AS trigger_catalog\n"
        "      ,Trim(tri.owner)        AS trigger_schema\n"
        "      ,trim(tab.tabname)      AS trigger_table\n"
        "      ,Trim(tri.trigname)     AS trigger_name\n"
        "      ,''    AS remarks\n"
        "      ,0     AS position\n"
        "      ,0     AS before\n"
        "      ,CASE tri.event WHEN 'I' THEN 1 ELSE 0 END AS trigger_insert\n"
        "      ,CASE tri.event WHEN 'U' THEN 1 ELSE 0 END AS trigger_update\n"
        "      ,CASE tri.event WHEN 'D' THEN 1 ELSE 0 END AS trigger_delete\n"
        "      ,0     AS trigger_select\n"
        "      ,0     AS trigger_session\n"
        "      ,0     AS trigger_transaction\n"
        "      ,0     AS trigger_rollback\n"
        "      ,'old as ' || tri.old || ' new as ' || tri.new  AS trigger_referencing\n"
        "      ,1     AS enabled\n"
        "      ,'<@>' AS source\n"
        "  FROM systriggers tri\n"
        "      ,systables   tab\n"
        " WHERE tab.tabid = tri.tabid\n";
  if(!p_schema.IsEmpty())
  {
    sql += "   AND tri.owner = '" + p_schema + "'\n";
  }
  if(!p_tablename.IsEmpty())
  {
    sql += "   AND tab.tabname = '";
    sql += p_tablename + "'\n";
  }
  if(!p_triggername.IsEmpty())
  {
    sql += "   AND tri.trigname ";
    sql += p_triggername.Find('%') >= 0 ? "LIKE '" : "= '";
    sql += p_triggername + "'\n";
  }
  sql += " ORDER BY 1,2,3,4";
  return sql;
}

CString
SQLInfoInformix::GetCATALOGTriggerCreate(MetaTrigger& /*p_trigger*/) const
{
  return "";
}

CString
SQLInfoInformix::GetCATALOGTriggerDrop(CString p_schema, CString p_tablename, CString p_triggername) const
{
  return "";
}

//////////////////////////////////////////////////////////////////////////
// ALL SEQUENCE FUNCTIONS

CString
SQLInfoInformix::GetCATALOGSequenceExists(CString p_schema, CString p_sequence) const
{
  p_schema.MakeLower();
  p_sequence.MakeLower();
  CString sql = "SELECT COUNT(*)\n"
                "  FROM syssequences seq\n"
                "      ,sysdomains   dom\n"
                " WHERE dom.id    = seq.id\n"
                "   AND dom.owner = '" + p_schema   + "'\n"
                "   AND dom.name  = '" + p_sequence + "'\n";
              //"   AND dom.type  = 3"; ??
  return sql;
}

CString
SQLInfoInformix::GetCATALOGSequenceList(CString p_schema,CString p_pattern) const
{
  return GetCATALOGSequenceAttributes(p_schema,p_pattern);
}

CString
SQLInfoInformix::GetCATALOGSequenceAttributes(CString p_schema, CString p_sequence) const
{
  p_schema.MakeLower();
  p_sequence.MakeLower();
  CString sql = "SELECT trim(DBINFO('dbname')) as catalog_name\n"
                "      ,trim(tab.owner)        as schema_name\n"
                "      ,trim(tab.tabname)      as sequence_name\n"
                "      ,seq.start_val as current_value\n"
                "      ,0             as minimal_value\n"
                "      ,seq.inc_val   as increment\n"
                "      ,seq.cache     as cache\n"
                "      ,seq.cycle     as cycle\n"
                "      ,seq.order     as ordering\n"
                "  FROM syssequences seq\n"
                "      ,systables    tab\n"
                " WHERE tab.tabid   = seq.tabid\n"
                "   AND tab.tabtype = 'Q'\n";
  if(!p_schema.IsEmpty())
  {
    sql += "   AND tab.owner = '" + p_schema + "'\n";
  }
  if(!p_sequence.IsEmpty())
  {
    sql += "   AND tab.tabname ";
    sql += p_sequence.Find('%') >= 0 ? "LIKE '" : "= '";
    sql += p_sequence + "'\n";
  }
  return sql;
}

CString
SQLInfoInformix::GetCATALOGSequenceCreate(MetaSequence& p_sequence) const
{
  CString sql("CREATE SEQUENCE ");

  if (!p_sequence.m_schemaName.IsEmpty())
  {
    sql += p_sequence.m_schemaName + ".";
  }
  sql += p_sequence.m_sequenceName;
  sql.AppendFormat("\n START WITH %-12.0f", p_sequence.m_currentValue);
  sql.AppendFormat("\n INCREMENT BY %d", p_sequence.m_increment);

  sql += p_sequence.m_cycle ? "\n CYCLE" : "\n NOCYCLE";
  sql += p_sequence.m_order ? "\n ORDER" : "\n NOORDER";
  if (p_sequence.m_cache > 0)
  {
    sql.AppendFormat("\n CACHE %d",p_sequence.m_cache);
  }
  else
  {
    sql += "\n NOCACHE";
  }
  return sql;
}

CString
SQLInfoInformix::GetCATALOGSequenceDrop(CString /*p_schema*/, CString p_sequence) const
{
  CString sql("DROP SEQUENCE " + p_sequence);
  return sql;
}

//////////////////////////////////////////////////////////////////////////
// ALL VIEW FUNCTIONS

CString 
SQLInfoInformix::GetCATALOGViewExists(CString /*p_schema*/,CString p_viewname) const
{
  p_viewname.MakeLower();

  CString sql = "SELECT count(*)\n"
                "  FROM sysviews\n"
                " WHERE viewname = '" + p_viewname + "'";
  return sql;
}

CString 
SQLInfoInformix::GetCATALOGViewList(CString p_schema,CString p_pattern) const
{
  return "";
}

CString 
SQLInfoInformix::GetCATALOGViewAttributes(CString p_schema,CString p_viewname) const
{
  return "";
}

CString 
SQLInfoInformix::GetCATALOGViewCreate(CString /*p_schema*/,CString p_viewname,CString p_contents) const
{
  return "CREATE VIEW " + p_viewname + "\n" + p_contents;
}

CString 
SQLInfoInformix::GetCATALOGViewRename(CString p_schema,CString p_viewname,CString p_newname)    const
{
  return "";
}

CString 
SQLInfoInformix::GetCATALOGViewDrop(CString /*p_schema*/,CString p_viewname,CString& p_precursor) const
{
  p_precursor.Empty();
  return "DROP VIEW " + p_viewname;
}

//////////////////////////////////////////////////////////////////////////
//
// SQL/PSM PERSISTENT STORED MODULES 
//         Also called SPL or PL/SQL
// o GetPSM<Object[s]><Function>
//   -Procedures / Functions
//   - Exists					GetPSMProcedureExists
//   - List					  GetPSMProcedureList
//   - Attributes
//   - Create
//   - Drop
//
// o PSM<Element>[End]
//   - PSM Declaration(first,variable,datatype[,precision[,scale]])
//   - PSM Assignment (variable,statement)
//   - PSM IF         (condition)
//   - PSM IFElse 
//   - PSM IFEnd
//   - PSM WHILE      (condition)
//   - PSM WHILEEnd
//   - PSM LOOP
//   - PSM LOOPEnd
//   - PSM BREAK
//   - PSM RETURN     ([statement])
//
// o CALL the FUNCTION/PROCEDURE
//
//////////////////////////////////////////////////////////////////////////

CString
SQLInfoInformix::GetPSMProcedureExists(CString /*p_schema*/, CString p_procedure) const
{
  p_procedure.MakeLower();
  return    "select count(*)\n"
            "  from sysprocedures\n"
            " where procname='" + p_procedure + "'";
}

CString
SQLInfoInformix::GetPSMProcedureList(CString p_schema) const
{
  return "";
}

CString
SQLInfoInformix::GetPSMProcedureAttributes(CString /*p_schema*/, CString p_procedure) const
{
  return "";
//   p_procedure.MakeLower();
//   CString sql = "SELECT sbody.data\n"
//                 "  FROM sysprocbody sbody\n"
//                 "      ,sysprocedures sproc\n"
//                 " WHERE sbody.procid   = sproc.procid\n"
//                 "   AND sproc.procname = '" + p_procedure + "'\n"
//                 "   AND datakey        = 'T'";
//   return sql;
}

CString
SQLInfoInformix::GetPSMProcedureSourcecode(CString p_schema, CString p_procedure) const
{
  p_schema.MakeLower();
  p_procedure.MakeLower();

  // TRIGGER PART
  CString sql;
  sql = "SELECT bod.datakey\n"
        "      ,bod.seqno\n"
        "      ,bod.data\n"
        "  FROM systrigbody bod\n"
        "      ,systriggers tri\n"
        " WHERE bod.trigid = tri.trigid\n"
        "   AND bod.datakey IN ('A','D')\n";
  if(!p_schema.IsEmpty())
  {
    sql += "   AND tri.owner = '" + p_schema + "'\n";
  }
  if(!p_procedure.IsEmpty())
  {
    sql += "   AND tri.trigname = '" + p_procedure + "'\n";
  }
  // UNION
  sql += "UNION ALL\n";
  // PROCEDURE PART
  sql += "  SELECT bod.datakey\n"
         "      ,bod.seqno\n"
         "      ,bod.data\n"
         "  FROM sysprocbody   bod\n"
         "      ,sysprocedures pro\n"
         " WHERE bod.procid   = pro.procid\n"
         "   AND bod.datakey  = 'T'\n";
  if(!p_schema.IsEmpty())
  {
    sql += "   AND pro.owner    = '" + p_schema + "'\n";
  }
  if(!p_procedure.IsEmpty())
  {
    sql += "   AND pro.procname = '" + p_procedure + "'\n";
  }
  // ORDERING
  sql += " ORDER BY datakey DESC\n"
         "         ,seqno";
  return sql;
}

CString
SQLInfoInformix::GetPSMProcedureCreate(MetaProcedure& /*p_procedure*/) const
{
  return "";
}

CString
SQLInfoInformix::GetPSMProcedureDrop(CString p_schema, CString p_procedure) const
{
  return "";
}

CString
SQLInfoInformix::GetPSMProcedureErrors(CString p_schema,CString p_procedure) const
{
  // Informix does not support procedure errors
  return "";
}

// And it's parameters
CString
SQLInfoInformix::GetPSMProcedureParameters(CString p_schema,CString p_procedure) const
{
  p_schema.MakeLower();
  p_procedure.MakeLower();
  CString sql;
  sql = "SELECT Trim(DBINFO('dbname')) AS procedure_catalog\n"
        "      ,Trim(pro.owner)        AS procedure_schema\n"
        "      ,Trim(pro.procname)     AS procedure_name\n"
        "      ,Trim(col.paramname)    AS parameter_name\n"
        "      ,paramattr              AS column_type\n"
        "      ,CASE paramtype\n"
        "            WHEN 0  THEN 1 -- char\n"
        "            WHEN 1  THEN 5 -- smallint\n"
        "            WHEN 2  THEN 4 -- integer\n"
        "            WHEN 3  THEN 8 -- float\n"
        "            WHEN 4  THEN 7 -- float\n"
        "            WHEN 5  THEN 2 -- decimal\n"
        "            WHEN 6  THEN 4 -- serial\n"
        "            WHEN 7  THEN 9 -- date\n"
        "            WHEN 8  THEN 2 -- money/decimal\n"
        "            WHEN 10 THEN 11 -- timestmap\n"
        "            WHEN 11 THEN -6 -- byte\n"
        "            WHEN 12 THEN -3 -- text\n"
        "            WHEN 14 THEN -89 -- interval\n"
        "            WHEN 15 THEN -9  -- nchar\n"
        "            WHEN 17 THEN -5  -- bigint\n"
        "            WHEN 18 THEN -5  -- bigint serial\n"
        "            WHEN 41 THEN -4  -- blob\n"
        "            WHEN 43 THEN -1  -- lvarchar\n"
        "            WHEN 45 THEN -7  -- bit/boolean\n"
        "            WHEN 52 THEN -5  -- bigint\n"
        "            WHEN 53 THEN -5  -- bigserial\n"
        "                    ELSE 0   -- unknown\n"
        "       END AS dataype\n"
        "      ,CASE paramtype\n"
        "            WHEN 0  THEN 'CHAR'\n"
        "            WHEN 1  THEN 'SMALLINT'\n"
        "            WHEN 2  THEN 'INTEGER'\n"
        "            WHEN 3  THEN 'DOUBLE'\n"
        "            WHEN 4  THEN 'FLOAT'\n"
        "            WHEN 5  THEN 'DECIMAL'\n"
        "            WHEN 6  THEN 'INTEGER'\n"
        "            WHEN 7  THEN 'DATE'\n"
        "            WHEN 8  THEN 'DECIMAL'\n"
        "            WHEN 10 THEN 'TIMESTAMP'\n"
        "            WHEN 11 THEN 'BYTE'\n"
        "            WHEN 12 THEN 'VARCHAR'\n"
        "            WHEN 14 THEN 'INTERVAL'\n"
        "            WHEN 15 THEN 'WCHAR'\n"
        "            WHEN 17 THEN 'BIGINT'\n"
        "            WHEN 18 THEN 'SERIAL'\n"
        "            WHEN 41 THEN 'LONGVARBINARY'\n"
        "            WHEN 43 THEN 'WVARCHAR'\n"
        "            WHEN 45 THEN 'BIT'\n"
        "            WHEN 52 THEN 'BIGINT'\n"
        "            WHEN 53 THEN 'BIGINT'\n"
        "                    ELSE 'UNKNOWN'\n"
        "       END AS type_name\n"
        "      ,CASE paramtype\n"
        "            WHEN 0  THEN paramlen  -- char\n"
        "            WHEN 1  THEN 5 -- smallint\n"
        "            WHEN 2  THEN 11 -- integer\n"
        "            WHEN 3  THEN 13 -- float\n"
        "            WHEN 4  THEN 21 -- float\n"
        "            WHEN 5  THEN 40 -- decimal\n"
        "            WHEN 6  THEN 11 -- serial\n"
        "            WHEN 7  THEN 10 -- date\n"
        "            WHEN 8  THEN 40 -- money/decimal\n"
        "            WHEN 10 THEN 22 -- timestmap\n"
        "            WHEN 11 THEN paramlen -- byte\n"
        "            WHEN 12 THEN paramlen -- text\n"
        "            WHEN 14 THEN 20 -- interval\n"
        "            WHEN 15 THEN paramlen  -- nchar\n"
        "            WHEN 17 THEN 22  -- bigint\n"
        "            WHEN 18 THEN 22  -- bigint serial\n"
        "            WHEN 41 THEN paramlen  -- blob\n"
        "            WHEN 43 THEN paramlen  -- lvarchar\n"
        "            WHEN 45 THEN 1  -- bit/boolean\n"
        "            WHEN 52 THEN 22  -- bigint\n"
        "            WHEN 53 THEN 22  -- bigserial\n"
        "                    ELSE 0   -- unknown\n"
        "       END AS column_size\n"
        "      ,CASE paramtype\n"
        "            WHEN 0  THEN paramlen -- char\n"
        "            WHEN 1  THEN 2 -- smallint\n"
        "            WHEN 2  THEN 4 -- integer\n"
        "            WHEN 3  THEN 8 -- double\n"
        "            WHEN 4  THEN 4 -- float\n"
        "            WHEN 5  THEN Trunc(paramlen/256) -- decimal\n"
        "            WHEN 6  THEN 4 -- serial\n"
        "            WHEN 7  THEN Trunc(paramlen/256) -- date\n"
        "            WHEN 8  THEN Trunc(paramlen/256) -- money/decimal\n"
        "            WHEN 10 THEN Trunc(paramlen/256) -- timestmap\n"
        "            WHEN 11 THEN paramlen     -- byte\n"
        "            WHEN 12 THEN paramlen     -- text\n"
        "            WHEN 14 THEN Trunc(paramlen/256) -- interval\n"
        "            WHEN 15 THEN paramlen  -- nchar\n"
        "            WHEN 17 THEN 8   -- bigint\n"
        "            WHEN 18 THEN 8   -- bigint serial\n"
        "            WHEN 41 THEN paramlen  -- blob\n"
        "            WHEN 43 THEN paramlen  -- lvarchar\n"
        "            WHEN 45 THEN 1   -- bit/boolean\n"
        "            WHEN 52 THEN 8   -- bigint\n"
        "            WHEN 53 THEN 8   -- bigserial\n"
        "                    ELSE 0   -- unknown\n"
        "       END  AS buffer_length\n"
        "      ,Mod(paramlen,16) AS decimal_digits\n"
        "      ,10       AS numRadix\n"
        "      ,1        AS nullable\n"
        "      ,''       AS remarks\n"
        "      ,''       AS defaults\n"
        "      ,Trunc(Mod(paramlen,256)/16)  AS datatype3\n"
        "      ,Mod(paramlen,16)             AS subtype3\n"
        "      ,paramlen AS octetlength\n"
        "      ,paramid  AS position\n"
        "      ,'YES'    AS isnullable\n"
        "  FROM sysproccolumns col\n"
        "      ,sysprocedures  pro\n"
        " WHERE pro.procid   = col.procid\n";
  if(!p_schema.IsEmpty())
  {
    sql += "   AND pro.owner    = '" + p_schema + "'\n";
  }
  if(!p_procedure.IsEmpty())
  {
    sql += "   AND pro.procname ";
    sql += p_procedure.Find('%') >= 0 ? "LIKE '" : "= '";
    sql += p_procedure + "'\n";
  }
  sql += " ORDER BY 1,2,3,18";
  return sql;
}

//////////////////////////////////////////////////////////////////////////
//
// ALL PSM LANGUAGE ELEMENTS
//
//////////////////////////////////////////////////////////////////////////

CString
SQLInfoInformix::GetPSMDeclaration(bool    /*p_first*/
                                  ,CString p_variable
                                  ,int     p_datatype
                                  ,int     p_precision /*= 0 */
                                  ,int     p_scale     /*= 0 */
                                  ,CString p_default   /*= ""*/
                                  ,CString /*p_domain    = ""*/
                                  ,CString p_asColumn  /*= ""*/) const
{
  CString line;
  line.Format("DEFINE %s ",p_variable.GetString());

  if(p_datatype)
  {
    // Getting type info and name
    TypeInfo* info = GetTypeInfo(p_datatype);
    line += info->m_type_name;

    if(p_precision > 0)
    {
      line.AppendFormat("(%d",p_precision);
      if(p_scale > 0)
      {
        line.AppendFormat("%d",p_scale);
      }
      line += ")";
    }

    if(!p_default.IsEmpty())
    {
      line += " DEFAULT " + p_default;
    }
  }
  else if(!p_asColumn)
  {
    line += " LIKE " + p_asColumn;
  }
  line += ";\n";
  return line;
}

CString
SQLInfoInformix::GetPSMAssignment(CString p_variable,CString p_statement /*=""*/) const
{
  CString line("LET ");
  line += p_variable;
  line += " = ";
  if(!p_statement.IsEmpty())
  {
    line += p_statement;
    line += ";";
  }
  return line;
}

CString
SQLInfoInformix::GetPSMIF(CString p_condition) const
{
  return "IF (" + p_condition + ") THEN\n";
}

CString
SQLInfoInformix::GetPSMIFElse() const
{
  return "ELSE\n";
}

CString
SQLInfoInformix::GetPSMIFEnd() const
{
  return "END IF;\n";
}

CString
SQLInfoInformix::GetPSMWhile(CString p_condition) const
{
  return "WHILE (" + p_condition + ")\n";
}

CString
SQLInfoInformix::GetPSMWhileEnd() const
{
  return "END WHILE;\n";
}

CString
SQLInfoInformix::GetPSMLOOP() const
{
  return "LOOP\n";
}

CString
SQLInfoInformix::GetPSMLOOPEnd() const
{
  return "END LOOP\n";
}

CString
SQLInfoInformix::GetPSMBREAK() const
{
  return "EXIT\n"; // [FOR][LOOP][WHILE][FOREACH]
}

CString
SQLInfoInformix::GetPSMRETURN(CString p_statement /*= ""*/) const
{
  CString line("RETURN");
  if(!p_statement.IsEmpty())
  {
    line += " " + p_statement;
  }
  line += ";\n";
  return line;
}

CString
SQLInfoInformix::GetPSMExecute(CString p_procedure,MParameterMap& p_parameters) const
{
  // EXECUTE PROCEDURE name[(:param[,:param �])] [RETURNING_VALUES:param[,:param �]];
  CString line;
  line.Format("EXECUTE PROCEDURE %s (",p_procedure.GetString());
  bool doReturning = false;
  bool doMore = false;

  for(auto& param : p_parameters)
  {
    // Extra ,
    if(doMore) line += ",";
    doMore = true;

    // Append input and in/out parameters
    if(param.m_columnType == P_SQL_PARAM_INPUT || param.m_columnType == P_SQL_PARAM_INPUT_OUTPUT)
    {
      line += param.m_parameter;
    }
    // See if we must do 'returning' clause
    if(param.m_columnType == P_SQL_PARAM_OUTPUT || param.m_columnType == P_SQL_PARAM_INPUT_OUTPUT)
    {
      doReturning = true;
    }
  }
  line += ")";

  // Do the returning clause
  if(doReturning)
  {
    line += " INTO ";
    doMore = false;
    for(auto& param : p_parameters)
    {
      // Extra ,
      if(doMore) line += ",";
      doMore = true;

      if(param.m_columnType == P_SQL_PARAM_OUTPUT || param.m_columnType == P_SQL_PARAM_INPUT_OUTPUT)
      {
        line += param.m_parameter;
      }
    }
  }
  line += ";\n";
  return line;
}

// The CURSOR
CString
SQLInfoInformix::GetPSMCursorDeclaration(CString p_cursorname,CString p_select) const
{
  return "FOREACH " + p_cursorname + " FOR " + p_select + "\n";
}

CString
SQLInfoInformix::GetPSMCursorFetch(CString /*p_cursorname*/,std::vector<CString>& /*p_columnnames*/,std::vector<CString>& /*p_variablenames*/) const
{
  return "";
}

// END FOREACH; !!


//////////////////////////////////////////////////////////////////////////
// PSM Exceptions

CString
SQLInfoInformix::GetPSMExceptionCatchNoData() const
{
  return "ON EXCEPTION (100)\n";
}

CString
SQLInfoInformix::GetPSMExceptionCatch(CString p_sqlState) const
{
  return "ON EXCEPTION (" + p_sqlState + ")\n";
}

CString
SQLInfoInformix::GetPSMExceptionRaise(CString p_sqlState) const
{
  return "RAISE EXCEPTION " + p_sqlState;
  // "[,isam-error [,'error text']]
}

// END EXCEPTION [WITH RESUME];

//////////////////////////////////////////////////////////////////////////
//
// SESSIONS
// - Sessions (No create and drop)
//   - GetSessionMyself
//   - GetSessionExists
//   - GetSessionList
//   - GetSessionAttributes
// - Transactions
//   - GetSessionDeferredConstraints
//   - GetSessionImmediateConstraints
//
//////////////////////////////////////////////////////////////////////////

CString
SQLInfoInformix::GetSESSIONMyself() const
{
  CString query = "SELECT sid\n"
                  "      ,user\n"
                  "      ,tty\n"
                  "      ,current_timestamp\n"  // timestamp
                  "      ,''\n"                 // remote address
                  "      ,''\n"                 // remote process name
                  "      ,''\n"                 // remote process ID
                  "  FROM sysmaster:syssessions\n"
                  " WHERE sid =\n"
                  "     ( SELECT dbinfo('sessionid')\n"
                  "         FROM systables\n"
                  "        WHERE tabid=1)";
  return query;
}

CString
SQLInfoInformix::GetSESSIONExists(CString p_sessionID) const
{
  return "SELECT COUNT(*)\n"
         "  FROM sysmaster:syssessions\n"
         " WHERE sid = " + p_sessionID;
}

CString
SQLInfoInformix::GetSESSIONList() const
{
  return GetSESSIONAttributes("");
}

CString
SQLInfoInformix::GetSESSIONAttributes(CString p_sessionID) const
{
  CString sql = "SELECT sid\n"
                "      ,user\n"
                "      ,tty\n"
                "      ,current_timestamp\n"  // timestamp
                "      ,''\n"                 // remote address
                "      ,''\n"                 // remote process name
                "      ,''\n"                 // remote process ID
                "  FROM sysmaster:syssessions";
  if (!p_sessionID.IsEmpty())
  {
    sql += "\n WHERE sid = " + p_sessionID;
  }
  return sql;
}

//////////////////////////////////////////////////////////////////////////
// Transactions

CString
SQLInfoInformix::GetSESSIONConstraintsDeferred() const
{
  return "SET CONSTRAINTS ALL DEFERRED";
}

CString
SQLInfoInformix::GetSESSIONConstraintsImmediate() const
{
  return "SET CONSTRAINTS ALL IMMEDIATE";
}

//////////////////////////////////////////////////////////////////////////
//
// Call FUNCTION/PROCEDURE from within program
// As a RDBMS dependent extension of "DoSQLCall" of the SQLQuery object
//
//////////////////////////////////////////////////////////////////////////

// Calling a stored function or procedure if the RDBMS does not support ODBC call escapes
SQLVariant*
SQLInfoInformix::DoSQLCall(SQLQuery* /*p_query*/,CString& /*p_schema*/,CString& /*p_procedure*/)
{
  return nullptr;
}

// End of namespace
}
