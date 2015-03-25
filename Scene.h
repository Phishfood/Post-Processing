#pragma once

#include <windows.h>
#include <d3d10.h>
#include <d3dx10.h>
#include <atlbase.h>
#include <sstream>
#include <iostream>
#include <random>
#include "resource.h"

#include "Defines.h" // General definitions shared by all source files
#include "Model.h"   // Model class - encapsulates working with vertex/index data and world matrix
#include "Camera.h"  // Camera class - encapsulates the camera's view and projection matrix
#include "CTimer.h"  // Timer class - not DirectX
#include "Input.h"   // Input functions - not DirectX

#include "Light.h"			// Light class.
#include "RenderObject.h"	// Render class.

#include "Colours.h"		// Functions to adjust colours

#include "AntTweakBar.h"
#pragma comment(lib, "AntTweakBar.lib")

#include <vector>

const int MAX_BLUR_RADIUS = 64;

class CScene
{
private:
	// DX Device pointer
	ID3D10Device* mpd3dDevice;

	//Viewport details - static because the message handler is static. 
	static int mViewportWidth, mViewportHeight;	

	static const int MAX_LIGHTS = 30;
	static const int MAX_SHADER_LIGHTS = 12;
	static const int MAX_OBJECTS = 200;

	// Models and cameras encapsulated in classes for flexibity and convenience
	// The CModel class collects together geometry and world matrix, and provides functions to control the model and render it
	// The CCamera class handles the view and projections matrice, and provides functions to control the camera
	CCamera* Camera;

	CRenderObject* mpObjects[MAX_OBJECTS];
	int miNumObjects;

	CLight* mpLights[MAX_LIGHTS];
	int miNumLights;

	// Textures - no texture class yet so using DirectX variables
	// * 2 since each object can potentially have 2 textures, although this is likely overkill.
	ID3D10ShaderResourceView* mpMaps[MAX_OBJECTS*2];
	int miNumMaps;

	// Ambient Light
	D3DXVECTOR3 AmbientColour;

	//defines the object to be controlled via keyboard
	int mControlObject;

	// Note: There are move & rotation speed constants in Defines.h


	//--------------------------------------------------------------------------------------
	// Shader Variables
	//--------------------------------------------------------------------------------------
	// Variables to connect C++ code to HLSL shaders

	// Effects / techniques
	ID3D10Effect*          Effect;
	ID3D10Effect*		   mPPEffect;
	
	////////////////////////////
	//POST PROCESS VARIABLES
	////////////////////////////
	enum PostProcesses
	{
		Copy, 
		Tint,
		Shock,
		BoxBlur3,
		BoxBlur5,
		BoxBlurV,
		DoubleVision,
		Edge,
		Contrast,
		Blood,
		Invert,
		SolariseA,
		SolariseB,
		CellShade,
		Gaussian,
		NumPostProcesses
	};

	ID3D10EffectTechnique* mPPTechniques[NumPostProcesses];
	ID3D10EffectTechnique* mTechniques[30];
	ID3D10EffectTechnique* mTechniquesMirror[30];
	int miNumTechniques;

	ID3D10Texture2D*	mInitialTexture;
	D3D10_TEXTURE2D_DESC mInitialTextureDesc;
	ID3D10RenderTargetView* mInitialRenderTarget;
	ID3D10ShaderResourceView* mInitialShaderResource;
	D3D10_SHADER_RESOURCE_VIEW_DESC mInitialShaderDesc;
	ID3D10EffectShaderResourceVariable* mInitialTextureVar;
	

	float mBlood;
	ID3D10EffectVariable* mdxBlood;
	ID3D10Texture2D* mPostProcessMap;
	ID3D10EffectShaderResourceVariable* mPostProcessMapVar;

	//shock effect
	float mShock, mShockStrength, mShockTime, mShockSpeed, mShockLength;
	ID3D10EffectVariable* mdxPPShock;

	//tint effect
	float mCumulativeFTime;
	ID3D10EffectVariable* mdxPPTintColour;
	D3DXVECTOR3 mTintColour;
	float mTintCycleTime;

	//Blur
	int mBlurRadius;
	float mBlurStrength;
	float mBlurMean;
	ID3D10EffectVariable* mdxBlurRadius;
	ID3D10EffectVariable* mdxBlurWeights;

	float mBlurWeights[MAX_BLUR_RADIUS];

