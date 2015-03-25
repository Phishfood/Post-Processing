//Global Variables
//Sampler States
//Structures
//Blender States
//Rasteriser States
//Depth States
//Vertex Shader
//Post Processing Effects
	//Copy
	//Tint
	//Shock
	//Box Blur 3
	//Box Blur 5
	//Box Blur V
	//Double Vision
	//Edge
	//Contrast
	//Jam
	//Invert
	//Solarise Above
	//Solarise Below
	//2 Pass Gaussian Blur



/////////////////////////////////////////////////////////////////
//Global Variables
/////////////////////////////////////////////////////////////////
Texture2D InitialTexture;
Texture2D BloodTexture;
Texture2D DepthMap;

float PixelX, PixelY;

int BlurRange;

float3 TintColour;

float Shock;

float DVRange;

float Contrast;

float Blood;

float SolariseThreshold;

float4 GaussTemp[16];

float4 GaussianFilter[64];

/////////////////////////////////////////////////////////////////
//Sampler States
/////////////////////////////////////////////////////////////////

SamplerState PointSample
{
    Filter = MIN_MAG_MIP_POINT;
    AddressU = Clamp;
    AddressV = Clamp;
};

// Use the usual filtering for the special purpose post-processing textures (e.g. the noise map)
SamplerState TrilinearWrap
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Wrap;
    AddressV = Wrap;
};

/////////////////////////////////////////////////////////////////
// Structures
/////////////////////////////////////////////////////////////////

struct VS_INPUT
{
    uint vertexId : SV_VertexID;
};

// Simple vertex shader output / pixel shader input for the main post processing step
struct PS_INPUT
{
    float4 ProjPos : SV_POSITION;
	float2 UV      : TEXCOORD0;
};

/////////////////////////////////////////////////////////////////
// Blender States
/////////////////////////////////////////////////////////////////

BlendState NoBlend
{
	BlendEnable[0] = FALSE;
};

BlendState AdditiveBlend
{
	BlendEnable[0] = TRUE;
	SrcBlend = ONE;
	DestBlend = ONE;
	BlendOp = ADD;
};

BlendState MultiplicativeBlend
{
	BlendEnable[0] = TRUE;
	SrcBlend = DEST_COLOR;
	BlendOp = ADD;
};

BlendState HaloBlend
{
	BlendEnable[0] = TRUE;
	SrcBlend = DEST_COLOR;
	DestBlend = ONE;
	BlendOp = ADD;
};

/////////////////////////////////////////////////////////////////
// Rasterizer States
/////////////////////////////////////////////////////////////////

RasterizerState CullNone  // Cull none of the polygons, i.e. show both sides
{
	CullMode = None;
};

///////////////////////////////////////////////////////////////////
// Depth States
///////////////////////////////////////////////////////////////////

DepthStencilState DepthWritesOff // Don't write to the depth buffer - polygons rendered will not obscure other polygons
{
	DepthWriteMask = ZERO;
	DepthFunc = LESS;
	StencilEnable = FALSE;
};

DepthStencilState DepthWritesOn  // Write to the depth buffer - normal behaviour 
{
	DepthWriteMask = ALL;
	DepthFunc = LESS;
	StencilEnable = FALSE;
};

DepthStencilState DisableDepth   // Disable depth buffer entirely
{
	DepthFunc      = ALWAYS;
	DepthWriteMask = ZERO;
};

/////////////////////////////////////////////////////////////////
// Vertex Shader
/////////////////////////////////////////////////////////////////
PS_INPUT FullScreenQuad(VS_INPUT vIn)
{
    PS_INPUT vOut;
	
	float4 QuadPos[4] = { float4(-1.0, 1.0, 0.0, 1.0),
	                      float4(-1.0,-1.0, 0.0, 1.0),
						  float4( 1.0, 1.0, 0.0, 1.0),
						  float4( 1.0,-1.0, 0.0, 1.0) };
	float2 QuadUV[4] =  { float2(0.0, 0.0),
	                      float2(0.0, 1.0),
						  float2(1.0, 0.0),
						  float2(1.0, 1.0) };

	vOut.ProjPos = QuadPos[vIn.vertexId];
	vOut.UV = QuadUV[vIn.vertexId];
	
    return vOut;
}


