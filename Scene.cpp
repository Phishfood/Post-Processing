#include "Scene.h"

int CScene::mViewportWidth, CScene::mViewportHeight;

CScene::CScene(void)
{
	//set counters to 0
	miNumObjects = 0;
	miNumLights = 0;
	miNumMaps = 0;

	//set the ambient colour for the scene
	AmbientColour = D3DXVECTOR3( 0.2f, 0.2f, 0.2f );

	//set the controlled object to the mirror
	mControlObject = 0;

	mCurrentPP = 0;
	mCumulativeFTime = 0.0f;
	
	mTintCycleTime = 8.0f;

	mShockStrength = 0.2f;
	mShockSpeed = 20.0f;
	mShockLength = 2.0f;

	mDoubleVisionRadius = 0.05f;

	mBlurRadius = 10;

	multiprocess = false;
	ppDirection = true;

	mbGaussian = false;

	mContrastChange = 0.0f;
	mContrastFactor = 0.0f;

	mBlood = 0.5;

	impact = false;
	mSolariseInt = 127;
	mSolariseFloat = 0.5f;
}

CScene::~CScene(void)
{
}

//--------------------------------------------------------------------------------------
// Scene Setup / Update / Rendering
//--------------------------------------------------------------------------------------

// Create / load the camera, models and textures for the scene
bool CScene::InitScene()
{
	//////////////////
	// Create camera

	Camera = new CCamera();
	Camera->SetPosition( D3DXVECTOR3(-15, 20,-40) );
	Camera->SetRotation( D3DXVECTOR3(ToRadians(13.0f), ToRadians(18.0f), 0.0f) ); // ToRadians is a new helper function to convert degrees to radians
	Camera->SetNearClip( 1.0f );

	//load all textures and maps
	if (FAILED( D3DX10CreateShaderResourceViewFromFile( mpd3dDevice, L"StoneDiffuseSpecular.dds",		NULL, NULL, &mpMaps[0],  NULL ) )) return false;
	if (FAILED( D3DX10CreateShaderResourceViewFromFile( mpd3dDevice, L"WoodDiffuseSpecular.dds",		NULL, NULL, &mpMaps[1],  NULL ) )) return false;
	if (FAILED( D3DX10CreateShaderResourceViewFromFile( mpd3dDevice, L"Moon.jpg",						NULL, NULL, &mpMaps[2],  NULL ) )) return false; 
	if (FAILED( D3DX10CreateShaderResourceViewFromFile( mpd3dDevice, L"WallDiffuseSpecular.dds",		NULL, NULL, &mpMaps[3],  NULL ) )) return false;
	if (FAILED( D3DX10CreateShaderResourceViewFromFile( mpd3dDevice, L"WallNormalDepth.dds",			NULL, NULL, &mpMaps[4],  NULL ) )) return false;
	if (FAILED( D3DX10CreateShaderResourceViewFromFile( mpd3dDevice, L"MetalDiffuseSpecular.dds",		NULL, NULL, &mpMaps[5],  NULL ) )) return false;
	if (FAILED( D3DX10CreateShaderResourceViewFromFile( mpd3dDevice, L"PatternDiffuseSpecular.dds",		NULL, NULL, &mpMaps[6],  NULL ) )) return false;
	if (FAILED( D3DX10CreateShaderResourceViewFromFile( mpd3dDevice, L"PatternNormalDepth.dds",			NULL, NULL, &mpMaps[7],  NULL ) )) return false;
	if (FAILED( D3DX10CreateShaderResourceViewFromFile( mpd3dDevice, L"Glass.jpg",						NULL, NULL, &mpMaps[8],  NULL ) )) return false;
	if (FAILED( D3DX10CreateShaderResourceViewFromFile( mpd3dDevice, L"Troll1DiffuseSpecular.dds",		NULL, NULL, &mpMaps[9],  NULL ) )) return false;
	if (FAILED( D3DX10CreateShaderResourceViewFromFile( mpd3dDevice, L"Troll2DiffuseSpecular.dds",		NULL, NULL, &mpMaps[10], NULL ) )) return false;
	if (FAILED( D3DX10CreateShaderResourceViewFromFile( mpd3dDevice, L"Troll3DiffuseSpecular.dds",		NULL, NULL, &mpMaps[11], NULL ) )) return false;
	if (FAILED( D3DX10CreateShaderResourceViewFromFile( mpd3dDevice, L"Troll4DiffuseSpecular.dds",		NULL, NULL, &mpMaps[12], NULL ) )) return false;
	if (FAILED( D3DX10CreateShaderResourceViewFromFile( mpd3dDevice, L"BrainDiffuseSpecular.dds",		NULL, NULL, &mpMaps[13], NULL ) )) return false;
	if (FAILED( D3DX10CreateShaderResourceViewFromFile( mpd3dDevice, L"BrainNormalDepth.dds",			NULL, NULL, &mpMaps[14], NULL ) )) return false;
	if (FAILED( D3DX10CreateShaderResourceViewFromFile( mpd3dDevice, L"Blood.jpg",                      NULL, NULL, &mpMaps[15], NULL ) )) return false;
	miNumMaps = 16;

	//Create the mirror as object 0, makes life easier. 
	mpObjects[0] = new CRenderObject("Mirror.x",	D3DXVECTOR3( -20, 30, 0 ),		D3DXVECTOR3(0.2f, 0.2f, 0.3f), mTechniquesMirror[18], mTechniquesMirror[19], NULL, NULL, false, false, false);
	mpObjects[0]->GetModel()->SetRotation( D3DXVECTOR3(1, 2, 0)  );
	mpObjects[0]->GetModel()->UpdateMatrix();
	miNumObjects++;

	//Load items for basic pass
	if(! BasicItems() ) return false;
	//Load test objects
	if(! TestItems() ) return false;
	//Load diarama
	if(! SceneItems() ) return false;

	
	return true;
}

void TW_CALL ToggleSingle(void* clientData)
{
	CScene* temp = static_cast<CScene*>(clientData);
	temp->multiprocess = !temp->multiprocess;
}

void TW_CALL SetPP00(void* clientData)
{
	CScene* temp = static_cast<CScene*>(clientData);
	temp->SetSinglePP( 0 );
}

void TW_CALL SetPP01(void* clientData)
{
	CScene* temp = static_cast<CScene*>(clientData);
	temp->SetSinglePP( 1 );
}

void TW_CALL SetPP02(void* clientData)
{
	CScene* temp = static_cast<CScene*>(clientData);
	temp->SetSinglePP( 2 );
	temp->ResetShock();
}

void TW_CALL SetPP03(void* clientData)
{
	CScene* temp = static_cast<CScene*>(clientData);
	temp->SetSinglePP( 3 );
}

void TW_CALL SetPP04(void* clientData)
{
	CScene* temp = static_cast<CScene*>(clientData);
	temp->SetSinglePP( 4 );
}

void TW_CALL SetPP05(void* clientData)
{
	CScene* temp = static_cast<CScene*>(clientData);
	temp->SetSinglePP( 5 );
}

void TW_CALL SetPP06(void* clientData)
{
	CScene* temp = static_cast<CScene*>(clientData);
	temp->SetSinglePP( 6 );
}

void TW_CALL SetPP07(void* clientData)
{
	CScene* temp = static_cast<CScene*>(clientData);
	temp->SetSinglePP( 7 );
}

void TW_CALL SetPP08(void* clientData)
{
	CScene* temp = static_cast<CScene*>(clientData);
	temp->SetSinglePP( 8 );
}

void TW_CALL SetPP09(void* clientData)
{
	CScene* temp = static_cast<CScene*>(clientData);
	temp->SetSinglePP( 9 );
}

void TW_CALL SetPP10(void* clientData)
{
	CScene* temp = static_cast<CScene*>(clientData);
	temp->SetSinglePP( 10 );
}

void TW_CALL SetPP11(void* clientData)
{
	CScene* temp = static_cast<CScene*>(clientData);
	temp->SetSinglePP( 11 );
}

void TW_CALL SetPP12(void* clientData)
{
	CScene* temp = static_cast<CScene*>(clientData);
	temp->SetSinglePP( 12 );
}

void TW_CALL SetGauss(void* clientData)
{
	CScene* temp = static_cast<CScene*>(clientData);
	temp->mbGaussian = !temp->mbGaussian;
}

void TW_CALL TWImpact(void* clientData)
{
	CScene* temp = static_cast<CScene*>(clientData);
	temp->StartImpact();
}

void CScene::SetSinglePP(int index)
{
	if(index < 0)
	{
		mCurrentPP = 0;
		return;
	}

	if(index > NumPostProcesses)
	{
		mCurrentPP = 0;
		return;
	}

	mCurrentPP = index;
}

void CScene::ResetShock()
{
	mShockTime = mShockLength;	
}

