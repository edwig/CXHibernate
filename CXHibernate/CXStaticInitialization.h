#pragma once
#pragma warning(disable : 4075)  

// Declare a pointer to a function (PF)
typedef void(__cdecl *PF)(void);

// Register the destructor of an object
int cxoexit(PF pf);

// To be called to let all classes register themselves
void CXOInitializeClasses();

// To be called to destroy the class registration
void CXODestroyClasses();


