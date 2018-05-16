#pragma once

typedef struct _modelWord
{
  const char* m_name;
  const int   m_code;
}
CXModelWord;

int     CXModelWordToCode(CString p_word);
CString CXCodeToModelWord(int p_code);

