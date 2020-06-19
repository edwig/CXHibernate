////////////////////////////////////////////////////////////////////////
//
// File: SQLAssociation.cpp
//
// Copyright (c) 1998-2020 ir. W.E. Huisman
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
// Version number: See SQLComponents.h
//
#include "Stdafx.h"
#include "SQLComponents.h"
#include "SQLAssociation.h"
#include "SQLFilter.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////////////
//
// REQUIREMENTS FOR WORKING WITH ASSOCIATIONS
//
// 1) Fill the SQLDataSet for both the master and the detail
//    SetQuery            -> "SELECT * FROM master"
//                        -> "SELECT * FROM detail"
//                           Queries CAN contain a WHERE clause BUT
//                           CANNOT contain ORDER BY / HAVING etc.
//    SetPrimaryTable     -> Filled "master" / "detail"
//    SetPrimaryKeyColumn -> Set with primary keys
// 2) If network database (1 primary key column of type INTEGER) use:
//    SetSearchableColumn -> Set unique primary key (id type)
// 3) Connect both datasets to the SQLAssocation
// 4) Use "SetAssociation"-> Fill in the connecting foreign/primary columns
//
//////////////////////////////////////////////////////////////////////////

namespace SQLComponents
{

SQLAssociation::SQLAssociation()
               :m_master(nullptr)
               ,m_detail(nullptr)
{
}

SQLAssociation::SQLAssociation(SQLDataSet* p_master,SQLDataSet* p_detail)
               :m_master(p_master)
               ,m_detail(p_detail)
{
}

SQLAssociation::~SQLAssociation()
{
  FreeAssocs();
}

void
SQLAssociation::FreeAssocs()
{
  for(unsigned ind = 0;ind < m_assocs.size();++ind)
  {
    delete m_assocs[ind];
  }
  m_assocs.clear();
}

// Create assocation mapping
// and optionally the value to follow
void 
SQLAssociation::SetAssociation(CString     p_primaryColumn
                              ,CString     p_foreignColumn
                              ,SQLVariant* p_value /*= NULL*/)
{
  PrimaryForeign* pf = new PrimaryForeign();
  pf->m_primary = p_primaryColumn;
  pf->m_foreign = p_foreignColumn;
  pf->m_value   = p_value;
  m_assocs.push_back(pf);
}

// Set a new value in the association map
// For reuse on each lookup of the association
void 
SQLAssociation::SetAssociation(int p_num,SQLVariant* p_value)
{
  if(p_num >= 0 && p_num < (int)m_assocs.size())
  {
    m_assocs[p_num]->m_value = p_value;
  }
}

CString 
SQLAssociation::GetAssocationName(int p_column)
{
  if(p_column >= 0 && p_column < (int)m_assocs.size())
  {
    // Returns "master.id->detail.id_master"
    CString name;
    name = m_master->GetName() + "." +
           m_assocs[p_column]->m_primary + "->" + 
           m_detail->GetName() + "." + 
           m_assocs[p_column]->m_foreign;
    return name;
  }
  return "";
}

SQLVariant*
SQLAssociation::GetAssocationValue(int p_column)
{
  if(p_column >= 0 && p_column < (int)m_assocs.size())
  {
    return m_assocs[p_column]->m_value;
  }
  return nullptr;
}

// Check if everything needed is givven
// 1) From master to details (p_toDetails = true)
// 2) From details to master (p_toDetails = false)
bool
SQLAssociation::BasicChecks()
{
  bool result = false;

  // Check 1: Master and detail both there
  if(m_master && m_detail)
  {
    // Check 2: Association columns present and filled
    if(m_assocs.size() > 0)
    {
      result = true;
      for(auto& key : m_assocs)
      {
        if(!key->m_value)
        {
          result = false;
        }
      }
    }
  }
  return result;
}

SQLRecord*
SQLAssociation::FollowToMaster()
{
  // Check if we can do the update
  if(!BasicChecks())
  {
    return false;
  }

  // Create filterset and add to the master
  SQLFilterSet filters;
  for(unsigned ind = 0;ind < m_assocs.size();++ind)
  {
    SQLFilter filter(m_assocs[ind]->m_primary,OP_Equal,m_assocs[ind]->m_value);
    filters.AddFilter(&filter);
    m_master->SetFilters(&filters);
  }

  bool result = m_master->IsOpen() ? m_master->Append() : m_master->Open();
  if(result)
  {
    // FindBy Filter (primary = true)
    return m_master->FindObjectFilter(filters,true);
  }
  return nullptr;
}

// Find a recordset of details
// Caller must 'delete' the RecordSet
RecordSet*
SQLAssociation::FollowToDetails()
{
  // Check if we can do the update
  if(!BasicChecks())
  {
    return false;
  }

  // Create filterset and add to the detail
  SQLFilterSet filters;
  for(unsigned ind = 0;ind < m_assocs.size();++ind)
  {
    SQLFilter filter(m_assocs[ind]->m_foreign,OP_Equal,m_assocs[ind]->m_value);
    filters.AddFilter(&filter);
    m_detail->SetFilters(&filters);
  }

  bool result = m_detail->IsOpen() ? m_detail->Append() : m_detail->Open();
  if(result)
  {
    return m_detail->FindRecordSet(filters);
  }
  return nullptr;
}

// End of namespace
}
