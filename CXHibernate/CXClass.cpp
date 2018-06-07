////////////////////////////////////////////////////////////////////////
//
// File: CXClass.cpp
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
#include "stdafx.h"
#include "CXClass.h"
#include <algorithm>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CXClass::CXClass(CString    p_name
                ,CString    p_discriminator
                ,CXClass*   p_super
                ,CreateCXO  p_create)
        :m_name(p_name)
        ,m_discriminator(p_discriminator)
        ,m_create(p_create)
        ,m_super(p_super)
        ,m_table(nullptr)
{
  // Registering with our super-class
  if(m_super)
  {
    m_super->AddSubClass(this);
  }

  // Find our object factory
  if(p_create == nullptr)
  {
    m_create = hibernate.FindCreateCXO(p_name);
  }

  // Getting a database table
  if(m_super == nullptr || hibernate.GetStrategy() != Strategy_one_table)
  {
    m_table = new CXTable(p_name);
  }
}

CXClass::~CXClass()
{
  // Remove underlying table
  if(m_table)
  {
    delete m_table;
    m_table = nullptr;
  }

  // Clear our attributes
  for(auto& attrib : m_attributes)
  {
    delete attrib;
  }

  // Clear our associations
  for(auto& fkey : m_associations)
  {
    delete fkey;
  }

  // Clear the indices
  for(auto& index : m_indices)
  {
    delete index;
  }
}

// The name of the game
CString
CXClass::GetName()
{
  return m_name;
}

// Object factory of this class
CreateCXO
CXClass::GetCreateCXO()
{
  return m_create;
}

// Getting our underlying database table
CXTable*
CXClass::GetTable()
{
  if(m_super && hibernate.GetStrategy() == Strategy_one_table)
  {
    return m_super->GetTable();
  }
  return m_table;
}

// Getting the superclass
CXClass*
CXClass::GetSuperClass()
{
  return m_super;
}

// Getting our subclasses
SubClasses&
CXClass::GetSubClasses()
{
  return m_subClasses;
}

// Getting the identity
CXPrimaryKey&
CXClass::GetIdentity()
{
  return m_primary;
}

// Add an attribute to the class
void
CXClass::AddAttribute(CXAttribute* p_attribute)
{
  CString name = p_attribute->GetName();
  if(FindAttribute(name))
  {
    throw new StdException("Duplicate attribute cannot be added to class: " + name);
  }
  // Just to be sure
  p_attribute->SetClass(this);
  // Keep this attribute
  m_attributes.push_back(p_attribute);
}

void
CXClass::AddIdentity(CXPrimaryKey& p_primary)
{
  m_primary = p_primary;
}

void
CXClass::AddAssociation(CXAssociation* p_assoc)
{
  m_associations.push_back(p_assoc);
}

void
CXClass::AddIndex(CXIndex* p_index)
{
  m_indices.push_back(p_index);
}

void
CXClass::AddGenerator(CString p_generator, int p_start)
{
  m_generator = p_generator;
  m_gen_value = p_start;
}

void
CXClass::AddPrivilege(CXAccess& p_access)
{
  m_privileges.push_back(p_access);
}

// Find an attribute
CXAttribute* 
CXClass::FindAttribute(CString p_name)
{
  for (auto& attrib : m_attributes)
  {
    if(p_name.CompareNoCase(attrib->GetName()) == 0)
    {
      return attrib;
    }
  }
  // Recursively ask our super classes
  if(m_super)
  {
    return m_super->FindAttribute(p_name);
  }
  return nullptr;
}

CXAttribute*
CXClass::FindAttribute(int p_index)
{
  if (p_index >= 0 && p_index < (int)m_attributes.size())
  {
    return m_attributes[p_index];
  }
  return nullptr;
}

// Find all the attribute names of all my attributes
// including all the attributes of my superclass(es)
WordList
CXClass::FindAllDBSAttributes()
{
  WordList list;

  // Gather names of all superclasses recursively
  if(m_super)
  {
    list = m_super->FindAllDBSAttributes();
  }

  // Add all my database attributes
  for(auto& attrib : m_attributes)
  {
    list.push_back(attrib->GetDatabaseColumn());
  }

  return list;
}


// Find the generator (if any)
CXAttribute* 
CXClass::FindGenerator()
{
  // Walk the list of attributes
  for(auto& attrib : m_attributes)
  {
    if(attrib->GetIsGenerator())
    {
      return attrib;
    }
  }
  // Recursively ask our super classes
  if(m_super)
  {
    return m_super->FindGenerator();
  }
  return nullptr;
}

