////////////////////////////////////////////////////////////////////////
//
// File: TEST_SubTable.cpp
//
// Copyright (c) 1998-2018 ir. W.E. Huisman
// All rights reserved
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
// Last Revision:   22-04-2018
// Version number:  0.0.1
//
#include "stdafx.h"
#include "CppUnitTest.h"
#include "CXSession.h"
#include "CXClass.h"
#include "Animal.h"
#include "Cat.h"
#include "Dog.h"
#include "Kitten.h"
#include <SQLComponents.h>
#include <SQLVariant.h>
#include <SQLQuery.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace SQLComponents;

namespace HibernateTest
{
  TEST_CLASS(SubTable)
  {
  public:
    ~SubTable()
    {
      hibernate.CloseAllSessions();
    }

    TEST_METHOD(T01_SelectKitten)
    {
      Logger::WriteMessage("Getting a record from the KITTEN class");

      try
      {
        OpenSession();

        Kitten* kitten = (Kitten*)m_session->Load(Kitten::ClassName(),1);
        Assert::IsNotNull(kitten);

        PrintKitten(kitten);
      }
      catch(StdException* er)
      {
        CString error = er->GetErrorMessage();
        Logger::WriteMessage(error);
        er->Delete();
        Assert::Fail();
      }
    }

    void PrintAnimal(Animal* p_animal)
    {
      CString text;
      text.Format("Animal ID             : %d",p_animal->GetId());                        Logger::WriteMessage(text);
      text.Format("Animal name           : %s",p_animal->GetAnimalName());                Logger::WriteMessage(text);
      text.Format("Has claws             : %s",p_animal->GetHas_claws() ? "yes" : "no");  Logger::WriteMessage(text);
      text.Format("Has hair              : %s",p_animal->GetHas_hair()  ? "yes" : "no");  Logger::WriteMessage(text);
      text.Format("Has wings             : %s",p_animal->GetHas_wings() ? "yes" : "no");  Logger::WriteMessage(text);
      text.Format("Number of legs        : %d",p_animal->GetNumberOfLegs());              Logger::WriteMessage(text);
    }

    void PrintCat(Cat* p_cat)
    {
      PrintAnimal(p_cat);

      CString text;
      text.Format("Color of cat          : %s",p_cat->GetColor());                        Logger::WriteMessage(text);
      text.Format("Used to a catdoor     : %s",p_cat->GetCatdoor()      ? "yes" : "no");  Logger::WriteMessage(text);
      text.Format("Cat likes Whiskas     : %s",p_cat->GetLikesWhiskas() ? "yes" : "no");  Logger::WriteMessage(text);
    }


    void PrintKitten(Kitten* p_kitten)
    {
      PrintCat(p_kitten);

      CString text;
      text.Format("Color of kitten       : %s",p_kitten->GetColor());                   Logger::WriteMessage(text);
      text.Format("Kitten is immuun      : %s",p_kitten->GetImmuun()   ? "yes" : "no"); Logger::WriteMessage(text);
      text.Format("Kitten still in litter: %s",p_kitten->GetInLitter() ? "yes" : "no"); Logger::WriteMessage(text);
    }

    // Open a CXHibernate session by config file
    // Without an external database
    bool OpenSession()
    {
      if(m_session)
      {
        return true;
      }

      // Init in the correct language
      InitSQLComponents(LN_ENGLISH);

      try
      {
        m_session = hibernate.CreateSession("s1","subtable.cfg.xml");
        if(m_session)
        {
          // We are a database session
          m_session->SetDatabaseConnection("hibtest","sysdba","altijd");

          return true;
        }
      }
      catch(StdException* er)
      {
        Assert::AreEqual("",er->GetErrorMessage());
        er->Delete();
      }
      Assert::Fail();
    }

    CXSession* m_session;
  };
}

