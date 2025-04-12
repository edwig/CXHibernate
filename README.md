# CXHibernate

C++ Hibernate Framework after the example of the Java Hibernate
and the C# .NET NHibernate projects.

## Version 1.4.1
- Objects kan be read-only when read extra through FollowAssociation
- AutoCXResultSet can be used to dispose result sets gotten by filtering
- Read-only objects will throw when you try to save/update/delete them
- CXSession::RemoveObjects -> Remove a set of objects from the object cache
- CXSession::Synchronize(classname) -> Synchronize just one class

### Version 1.4.0
- Now supporting Unicode builds
- Now fully tested in Oracle 21c and Firebird 3.0/4.0/5.0
- Upgraded to Marlin 8.4.1/ SQLCompoenents 2.3.2
- Can now run on a commonly shared SQLDatabasePool in a larger application
- Logging can now be integrated in the logging framework of a larger application
- cfg2cpp tool can now also generate assocation following code

### Version 1.3.0
- Upgraded to Marlin 8.0.0
- Upgraded to SQLComponents 2.0.0

### Version 1.2.0
- Support for MariaDB has been added
- Marlin Upgraded to version 6.5
- SQLComponents upgraded to version 1.7.0

### Version 1.1.0
(Also see the 'Releasenotes.txt' file)
Higher versions of the Marlin and SQLComponents framework and a 
testserver for Microsoft IIS.
Also late binding for the definitions of assications and full
unit testing of the library's main functions.

### Version 1.0.0
The following functions are already working:

* Serialization and de-serialization of objects
* Storing, updating, retrieval and deleting of objects in the database
* Storing, updating, retrieval and deleting of objects from the filestore
* Storing, updating, retrieval and deleting of objects from the internet
* Sessions, transactions and flushing of the caches
* Storing of the database definitions
* Stable 1-class 1-table database engine
* HTTP Internet Role
* Following of associations
* Derived classes in a one-table strategy
* Derived tables in their own tables

### Documentation
There is an [extensive manual in the documentation sub-directory](https://github.com/edwig/CXHibernate/blob/master/Documentation/CXHibernate.docx)
This manual contains backgrounds, examples and full documentation
of all features of the current CXHibernate library.
Look for "CXHibernate.docx"

### Roadmap
For more features to come, see the file [Roadmap.txt](https://github.com/edwig/CXHibernate/blob/master/Roadmap.txt) in the root of the project

### Friends
The CXHibernate Framework relies heave(n)ly on the following libraries:

* [The Marlin Webserver Framework](https://github.com/edwig/Marlin)
* [SQLComponents for database access](https://github.com/edwig/SQLComponents)