// Find an association
CXAssociation* 
CXClass::FindAssociation(CString p_toClass,CString p_associationName)
{
  CXAssociation* found = nullptr;

  for(auto& assoc : m_associations)
  {
    if(!p_associationName.IsEmpty() && assoc->m_constraintName.CompareNoCase(p_associationName) == 0)
    {
      // Definitely our association
      return assoc;
    }
    if(assoc->m_primaryTable.CompareNoCase(p_toClass) == 0)
    {
      // If table is found more than once, you must specify the association name!
      if(found) found = nullptr;
      else      found = assoc;
    }
  }
  return found;
}

CXAssociation* 
CXClass::FindAssociation(int index)
{
  if(index >= 0 && index < (int)m_associations.size())
  {
    return m_associations[index];
  }
  return nullptr;
}

// Serialize to a configuration XML file
bool
CXClass::SaveMetaInfo(XMLMessage& p_message,XMLElement* p_elem)
{
  XMLElement* theclass = p_message.AddElement(p_elem,"class",XDT_String,"");

  // Class attributes
  p_message.AddElement(theclass,"name",XDT_String,m_name);
  if(!m_discriminator.IsEmpty())
  {
    p_message.AddElement(theclass,"discriminator",XDT_String,m_discriminator);
  }
  if(m_super)
  {
    p_message.AddElement(theclass,"super",XDT_String,m_super->GetName());
  }
  if(m_table)
  {
    p_message.AddElement(theclass,"table",XDT_String,m_table->TableName());
  }

  // Add subclass references
  SaveMetaInfoSubClasses(p_message,theclass);

  // All other class info
  SaveMetaInfoAttributes  (p_message,theclass);
  SaveMetaInfoIdentity    (p_message,theclass);
  SaveMetaInfoAssociations(p_message,theclass);
  SaveMetaInfoIndices     (p_message,theclass);
  SaveMetaInfoGenerator   (p_message,theclass);
  SaveMetaInfoPrivileges  (p_message,theclass);

  return true;
}


// DeSerialize from a XML configuration file
bool
CXClass::LoadMetaInfo(CXSession* p_session,XMLMessage& p_message,XMLElement* p_elem)
{
  // Load underlying table name
  if(m_table)
  {
    CString schemaName = p_message.GetElement(p_elem,"schema");
    CString tableName  = p_message.GetElement(p_elem,"table");
    m_table->SetSchemaTableType(schemaName,tableName,"TABLE");
  }
  // Load subclasses
  LoadMetaInfoSubClasses(p_message,p_elem);

  // Load all other meta info
  LoadMetaInfoAttributes  (p_message,p_elem);
  LoadMetaInfoIdentity    (p_message,p_elem);
  LoadMetaInfoAssociations(p_message,p_elem,p_session);
  LoadMetaInfoIndices     (p_message,p_elem);
  LoadMetaInfoGenerator   (p_message,p_elem);
  LoadMetaInfoPrivileges  (p_message,p_elem);

  // Now build our table
  BuildClassTable(p_session);

  return !m_attributes.empty();
}

// Build default SELECT query
bool
CXClass::BuildDefaultSelectQuery(SQLInfoDB* p_info)
{
  CString  columns("SELECT ");
  CString  asalias  = " as " + m_discriminator;
  WordList dbsNames = FindAllDBSAttributes();
  bool     firstdone = false;

  for(auto& dbsname : dbsNames)
  {
    if(firstdone)
    {
      columns += "      ,";
    }
    columns  += m_discriminator + "." + dbsname + "\n";
    firstdone = true;
  }

  // Default query will be built as
  // "SELECT disc.columnname\n"
  // "  FROM table as disc"
  CString query = columns + "  FROM " + GetTable()->DMLTableName(p_info) + asalias;

  // Set on the dataset
  if(GetTable())
  {
    SQLDataSet* set = GetTable()->GetDataSet();
    if(set)
    {
      set->SetQuery(query);
      return true;
    }
  }
  return false;
}

