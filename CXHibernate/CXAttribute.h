////////////////////////////////////////////////////////////////////////
//
// File: CXAttribute.h
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
#pragma once
#include <sqlext.h>
#include <vector>

class CXClass;
class CXAttribute;
class XMLMessage;
class XMLElement;
using CXAttribMap = std::vector<CXAttribute*>;

// Identity of an object: candidate primary key
// Does sync with the primary key of a table
typedef struct _identity
{
  CString     m_constraintName;
  CXAttribMap m_attributes;
  // SQL_INITIALLY_DEFERRED(5) / SQL_INITIALLY_IMMEDIATE(6) / SQL_NOT_DEFERRABLE(7)
  int         m_deferrable        { SQL_NOT_DEFERRABLE };
  // 0=Immediate, 1=initially deferred
  int         m_initiallyDeferred { 0 };
}
CXIdentity;

typedef enum _assocType
{
  ASSOC_UNKNOWN           // Not yet known association
 ,ASSOC_MANY_TO_ONE       // Regular foreign key to a master
 ,ASSOC_ONE_TO_MANY       // Master sees all details
 ,ASSOC_MANY_TO_MANY      // Assoc table in between
}
CXAssocType;

CXAssocType CXStringToAssocType(CString p_type);
CString CXAssocTypeToSTring(CXAssocType p_type);

typedef struct _assoc
{
  CXAssocType m_assocType;
  CString     m_constraintName;
  CXAttribMap m_attributes;
  CString     m_primaryTable;

  int       m_updateRule{ 1 };          // SQL_CASCADE(0) / SQL_NO_ACTION(3) / SQL_SET_NULL(2) / SQL_SET_DEFAULT(4) / SQL_RESTRICT(1)
  int       m_deleteRule{ 1 };          // SQL_CASCADE(0) / SQL_NO_ACTION(3) / SQL_SET_NULL(2) / SQL_SET_DEFAULT(4) / SQL_RESTRICT(1)
  int       m_deferrable{ 7 };          // SQL_INITIALLY_DEFERRED(5) / SQL_INITIALLY_IMMEDIATE(6) / SQL_NOT_DEFERRABLE(7)
  int       m_match     { 0 };          // SQL_MATCH_FULL(0) / SQL_MATCH_PARTIAL(1) / SQL_MATCH_SIMPLE(2)
  int       m_initiallyDeferred{ 0 };   // 0=Immediate, 1=initially deferred
  int       m_enabled   { 0 };          // 1=Disabled,  0=enabled
}
CXAssociation;

using CXAssociations = std::vector<CXAssociation*>;

typedef struct _index
{
  CString     m_name;
  bool        m_unique    { false };
  bool        m_ascending { true };
  CString     m_filter;
  CXAttribMap m_attributes;
}
CXIndex;

using CXIndices = std::vector<CXIndex*>;

typedef struct _access
{
  CString m_grantee;
  CString m_privilege;
  bool    m_grantable { false };
}
CXAccess;

using CXPrivileges = std::vector<CXAccess>;

//////////////////////////////////////////////////////////////////////////

class CXAttribute
{
public:
  CXAttribute(CString p_name);
  CXAttribute(int     p_datatype,CString p_name);
  CXAttribute(CString p_datatype
             ,CString p_name
             ,int     p_maxlength  = 0
             ,bool    p_generator  = false
             ,bool    p_primary    = false
             ,bool    p_foreign    = false
             ,bool    p_notnull    = false
             ,CString p_dbs_column = ""
             ,CString p_default    = "");
  ~CXAttribute();

  // GETTERS

  CXClass*  GetClass();
  CString   GetName();
  int       GetDataType();
  int       GetMaxLength();
  CString   GetDatabaseColumn();
  bool      GetIsGenerator();
  bool      GetIsPrimary();
  bool      GetIsForeign();
  bool      GetHasNotNull();

  // SETTERS
  void      SetClass(CXClass* p_class);
  void      SetDataType(int     p_datatype);
  void      SetDataType(CString p_datatype);
  void      SetMaxLength(int p_length);
  void      SetDatabaseColumn(CString p_column);
  void      SetGenerator(bool p_generator);
  void      SetIsPrimary(bool p_primary);
  void      SetIsForeign(bool p_foreign);
  void      SetHasNotNull(bool p_notNull);

  // Serialize to a configuration XML file
  bool      SaveMetaInfo(XMLMessage& p_message, XMLElement* p_elem);
  // DeSerialize from a XML configuration file
  bool      LoadMetaInfo(XMLMessage& p_message, XMLElement* p_elem);

private:
  CXClass*  m_class     { nullptr };  // Belongs to this class
  CString   m_name;                   // Name of the class attribute
  int       m_datatype  { 0     };    // Datatype
  int       m_maxlength { 0     };    // Length constraint
  bool      m_generator { false };    // Primary key generator
  bool      m_isPrimary { false };    // Is part of the primary key
  bool      m_isForeign { false };    // Is part of a foreign key
  bool      m_notNull   { false };    // NOT NULL constraint in the database
  CString   m_dbs_column;             // If database column differs from name
  CString   m_default;                // Default value for the column
};