bool CScene::InitATB()
{
	TwInit(TW_DIRECT3D10, mpd3dDevice);
	TwWindowSize(mViewportWidth, mViewportHeight);

	mtwBarPP = TwNewBar("PostProcessing");
	TwDefine(" PostProcessing position='5 5' ");
	TwAddVarRW(mtwBarPP, "Tint Cycle Time", TW_TYPE_FLOAT, &mTintCycleTime, "min=0.1 max=30 step=0.1");
	TwAddSeparator(mtwBarPP, "", "");
	TwAddVarRW(mtwBarPP, "Shock Strength", TW_TYPE_FLOAT, &mShockStrength, "min=0.01 max=0.5 step=0.01");
	TwAddVarRW(mtwBarPP, "Shock Speed", TW_TYPE_FLOAT, &mShockSpeed, "min=0.1 max=30 step=0.1");
	TwAddVarRW(mtwBarPP, "Shock Length", TW_TYPE_FLOAT, &mShockLength, "min=0.1 max=5 step=0.1");
	TwAddSeparator(mtwBarPP, "", "");
	TwAddVarRW(mtwBarPP, "Blur Radius", TW_TYPE_INT32, &mBlurRadius, "min=2 max=50 step=1");
	TwAddSeparator(mtwBarPP, "", "");
	TwAddVarRW(mtwBarPP, "Double Vision Radius", TW_TYPE_FLOAT, &mDoubleVisionRadius, "min=0 max=0.5 step=0.005");
	TwAddSeparator(mtwBarPP, "", "");
	TwAddVarRW(mtwBarPP, "Contrast Shift", TW_TYPE_FLOAT, &mContrastChange, "min=-128 max=128 step=1");
	//TwAddVarRO(mtwBarPP, "Contrast Factor", TW_TYPE_FLOAT, &mContrastFactor, "");
	TwAddVarRW(mtwBarPP, "Jamminess", TW_TYPE_FLOAT, &mBlood, "min=0 max=1 step=0.05");
	TwAddSeparator(mtwBarPP, "", "");
	TwAddVarRW(mtwBarPP, "Solarise Threshold", TW_TYPE_INT32, &mSolariseInt, "min=32 max=222 step=1");
	TwAddSeparator(mtwBarPP, "", "");
	TwAddVarRO(mtwBarPP, "Using multiple: ", TW_TYPE_BOOLCPP, &multiprocess, "");
	TwAddButton( mtwBarPP, "Toggle Single", ToggleSingle, this, "");
	
	mtwBarSinglePP = TwNewBar("Single Pass Select");
	TwDefine(" 'Single Pass Select' position='210 5' ");
	TwAddButton(mtwBarSinglePP, "No PP", SetPP00, this, "");
	TwAddButton(mtwBarSinglePP, "Tint", SetPP01, this, "");
	TwAddButton(mtwBarSinglePP, "Shock", SetPP02, this, "");
	TwAddButton(mtwBarSinglePP, "Box Blur (3)", SetPP03, this, "");
	TwAddButton(mtwBarSinglePP, "Box Blur (5)", SetPP04, this, "");
	TwAddButton(mtwBarSinglePP, "Box Blur (Custom)", SetPP05, this, "");
	TwAddButton(mtwBarSinglePP, "Double Vision", SetPP06, this, "");
	TwAddButton(mtwBarSinglePP, "Edges", SetPP07, this, "");
	TwAddButton(mtwBarSinglePP, "Contrast", SetPP08, this, "");
	TwAddButton(mtwBarSinglePP, "Jam on the screen", SetPP09, this, "");
	TwAddButton(mtwBarSinglePP, "Invert", SetPP10, this, "");
	TwAddButton(mtwBarSinglePP, "Solarise (Above)", SetPP11, this, "");
	TwAddButton(mtwBarSinglePP, "Solarise (Below)", SetPP12, this, "");
	TwAddSeparator(mtwBarSinglePP, "", "");
	TwAddButton(mtwBarSinglePP, "Toggle Gaussian Blur", SetGauss, this, "");

	mtwBarMultiPP = TwNewBar("Multi Pass Controls");
	TwDefine(" 'Multi Pass Controls' position='415 5' size='300 320' ");
	TwAddButton(mtwBarMultiPP, "Stop trying to hit me and hit me!", TWImpact, this, "");

	return true;
}