// Load filters in message for an internet selection
void
CXClass::BuildPrimaryKeyFilter(SOAPMessage& p_message,XMLElement* p_entity,VariantSet& p_primary)
{
  // Check number of primaries against VariantSet
  if(m_primary.m_attributes.size() != p_primary.size())
  {
    throw new StdException("Primary key size mismatches the load key");
  }
  if(p_primary.empty())
  {
    throw new StdException("Cannot select an object without a filter");
  }

  // All filters under this node
  XMLElement* filters = p_message.AddElement(p_entity,"Filters",XDT_String,"");

  // Prepare for iteration
  CXAttribMap::iterator att = m_primary.m_attributes.begin();
  VariantSet::iterator  key = p_primary.begin();

  while(att != m_primary.m_attributes.end())
  {
    int     xmltype = XDT_String | ODBCToXmlDataType((*key)->GetDataType());
    CString column  = (*att)->GetDatabaseColumn();
    CString operat  = SQLOperatorToString(OP_Equal);
    CString value;
    (*key)->GetAsString(value);

    XMLElement* filter = p_message.AddElement(filters,"Filter",XDT_String,"");
    p_message.AddElement(filter,"Column",  XDT_String,column);
    p_message.AddElement(filter,"Operator",XDT_String,operat);
    p_message.AddElement(filter,"Value",   xmltype,   value);

    // Next attribute
    ++att;
    ++key;
  }
}

// Build filter for primary key or association selection
void
CXClass::BuildFilter(CXAttribMap& p_attributes,VariantSet& p_values,SQLFilterSet& p_filters)
{
  if(p_attributes.size() != p_values.size())
  {
    throw new StdException("Attributes / values mismatch in building an SQL Filter set");
  }
  CXAttribMap::iterator att = p_attributes.begin();
  VariantSet::iterator  val = p_values.begin();

  while(att != p_attributes.end())
  {
    SQLFilter filter((*att)->GetDatabaseColumn(),SQLOperator::OP_Equal,*val);
    p_filters.AddFilter(filter);
    // Next attribute
    ++att;
    ++val;
  }
}

// Call after adding everything. Also called by LoadMetaInfo
void
CXClass::BuildClassTable(CXSession* p_session)
{
  // See if we need the discriminator in the table
  if(hibernate.GetStrategy() != MapStrategy::Strategy_standalone)
  {
    CXAttribute* attrib = new CXAttribute("string","discriminator",3);
    m_attributes.push_back(attrib);
  }

  // See if we must copy the identity to our class/table
  if((hibernate.GetStrategy() == MapStrategy::Strategy_sub_tables) ||
     (hibernate.GetStrategy() == MapStrategy::Strategy_classtable)  )
  {
    // But only if we are NOT the superclass
    if(m_super)
    {
      CXClass* super = GetSuperClass();
      while(super->GetSuperClass())
      {
        super = super->GetSuperClass();
      }
      m_primary = super->GetIdentity();
    }
  }

  // Fill in the table info
  FillTableInfoFromClassInfo();

}


//////////////////////////////////////////////////////////////////////////
//
// PROTECTED
//
//////////////////////////////////////////////////////////////////////////

// Adding a sub-class
void
CXClass::AddSubClass(CXClass* p_subclass)
{
  SubClasses::iterator it = std::find(m_subClasses.begin(),m_subClasses.end(),p_subclass);
  if(it == m_subClasses.end())
  {
    m_subClasses.push_back(p_subclass);
  }
}

// SAVING THE CONFIGURATION

// Saving all references to all subclasses of the class
void 
CXClass::SaveMetaInfoSubClasses(XMLMessage& p_message,XMLElement* p_theClass)
{
  if(!m_subClasses.empty())
  {
    XMLElement* subs = p_message.AddElement(p_theClass,"subclasses",XDT_String,"");
    for(auto& sub : m_subClasses)
    {
      p_message.AddElement(subs,"subclass",XDT_String,sub->GetName());
    }
  }
}

// Saving all the attributes of the class
void 
CXClass::SaveMetaInfoAttributes(XMLMessage& p_message,XMLElement* p_theClass)
{
  XMLElement* attribs = p_message.AddElement(p_theClass,"attributes",XDT_String,"");
  for(auto& attr : m_attributes)
  {
    attr->SaveMetaInfo(p_message,attribs);
  }
}

