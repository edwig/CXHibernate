// Interface definition file for class: Cat
// Automatically generated by: CX-Hibernate
// File: Cat.h
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

class Cat : public Animal
{
public:
  // CTOR of an CXObject derived class
  Cat();
  // DTOR of an CXObject derived class
  virtual ~Cat();

  // Serialization of our persistent objects
  DECLARE_CXO_SERIALIZATION;

  // GETTERS
  CString    GetColor()           { return m_color;             };
  bool       GetCatdoor()         { return m_catdoor;           };
  bool       GetLikesWhiskas()    { return m_likesWhiskas;      };

  // SETTERS
  void SetColor(CString p_color)            { m_color         = p_color;        };
  void SetCatdoor(bool p_catdoor)           { m_catdoor       = p_catdoor;      };
  void SetLikesWhiskas(bool p_likesWhiskas) { m_likesWhiskas  = p_likesWhiskas; };

protected:
  // Database persistent attributes
  CString    m_color             ;
  bool       m_catdoor            { false };
  bool       m_likesWhiskas       { false };

private:
  // Transient attributes go here
};