bool CScene::InitPP()
{
	mInitialTextureDesc.Width = mViewportWidth;
	mInitialTextureDesc.Height = mViewportHeight;
	mInitialTextureDesc.MipLevels = 1;
	mInitialTextureDesc.ArraySize = 1;
	mInitialTextureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	mInitialTextureDesc.SampleDesc.Count = 1;
	mInitialTextureDesc.SampleDesc.Quality = 0;
	mInitialTextureDesc.Usage = D3D10_USAGE_DEFAULT;
	mInitialTextureDesc.BindFlags = D3D10_BIND_RENDER_TARGET | D3D10_BIND_SHADER_RESOURCE;
	mInitialTextureDesc.CPUAccessFlags = 0;
	mInitialTextureDesc.MiscFlags = 0;
	if( FAILED( mpd3dDevice->CreateTexture2D( &mInitialTextureDesc, NULL, &mInitialTexture) ) ) return false;
	if (FAILED( mpd3dDevice->CreateTexture2D( &mInitialTextureDesc, NULL, &mTextureOne ) ) ) return false;
	if (FAILED( mpd3dDevice->CreateTexture2D( &mInitialTextureDesc, NULL, &mTextureTwo ) ) ) return false;

	if( FAILED( mpd3dDevice->CreateRenderTargetView( mInitialTexture, NULL, &mInitialRenderTarget ) ) ) return false;
	if (FAILED( mpd3dDevice->CreateRenderTargetView( mTextureOne, NULL, &mRenderTargetOne) ) ) return false;
	if (FAILED( mpd3dDevice->CreateRenderTargetView( mTextureTwo, NULL, &mRenderTargetTwo) ) ) return false;

	mInitialShaderDesc.Format = mInitialTextureDesc.Format;
	mInitialShaderDesc.ViewDimension = D3D10_SRV_DIMENSION_TEXTURE2D;
	mInitialShaderDesc.Texture2D.MostDetailedMip = 0;
	mInitialShaderDesc.Texture2D.MipLevels = 1;
	if( FAILED( mpd3dDevice->CreateShaderResourceView( mInitialTexture, &mInitialShaderDesc, &mInitialShaderResource ) ) ) return false;
	if (FAILED( mpd3dDevice->CreateShaderResourceView( mTextureOne, &mInitialShaderDesc, &mTextureOneShader))) return false;
	if (FAILED( mpd3dDevice->CreateShaderResourceView( mTextureTwo, &mInitialShaderDesc, &mTextureTwoShader))) return false;

	ID3D10Blob* pErrors; // This strangely typed variable collects any errors when compiling the effect file
	DWORD dwShaderFlags = D3D10_SHADER_ENABLE_STRICTNESS; // These "flags" are used to set the compiler options

	// Load and compile the effect file
	HRESULT hr = D3DX10CreateEffectFromFile( L"PostProcess.fx", NULL, NULL, "fx_4_0", dwShaderFlags, 0, mpd3dDevice, NULL, NULL, &mPPEffect, &pErrors, NULL );
	if( FAILED( hr ) )
	{
		if (pErrors != 0)  MessageBox( NULL, CA2CT(reinterpret_cast<char*>(pErrors->GetBufferPointer())), L"Error", MB_OK ); // Compiler error: display error message
		else               MessageBox( NULL, L"Error loading Post Processing FX file. Ensure your FX file is in the same folder as this executable.", L"Error", MB_OK );  // No error message - probably file not found
		return false;
	}

	mPPTechniques[0]  = mPPEffect->GetTechniqueByName( "PPCopy" );
	mPPTechniques[1]  = mPPEffect->GetTechniqueByName( "PPTint" );
	mPPTechniques[2]  = mPPEffect->GetTechniqueByName( "PPShock" );
	mPPTechniques[3]  = mPPEffect->GetTechniqueByName( "PPBoxBlur3" );
	mPPTechniques[4]  = mPPEffect->GetTechniqueByName( "PPBoxBlur5" );
	mPPTechniques[5]  = mPPEffect->GetTechniqueByName( "PPBoxBlurV" );
	mPPTechniques[6]  = mPPEffect->GetTechniqueByName( "PPDoubleVision" );
	mPPTechniques[7]  = mPPEffect->GetTechniqueByName( "PPEdge" );
	mPPTechniques[8]  = mPPEffect->GetTechniqueByName( "PPContrast" );
	mPPTechniques[9]  = mPPEffect->GetTechniqueByName( "PPBlood" );
	mPPTechniques[10] = mPPEffect->GetTechniqueByName( "PPInvert" );
	mPPTechniques[11] = mPPEffect->GetTechniqueByName( "PPSolariseA" );
	mPPTechniques[12] = mPPEffect->GetTechniqueByName( "PPSolariseB" );

	mInitialTextureVar = mPPEffect->GetVariableByName( "InitialTexture" )->AsShaderResource();
	mPostProcessMapVar = mPPEffect->GetVariableByName( "BloodTexture" )->AsShaderResource();

	mdxPPTintColour = mPPEffect->GetVariableByName( "TintColour" )->AsVector();

	mdxPPShock = mPPEffect->GetVariableByName( "Shock" )->AsScalar();

	mPPEffect->GetVariableByName( "PixelX" )->AsScalar()->SetFloat(1.0f/mViewportWidth);
	mPPEffect->GetVariableByName( "PixelY" )->AsScalar()->SetFloat(1.0f/mViewportHeight);

	mdxBlurRadius = mPPEffect->GetVariableByName( "BlurRange" )->AsScalar();

	mdxDoubleVisionRadius = mPPEffect->GetVariableByName( "DVRange" )->AsScalar();

	mdxContrastFactor = mPPEffect->GetVariableByName( "Contrast" )->AsScalar();

	mdxBlood = mPPEffect->GetVariableByName( "Blood" )->AsScalar();

	mdxSolariseFloat = mPPEffect->GetVariableByName("SolariseThreshold")->AsScalar();

	return true;

}
// Update the scene - move/rotate each model and the camera, then update their matrices
void CScene::UpdateScene( float frameTime )
{
	//Show Framerate
	char caption[100];
	char buffer[6];
	
	//copy in base text
	strcpy_s( caption,  "Post Processing - FPS ");
	
	//calculate FPS
	int FPS = int ( 1.0f / frameTime );
	
	//convert to string in base 10 and add to caption
	_itoa_s( FPS,buffer,10 );
	strcat_s( caption, buffer );
	
	//add in object and light counts as above
	strcat_s( caption, " O: " );
	_itoa_s( miNumObjects, buffer, 10 );
	strcat_s( caption, buffer );

	strcat_s( caption, " L: " );
	_itoa_s( miNumLights, buffer, 10 );
	strcat_s( caption, buffer );

	//pass the new caption to the window
	SetWindowTextA( HWnd, caption );

	// Control camera position and update its matrices (view matrix, projection matrix) each frame
	// Don't be deceived into thinking that this is a new method to control models - the same code we used previously is in the camera class
	Camera->Control( frameTime, Key_Up, Key_Down, Key_Left, Key_Right, Key_W, Key_S, Key_A, Key_D );
	Camera->UpdateMatrices();
	
	// Control cube position and update its world matrix each frame
	mpObjects[mControlObject]->GetModel()->Control( frameTime, Key_I, Key_K, Key_J, Key_L, Key_U, Key_O, Key_Period, Key_Comma );

	if (impact)
	{
		UpdateImpact(frameTime);
	}
	else
	{
		mCumulativeFTime += frameTime;

		//Tint colour cycle
		float hue = fmodf(mCumulativeFTime, mTintCycleTime) / mTintCycleTime;
		fHSLToRGB(hue, 0.5f, 0.5f, mTintColour.x, mTintColour.y, mTintColour.z);

		//Shock update
		if (mShockTime > 0)
		{
			mShockTime -= frameTime;
			mShock = sin(mShockSpeed * mShockTime);
			mShock -= 0.5f;
			mShock *= mShockStrength;
		}
		else
		{
			mShock = 0;
		}

		//Calculate contrast factor
		mContrastFactor = (259 * (mContrastChange + 255.0f)) / (255.0f * (259.0f - mContrastChange));

		mSolariseFloat = mSolariseInt / 255.0f;

	}//end if impact

	//update all the objects, including calculating the matrix
	for(int i = 0; i < miNumObjects; i++)
	{
		mpObjects[i]->Update(frameTime);
	}

	//same for the lights
	for(int i = 0; i < miNumLights; i++)
	{
		mpLights[i]->Update(frameTime);
	}


	// Allow user to quit with escape key
	if (KeyHit( Key_Escape )) 
	{
		DestroyWindow( HWnd );
	}

	if( KeyHit( Key_F1  ) ) { mCurrentPP = 0; }
	if( KeyHit( Key_F2  ) ) { mCurrentPP = 1; }
	if( KeyHit( Key_F3  ) ) { mCurrentPP = 2;ResetShock();}
	if( KeyHit( Key_F4  ) ) { mCurrentPP = 3; }
	if( KeyHit( Key_F5  ) ) { mCurrentPP = 4; }
	if( KeyHit( Key_F6  ) ) { mCurrentPP = 5; }
	if( KeyHit( Key_F7  ) ) { mCurrentPP = 6; }
	if( KeyHit( Key_F8  ) ) { mCurrentPP = 7; }
	if( KeyHit( Key_F9  ) ) { mCurrentPP = 8; }
	if( KeyHit( Key_F10 ) ) { mCurrentPP = 9; }
	if( KeyHit( Key_F11 ) ) { mCurrentPP = 10; }
	if( KeyHit( Key_F12 ) ) { mCurrentPP = 11; }

	//change the controlled object
	if( KeyHit( Key_1 ) )
	{
		mControlObject = 0;
	}

	if( KeyHit( Key_2 ) )
	{
		mControlObject = 1;
	}

	if( KeyHit( Key_3 ) )
	{
		mControlObject = 2;
	}

	if( KeyHit( Key_4 ) )
	{
		mControlObject = 3;
	}

	if( KeyHit( Key_5 ) )
	{
		mControlObject = 4;
	}

	if( KeyHit( Key_6 ) )
	{
		mControlObject = 5;
	}

	if( KeyHit( Key_7 ) )
	{
		mControlObject = 6;
	}

	if( KeyHit( Key_8 ) )
	{
		mControlObject = 7;
	}

	if( KeyHit( Key_9 ) )
	{
		mControlObject = 8;
	}

	if( KeyHit( Key_0 ) )
	{
		mControlObject = 9;
	}

	if( KeyHit ( Key_Minus ) )
	{
		if ( mControlObject == 0 )
		{
			mControlObject = miNumObjects-1;
		}
		else
		{
			mControlObject--;
		}
	}

	if( KeyHit ( Key_Plus ) )
	{
		if ( mControlObject == miNumObjects-1 )
		{
			mControlObject = 0;
		}
		else
		{
			mControlObject++;
		}
	}
}

//function to pass data to the shaders and render the object.
void CScene::DrawObject(int i, bool mirror)
{
	//first the wiggle value
	float wiggle =  mpObjects[i]->GetWiggle();
	dxWiggle->SetRawValue( &wiggle, 0, 4);

	//outline thickness
	float thickness = mpObjects[i]->GetOutlineThickness();
	dxOutlineThickness->SetRawValue( &thickness, 0, 4);

	//pass the matrix
	WorldMatrixVar->SetMatrix( (float*)mpObjects[i]->GetModel()->GetWorldMatrix() );
	
	//pass the textures and maps, if there are any. 
	if( mpObjects[i]->GetTexture() != nullptr )
	{
		DiffuseMapVar->SetResource( mpObjects[i]->GetTexture() );
	}
	if( mpObjects[i]->GetNormalMap() != nullptr )
	{
		NormalMapVar->SetResource( mpObjects[i]->GetNormalMap() );
	}

	//pass the model colour - used for untextured models, texture colour change and outline colour in cell shading
	ModelColourVar->SetRawValue( mpObjects[i]->GetColourV(), 0, 12 );
	
	//if the object is lit, pass over the 12 closest lights. 
	if( mpObjects[i]->IsLit() )
	{
		SetLights( mpObjects[i]->GetModel()->GetPosition(), mpLights, miNumLights);
	}

	// choose how to render the model based on where we are rendering it. 
	if( mirror )
	{
		mpObjects[i]->RenderMirror();
	}
	else
	{
		mpObjects[i]->Render();
		
	}
}

// function to render everything in the scene
void CScene::DrawAllObjects(bool mirror)
{
	//
	// Solid objects
	//
	//start rendering AFTER the mirror.
	for(int i = 1; i < miNumObjects; i++)
	{
		if( !mpObjects[i]->IsTransparent() )
		{
			DrawObject(i, mirror);
		}
	}

	//
	// Light objects
	//


	for(int i = 0; i < miNumLights; i++)
	{
		WorldMatrixVar->SetMatrix( (float*)mpLights[i]->GetModel()->GetWorldMatrix() );
		D3DXVECTOR3 mColour =  mpLights[i]->GetColourV();
		ModelColourVar->SetRawValue(mColour, 0, 12 );
		if( mirror )
		{
			mpLights[i]->GetModel()->Render( mTechniquesMirror[0] );
		}
		else
		{
			mpLights[i]->GetModel()->Render( mTechniques[0] );
		}
	}

	//
	// Transparent objects
	//

	for(int i = 1; i < miNumObjects; i++)
	{
		if( mpObjects[i]->IsTransparent() )
		{
			DrawObject(i, mirror);
		}
	}
}

