////////////////////////////////////////////////////////////////////////
//
// File: SQLDataSet.h
//
// Copyright (c) 1998-2020 ir. W.E. Huisman
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
#pragma once
#include "SQLDatabase.h"
#include "SQLRecord.h"
#include "SQLVariant.h"
#include "SQLFilter.h"
#include "XMLMessage.h"
#include <vector>

namespace SQLComponents
{

// SET status is a OR-ed bitmap status
#define SQL_Empty       0x00
#define SQL_Selections  0x01
#define SQL_Updates     0x02
#define SQL_Deletions   0x04
#define SQL_Insertions  0x08

// Names for saving the dataset to XML structures
#define DATASET_NAME      0
#define DATASET_STRUCTURE 1
#define DATASET_FIELD     2
#define DATASET_RECORD    3
#define DATASET_RECORDS   4
#define DATASET_ID        5
#define DATASET_TYPE      6
#define DATASET_TYPENAME  7
#define NUM_DATASET_NAMES 8

// Names for saving datasets to XML in various languages
extern const char* dataset_names[LN_NUMLANG][NUM_DATASET_NAMES];

// Parameter for the query
typedef struct _sql_parameter
{
  CString    m_name;
  SQLVariant m_value;
}
SQLParameter;

class AggregateInfo
{
public:
  AggregateInfo() { Init(); };
  void    Init();

  double  m_sum;
  double  m_max;
  double  m_min;
  double  m_mean;
};

class SQLDatabase;
class SQLQuery;
typedef std::vector<SQLRecord*>   RecordSet;
typedef std::vector<SQLVariant*>  VariantSet;
typedef std::vector<SQLParameter> ParameterSet;
typedef std::vector<CString>      NamenMap;
typedef std::vector<int>          TypenMap;
typedef std::map<CString,int>     ObjectMap;
typedef std::list<CString>        WordList;

class SQLDataSet
{
public:
  SQLDataSet();
  SQLDataSet(CString p_name,SQLDatabase* p_database = NULL);
  virtual ~SQLDataSet();

  // Perform query: Do SetQuery first
  virtual bool Open(bool p_stopIfNoColumns = false);
  // Append (read extra) into the dataset
  virtual bool Append();
  // Remove result set
  virtual void Close();
  // Query done and records gotten?
  bool IsOpen();

  // Navigate in the records
  int  First();
  int  Next();
  int  Prev();
  int  Last();
  int  Goto(int p_record);
  bool IsFirst();
  bool IsLast();
  int  Current();

  // OPERATIONS

  // Insert new record (Manually)
  SQLRecord*   InsertRecord();
  // Insert new field in new record (manually)
  int          InsertField(CString& p_name,SQLVariant* p_value);
  // Calculate aggregate functions
  int          Aggregate(int p_num,AggregateInfo& p_info);
  // Cancel the mutations of this mutation ID
  void         CancelMutation(int p_mutationID);
  // Insert / Update / delete records from the database
  bool         Synchronize(int p_mutationID = 0);
  // In case synchronize doesn't work, ask for mixed mutations
  int          AllMixedMutations(MutationIDS& p_list,int p_mutationID);
  // Find the object record of a primary key
  int          FindObjectRecNum(int p_primary);           // If your primary is an INTEGER     (Fast!!)
  int          FindObjectRecNum(VariantSet& p_primary);   // If your primary is a compound key (Slower)
  SQLRecord*   FindObjectRecord(int p_primary);           // If your primary is an INTEGER     (Fast!!)
  SQLRecord*   FindObjectRecord(VariantSet& p_primary);   // If your primary is a compound key (Slower)
  SQLRecord*   FindObjectFilter(SQLFilterSet& p_filters,bool p_primary = false);  // Fast & slow
  RecordSet*   FindRecordSet   (SQLFilterSet& p_filters);                         // Always slow
  // Forget the records
  bool         Forget(bool p_force = false);
  // Forget just one record AND reset current cursor to first position
  bool         ForgetObject(int p_primary,        bool p_force = false); // Forget 1 record and primary is an INTEGER     (Fast!!)
  bool         ForgetObject(VariantSet& p_primary,bool p_force = false); // Forget 1 record and primary is a compound key (Slower)

  // SETTERS

