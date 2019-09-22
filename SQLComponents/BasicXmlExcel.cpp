//////////////////////////////////////////////////////////////////////////
//
// BasicExcel
//
// Original Version: Yap Chun Wei  (2006)
// Expanded Version: Martin Fuchs  (2009) and Ami Castonguay
// Expanded Version: Long Wenbiao  (2010)
// Expanded Version: Edwig Huisman (2012)
// For more credits: See the .h interface file
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
#include "stdafx.h"
#include "SQLComponents.h"
#include "BasicXmlExcel.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace SQLComponents
{

//////////////////////////////////////////////////////////////////////////
//
// BasicXmlCell
//
//////////////////////////////////////////////////////////////////////////

BasicXmlCell::BasicXmlCell(int p_row,int p_col)
{
  m_row  = p_row;
  m_col  = p_col;
  m_type = XCT_EMPTY;
  m_val.m_intval = 0;
}

// Can be called for p_type == CT_INTEGER / CT_STRING
BasicXmlCell::BasicXmlCell(int p_row,int p_col,int p_value,CellType p_type)
{
  m_row = p_row;
  m_col = p_col;
  m_val.m_intval = p_value;
  m_type = p_type;
}

BasicXmlCell::BasicXmlCell(int p_row,int p_col,double p_value)
{
  m_row = p_row;
  m_col = p_col;
  m_val.m_number = p_value;
  m_type = XCT_DOUBLE;
}

BasicXmlCell::~BasicXmlCell()
{
}

bool     
BasicXmlCell::GetCellValue(int& p_value)
{
  if(m_type == XCT_INTEGER)
  {
    p_value = m_val.m_intval;
    return true;
  }
  return false;
}

bool     
BasicXmlCell::GetCellValue(double& p_value)
{
  if(m_type == XCT_DOUBLE)
  {
    p_value = m_val.m_number;
    return true;
  }
  return false;
}

bool     
BasicXmlCell::GetCellString(int& p_value)
{
  if(m_type == XCT_STRING)
  {
    p_value = m_val.m_string;
    return true;
  }
  return false;
}

CellType
BasicXmlCell::GetCellType()
{
  return m_type;
}

//////////////////////////////////////////////////////////////////////////
//
// BasicXmlWorksheet
//
//////////////////////////////////////////////////////////////////////////

// XTOR for the sheet
BasicXmlWorksheet::BasicXmlWorksheet(BasicXmlExcel* p_workbook,CString p_sheetname)
                  :m_workbook(p_workbook)
                  ,m_sheetname(p_sheetname)
                  ,m_maxCol(0)
                  ,m_maxRow(0)
{
}

// DTOR for the sheet
BasicXmlWorksheet::~BasicXmlWorksheet()
{
  // Clean-up all the cells
  XmlCells::iterator it;
  for(it = m_cells.begin();it != m_cells.end(); ++it)
  {
    BasicXmlCell* cell = it->second;
    delete cell;
  }
  m_cells.clear();
}

CString
BasicXmlWorksheet::GetSheetName()
{
  return m_sheetname;
}

int
BasicXmlWorksheet::GetMaxRows()
{
  return m_maxRow;
}

int
BasicXmlWorksheet::GetMaxCols()
{
  return m_maxCol;
}

BasicXmlCell* 
BasicXmlWorksheet::GetCell(int p_row,int p_col)
{
  ulong cellnum = CELLNUM(p_row,p_col);
  XmlCells::iterator it = m_cells.find(cellnum);
  if(it != m_cells.end())
  {
    return it->second;
  }
  // Cell did not exist, make an empty cell
  BasicXmlCell* cell = new BasicXmlCell(p_row,p_col);
  m_cells.insert(std::make_pair(cellnum,cell));
  return cell;
}

void
BasicXmlWorksheet::Load(XMLMessage& p_msg,XMLElement* p_root)
{
  // See if there is data in the worksheet
  XMLElement* data = p_msg.FindElement(p_root,"sheetData");
  if(data == NULL)
  {
    return;
  }
  // Finding all the rows of data
  XMLElement* row = p_msg.FindElement(data,"row");
  while(row)
  {
    // Get the row number and preserve
    CString rowName = p_msg.GetAttribute(row,"r");
    int   rowNumber = p_msg.GetAttributeInteger(row,"r");
    if(rowNumber > m_maxRow)
    {
      m_maxRow = rowNumber;
    }

    // Finding the columns
    XMLElement* col = p_msg.FindElement(row,"c");
    while(col)
    {
      // Getting the column name/number and preserve
      CString colName = p_msg.GetAttribute(col,"r");
      int colNumber = CalculateColumnNumber(colName,rowName);
      if(colNumber > m_maxCol)
      {
        m_maxCol = colNumber;
      }
      // Find datatype
      bool isString = false;
      CString dataType = p_msg.GetAttribute(col,"t");
      if(dataType && dataType.GetAt(0) == 's')
      {
        isString = true;
      }
      // Find the value of the cell
      XMLElement* val = p_msg.FindElement(col,"v");
      if(val)
      {
        CString value = val->GetValue();
        BasicXmlCell* cell = NULL;
        if(isString)
        {
          int stringNum = atoi(value);
          cell = new BasicXmlCell(rowNumber,colNumber,stringNum,XCT_STRING);
        }
        else
        {
          if(strchr(value,'.') != NULL)
          {
            // Floating point
            double fl = atof(value);
            cell = new BasicXmlCell(rowNumber,colNumber,fl);
          }
          else
          {
            // Integer number
            int intNum = atoi(value);
            cell = new BasicXmlCell(rowNumber,colNumber,intNum,XCT_INTEGER);
          }
        }
        ulong cellnum = CELLNUM(rowNumber,colNumber);
        m_cells.insert(std::make_pair(cellnum,cell));
      }
      // Next column of data
      col = p_msg.GetElementSibling(col);
    }
    // Next row of data
    row = p_msg.GetElementSibling(row);
  }
}

// Convert Excel column in alphabet into column number
// A  -> 1
// IV -> 255
int
BasicXmlWorksheet::CalculateColumnNumber(CString p_column,CString p_row)
{
  int firstLetter, secondLetter;

  // Strip rownumber from column name
  p_column = p_column.Mid(0,p_column.GetLength() - p_row.GetLength());
  p_column.MakeUpper();

  if (p_column.GetLength() == 1)
  {
    firstLetter = p_column.GetAt(0);
    // 65 is A in ascii
    return (firstLetter - 65 + 1); 
  }
  else if (p_column.GetLength() == 2)
  {
    firstLetter = p_column.GetAt(0);
    secondLetter = p_column.GetAt(1);
    // 65 is A in ascii
    return ((firstLetter - 65 + 1)*26 + (secondLetter - 65 + 1)); 
  }
  CString error;
  error.Format("Invalid column alphabet name: %s",p_column.GetString());
  m_workbook->SetError(error);
  return 0;	
}

CString  
BasicXmlWorksheet::GetCellValue(int p_row,int p_col)
{
  CString value;
  int     intValue;
  double  dblValue;

  BasicXmlCell* cell = GetCell(p_row,p_col);
  if(cell)
  {
    CellType type = cell->GetCellType();

    switch(type)
    {
      case XCT_INTEGER:  if(cell->GetCellValue(intValue))
                        {
                          value.Format("%d",intValue);
                        }
                        break;
      case XCT_DOUBLE:   if(cell->GetCellValue(dblValue))
                        {
                          value.Format("%f",dblValue);
                        }
                        break;
      case XCT_STRING:   if(cell->GetCellString(intValue))
                        {
                          value = m_workbook->GetSharedString(intValue);
                        }
                        break;
      case XCT_EMPTY:    // Fall through
      default:          break;
    }
  }
  return value;
}
  
//////////////////////////////////////////////////////////////////////////
//
// BasicXmlExcel
//
//////////////////////////////////////////////////////////////////////////

BasicXmlExcel::BasicXmlExcel(CString p_filename)
              :m_filename(p_filename)
{
  m_namesRead = false;
  m_sheetRead = false;
  // Try to open zipfile without a password
  m_zip = OpenZip(m_filename,"");
  if(m_zip == NULL)
  {
    m_error.Format("Cannot open the file: %s",m_filename.GetString());
    m_error.AppendFormat("\nOS Error: %d",GetLastError);
  }
}

BasicXmlExcel::~BasicXmlExcel()
{
  // Closing ZIP file
  if(m_zip)
  {
    CloseZip(m_zip);
    m_zip = NULL;
  }

  // Dealloc memory
  for(unsigned ind = 0;ind < m_worksheets.size(); ++ind)
  {
    BasicXmlWorksheet* sheet = m_worksheets[ind];
    delete sheet;
  }
  m_worksheets.clear();
  m_sheetnames.clear();
  m_sharedStrings.clear();
}

CString
BasicXmlExcel::GetError()
{
  return m_error;
}

bool
BasicXmlExcel::GetSheetNames(Names& p_names)
{
  if(m_sheetRead == false)
  {
    ReadSheetNames();
  }
  p_names.clear();
  for(unsigned ind = 0;ind < m_sheetnames.size();++ind)
  {
    CString& name = m_sheetnames[ind];
    p_names.push_back(name);
  }
  return true;
}

BasicXmlWorksheet*
BasicXmlExcel::GetWorksheet(int p_index)
{
  if(p_index >= 0 || p_index < (int)m_worksheets.size())
  {
    return m_worksheets[p_index];
  }
  return NULL;
}

BasicXmlWorksheet*
BasicXmlExcel::GetWorksheet(CString p_name)
{
  for(unsigned ind = 0;ind < m_worksheets.size();++ind)
  {
    BasicXmlWorksheet* sheet = m_worksheets[ind];
    if(p_name.CompareNoCase(sheet->GetSheetName()) == 0)
    {
      return sheet;
    }
  }
  return NULL;
}

void
BasicXmlExcel::ReadSheetNames()
{
  if(m_zip == NULL)
  {
    return;
  }
  ZIPENTRY ze;
  ZRESULT  res = ZR_OK;
  int entries = 0;
  // get the number of entries
  res = GetZipItem(m_zip,-1,&ze);
  if(res == ZR_OK)
  {
    entries = ze.index;
  }
  else
  {
    SetError(res);
    return;
  }
  // Loop through all entries
  for(int ind = 0; ind < entries; ++ind)
  {
    res = GetZipItem(m_zip,ind,&ze);
    if(res != ZR_OK)
    {
      m_error.Format("Cannot get entries from file: %s",m_filename.GetString());
      SetError(res);
      break;
    }
    if(_stricmp(ze.name,"xl/workbook.xml") == 0)
    {
      int   bufflen = ze.unc_size + 1;
      char* buffer  = (char*) malloc(bufflen);
      // workbook xml contains the names of the sheets
      res = UnzipItem(m_zip,ind,buffer,bufflen);
      if(res != ZR_OK)
      {
        m_error.Format("Cannot read the workbook definition of: %s",m_filename.GetString());
        SetError(res);
        free(buffer);
        break;
      }
      // delimit the buffer
      buffer[ze.unc_size] = 0;
      CString sheetInfo(buffer);

      XMLMessage doc;
      doc.ParseMessage(sheetInfo);
      XMLElement* root = doc.GetRoot();
      if(root == NULL)
      {
        m_error.Format("Workbook definition incorrect in: %s",m_filename.GetString());
        free(buffer);
        break;
      }
      XMLElement* sheets = doc.FindElement(root,"sheets");
      if(sheets == NULL)
      {
        SetError("Workbook is empty. No worksheets in spreadsheet");
        free(buffer);
        break;
      }
      XMLElement* sheet  = doc.GetElementFirstChild(sheets);
      while(sheet)
      {
        CString sheetname = doc.GetAttribute(sheet,"name");
        m_sheetnames.push_back(sheetname);
        // Get next sheet
        sheet = doc.GetElementSibling(sheet);
      }
      // Ready with the buffer
      free(buffer);
      break;
    }
  }
}

void
BasicXmlExcel::SetError(ZRESULT p_result)
{
  char buffer[1024];
  FormatZipMessage(p_result,buffer,1024);
  if(m_error.IsEmpty())
  {
    m_error = buffer;
  }
  else
  {
    m_error += " : ";
    m_error += CString(buffer);
  }
  m_error.Remove('\r');
  m_error.Replace('\n',' ');
}

void
BasicXmlExcel::SetError(CString p_error)
{
  if(m_error.IsEmpty())
  {
    m_error = p_error;
  }
  else
  {
    m_error += " : ";
    m_error += p_error;
  }
  m_error.Remove('\r');
  m_error.Replace('\n',' ');
}

bool
BasicXmlExcel::Load()
{
  // Ignore return value from LoadStrings
  // After Office-2013 it shared-strings are not mandatory any more
  LoadStrings();
  return LoadWorksheets();
}

bool
BasicXmlExcel::LoadStrings()
{
  if(m_zip == NULL)
  {
    return false;
  }
  if(m_sheetRead)
  {
    return true;
  }
  ZIPENTRY ze;
  ZRESULT  res     = ZR_OK;
  int      entries = 0;
  CString  sstName = "xl/sharedStrings.xml";

  // get the number of entries
  res = GetZipItem(m_zip,-1,&ze);
  if(res == ZR_OK)
  {
    entries = ze.index;
  }
  else
  {
    SetError(res);
    return false;
  }
  // Loop through all entries
  for(int ind = 0; ind < entries; ++ind)
  {
    res = GetZipItem(m_zip,ind,&ze);
    if(res != ZR_OK)
    {
      m_error.Format("Cannot get entries from file: %s",m_filename.GetString());
      SetError(res);
      return false;
    }
    if(sstName.CompareNoCase(ze.name) == 0)
    {
      int   bufflen = ze.unc_size + 1;
      char* buffer  = (char*) malloc(bufflen);
      // workbook xml contains the names of the sheets
      res = UnzipItem(m_zip,ind,buffer,bufflen);
      if(res != ZR_OK)
      {
        m_error.Format("Cannot read the shared-strings of: %s",m_filename.GetString());
        SetError(res);
        free(buffer);
        return false;
      }
      // delimit the buffer
      buffer[ze.unc_size] = 0;
      CString stringInfo(buffer);

      XMLMessage doc;
      doc.ParseMessage(stringInfo);
      XMLElement* root = doc.GetRoot();
      if(root == NULL)
      {
        m_error.Format("Shared-strings definition incorrect in: %s",m_filename.GetString());
        free(buffer);
        return false;
      }
      // Reading the strings
      XMLElement* si = doc.FindElement(root,"si");
      while(si)
      {
        XMLElement* tt = doc.GetElementFirstChild(si);
        CString text = tt->GetValue();
        m_sharedStrings.push_back(text);
        // next string
        si = doc.GetElementSibling(si);
      }
      // Ready with the buffer
      free(buffer);
      // Ready reading strings
      return true;
    }
  }
  return false;
}

bool
BasicXmlExcel::LoadWorksheets()
{
  if(m_zip == NULL)
  {
    return false;
  }
  if(m_sheetRead)
  {
    return true;
  }
  // Make sure we have the sheetnames
  if(m_sheetRead == false)
  {
    ReadSheetNames();
  }
  // Variables
  ZIPENTRY ze;
  ZRESULT  res = ZR_OK;
  int entries = 0;

  // get the number of entries
  res = GetZipItem(m_zip,-1,&ze);
  if(res == ZR_OK)
  {
    entries = ze.index;
  }
  else
  {
    SetError(res);
    return false;
  }
  // Loop through all worksheets
  for(unsigned sheetnum = 0; sheetnum < m_sheetnames.size(); ++sheetnum)
  {
    CString sheetName;
    sheetName.Format("xl/worksheets/sheet%u.xml",sheetnum + 1);

    // Loop through all entries
    for(int ind = 0; ind < entries; ++ind)
    {
      res = GetZipItem(m_zip,ind,&ze);
      if(res != ZR_OK)
      {
        m_error.Format("Cannot get entries from file: %s",m_filename.GetString());
        SetError(res);
        return false;
      }
      if(sheetName.CompareNoCase(ze.name) == 0)
      {
        int   bufflen = ze.unc_size + 1;
        char* buffer  = (char*) malloc(bufflen);
        // workbook xml contains the names of the sheets
        res = UnzipItem(m_zip,ind,buffer,bufflen);
        if(res != ZR_OK)
        {
          m_error.Format("Cannot read the worksheet definition of: %s",m_filename.GetString());
          SetError(res);
          free(buffer);
          return false;
        }
        // delimit the buffer
        buffer[ze.unc_size] = 0;
        CString sheetInfo(buffer);

        XMLMessage doc;
        doc.ParseMessage(sheetInfo);
        XMLElement* root = doc.GetRoot();
        if(root == NULL)
        {
          m_error.Format("Worksheet definition incorrect in: %s",m_filename.GetString());
          free(buffer);
          return false;
        }
        // Make a new worksheet
        BasicXmlWorksheet* sheet = new BasicXmlWorksheet(this,m_sheetnames[sheetnum]);
        m_worksheets.push_back(sheet);
        
        // Load data in the worksheet
        sheet->Load(doc,root);

        // Ready with the buffer
        free(buffer);
        // Break inner loop: next worksheet please
        break;
      }
    }
  }
  m_sheetRead = true;
  // See if we did read some worksheets
  return m_worksheets.size() > 0;
}

CString
BasicXmlExcel::GetSharedString(int p_string)
{
  CString value;
  if(p_string >= 0 && p_string < (int)m_sharedStrings.size())
  {
    value = m_sharedStrings[p_string];
  }
  return value;
}

// End of namespace
}