// Render everything in the scene
void CScene::RenderScene()
{
	//set render target to a texture for post processing
	mpd3dDevice->OMSetRenderTargets(1, &RenderTargetView, DepthStencilView);
	mpd3dDevice->ClearRenderTargetView(mInitialRenderTarget, AmbientColour);
	mpd3dDevice->ClearDepthStencilView(DepthStencilView, D3D10_CLEAR_DEPTH | D3D10_CLEAR_STENCIL, 1.0f, 0); // Clear the depth buffer too

	mpd3dDevice->OMSetRenderTargets( 1, &mInitialRenderTarget, DepthStencilView);

	// Clear the back buffer - before drawing the geometry clear the entire window to a fixed colour
	//float ClearColor[4] = { 0.2f, 0.2f, 0.3f, 1.0f }; // Good idea to match background to ambient colour
	mpd3dDevice->ClearRenderTargetView( mInitialRenderTarget, AmbientColour );
	mpd3dDevice->ClearDepthStencilView( DepthStencilView, D3D10_CLEAR_DEPTH | D3D10_CLEAR_STENCIL , 1.0f, 0 ); // Clear the depth buffer too

	//---------------------------
	// Common rendering settings

	// Common features for all models, set these once only

	// Pass the camera's matrices to the vertex shader
	ViewMatrixVar->SetMatrix( (float*)&Camera->GetViewMatrix() );
	ProjMatrixVar->SetMatrix( (float*)&Camera->GetProjectionMatrix() );

	//pass the camera position
	dxCameraPos->SetRawValue( D3DXVECTOR3( Camera->GetPosition()), 0, 12);

	//pass the lighting colours
	dxAmbientColour->SetRawValue( AmbientColour, 0, 12  );

	//---------------------------
	// Render each model
	
	RenderMirrors();
	DrawAllObjects(false);

	/////////////////////////////
	// POST PROCESS PASS
	/////////////////////////////
	
	//Pass the tint colour
	mdxPPTintColour->SetRawValue( mTintColour, 0, 12 );

	//Pass the shock value
	mdxPPShock->SetRawValue( &mShock, 0, 4 );

	//Pass the blur radius
	mdxBlurRadius->SetRawValue( &mBlurRadius, 0, 4 );

	//Pass the double vision radius
	mdxDoubleVisionRadius->SetRawValue( &mDoubleVisionRadius, 0, 4 );

	//Pass the contrast factor
	mdxContrastFactor->SetRawValue(&mContrastFactor, 0, 4);

	//Only one person would dare give me the raspberry....LONESTAR!!
	mdxBlood->SetRawValue(&mBlood, 0, 4);
	mPostProcessMapVar->SetResource(mpMaps[15]);

	mdxSolariseFloat->SetRawValue(&mSolariseFloat, 0, 4);

	if(mCurrentPP < 0) mCurrentPP = 0;
	if(mCurrentPP > NumPostProcesses ) mCurrentPP = 0;

	if (impact)
	{
		RenderImpact();
	}
	else
	{
		if (mbGaussian)
		{

		}

		if (!multiprocess)
		{
			//Set render target back to output
			mpd3dDevice->OMSetRenderTargets(1, &RenderTargetView, DepthStencilView);
			//send rendered scene to shader as a texture
			mInitialTextureVar->SetResource(mInitialShaderResource);

			//Apply Post Processing Effect
			mpd3dDevice->IASetInputLayout(NULL);
			mpd3dDevice->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
			mPPTechniques[mCurrentPP]->GetPassByIndex(0)->Apply(0);

			mpd3dDevice->Draw(4, 0);

			mInitialTextureVar->SetResource(0);
			mPPTechniques[mCurrentPP]->GetPassByIndex(0)->Apply(0);
		}
		else
		{

			//Box Blur
			mpd3dDevice->OMSetRenderTargets(1, &RenderTargetView, DepthStencilView);
			mInitialTextureVar->SetResource(mInitialShaderResource);
			mpd3dDevice->IASetInputLayout(NULL);
			mpd3dDevice->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
			mPPTechniques[5]->GetPassByIndex(0)->Apply(0);
			mpd3dDevice->Draw(4, 0);
			mInitialTextureVar->SetResource(0);
			mPPTechniques[5]->GetPassByIndex(0)->Apply(0);

			//Double Vision
			mpd3dDevice->OMSetRenderTargets(1, &RenderTargetView, DepthStencilView);
			mInitialTextureVar->SetResource(mTextureOneShader);
			mpd3dDevice->IASetInputLayout(NULL);
			mpd3dDevice->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
			mPPTechniques[6]->GetPassByIndex(0)->Apply(0);
			mpd3dDevice->Draw(4, 0);
			mInitialTextureVar->SetResource(0);
			mPPTechniques[6]->GetPassByIndex(0)->Apply(0);

			//Copy to Output
			/*mpd3dDevice->OMSetRenderTargets(1, &RenderTargetView, DepthStencilView);
			mInitialTextureVar->SetResource(mTextureTwoShader);
			mpd3dDevice->IASetInputLayout(NULL);
			mpd3dDevice->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
			mPPTechniques[0]->GetPassByIndex(0)->Apply(0);
			mpd3dDevice->Draw(4, 0);
			mInitialTextureVar->SetResource(0);
			mPPTechniques[0]->GetPassByIndex(0)->Apply(0);
			*/
		}
	}//end else impact
	//---------------------------
	// Display the Scene

	// After we've finished drawing to the off-screen back buffer, we "present" it to the front buffer (the screen)
	TwDraw();
	SwapChain->Present( 0, 0 );
}

void CScene::StartImpact()
{
	if (!impact)
	{
		impact = true;
		mShockTime = mShockLength;
	}
}

void CScene::UpdateImpact(float frameTime)
{
	if (mShockTime < 0.0f)
	{
		impact = false; 
		return;
	}

	//mShock = sin(mShockSpeed * mShockTime);
	//mShock -= 0.5f;
	//mShock *= mShockStrength;
	
	

	//build phase
	if (mShockTime > 1.0f)
	{
		mContrastChange = -128.0f * (1.0f - (mShockTime - 1.0f));
		mTintColour.x = 1.0f;
		mTintColour.y = 1.0f - (1.0f - (mShockTime - 1.0f));
		mTintColour.z = 1.0f - (1.0f - (mShockTime - 1.0f));
		mBlurRadius = 20 * (1.0f - (mShockTime - 1.0f));
		mShock = mShockStrength * sin( mShockSpeed * (1.0f - (mShockTime - 1.0f)) );
	}
	else //fade phase
	{
		mContrastChange = -128.0f * mShockTime;
		mTintColour.x = 1.0f;
		mTintColour.y = 1.0f - mShockTime;
		mTintColour.z = 1.0f - mShockTime;
		mBlurRadius = 20 * mShockTime;
		mShock = mShockStrength * sin(mShockSpeed * mShockTime);
	}
	
	mContrastFactor = (259 * (mContrastChange + 255.0f)) / (255.0f * (259.0f - mContrastChange));

	mShockTime -= (frameTime/2.0f);
}

void CScene::RenderImpact()
{
	//PP One - Initial -> One, Blur.
	mpd3dDevice->OMSetRenderTargets(1, &mRenderTargetOne, DepthStencilView);
	mInitialTextureVar->SetResource(mInitialShaderResource);
	mpd3dDevice->IASetInputLayout(NULL);
	mpd3dDevice->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	mPPTechniques[5]->GetPassByIndex(0)->Apply(0);
	mpd3dDevice->Draw(4, 0);
	mInitialTextureVar->SetResource(0);
	mPPTechniques[5]->GetPassByIndex(0)->Apply(0);

	//PP Two = One -> Two, Contrast
	mpd3dDevice->OMSetRenderTargets(1, &mRenderTargetTwo, DepthStencilView);
	mInitialTextureVar->SetResource(mTextureOneShader);
	mpd3dDevice->IASetInputLayout(NULL);
	mpd3dDevice->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	mPPTechniques[8]->GetPassByIndex(0)->Apply(0);
	mpd3dDevice->Draw(4, 0);
	mInitialTextureVar->SetResource(0);
	mPPTechniques[8]->GetPassByIndex(0)->Apply(0);

	//PP Three = Two -> One, Tint
	mpd3dDevice->OMSetRenderTargets(1, &mRenderTargetOne, DepthStencilView);
	mInitialTextureVar->SetResource(mTextureTwoShader);
	mpd3dDevice->IASetInputLayout(NULL);
	mpd3dDevice->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	mPPTechniques[1]->GetPassByIndex(0)->Apply(0);
	mpd3dDevice->Draw(4, 0);
	mInitialTextureVar->SetResource(0);
	mPPTechniques[1]->GetPassByIndex(0)->Apply(0);

	//PP Four = One -> Two, Shake
	mpd3dDevice->OMSetRenderTargets(1, &mRenderTargetTwo, DepthStencilView);
	mInitialTextureVar->SetResource(mTextureOneShader);
	mpd3dDevice->IASetInputLayout(NULL);
	mpd3dDevice->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	mPPTechniques[2]->GetPassByIndex(0)->Apply(0);
	mpd3dDevice->Draw(4, 0);
	mInitialTextureVar->SetResource(0);
	mPPTechniques[2]->GetPassByIndex(0)->Apply(0);

	//Copy to Output
	mpd3dDevice->OMSetRenderTargets(1, &RenderTargetView, DepthStencilView);
	mInitialTextureVar->SetResource(mTextureTwoShader);
	mpd3dDevice->IASetInputLayout(NULL);
	mpd3dDevice->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	mPPTechniques[0]->GetPassByIndex(0)->Apply(0);
	mpd3dDevice->Draw(4, 0);
	mInitialTextureVar->SetResource(0);
	mPPTechniques[0]->GetPassByIndex(0)->Apply(0);

}

