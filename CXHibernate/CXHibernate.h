#pragma once
#include <vector>


// ORM Mapping strategy classes -> tables

typedef enum _map_strategy
{
   Strategy_standalone   // No sub/super classes. 1 table = 1 class
  ,Strategy_one_table    // 1 table for super + all sub-classes
  ,Strategy_sub_tables   // 1 table for super + 1 table per sub-class
//,Strategy_unions       // 1 table for subclass + all its supers
}
MapStrategy;

class CXSession;
using MapSessions = std::vector<CXSession*>;


class CXHibernate
{
public:
  CXHibernate();
 ~CXHibernate();

  // FUNCTIONS

  // Create a new Hibernate session
  CXSession*   CreateSession();
  // Adding a externally created session (but we now own it)
  void         AddSession(CXSession* p_session);


  // SETTERS

  // Setting a hibernate mapping strategy
  bool          SetStrategy(MapStrategy p_strategy);
  // Setting a default catalog for the execution time
  void          SetDefaultCatalog(CString p_catalog);
  // Setting a default database schema for the execution time
  void          SetDefaultSchema (CString p_schema);

  // GETTERS

  MapStrategy   GetStrategy();
  CString       GetDefaultCatalog();
  CString       GetDefaultSchema();

private:
  // Flushing all data to the database and closing all sessions
  void          CloseAllSessions();

  // Relational-Object-Mapping strategy
  MapStrategy   m_strategy { Strategy_standalone };
  // Mapping to all open sessions
  MapSessions   m_sessions;

  // DEFAULTS
  CString       m_default_catalog;      // Default catalog
  CString       m_default_schema;       // Default schema of all tables
};

// Singleton pointer to the library
extern CXHibernate hibernate;