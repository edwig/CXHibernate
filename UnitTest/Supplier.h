// Interface definition file for class: Supplier
// Automatically generated by: CX-Hibernate
// File: Supplier.h
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

class Supplier : public Subject
{
public:
  // CTOR of an CXObject derived class
  Supplier();
  // DTOR of an CXObject derived class
  virtual ~Supplier();

  // Serialization of our persistent objects
  DECLARE_CXO_SERIALIZATION;

  // GETTERS
  CString    GetDeliveryAddress() { return m_deliveryAddress;   };
  bcd        GetBalance()         { return m_balance;           };
  bcd        GetOpen_orders()     { return m_open_orders;       };
  bcd        GetOpen_billings()   { return m_open_billings;     };

protected:
  // Database persistent attributes
  CString    m_deliveryAddress   ;
  bcd        m_balance           ;
  bcd        m_open_orders       ;
  bcd        m_open_billings     ;

private:
  // Transient attributes go here
};
