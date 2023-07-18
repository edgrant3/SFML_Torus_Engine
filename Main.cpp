#include "TorusEngine.h"
#include <Windows.h>

// Windows application entry point requires this Main format 
// (required to remove the Debug Console from showing via 
// Project Properties > Linker > System > Subsystem [change from Console to Windows Subsystem]

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
//int main()
{
	TorusEngine engine;
	engine.run();
	return 0;
}