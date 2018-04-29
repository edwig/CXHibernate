#pragma once
#include <CXObject.h>
#include <bcd.h>

class CXTable;

class TestNumber : public CXObject
{
public:
  // CTOR of an CXObject
  TestNumber(CXTable* p_table);

  // Bring the contents of the class to a SOAPMessage or a SQLRecord
  virtual void Serialize(SOAPMessage& p_message, int p_mutation = 0);
  virtual void Serialize(SQLRecord&   p_record, int p_mutation = 0);

  // Read the contents of an object from a SOAPMessage or a SQLRecord
  virtual void DeSerialize(SOAPMessage& p_msg);
  virtual void DeSerialize(SQLRecord&   p_record);

  // GETTERS
  long    GetID()     { return m_id;     };
  int     GetField1() { return m_field1; };
  double  GetField2() { return m_field2; };
  bcd     GetField3() { return m_field3; };

  // SETTERS
  void    SetID(long p_id)          { m_id     = p_id;    };
  void    SetField1(int p_field)    { m_field1 = p_field; };
  void    SetField2(double p_field) { m_field2 = p_field; };
  void    SetField3(bcd p_field)    { m_field3 = p_field; };

protected:

private:
  // Database persistent attributes
  long    m_id;
  int     m_field1;
  double  m_field2;
  bcd     m_field3;
  // Virtual attributes
};

DECLARE_CXO_FACTORY(TestNumber);

