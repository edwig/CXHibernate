//////////////////////////////////////////////////////////////////////////
// cfg2ddl.cpp : Generate SQL scripts for CX-Hibernate
//
#include "stdafx.h"
#include <CXHibernate.h>
#include <CXSession.h>
#include <CXClass.h>
#include <DDLCreateTable.h>
#include <GetExePath.h>
#include <map>

using SQLComponents::DDLCreateTable;

static CString g_configFile;
static CString g_datasource;
static CString g_datauser;
static CString g_password;

// TESTING
// Standalone: /datasource:hibtest /user:sysdba /password:altijd
// One_table : /config:onetable.cfg.xml /datasource:hibtest /user:sysdba /password:altijd
// Sub_table : /config:subtable.cfg.xml /datasource:hibtest /user:sysdba /password:altijd

void WriteDDLHeader(FILE* p_file)
{
  CString line("------------------------------------------------------------");

  fprintf(p_file,"%s\n",line.GetString());
  fprintf(p_file,"--\n");
  fprintf(p_file,"-- SQL Script for CX-Hibernate: %s\n",g_configFile.GetString());
  fprintf(p_file,"--\n");
  fprintf(p_file,"%s\n",line.GetString());
  fprintf(p_file,"\n");
}

void WriteDDLFooter(FILE* p_file)
{
  fprintf(p_file,"-- *** End of the SQL script ***\n");
}

void GenerateTable(FILE* p_file,CXSession* p_session,CXClass* p_class)
{
  // Get info about flat table
  SQLInfoDB* info = p_session->GetDatabase()->GetSQLInfoDB();
  DDLCreateTable create(info);
  CXTable* table = p_class->GetTable();

  fprintf(p_file,"-- Table: %s\n",table->GetFullQualifiedTableName().GetString());

  MTableMap tabinfo;
  tabinfo.push_back(table->GetTableInfo());

  create.SetTableInfoTable(tabinfo);
  create.SetTableInfoColumns(table->GetColumnInfo());
  create.SetTableInfoIndices(table->GetIndicesInfo());
  create.SetTableInfoPrimary(table->GetPrimaryInfo());
  create.SetTableInfoForeign(table->GetForeignInfo());
  create.SetTableInfoPrivilege(table->GetPrivilegesInfo());

  DDLS ddls = create.GetTableStatements(table->GetTableName());
  for(auto& ddl : ddls)
  {
    ddl = ddl.TrimRight();
    fprintf(p_file,"\n%s;\n",ddl.GetString());
  }
  fprintf(p_file,"\n");
}

void WriteDefinedBy(FILE* p_file,CXClass* p_class)
{
  CXClass* root = p_class->GetRootClass();
  fprintf(p_file,"-- Is defined by class: %s, table: %s\n"
                ,root->GetName().GetString()
                ,root->GetTable()->GetFullQualifiedTableName().GetString());
}

void WriteClassDDL(FILE* p_file,CXSession* p_session,CXClass* p_class)
{
  fprintf(p_file,"-- Class %s\n",p_class->GetName().GetString());

  // Depending on the strategy we generate different table sets
  switch(hibernate.GetStrategy())
  {
    case MapStrategy::Strategy_sub_table:   // Fall through
    case MapStrategy::Strategy_standalone:  GenerateTable(p_file,p_session,p_class);
                                            break;
    case MapStrategy::Strategy_one_table:   if(p_class->GetIsRootClass())
                                            {
                                              GenerateTable(p_file,p_session,p_class);
                                            }
                                            else
                                            {
                                              WriteDefinedBy(p_file,p_class);
                                            }
                                            break;
  }
}

void WriteHibernateDDL(FILE* p_file,CXSession* p_session)
{
  ClassMap& classes = p_session->GetClasses();

  WriteDDLHeader(p_file);

  for(auto& iter : classes)
  {
    WriteClassDDL(p_file,p_session,iter.second);
  }

  WriteDDLFooter(p_file);
}

CString GetParameters(int argc, char** argv)
{
  int index = 1;
  CString filename;
  bool error = false;

  while (index < argc)
  {
    CString param(argv[index]);

    if (param.GetAt(0) == '/')
    {
      if (param.Left(8).CompareNoCase("/config:") == 0)
      {
        g_configFile = param.Mid(8);
      }
      else if(param.Left(12).CompareNoCase("/datasource:") == 0)
      {
        g_datasource = param.Mid(12);
      }
      else if(param.Left(6).CompareNoCase("/user:") == 0)
      {
        g_datauser = param.Mid(6);
      }
      else if(param.Left(10).CompareNoCase("/password:") == 0)
      {
        g_password = param.Mid(10);
      }
      else
      {
        error = true;
      }
    }
    else
    {
      filename = param;
    }
    ++index;
  }

  // In case of an error: print usage of the tool
  if (error)
  {
    printf("USAGE: cfg2ddl [/config:hibernate.cfg.xml] /datasource:<odbc> /user:<user> /password:<secret> [outputfile.sql]\n");
    printf("Where:\n");
    printf("/config         The hibernate configuration file (OPTIONAL)\n");
    printf("/datasource     The name of the ODBC datasource\n");
    printf("/user           The connection database DBA user\n");
    printf("/password       The password of the database user\n");
    printf("outputfile.sql  The name of the SQL script to write (OPTIONAL)\n");
    printf("\n");
    printf("if 'config' is not given, hibernate.cfg.xml is used.\n");
    printf("if 'outputfile.sql' is not given, stdout is written.\n");
    exit(-3);
  }
  return filename;
}

CString ShowConfigFile(CString p_file)
{
  if(p_file.IsEmpty())
  {
    p_file = HIBERNATE_CONFIG_FILE;
  }
  if(p_file.GetAt(1) != ':' && p_file.GetAt(0) != '\\')
  {
    p_file = GetExePath() + p_file;
  }
  return p_file;
}


int main(int argc, char** argv)
{
  printf("CFG2DDL Hibernate.cfg.xml to *.sql file converter\n");
  printf("Copyright (c) ir. W.E. Huisman\n");
  printf("Version: %s of %s\n", HIBERNATE_VERSION, HIBERNATE_DATE);
  printf("\n");

  CString filename = GetParameters(argc, argv);

  printf("Creating session\n");
  printf("Reading configuration file: %s\n",ShowConfigFile(g_configFile).GetString());

  // Tools: set incomplete mode!!
  hibernate.SetIncomplete(true);

  try
  {
    // Request a session
    CXSession* session = hibernate.CreateSession("",g_configFile);
    if(session)
    {
      // Preparing a database connection
      session->SetDatabaseConnection(g_datasource,g_datauser,g_password);

      // Preparing an output file
      FILE* file = nullptr;
      errno_t error = 0;
      
      if(filename.IsEmpty())
      {
        file = stdout;
      }
      else
      {
        fopen_s(&file, filename, "w");
      }
      if(!error)
      {
        WriteHibernateDDL(file,session);
        fclose(file);
      }
      else
      {
        printf("ERROR: Cannot open file: %s\n",filename.GetString());
      }
      session->CloseSession();
    }
    else
    {
      printf("ERROR: Cannot create a hibernate session\n");
      printf("Have you defined a correct 'hibernate.cfg.xml' file?\n");
    }
  }
  catch (StdException& ex)
  {
    printf("ERROR: %s\n", ex.GetErrorMessage().GetString());
  }
  return 0;
}