/////////////////////////////////////////////////////////////////
//Post Processing Effects
//////////////////////////////////////////////////////////////////

///////////////
// Copy
//////////////

float4 PPCopyShader( PS_INPUT ppIn ) : SV_Target
{
	float3 ppColour = InitialTexture.Sample(PointSample, ppIn.UV);
	float3 dmColour = DepthMap.Sample(PointSample, ppIn.UV);
	return float4( ppColour, 1.0f );
}

technique10 PPCopy
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_4_0, FullScreenQuad() ) );
        SetGeometryShader( NULL );                                   
        SetPixelShader( CompileShader( ps_4_0, PPCopyShader() ) );

		SetBlendState( NoBlend, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
		SetRasterizerState( CullNone ); 
		SetDepthStencilState( DisableDepth, 0 );
     }
}

///////////////
// Tint
//////////////

float4 PPTintShader( PS_INPUT ppIn ) : SV_Target
{
	float3 ppCol = InitialTexture.Sample( PointSample, ppIn.UV );
	ppCol = ppCol * TintColour;
	return float4(ppCol, 1.0f);
}

technique10 PPTint
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_4_0, FullScreenQuad() ) );
        SetGeometryShader( NULL );                                   
        SetPixelShader( CompileShader( ps_4_0, PPTintShader() ) );

		SetBlendState( NoBlend, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
		SetRasterizerState( CullNone ); 
		SetDepthStencilState( DisableDepth, 0 );
     }
}

/////////////
// Shock
////////////

float4 PPShockShader( PS_INPUT ppIn ) : SV_Target
{
	float3 ppColour = InitialTexture.Sample( PointSample, ppIn.UV+float2(Shock, 0) );
	return float4( ppColour, 1.0f );
}

technique10 PPShock
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_4_0, FullScreenQuad() ) );
        SetGeometryShader( NULL );                                   
        SetPixelShader( CompileShader( ps_4_0, PPShockShader() ) );

		SetBlendState( NoBlend, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
		SetRasterizerState( CullNone ); 
		SetDepthStencilState( DisableDepth, 0 );
     }
}

/////////////
// Box Blur 3
/////////////

float4 PPBoxBlur3Shader( PS_INPUT ppIn ) : SV_Target
{
	float3 ppCol = float3(0.0f, 0.0f, 0.0f);
	
	ppCol += InitialTexture.Sample( PointSample, ppIn.UV + float2(-PixelX, -PixelY) );
	ppCol += InitialTexture.Sample( PointSample, ppIn.UV + float2(0		 , -PixelY) );
	ppCol += InitialTexture.Sample( PointSample, ppIn.UV + float2(+PixelX, -PixelY) );

	ppCol += InitialTexture.Sample( PointSample, ppIn.UV + float2(-PixelX, 0) );
	ppCol += InitialTexture.Sample( PointSample, ppIn.UV + float2(0		 , 0) );
	ppCol += InitialTexture.Sample( PointSample, ppIn.UV + float2(+PixelX, 0) );

	ppCol += InitialTexture.Sample( PointSample, ppIn.UV + float2(-PixelX, +PixelY) );
	ppCol += InitialTexture.Sample( PointSample, ppIn.UV + float2(0		 , +PixelY) );
	ppCol += InitialTexture.Sample( PointSample, ppIn.UV + float2(+PixelX, +PixelY) );

	ppCol /= 9;

	return float4(ppCol, 1.0f);
}

technique10 PPBoxBlur3
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_4_0, FullScreenQuad() ) );
        SetGeometryShader( NULL );                                   
        SetPixelShader( CompileShader( ps_4_0, PPBoxBlur3Shader() ) );

		SetBlendState( NoBlend, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
		SetRasterizerState( CullNone ); 
		SetDepthStencilState( DisableDepth, 0 );
     }
}

/////////////
// Box Blur 5
/////////////

