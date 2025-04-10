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
  int          m_value;
  const TCHAR* m_name;
  const TCHAR* m_initialization;
}
TypeMacro;

TypeMacro tmacro[] =
{
   { SQL_C_CHAR,                      _T("XDT_String")            ,_T("")       }
  ,{ SQL_C_LONG,                      _T("XDT_Integer")           ,_T("0")      }
  ,{ SQL_C_SLONG,                     _T("XDT_Integer")           ,_T("0")      }
  ,{ SQL_C_BIT,                       _T("XDT_Boolean")           ,_T("false")  }
  ,{ SQL_C_NUMERIC,                   _T("XDT_Decimal")           ,_T("")       }
  ,{ SQL_C_ULONG,                     _T("XDT_UnsignedInt")       ,_T("0")      }
  ,{ SQL_C_SHORT,                     _T("XDT_Short")             ,_T("0")      }
  ,{ SQL_C_SSHORT,                    _T("XDT_Short")             ,_T("0")      }
  ,{ SQL_C_USHORT,                    _T("XDT_UnsignedShort")     ,_T("0")      }
  ,{ SQL_C_LONG,                      _T("XDT_Long")              ,_T("0")      }
  ,{ SQL_C_ULONG,                     _T("XDT_UnsignedLong")      ,_T("0")      }
  ,{ SQL_C_TINYINT,                   _T("XDT_Byte")              ,_T("0")      }
  ,{ SQL_C_STINYINT,                  _T("XDT_Byte")              ,_T("0")      }
  ,{ SQL_C_UTINYINT,                  _T("XDT_UnsignedByte")      ,_T("0")      }
  ,{ SQL_C_SBIGINT,                   _T("XDT_Decimal")           ,_T("0")      }
  ,{ SQL_C_UBIGINT,                   _T("XDT_Decimal")           ,_T("0")      }
  ,{ SQL_C_FLOAT,                     _T("XDT_Float")             ,_T("0.0")    }
  ,{ SQL_C_DOUBLE,                    _T("XDT_Decimal")           ,_T("0.0")    }
  ,{ SQL_C_DATE,                      _T("XDT_Date")              ,_T("")       }
  ,{ SQL_C_TYPE_DATE,                 _T("XDT_Date")              ,_T("")       }
  ,{ SQL_C_TIME,                      _T("XDT_Time")              ,_T("")       }
  ,{ SQL_C_TYPE_TIME,                 _T("XDT_Time")              ,_T("")       }
  ,{ SQL_C_TIMESTAMP,                 _T("XDT_DateTimeStamp")     ,_T("")       }
  ,{ SQL_C_TYPE_TIMESTAMP,            _T("XDT_DateTimeStamp")     ,_T("")       }
  ,{ SQL_C_GUID,                      _T("XDT_String")            ,_T("")       }
  ,{ SQL_C_INTERVAL_DAY_TO_SECOND,    _T("XDT_DayTimeDuration")   ,_T("")       }
  ,{ SQL_C_INTERVAL_YEAR_TO_MONTH,    _T("XDT_DayTimeDuration")   ,_T("")       }
  ,{ SQL_C_INTERVAL_HOUR_TO_SECOND,   _T("XDT_DayTimeDuration")   ,_T("")       }
  ,{ SQL_C_INTERVAL_HOUR_TO_MINUTE,   _T("XDT_DayTimeDuration")   ,_T("")       }
  ,{ SQL_C_INTERVAL_DAY_TO_MINUTE,    _T("XDT_DayTimeDuration")   ,_T("")       }
  ,{ SQL_C_INTERVAL_DAY_TO_HOUR,      _T("XDT_DayTimeDuration")   ,_T("")       }
  ,{ SQL_C_INTERVAL_MINUTE_TO_SECOND, _T("XDT_DayTimeDuration")   ,_T("")       }
  ,{ SQL_C_INTERVAL_YEAR,             _T("XDT_YearMonthDuration") ,_T("")       }
  ,{ SQL_C_INTERVAL_MONTH,            _T("XDT_YearMonthDuration") ,_T("")       }
  ,{ SQL_C_INTERVAL_DAY,              _T("XDT_DayTimeDuration")   ,_T("")       }
  ,{ SQL_C_INTERVAL_HOUR,             _T("XDT_DayTimeDuration")   ,_T("")       }
  ,{ SQL_C_INTERVAL_MINUTE,           _T("XDT_DayTimeDuration")   ,_T("")       }
  ,{ SQL_C_INTERVAL_SECOND,           _T("XDT_DayTimeDuration")   ,_T("")       }
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
  return _T("<ERROR>");
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
  return _T("");
}

