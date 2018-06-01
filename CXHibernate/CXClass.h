////////////////////////////////////////////////////////////////////////
//
// File: CXClass.h
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
// Last Revision:   27-05-2018
// Version number:  0.5.0
//
#pragma once
#include "CXHibernate.h"
#include "CXAttribute.h"
#include "CXTable.h"
#include "CXObject.h"
#include <vector>

using SubClasses = std::vector<CXClass*>;

class CXClass
{
public:
  CXClass(CString   p_name
         ,CString   p_discriminator = ""
         ,CXClass*  p_super  = nullptr
         ,CreateCXO p_create = nullptr);
 ~CXClass();

  // The name of the game
  CString     GetName();
  // Object factory of this table
  CreateCXO   GetCreateCXO();
  // Getting our underlying database table
  CXTable*    GetTable();
  // Getting the superclass
  CXClass*    GetSuperClass();
  // Getting our subclasses
  SubClasses& GetSubClasses();

  // Add attributes to the class
  void        AddAttribute  (CXAttribute*  p_attribute);
  void        AddIdentity   (CXPrimaryKey& p_primary);
  void        AddAssociation(CXForeignKey* p_key);
  void        AddIndex      (CXIndex*      p_index);
  void        AddGenerator  (CString       p_generator,int p_start);
  void        AddPrivilege  (CXAccess&     p_access);
  // Find an attribute
  CXAttribute* FindAttribute(CString p_name);
  CXAttribute* FindAttribute(int     p_index);
  // Find the generator attribute (if any)
  CXAttribute* FindGenerator();

  // Serialize to a configuration XML file
  bool        SaveMetaInfo(XMLMessage& p_message,XMLElement* p_elem);
  // DeSerialize from a XML configuration file
  bool        LoadMetaInfo(CXSession* p_session,XMLMessage& p_message,XMLElement* p_elem);
  // After loading we do the linking
  void        LinkClasses(CXSession* p_session);

  // Build default SELECT query
  bool        BuildDefaultSelectQuery(SQLInfoDB* p_info);
  // Load filters in message for an internet selection
  void        BuildPrimaryKeyFilter(SOAPMessage& p_message,XMLElement* p_entity,VariantSet& p_primary);


protected:
  // Adding a sub-class
  void        AddSubClass(CXClass* p_subclass);
  // Fill in our underlying table
  void        FillTableInfoFromClassInfo();

  // SAVING CONFIGURATION INFO
  void SaveMetaInfoSubClasses  (XMLMessage& p_message,XMLElement* p_theClass);
  void SaveMetaInfoAttributes  (XMLMessage& p_message,XMLElement* p_theClass);
  void SaveMetaInfoIdentity    (XMLMessage& p_message,XMLElement* p_theClass);
  void SaveMetaInfoAssociations(XMLMessage& p_message,XMLElement* p_theClass);
  void SaveMetaInfoIndices     (XMLMessage& p_message,XMLElement* p_theClass);
  void SaveMetaInfoGenerator   (XMLMessage& p_message,XMLElement* p_theClass);
  void SaveMetaInfoPrivileges  (XMLMessage& p_message,XMLElement* p_theClass);

  // LOAD THE CONFIGURATION INFO
  void LoadMetaInfoSubClasses  (XMLMessage& p_message,XMLElement* p_theClass);
  void LoadMetaInfoAttributes  (XMLMessage& p_message,XMLElement* p_theClass);
  void LoadMetaInfoIdentity    (XMLMessage& p_message,XMLElement* p_theClass);
  void LoadMetaInfoAssociations(XMLMessage& p_message,XMLElement* p_theClass);
  void LoadMetaInfoIndices     (XMLMessage& p_message,XMLElement* p_theClass);
  void LoadMetaInfoGenerator   (XMLMessage& p_message,XMLElement* p_theClass);
  void LoadMetaInfoPrivileges  (XMLMessage& p_message,XMLElement* p_theClass);

private:
  // The name of our class
  CString       m_name;
  // Discriminator of our class
  CString       m_discriminator;
  // In case of a sub-class, this is our super-class
  CXClass*      m_super { nullptr };
  // In case of a super-class, these are our sub-classes
  SubClasses    m_subClasses;
  WordList      m_subNames;
  // Our CXObject factory function
  CreateCXO     m_create { nullptr };
  // Underlying database table (in current mapping strategy)
  CXTable*      m_table;
  // All attributes of this class
  CXAttribMap   m_attributes;   // Column attributes
  CXPrimaryKey  m_primary;      // Our primary key
  CXForeigns    m_foreigns;     // Foreign key associations
  CXIndices     m_indices;      // Constraints and performance speed-ups
  CString       m_generator;    // Generator name
  int           m_gen_value;    // Initial generator value
  CXPrivileges  m_privileges;   // All access rights
};
