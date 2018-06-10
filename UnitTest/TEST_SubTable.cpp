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
      Logger::WriteMessage("Getting an object from the KITTEN class");

      try
      {
        OpenSession();

        Kitten* kitten = (Kitten*)m_session->Load(Kitten::ClassName(),1);
        Assert::IsNotNull(kitten);

        PrintKitten(kitten);

        CloseSession();
      }
      catch(StdException* er)
      {
        CString error = er->GetErrorMessage();
        Logger::WriteMessage(error);
        er->Delete();
        Assert::Fail();
      }
    }

    TEST_METHOD(T02_SelectCat)
    {
      Logger::WriteMessage("Getting an object from the CAT class");

      try
      {
        OpenSession();

        Cat* pussy = (Cat*)m_session->Load(Cat::ClassName(), 2);
        Assert::IsNotNull(pussy);

        PrintCat(pussy);

        CloseSession();
      }
      catch (StdException* er)
      {
        CString error = er->GetErrorMessage();
        Logger::WriteMessage(error);
        er->Delete();
        Assert::Fail();
      }
    }

    TEST_METHOD(T03_SelectDog)
    {
      Logger::WriteMessage("Getting an object from the DOG class");

      try
      {
        OpenSession();

        Dog* bello = (Dog*)m_session->Load(Dog::ClassName(), 3);
        Assert::IsNotNull(bello);

        PrintDog(bello);

        CloseSession();
      }
      catch (StdException* er)
      {
        CString error = er->GetErrorMessage();
        Logger::WriteMessage(error);
        er->Delete();
        Assert::Fail();
      }
    }

    TEST_METHOD(T04_SelectCats)
    {
      Logger::WriteMessage("Getting objects from the CAT/KITTEN class");

      try
      {
        OpenSession();

        SQLFilter filt("ani.id",OP_Smaller,4);
        CXResultSet set = m_session->Load(Cat::ClassName(),&filt);
        Assert::IsTrue(!set.empty());

        // Print all gotten animals, checking the class
        for(auto& object : set)
        {
          if(object->GetDiscriminator() == "cat")
          {
            PrintCat((Cat*)object);
          }
          else if(object->GetDiscriminator() == "kit")
          {
            PrintKitten((Kitten*)object);
          }
          else
          {
            Assert::Fail(L"Neither a CAT nor a KITTEN");
          }
        }

        CloseSession();
      }
      catch (StdException* er)
      {
        CString error = er->GetErrorMessage();
        Logger::WriteMessage(error);
        er->Delete();
        Assert::Fail();
      }
    }

    TEST_METHOD(T05_InsertCat)
    {
      Logger::WriteMessage("Inserting an object of the CAT class");

      try
      {
        OpenSession();

        Cat* pussy = (Cat*) m_session->CreateObject(Cat::ClassName());

        // Fill in our object with reasonable values
        pussy->SetAnimalName("Silvester");
        pussy->SetHas_claws(true);
        pussy->SetHas_hair(true);
        pussy->SetHas_wings(false);
        pussy->SetNumberOfLegs(4);
        pussy->SetColor("black");
        pussy->SetCatdoor(true);
        pussy->SetLikesWhiskas(true);

        // Go save it in the database in two tables
        bool result = m_session->Save(pussy);
        if (!result)
        {
          delete pussy;
          Assert::Fail();
        }
        else
        {
          PrintCat(pussy);
        }
        CloseSession();
      }
      catch (StdException* er)
      {
        CString error = er->GetErrorMessage();
        Logger::WriteMessage(error);
        er->Delete();
        Assert::Fail();
      }
    }

    TEST_METHOD(T06_InsertDeleteKitten)
    {
      Logger::WriteMessage("Inserting an object of the KITTEN class and deleting it right away");

      try
      {
        OpenSession();

        Kitten* little = (Kitten*)m_session->CreateObject(Kitten::ClassName());

        // Fill in our object with reasonable values
        little->SetAnimalName("Silvester's son");
        little->SetHas_claws(false);
        little->SetHas_hair(true);
        little->SetHas_wings(false);
        little->SetNumberOfLegs(4);
        little->SetColor("White");
        little->SetCatdoor(false);
        little->SetLikesWhiskas(false);
        little->SetImmuun(true);
        little->SetInLitter(true);
        little->SetKit_color("gray");

        // Go save it in the database in two tables
        bool result = m_session->Save(little);
        if (!result)
        {
          delete little;
          Assert::Fail();
        }
        else
        {
          PrintCat(little);

          result = m_session->Delete(little);
        }
        CloseSession();
      }
      catch (StdException* er)
      {
        CString error = er->GetErrorMessage();
        Logger::WriteMessage(error);
        er->Delete();
        Assert::Fail();
      }
    }

    TEST_METHOD(T07_UpdateDog)
    {
      Logger::WriteMessage("Updating an object from the DOG class");

      try
      {
        OpenSession();

        Dog* bello = (Dog*)m_session->Load(Dog::ClassName(),3);
        Assert::IsNotNull(bello);

        PrintDog(bello);

        bello->SetNumberOfLegs(4);
        bello->SetWalksPerDay(4);

        bool result = m_session->Save(bello);
        Assert::IsTrue(result);

        CloseSession();
      }
      catch (StdException* er)
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

    void PrintDog(Dog* p_dog)
    {
      PrintAnimal(p_dog);

      CString text;
      text.Format("Subrace of dog        : %s",p_dog->GetSubrace());
      text.Format("Need walks per day    : %d",p_dog->GetWalksPerDay());                  Logger::WriteMessage(text);
      text.Format("Can be used hunting   : %s",p_dog->GetHunting()  ? "yes" : "no");      Logger::WriteMessage(text);
      text.Format("Is a waterdog         : %s",p_dog->GetWaterdog() ? "yes" : "no");      Logger::WriteMessage(text);
    }


    void PrintKitten(Kitten* p_kitten)
    {
      PrintCat(p_kitten);

      CString text;
      text.Format("Color of kitten       : %s",p_kitten->GetKit_color());               Logger::WriteMessage(text);
      text.Format("Kitten is immuun      : %s",p_kitten->GetImmuun()   ? "yes" : "no"); Logger::WriteMessage(text);
      text.Format("Kitten still in litter: %s",p_kitten->GetInLitter() ? "yes" : "no"); Logger::WriteMessage(text);
    }

    // Open a CXHibernate session by config file
    // Without an external database
    void OpenSession()
    {
      // See if we already have a session
      if(m_session)
      {
        return;
      }

      try
      {
        m_session = hibernate.CreateSession("s1","subtable.cfg.xml");
        if(m_session)
        {
          // We are a database session
          m_session->SetDatabaseConnection("hibtest","sysdba","altijd");

          Logger::WriteMessage("Opened session for sub_table configuration");

          return;
        }
      }
      catch(StdException* er)
      {
        CString msg = er->GetErrorMessage();
        Logger::WriteMessage(msg);
        er->Delete();
      }
      Assert::Fail();
    }

    // Test, closing our session without relying on hibernate closing
    void CloseSession()
    {
      if(m_session)
      {
        m_session->CloseSession();
        m_session = nullptr;
        Logger::WriteMessage("Closed session");
      }
    }
    
    CXSession* m_session;
  };
}

