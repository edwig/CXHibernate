//////////////////////////////////////////////////////////////////////////
// cfg2cpp.cpp : Generate C++ classes for CX-Hibernate
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

static CString g_configFile;

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
  CString superclass;
  CString classname = p_class->GetName();
  if(p_class->GetSuperClass())
  {
    superclass = p_class->GetSuperClass()->GetName();
  }

  fprintf(p_file,"// Interface definition file for class: %s\n",classname.GetString());
  fprintf(p_file,"// Automatically generated by: CX-Hibernate\n");
  fprintf(p_file,"// File: %s.h\n",classname.GetString());
  fprintf(p_file,"//\n");
  fprintf(p_file,"#pragma once\n");
  fprintf(p_file,"#include <CXObject.h>\n");

  if(!superclass.IsEmpty())
  {
    fprintf(p_file,"#include \"%s.h\"\n",superclass.GetString());
  }

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
  CString classname  = p_class->GetName();
  CString superclass = p_class->GetSuperClass() ? p_class->GetSuperClass()->GetName() : CString("CXObject");

  fprintf(p_file,"class %s : public %s\n",classname.GetString(),superclass.GetString());
  fprintf(p_file,"{\n");
  fprintf(p_file,"public:\n");
  fprintf(p_file,"  // CTOR of an CXObject derived class\n");
  fprintf(p_file,"  %s();\n",classname.GetString());
  fprintf(p_file,"  // DTOR of an CXObject derived class\n");
  fprintf(p_file,"  virtual ~%s();\n",classname.GetString());
  fprintf(p_file,"\n");
  fprintf(p_file,"  // Serialization of our persistent objects\n");
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
    getter += "()";
    member += ";";

    fprintf(p_file,"  %-10s %-20s { return %-20s };\n"
                  ,type.GetString()
                  ,getter.GetString()
                  ,member.GetString());
    // Next attribute
    attribute = p_class->FindAttribute(++index);
  }
  fprintf(p_file,"\n");
}

void WriteInterfaceSetters(FILE* p_file,CXClass* p_class)
{
  fprintf(p_file, "  // SETTERS\n");

  int index = 0;
  CXAttribute* attribute = p_class->FindAttribute(index);
  while (attribute)
  {
    CString type   = CXDataTypeToString(attribute->GetDataType());
    CString setter = attribute->GetName();
    CString member = "m_" + setter;
    CString param  = "p_" + setter;
    CString assign = param + ";";
    setter = InitCapital(setter);

    fprintf(p_file,"  void       Set%-20s(%-10s %-20s) { %-20s = %-20s };\n"
                  ,setter.GetString()
                  ,type  .GetString()
                  ,param .GetString()
                  ,member.GetString()
                  ,assign.GetString());
    // Next attribute
    attribute = p_class->FindAttribute(++index);
  }
  fprintf(p_file,"\n");
}

void WriteInterfaceAssocs(FILE* p_file, CXClass* p_class)
{
  int index = 0;

  CXAssociation* assoc = p_class->FindAssociation(index);
  while(assoc)
  {
    CString type = CXAssocTypeToSTring(assoc->m_assocType);
    CString otherclass  = assoc->m_primaryTable;
    CString association = assoc->m_constraintName;
    CString resulttype  = assoc->m_assocType == ASSOC_MANY_TO_ONE ? otherclass : CString("CXResultSet");

    fprintf(p_file,"  // Association %s to class %s\n",type.GetString(),otherclass.GetString());
    fprintf(p_file,"  %s* Get%s(CXSession* p_session);\n",resulttype.GetString(),association.GetString());
    fprintf(p_file,"\n");

    // next association
    assoc = p_class->FindAssociation(++index);
  }
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
  fprintf(p_file,"};\n");
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
  WriteInterfaceSetters(file, p_class);
  WriteInterfaceAssocs (file, p_class);
  WriteInterfaceMembers(file, p_class);
  WriteInterfaceFooter (file, p_class);

  fclose(file);
}