  // Set database connection
  virtual void SetDatabase(SQLDatabase* p_database);
  // Set SELECT one or more columns to select
  virtual void SetSelection(CString p_selection);
  // Set FROM  primary table (for updates)
  virtual void SetPrimaryTable(CString p_schema, CString p_tableName, CString p_alias = "");
  // Set WHERE condition by hand
  virtual void SetWhereCondition(CString p_condition);
  // Set WHERE filters for a query
  virtual void SetFilters(SQLFilterSet* p_filters);
  // Set GROUP BY 
  virtual void SetGroupBy(CString p_groupby);
  // Set ORDER BY
  virtual void SetOrderBy(CString p_orderby);
  // Set HAVING
  virtual void SetHavings(SQLFilterSet* p_havings);
  // Set SQL-QUERY:  a new full query (Supersedes SetSelection, -Where, -groupby, -orderby and -having)
  virtual void SetQuery(CString& p_query);
  // Set primary key column name (for updates)
  virtual void SetPrimaryKeyColumn(CString p_name);
  virtual void SetPrimaryKeyColumn(WordList& p_list);

  // Setting the sequence/generator name to something different than "<tablename>_seq"
  void         SetSequenceName(CString p_sequence);
  // Set parameter for a query
  void         SetParameter(SQLParameter p_parameter);
  void         SetParameter(CString p_naam,SQLVariant p_waarde);
  // Set maximum number of milliseconds alloted to the Open query
  void         SetQueryTime(int p_milliseconds);
  // Set top <n> records selection
  void         SetTopNRecords(int p_top,int p_skip = 0);
  // Set columns that can be updated
  void         SetUpdateColumns(WordList p_list);
  // Set the status to modified/saved
  void         SetStatus(int m_add,int m_delete = 0);
  // Set a field value in the current record
  void         SetField(int p_num,      SQLVariant* p_value,int p_mutationID = 0);
  // Set a field value in the current record
  void         SetField(CString& p_name,SQLVariant* p_value,int p_mutationID = 0);

  // GETTERS

  // Get the name of the dataset
  CString      GetName();
  // Getting the database used
  SQLDatabase* GetDatabase();
  // Get the number of records
  int          GetNumberOfRecords();
  // Get number of fields
  int          GetNumberOfFields();
  // Get the current record (number)
  int          GetCurrentRecord();
  // Get a specific record
  SQLRecord*   GetRecord(int p_recnum);
  // Gets the status of records of the dataset
  int          GetStatus();
  // Get a field name
  CString      GetFieldName(int p_num);
  // Get datatype of a field
  int          GetFieldType(int p_num);
  // Get a field number
  int          GetFieldNumber(CString p_name);
  // Get a field from the current record
  SQLVariant*  GetCurrentField(int p_num);
  // Getting info about the primary key
  CString      GetPrimarySchema();
  CString      GetPrimaryTableName();
  // Getting the sequence name
  CString      GetSequenceName();
  // Getting the query settings
  CString      GetSelection();
  CString      GetWhereCondition();
  CString      GetGroupBy();
  CString      GetOrderBy();
  SQLFilterSet* GetHavings();
  void         AddGroupby(const CString& p_property);

  // XML Saving and loading
  bool         XMLSave(CString p_filename,CString p_name,XMLEncoding p_encoding = XMLEncoding::ENC_UTF8);
  bool         XMLLoad(CString p_filename);
  void         XMLSave(XMLMessage* p_msg,XMLElement* p_dataset);
  void         XMLLoad(XMLMessage* p_msg,XMLElement* p_dataset);

protected:
  // Set parameters in the query
  virtual CString ParseQuery();
  // Construct the selection SQL for opening the dataset
  virtual CString GetSelectionSQL(SQLQuery& p_qry);
  // Parse the selection
  virtual CString ParseSelection(SQLQuery& p_query);
  // Parse the filters
  virtual CString ParseFilters(SQLQuery& p_query,CString p_sql);

  // Get the variant of a parameter
  SQLVariant*  GetParameter(CString& p_name);
  // Get all the columns of the record
  void         ReadNames(SQLQuery& qr);
  // Get all the datatypes of the columns
  void         ReadTypes(SQLQuery& qr);
  // Check that all names are the same
  void         CheckNames(SQLQuery& p_query);
    // Check that all the types are the same
  void         CheckTypes(SQLQuery& p_query);
  // Getting the primary key for a table
  bool         GetPrimaryKeyInfo();
  // Check that all primary key columns are in the dataset
  bool         CheckPrimaryKeyColumns();
  // Read in a record from a SQLQuery
  bool         ReadRecordFromQuery(SQLQuery& p_query,bool p_modifiable,bool p_append = false);
  // Make a primary key record
  CString      MakePrimaryKey(SQLRecord*  p_record);
  CString      MakePrimaryKey(VariantSet& p_primary);
  // Forget about a record
  bool         ForgetRecord(SQLRecord* p_record,bool p_force);
  void         ForgetPrimaryObject(SQLRecord* p_record);
  // Init the high performance counter
  void         InitCounter();
  ULONG64      GetCounter();