float4 PPBoxBlur5Shader( PS_INPUT ppIn ) : SV_Target
{
	float3 ppCol = float3(0.0f, 0.0f, 0.0f);

	ppCol += InitialTexture.Sample( PointSample, ppIn.UV + float2(-2*PixelX	, -2*PixelY) );
	ppCol += InitialTexture.Sample( PointSample, ppIn.UV + float2(-PixelX	, -2*PixelY) );
	ppCol += InitialTexture.Sample( PointSample, ppIn.UV + float2(0			, -2*PixelY) );
	ppCol += InitialTexture.Sample( PointSample, ppIn.UV + float2(+PixelX	, -2*PixelY) );
	ppCol += InitialTexture.Sample( PointSample, ppIn.UV + float2(2*PixelX	, -2*PixelY) );

	ppCol += InitialTexture.Sample( PointSample, ppIn.UV + float2(-2*PixelX	, -PixelY) );
	ppCol += InitialTexture.Sample( PointSample, ppIn.UV + float2(-PixelX	, -PixelY) );
	ppCol += InitialTexture.Sample( PointSample, ppIn.UV + float2(0			, -PixelY) );
	ppCol += InitialTexture.Sample( PointSample, ppIn.UV + float2(+PixelX	, -PixelY) );
	ppCol += InitialTexture.Sample( PointSample, ppIn.UV + float2(2*PixelX	, -PixelY) );

	ppCol += InitialTexture.Sample( PointSample, ppIn.UV + float2(-2*PixelX	, 0) );
	ppCol += InitialTexture.Sample( PointSample, ppIn.UV + float2(-PixelX	, 0) );
	ppCol += InitialTexture.Sample( PointSample, ppIn.UV + float2(0			, 0) );
	ppCol += InitialTexture.Sample( PointSample, ppIn.UV + float2(+PixelX	, 0) );
	ppCol += InitialTexture.Sample( PointSample, ppIn.UV + float2(2*PixelX	, 0) );

	ppCol += InitialTexture.Sample( PointSample, ppIn.UV + float2(-2*PixelX	, +PixelY) );
	ppCol += InitialTexture.Sample( PointSample, ppIn.UV + float2(-PixelX	, +PixelY) );
	ppCol += InitialTexture.Sample( PointSample, ppIn.UV + float2(0			, +PixelY) );
	ppCol += InitialTexture.Sample( PointSample, ppIn.UV + float2(+PixelX	, +PixelY) );
	ppCol += InitialTexture.Sample( PointSample, ppIn.UV + float2(2*PixelX	, +PixelY) );

	ppCol += InitialTexture.Sample( PointSample, ppIn.UV + float2(-2*PixelX	, +2*PixelY) );
	ppCol += InitialTexture.Sample( PointSample, ppIn.UV + float2(-PixelX	, +2*PixelY) );
	ppCol += InitialTexture.Sample( PointSample, ppIn.UV + float2(0			, +2*PixelY) );
	ppCol += InitialTexture.Sample( PointSample, ppIn.UV + float2(+PixelX	, +2*PixelY) );
	ppCol += InitialTexture.Sample( PointSample, ppIn.UV + float2(2*PixelX	, +2*PixelY) );

	ppCol /= 25;

	return float4(ppCol, 1.0f);
}

technique10 PPBoxBlur5
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_4_0, FullScreenQuad() ) );
        SetGeometryShader( NULL );                                   
        SetPixelShader( CompileShader( ps_4_0, PPBoxBlur5Shader() ) );

		SetBlendState( NoBlend, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
		SetRasterizerState( CullNone ); 
		SetDepthStencilState( DisableDepth, 0 );
     }
}

/////////////
// Box Blur V
/////////////


float4 PPBoxBlurVShader( PS_INPUT ppIn ) : SV_Target
{
	float3 ppCol = float3(0.0f, 0.0f, 0.0f);
	
	int halfBlurRange = BlurRange / 2;

	for(int i = 0; i < BlurRange; i++)
	{
		for(int j = 0; j < BlurRange; j++)
		{
			float xOffset = (i - halfBlurRange) * PixelX;
			float yOffset = (j - halfBlurRange) * PixelY;
			ppCol += InitialTexture.Sample( PointSample, ppIn.UV + float2(xOffset, yOffset ) );
		}
	}

	ppCol /= BlurRange*BlurRange;

	return float4(ppCol, 1.0f);
}