void WriteCPPHeader(FILE* p_file,CXClass* p_class)
{
  CString superclass;
  CString classname = p_class->GetName();
  if(p_class->GetSuperClass())
  {
    superclass = p_class->GetSuperClass()->GetName();
  }

  fprintf(p_file,"// Implementation file for class: %s\n",classname.GetString());
  fprintf(p_file,"// Automatically generated by: CX-Hibernate\n");
  fprintf(p_file,"//\n");
  fprintf(p_file,"#include \"stdafx.h\"\n");
  fprintf(p_file,"#include \"%s.h\"\n",classname.GetString());

  if(!superclass.IsEmpty())
  {
    fprintf(p_file,"#include \"%s.h\"\n",superclass.GetString());
  }

  fprintf(p_file,"\n");
  fprintf(p_file,"#ifdef _DEBUG\n");
  fprintf(p_file,"#define new DEBUG_NEW\n");
  fprintf(p_file,"#undef THIS_FILE\n");
  fprintf(p_file,"static char THIS_FILE[] = __FILE__;\n");
  fprintf(p_file,"#endif\n");
  fprintf(p_file,"\n");
  fprintf(p_file,"// CTOR for class\n");
  fprintf(p_file,"%s::%s()\n",classname.GetString(),classname.GetString());
  fprintf(p_file,"{\n");
  fprintf(p_file,"  // Things to do in the constructor\n");
  fprintf(p_file,"}\n");
  fprintf(p_file,"\n");
  fprintf(p_file,"// DTOR for class\n");
  fprintf(p_file,"%s::~%s()\n",classname.GetString(),classname.GetString());
  fprintf(p_file,"{\n");
  fprintf(p_file,"  // Things to do in the destructor\n");
  fprintf(p_file,"}\n");
  fprintf(p_file,"\n");
}

void WriteAssocOneToMany(FILE* p_file,CXClass* p_class,CXAssociation* p_assoc)
{
  CString classname   = p_class->GetName();
  CString otherclass  = p_assoc->m_primaryTable;
  CString association = p_assoc->m_constraintName;
  CString key;
  bool optimized = false;

  if(p_assoc->m_attributes.size() == 1)
  {
    // Check for optimized primary key of 1 INTEGER column
    CXAttribute* attribute = p_assoc->m_attributes[0];
    CString  attributeName = attribute->GetName();
    int datatype = attribute->GetDataType();
    if(datatype == SQL_INTEGER || datatype == SQL_C_SLONG || datatype == SQL_C_ULONG)
    {
      optimized = true;
      key = attribute->GetDatabaseColumn();
    }
  }

  fprintf(p_file,"// Getting a one-to-many association's objects from %s\n",classname.GetString());
  fprintf(p_file,"CXResultSet\n");
  fprintf(p_file,"%s::Get%s(CXSession* p_session)\n",classname.GetString(),association.GetString());
  fprintf(p_file,"{\n");
  if(optimized)
  {
    fprintf(p_file,"  return p_session->FollowAssociation(ClassName(),%s::ClassName(),m_%s);\n",otherclass.GetString(),key.GetString());
  }
  else
  {
    fprintf(p_file,"  VariantSet set = GetPrimaryKey();\n");
    fprintf(p_file,"  return p_session->FollowAssociation(ClassName(),%s::ClassName(),set);\n",otherclass.GetString());
  }
  fprintf(p_file,"}\n");
  fprintf(p_file,"\n");
}

void WriteAssocManyToOne(FILE* p_file,CXClass* p_class,CXAssociation* p_assoc)
{
  CString classname   = p_class->GetName();
  CString otherclass  = p_assoc->m_primaryTable;
  CString association = p_assoc->m_constraintName;
  CString key;
  bool optimized = false;

  if(p_assoc->m_attributes.size() == 1)
  {
    // Check for optimized primary key of 1 INTEGER column
    CXAttribute* attribute = p_assoc->m_attributes[0];
    CString  attributeName = attribute->GetName();
    int datatype = attribute->GetDataType();
    if(datatype == SQL_INTEGER || datatype == SQL_C_SLONG || datatype == SQL_C_ULONG)
    {
      optimized = true;
      key = attribute->GetDatabaseColumn();
    }
  }

  fprintf(p_file,"// Getting a many-to-one association's object from Master\n");
  fprintf(p_file,"%s*\n",otherclass.GetString());
  fprintf(p_file,"%s::Get%s(CXSession* p_session)\n",classname.GetString(),association.GetString());
  fprintf(p_file,"{\n");
  if(optimized)
  {
    fprintf(p_file,"  CXResultSet set = p_session->FollowAssociation(ClassName(),%s::ClassName(),m_%s);\n",otherclass.GetString(),key.GetString());
  }
  else
  {
    int index = 1;
    fprintf(p_file,"  VariantSet values\n");
    for(auto& attrib : p_assoc->m_attributes)
    {
      fprintf(p_file,"  SQLVariant var%d(m_%s)\n",index,attrib->GetName().GetString());
      fprintf(p_file,"  values.push_back(var%d);\n",index);
    }
    fprintf(p_file,"  CXResultSet set = p_session->FollowAssociation(ClassName(),%s::ClassName(),values);\n",otherclass.GetString());
  }
  fprintf(p_file,"  if(set.size() == 1)\n");
  fprintf(p_file,"  {\n");
  fprintf(p_file,"    return (%s*)set.front();\n",otherclass.GetString());
  fprintf(p_file,"  }\n");
  fprintf(p_file,"  return nullptr;\n");
  fprintf(p_file,"}\n");
  fprintf(p_file,"\n");
}

