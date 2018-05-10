////////////////////////////////////////////////////////////////////////
//
// File: CXAttribute.h
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
// Last Revision:   22-04-2018
// Version number:  0.0.1
//
#pragma once
#include <map>

class CXClass;
class CXAttribute;
class XMLMessage;
class XMLElement;
using CXAttribMap = std::map<CString,CXAttribute>;

class CXAttribute
{
public:
  CXAttribute(CString p_name);
  CXAttribute(int     p_datatype,CString p_name);
  CXAttribute(CString p_datatype,CString p_name);

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
  bool      SaveMetaInfo(XMLMessage* p_message, XMLElement* p_elem);
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
  CString   m_dbs_column;             // If database column differs from name
  bool      m_notNull;                // NOT NULL constraint in the database
  CString   m_default;                // Default value for the column
};
