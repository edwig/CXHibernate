@echo off

rmdir /s /q .vs
rmdir /s /q x64
rmdir /s /q lib
rmdir /s /q SQLComponents\Debug
rmdir /s /q SQLComponents\Release
rmdir /s /q SQLComponents\x64
rmdir /s /q Marlin\Debug
rmdir /s /q Marlin\Release
rmdir /s /q Marlin\x64
rmdir /s /q MarlinModule\x64
rmdir /s /q MarlinModule\Debug
rmdir /s /q MarlinModule\Release
rmdir /s /q CXHibernate\x64
rmdir /s /q CXHibernate\Debug
rmdir /s /q CXHibernate\Release
rmdir /s /q HelloWorld\x64
rmdir /s /q HelloWorld\Debug
rmdir /s /q HelloWorld\Release
rmdir /s /q cfg2cpp\x64
rmdir /s /q cfg2cpp\Debug
rmdir /s /q cfg2cpp\Release
rmdir /s /q cfg2ddl\x64
rmdir /s /q cfg2ddl\Debug
rmdir /s /q cfg2ddl\Release
rmdir /s /q TestClient\x64
rmdir /s /q TestClient\Debug
rmdir /s /q TestClient\Release
rmdir /s /q TestServer\x64
rmdir /s /q TestServer\Debug
rmdir /s /q TestServer\Release
rmdir /s /q TestServerIIS\x64
rmdir /s /q TestServerIIS\Debug
rmdir /s /q TestServerIIS\Release
rmdir /s /q UnitTest\x64
rmdir /s /q UnitTest\Debug
rmdir /s /q UnitTest\Release
del      /q BIN_Debug_x64\*.exe
del      /q BIN_Debug_x64\*.pdb
del      /q BIN_Debug_x64\*.ilk
del      /q BIN_Debug_x64\*.dll
del      /q BIN_Debug_x64\*.exp
del      /q BIN_Debug_x64\*.lib
del      /q BIN_Release_x64\*.exe
del      /q BIN_Release_x64\*.pdb
del      /q BIN_Release_x64\*.ilk
del      /q BIN_Release_x64\*.dll
del      /q BIN_Release_x64\*.exp
del      /q BIN_Release_x64\*.lib
