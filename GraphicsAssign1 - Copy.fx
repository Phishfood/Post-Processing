//--------------------------------------------------------------------------------------
// Global Variables
//--------------------------------------------------------------------------------------
// All these variables are created & manipulated in the C++ code and passed into the shader here

// The matrices (4x4 matrix of floats) for transforming from 3D model to 2D projection (used in vertex shader)
float4x4 WorldMatrix;
float4x4 ViewMatrix;
float4x4 ProjMatrix;

// A single colour for an entire model - used for light models and the intial basic shader
float3 ModelColour;

// Diffuse texture map (the main texture colour) - may contain specular map in alpha channel
Texture2D DiffuseMap;

// Normal Map
Texture2D NormalMap;

static const uint MAX_LIGHTS = 4;

float4 lightPos[MAX_LIGHTS];
float4 lightColour[MAX_LIGHTS];
float lightBright[MAX_LIGHTS];

float3 cameraPos;

float3 ambientColour;


//--------------------------------------------------------------------------------------
// Sampler States
//--------------------------------------------------------------------------------------

//Bilinear filtering
SamplerState Bilinear
{
    Filter = MIN_MAG_LINEAR_MIP_POINT;
    AddressU = Wrap;
    AddressV = Wrap;
};

//Trilinear filtering
SamplerState Trilinear
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Wrap;
    AddressV = Wrap;
};

//--------------------------------------------------------------------------------------
// Blender States
//--------------------------------------------------------------------------------------

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

BlendState MultiplicativeBlending
{
	BlendEnable[0] = TRUE;
	SrcBlend = DEST_COLOR;
	BlendOp = ADD;
};

//--------------------------------------------------------------------------------------
// Rasterizer States
//--------------------------------------------------------------------------------------

RasterizerState CullNone  // Cull none of the polygons, i.e. show both sides
{
	CullMode = None;
};
RasterizerState CullBack  // Cull back side of polygon - normal behaviour, only show front of polygons
{
	CullMode = Back;
};


//--------------------------------------------------------------------------------------
// Unlit Textures
//--------------------------------------------------------------------------------------

struct VS_BASIC_INPUT
{
    float3 Pos    : POSITION;
    float3 Normal : NORMAL;
	float2 UV     : TEXCOORD0;
};

struct VS_BASIC_OUTPUT
{
    float4 ProjPos : SV_POSITION;
    float2 UV      : TEXCOORD0;
};

VS_BASIC_OUTPUT BasicTransform( VS_BASIC_INPUT vIn )
{
	VS_BASIC_OUTPUT vOut;
	
	// Use world matrix passed from C++ to transform the input model vertex position into world space
	float4 modelPos = float4(vIn.Pos, 1.0f); // Promote to 1x4 so we can multiply by 4x4 matrix, put 1.0 in 4th element for a point (0.0 for a vector)
	float4 worldPos = mul( modelPos, WorldMatrix );
	float4 viewPos  = mul( worldPos, ViewMatrix );
	vOut.ProjPos    = mul( viewPos,  ProjMatrix );
	
	// Pass texture coordinates (UVs) on to the pixel shader
	vOut.UV = vIn.UV;

	return vOut;
}

float4 OneColour( VS_BASIC_OUTPUT vOut ) : SV_Target
{
	return float4( ModelColour, 1.0 ); // Set alpha channel to 1.0 (opaque)
}

//Textured pixel Shader
float4 BasicTexture( VS_BASIC_OUTPUT vOut ) : SV_Target
{
	return DiffuseMap.Sample(Trilinear, vOut.UV);
}


//Changes the colour of the texture over time
float4 ColourTexture( VS_BASIC_OUTPUT vOut ) : SV_Target
{
	return ( DiffuseMap.Sample(Bilinear, vOut.UV) * float4( ModelColour, 1.0) ) / 2;
}