CString InitCapital(CString p_name)
{
  if(p_name.GetLength() && _istalpha(p_name.GetAt(0)))
  {
    p_name.SetAt(0,_totupper(p_name.GetAt(0)));
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

  _ftprintf(p_file,_T("// Interface definition file for class: %s\n"),classname.GetString());
  _ftprintf(p_file,_T("// Automatically generated by: CX-Hibernate\n"));
  _ftprintf(p_file,_T("// File: %s.h\n"),classname.GetString());
  _ftprintf(p_file,_T("//\n"));
  _ftprintf(p_file,_T("#pragma once\n"));
  _ftprintf(p_file,_T("#include <CXObject.h>\n"));

  if(!superclass.IsEmpty())
  {
    _ftprintf(p_file,_T("#include \"%s.h\"\n"),superclass.GetString());
  }

  _ftprintf(p_file,_T("#include <bcd.h>\n"));
  _ftprintf(p_file,_T("#include <SQLDate.h>\n"));
  _ftprintf(p_file,_T("#include <SQLTime.h>\n"));
  _ftprintf(p_file,_T("#include <SQLTimestamp.h>\n"));
  _ftprintf(p_file,_T("#include <SQLInterval.h>\n"));
  _ftprintf(p_file,_T("#include <SQLGuid.h>\n"));
  _ftprintf(p_file,_T("#include <SQLVariant.h>\n"));
  _ftprintf(p_file,_T("\n"));

  int index = 0;
  CXAssociation* assoc = p_class->FindAssociation(index);
  while(assoc)
  {
    _ftprintf(p_file,_T("class %s;\n"),assoc->m_primaryTable.GetString());
    assoc = p_class->FindAssociation(++index);
  }
  _ftprintf(p_file,_T("\n"));
}

void WriteInterfaceClass(FILE* p_file,CXClass* p_class)
{
  CString classname  = p_class->GetName();
  CString superclass = p_class->GetSuperClass() ? p_class->GetSuperClass()->GetName() : CString(_T("CXObject"));

  _ftprintf(p_file,_T("class %s : public %s\n"),classname.GetString(),superclass.GetString());
  _ftprintf(p_file,_T("{\n"));
  _ftprintf(p_file,_T("public:\n"));
  _ftprintf(p_file,_T("  // CTOR of an CXObject derived class\n"));
  _ftprintf(p_file,_T("  %s();\n"),classname.GetString());
  _ftprintf(p_file,_T("  // DTOR of an CXObject derived class\n"));
  _ftprintf(p_file,_T("  virtual ~%s();\n"),classname.GetString());
  _ftprintf(p_file,_T("\n"));
  _ftprintf(p_file,_T("  // Serialization of our persistent objects\n"));
  _ftprintf(p_file,_T("  DECLARE_CXO_SERIALIZATION;\n"));
  _ftprintf(p_file,_T("\n"));
}

void WriteInterfaceGetters(FILE* p_file,CXClass* p_class)
{
  _ftprintf(p_file,_T("  // GETTERS\n"));

  int index = 0;
  CXAttribute* attribute = p_class->FindAttribute(index);
  while(attribute)
  {
    CString type = CXDataTypeToString(attribute->GetDataType());
    CString getter = attribute->GetName();
    CString member = _T("m_") + getter;
    getter = _T("Get") + InitCapital(getter);
    getter += _T("()");
    member += _T(";");

    _ftprintf(p_file,_T("  %-10s %-20s { return %-20s };\n")
                  ,type.GetString()
                  ,getter.GetString()
                  ,member.GetString());
    // Next attribute
    attribute = p_class->FindAttribute(++index);
  }
  _ftprintf(p_file,_T("\n"));
}

void WriteInterfaceSetters(FILE* p_file,CXClass* p_class)
{
  _ftprintf(p_file, _T("  // SETTERS\n"));

  int index = 0;
  CXAttribute* attribute = p_class->FindAttribute(index);
  while (attribute)
  {
    CString type   = CXDataTypeToString(attribute->GetDataType());
    CString setter = attribute->GetName();
    CString member = _T("m_") + setter;
    CString param  = _T("p_") + setter;
    CString assign = param + _T(";");
    setter = InitCapital(setter);

    _ftprintf(p_file,_T("  void       Set%-20s(%-10s %-20s) { %-20s = %-20s };\n")
                  ,setter.GetString()
                  ,type  .GetString()
                  ,param .GetString()
                  ,member.GetString()
                  ,assign.GetString());
    // Next attribute
    attribute = p_class->FindAttribute(++index);
  }
  _ftprintf(p_file,_T("\n"));
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
    CString resulttype  = assoc->m_assocType == ASSOC_MANY_TO_ONE ? otherclass + _T("*") : CString(_T("CXResultSet"));

    _ftprintf(p_file,_T("  // Association %s to class %s\n"),type.GetString(),otherclass.GetString());
    _ftprintf(p_file,_T("  %s Get_%s(CXSession* p_session);\n"),resulttype.GetString(),association.GetString());
    _ftprintf(p_file,_T("\n"));

    // next association
    assoc = p_class->FindAssociation(++index);
  }
}

