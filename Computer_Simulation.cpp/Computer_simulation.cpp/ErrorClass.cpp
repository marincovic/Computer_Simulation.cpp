#include<iostream>


#include "ErrorClassDefinitions.h"



class RegistrationFailed : Error
{
public:
	std::string error()	{ return "Problem je nastao tijekom registracije klase"; }

};

class SwapChainFaild : Error
{
public:
	std::string error() { return "Error while calling SwapChain creation pDesc or ppSwapChain may be undefined"; }
};