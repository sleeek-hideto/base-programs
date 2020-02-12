#pragma once

class CGlTextureImpl
{
protected:
	typedef struct _BoxTexture{
		char loc[ 128 ];
		ChImageInterface *img;
		unsigned int name;
	} BoxTexture;
	static const int MAX_TEX = 32;	//!<テクスチャ・イメージの最大数
	BoxTexture m_ArrBox[ MAX_TEX ];	//!<テクスチャ・イメージを格納する場所

	bool m_bEnableClamp;
	bool m_bImgReverseUpDown;
	bool m_bImgReverseRGB2BGR;

	int SearchSameLocImg( char *loc );
	int GetEmptyBox();

public:
	CGlTextureImpl(){
		for( int t=0; t<MAX_TEX; t++ ){
			m_ArrBox[ t ].loc[ 0 ] = 0;
			m_ArrBox[ t ].img = NULL;
			m_ArrBox[ t ].name = 0;
		}

		m_bEnableClamp = true;
		m_bImgReverseUpDown = false;
		m_bImgReverseRGB2BGR = true;
	}
	~CGlTextureImpl(){
		for( int t=0; t<MAX_TEX; t++ ){
			delete m_ArrBox[ t ].img;
		}
	}

	unsigned int GetName( int id ){
		return m_ArrBox[ id ].name;
	}
	ChImageInterface *GetImg( int id ){
		return m_ArrBox[ id ].img;
	}

	int GetIndexTex( char *loc ){ return SearchSameLocImg(loc); }

	void EnableClamp( bool b ){		m_bEnableClamp = b;		}
	void EnableImgReverseUpDown( bool b ){		m_bImgReverseUpDown = b;		}
	void EnableImgReverseRGB2BGR( bool b ){		m_bImgReverseRGB2BGR = b;		}

	int MakeTexture( ChImgC3* img );
	int MakeTextureWithAlpha( ChImgC4* img );
	int MakeTexture( char *locImg );
	int MakeTextureWithAlpha( char *locImg, unsigned char r=0, unsigned char g=0, unsigned char b=0 );
	int MakeTextureWithAlpha( char *locImg, char *locAlpha );
	int MakeTextureDepth( int width, int height );
	bool OverDrawSubImage( int id, ChImageChar* imgSub, int offsetw, int offseth );
	bool OverDrawSubImage( int fromId, int toId, int fromx, int fromy, int tox, int toy, int width, int height );
	bool OverDrawSubImageDepth( int id );
	bool ClearSubImage( int id );

	void BindUnit0( int id );
	void BindUnit1( int id );
	void BindUnit2( int id );
	void BindUnit3( int id );
	void BindUnit( int id ){
		BindUnit0( id );
	}
	void UnbindUnit();
};

class CGlTexture
{
private:
	static CGlTextureImpl *m_Impl;
public:
	static CGlTextureImpl *Get(){
		if( m_Impl == NULL )m_Impl = new CGlTextureImpl;
		return m_Impl;
	}
	static void Destroy(){
		delete m_Impl;
		m_Impl = NULL;
	}
};
