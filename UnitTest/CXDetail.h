#pragma once
#include <CXObject.h>
#include <bcd.h>

class CXTable;

class CXDetail : public CXObject
{
public:
  // CTOR of an CXObject
  CXDetail(CXTable* p_table);

  // DECLARE_CXO_FACTORY(CXMaster);

  // Bring the contents of the class to a SOAPMessage or a SQLRecord
  virtual void Serialize(SOAPMessage& p_message,int p_mutation = 0);
  virtual void Serialize(SQLRecord&   p_record, int p_mutation = 0);

  // Read the contents of an object from a SOAPMessage or a SQLRecord
  virtual void DeSerialize(SOAPMessage& p_msg);
  virtual void DeSerialize(SQLRecord&   p_record);

  // GETTERS
  int     GetID()            { return m_id;          };
  int     GetMasterID()      { return m_mast_id;     };
  int     GetLine()          { return m_line;        };
  CString GetDescription()   { return m_description; };
  bcd     GetAmount()        { return m_amount;      };

protected:

private:
  // Database persistent attributes
  long    m_id;
  long    m_mast_id;
  long    m_line;
  CString m_description;
  bcd     m_amount;
  // Virtual attributes
};

DECLARE_CXO_FACTORY(CXDetail);