technique10 PPBoxBlurV
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_4_0, FullScreenQuad() ) );
        SetGeometryShader( NULL );                                   
        SetPixelShader( CompileShader( ps_4_0, PPBoxBlurVShader() ) );

		SetBlendState( NoBlend, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
		SetRasterizerState( CullNone ); 
		SetDepthStencilState( DisableDepth, 0 );
     }
}

///////////
// Double Vision
///////////

float4 PPDoubleVisionShader( PS_INPUT ppIn ) : SV_Target
{
	float3 ppCol = float3(0.0f, 0.0f, 0.0f);
	
	ppCol += InitialTexture.Sample( PointSample, ppIn.UV ) * 2;
	ppCol += InitialTexture.Sample( PointSample, ppIn.UV + float2(-DVRange,0) );	
	ppCol += InitialTexture.Sample( PointSample, ppIn.UV + float2( DVRange,0) );

	ppCol /= 4;

	return float4(ppCol, 1.0f);		
}

technique10 PPDoubleVision
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_4_0, FullScreenQuad() ) );
        SetGeometryShader( NULL );                                   
        SetPixelShader( CompileShader( ps_4_0, PPDoubleVisionShader() ) );

		SetBlendState( NoBlend, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
		SetRasterizerState( CullNone ); 
		SetDepthStencilState( DisableDepth, 0 );
     }
}

///////////
// Edge
//////////

float4 PPEdgeShader( PS_INPUT ppIn ) : SV_Target
{
	float3 ppCol = float3(0.0f, 0.0f, 0.0f);
	
	float e11 = InitialTexture.Sample( PointSample, ppIn.UV + float2(-PixelX, -PixelY) );
	float e12 = InitialTexture.Sample( PointSample, ppIn.UV + float2(0		 , -PixelY) );
	float e13 = InitialTexture.Sample( PointSample, ppIn.UV + float2(+PixelX, -PixelY) );

	float e21 = InitialTexture.Sample( PointSample, ppIn.UV + float2(-PixelX, 0) );
	float e22 = InitialTexture.Sample( PointSample, ppIn.UV + float2(0		 , 0) );
	float e23 = InitialTexture.Sample( PointSample, ppIn.UV + float2(+PixelX, 0) );

	float e31 = InitialTexture.Sample( PointSample, ppIn.UV + float2(-PixelX, +PixelY) );
	float e32 = InitialTexture.Sample( PointSample, ppIn.UV + float2(0		 , +PixelY) );
	float e33 = InitialTexture.Sample( PointSample, ppIn.UV + float2(+PixelX, +PixelY) );

	float t1 = e13 + e33 + (2 * e23) - e11 - (2 * e21) - e31;
	float t2 = e31 + (2 * e32) + e33 - e11 - (2 * e12) - e13;
	
	if( ( (t1 * t1) + (t2 * t2) ) > 0.05)
	{
		return float4(0.0f, 0.0f, 0.0f, 1.0f);
	}
	else
	{
		return float4(1.0f, 1.0f, 1.0f, 1.0f);
	}
}

technique10 PPEdge
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_4_0, FullScreenQuad() ) );
        SetGeometryShader( NULL );                                   
        SetPixelShader( CompileShader( ps_4_0, PPEdgeShader() ) );

		SetBlendState( NoBlend, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
		SetRasterizerState( CullNone ); 
		SetDepthStencilState( DisableDepth, 0 );
     }
}

///////////////
// Contrast
///////////////

float4 PPContrastShader(PS_INPUT ppIn) : SV_Target
{
	float3 ppCol;
	ppCol = InitialTexture.Sample(PointSample, ppIn.UV);

	ppCol.r = saturate(Contrast * (ppCol.r - 0.5) + 0.5);
	ppCol.g = saturate(Contrast * (ppCol.g - 0.5) + 0.5);
	ppCol.b = saturate(Contrast * (ppCol.b - 0.5) + 0.5);

	return float4(ppCol, 1.0f);
}