void WriteInterfaceMembers(FILE* p_file,CXClass* p_class)
{
  _ftprintf(p_file,_T("protected:\n"));
  _ftprintf(p_file,_T("  // Database persistent attributes\n"));

  int index = 0;
  CXAttribute* attribute = p_class->FindAttribute(index);
  while (attribute)
  {
    CString type   = CXDataTypeToString(attribute->GetDataType());
    CString member = _T("m_") + attribute->GetName();
    CString init   = DatatypeToInitialization(attribute->GetDataType());
    
    _ftprintf(p_file,_T("  %-10s %-20s"),type.GetString(),member.GetString());
    if (!init.IsEmpty())
    {
      _ftprintf(p_file,_T(" { %s }"),init.GetString());
    }
    _ftprintf(p_file,_T(";\n"));

    // Next attribute
    attribute = p_class->FindAttribute(++index);
  }
  _ftprintf(p_file, _T("\n"));
}

void WriteInterfaceFooter(FILE* p_file,CXClass* p_class)
{
  _ftprintf(p_file,_T("private:\n"));
  _ftprintf(p_file,_T("  // Transient attributes go here\n"));
  _ftprintf(p_file,_T("};\n"));
}

void WriteInterface(CXClass* p_class)
{
  CString classname = p_class->GetName();
  CString filename  = classname + _T(".h");
  FILE* file = nullptr;
  int error = _tfopen_s(&file, filename, _T("w"));
  if (error)
  {
    _tprintf(_T("OS error [%d] opening file: %s\n"), error, filename.GetString());
    return;
  }
  _tprintf(_T("Writing output to: %s\n"), filename.GetString());

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

  _ftprintf(p_file,_T("// Implementation file for class: %s\n"),classname.GetString());
  _ftprintf(p_file,_T("// Automatically generated by: CX-Hibernate\n"));
  _ftprintf(p_file,_T("//\n"));
  _ftprintf(p_file,_T("#include \"stdafx.h\"\n"));
  _ftprintf(p_file,_T("#include \"%s.h\"\n"),classname.GetString());

  if(!superclass.IsEmpty())
  {
    _ftprintf(p_file,_T("#include \"%s.h\"\n"),superclass.GetString());
  }

  int index = 0;
  CXAssociation* assoc = p_class->FindAssociation(index);
  while(assoc)
  {
    _ftprintf(p_file,_T("#include \"%s.h\"\n"),assoc->m_primaryTable.GetString());
    assoc = p_class->FindAssociation(++index);
  }
  _ftprintf(p_file,_T("#include <CXSession.h>\n"));

  _ftprintf(p_file,_T("\n"));
  _ftprintf(p_file,_T("#ifdef _DEBUG\n"));
  _ftprintf(p_file,_T("#define new DEBUG_NEW\n"));
  _ftprintf(p_file,_T("#undef THIS_FILE\n"));
  _ftprintf(p_file,_T("static char THIS_FILE[] = __FILE__;\n"));
  _ftprintf(p_file,_T("#endif\n"));
  _ftprintf(p_file,_T("\n"));
  _ftprintf(p_file,_T("// CTOR for class\n"));
  _ftprintf(p_file,_T("%s::%s()\n"),classname.GetString(),classname.GetString());
  _ftprintf(p_file,_T("{\n"));
  _ftprintf(p_file,_T("  // Things to do in the constructor\n"));
  _ftprintf(p_file,_T("}\n"));
  _ftprintf(p_file,_T("\n"));
  _ftprintf(p_file,_T("// DTOR for class\n"));
  _ftprintf(p_file,_T("%s::~%s()\n"),classname.GetString(),classname.GetString());
  _ftprintf(p_file,_T("{\n"));
  _ftprintf(p_file,_T("  // Things to do in the destructor\n"));
  _ftprintf(p_file,_T("}\n"));
  _ftprintf(p_file,_T("\n"));
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

  _ftprintf(p_file,_T("// Getting a one-to-many association's objects from %s\n"),classname.GetString());
  _ftprintf(p_file,_T("CXResultSet\n"));
  _ftprintf(p_file,_T("%s::Get_%s(CXSession* p_session)\n"),classname.GetString(),association.GetString());
  _ftprintf(p_file,_T("{\n"));
  if(optimized)
  {
    _ftprintf(p_file,_T("  return p_session->FollowAssociation(this,%s::ClassName(),m_%s);\n"),otherclass.GetString(),key.GetString());
  }
  else
  {
    _ftprintf(p_file,_T("  VariantSet set = GetPrimaryKey();\n"));
    _ftprintf(p_file,_T("  return p_session->FollowAssociation(this,%s::ClassName(),set);\n"),otherclass.GetString());
  }
  _ftprintf(p_file,_T("}\n"));
  _ftprintf(p_file,_T("\n"));
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

  _ftprintf(p_file,_T("// Getting a many-to-one association's object from Master\n"));
  _ftprintf(p_file,_T("%s*\n"),otherclass.GetString());
  _ftprintf(p_file,_T("%s::Get_%s(CXSession* p_session)\n"),classname.GetString(),association.GetString());
  _ftprintf(p_file,_T("{\n"));
  if(optimized)
  {
    _ftprintf(p_file,_T("  CXResultSet set = p_session->FollowAssociation(this,%s::ClassName(),m_%s);\n"),otherclass.GetString(),key.GetString());
  }
  else
  {
    _ftprintf(p_file,_T("  VariantSet values;\n"));
    int index = 1;
    for(auto& attrib : p_assoc->m_attributes)
    {
      _ftprintf(p_file,_T("  SQLVariant var%d(m_%s);\n"),index,attrib->GetName().GetString());
      ++index;
    }
    index = 1;
    for(auto& attrib : p_assoc->m_attributes)
    {
      _ftprintf(p_file,_T("  values.push_back(&var%d);\n"),index);
      ++index;
    }
    _ftprintf(p_file,_T("  CXResultSet set = p_session->FollowAssociation(this,%s::ClassName(),values);\n"),otherclass.GetString());
  }
  _ftprintf(p_file,_T("  if(set.size() == 1)\n"));
  _ftprintf(p_file,_T("  {\n"));
  _ftprintf(p_file,_T("    return (%s*)set.front();\n"),otherclass.GetString());
  _ftprintf(p_file,_T("  }\n"));
  _ftprintf(p_file,_T("  return nullptr;\n"));
  _ftprintf(p_file,_T("}\n"));
  _ftprintf(p_file,_T("\n"));
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
  CString filename  = classname + _T(".cpp");
  FILE* file = nullptr;
  int error = _tfopen_s(&file, filename, _T("w"));
  if (error)
  {
    _tprintf(_T("OS error [%d] opening file: %s\n"), error, filename.GetString());
    return;
  }
  _tprintf(_T("Writing output to: %s\n"), filename.GetString());

  WriteCPPHeader(file,p_class);
  WriteCPPAssocs(file,p_class);

  fclose(file);
}