void WriteCPPAssocs(FILE* p_file,CXClass* p_class)
{
  int index = 0;

  CXAssociation* assoc = p_class->FindAssociation(index);
  while(assoc)
  {
    switch(assoc->m_assocType)
    {
      case ASSOC_ONE_TO_MANY: WriteAssocOneToMany(p_file, p_class, assoc);
                              break;
      case ASSOC_MANY_TO_ONE: WriteAssocManyToOne(p_file,p_class,assoc);
                              break;
    }
    // next association
    assoc = p_class->FindAssociation(++index);
  }
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

  WriteCPPHeader(file,p_class);
  WriteCPPAssocs(file,p_class);

  fclose(file);
}

void PrintCXHFileHeader(FILE* p_file,CXClass* p_class)
{
  fprintf(p_file,"// (De-)Serializing factories for class: %s\n",p_class->GetName().GetString());
  fprintf(p_file,"// Generated by CX-Hibernate cfg2cpp tool\n");
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
  CString classname = p_class->GetName();
  CString superclass = p_class->GetSuperClass() ? p_class->GetSuperClass()->GetName() : CString("CXObject");

  fprintf(p_file,"BEGIN_%sSERIALIZE(%s,%s)\n",p_type.GetString(),classname.GetString(),superclass.GetString());

  int index = 0;
  CXAttribute* attribute = p_class->FindAttribute(index);
  while (attribute)
  {
    fprintf(p_file,"  CXO_%sSERIALIZE(%-18s,%-18s);\n"
                  , p_type.GetString()
                  , ("m_" + attribute->GetName()).GetString()
                  , ("\"" + attribute->GetDatabaseColumn() + "\"").GetString());

    // Next attribute
    attribute = p_class->FindAttribute(++index);
  }
  fprintf(p_file,"END_%sSERIALIZE\n",p_type.GetString());
  fprintf(p_file,"\n");
}

void PrintGenDeSerialize(FILE* p_file,CXClass* p_class)
{
  CXAttribute* gen = p_class->FindGenerator();
  if(gen)
  {
    fprintf(p_file,"BEGIN_DESERIALIZE_GENERATOR(%s)\n",p_class->GetName().GetString());
    fprintf(p_file,"  CXO_DBS_DESERIALIZE(%-18s,%-18s);\n"
                  ,("m_" + gen->GetName()).GetString()
                  ,("\"" + gen->GetDatabaseColumn() + "\"").GetString());
    fprintf(p_file,"END_DESERIALIZE_GENERATOR\n");
    fprintf(p_file,"\n");
  }
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
  PrintGenDeSerialize(file, p_class);
  PrintCXHFactory    (file, p_class);

  fclose(file);
}

CString GetParameters(int argc,char** argv)
{
  int index = 1;
  CString classname;
  bool error = false;

  while(index < argc)
  {
    CString param(argv[index]);

    if(param.GetAt(0) == '/')
    {
      if(param.Left(8).CompareNoCase("/config:") == 0)
      {
        g_configFile = param.Mid(8);
      }
      else
      {
        error = true;
      }
    }
    else
    {
      classname = param;
    }
    ++index;
  }

  // In case of an error: print usage of the tool
  if(error || classname.IsEmpty())
  {
    printf("USAGE: cfg2cpp [/config:hibernate.cfg.xml] classname\n");
    exit(-3);
  }
  return classname;
}

int main(int argc,char** argv)
{
  printf("CFG2CPP Hibernate.cfg.xml to *.cpp file converter\n");
  printf("Copyright (c) ir. W.E. Huisman\n");
  printf("Version: %s of %s\n", HIBERNATE_VERSION,HIBERNATE_DATE);
  printf("\n");

  CString classname = GetParameters(argc,argv);

  printf("Creating session\n");
  printf("Reading configuration file: %s\n",g_configFile.GetString());

  // Tools: set incomplete mode!!
  hibernate.SetIncomplete(true);

  try
  {
    // Request a session
    CXSession* session = hibernate.CreateSession("",g_configFile);
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
      session->CloseSession();
    }
    else
    {
      printf("ERROR: Cannot create a hibernate session\n");
      printf("Have you defined a correct 'hibernate.cfg.xml' file?\n");
    }
  }
  catch(StdException& ex)
  {
    printf("ERROR: %s\n",ex.GetErrorMessage().GetString());
  }
  return 0;
}