// Saving the identity of an object in the class
void 
CXClass::SaveMetaInfoIdentity(XMLMessage& p_message,XMLElement* p_theClass)
{
  if(!m_primary.m_attributes.empty())
  {
    XMLElement* primary = p_message.AddElement(p_theClass,"identity",XDT_String,"");
    p_message.SetAttribute(primary,"name",m_primary.m_constraintName);
    CString deferrable;
    switch(m_primary.m_deferrable)
    {
      case SQL_INITIALLY_DEFERRED:    deferrable = "initially_deferred";  break;
      case SQL_INITIALLY_IMMEDIATE:   deferrable = "initially_immediate"; break;
      case SQL_NOT_DEFERRABLE:        deferrable = "not_deferrable";      break;
    }
    p_message.SetAttribute(primary,"deferrable",deferrable);
    p_message.SetAttribute(primary,"initially_deferred",m_primary.m_initiallyDeferred ? "deferred" : "immediate");

    // Add the columns of the primary key
    for(auto& col : m_primary.m_attributes)
    {
      XMLElement* column = p_message.AddElement(primary,"attribute",XDT_String,"");
      p_message.SetAttribute(column,"name",col->GetDatabaseColumn());
    }
  }
}

// Saving the associations to other classes
void 
CXClass::SaveMetaInfoAssociations(XMLMessage& p_message,XMLElement* p_theClass)
{
  XMLElement* associations = p_message.AddElement(p_theClass,"associations",XDT_String,"");
  for(auto& assoc : m_associations)
  {
    XMLElement* ass = p_message.AddElement(associations,"association",XDT_String,"");
    p_message.SetAttribute(ass,"name",assoc->m_constraintName);
    p_message.SetAttribute(ass,"type",CXAssocTypeToSTring(assoc->m_assocType));
    // update / delete / deferrable / match / initially-deferred / enabled
    p_message.AddElement(ass,"association_class",XDT_String,assoc->m_primaryTable);
    for(auto& col : assoc->m_attributes)
    {
      XMLElement* column = p_message.AddElement(ass,"attribute",XDT_String,"");
      p_message.SetAttribute(column,"name",col->GetDatabaseColumn());
    }
  }
}

// Saving the unique constraints and lookup indices
void 
CXClass::SaveMetaInfoIndices(XMLMessage& p_message,XMLElement* p_theClass)
{
  XMLElement* indices = p_message.AddElement(p_theClass,"indices",XDT_String,"");
  for(auto& ind : m_indices)
  {
    XMLElement* index = p_message.AddElement(indices,"index",XDT_String,"");
    p_message.SetAttribute(index,"name",      ind->m_name);
    p_message.SetAttribute(index,"unique",    ind->m_unique);
    p_message.SetAttribute(index,"ascending", ind->m_ascending);
    for(auto& attrib : ind->m_attributes)
    {
      XMLElement* attr = p_message.AddElement(index,"attribute",XDT_String,"");
      p_message.SetAttribute(attr,"name",attrib->GetName());
    }
    if(!ind->m_filter.IsEmpty())
    {
      p_message.AddElement(index,"filter",XDT_String|XDT_CDATA,ind->m_filter);
    }
  }
}

// Saving the identity generator information
void 
CXClass::SaveMetaInfoGenerator(XMLMessage& p_message,XMLElement* p_theClass)
{
  if(!m_generator.IsEmpty())
  {
    XMLElement* generator = p_message.AddElement(p_theClass, "generator", XDT_String, "");
    p_message.SetAttribute(generator,"name", m_generator);
    p_message.SetAttribute(generator,"start",m_gen_value);
  }
}

// Saving the access privileges of the class
void 
CXClass::SaveMetaInfoPrivileges(XMLMessage& p_message,XMLElement* p_theClass)
{
  XMLElement* access = p_message.AddElement(p_theClass,"access",XDT_String,"");
  for(auto& priv : m_privileges)
  {
    XMLElement* user = p_message.AddElement(access,"user",XDT_String,"");
    p_message.SetAttribute(user,"name",  priv.m_grantee);
    p_message.SetAttribute(user,"rights",priv.m_privilege);
    if(priv.m_grantable)
    {
      p_message.SetAttribute(user,"grantable",true);
    }
  }
}

// LOADING THE CONFIGURATION

// Loading the names to the subclass references only
// The real linking will be done after the configuration is fully loaded!
void 
CXClass::LoadMetaInfoSubClasses(XMLMessage& p_message,XMLElement* p_theClass)
{
  XMLElement* subs = p_message.FindElement(p_theClass,"subclasses");
  if(subs)
  {
    XMLElement* sub = p_message.FindElement(subs,"subclass");
    while(sub)
    {
      m_subNames.push_back(sub->GetValue());
      sub = p_message.GetElementSibling(sub);
    }
  }
}