void PrintCXHFileHeader(FILE* p_file,CXClass* p_class)
{
  _ftprintf(p_file,_T("// (De-)Serializing factories for class: %s\n"),p_class->GetName().GetString());
  _ftprintf(p_file,_T("// Generated by CX-Hibernate cfg2cpp tool\n"));
  _ftprintf(p_file,_T("// \n"));
  _ftprintf(p_file,_T("#include \"stdafx.h\"\n"));
  _ftprintf(p_file,_T("#include \"%s.h\"\n"),p_class->GetName().GetString());
  _ftprintf(p_file,_T("#include <SQLRecord.h>\n"));
  _ftprintf(p_file,_T("#include <SOAPMessage.h>\n"));
  _ftprintf(p_file,_T("\n"));
  _ftprintf(p_file,_T("#ifdef _DEBUG\n"));
  _ftprintf(p_file,_T("#define new DEBUG_NEW\n"));
  _ftprintf(p_file,_T("#undef THIS_FILE\n"));
  _ftprintf(p_file,_T("static char THIS_FILE[] = __FILE__;\n"));
  _ftprintf(p_file,_T("#endif\n"));
  _ftprintf(p_file, _T("\n"));
}

void PrintCXHDeSerialize(FILE* p_file,CXClass* p_class,CString p_type)
{
  CString classname = p_class->GetName();
  CString superclass = p_class->GetSuperClass() ? p_class->GetSuperClass()->GetName() : CString(_T("CXObject"));

  _ftprintf(p_file,_T("BEGIN_%sSERIALIZE(%s,%s)\n"),p_type.GetString(),classname.GetString(),superclass.GetString());

  int index = 0;
  CXAttribute* attribute = p_class->FindAttribute(index);
  while (attribute)
  {
    _ftprintf(p_file,_T("  CXO_%sSERIALIZE(%-18s,%-18s);\n")
                  , p_type.GetString()
                  , (_T("m_") + attribute->GetName()).GetString()
                  , (_T("_T(\"") + attribute->GetDatabaseColumn() + _T("\")")).GetString());

    // Next attribute
    attribute = p_class->FindAttribute(++index);
  }
  _ftprintf(p_file,_T("END_%sSERIALIZE\n"),p_type.GetString());
  _ftprintf(p_file,_T("\n"));
}

