// Interface definition file for class: country
// Automatically generated by: CX-Hibernate
// File: country.h
//
#pragma once
#include <CXObject.h>
#include <bcd.h>
#include <SQLDate.h>
#include <SQLTime.h>
#include <SQLTimestamp.h>
#include <SQLInterval.h>
#include <SQLGuid.h>
#include <SQLVariant.h>

class country : public CXObject
{
public:
  // CTOR of an CXObject derived class
  country();
  // DTOR of an CXObject derived class
  virtual ~country();

  // Serialization of our persistent objects
  DECLARE_CXO_SERIALIZATION;

  // GETTERS
  int        GetId()              { return m_id;                };
  CString    GetName()            { return m_name;              };
  int        GetInhabitants()     { return m_inhabitants;       };
  CString    GetContinent()       { return m_continent;         };

protected:
  // Database persistent attributes
  int        m_id                 { 0 };
  CString    m_name              ;
  int        m_inhabitants        { 0 };
  CString    m_continent         ;

private:
  // Transient attributes go here
};