// Loading the class attributes
void 
CXClass::LoadMetaInfoAttributes(XMLMessage& p_message,XMLElement* p_theClass)
{
  XMLElement* attribs = p_message.FindElement(p_theClass,"attributes");
  if(attribs)
  {
    XMLElement* attrib = p_message.FindElement(attribs,"attribute");
    while(attrib)
    {
      // Create the attribute
      CString name = p_message.GetAttribute(attrib,"name");
      CXAttribute* attribute = new CXAttribute(name);

      // Load the attribute
      attribute->LoadMetaInfo(p_message,attrib);

      // Keep the attribute
      m_attributes.push_back(attribute);
      // Next attributes
      attrib = p_message.GetElementSibling(attrib);
    }
  }
}

// Loading the primary key identity info
void 
CXClass::LoadMetaInfoIdentity(XMLMessage& p_message,XMLElement* p_theClass)
{
  XMLElement* primary = p_message.FindElement(p_theClass,"identity");
  if(primary)
  {
    m_primary.m_constraintName = p_message.GetAttribute(primary,"name");
    CString deferr  = p_message.GetAttribute(primary,"deferrable");
    CString initdef = p_message.GetAttribute(primary,"initially_deferred");
    if(deferr.CompareNoCase("initailly_deferred") == 0) m_primary.m_deferrable = SQL_INITIALLY_DEFERRED;
    if(deferr.CompareNoCase("initially_immediate")== 0) m_primary.m_deferrable = SQL_INITIALLY_IMMEDIATE;
    if(deferr.CompareNoCase("not_deferrable") == 0)     m_primary.m_deferrable = SQL_NOT_DEFERRABLE;
    m_primary.m_initiallyDeferred = initdef.CompareNoCase("deferred") == 0;

    XMLElement* column = p_message.FindElement(primary,"attribute");
    while(column)
    {
      CString name = p_message.GetAttribute(column,"name");
      CXAttribute* attrib = FindAttribute(name);
      if(attrib)
      {
        m_primary.m_attributes.push_back(attrib);
      }
      column = p_message.GetElementSibling(column);
    }
  }
}

// Loading the associations to other classes
void 
CXClass::LoadMetaInfoAssociations(XMLMessage& p_message,XMLElement* p_theClass,CXSession* p_session)
{
  XMLElement* assocs = p_message.FindElement(p_theClass,"associations");
  if(assocs)
  {
    XMLElement* assoc = p_message.FindElement(assocs,"association");
    while(assoc)
    {
      CXAssociation* ass = new CXAssociation();
      ass->m_assocType      = CXStringToAssocType(p_message.GetAttribute(assoc,"type"));
      ass->m_constraintName = p_message.GetAttribute(assoc,"name");
      ass->m_primaryTable   = p_message.GetElement(assoc,"association_class");

      XMLElement* column = p_message.FindElement(assoc,"attribute");
      while(column)
      {
        CString name = p_message.GetAttribute(column,"name");
        CXAttribute* attrib = FindAttribute(name);
        if(attrib)
        {
          ass->m_attributes.push_back(attrib);
        }
        else
        {
          CXClass* related = p_session->FindClass(ass->m_primaryTable);
          attrib = related->FindAttribute(name);
          if (attrib)
          {
            ass->m_attributes.push_back(attrib);
          }
        }
        column = p_message.GetElementSibling(column);
      }
      // Keep this foreign key
      m_associations.push_back(ass);
      // Next foreign key
      assoc = p_message.GetElementSibling(assoc);
    }
  }
}

// Loading unique constraints and lookup indices
void 
CXClass::LoadMetaInfoIndices(XMLMessage& p_message,XMLElement* p_theClass)
{
  XMLElement* indices = p_message.FindElement(p_theClass,"indices");
  if(indices)
  {
    XMLElement* index = p_message.FindElement(indices,"index");
    while (index)
    {
      CXIndex* ind = new CXIndex();
      ind->m_name      = p_message.GetAttribute(index,"name");
      ind->m_unique    = p_message.GetAttributeBoolean(index,"unique");
      ind->m_ascending = p_message.GetAttributeBoolean(index,"ascending");
      ind->m_filter    = p_message.GetElement(index,"filter");

      XMLElement* attr = p_message.FindElement(index,"attribute");
      while(attr)
      {
        CString name = p_message.GetAttribute(attr,"name");
        CXAttribute* attribute = FindAttribute(name);
        if (attribute)
        {
          ind->m_attributes.push_back(attribute);
        }
        attr = p_message.GetElementSibling(attr);
      }

      // Keep the index
      m_indices.push_back(ind);
      // next index
      index = p_message.GetElementSibling(index);
    }
  }
}

