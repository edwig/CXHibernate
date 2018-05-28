// cfg2cxh.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "CXHibernate.h"
#include "CXClass.h"

// All Marlin XML type macros and there macro names
// This translation is **NOT** in the Marlin library

typedef struct _typeMacro
{
  int         m_value;
  const char* m_name;
  const char* m_initialization;
}
TypeMacro;

TypeMacro tmacro[] =
{
   { SQL_C_CHAR,                      "XDT_String"            ,""       }
  ,{ SQL_C_LONG,                      "XDT_Integer"           ,"0"      }
  ,{ SQL_C_SLONG,                     "XDT_Integer"           ,"0"      }
  ,{ SQL_C_BIT,                       "XDT_Boolean"           ,"false"  }
  ,{ SQL_C_NUMERIC,                   "XDT_Decimal"           ,""       }
  ,{ SQL_C_ULONG,                     "XDT_UnsignedInt"       ,"0"      }
  ,{ SQL_C_SHORT,                     "XDT_Short"             ,"0"      }
  ,{ SQL_C_SSHORT,                    "XDT_Short"             ,"0"      }
  ,{ SQL_C_USHORT,                    "XDT_UnsignedShort"     ,"0"      }
  ,{ SQL_C_LONG,                      "XDT_Long"              ,"0"      }
  ,{ SQL_C_ULONG,                     "XDT_UnsignedLong"      ,"0"      }
  ,{ SQL_C_TINYINT,                   "XDT_Byte"              ,"0"      }
  ,{ SQL_C_STINYINT,                  "XDT_Byte"              ,"0"      }
  ,{ SQL_C_UTINYINT,                  "XDT_UnsignedByte"      ,"0"      }
  ,{ SQL_C_SBIGINT,                   "XDT_Decimal"           ,"0"      }
  ,{ SQL_C_UBIGINT,                   "XDT_Decimal"           ,"0"      }
  ,{ SQL_C_FLOAT,                     "XDT_Float"             ,"0.0"    }
  ,{ SQL_C_DOUBLE,                    "XDT_Decimal"           ,"0.0"    }
  ,{ SQL_C_DATE,                      "XDT_Date"              ,""       }
  ,{ SQL_C_TYPE_DATE,                 "XDT_Date"              ,""       }
  ,{ SQL_C_TIME,                      "XDT_Time"              ,""       }
  ,{ SQL_C_TYPE_TIME,                 "XDT_Time"              ,""       }
  ,{ SQL_C_TIMESTAMP,                 "XDT_DateTimeStamp"     ,""       }
  ,{ SQL_C_TYPE_TIMESTAMP,            "XDT_DateTimeStamp"     ,""       }
  ,{ SQL_C_GUID,                      "XDT_String"            ,""       }
  ,{ SQL_C_INTERVAL_DAY_TO_SECOND,    "XDT_DayTimeDuration"   ,""       }
  ,{ SQL_C_INTERVAL_YEAR_TO_MONTH,    "XDT_DayTimeDuration"   ,""       }
  ,{ SQL_C_INTERVAL_HOUR_TO_SECOND,   "XDT_DayTimeDuration"   ,""       }
  ,{ SQL_C_INTERVAL_HOUR_TO_MINUTE,   "XDT_DayTimeDuration"   ,""       }
  ,{ SQL_C_INTERVAL_DAY_TO_MINUTE,    "XDT_DayTimeDuration"   ,""       }
  ,{ SQL_C_INTERVAL_DAY_TO_HOUR,      "XDT_DayTimeDuration"   ,""       }
  ,{ SQL_C_INTERVAL_MINUTE_TO_SECOND, "XDT_DayTimeDuration"   ,""       }
  ,{ SQL_C_INTERVAL_YEAR,             "XDT_YearMonthDuration" ,""       }
  ,{ SQL_C_INTERVAL_MONTH,            "XDT_YearMonthDuration" ,""       }
  ,{ SQL_C_INTERVAL_DAY,              "XDT_DayTimeDuration"   ,""       }
  ,{ SQL_C_INTERVAL_HOUR,             "XDT_DayTimeDuration"   ,""       }
  ,{ SQL_C_INTERVAL_MINUTE,           "XDT_DayTimeDuration"   ,""       }
  ,{ SQL_C_INTERVAL_SECOND,           "XDT_DayTimeDuration"   ,""       }
};

CString DatatypeToMacro(int p_datatype)
{
  for(int ind = 0;ind < sizeof(tmacro)/sizeof(TypeMacro); ++ind)
  {
    if(tmacro[ind].m_value == p_datatype)
    {
      return tmacro[ind].m_name;
    }
  }
  return "<ERROR>";
}

CString DatatypeToInitialization(int p_datatype)
{
  for (int ind = 0; ind < sizeof(tmacro) / sizeof(TypeMacro); ++ind)
  {
    if (tmacro[ind].m_value == p_datatype)
    {
      return tmacro[ind].m_initialization;
    }
  }
  return "";
}