void CScene::RenderMirrors()
{
	
	//*******************************************************************************************************
	// First set the stencil value for all mirror pixels to 1 and clear the mirror to a fixed colour and 
	// set its depth-buffer values to maximum (so we can render "inside" the mirror)

	D3DXMATRIXA16 mirrorMatrix = mpObjects[0]->GetModel()->GetWorldMatrix();
	WorldMatrixVar->SetMatrix( (float*)mirrorMatrix );
	ModelColourVar->SetRawValue( mpObjects[0]->GetColourV(), 0, 12 );
	mpObjects[0]->Render();

	//*******************************************************************************************************
	// Next reflect the camera in the mirror

	// Some mathematics to get as reflected version of the camera - using DirectX helper functions mostly

	// Create a plane for the mirror
	D3DXPLANE mirrorPlane;
	D3DXVECTOR3 mirrorPoint  = D3DXVECTOR3( mirrorMatrix(3,0), mirrorMatrix(3,1), mirrorMatrix(3,2) );
	D3DXVECTOR3 mirrorNormal = D3DXVECTOR3( mirrorMatrix(2,0), mirrorMatrix(2,1), mirrorMatrix(2,2) );
	D3DXPlaneFromPointNormal( &mirrorPlane, &mirrorPoint, &mirrorNormal );
	
	// Reflect the camera's view matrix in the mirror plane
	D3DXMATRIXA16 reflectMatrix;
	D3DXMatrixReflect( &reflectMatrix, &mirrorPlane );
	D3DXMATRIXA16 reflectViewMatrix = reflectMatrix * Camera->GetViewMatrix();

	// Reflect the camera's position in the mirror plane
	D3DXVECTOR3 cameraPos = Camera->GetPosition();
	D3DXVECTOR4 reflectCameraPos4; // Initially generate a 4 element vector
	D3DXVec3Transform( &reflectCameraPos4, &cameraPos, &reflectMatrix );
	D3DXVECTOR3 reflectCameraPos = D3DXVECTOR3( (float*)reflectCameraPos4 ); // Drop 4th element


	//*******************************************************************************************************
	// Render all the models "inside" the mirror

	// Set the reflected camera data in the shaders
	ViewMatrixVar->SetMatrix( (float*)&reflectViewMatrix );
	dxCameraPos->SetRawValue( reflectCameraPos, 0, 12 );
	ClipPlaneVar->SetRawValue( mirrorPlane, 0, 16 );

	// Need to use slightly different techniques to avoid mirror rendering being "inside out"
	
	DrawAllObjects(true);

	// Restore main camera data in the shaders
	ViewMatrixVar->SetMatrix( (float*)&Camera->GetViewMatrix() );
	dxCameraPos->SetRawValue( Camera->GetPosition(), 0, 12 );
	ClipPlaneVar->SetRawValue( D3DXVECTOR4(0,0,0,0), 0, 16 );


	//*******************************************************************************************************
	// Finally draw a "surface" for the mirror - a transparent layer over the mirror contents. This sets the correct depth-buffer values 
	// for the mirror surface, so further rendering won't go "inside" the mirrored scene

	WorldMatrixVar->SetMatrix( (float*)mpObjects[0]->GetModel()->GetWorldMatrix() );
	mpObjects[0]->RenderMirror();
}


//--------------------------------------------------------------------------------------
// Register class and create window
//--------------------------------------------------------------------------------------
bool CScene::InitWindow( HINSTANCE hInstance, int nCmdShow )
{
	// Register class
	WNDCLASSEX wcex;
	wcex.cbSize = sizeof( WNDCLASSEX );
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = CScene::WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon( hInstance, ( LPCTSTR )IDI_TUTORIAL1 );
	wcex.hCursor = LoadCursor( NULL, IDC_ARROW );
	wcex.hbrBackground = ( HBRUSH )( COLOR_WINDOW + 1 );
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = L"TutorialWindowClass";
	wcex.hIconSm = LoadIcon( wcex.hInstance, ( LPCTSTR )IDI_TUTORIAL1 );
	if( !RegisterClassEx( &wcex ) )	return false;

	// Create window
	HInst = hInstance;
	RECT rc = { 0, 0, 1920, 960 };
	AdjustWindowRect( &rc, WS_OVERLAPPEDWINDOW, FALSE );
	HWnd = CreateWindow( L"TutorialWindowClass", L"CO2409 - Graphics Assignment - Shaders", WS_OVERLAPPEDWINDOW,
	                     CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top, NULL, NULL, hInstance, NULL );
	if( !HWnd )	return false;

	ShowWindow( HWnd, nCmdShow );

	return true;
}

//--------------------------------------------------------------------------------------
// Called every time the application receives a message
//--------------------------------------------------------------------------------------
LRESULT CALLBACK CScene::WndProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	PAINTSTRUCT ps;
	HDC hdc;
	if( TwEventWin(hWnd, message, wParam, lParam) ) // send event message to AntTweakBar
        return 0;

	switch( message )
	{
		case WM_PAINT:
			hdc = BeginPaint( hWnd, &ps );
			EndPaint( hWnd, &ps );
			break;

		case WM_DESTROY:
			PostQuitMessage( 0 );
			break;

		// These windows messages (WM_KEYXXXX) can be used to get keyboard input to the window
		// This application has added some simple functions (not DirectX) to process these messages (all in Input.cpp/h)
		case WM_KEYDOWN:
			KeyDownEvent( static_cast<EKeyState>(wParam) );
			break;

		case WM_KEYUP:
			KeyUpEvent( static_cast<EKeyState>(wParam) );
			break;
		// catch window resize
		case WM_SIZE:
			RECT rc;
			GetClientRect( hWnd, &rc );
			mViewportWidth = rc.right - rc.left;
			mViewportHeight = rc.bottom - rc.top;
			// pass to camera
			CCamera::SetViewport( mViewportWidth, mViewportHeight );
			break;
		default:
			return DefWindowProc( hWnd, message, wParam, lParam );
	}

	return 0;
}


//--------------------------------------------------------------------------------------
// Create Direct3D device and swap chain
//--------------------------------------------------------------------------------------
bool CScene::InitDevice()
{
	// Many DirectX functions return a "HRESULT" variable to indicate success or failure. Microsoft code often uses
	// the FAILED macro to test this variable, you'll see it throughout the code - it's fairly self explanatory.
	HRESULT hr = S_OK;


	////////////////////////////////
	// Initialise Direct3D

	// Calculate the visible area the window we are using - the "client rectangle" refered to in the first function is the 
	// size of the interior of the window, i.e. excluding the frame and title
	RECT rc;
	GetClientRect( HWnd, &rc );
	mViewportWidth = rc.right - rc.left;
	mViewportHeight = rc.bottom - rc.top;


	// Create a Direct3D device (i.e. initialise D3D), and create a swap-chain (create a back buffer to render to)
	DXGI_SWAP_CHAIN_DESC sd;         // Structure to contain all the information needed
	ZeroMemory( &sd, sizeof( sd ) ); // Clear the structure to 0 - common Microsoft practice, not really good style
	sd.BufferCount = 1;
	sd.BufferDesc.Width = mViewportWidth;             // Target window size
	sd.BufferDesc.Height = mViewportHeight;           // --"--
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // Pixel format of target window
	sd.BufferDesc.RefreshRate.Numerator = 60;          // Refresh rate of monitor
	sd.BufferDesc.RefreshRate.Denominator = 1;         // --"--
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.OutputWindow = HWnd;                          // Target window
	sd.Windowed = TRUE;                                // Whether to render in a window (TRUE) or go fullscreen (FALSE)
	hr = D3D10CreateDeviceAndSwapChain( NULL, D3D10_DRIVER_TYPE_HARDWARE, NULL, D3D10_CREATE_DEVICE_DEBUG,
										D3D10_SDK_VERSION, &sd, &SwapChain, &mpd3dDevice );
	if( FAILED( hr ) ) return false;


	// Specify the render target as the back-buffer - this is an advanced topic. This code almost always occurs in the standard D3D setup
	ID3D10Texture2D* pBackBuffer;
	hr = SwapChain->GetBuffer( 0, __uuidof( ID3D10Texture2D ), ( LPVOID* )&pBackBuffer );
	if( FAILED( hr ) ) return false;
	hr = mpd3dDevice->CreateRenderTargetView( pBackBuffer, NULL, &RenderTargetView );
	pBackBuffer->Release();
	if( FAILED( hr ) ) return false;


	// Create a texture (bitmap) to use for a depth buffer
	D3D10_TEXTURE2D_DESC descDepth;
	descDepth.Width = mViewportWidth;
	descDepth.Height = mViewportHeight;
	descDepth.MipLevels = 1;
	descDepth.ArraySize = 1;
	descDepth.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;		// 24 bits for depth, 8 for stencil - more memory efficient
	//descDepth.Format = DXGI_FORMAT_D32_FLOAT;				// 32 bit float depth only 
	//descDepth.Format = DXGI_FORMAT_D32_FLOAT_S8X24_UINT;	// 32 bit depth + 8 for stencil, 24 unused bytes - more precise depth buffer
	descDepth.SampleDesc.Count = 1;
	descDepth.SampleDesc.Quality = 0;
	descDepth.Usage = D3D10_USAGE_DEFAULT;
	descDepth.BindFlags = D3D10_BIND_DEPTH_STENCIL;
	descDepth.CPUAccessFlags = 0;
	descDepth.MiscFlags = 0;
	hr = mpd3dDevice->CreateTexture2D( &descDepth, NULL, &DepthStencil );
	if( FAILED( hr ) ) return false;

	// Create the depth stencil view, i.e. indicate that the texture just created is to be used as a depth buffer
	D3D10_DEPTH_STENCIL_VIEW_DESC descDSV;
	descDSV.Format = descDepth.Format;
	descDSV.ViewDimension = D3D10_DSV_DIMENSION_TEXTURE2D;
	descDSV.Texture2D.MipSlice = 0;
	hr = mpd3dDevice->CreateDepthStencilView( DepthStencil, &descDSV, &DepthStencilView );
	if( FAILED( hr ) ) return false;

	// Select the back buffer and depth buffer to use for rendering now
	mpd3dDevice->OMSetRenderTargets( 1, &RenderTargetView, DepthStencilView );


	// Setup the viewport - defines which part of the window we will render to, almost always the whole window
	D3D10_VIEWPORT vp;
	vp.Width  = mViewportWidth;
	vp.Height = mViewportHeight;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	mpd3dDevice->RSSetViewports( 1, &vp );

	CCamera::SetViewport( mViewportWidth, mViewportHeight );
	
	CModel::SetDevice( mpd3dDevice );

	//test->SetDevice( mpd3dDevice );

	return true;
}