	//DoubleVision
	float mDoubleVisionRadius;
	ID3D10EffectVariable* mdxDoubleVisionRadius;

	//Contrast
	float mContrastChange;
	float mContrastFactor;
	ID3D10EffectVariable* mdxContrastFactor;

	int mCurrentPP;
	int mChainLength;

	ID3D10Texture2D* mTextureOne, *mTextureTwo;
	ID3D10RenderTargetView* mRenderTargetOne, *mRenderTargetTwo;
	ID3D10ShaderResourceView* mTextureOneShader, *mTextureTwoShader;
	bool ppDirection, fromSource;

	int mSolariseInt;
	float mSolariseFloat;
	ID3D10EffectVariable* mdxSolariseFloat;

	bool impact;

	ID3D10ShaderResourceView* mDepthShaderView;
	D3D10_DEPTH_STENCIL_VIEW_DESC mDepthStencilViewDesc;
	D3D10_SHADER_RESOURCE_VIEW_DESC mDepthShaderDesc;
	D3D10_TEXTURE2D_DESC mDepthDesc;

	ID3D10EffectShaderResourceVariable* mPPDepthMap;

	////////////////////////////
	//RENDER VARIABLES
	////////////////////////////
	// Matrices
	ID3D10EffectMatrixVariable* WorldMatrixVar;
	ID3D10EffectMatrixVariable* ViewMatrixVar;
	ID3D10EffectMatrixVariable* ProjMatrixVar;
	ID3D10EffectMatrixVariable* ViewProjMatrixVar;

	// Textures
	ID3D10EffectShaderResourceVariable* DiffuseMapVar;
	ID3D10EffectShaderResourceVariable* NormalMapVar;

	// Miscellaneous
	ID3D10EffectVectorVariable* ModelColourVar;

	//Positions
	ID3D10EffectVectorVariable* dxLightPosA;
	ID3D10EffectVectorVariable* dxCameraPos;

	//Lights
	ID3D10EffectVectorVariable* dxLightColourA;
	ID3D10EffectVectorVariable* dxAmbientColour;
	ID3D10EffectVariable*		dxLightBrightnessA;

	ID3D10EffectVariable*		dxWiggle;
	ID3D10EffectVariable*		dxOutlineThickness;

	ID3D10EffectVectorVariable* ClipPlaneVar;

	//--------------------------------------------------------------------------------------
	// DirectX Variables
	//--------------------------------------------------------------------------------------

	// Variables used to setup D3D
	IDXGISwapChain*         SwapChain;
	ID3D10Texture2D*        DepthStencil;
	ID3D10DepthStencilView* DepthStencilView;
	ID3D10RenderTargetView* RenderTargetView;

	// Variables used to setup the Window
	HINSTANCE HInst;
	HWND      HWnd;

	////////////////////////////
	//AntTweakBar Variables
	////////////////////////////

	TwBar* mtwBarPP; //general settings
	TwBar* mtwBarSinglePP; //Single Pass PP options
	TwBar* mtwBarMultiPP; //Multi Pass PP options

	// Functions to load items into the scene - purely for neatness
	bool BasicItems();  
	bool SceneItems();
	bool TestItems();

	// Choose the lights to send to the shader
	void SetLights( D3DXVECTOR3 source, CLight* lightsSource[MAX_LIGHTS], int lightCount);
	// Sort function for the lights - static for function pointer.
	static inline int compareLights( const void* a, const void* b);

	// Message Handler - static for function pointer.
	static LRESULT CALLBACK WndProc( HWND, UINT, WPARAM, LPARAM );

	inline void DrawObject(int i, bool mirror = false);
	inline void DrawAllObjects(bool mirror);

	
	void UpdateImpact(float frameTime);
	void RenderImpact();
	
	void UpdateGaussianDist(float sigma, int samples);

	void PostProcess(int process);

public:

	void StartImpact();

	bool multiprocess;
	bool mbGaussian;

	std::vector<int> mPPSteps;

	CScene(void);
	~CScene(void);

	void ResetShock();

	void SetSinglePP(int index);
	bool InitDevice();
	void ReleaseResources();
	bool LoadEffectFile();
	bool InitScene();
	bool InitPP();
	bool InitATB();
	void UpdateScene( float frameTime );
	void RenderScene();
	void RenderMirrors();
	bool InitWindow( HINSTANCE hInstance, int nCmdShow );

};

struct ScenePP
{
	static CScene* scenePointer;
	int PPindex;
};