  // WRITEBACK OPERATIONS

  void         Deletes(int p_mutationID);
  void         Updates(int p_mutationID);
  void         Inserts(int p_mutationID);
  void         Reduce (int p_mutationID);

  CString      GetSQLDelete  (SQLQuery* p_query,SQLRecord* p_record);
  CString      GetSQLUpdate  (SQLQuery* p_query,SQLRecord* p_record);
  CString      GetSQLInsert  (SQLQuery* p_query,SQLRecord* p_record,CString& p_serial);
  CString      GetWhereClause(SQLQuery* p_query,SQLRecord* p_record,int& p_parameter);

  // Base class data of the dataset

  CString      m_name;
  SQLDatabase* m_database;
  bool         m_open { false };
  // The query to run
  CString      m_query;
  CString      m_selection;
  CString      m_whereCondition;
  CString      m_orderby;
  CString      m_groupby;
  // Parts of the query
  CString      m_primarySchema;
  CString      m_primaryTableName;
  CString      m_primaryAlias;
  CString      m_sequenceName;
  ParameterSet m_parameters;
  NamenMap     m_primaryKey;
  WordList     m_updateColumns;
  int          m_topRecords  { 0 };
  int          m_skipRecords { 0 };
  // Filter sets
  SQLFilterSet* m_filters { nullptr };
  SQLFilterSet* m_havings { nullptr };

  // Records and objects
  int          m_status      { SQL_Empty };
  int          m_current     { -1 };
  NamenMap     m_names;
  TypenMap     m_types;
  RecordSet    m_records;
  ObjectMap    m_objects;
  // Maximum query timing
  int          m_queryTime { 0 };
  ULONG64      m_frequency { 0 };
};

inline void 
SQLDataSet::SetDatabase(SQLDatabase* p_database)
{
  m_database = p_database;
}

inline SQLDatabase*
SQLDataSet::GetDatabase()
{
  return m_database;
}

inline bool
SQLDataSet::IsOpen()
{
  return m_open;
}

inline void
SQLDataSet::SetPrimaryTable(CString p_schema,CString p_tableName,CString p_alias /*= ""*/)
{
  m_primarySchema    = p_schema;
  m_primaryTableName = p_tableName;
  m_primaryAlias     = p_alias;
}

inline void
SQLDataSet::SetSequenceName(CString p_sequence)
{
  m_sequenceName = p_sequence;
}

inline int
SQLDataSet::GetNumberOfRecords()
{
  return (int)m_records.size();
}

inline int
SQLDataSet::GetNumberOfFields()
{
  return (int)m_names.size();
}

inline int
SQLDataSet::GetCurrentRecord()
{
  return m_current;
}

inline int
SQLDataSet::GetStatus()
{
  return m_status;
}

inline CString    
SQLDataSet::GetName()
{
  return m_name;
}

inline CString
SQLDataSet::GetPrimarySchema()
{
  return m_primarySchema;
}

inline CString
SQLDataSet::GetPrimaryTableName()
{
  return m_primaryTableName;
}

inline void
SQLDataSet::SetPrimaryKeyColumn(CString p_name)
{
  m_primaryKey.push_back(p_name);
}

inline int
SQLDataSet::Current()
{
  return m_current;
}

inline void
SQLDataSet::SetUpdateColumns(WordList p_list)
{
  m_updateColumns = p_list;
}

inline CString
SQLDataSet::GetSelection()
{
  return m_selection;
}

inline CString
SQLDataSet::GetWhereCondition()
{
  return m_whereCondition;
}

inline CString
SQLDataSet::GetGroupBy()
{
  return m_groupby;
}

inline void 
SQLDataSet::AddGroupby(const CString& p_property)
{
  if (!m_groupby.IsEmpty())
  {
    m_groupby += ", ";
  }
  m_groupby += p_property;
}

inline CString
SQLDataSet::GetOrderBy()
{
  return m_orderby;
}

inline SQLFilterSet*
SQLDataSet::GetHavings()
{
  return m_havings;
}

inline void
AggregateInfo::Init()
{
  m_sum  = 0.0;
  m_max  = 0.0;
  m_min  = 0.0;
  m_mean = 0.0;
}

// End of namespace
}