// Release the memory held by all objects created
void CScene::ReleaseResources()
{
	// The D3D setup and preparation of the geometry created several objects that use up memory (e.g. textures, vertex/index buffers etc.)
	// Each object that allocates memory (or hardware resources) needs to be "released" when we exit the program
	// There is similar code in every D3D program, but the list of objects that need to be released depends on what was created
	// Test each variable to see if it exists before deletion
	if( mpd3dDevice )     mpd3dDevice->ClearState();

	delete Camera;

	for(int i = 0; i < miNumObjects; i++)
	{
		delete mpObjects[i];
	}

	for(int i = 0; i < miNumLights; i++)
	{
		delete mpLights[i];
	}
	
	for(int i = 0; i < miNumMaps; i++)
	{
		if ( mpMaps[i] )
			mpMaps[i]->Release();
	}

	if( Effect )           Effect->Release();
	if( DepthStencilView ) DepthStencilView->Release();
	if( RenderTargetView ) RenderTargetView->Release();
	if( DepthStencil )     DepthStencil->Release();
	if( SwapChain )        SwapChain->Release();
	if( mpd3dDevice )     mpd3dDevice->Release();

}



//--------------------------------------------------------------------------------------
// Load and compile Effect file (.fx file containing shaders)
//--------------------------------------------------------------------------------------
// An effect file contains a set of "Techniques". A technique is a combination of vertex, geometry and pixel shaders (and some states) used for
// rendering in a particular way. We load the effect file at runtime (it's written in HLSL and has the extension ".fx"). The effect code is compiled
// *at runtime* into low-level GPU language. When rendering a particular model we specify which technique from the effect file that it will use

bool CScene::LoadEffectFile()
{
	ID3D10Blob* pErrors; // This strangely typed variable collects any errors when compiling the effect file
	DWORD dwShaderFlags = D3D10_SHADER_ENABLE_STRICTNESS; // These "flags" are used to set the compiler options

	// Load and compile the effect file
	HRESULT hr = D3DX10CreateEffectFromFile( L"GraphicsAssign1.fx", NULL, NULL, "fx_4_0", dwShaderFlags, 0, mpd3dDevice, NULL, NULL, &Effect, &pErrors, NULL );
	if( FAILED( hr ) )
	{
		if (pErrors != 0)  MessageBox( NULL, CA2CT(reinterpret_cast<char*>(pErrors->GetBufferPointer())), L"Error", MB_OK ); // Compiler error: display error message
		else               MessageBox( NULL, L"Error loading FX file. Ensure your FX file is in the same folder as this executable.", L"Error", MB_OK );  // No error message - probably file not found
		return false;
	}

	// Now we can select techniques from the compiled effect file
	// Regular techniques for the main scene
	mTechniques[ 0]	= Effect->GetTechniqueByName( "tPlainColour" );
	mTechniques[ 1]	= Effect->GetTechniqueByName( "tPlainTexture" );
	mTechniques[ 2]	= Effect->GetTechniqueByName( "tColourChangeTexture" );
	mTechniques[ 3]	= Effect->GetTechniqueByName( "tLitTexture" );
	mTechniques[ 4]	= Effect->GetTechniqueByName( "t4LitTexture" );
	mTechniques[ 5] = Effect->GetTechniqueByName( "t4LitAttenTexture" );
	mTechniques[ 6]	= Effect->GetTechniqueByName( "t4LitSpecularMap" );
	mTechniques[ 7]	= Effect->GetTechniqueByName( "tParallaxMap" );
	mTechniques[ 8]	= Effect->GetTechniqueByName( "tJelly" );
	mTechniques[ 9]	= Effect->GetTechniqueByName( "tCellShading" );
	mTechniques[10] = Effect->GetTechniqueByName( "tCellShadingWrong" );
	mTechniques[11] = Effect->GetTechniqueByName( "tHalo" );
	mTechniques[12] = Effect->GetTechniqueByName( "tSpaceHaloPara" );
	mTechniques[13] = Effect->GetTechniqueByName( "tSpaceOutlinePara" );
	mTechniques[14] = Effect->GetTechniqueByName( "tLitGlass" );
	mTechniques[15] = Effect->GetTechniqueByName( "tSpaceHalo" );
	mTechniques[16] = Effect->GetTechniqueByName( "tSpaceOutline" );
	mTechniques[17] = Effect->GetTechniqueByName( "tTextureMorph" );
	mTechniques[18] = Effect->GetTechniqueByName( "MirrorClear" );		//added to both just to keep the enum consistant.
	mTechniques[19] = Effect->GetTechniqueByName( "MirrorSurface" );	
	miNumTechniques = 20;

	// Techniques for the mirrored objects
	mTechniquesMirror[ 0] = Effect->GetTechniqueByName( "tPlainColourMirror" );
	mTechniquesMirror[ 1] = Effect->GetTechniqueByName( "tPlainTextureMirror" );
	mTechniquesMirror[ 2] = Effect->GetTechniqueByName( "tColourChangeTextureMirror" );
	mTechniquesMirror[ 3] = Effect->GetTechniqueByName( "tLitTextureMirror" );
	mTechniquesMirror[ 4] = Effect->GetTechniqueByName( "t4LitTextureMirror" );
	mTechniquesMirror[ 5] = Effect->GetTechniqueByName( "t4LitAttenTextureMirror" );
	mTechniquesMirror[ 6] = Effect->GetTechniqueByName( "t4LitSpecularMapMirror" );
	mTechniquesMirror[ 7] = Effect->GetTechniqueByName( "tParallaxMapMirror" );
	mTechniquesMirror[ 8] = Effect->GetTechniqueByName( "tJellyMirror" );
	mTechniquesMirror[ 9] = Effect->GetTechniqueByName( "tCellShadingMirror" );
	mTechniquesMirror[10] = Effect->GetTechniqueByName( "tCellShadingWrongMirror" );
	mTechniquesMirror[11] = Effect->GetTechniqueByName( "tHaloMirror" );
	mTechniquesMirror[12] = Effect->GetTechniqueByName( "tSpaceHaloParaMirror" );
	mTechniquesMirror[13] = Effect->GetTechniqueByName( "tSpaceOutlineParaMirror" );
	mTechniquesMirror[14] = Effect->GetTechniqueByName( "tLitGlassMirror" );
	mTechniquesMirror[15] = Effect->GetTechniqueByName( "tSpaceHaloMirror" );
	mTechniquesMirror[16] = Effect->GetTechniqueByName( "tSpaceOutlineMirror" );
	mTechniquesMirror[17] = Effect->GetTechniqueByName( "tTextureMorphMirror" );
	mTechniquesMirror[18] = Effect->GetTechniqueByName( "MirrorClear" );
	mTechniquesMirror[19] = Effect->GetTechniqueByName( "MirrorSurface" );

	// Create special variables to allow us to access global variables in the shaders from C++
	WorldMatrixVar		= Effect->GetVariableByName( "WorldMatrix" )->AsMatrix();
	ViewMatrixVar		= Effect->GetVariableByName( "ViewMatrix"  )->AsMatrix();
	ProjMatrixVar		= Effect->GetVariableByName( "ProjMatrix"  )->AsMatrix();

	// We access the texture variable in the shader in the same way as we have before for matrices, light data etc.
	// Only difference is that this variable is a "Shader Resource"
	DiffuseMapVar = Effect->GetVariableByName( "DiffuseMap" )->AsShaderResource();
	NormalMapVar = Effect->GetVariableByName( "NormalMap" )->AsShaderResource();

	//Camera Position
	dxCameraPos = Effect->GetVariableByName( "cameraPos" )->AsVector();

	// Other shader variables
	ModelColourVar = Effect->GetVariableByName( "ModelColour"  )->AsVector();

	//Lighting Values
	dxLightPosA = Effect->GetVariableByName( "lightPos" )->AsVector();
	dxLightColourA = Effect->GetVariableByName( "lightColour" )->AsVector();
	dxLightBrightnessA = Effect->GetVariableByName( "lightBright" )->AsVector();
	
	dxAmbientColour = Effect->GetVariableByName( "ambientColour" )->AsVector();

	dxWiggle = Effect->GetVariableByName( "wiggle" )->AsScalar();
	dxOutlineThickness = Effect->GetVariableByName( "OutlineThickness" )->AsScalar();

	ClipPlaneVar      = Effect->GetVariableByName( "ClipPlane"      )->AsVector();

	return true;
}