technique10 PPContrast
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_4_0, FullScreenQuad()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_4_0, PPContrastShader()));

		SetBlendState(NoBlend, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
		SetRasterizerState(CullNone);
		SetDepthStencilState(DisableDepth, 0);
	}
}

///////////////
// Screen Jam
///////////////

float4 PPJamShader(PS_INPUT ppIn) : SV_Target
{
	float3 iCol, bCol, ppCol;
	iCol = InitialTexture.Sample(PointSample, ppIn.UV);
	bCol = BloodTexture.Sample(PointSample, ppIn.UV);
	ppCol = (Blood * bCol) + ((1 - Blood) * iCol);
	
	return float4(ppCol, 1.0f);
}

technique10 PPBlood
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_4_0, FullScreenQuad()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_4_0, PPJamShader()));

		SetBlendState(NoBlend, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
		SetRasterizerState(CullNone);
		SetDepthStencilState(DisableDepth, 0);
	}
}

/////////////
// Invert
/////////////

float4 PPInvertShader(PS_INPUT ppIn) : SV_Target
{
	float3 ppCol;
	ppCol = InitialTexture.Sample(PointSample, ppIn.UV);
	ppCol.r = 1.0f - ppCol.r;
	ppCol.g = 1.0f - ppCol.g;
	ppCol.b = 1.0f - ppCol.b;

	return float4(ppCol, 1.0f);
}

technique10 PPInvert
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_4_0, FullScreenQuad()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_4_0, PPInvertShader()));

		SetBlendState(NoBlend, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
		SetRasterizerState(CullNone);
		SetDepthStencilState(DisableDepth, 0);
	}
}

/////////////
// Solarise A
/////////////

float4 PPSolariseAShader(PS_INPUT ppIn) : SV_Target
{
	float3 ppCol;
	ppCol = InitialTexture.Sample(PointSample, ppIn.UV);
	if (ppCol.r > SolariseThreshold)
	{
		ppCol.r = 1.0f - ppCol.r;
	}

	if (ppCol.g > SolariseThreshold)
	{
		ppCol.g = 1.0f - ppCol.g;
	}

	if (ppCol.b > SolariseThreshold)
	{
		ppCol.b = 1.0f - ppCol.b;
	}

	return float4(ppCol, 1.0f);
}

technique10 PPSolariseA
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_4_0, FullScreenQuad()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_4_0, PPSolariseAShader()));

		SetBlendState(NoBlend, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
		SetRasterizerState(CullNone);
		SetDepthStencilState(DisableDepth, 0);
	}
}

/////////////
// Solarise B
/////////////

float4 PPSolariseBShader(PS_INPUT ppIn) : SV_Target
{
	float3 ppCol;
	ppCol = InitialTexture.Sample(PointSample, ppIn.UV);
	if (ppCol.r < SolariseThreshold)
	{
		ppCol.r = 1.0f - ppCol.r;
	}

	if (ppCol.g < SolariseThreshold)
	{
		ppCol.g = 1.0f - ppCol.g;
	}

	if (ppCol.b < SolariseThreshold)
	{
		ppCol.b = 1.0f - ppCol.b;
	}

	return float4(ppCol, 1.0f);
}

technique10 PPSolariseB
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_4_0, FullScreenQuad()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_4_0, PPSolariseBShader()));

		SetBlendState(NoBlend, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
		SetRasterizerState(CullNone);
		SetDepthStencilState(DisableDepth, 0);
	}
}

///////////
// CellShade
//////////