CString InitCapital(CString p_name)
{
  if(p_name.GetLength() && isalpha(p_name.GetAt(0)))
  {
    p_name.SetAt(0,toupper(p_name.GetAt(0)));
  }
  return p_name;
}

void WriteInterfaceHeader(FILE* p_file,CXClass* p_class)
{
  CString classname = p_class->GetName();

  fprintf(p_file,"// Interface definition file for class: %s\n",classname.GetString());
  fprintf(p_file,"// File: %s.h\n",classname.GetString());
  fprintf(p_file,"//\n");
  fprintf(p_file,"#pragma once\n");
  fprintf(p_file,"#include <CXObject.h>\n");
  fprintf(p_file,"#include <bcd.h>\n");
  fprintf(p_file,"#include <SQLDate.h>\n");
  fprintf(p_file,"#include <SQLTime.h>\n");
  fprintf(p_file,"#include <SQLTimestamp.h>\n");
  fprintf(p_file,"#include <SQLInterval.h>\n");
  fprintf(p_file,"#include <SQLGuid.h>\n");
  fprintf(p_file,"#include <SQLVariant.h>\n");
  fprintf(p_file,"\n");
}

void WriteInterfaceClass(FILE* p_file,CXClass* p_class)
{
  CString classname = p_class->GetName();

  fprintf(p_file,"class %s : public CXObject\n",classname.GetString());
  fprintf(p_file,"{\n");
  fprintf(p_file,"public:\n");
  fprintf(p_file,"  // CTOR of an CXObject derived class\n");
  fprintf(p_file,"  %s();\n",classname.GetString());
  fprintf(p_file,"\n");
  fprintf(p_file,"// Serialization of our persistent objects\n");
  fprintf(p_file,"  DECLARE_CXO_SERIALIZATION;\n");
  fprintf(p_file,"\n");
}

void WriteInterfaceGetters(FILE* p_file,CXClass* p_class)
{
  fprintf(p_file,"  // GETTERS\n");

  int index = 0;
  CXAttribute* attribute = p_class->FindAttribute(index);
  while(attribute)
  {
    CString type = CXDataTypeToString(attribute->GetDataType());
    CString getter = attribute->GetName();
    CString member = "m_" + getter;
    getter = "Get" + InitCapital(getter);

    fprintf(p_file,"  %-10s %-20s{ return %-20s; };\n"
                  ,type.GetString()
                  ,getter.GetString()
                  ,member.GetString());
    // Next attribute
    attribute = p_class->FindAttribute(++index);
  }
  fprintf(p_file,"\n");
}

void WriteInterfaceMembers(FILE* p_file,CXClass* p_class)
{
  fprintf(p_file,"protected:\n");
  fprintf(p_file,"  // Database persistent attributes\n");

  int index = 0;
  CXAttribute* attribute = p_class->FindAttribute(index);
  while (attribute)
  {
    CString type   = CXDataTypeToString(attribute->GetDataType());
    CString member = "m_" + attribute->GetName();
    CString init   = DatatypeToInitialization(attribute->GetDataType());
    
    fprintf(p_file,"  %-10s %-20s",type.GetString(),member.GetString());
    if (!init.IsEmpty())
    {
      fprintf(p_file," { %s }",init.GetString());
    }
    fprintf(p_file,";\n");

    // Next attribute
    attribute = p_class->FindAttribute(++index);
  }
  fprintf(p_file, "\n");
}

void WriteInterfaceFooter(FILE* p_file,CXClass* p_class)
{
  fprintf(p_file,"private:\n");
  fprintf(p_file,"  // Transient attributes go here\n");
  fprintf(p_file,"}\n");
}

void WriteInterface(CXClass* p_class)
{
  CString classname = p_class->GetName();
  CString filename  = classname + ".h";
  FILE* file = nullptr;
  int error = fopen_s(&file, filename, "w");
  if (error)
  {
    printf("OS error [%d] opening file: %s\n", error, filename.GetString());
    return;
  }
  printf("Writing output to: %s\n", filename.GetString());

  WriteInterfaceHeader (file, p_class);
  WriteInterfaceClass  (file, p_class);
  WriteInterfaceGetters(file, p_class);
  WriteInterfaceMembers(file, p_class);
  WriteInterfaceFooter (file, p_class);

  fclose(file);
}

