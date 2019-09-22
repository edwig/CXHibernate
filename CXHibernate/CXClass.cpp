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
// Last version date: See CXHibernate.h
// Version number:    See CXHibernate.h
//
#include "stdafx.h"
#include "CXHibernate.h"
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

  // Destroy our dataset
  if(m_dataSet)
  {
    delete m_dataSet;
    m_dataSet = nullptr;
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

// Getting the root class
CXClass*
CXClass::GetRootClass()
{
  if(m_super)
  {
    return m_super->GetRootClass();
  }
  return this;
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
CXIdentity&
CXClass::GetIdentity()
{
  return m_identity;
}

// Getting our discriminator value
CString
CXClass::GetDiscriminator()
{
  return m_discriminator;
}

// Are we the root class of an hierarchy?
bool
CXClass::GetIsRootClass()
{
  return m_super == nullptr;
}

SQLDataSet*
CXClass::GetDataSet()
{
  if (m_dataSet == nullptr)
  {
    m_dataSet = new SQLDataSet();

    InitDataSet(m_dataSet);
  }
  return m_dataSet;
}

// Add an attribute to the class
void
CXClass::AddAttribute(CXAttribute* p_attribute)
{
  CString name = p_attribute->GetName();
  if(FindAttribute(name))
  {
    CString error;
    error.Format("Duplicate attribute [%s] cannot be added to class: %s",name.GetString(),m_name.GetString());
    throw StdException(error);
  }
  // Just to be sure
  p_attribute->SetClass(this);
  // Keep this attribute
  m_attributes.push_back(p_attribute);
}

void
CXClass::AddIdentity(CXIdentity& p_identity)
{
  m_identity = p_identity;
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
CXClass::FindAllDBSAttributes(bool p_superIncluded)
{
  WordList list;

  // Gather names of all super classes recursively
  if(m_super && p_superIncluded)
  {
    list = m_super->FindAllDBSAttributes(p_superIncluded);
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

// Register a CalcHashcode function (Can only be called once)
void
CXClass::RegisterCalcHash(CalcHash p_calcHashcode)
{
  if(m_calcHashcode == nullptr)
  {
    m_calcHashcode = p_calcHashcode;
  }
}

// Our function to calculate an override of a hash code for an object
CalcHash
CXClass::GetCalcHashcode()
{
  return m_calcHashcode;
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
    p_message.AddElement(theclass,"table",XDT_String,m_table->GetTableName());
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
  // Except for associations which are only done as late binding
  LoadMetaInfoAttributes  (p_message,p_elem);
  LoadMetaInfoIdentity    (p_message,p_elem);
  LoadMetaInfoIndices     (p_message,p_elem);
  LoadMetaInfoGenerator   (p_message,p_elem);
  LoadMetaInfoPrivileges  (p_message,p_elem);

  // Now build our table
  BuildClassTable(p_session);

  return !m_attributes.empty();
}

// Build default SELECT query
void
CXClass::BuildDefaultSelectQuery(SQLDataSet* p_dataset,SQLInfoDB* p_info)
{
  CString query;

  // Calculate our query
  switch(hibernate.GetStrategy())
  {
    case MapStrategy::Strategy_standalone:  // Fall through
    case MapStrategy::Strategy_one_table:   query = BuildSelectQueryOneTable(p_info); break;
    case MapStrategy::Strategy_sub_table:   query = BuildSelectQuerySubTable(p_info); break;
    case MapStrategy::Strategy_classtable:  query = BuildSelectQuerySubclass(p_info); break;
  }

  // Set query on the dataset
  p_dataset->SetQuery(query);
}

// Load filters in message for an internet selection
void
CXClass::BuildPrimaryKeyFilter(SOAPMessage& p_message,XMLElement* p_entity,VariantSet& p_primary)
{
  // Check number of primaries against VariantSet
  if(m_identity.m_attributes.size() != p_primary.size())
  {
    throw StdException("Primary key size mismatches the load key");
  }
  if(p_primary.empty())
  {
    throw StdException("Cannot select an object without a filter");
  }

  // All filters under this node
  XMLElement* filters = p_message.AddElement(p_entity,"Filters",XDT_String,"");

  // Prepare for iteration
  CXAttribMap::iterator att = m_identity.m_attributes.begin();
  VariantSet::iterator  key = p_primary.begin();

  while(att != m_identity.m_attributes.end())
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
    throw StdException("Attributes / values mismatch in building an SQL Filter set");
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
  // See if we need the discriminator in the table (super table only!)
  if(hibernate.GetStrategy() != MapStrategy::Strategy_standalone)
  {
    if(m_super == nullptr)
    {
      CXAttribute* attrib = new CXAttribute("string","discriminator",3);
      AddAttribute(attrib);
    }
  }

  // See if we must copy the identity to our class/table
  if((hibernate.GetStrategy() == MapStrategy::Strategy_sub_table) ||
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
      m_identity = super->GetIdentity();
    }
  }

  // Fill in the table info
  FillTableInfoFromClassInfo();
}

// DATABASE INTERFACE

SQLRecord*
CXClass::SelectObjectInDatabase(SQLDatabase* p_database,SQLDataSet* p_dataset,VariantSet& p_set)
{
  // Make sure we have a dataset
  if(p_dataset == nullptr)
  {
    p_dataset = GetDataSet();
  }
  // Connect our database
  p_dataset->SetDatabase(p_database);

  // Create correct query
  BuildDefaultSelectQuery(p_dataset,p_database->GetSQLInfoDB());

  // Open dataset by the filter of the primary key
  SQLFilterSet fset;
  if(CreateFilterSet(p_set,fset))
  {
    p_dataset->SetFilters(&fset);

    // Open our dataset (and search)
    if(p_dataset->IsOpen() == false)
    {
      p_dataset->Open();
    }
    else
    {
      p_dataset->Append();
    }
    p_dataset->SetFilters(nullptr);
  }
  else return nullptr;

  // See if we have a record
  return p_dataset->FindObjectRecord(p_set);
}

bool
CXClass::InsertObjectInDatabase(SQLDatabase* p_database,SQLDataSet* p_dataset,CXObject* p_object,int p_mutation)
{
  // Make sure we have a dataset
  if(p_dataset == nullptr)
  {
    p_dataset = GetDataSet();
  }

  // Check for a table definition
  if(m_table == nullptr)
  {
    if(hibernate.GetStrategy() == MapStrategy::Strategy_one_table && m_super)
    {
      return GetRootClass()->REALInsertObjectInDatabase(p_database,p_dataset,p_object,p_mutation,true);
    }
    // No legal table definition found
    throw StdException("No table while inserting object into class: " + m_name);
  }

  // Default status for stand alone / single-table classes
  bool inserted = true;

  // Insert into superclass tables first (recursively!)
  if(m_super && (hibernate.GetStrategy() == MapStrategy::Strategy_sub_table))
  {
    inserted = m_super->InsertObjectInDatabase(p_database,nullptr,p_object,p_mutation);
  }

  // If succeeded, insert into our table
  if(inserted)
  {
    return REALInsertObjectInDatabase(p_database,p_dataset,p_object,p_mutation,GetIsRootClass());
  }
  return false;
}

bool
CXClass::REALInsertObjectInDatabase(SQLDatabase* p_database,SQLDataSet* p_dataset,CXObject* p_object,int p_mutation,bool p_root)
{
  SQLRecord* record = p_dataset->InsertRecord();
  SQLVariant zero;

  // Connect our database
  p_dataset->SetDatabase(p_database);

  // See if dataset is empty
  MColumnMap& columns = GetTable()->GetColumnInfo();
  if (p_dataset->GetNumberOfRecords() == 1 && p_dataset->GetNumberOfFields() == 0)
  {
    for (auto& column : columns)
    {
      p_dataset->InsertField(column.m_column, &zero);
    }
  }

  // Set the values on the record
  for (int ind = 0; ind < p_dataset->GetNumberOfFields(); ++ind)
  {
    record->AddField(&zero, true);
  }

  // For a root-class object, set the discriminator and generator
  if(p_root)
  {
    // Now serialize our object with the correct discriminator
    SerializeDiscriminator(p_object, record, p_mutation);

    // Check if we must generate our primary key
    CXAttribute* gen = p_object->GetClass()->GetRootClass()->FindGenerator();
    if (gen && p_object->IsTransient())
    {
      // -1: not found, 0 -> (n-1) is the field number of the generator
      int generator = p_dataset->GetFieldNumber(gen->GetName());
      record->SetGenerator(generator);
    }
  }

  // Now serialize our object with the 'real' values
  p_object->Serialize(*record, p_mutation);
  // Set the record to 'insert-only'
  record->Inserted();

  // Go save the record
  bool saved = p_dataset->Synchronize(p_mutation);

  // Getting the generated value in the primary key
  if (saved && p_root)
  {
    // Re-sync the primary key
    p_object->ResetPrimaryKey();
    // Re-sync the class object
    p_object->DeSerializeGenerator(*record);
  }
  return saved;
}

bool
CXClass::UpdateObjectInDatabase(SQLDatabase* p_database,SQLDataSet* p_dataset,CXObject* p_object,int p_mutation)
{
  // Check that we have a dataset
  if(p_dataset == nullptr)
  {
    p_dataset = GetDataSet();
  }

  // Check for a table definition
  if(m_table == nullptr)
  {
    if(hibernate.GetStrategy() == MapStrategy::Strategy_one_table && m_super)
    {
      return GetRootClass()->REALUpdateObjectInDatabase(p_database,p_dataset,p_object,p_mutation);
    }
    // No legal table definition found
    throw StdException("No table while updating an object of class: " + m_name);
  }

  // Default status for stand alone / single-table classes
  bool updated = true;

  // Update in superclass tables first (recursively!)
  if(m_super && (hibernate.GetStrategy() == MapStrategy::Strategy_sub_table))
  {
    // Select from that table first, so we can do a partial update
    SQLDataSet dataset;
    InitDataSet(&dataset);
    SQLRecord* record = m_super->SelectObjectInDatabase(p_database,&dataset,p_object->GetPrimaryKey());
    if(record)
    {
      try
      {
        // Tell this record that what mutations we have and update it
        AutoObjectRecord temprecord(p_object, record);
        updated = m_super->UpdateObjectInDatabase(p_database,&dataset,p_object,p_mutation);
      }
      catch(StdException& ex)
      {
        hibernate.Log(LOGLEVEL_ERROR,true,"Error updating in super table: %s. Error: %s"
                      ,m_super->GetName().GetString()
                      ,ex.GetErrorMessage().GetString());
      }
    }
  }

  // If succeeded, update into our table
  if(updated)
  {
    return REALUpdateObjectInDatabase(p_database,p_dataset,p_object,p_mutation);
  }
  return false;
}

bool
CXClass::REALUpdateObjectInDatabase(SQLDatabase* p_database,SQLDataSet* p_dataset,CXObject* p_object,int p_mutation)
{
  // Get the record
  SQLRecord* record = p_object->GetDatabaseRecord();

  // Setting allowed updatable columns
  WordList list = GetTable()->GetAttributesAsList();
  p_dataset->SetUpdateColumns(list);

  // Connect our database
  p_dataset->SetDatabase(p_database);

  // Serialize object to database record
  p_object->Serialize(*record, p_mutation);
  return p_dataset->Synchronize(p_mutation);
}

bool
CXClass::DeleteObjectInDatabase(SQLDatabase* p_database,SQLDataSet* p_dataset,CXObject* p_object,int p_mutation)
{
  // Be sure we have a dataset
  if(p_dataset == nullptr)
  {
    p_dataset = GetDataSet();
  }

  // Check for a table definition
  if(m_table == nullptr)
  {
    if(hibernate.GetStrategy() == MapStrategy::Strategy_one_table && m_super)
    {
      return GetRootClass()->REALDeleteObjectInDatabase(p_database,p_dataset,p_object,p_mutation);
    }
    // No legal table definition found
    throw StdException("No table while deleting an object of class: " + m_name);
  }

  // Default status for stand alone / single-table classes
  bool deleted = true;

  // Delete in superclass tables first (recursively!)
  if(m_super && (hibernate.GetStrategy() == MapStrategy::Strategy_sub_table))
  {
    // Select from that table first, so we can do a partial update
    SQLDataSet dataset;
    InitDataSet(&dataset);
    SQLRecord* record = m_super->SelectObjectInDatabase(p_database,&dataset,p_object->GetPrimaryKey());
    if(record)
    {
      try
      {
        // Delete this record from the database
        AutoObjectRecord temprecord(p_object, record);
        deleted = m_super->DeleteObjectInDatabase(p_database,&dataset,p_object,p_mutation);
      }
      catch(StdException& ex)
      {
        hibernate.Log(LOGLEVEL_ERROR,true,"Error deleting in super table: %s. Error: %s"
                     ,m_super->GetName().GetString()
                     ,ex.GetErrorMessage().GetString());
        throw;
      }
    }
  }

  // If succeeded, delete from our table
  if(deleted)
  {
    return REALDeleteObjectInDatabase(p_database,p_dataset,p_object,p_mutation);
  }
  return false;
}

bool
CXClass::REALDeleteObjectInDatabase(SQLDatabase* p_database,SQLDataSet* p_dataSet,CXObject* p_object,int p_mutation)
{
  SQLRecord* record = p_object->GetDatabaseRecord();

  // Connect our database
  p_dataSet->SetDatabase(p_database);
  if(record == nullptr || p_dataSet == nullptr)
  {
    return false;
  }

  // Set the record to the delete status
  record->Delete();

  // BEWARE: We need not "Serialize" our object
  // We take the assumption that the primary key is "immutable"

  // Go delete the record
  return p_dataSet->Synchronize(p_mutation);
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

// Create a filters set for a DataSet
bool
CXClass::CreateFilterSet(VariantSet& p_primary,SQLFilterSet&  p_filters)
{
  // Check if we have a primary key, or an empty key
  if(p_primary.empty())
  {
    return true;
  }

  // Check if number of columns of the primary key matches the number of values
  WordList list = GetTable()->GetPrimaryKeyAsList();
  if (list.size() != p_primary.size())
  {
    return false;
  }

  // Walk the list of columns / values
  int ind = 0;
  CString discriminator = GetRootClass()->GetDiscriminator();
  for (auto& column : list)
  {
    CString dbscolumn = discriminator + "." + column;
    SQLFilter filter(dbscolumn, SQLOperator::OP_Equal, p_primary[ind++]);
    p_filters.AddFilter(filter);
  }

  // Adding our discriminators (if any)
  AddDiscriminatorToFilters(p_filters);

  return true;
}

// Adding our discriminators to the filter
void
CXClass::AddDiscriminatorToFilters(SQLFilterSet& p_filters)
{
  // Add class filter by discriminator
  if (hibernate.GetStrategy() != MapStrategy::Strategy_standalone)
  {
    // Find discriminators
    CString discriminator = GetDiscriminator();
    CString rootdiscrim   = GetRootClass()->GetDiscriminator();
    CString column = rootdiscrim + ".discriminator";

    SubClasses& subs = GetSubClasses();
    if (subs.empty())
    {
      // Optimized for 1 (one) class
      SQLFilter discrim(column, SQLOperator::OP_Equal, discriminator);
      p_filters.AddFilter(discrim);
    }
    else
    {
      // Filter for this class and all it's subclasses
      SQLFilter discrim(column, SQLOperator::OP_IN, discriminator);
      for (auto& cl : subs)
      {
        SQLVariant dis(cl->GetDiscriminator());
        discrim.AddValue(&dis);
      }
      p_filters.AddFilter(discrim);
    }
  }
}

// Serialize the discriminator value to the database record for this object
void
CXClass::SerializeDiscriminator(CXObject* p_object,SQLRecord* p_record,int p_mutation)
{
  if(hibernate.GetStrategy() != MapStrategy::Strategy_standalone)
  {
    SQLVariant disc(p_object->GetDiscriminator());
    p_record->SetField("discriminator", &disc, p_mutation);
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
  if(!m_identity.m_attributes.empty())
  {
    XMLElement* identity = p_message.AddElement(p_theClass,"identity",XDT_String,"");
    p_message.SetAttribute(identity,"name",m_identity.m_constraintName);
    CString deferrable;
    switch(m_identity.m_deferrable)
    {
      case SQL_INITIALLY_DEFERRED:    deferrable = "initially_deferred";  break;
      case SQL_INITIALLY_IMMEDIATE:   deferrable = "initially_immediate"; break;
      case SQL_NOT_DEFERRABLE:        deferrable = "not_deferrable";      break;
    }
    p_message.SetAttribute(identity,"deferrable",deferrable);
    p_message.SetAttribute(identity,"initially_deferred",m_identity.m_initiallyDeferred ? "deferred" : "immediate");

    // Add the columns of the primary key
    for(auto& col : m_identity.m_attributes)
    {
      XMLElement* column = p_message.AddElement(identity,"attribute",XDT_String,"");
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
      AddAttribute(attribute);

      // Next attributes
      attrib = p_message.GetElementSibling(attrib);
    }
  }
}

// Loading the identity info
void 
CXClass::LoadMetaInfoIdentity(XMLMessage& p_message,XMLElement* p_theClass)
{
  XMLElement* identity = p_message.FindElement(p_theClass,"identity");
  if(identity)
  {
    m_identity.m_constraintName = p_message.GetAttribute(identity,"name");
    CString deferr  = p_message.GetAttribute(identity,"deferrable");
    CString initdef = p_message.GetAttribute(identity,"initially_deferred");
    if(deferr.CompareNoCase("initailly_deferred") == 0) m_identity.m_deferrable = SQL_INITIALLY_DEFERRED;
    if(deferr.CompareNoCase("initially_immediate")== 0) m_identity.m_deferrable = SQL_INITIALLY_IMMEDIATE;
    if(deferr.CompareNoCase("not_deferrable") == 0)     m_identity.m_deferrable = SQL_NOT_DEFERRABLE;
    m_identity.m_initiallyDeferred = initdef.CompareNoCase("deferred") == 0;

    XMLElement* column = p_message.FindElement(identity,"attribute");
    while(column)
    {
      CString name = p_message.GetAttribute(column,"name");
      CXAttribute* attrib = FindAttribute(name);
      if(attrib)
      {
        m_identity.m_attributes.push_back(attrib);
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
          // [EH] MUST BUILD LATE-BINDING !!
          CXClass* related = p_session->FindClass(ass->m_primaryTable);
          if (related)
          {
            attrib = related->FindAttribute(name);
            if(attrib)
            {
              ass->m_attributes.push_back(attrib);
            }
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
      AddSubClass(subclass);
    }
    else throw StdException("Declared subclass not found: " + sub);
  }
}

// Fill in our underlying table
void
CXClass::FillTableInfoFromClassInfo()
{
  int position = 1;

  if((hibernate.GetStrategy() == MapStrategy::Strategy_sub_table ||
      hibernate.GetStrategy() == MapStrategy::Strategy_classtable) &&
      m_super != nullptr)
  {
    // Add primary key attributes as columns
    for(auto& attrib : m_identity.m_attributes)
    {
      MetaColumn column;
      column.m_table      = GetTable()->GetTableName();
      column.m_column     = attrib->GetDatabaseColumn();
      column.m_datatype   = attrib->GetDataType();
      column.m_columnSize = attrib->GetMaxLength();
      column.m_position   = position++;

      GetTable()->AddInfoColumn(column);
    }
  }

  // Add column info
  for(auto& attrib : m_attributes)
  {
    MetaColumn column;
    column.m_table      = GetTable()->GetTableName();
    column.m_column     = attrib->GetDatabaseColumn();
    column.m_datatype   = attrib->GetDataType();
    column.m_columnSize = attrib->GetMaxLength();
    column.m_position   = position++;

    GetTable()->AddInfoColumn(column);
  }

  // Add primary key info
  position = 1;
  for(auto& attrib : m_identity.m_attributes)
  {
    MetaPrimary prim;
    prim.m_table             = GetTable()->GetTableName();
    prim.m_columnName        = attrib->GetDatabaseColumn();
    prim.m_columnPosition    = position++;
    prim.m_constraintName    = m_identity.m_constraintName;
    prim.m_deferrable        = m_identity.m_deferrable;
    prim.m_initiallyDeferred = m_identity.m_initiallyDeferred;

    GetTable()->AddPrimaryKey(prim);
  }

  // Add foreign keys
  for(auto& foreign : m_associations)
  {
    position = 1;
    for(auto& col : foreign->m_attributes)
    {
      MetaForeign fkey;

      fkey.m_fkSchemaName      = GetTable()->GetSchemaName();
      fkey.m_fkTableName       = GetTable()->GetTableName();
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
      ind.m_schemaName = GetTable()->GetSchemaName();
      ind.m_tableName  = GetTable()->GetTableName();
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
    seq.m_schemaName   = GetTable()->GetSchemaName();
    seq.m_sequenceName = m_generator;
    seq.m_currentValue = m_gen_value;

    GetTable()->AddSequence(seq);
  }

  // Add privileges
  for(auto& access : m_privileges)
  {
    MetaPrivilege priv;
    priv.m_schemaName = GetTable()->GetSchemaName();
    priv.m_tableName  = GetTable()->GetTableName();
    priv.m_grantee    = access.m_grantee;
    priv.m_privilege  = access.m_privilege;
    priv.m_grantable  = access.m_grantable;

    GetTable()->AddPrivilege(priv);
  }
}

//////////////////////////////////////////////////////////////////////////
//
// Default SELECT query
//
//////////////////////////////////////////////////////////////////////////

CString 
CXClass::BuildSelectQueryOneTable(SQLInfoDB* p_info)
{
  CString  columns("SELECT ");
  CString  discrim   = GetRootClass()->GetDiscriminator();
  CString  asalias   = " as " + discrim;
  WordList dbsNames  = FindAllDBSAttributes(true);
  bool     firstdone = false;

  // Build select part
  for(auto& dbsname : dbsNames)
  {
    if(firstdone)
    {
      columns += "      ,";
    }
    columns  += discrim + "." + dbsname + "\n";
    firstdone = true;
  }

  // Default query will be built as
  // "SELECT disc.columnname\n"
  // "  FROM table as disc"
  CString query = columns + "  FROM " + GetTable()->GetDMLTableName(p_info) + asalias;

  return query;
}

CString 
CXClass::BuildSelectQuerySubTable(SQLInfoDB* p_info)
{
  CString columns("SELECT ");
  CString frompart;
  bool    firstdone = false;

  BuildSelectQuerySubTableRecursive(p_info,columns,frompart,firstdone);

  return columns + frompart;
}

CString 
CXClass::BuildSelectQuerySubclass(SQLInfoDB* p_info)
{
  return "";
}

void 
CXClass::BuildSelectQuerySubTableRecursive(SQLInfoDB* p_info,CString& p_columns,CString& p_frompart,bool& p_firstdone)
{
  // Build superclass part first
  if(m_super)
  {
    m_super->BuildSelectQuerySubTableRecursive(p_info,p_columns,p_frompart,p_firstdone);
  }

  // Build select part
  WordList dbsNames = FindAllDBSAttributes(false);
  for(auto& dbsname : dbsNames)
  {
    if(p_firstdone)
    {
      p_columns += "      ,";
    }
    p_columns += m_discriminator + "." + dbsname + "\n";
    p_firstdone = true;
  }

  if(p_frompart.IsEmpty())
  {
    p_frompart = "  FROM " + GetTable()->GetDMLTableName(p_info) + " as " + m_discriminator;
  }
  else
  {
    p_frompart += "\n       INNER JOIN " + GetTable()->GetDMLTableName(p_info) + " as " + m_discriminator;
    p_frompart += " ON (";

    // Link on the identities of the classes
    int ind = 0;
    CString supdis = m_super->GetDiscriminator();
    CXIdentity& supiden = m_super->GetIdentity();
    for(auto& attrib : m_identity.m_attributes)
    {
      if(ind > 0)
      {
        p_frompart += " AND ";
      }
      p_frompart += supdis + "." + supiden.m_attributes[ind]->GetDatabaseColumn();
      p_frompart += " = ";
      p_frompart += m_discriminator + "." + attrib->GetDatabaseColumn();
      ++ind;
    }
    p_frompart += ")";
  }
}

// Initialize a dataset for the class/table
void
CXClass::InitDataSet(SQLDataSet* p_dataSet)
{
  // Fill in the dataset
  CXTable* table = GetTable();
  p_dataSet->SetPrimaryTable(table->GetSchemaName(),table->GetTableName());
  WordList list = table->GetPrimaryKeyAsList();
  p_dataSet->SetPrimaryKeyColumn(list);
}