//--------------------------------------------------------------------------------------
// Light helper functions
//--------------------------------------------------------------------------------------


struct LightIndex
{
	int index;
	float distance;
};

int CScene::compareLights( const void* a, const void* b)
{
	LightIndex* x = (LightIndex*)a;
	LightIndex* y = (LightIndex*)b;
	return int(x->distance - y->distance);
}

// calculates the closest lights to the object to pass to the shader. 
void CScene::SetLights( D3DXVECTOR3 source, CLight* lightsSource[MAX_LIGHTS], int lightCount)
{
	LightIndex indexes[MAX_LIGHTS];

	for(int i = 0; i < MAX_LIGHTS; i++)
	{
		indexes[i].distance = 777777777.0f;
	}

	D3DXVECTOR3 vLight;
	for(int i = 0; i < lightCount; i++)
	{
		indexes[i].index = i;
		vLight = lightsSource[i]->GetModel()->GetPosition();
		indexes[i].distance = square(vLight.x - source.x) + square (vLight.y - source.y) + square(vLight.z - source.z);
	}

	qsort( (void*) indexes, MAX_LIGHTS, sizeof(LightIndex), &CScene::compareLights);

	int count;
	
	if( lightCount < MAX_SHADER_LIGHTS )
	{
		count = lightCount;
	}
	else
	{
		count = MAX_SHADER_LIGHTS;
	}

	D3DXVECTOR4 positions[MAX_SHADER_LIGHTS];
	D3DXVECTOR4 colours[MAX_SHADER_LIGHTS];
	D3DXVECTOR4 bright[MAX_SHADER_LIGHTS];

	for(int i = 0; i < count; i++)
	{
		int c = indexes[i].index;
		positions[i] = D3DXVECTOR4 (mpLights[c]->GetModel()->GetPosition(), 0 );
		colours[i] = D3DXVECTOR4 ( mpLights[c]->GetColourV(), 0 );
		bright[i] = D3DXVECTOR4( mpLights[c]->GetBrightness(), 0, 0, 0 );   // vector 4 for packing, remaining variables would be useful for light type and angle. 
	}

	dxLightPosA->SetRawValue( positions, 0, 16 * count );
	dxLightColourA->SetRawValue( colours, 0, 16 * count );
	dxLightBrightnessA->SetRawValue( bright, 0, 16 * count );
}

bool CScene::BasicItems()
{
	
	//Parallax mapped cube
	mpObjects[miNumObjects] = new CRenderObject("Cube.x",	D3DXVECTOR3(0, 10, 0),			D3DXVECTOR3(1.0f, 1.0f, 1.0f), mTechniques[7], mTechniquesMirror[7], mpMaps[3], mpMaps[4], true, false, true);
	mpObjects[miNumObjects]->SetSpinY(0.5f);
	miNumObjects++;

	//Parallax mapped cube
	mpObjects[miNumObjects] = new CRenderObject("Cube.x",	D3DXVECTOR3(0, 10, 40),			D3DXVECTOR3(1.0f, 1.0f, 1.0f), mTechniques[7], mTechniquesMirror[7], mpMaps[6], mpMaps[7], true, false, true);
	mpObjects[miNumObjects]->SetSpinY(0.5f);
	miNumObjects++;
	
	//Jelly cube 
	
	mpObjects[miNumObjects] = new CRenderObject("Cube.x",	D3DXVECTOR3(40, 10, 40),		D3DXVECTOR3(1.0f, 1.0f, 1.0f), mTechniques[8], mTechniquesMirror[8], mpMaps[6], NULL, true, true);
	mpObjects[miNumObjects]->SetWiggle( 2.0f );
	miNumObjects++;
	
	//Basic Cube
	mpObjects[miNumObjects] = new CRenderObject("Cube.x",	D3DXVECTOR3( 20, 10, 0 ),		D3DXVECTOR3(1.0f, 1.0f, 1.0f), mTechniques[1], mTechniquesMirror[1], mpMaps[0], NULL, false, false);
	miNumObjects++;
	
	//Specular mapped cube
	
	mpObjects[miNumObjects] = new CRenderObject("Cube.x",	D3DXVECTOR3(-20, 10, 0),		D3DXVECTOR3(1.0f, 1.0f, 1.0f), mTechniques[6], mTechniquesMirror[6], mpMaps[0], NULL, true, false);
	miNumObjects++;
	
	//Shiny Teapot
	mpObjects[miNumObjects] = new CRenderObject("Teapot.x",	D3DXVECTOR3( 15, 5, 15 ),		D3DXVECTOR3(1.0f, 1.0f, 1.0f), mTechniques[5], mTechniquesMirror[5], mpMaps[2], NULL, true, false);
	mpObjects[miNumObjects]->SetSpinX(5.0f);
	mpObjects[miNumObjects]->SetSpinZ(2.5f);
	miNumObjects++;

	//Additive Blending Sphere
	
	mpObjects[miNumObjects] = new CRenderObject("Sphere.x",	D3DXVECTOR3(50, 30, 10),		D3DXVECTOR3(1.0f, 1.0f, 1.0f), mTechniques[2], mTechniquesMirror[2], mpMaps[0], NULL, false, true);
	mpObjects[miNumObjects]->SetColourCycle();
	miNumObjects++;
	
	//4 Lit test cube
	mpObjects[miNumObjects] = new CRenderObject("Cube.x",	D3DXVECTOR3( 180, 5, 152 ),		D3DXVECTOR3(1.0f, 1.0f, 1.0f), mTechniques[4], mTechniquesMirror[4], mpMaps[0], NULL, true, false);
	miNumObjects++;

	//Disco Cube
	mpObjects[miNumObjects] = new CRenderObject("Cube.x",	D3DXVECTOR3( 200, 5, 200 ),		D3DXVECTOR3(1.0f, 1.0f, 1.0f), mTechniques[6], mTechniquesMirror[6], mpMaps[0], NULL, true, false);
	miNumObjects++;

	//Cell Shaded Troll
	mpObjects[miNumObjects] = new CRenderObject("Troll.x",	D3DXVECTOR3( 0, 0, -10 ),		D3DXVECTOR3( 0.0f, 0.0f, 0.0f), mTechniques[9], mTechniquesMirror[9], mpMaps[5], NULL, true, false);
	mpObjects[miNumObjects]->GetModel()->SetScale( 5.0f );
	miNumObjects++;

	//Wrongly Cell Shaded Troll
	mpObjects[miNumObjects] = new CRenderObject("Troll.x",	D3DXVECTOR3( 0, 0, -30 ),		D3DXVECTOR3( 0.0f, 0.0f, 0.0f), mTechniques[10], mTechniquesMirror[10], mpMaps[5], NULL, true, false);
	mpObjects[miNumObjects]->GetModel()->SetScale( 5.0f );
	miNumObjects++;

	//Halo Troll
	
	mpObjects[miNumObjects] = new CRenderObject("Troll.x",	D3DXVECTOR3( 20, 5, -10 ),		D3DXVECTOR3( 0.5f, 0.5f, 0.0f), mTechniques[11], mTechniquesMirror[11], mpMaps[3], mpMaps[4], true, true, true);
	mpObjects[miNumObjects]->GetModel()->SetScale( 5.0f );
	miNumObjects++;


	//Space Halo Troll
	mpObjects[miNumObjects] = new CRenderObject("Troll.x",	D3DXVECTOR3( 50, 5, -10 ),		D3DXVECTOR3( 0.5f, 0.5f, 0.0f), mTechniques[12], mTechniquesMirror[12], mpMaps[6], mpMaps[7], true, true, true);
	mpObjects[miNumObjects]->GetModel()->SetScale( 5.0f );
	miNumObjects++;

	//Space Halo Troll
	mpObjects[miNumObjects] = new CRenderObject("Troll.x",	D3DXVECTOR3( 80, 5, -10 ),		D3DXVECTOR3( 0.5f, 0.5f, 0.0f), mTechniques[13], mTechniquesMirror[13], mpMaps[6], mpMaps[7], true, true, true);
	mpObjects[miNumObjects]->GetModel()->SetScale( 5.0f );
	miNumObjects++;
	

	//Floor
	mpObjects[miNumObjects] = new CRenderObject("Floor.x",	D3DXVECTOR3(0,0,0),				D3DXVECTOR3(1.0f, 1.0f, 1.0f), mTechniques[6], mTechniquesMirror[6], mpMaps[1], NULL, true, false);
	miNumObjects++;


	CModel* temp;
	
	//Light 1 - Orbit Light
	temp = new CModel;
	mpLights[miNumLights] = new CLight();
	if (!temp->	Load( "Sphere.x", mTechniques[0] )) return false;
	mpLights[miNumLights]->SetModel(temp);
	mpLights[miNumLights]->SetColour(1.0f, 0.0f, 0.7f);
	mpLights[miNumLights]->GetModel()->SetPosition( D3DXVECTOR3(30, 10, 0) );
	mpLights[miNumLights]->GetModel()->SetScale( 0.1f );
	mpLights[miNumLights]->SetBrightness( 10.0f );
	mpLights[miNumLights]->SetOrbit(  D3DXVECTOR3(0, 20, 0), 25.0f, 0.1f );
	mpLights[miNumLights]->SetColourCycle( 17.0f );

	miNumLights++;
	
	//Light 2 - Pulse Light
	temp = new CModel;
	mpLights[miNumLights] = new CLight();
	if (!temp->	Load( "Sphere.x", mTechniques[0] )) return false;
	mpLights[miNumLights]->SetModel(temp);
	mpLights[miNumLights]->SetColour(1.0f, 0.8f, 0.2f);
	mpLights[miNumLights]->GetModel()->SetPosition( D3DXVECTOR3(-20, 30, 50) );
	mpLights[miNumLights]->GetModel()->SetScale( 0.2f );
	mpLights[miNumLights]->GetModel()->UpdateMatrix();
	mpLights[miNumLights]->SetBrightness( 5.0f );
	mpLights[miNumLights]->SetFade( 2.0f );

	miNumLights++;

	//Light 3 - Disco Light
	
	temp = new CModel;
	mpLights[miNumLights] = new CLight();
	if (!temp->	Load( "Sphere.x", mTechniques[0] )) return false;
	mpLights[miNumLights]->SetModel(temp);
	mpLights[miNumLights]->SetColour(1.0f, 0.8f, 0.2f);
	mpLights[miNumLights]->GetModel()->SetPosition( D3DXVECTOR3(200, 25, 200) );
	mpLights[miNumLights]->GetModel()->SetScale( 0.2f );
	mpLights[miNumLights]->GetModel()->UpdateMatrix();
	mpLights[miNumLights]->SetBrightness( 5.0f );
	mpLights[miNumLights]->SetDisco();

	miNumLights++;
	
	//Light 4 - Cycle, Orbit and Pulse Light - Slightly buggy.
	/*
	temp = new CModel;
	mpLights[miNumLights] = new CLight();
	if (!temp->	Load( "Sphere.x", PlainColourTechnique )) return false;
	mpLights[miNumLights]->SetModel(temp);
	mpLights[miNumLights]->SetColour(1.0f, 0.8f, 0.2f);
	mpLights[miNumLights]->GetModel()->SetPosition( D3DXVECTOR3(200, 25, 200) );
	mpLights[miNumLights]->GetModel()->SetScale( 0.2f );
	mpLights[miNumLights]->GetModel()->UpdateMatrix();
	mpLights[miNumLights]->SetBrightness( 2.0f );
	mpLights[miNumLights]->SetOrbit( mpObjects[5]->GetModel(), 25.0f, 0.5f );
	mpLights[miNumLights]->SetColourCycle( 15.0f );
	mpLights[miNumLights]->SetPulse ( 2.0f );

	miNumLights++;
	*/
	return true;
}