void WriteCPPFile(CXClass* p_class)
{
  CString classname = p_class->GetName();
  CString filename  = classname + ".cpp";
  FILE* file = nullptr;
  int error = fopen_s(&file, filename, "w");
  if (error)
  {
    printf("OS error [%d] opening file: %s\n", error, filename.GetString());
    return;
  }
  printf("Writing output to: %s\n", filename.GetString());

  fprintf(file,"// Implementation file for class: %s\n",classname.GetString());
  fprintf(file,"//\n");
  fprintf(file,"#include \"stdafx.h\"\n");
  fprintf(file,"#include \"%s.h\"\n",classname.GetString());
  fprintf(file,"\n");
  fprintf(file,"#ifdef _DEBUG\n");
  fprintf(file,"#define new DEBUG_NEW\n");
  fprintf(file,"#undef THIS_FILE\n");
  fprintf(file,"static char THIS_FILE[] = __FILE__;\n");
  fprintf(file,"#endif\n");
  fprintf(file,"\n");
  fprintf(file,"// CTOR for class\n");
  fprintf(file,"%s::%s()\n",classname.GetString(),classname.GetString());
  fprintf(file,"{\n");
  fprintf(file,"  // Things to do in the constructor\n");
  fprintf(file,"}\n");

  fclose(file);
}

void PrintCXHFileHeader(FILE* p_file,CXClass* p_class)
{
  fprintf(p_file,"// (De-)Serializing factories for class: %s\n",p_class->GetName().GetString());
  fprintf(p_file,"// Generated by CX-Hibernate cfg2cxh tool\n");
  fprintf(p_file,"// \n");
  fprintf(p_file,"#include \"stdafx.h\"\n");
  fprintf(p_file,"#include \"%s.h\"\n",p_class->GetName().GetString());
  fprintf(p_file,"#include <SQLRecord.h>\n");
  fprintf(p_file,"#include <SOAPMessage.h>\n");
  fprintf(p_file,"\n");
  fprintf(p_file,"#ifdef _DEBUG\n");
  fprintf(p_file,"#define new DEBUG_NEW\n");
  fprintf(p_file,"#undef THIS_FILE\n");
  fprintf(p_file,"static char THIS_FILE[] = __FILE__;\n");
  fprintf(p_file,"#endif\n");
  fprintf(p_file, "\n");
}

void PrintCXHDeSerialize(FILE* p_file,CXClass* p_class,CString p_type)
{
  fprintf(p_file,"BEGIN_%sSERIALIZE(%s)\n",p_type.GetString(),p_class->GetName().GetString());

  int index = 0;
  CXAttribute* attribute = p_class->FindAttribute(index);
  while (attribute)
  {
    fprintf(p_file,"  CXO_%sSERIALIZE(%-12s,%-18s,%-18s,%s);\n"
                  , p_type.GetString()
                  , CXDataTypeToString(attribute->GetDataType()).GetString()
                  , ("m_" + attribute->GetName()).GetString()
                  , ("\"" + attribute->GetDatabaseColumn() + "\"").GetString()
                  , DatatypeToMacro(attribute->GetDataType()).GetString());

    // Next attribute
    attribute = p_class->FindAttribute(++index);
  }
  fprintf(p_file,"END_%sSERIALIZE\n",p_type.GetString());
  fprintf(p_file,"\n");
}

void PrintCXHFactory(FILE* p_file, CXClass* p_class)
{
  fprintf(p_file,"// Static factory to create a new object if this class\n");
  fprintf(p_file, "DEFINE_CXO_FACTORY(%s);\n", p_class->GetName().GetString());
  fprintf(p_file, "\n");
}

void WriteCXHFile(CXClass* p_class)
{
  CString filename = p_class->GetName() + "_cxh.cpp";
  FILE* file = nullptr;
  int error = fopen_s(&file,filename,"w");
  if (error)
  {
    printf("OS error [%d] opening file: %s\n",error,filename.GetString());
    return;
  }
  printf("Writing output to: %s\n",filename.GetString());

  PrintCXHFileHeader (file, p_class);
  PrintCXHDeSerialize(file, p_class,"XML_");
  PrintCXHDeSerialize(file, p_class,"XML_DE");
  PrintCXHDeSerialize(file, p_class,"DBS_");
  PrintCXHDeSerialize(file, p_class,"DBS_DE");
  PrintCXHFactory    (file, p_class);

  fclose(file);
}

int main(int argc,char** argv)
{
  printf("CFG2CPP Hibernate.cfg.xml to *.cpp file converter\n");
  printf("Copyright (c) ir. W.E. Huisman\n");
  printf("Version: %s of %s\n", HIBERNATE_VERSION,HIBERNATE_DATE);
  printf("\n");
  if(argc == 2)
  {
    CString classname = argv[1];

    printf("Creating session\n");
    printf("Reading default hibernate.cfg.xml file\n");
    CXSession* session = hibernate.CreateSession();
    if(session)
    {
      printf("Finding our class: %s\n",classname.GetString());
      CXClass* cl = session->FindClass(classname);
      if(cl)
      {
        WriteInterface(cl);
        WriteCPPFile(cl);
        WriteCXHFile(cl);
      }
      else
      {
        printf("ERROR: Cannot find classname: %s\n",classname.GetString());
      }
    }
    else
    {
      printf("ERROR: Cannot create a hibernate session\n");
      printf("Have you defined a correct 'hibernate.cfg.xml' file?\n");
    }
  }
  else
  {
    printf("USAGE: cfg2cpp classname\n");
  }
  return 0;
}
