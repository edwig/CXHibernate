// Interface definition file for class: Dog
// Automatically generated by: CX-Hibernate
// File: Dog.h
//
#pragma once
#include <CXObject.h>
#include "Animal.h"
#include <bcd.h>
#include <SQLDate.h>
#include <SQLTime.h>
#include <SQLTimestamp.h>
#include <SQLInterval.h>
#include <SQLGuid.h>
#include <SQLVariant.h>

class Dog : public Animal
{
public:
  // CTOR of an CXObject derived class
  Dog();
  // DTOR of an CXObject derived class
  virtual ~Dog();

  // Serialization of our persistent objects
  DECLARE_CXO_SERIALIZATION;

  // GETTERS
  CString    GetSubrace()         { return m_subrace;           };
  int        GetWalksPerDay()     { return m_walksPerDay;       };
  bool       GetHunting()         { return m_hunting;           };
  bool       GetWaterdog()        { return m_waterdog;          };

protected:
  // Database persistent attributes
  CString    m_subrace           ;
  int        m_walksPerDay        { 0 };
  bool       m_hunting            { false };
  bool       m_waterdog           { false };

private:
  // Transient attributes go here
};