void PrintGenDeSerialize(FILE* p_file,CXClass* p_class)
{
  CXAttribute* gen = p_class->FindGenerator();
  if(gen)
  {
    _ftprintf(p_file,_T("BEGIN_DESERIALIZE_GENERATOR(%s)\n"),p_class->GetName().GetString());
    _ftprintf(p_file,_T("  CXO_DBS_DESERIALIZE(%-18s,%-18s);\n")
                  ,(_T("m_") + gen->GetName()).GetString()
                  ,(_T("_T(\"") + gen->GetDatabaseColumn() + _T("\")")).GetString());
    _ftprintf(p_file,_T("END_DESERIALIZE_GENERATOR\n"));
    _ftprintf(p_file,_T("\n"));
  }
  else
  {
    // Generate the whole primary key as the identity generator
    WordList list = p_class->GetPrimaryKeyAsList();

    _ftprintf(p_file,_T("BEGIN_DESERIALIZE_GENERATOR(%s)\n"),p_class->GetName().GetString());
    for(auto& word : list)
    {
      _ftprintf(p_file,_T("  CXO_DBS_DESERIALIZE(%-18s,%-18s);\n")
                ,(_T("m_") + word).GetString()
                ,(_T("_T(\"") + word + _T("\")")).GetString());
    }
    _ftprintf(p_file,_T("END_DESERIALIZE_GENERATOR\n"));
    _ftprintf(p_file,_T("\n"));
  }
}

