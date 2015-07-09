#include<iostream>


#include "ErrorClassDefinitions.h"



class RegistrationFailed : Error
{
public:
	std::string error()	{ return "Problem je nastao tijekom registracije klase"; }

};