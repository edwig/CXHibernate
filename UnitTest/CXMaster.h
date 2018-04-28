#pragma once
#include <CXObject.h>
#include <bcd.h>

class CXTable;

class CXMaster : public CXObject
{
public:
  // CTOR of an CXObject
  CXMaster(CXTable* p_table);

  // DECLARE_CXO_FACTORY(CXMaster);

  // Bring the contents of the class to a SOAPMessage or a SQLRecord
  virtual void Serialize(SOAPMessage& p_message,int p_mutation = 0);
  virtual void Serialize(SQLRecord&   p_record, int p_mutation = 0);

  // Read the contents of an object from a SOAPMessage or a SQLRecord
  virtual void DeSerialize(SOAPMessage& p_msg);
  virtual void DeSerialize(SQLRecord&   p_record);

  // GETTERS
  int     GetID()           { return m_id;          };
  int     GetInvoice()      { return m_invoice;     };
  CString GetDescription()  { return m_description; };
  bcd     GetTotal()        { return m_total;       };

  // Setters
  void    SetTotal(bcd p_bcd);

protected:

private:
  // Database persistent attributes
  long    m_id;
  long    m_invoice;
  CString m_description;
  bcd     m_total;
  // Virtual attributes
};

// Declare our factory
DECLARE_CXO_FACTORY(CXMaster);
