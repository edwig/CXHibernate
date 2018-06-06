// Interface definition file for class: NaturalPerson
// Automatically generated by: CX-Hibernate
// File: NaturalPerson.h
//
#pragma once
#include <CXObject.h>
#include "Subject.h"
#include <bcd.h>
#include <SQLDate.h>
#include <SQLTime.h>
#include <SQLTimestamp.h>
#include <SQLInterval.h>
#include <SQLGuid.h>
#include <SQLVariant.h>

class NaturalPerson : public Subject
{
public:
  // CTOR of an CXObject derived class
  NaturalPerson();
  // DTOR of an CXObject derived class
  virtual ~NaturalPerson();

  // Serialization of our persistent objects
  DECLARE_CXO_SERIALIZATION;

  // GETTERS
  CString    GetFirstname()       { return m_firstname;         };
  SQLDate    GetDate_of_birth()   { return m_date_of_birth;     };
  bcd        GetLength()          { return m_length;            };

protected:
  // Database persistent attributes
  CString    m_firstname         ;
  SQLDate    m_date_of_birth     ;
  bcd        m_length            ;

private:
  // Transient attributes go here
};
