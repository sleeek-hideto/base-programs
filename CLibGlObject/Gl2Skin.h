#pragma once
#include "glskin.h"

class CGl2Skin : public CGlSkin
{
protected:
	CGl2Skin *m_SkinWorld, *m_SkinDeformed;	//!<世界座標内でのスキン、デフォームされたスキン
	CMatrix4x4 m_MatDeformed;

	int m_TexIds[ 4 ];
	CArrayVector3H *m_ArrTex1, *m_ArrTex2, *m_ArrTex3;	//!<マルチテクスチャ用のテクスチャ座標。１をToonに、２をBumpに使う予定

	CArrayVector3H *GetTargetTex( int ind ){
		if( ind == 0 ){
			return m_ArrTex;
		}else if( ind == 1 ){
			return m_ArrTex1;
		}else if( ind == 2 ){
			return m_ArrTex2;
		}else if( ind == 3 ){
			return m_ArrTex3;
		}
		return NULL;
	}

public:
	CGl2Skin(void){
		m_SkinWorld = NULL;
		m_SkinDeformed = NULL;
		m_MatDeformed.SetI();

		m_TexIds[ 0 ] = -1;
		m_TexIds[ 1 ] = -1;
		m_TexIds[ 2 ] = -1;
		m_TexIds[ 3 ] = -1;
		m_ArrTex1 = NULL;
		m_ArrTex2 = NULL;
		m_ArrTex3 = NULL;
	}
	~CGl2Skin(void){
		if( m_ArrTex1 != NULL ){	delete m_ArrTex1;	m_ArrTex1 = NULL;	}
		if( m_ArrTex2 != NULL ){	delete m_ArrTex2;	m_ArrTex2 = NULL;	}
		if( m_ArrTex3 != NULL ){	delete m_ArrTex3;	m_ArrTex3 = NULL;	}
	}

	void Copy( CGl2Skin *pSkin );

	CGl2Skin *GetSibling()						{	return (CGl2Skin*)m_SkinSibling;	}

	void SetSkinWorld( CGl2Skin *skin )		{	m_SkinWorld = skin;	}
	CGl2Skin *GetSkinWorld()					{	return m_SkinWorld;	}
	void SetSkinDeformed( CGl2Skin *skin )		{	m_SkinDeformed = skin;	}
	CGl2Skin *GetSkinDeformed()				{	return m_SkinDeformed;	}

	void SetMatDeformed( double* data )				{	m_MatDeformed.SetData( data );	}
	void SetMatDeformed( CMatrix4x4* mat )			{	m_MatDeformed.Copy( mat );	}
	CMatrix4x4* GetMatDeformed()					{	return &m_MatDeformed;	}

	int MakeTexture( int unitInd, ChImgC3 *pImg, bool isClamped=false );
	int MakeTextureWithAlpha( int unitInd, ChImgC4 *pImg, bool isClamped=false );
	int MakeTexture( int unitInd, char *locFile, bool isClamped=false );
	int MakeTextureWithAlpha( int unitInd, char *locFile, bool isClamped=false, unsigned char r=0, unsigned char g=0, unsigned char b=0 );
	int MakeTextureWithAlpha( int unitInd, char *locFile, char *locAlpha, bool isClamped=false );

	void SetTexture( int unitInd, unsigned int tid, bool alphaBlend=false ){
		if( unitInd > 3 )return;
		m_TexIds[ unitInd ] = tid;
		m_bAlphaBlend = alphaBlend;
	}
	int GetTexture( int unitInd ){
		if( unitInd > 3 || unitInd < 0 )return -1;
		return m_TexIds[ unitInd ];
	}

	bool CalcTex_FromSphereNormal( int texInd=0 );

	void Draw( char *type=NULL );
};
