#include "TextureManager.h"


CTextureManager::CTextureManager(ID3D10Device* device)
{
	mpd3dDevice = device;
}


CTextureManager::~CTextureManager(void)
{
}

ID3D10ShaderResourceView* CTextureManager::LoadTexture ( char filename[50] )
{
	
	for( int i = 0; i < miNumTextures; i++ )
	{
		if( strcmp( filename, mcTexturesFile[i] ) == 0 )
		{
			return mpTextures[i];
		}
	}
	
	ID3D10ShaderResourceView* newTexture;
	D3DX10CreateShaderResourceViewFromFileA( mpd3dDevice, filename,		NULL, NULL, &newTexture, NULL );
	if( newTexture == nullptr )
	{
		return nullptr;
	}

	mpTextures[miNumTextures] = newTexture;
	miNumTextures++;
	
}

void CTextureManager::UnloadTexture( char filename[50] )
{

}

void CTextureManager::UnloadTexture( int index )
{

}