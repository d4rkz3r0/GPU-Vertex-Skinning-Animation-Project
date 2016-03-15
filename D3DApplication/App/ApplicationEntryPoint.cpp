///***********************************************************************
//File: ApplicationEntryPoint.cpp
/*File Info:
*
*Includes:
*	<memory> - smart pointer functionality
*	<crtdbg.h> - Print memory leaks to the output window
*	"BaseApp.h" - self explanatory
*	"Exception.h - extends std::exception
*
*Notes:
*	This application is structured using component based architecture and lots of inheritance.
*	BaseApp ensures all derived App Classes get an Initialize(), Update() and/or Draw() function
*   called for free, with out explicit invocation.
*	So long as Components are added to the mComponents vector and Utilities are added to the ServiceContainer
*	through a custom RTTI implementation they can be identified and queried by calling ClassName::TypeIdClass();
*
*	If you don't want to worry about having to free and null out allocated memory, consider using smart pointers.
*	Usage:
*			std::unique_ptr<ClassName> variableName; - Declaration
*			variableName = std::make_unique<ClassName>(//Constructor Arguments Go Here//); - Allocation and Assignment, returns a unique_ptr;
*			variableName.get(); = returns a pointer to the managed object if the object is valid, return nullptr if not.
*			variableName.swap(//other unique ptr); - self explanatory
*
*	Caught exceptions will be displayed via Win32 Message Box.
*/
///***********************************************************************

#pragma once
#include <memory>
#include "IntermediaryApp.h"
#include "AppHelpers/Misc/Exception.h"

#if defined (DEBUG) || defined (_DEBUG)
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#endif

//App Entry Point
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance, LPSTR commandLine, int showCMD)
{
#if defined (DEBUG) | defined (_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	auto CoreApp = make_unique<IntermediaryApp>(hInstance, L"WindowClass", L"RTA Rendering Project", showCMD);

	try
	{
		CoreApp->Run();
	}
	catch (Exception ex)
	{
		MessageBox(CoreApp->GetWindowHandle(), ex.whatw().c_str(), CoreApp->GetWindowTitle().c_str(), MB_ABORTRETRYIGNORE);
	}

	return EXIT_SUCCESS;
}