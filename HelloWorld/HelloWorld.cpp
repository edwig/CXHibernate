// HelloWorld.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"
#include "country.h"
#include <CXHibernate.h>
#include <CXSession.h>

int main()
{
  CXSession* session = hibernate.CreateSession();
  if(session)
  {
    // Set a database session
    session->SetDatabaseConnection("hibtest","sysdba","altijd");

    country* land = (country*)session->Load(country::ClassName(),1);
    if(land)
    {
      printf("Hallo World! to all %d inhabitants of %s\n"
            ,land->GetInhabitants()
            ,land->GetName().GetString());
    }
    else
    {
      printf("Cannot find a country with id = %d\n",1);
    }
    // And close our session
    session->CloseSession();
  }
  return 0;
}
