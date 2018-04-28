////////////////////////////////////////////////////////////////////////
//
// File: SQLDataSet.h
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
 ~SQLDataSet();

  // Query done and records gotten?
  bool IsOpen();
  // Perform query: Do SetQuery first
  bool Open(bool p_stopIfNoColumns = false);
  // Append (read extra) into the dataset
  bool Append();
  // Remove resultset
  void Close();

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
  void         SetDatabase(SQLDatabase* p_database);
  // Set one or more columns to select
  void         SetSelection(CString p_selection);
  // Set a new full query (Superseeds 'SetSelection'!!)
  void         SetQuery(CString& p_query);
  // Set primary table (for updates)
  void         SetPrimaryTable(CString p_schema,CString p_tableName);
  // Set primary key column name (for updates)
  void         SetPrimaryKeyColumn(CString p_name);
  void         SetPrimaryKeyColumn(WordList& p_list);
  // Set searchable column
  // void         SetSearchableColumn(CString p_name);
  // Set parameter for a query
  void         SetParameter(SQLParameter p_parameter);
  void         SetParameter(CString p_naam,SQLVariant p_waarde);
  // Set filters for a query
  void         SetFilters(SQLFilterSet& p_filters);
  // Set the status to modified/saved
  void         SetStatus(int m_add,int m_delete = 0);
  // Set a field value in the current record
  void         SetField(int p_num,      SQLVariant* p_value,int p_mutationID = 0);
  // Set a field value in the current record
  void         SetField(CString& p_name,SQLVariant* p_value,int p_mutationID = 0);

  // GETTERS

  // Get the name of the dataset
  CString      GetName();
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

  // XML Saving and loading
  bool         XMLSave(CString p_filename,CString p_name,XMLEncoding p_encoding = XMLEncoding::ENC_UTF8);
  bool         XMLLoad(CString p_filename);
  void         XMLSave(XMLMessage* p_msg,XMLElement* p_dataset);
  void         XMLLoad(XMLMessage* p_msg,XMLElement* p_dataset);
private:
  // Set parameters in the query
  CString      ParseQuery();
  // Parse the selection
  CString      ParseSelection(SQLQuery& p_query);
  // Parse the fitlers
  CString      ParseFilters();
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
  // Forget about a record in the recordset
  bool         ForgetRecord(SQLRecord* p_record,bool p_force);
  void         ForgetPrimaryObject(SQLRecord* p_record);

  // WRITEBACK OPERATIONS

  void         Deletes(int p_mutationID);
  void         Updates(int p_mutationID);
  void         Inserts(int p_mutationID);
  void         Reduce (int p_mutationID);

  CString      GetSQLDelete  (SQLQuery* p_query,SQLRecord* p_record);
  CString      GetSQLUpdate  (SQLQuery* p_query,SQLRecord* p_record);
  CString      GetSQLInsert  (SQLQuery* p_query,SQLRecord* p_record);
  CString      GetWhereClause(SQLQuery* p_query,SQLRecord* p_record,int& p_parameter);

  // Private data of the dataset

  CString      m_name;
  SQLDatabase* m_database;
  bool         m_open;
  CString      m_query;
  CString      m_selection;
  CString      m_primarySchema;
  CString      m_primaryTableName;
  ParameterSet m_parameters;
  SQLFilterSet m_filters;
  NamenMap     m_primaryKey;
protected:
  int          m_status;
  int          m_current;
  NamenMap     m_names;
  TypenMap     m_types;
  RecordSet    m_records;
  ObjectMap    m_objects;
};

inline void 
SQLDataSet::SetDatabase(SQLDatabase* p_database)
{
  m_database = p_database;
}

inline bool
SQLDataSet::IsOpen()
{
  return m_open;
}

inline void 
SQLDataSet::SetQuery(CString& p_query)
{
  m_selection.Empty();
  m_query = p_query;
}

inline void
SQLDataSet::SetSelection(CString p_selection)
{
  m_query.Empty();
  m_selection = p_selection;
}

inline void
SQLDataSet::SetPrimaryTable(CString p_schema,CString p_tableName)
{
  m_primarySchema    = p_schema;
  m_primaryTableName = p_tableName;
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
AggregateInfo::Init()
{
  m_sum  = 0.0;
  m_max  = 0.0;
  m_min  = 0.0;
  m_mean = 0.0;
}

// End of namespace
}