// Loading the generator for the identity
void 
CXClass::LoadMetaInfoGenerator(XMLMessage& p_message,XMLElement* p_theClass)
{
  XMLElement* generator = p_message.FindElement(p_theClass,"generator");
  if(generator)
  {
    m_generator = p_message.GetAttribute(generator,"name");
    m_gen_value = p_message.GetAttributeInteger(generator,"start");
  }
}

// Loading access privileges of the class
void 
CXClass::LoadMetaInfoPrivileges(XMLMessage& p_message,XMLElement* p_theClass)
{
  XMLElement* access = p_message.FindElement(p_theClass,"access");
  if(access)
  {
    XMLElement* user = p_message.FindElement(access,"user");
    while(user)
    {
      CXAccess acc;
      acc.m_grantee   = p_message.GetAttribute(user,"name");
      acc.m_privilege = p_message.GetAttribute(user,"rights");
      acc.m_grantable = p_message.GetAttributeBoolean(user,"grantable");

      m_privileges.push_back(acc);
      // Next privilege
      user = p_message.GetElementSibling(user);
    }
  }
}

// After loading we do the linking
void 
CXClass::LinkClasses(CXSession* p_session)
{
  for(auto& sub : m_subNames)
  {
    CXClass* subclass = p_session->FindClass(sub);
    if(subclass)
    {
      m_subClasses.push_back(subclass);
    }
  }
}


// Fill in our underlying table
void
CXClass::FillTableInfoFromClassInfo()
{
  int position = 1;

  // Add column info
  for(auto& attrib : m_attributes)
  {
    MetaColumn column;
    column.m_table      = GetTable()->TableName();
    column.m_column     = attrib->GetDatabaseColumn();
    column.m_datatype   = attrib->GetDataType();
    column.m_columnSize = attrib->GetMaxLength();
    column.m_position   = position++;

    GetTable()->AddInfoColumn(column);
  }

  // Add primary key info
  position = 1;
  for(auto& attrib : m_primary.m_attributes)
  {
    MetaPrimary prim;
    prim.m_table             = GetTable()->TableName();
    prim.m_columnName        = attrib->GetDatabaseColumn();
    prim.m_columnPosition    = position++;
    prim.m_constraintName    = m_primary.m_constraintName;
    prim.m_deferrable        = m_primary.m_deferrable;
    prim.m_initiallyDeferred = m_primary.m_initiallyDeferred;

    GetTable()->AddPrimaryKey(prim);
  }

  // Add foreign keys
  for(auto& foreign : m_associations)
  {
    position = 1;
    for(auto& col : foreign->m_attributes)
    {
      MetaForeign fkey;

      fkey.m_fkSchemaName      = GetTable()->SchemaName();
      fkey.m_fkTableName       = GetTable()->TableName();
      fkey.m_pkTableName       = foreign->m_primaryTable;
      fkey.m_foreignConstraint = foreign->m_constraintName;
      fkey.m_keySequence       = position++;
      fkey.m_fkColumnName      = col->GetDatabaseColumn();

      GetTable()->AddForeignKey(fkey);
    }
  }

  // Add indices
  for(auto& index : m_indices)
  {
    position = 1;
    for(auto& col : index->m_attributes)
    {
      MetaIndex ind;
      ind.m_schemaName = GetTable()->SchemaName();
      ind.m_tableName  = GetTable()->TableName();
      ind.m_indexName  = index->m_name;
      ind.m_columnName = col->GetDatabaseColumn();
      ind.m_unique     = index->m_unique;
      ind.m_ascending  = index->m_ascending ? 'A' : 'D';
      ind.m_position   = position++;
      ind.m_filter     = index->m_filter;

      GetTable()->AddIndex(ind);
    }
  }

  // Add generator / sequence
  if(!m_generator.IsEmpty())
  {
    MetaSequence seq;
    seq.m_schemaName   = GetTable()->SchemaName();
    seq.m_sequenceName = m_generator;
    seq.m_currentValue = m_gen_value;

    GetTable()->AddSequence(seq);
  }

  // Add privileges
  for(auto& access : m_privileges)
  {
    MetaPrivilege priv;
    priv.m_schemaName = GetTable()->SchemaName();
    priv.m_tableName  = GetTable()->TableName();
    priv.m_grantee    = access.m_grantee;
    priv.m_privilege  = access.m_privilege;
    priv.m_grantable  = access.m_grantable;

    GetTable()->AddPrivilege(priv);
  }
}