float4 PPCellShader(PS_INPUT ppIn) : SV_Target
{
	float3 ppCol = float3(0.0f, 0.0f, 0.0f);

	float e11 = InitialTexture.Sample(PointSample, ppIn.UV + float2(-PixelX, -PixelY));
	float e12 = InitialTexture.Sample(PointSample, ppIn.UV + float2(0, -PixelY));
	float e13 = InitialTexture.Sample(PointSample, ppIn.UV + float2(+PixelX, -PixelY));

	float e21 = InitialTexture.Sample(PointSample, ppIn.UV + float2(-PixelX, 0));
	float e22 = InitialTexture.Sample(PointSample, ppIn.UV + float2(0, 0));
	float e23 = InitialTexture.Sample(PointSample, ppIn.UV + float2(+PixelX, 0));

	float e31 = InitialTexture.Sample(PointSample, ppIn.UV + float2(-PixelX, +PixelY));
	float e32 = InitialTexture.Sample(PointSample, ppIn.UV + float2(0, +PixelY));
	float e33 = InitialTexture.Sample(PointSample, ppIn.UV + float2(+PixelX, +PixelY));

	float t1 = e13 + e33 + (2 * e23) - e11 - (2 * e21) - e31;
	float t2 = e31 + (2 * e32) + e33 - e11 - (2 * e12) - e13;

	if (((t1 * t1) + (t2 * t2)) > 0.05)
	{
		return float4(0.0f, 0.0f, 0.0f, 1.0f);
	}
	else
	{
		return InitialTexture.Sample(PointSample, ppIn.UV + float2(0, 0));
	}
}

technique10 PPCell
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_4_0, FullScreenQuad()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_4_0, PPCellShader()));

		SetBlendState(NoBlend, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
		SetRasterizerState(CullNone);
		SetDepthStencilState(DisableDepth, 0);
	}
}

///////////////
// 2 Pass Gaussian
///////////////

float4 PPGaussianHorizontalShader( PS_INPUT ppIn ) : SV_Target
{
	
	float3 ppCol = float3(0.0f, 0.0f, 0.0f);
	int halfBlurRange = BlurRange / 2;

	float weightTotal = 0;

	for (int i = 0; i < BlurRange; i++)
	{
		float xOffset = (i - halfBlurRange) * PixelX;
		float weight = ((float[4])(GaussianFilter[i / 4]))[i % 4];
		weightTotal += weight;
		ppCol += InitialTexture.Sample(PointSample, ppIn.UV + float2(xOffset, 0.0f)) * weight;
	}

	ppCol /= weightTotal;
	return float4(ppCol, 1.0f);
}

float4 PPGaussianVerticalShader( PS_INPUT ppIn ) : SV_Target
{
	float3 ppCol = float3(0.0f, 0.0f, 0.0f);
	int halfBlurRange = BlurRange / 2;

	float weightTotal = 0;

	for (int i = 0; i < BlurRange; i++)
	{
		float yOffset = (i - halfBlurRange) * PixelY;
		float weight = ((float[4])(GaussianFilter[i / 4]))[i % 4];
		weightTotal += weight;
		ppCol += InitialTexture.Sample(PointSample, ppIn.UV + float2(0.0f, yOffset)) * weight;
	}

	ppCol /= weightTotal;
	ppCol.r += 0.5f;
	return float4(ppCol, 1.0f);

}

technique10 PPGaussian
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_4_0, FullScreenQuad()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_4_0, PPGaussianHorizontalShader()));

		SetBlendState(NoBlend, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
		SetRasterizerState(CullNone);
		SetDepthStencilState(DisableDepth, 0);
	}

	pass P1
	{
		SetVertexShader(CompileShader(vs_4_0, FullScreenQuad()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_4_0, PPGaussianVerticalShader()));

		SetBlendState(NoBlend, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
		SetRasterizerState(CullNone);
		SetDepthStencilState(DisableDepth, 0);
	}
}

///////////////
// Copy Depth Buffer
//////////////

float4 PPDMCopyShader(PS_INPUT ppIn) : SV_Target
{
	float3 ppColour = DepthMap.Sample(PointSample, ppIn.UV);
	return float4(ppColour.r,0.5f,0.5f, 1.0f);
}

technique10 PPDMCopy
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_4_0, FullScreenQuad()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_4_0, PPDMCopyShader()));

		SetBlendState(NoBlend, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
		SetRasterizerState(CullNone);
		SetDepthStencilState(DisableDepth, 0);
	}
}