void PrintCXHFactory(FILE* p_file, CXClass* p_class)
{
  _ftprintf(p_file,_T("// Static factory to create a new object if this class\n"));
  _ftprintf(p_file, _T("DEFINE_CXO_FACTORY(%s);\n"), p_class->GetName().GetString());
  _ftprintf(p_file, _T("\n"));
}

void WriteCXHFile(CXClass* p_class)
{
  CString filename = p_class->GetName() + _T("_cxh.cpp");
  FILE* file = nullptr;
  int error = _tfopen_s(&file,filename,_T("w"));
  if (error)
  {
    _tprintf(_T("OS error [%d] opening file: %s\n"),error,filename.GetString());
    return;
  }
  _tprintf(_T("Writing output to: %s\n"),filename.GetString());

  PrintCXHFileHeader (file, p_class);
  PrintCXHDeSerialize(file, p_class,_T("XML_"));
  PrintCXHDeSerialize(file, p_class,_T("XML_DE"));
  PrintCXHDeSerialize(file, p_class,_T("DBS_"));
  PrintCXHDeSerialize(file, p_class,_T("DBS_DE"));
  PrintGenDeSerialize(file, p_class);
  PrintCXHFactory    (file, p_class);

  fclose(file);
}

CString GetParameters(int argc,TCHAR** argv)
{
  int index = 1;
  CString classname;
  bool error = false;

  while(index < argc)
  {
    CString param(argv[index]);

    if(param.GetAt(0) == _T('/'))
    {
      if(param.Left(8).CompareNoCase(_T("/config:")) == 0)
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
    _tprintf(_T("USAGE: cfg2cpp [/config:hibernate.cfg.xml] classname\n"));
    exit(-3);
  }
  return classname;
}

int _tmain(int argc,TCHAR** argv)
{
  _tprintf(_T("CFG2CPP Hibernate.cfg.xml to *.cpp file converter\n"));
  _tprintf(_T("Copyright (c) ir. W.E. Huisman\n"));
  _tprintf(_T("Version: %s of %s\n"), HIBERNATE_VERSION,HIBERNATE_DATE);
  _tprintf(_T("\n"));

  CString classname = GetParameters(argc,argv);

  _tprintf(_T("Creating session\n"));
  _tprintf(_T("Reading configuration file: %s\n"),g_configFile.GetString());

  // Tools: set incomplete mode!!
  hibernate.SetIncomplete(true);

  try
  {
    // Request a session
    CXSession* session = hibernate.CreateSession(_T(""),g_configFile);
    if(session)
    {
      _tprintf(_T("Finding our class: %s\n"),classname.GetString());
      CXClass* cl = session->FindClass(classname);
      if(cl)
      {
        WriteInterface(cl);
        WriteCPPFile(cl);
        WriteCXHFile(cl);
      }
      else
      {
        _tprintf(_T("ERROR: Cannot find classname: %s\n"),classname.GetString());
      }
      session->CloseSession();
    }
    else
    {
      _tprintf(_T("ERROR: Cannot create a hibernate session\n"));
      _tprintf(_T("Have you defined a correct 'hibernate.cfg.xml' file?\n"));
    }
  }
  catch(StdException& ex)
  {
    _tprintf(_T("ERROR: %s\n"),ex.GetErrorMessage().GetString());
  }
  return 0;
}
