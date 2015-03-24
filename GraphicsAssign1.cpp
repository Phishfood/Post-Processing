//--------------------------------------------------------------------------------------
//	GraphicsAssign1.cpp
//
//	Shaders Graphics Assignment
//	Add further models using different shader techniques
//	See assignment specification for details
//--------------------------------------------------------------------------------------

//***|  INFO  |****************************************************************
// Lights:
//   The initial project shows models for a couple of point lights, but the
//   initial shaders don't actually apply any lighting. Part of the assignment
//   is to add a shader that uses this information to actually light a model.
//   Refer to lab work to determine how best to do this.
// 
// Textures:
//   The models in the initial scene have textures loaded but the initial
//   technique/shaders doesn't show them. Part of the assignment is to add 
//   techniques to show a textured model
//
// Shaders:
//   The initial shaders render plain coloured objects with no lighting:
//   - The vertex shader performs basic transformation of 3D vertices to 2D
//   - The pixel shader colours every pixel the same colour
//   A few shader variables are set up to communicate between C++ and HLSL
//   You will usually need to add further variables to add new techniques
//*****************************************************************************

#include "resource.h"

#include "Defines.h" // General definitions shared by all source files
#include "CTimer.h"  // Timer class - not DirectX
#include "Input.h"   // Input functions - not DirectX

#include "Scene.h"			// Scene class - reduces globals.

//--------------------------------------------------------------------------------------
// Entry point to the program. Initializes everything and goes into a message processing 
// loop. Idle time is used to render the scene.
//--------------------------------------------------------------------------------------
int WINAPI wWinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow )
{
	CScene* myScene = new CScene();
	// Initialise everything in turn
	if( !myScene->InitWindow( hInstance, nCmdShow) )
	{
		return 0;
	}
	if( !myScene->InitDevice() || !myScene->LoadEffectFile() || !myScene->InitScene() || !myScene->InitPP() )
	{
		myScene->ReleaseResources(); // Cleanup DirectX on failure
		return 0;
	}

	myScene->InitATB();

	// Initialise simple input functions (in Input.h/.cpp, not part of DirectX)
	InitInput();

	// Initialise a timer class (in CTimer.h/.cpp, not part of DirectX). It's like a stopwatch - start it counting now
	CTimer Timer;
	Timer.Start();

	// Main message loop. The window will stay in this loop until it is closed
	MSG msg = {0};
	while( WM_QUIT != msg.message )
	{
		// First check to see if there are any messages that need to be processed for the window (window resizing, minimizing, whatever), if there are then deal with them
		// If not then the window is idle and the D3D rendering occurs. This is in a loop. So the window is rendered over and over, as fast as possible as long as we are
		// not manipulating the window in some way
		if( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ) )
		{
			// Deal with messages
			TranslateMessage( &msg );
			DispatchMessage( &msg );
		}
		else // Otherwise render & update
		{
			myScene->RenderScene();

			// Get the time passed since the last frame (since the last time this line was reached) - used so the rendering and update can be
			// synchronised to real time and won't be dependent on machine speed
			float frameTime = Timer.GetLapTime();
			myScene->UpdateScene( frameTime );
		}
	}

	// Release all the resources we've created before leaving
	myScene->ReleaseResources();

	delete myScene;

	return ( int )msg.wParam;
}