technique10 tPlainColour
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_4_0, BasicTransform() ) );
        SetGeometryShader( NULL );                                   
        SetPixelShader( CompileShader( ps_4_0, OneColour() ) );
		SetBlendState( NoBlend, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
		SetRasterizerState( CullBack ); 
	}
}

technique10 tPlainTexture
{
	pass P0
	{
        SetVertexShader( CompileShader( vs_4_0, BasicTransform() ) );
        SetGeometryShader( NULL );  
		SetPixelShader( CompileShader( ps_4_0, BasicTexture() ) );
		SetBlendState( NoBlend, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
		SetRasterizerState( CullBack ); 
	}
}

technique10 tColourChangeTexture
{
	pass P0
	{
		SetVertexShader( CompileShader( vs_4_0, BasicTransform() ) );
		SetGeometryShader( NULL );  
		SetPixelShader( CompileShader( ps_4_0, ColourTexture() ) );
		SetBlendState( AdditiveBlend, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
		SetRasterizerState( CullBack ); 
	}
}

//--------------------------------------------------------------------------------------
// Simply Lit Textures
//--------------------------------------------------------------------------------------

struct VS_LIT_OUTPUT
{
	float4 ProjPos		: SV_POSITION;
	float2 UV			: TEXCOORD0;
	float4 worldPos		: POSITION;
	float4 worldNormal	: NORMAL;
};

VS_LIT_OUTPUT BasicTransformLitTex( VS_BASIC_INPUT vIn )
{
	VS_LIT_OUTPUT vOut;
	
	// Use world matrix passed from C++ to transform the input model vertex position into world space
	float4 modelPos = float4(vIn.Pos, 1.0f); // Promote to 1x4 so we can multiply by 4x4 matrix, put 1.0 in 4th element for a point (0.0 for a vector)
	vOut.worldPos = mul( modelPos, WorldMatrix );
	float4 viewPos  = mul( vOut.worldPos, ViewMatrix );
	vOut.ProjPos    = mul( viewPos,  ProjMatrix );
	
	// Now the normals
	float4 modelNormal = float4(vIn.Normal, 0.0f); //promote to 4 so we can multiply
	vOut.worldNormal = mul( modelNormal, WorldMatrix );

	// Pass texture coordinates (UVs) on to the pixel shader
	vOut.UV = vIn.UV;

	return vOut;
}

//------------------
// 2 Lights
//------------------

//Lit and Textured pixel Shader - 2 lights
float4 LitTexture( VS_LIT_OUTPUT vOut ) : SV_Target
{
	float4 worldNormal = normalize( vOut.worldNormal );

	float3 cameraVector = normalize(cameraPos - vOut.worldPos.xyz);
	float3 lightVector = normalize(lightPos[0] - vOut.worldPos.xyz);
	float3 lightVector2 = normalize(lightPos[1] - vOut.worldPos.xyz);

	float3 light1Light = lightColour[0] * max( dot( worldNormal.xyz, lightVector), 0 );
	float3 light2Light = lightColour[1] * max( dot( worldNormal.xyz, lightVector2), 0 );
	float3 diffuseLight = ambientColour + light1Light + light2Light;

	float3 halfway1 = normalize( lightVector + cameraVector );
	float3 halfway2 = normalize( lightVector2 + cameraVector );

	light1Light = lightColour[0] * pow( max( dot( worldNormal.xyz, halfway1), 0 ), 256);
	light2Light = lightColour[1] * pow( max( dot( worldNormal.xyz, halfway2), 0 ), 256);
	float3 specularLight = light1Light + light2Light;

	float4 texColour = DiffuseMap.Sample(Bilinear, vOut.UV);
	return texColour * ( float4(diffuseLight, 1.0) + float4(specularLight, 1.0) );
}

technique10 tLitTexture
{
	pass P0
	{
		SetVertexShader( CompileShader( vs_4_0, BasicTransformLitTex() ) );
        SetGeometryShader( NULL );  
		SetPixelShader( CompileShader( ps_4_0, LitTexture() ) );
		SetBlendState( NoBlend, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
		SetRasterizerState( CullNone ); 
	}
}

//------------------
// 4 Lights
//------------------

//Lit and Textured pixel Shader - all 4 lights
float4 Lit4Texture( VS_LIT_OUTPUT vOut ) : SV_Target
{
	float4 worldNormal = normalize( vOut.worldNormal );

	float3 cameraVector = normalize(cameraPos - vOut.worldPos.xyz);

	float4 diffuseLight;
	float4 specularLight;

	for(uint i = 0; i < MAX_LIGHTS; i++)
	{
		float4 lightVector = normalize(lightPos[0] - vOut.worldPos);
		diffuseLight += lightColour[0] * max( dot( worldNormal.xyz, lightVector), 0 );
		float4 halfway1 = normalize( lightVector + cameraVector );
	}


	light1Light = lightColour[0] * pow( max( dot( worldNormal.xyz, halfway1), 0 ), 256);
	light2Light = lightColour[1] * pow( max( dot( worldNormal.xyz, halfway2), 0 ), 256);
	light3Light = lightColour[2] * pow( max( dot( worldNormal.xyz, halfway3), 0 ), 256);
	light4Light = lightColour[3] * pow( max( dot( worldNormal.xyz, halfway4), 0 ), 256);
	float3 specularLight = light1Light + light2Light + light3Light + light4Light;

	float4 texColour = DiffuseMap.Sample(Bilinear, vOut.UV);
	return texColour * ( float4(diffuseLight, 1.0) + float4(specularLight, 1.0) );
}

//Lit and Textured pixel Shader - all 4 lights with attenuation
float4 Lit4AttenTexture( VS_LIT_OUTPUT vOut ) : SV_Target
{
	float4 worldNormal = normalize( vOut.worldNormal );

	float3 cameraVector = normalize(cameraPos - vOut.worldPos.xyz);
	float3 lightVector = normalize(lightPos[0] - vOut.worldPos.xyz);
	float3 lightVector2 = normalize(lightPos[1] - vOut.worldPos.xyz);
	float3 lightVector3 = normalize(lightPos[2] - vOut.worldPos.xyz);
	float3 lightVector4 = normalize(lightPos[3] - vOut.worldPos.xyz);

	float3 light1Light = lightColour[0] * max( dot( worldNormal.xyz, lightVector), 0 );
	float light1Dist = length(lightPos[0] - vOut.worldPos.xyz) / lightBright[0];
	light1Light = light1Light / (  light1Dist );
	
	float3 light2Light = lightColour[1] * max( dot( worldNormal.xyz, lightVector2), 0 );
	float light2Dist = length(lightPos[0] - vOut.worldPos.xyz) / lightBright[1];
	light2Light = light2Light / (  light2Dist );
	
	float3 light3Light = lightColour[2] * max( dot( worldNormal.xyz, lightVector3), 0 );
	float light3Dist = length(lightPos[2] - vOut.worldPos.xyz) / lightBright[2];
	light3Light = light3Light / (  light3Dist );

	float3 light4Light = lightColour[3] * max( dot( worldNormal.xyz, lightVector4), 0 );
	float light4Dist = length(lightPos[0] - vOut.worldPos.xyz) / lightBright[3];
	light4Light = light4Light / (  light4Dist );
	
	float3 diffuseLight = ambientColour + light1Light + light2Light + light3Light + light4Light;

	//float3 halfway = normalize(LightDir + CameraDir);
	//float3 specularLight = LightColour * pow( max( dot(worldNormal.xyz, halfway), 0 ), SpecularPower );
		
	float3 halfway1 = normalize( lightVector + cameraVector );
	float3 halfway2 = normalize( lightVector2 + cameraVector );
	float3 halfway3 = normalize( lightVector3 + cameraVector );
	float3 halfway4 = normalize( lightVector4 + cameraVector );

	light1Light = lightColour[0] * pow( max( dot( worldNormal.xyz, halfway1), 0 ), 256);
	light1Dist = length(lightPos[0] - vOut.worldPos.xyz) / lightBright[0];
	light1Light = light1Light / (  light1Dist );

	light2Light = lightColour[1] * pow( max( dot( worldNormal.xyz, halfway2), 0 ), 256);
	light2Dist = length(lightPos[1] - vOut.worldPos.xyz) / lightBright[1];
	light2Light = light2Light / (  light2Dist );

	light3Light = lightColour[2] * pow( max( dot( worldNormal.xyz, halfway3), 0 ), 256);
	light3Dist = length(lightPos[2] - vOut.worldPos.xyz) / lightBright[2];
	light3Light = light3Light / (  light3Dist );

	light4Light = lightColour[3] * pow( max( dot( worldNormal.xyz, halfway4), 0 ), 256);
	light4Dist = length(lightPos[3] - vOut.worldPos.xyz) / lightBright[3];
	light4Light = light4Light / (  light4Dist );

	float3 specularLight = light1Light + light2Light + light3Light + light4Light;

	float4 texColour = DiffuseMap.Sample(Bilinear, vOut.UV);
	return texColour * ( float4(diffuseLight, 1.0) + float4(specularLight, 1.0) );
}

technique10 t4LitTexture
{
	pass P0
	{
		SetVertexShader( CompileShader( vs_4_0, BasicTransformLitTex() ) );
        SetGeometryShader( NULL );  
		SetPixelShader( CompileShader( ps_4_0, Lit4Texture() ) );
		SetBlendState( NoBlend, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
		SetRasterizerState( CullNone ); 
	}
}

technique10 t4LitAttenTexture
{
	pass P0
	{
		SetVertexShader( CompileShader( vs_4_0, BasicTransformLitTex() ) );
        SetGeometryShader( NULL );  
		SetPixelShader( CompileShader( ps_4_0, Lit4AttenTexture() ) );
		SetBlendState( NoBlend, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
		SetRasterizerState( CullNone ); 
	}
}

//--------------------------------------------------------------------------------------
// Advanced Lit Textures
//--------------------------------------------------------------------------------------

struct VS_PMAP_INPUT
{
    float3 Pos     : POSITION;
    float3 Normal  : NORMAL;
	float3 Tangent : TANGENT;
	float2 UV      : TEXCOORD0;
};

struct VS_PMAP_OUTPUT
{
	float4 ProjPos		: SV_POSITION;
	float2 UV			: TEXCOORD0;
	float4 worldPos		: POSITION;
	float4 Normal		: NORMAL;
	float3 Tangent		: TANGENT;
};

VS_BASIC_OUTPUT NormMapTransform( VS_BASIC_INPUT vIn )
{
	VS_BASIC_OUTPUT vOut;
	
	// Use world matrix passed from C++ to transform the input model vertex position into world space
	float4 modelPos = float4(vIn.Pos, 1.0f); // Promote to 1x4 so we can multiply by 4x4 matrix, put 1.0 in 4th element for a point (0.0 for a vector)
	float4 worldPos = mul( modelPos, WorldMatrix );
	float4 viewPos  = mul( worldPos, ViewMatrix );
	vOut.ProjPos    = mul( viewPos,  ProjMatrix );
	
	// Pass texture coordinates (UVs) on to the pixel shader
	vOut.UV = vIn.UV;

	return vOut;
}

float4 Lit4SpecMap( VS_LIT_OUTPUT vOut ) : SV_Target
{
	float4 texColour = DiffuseMap.Sample(Bilinear, vOut.UV);

	float4 worldNormal = normalize( vOut.worldNormal );
	
	float3 cameraVector = normalize(cameraPos - vOut.worldPos.xyz);
	float3 lightVector  = normalize(lightPos[0] - vOut.worldPos.xyz);
	float3 lightVector2 = normalize(lightPos[1] - vOut.worldPos.xyz);
	float3 lightVector3 = normalize(lightPos[2] - vOut.worldPos.xyz);
	float3 lightVector4 = normalize(lightPos[3] - vOut.worldPos.xyz);

	float specPower = texColour.a;

	float3 light1Light = lightColour[0] * max( dot( worldNormal.xyz, lightVector), 0 );
	float light1Dist = length(lightPos[0] - vOut.worldPos.xyz) / lightBright[0];
	light1Light = light1Light / (  light1Dist );
	
	float3 light2Light = lightColour[1] * max( dot( worldNormal.xyz, lightVector2), 0 );
	float light2Dist = length(lightPos[0] - vOut.worldPos.xyz) / lightBright[1];
	light2Light = light2Light / (  light2Dist );
	
	float3 light3Light = lightColour[2] * max( dot( worldNormal.xyz, lightVector3), 0 );
	float light3Dist = length(lightPos[2] - vOut.worldPos.xyz) / lightBright[2];
	light3Light = light3Light / (  light3Dist );

	float3 light4Light = lightColour[3] * max( dot( worldNormal.xyz, lightVector4), 0 );
	float light4Dist = length(lightPos[0] - vOut.worldPos.xyz) / lightBright[3];
	light4Light = light4Light / (  light4Dist );
	
	float3 diffuseLight = ambientColour + light1Light + light2Light + light3Light + light4Light;

	float3 halfway1 = normalize( lightVector + cameraVector );
	float3 halfway2 = normalize( lightVector2 + cameraVector );
	float3 halfway3 = normalize( lightVector3 + cameraVector );
	float3 halfway4 = normalize( lightVector4 + cameraVector );

	light1Light = lightColour[0] * pow( max( dot( worldNormal.xyz, halfway1), 0 ), specPower);
	light1Dist = length(lightPos[0] - vOut.worldPos.xyz) / lightBright[0];
	light1Light = light1Light / (  light1Dist );

	light2Light = lightColour[1] * pow( max( dot( worldNormal.xyz, halfway2), 0 ), specPower);
	light2Dist = length(lightPos[1] - vOut.worldPos.xyz) / lightBright[1];
	light2Light = light2Light / (  light2Dist );

	light3Light = lightColour[2] * pow( max( dot( worldNormal.xyz, halfway3), 0 ), specPower);
	light3Dist = length(lightPos[2] - vOut.worldPos.xyz) / lightBright[2];
	light3Light = light3Light / (  light3Dist );

	light4Light = lightColour[3] * pow( max( dot( worldNormal.xyz, halfway4), 0 ), specPower);
	light4Dist = length(lightPos[3] - vOut.worldPos.xyz) / lightBright[3];
	light4Light = light4Light / (  light4Dist );

	float3 specularLight = light1Light + light2Light + light3Light + light4Light;


	return texColour * ( float4(diffuseLight, 1.0) + float4(specularLight, 1.0) );
}

technique10 t4LitSpecularMap
{
	pass P0
	{
		SetVertexShader( CompileShader( vs_4_0, BasicTransformLitTex() ) );
        SetGeometryShader( NULL );  
		SetPixelShader( CompileShader( ps_4_0, Lit4SpecMap() ) );
		SetBlendState( NoBlend, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
		SetRasterizerState( CullNone ); 
	}
}
/*
technique10 t4LitSpecNormParaMapped
{
	pass P0
	{
		SetVertexShader( CompileShader( vs_4_0, BasicTransformLitTex() ) );
		SetGeometryShader( NULL );
		SetPixelShader( CompileShader( ps_4_0, Lit4AllMaps() ) );
		SetBlendState( NoBlend, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
		SetRasterizerState( CullNone );
	}
}
*/