bool CScene::TestItems()
{
	for( int i = 0; i < 30; i++)
	{
		mpObjects[miNumObjects] = new CRenderObject("Teapot.x",D3DXVECTOR3(250, 10, i*30.0f), D3DXVECTOR3(1.0f, i/10.0f, i/10.0f), mTechniques[0], mTechniquesMirror[0], NULL, NULL, NULL, NULL);
		miNumObjects++;
	}

	CModel* temp;
	for(int i = 0; i < 10; i++ )
	{
		temp = new CModel;
		mpLights[miNumLights] = new CLight();
		if (!temp->	Load( "Sphere.x", mTechniques[0] )) return false;
		mpLights[miNumLights]->SetModel(temp);
		mpLights[miNumLights]->SetColour(i/10.0f, i/10.0f, 1.0f);
		mpLights[miNumLights]->GetModel()->SetPosition( D3DXVECTOR3(200, 10, i*30.0f) );
		mpLights[miNumLights]->GetModel()->SetScale( i/10.0f );
		mpLights[miNumLights]->GetModel()->UpdateMatrix();
		miNumLights++;
	}
	return true;
}

bool CScene::SceneItems()
{
	float xOff = -250.0f;
	float yOff =  5.0f;
	float zOff = 50.0f;

	CModel* temp;

	temp = new CModel;
	mpLights[miNumLights] = new CLight();
	if (!temp->	Load( "Sphere.x", mTechniques[0] )) return false;
	mpLights[miNumLights]->SetModel(temp);
	mpLights[miNumLights]->SetColour(1.0f, 1.0f, 1.0f);
	mpLights[miNumLights]->GetModel()->SetPosition( D3DXVECTOR3(xOff, yOff+50.0f, zOff) );
	mpLights[miNumLights]->GetModel()->SetScale( 0.2f );
	mpLights[miNumLights]->SetBrightness( 10.0f );
	mpLights[miNumLights]->GetModel()->UpdateMatrix();
	miNumLights++;

	temp = new CModel;
	mpLights[miNumLights] = new CLight();
	if (!temp->	Load( "Sphere.x", mTechniques[0] )) return false;
	mpLights[miNumLights]->SetModel(temp);
	mpLights[miNumLights]->SetColour(1.0f, 1.0f, 1.0f);
	mpLights[miNumLights]->GetModel()->SetPosition( D3DXVECTOR3(xOff, yOff+80.0f, zOff+30.0f) );
	mpLights[miNumLights]->GetModel()->SetScale( 0.2f );
	mpLights[miNumLights]->SetBrightness( 40.0f );
	mpLights[miNumLights]->GetModel()->UpdateMatrix();
	miNumLights++;

	temp = new CModel;
	mpLights[miNumLights] = new CLight();
	if (!temp->	Load( "Sphere.x", mTechniques[0] )) return false;
	mpLights[miNumLights]->SetModel(temp);
	mpLights[miNumLights]->SetColour(1.0f, 1.0f, 1.0f);
	mpLights[miNumLights]->GetModel()->SetPosition( D3DXVECTOR3(xOff, yOff+80.0f, zOff-30.0f) );
	mpLights[miNumLights]->GetModel()->SetScale( 0.2f );
	mpLights[miNumLights]->SetBrightness( 40.0f );
	mpLights[miNumLights]->GetModel()->UpdateMatrix();
	miNumLights++;

	mpObjects[miNumObjects] = new CRenderObject("Troll.x", D3DXVECTOR3(xOff, yOff, 25.0f+zOff), D3DXVECTOR3( 0.8f, 0.8f, 0.0f ), mTechniques[TripleOutline], mTechniquesMirror[TripleOutline], mpMaps[9], NULL, true, true, false ); 
	mpObjects[miNumObjects]->GetModel()->SetScale(10.0f);
	miNumObjects++;

	mpObjects[miNumObjects] = new CRenderObject("Troll.x", D3DXVECTOR3(xOff, yOff, -25.0f+zOff), D3DXVECTOR3( 0.8f, 0.8f, 0.0f ), mTechniques[TextureMorph], mTechniquesMirror[TextureMorph], mpMaps[10], mpMaps[11], true, false, false ); 
	mpObjects[miNumObjects]->GetModel()->SetScale(10.0f);
	mpObjects[miNumObjects]->GetModel()->SetRotation(D3DXVECTOR3( 0.0f, (float)D3DX_PI, 0.0f ) );
	mpObjects[miNumObjects]->SetWiggle( 2.0f );
	miNumObjects++;

	mpObjects[miNumObjects] = new CRenderObject("Cube.x", D3DXVECTOR3( xOff, yOff+5.0f, zOff), D3DXVECTOR3( 1.0f, 1.0f, 1.0f ), mTechniques[ParallaxMapped], mTechniquesMirror[ParallaxMapped], mpMaps[6], mpMaps[7], true, false, true);
	mpObjects[miNumObjects]->GetModel()->SetScale(1.5f);
	miNumObjects++;

	mpObjects[miNumObjects] = new CRenderObject("Teapot.x", D3DXVECTOR3( xOff, yOff+12.5f, zOff), D3DXVECTOR3( 1.0f, 1.0f, 1.0f ), mTechniques[ParallaxMapped], mTechniquesMirror[ParallaxMapped], mpMaps[13], mpMaps[14], true, false, true);
	miNumObjects++;

	
	mpObjects[miNumObjects] = new CRenderObject("Cube.x", D3DXVECTOR3(xOff, yOff+15.0f, zOff), D3DXVECTOR3( 0.8f, 0.8f, 0.0f ), mTechniques[LitGlass], mTechniquesMirror[LitGlass], mpMaps[8], NULL, true, true, false ); 
	mpObjects[miNumObjects]->GetModel()->SetScale(10.0f);
	miNumObjects++;
	

	return true;
}