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
// Last version date: See CXHibernate.h
// Version number:    See CXHibernate.h
//
#pragma once
#include "CXHibernate.h"
#include "CXAttribute.h"
#include "CXTable.h"
#include "CXObject.h"
#include <vector>

// A vector with all our subclasses
using SubClasses = std::vector<CXClass*>;
// Override functon to create a hashcode for an object in this class;
typedef CString (*CalcHash)(VariantSet& p_primary);


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
  // Getting the root class
  CXClass*    GetRootClass();
  // Getting our main identity
  CXIdentity& GetIdentity();
  // Getting our discriminator value
  CString     GetDiscriminator();
  // Are we the root class of an hierarchy?
  bool        GetIsRootClass();
  // Primary data set
  SQLDataSet* GetDataSet();
  // Our function to calculate an override of a hash code for an object
  CalcHash    GetCalcHashcode();

  // Add attributes to the class
  void        AddAttribute  (CXAttribute*   p_attribute);
  void        AddIdentity   (CXIdentity&  p_primary);
  void        AddAssociation(CXAssociation* p_key);
  void        AddIndex      (CXIndex*       p_index);
  void        AddGenerator  (CString        p_generator,int p_start);
  void        AddPrivilege  (CXAccess&      p_access);
  // Register a CalcHashcode function
  void        RegisterCalcHash(CalcHash p_calcHashcode);
  // Find an attribute
  CXAttribute*   FindAttribute(CString p_name);
  CXAttribute*   FindAttribute(int     p_index);
  WordList       FindAllDBSAttributes(bool p_superIncluded);
  // Find an association
  CXAssociation* FindAssociation(CString p_toClass,CString p_associationName);
  CXAssociation* FindAssociation(int index);
  // Find the generator attribute (if any)
  CXAttribute*   FindGenerator();

  // Serialize to a configuration XML file
  bool        SaveMetaInfo(XMLMessage& p_message,XMLElement* p_elem);
  // DeSerialize from a XML configuration file
  bool        LoadMetaInfo(CXSession* p_session,XMLMessage& p_message,XMLElement* p_elem);
  // After loading we do the linking
  void        LinkClasses(CXSession* p_session);
  // Call after adding everything. Also called by LoadMetaInfo
  void        BuildClassTable(CXSession* p_session);

  // Build default SELECT query
  void        BuildDefaultSelectQuery(SQLDataSet* p_dataset,SQLInfoDB* p_info);
  // Load filters in message for an internet selection
  void        BuildPrimaryKeyFilter(SOAPMessage& p_message,XMLElement* p_entity,VariantSet& p_primary);
  // Build filter for primary key or association selection
  void        BuildFilter(CXAttribMap& p_attributes,VariantSet& p_values,SQLFilterSet& p_filters);

  // THE DATABASE INTERFACE

  // First level operations
  SQLRecord*  SelectObjectInDatabase(SQLDatabase* p_database,SQLDataSet* p_dataset,VariantSet& p_set);
  bool        InsertObjectInDatabase(SQLDatabase* p_database,SQLDataSet* p_dataset,CXObject* p_object,int p_mutation);
  bool        UpdateObjectInDatabase(SQLDatabase* p_database,SQLDataSet* p_dataset,CXObject* p_object,int p_mutation);
  bool        DeleteObjectInDatabase(SQLDatabase* p_database,SQLDataSet* p_dataset,CXObject* p_object,int p_mutation);
  // Second level operations
  bool        REALInsertObjectInDatabase(SQLDatabase* p_database,SQLDataSet* p_dataset,CXObject* p_object,int p_mutation,bool p_root);
  bool        REALUpdateObjectInDatabase(SQLDatabase* p_database,SQLDataSet* p_dataset,CXObject* p_object,int p_mutation);
  bool        REALDeleteObjectInDatabase(SQLDatabase* p_database,SQLDataSet* p_dataSet,CXObject* p_object,int p_mutation);

  // Late binding of associations
  void        LoadMetaInfoAssociations(XMLMessage& p_message,XMLElement* p_theClass,CXSession* p_session);

protected:
  // Adding a sub-class
  void        AddSubClass(CXClass* p_subclass);
  // Fill in our underlying table
  void        FillTableInfoFromClassInfo();
  // Initialize a dataset for the class/table
  void        InitDataSet(SQLDataSet* p_dataSet);
  // Serialize the discriminator value to the database record for this object
  void        SerializeDiscriminator(CXObject* p_object,SQLRecord* p_record,int p_mutation);

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
  void LoadMetaInfoIndices     (XMLMessage& p_message,XMLElement* p_theClass);
  void LoadMetaInfoGenerator   (XMLMessage& p_message,XMLElement* p_theClass);
  void LoadMetaInfoPrivileges  (XMLMessage& p_message,XMLElement* p_theClass);

  CString BuildSelectQueryOneTable(SQLInfoDB* p_info);
  CString BuildSelectQuerySubTable(SQLInfoDB* p_info);
  CString BuildSelectQuerySubclass(SQLInfoDB* p_info);
  void    BuildSelectQuerySubTableRecursive(SQLInfoDB* p_info,CString& p_columns,CString& p_frompart,bool& p_firstdone);

private:
  bool    CreateFilterSet(VariantSet& p_primary,SQLFilterSet& p_filters);
  void    AddDiscriminatorToFilters(SQLFilterSet& p_filters);


  // The name of our class
  CString         m_name;
  // Discriminator of our class
  CString         m_discriminator;
  // In case of a sub-class, this is our super-class
  CXClass*        m_super { nullptr };
  // In case of a super-class, these are our sub-classes
  SubClasses      m_subClasses;
  WordList        m_subNames;
  // Our CXObject factory function
  CreateCXO       m_create { nullptr };
  // Our Hashcode function
  CalcHash        m_calcHashcode { nullptr };
  // Underlying database table (in current mapping strategy!!)
  CXTable*        m_table;
  // Standard data set for selected objects
  SQLDataSet*     m_dataSet { nullptr };
  // All attributes of this class
  CXAttribMap     m_attributes;       // Column attributes
  CXIdentity      m_identity;         // Our candidate primary key
  CXAssociations  m_associations;     // Associations to other classes
  CXIndices       m_indices;          // Constraints and performance speed-ups
  CString         m_generator;        // Generator name
  int             m_gen_value { 0 };  // Initial generator value
  CXPrivileges    m_privileges;       // All access rights
};
