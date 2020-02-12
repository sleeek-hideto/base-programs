#include "StdAfx.h"
#include "GlWindowBoard.h"

/*!
	@brief 初期化する、ボードの数を指定する
	@param[in] cntBoard ボードの数
*/
void CGlWindowBoard::Create( int cntBoard )
{
	if( cntBoard < 1 )return;
	if( m_Bone != NULL )delete [] m_Bone;
	if( m_Skin != NULL )delete [] m_Skin;

	m_BoardMax = cntBoard;
	m_Bone = new CGl2Bone[ cntBoard ];
	m_Skin = new CGl2Skin[ cntBoard ];

	double col[4] = { 1.0, 1.0, 1.0, 1.0 };
	for( int b=0; b<cntBoard; b++ ){
		m_Bone[ b ].AddSkin( m_Skin +b );
		m_Skin[ b ].SetColorFilter( col );
	}
}

/*!
	@brief ステータス・バーを作る
	@param[in] ind ボード・インデックス
	@param[in] locFile テクスチャ画像の場所
*/
bool CGlWindowBoard::MakeObject( int ind, char *locFile )
{
	if( MakePolygon( ind ) == false )return false;
	if( m_Skin[ ind ].MakeTexture( 0, locFile ) < 0 )return false;
	
	return true;
}

/*!
	@brief ステータス・バーを作る
	@param[in] ind ボード・インデックス
	@param[in] img テクスチャ画像
*/
bool CGlWindowBoard::MakeObject( int ind, ChImgC3 *img )
{
	if( MakePolygon( ind ) == false )return false;
	if( m_Skin[ ind ].MakeTexture( 0, img ) < 0 )return false;
	return true;
}

/*!
	@brief アルファ付きのステータス・バーを作る
	@param[in] ind ボード・インデックス
	@param[in] locFile テクスチャ画像の場所
	@param[in] r,g,b 透明色の指定
*/
bool CGlWindowBoard::MakeObjectWithAlpha( int ind, char *locFile, unsigned char r, unsigned char g, unsigned char b )
{
	if( MakePolygon( ind ) == false )return false;
	if( m_Skin[ ind ].MakeTextureWithAlpha( 0, locFile, false, r,g,b ) < 0 )return false;
	return true;
}

/*!
	@brief アルファ付きステータス・バーを作る
	@param[in] ind ボード・インデックス
	@param[in] locFile イメージの場所
	@param[in] locALpha アルファ画像の場所
*/
bool CGlWindowBoard::MakeObjectWithAlpha( int ind, char *locFile, char *locAlpha )
{
	if( MakePolygon( ind ) == false )return false;
	if( m_Skin[ ind ].MakeTextureWithAlpha( 0, locFile, locAlpha ) < 0 )return false;
	return true;
}

/*!
	@brief アルファ付きステータス・バーを作る
	@param[in] ind ボード・インデックス
	@param[in] img テクスチャ画像
*/
bool CGlWindowBoard::MakeObjectWithAlpha( int ind, ChImgC4 *img )
{
	if( MakePolygon( ind ) == false )return false;
	if( m_Skin[ ind ].MakeTextureWithAlpha( 0, img ) < 0 )return false;
	return true;
}

/*!
	@brief ストレージからステータス・バーを作る
	@param[in] data ストレージ
*/
bool CGlWindowBoard::LoadObject( Ch2PlainData *data )
{
	///// make object
	if( data->get( -1, "boardMax", &m_BoardMax ) == 0 )return false;
	if( m_BoardMax < 1 )return false;

	Create( m_BoardMax );

	for( int b=0; b<m_BoardMax; b++ )
	{
		char locTex[ 64 ], locAlpha[ 64 ];
		if( data->get( b, "boardLocTex", locTex ) > 0 )
		{
			if( data->get( b, "boardLocAlpha", locAlpha ) > 0 )
			{
				MakeObjectWithAlpha( b, locTex, locAlpha );
			}else{
				MakeObject( b, locTex );
			}
		}else if( data->get( b, "boardLocTexAlphaBlack", locTex ) > 0 ){
			MakeObjectWithAlpha( b, locTex, 0,0,0 );
		}

		///// scale
		double tmpV4[ 4 ] = { 1,1,1,1 };
		if( data->get( b, "boardScale", tmpV4 ) > 1 ){
			SetScale( b, tmpV4[0], tmpV4[1] );
		}

		///// pos
		if( data->get( b, "boardPos", tmpV4 ) > 2 ){
			SetPos( b, tmpV4 );
		}

		///// color
		if( data->get( b, "boardColor", tmpV4 ) > 3 ){
			SetColor( b, tmpV4 );
		}
	}

	return true;
}

/*!
	@brief ストレージからステータス・バーを作る
	@param[in] data ストレージ
*/
bool CGlWindowBoard::LoadObject( Ch3PlainData *data )
{
	///// make object
	data->SetNameSpace( "Board" );
	if( data->Get( -1, "boardMax", &m_BoardMax ) == 0 )return false;
	if( m_BoardMax < 1 )return false;

	Create( m_BoardMax );

	for( int b=0; b<m_BoardMax; b++ )
	{
		char locTex[ 128 ], locAlpha[ 128 ];
		if( data->Get( b, "boardLocTex", locTex ) > 0 )
		{
			if( data->Get( b, "boardLocAlpha", locAlpha ) > 0 )
			{
				if( !MakeObjectWithAlpha( b, locTex, locAlpha ) ) continue;
			}else{
				if( !MakeObject( b, locTex ) ) continue;
			}
		}else if( data->Get( b, "boardLocTexAlphaBlack", locTex ) > 0 ){
			if( !MakeObjectWithAlpha( b, locTex, 0,0,0 ) ) continue;
		}else{
			continue;
		}

		///// 位置
		double tmpV4[ 4 ] = { 0,0,0,1 };
		if( data->Get( b, "boardPos", tmpV4 ) > 2 ){
			SetPos( b, tmpV4 );
		}

		///// 大きさ
		if( data->Get( b, "boardSize", tmpV4 ) > 1 ){
			SetSize( b, tmpV4[ 0 ], tmpV4[ 1 ] );
		}
		///// 拡大縮小
		if( data->Get( b, "boardScale", tmpV4 ) > 1 ){
			SetScale( b, tmpV4[ 0 ], tmpV4[ 1 ] );
		}
		///// テクスチャ・クリップ
		if( data->Get( b, "boardClipTex", tmpV4 ) > 3 ){
			SetClipTex( b, tmpV4[0], tmpV4[1], tmpV4[2], tmpV4[3] );
		}

		///// 色
		if( data->Get( b, "boardCol", tmpV4 ) > 3 ){
			SetColor( b, tmpV4 );
		}
	}

	return true;
}

/*!
	@brief 姿勢などを読み込む
	@param[in] data ストレージ
	@param[in] frame フレーム数
	@param[in] name このボードの名前
*/
bool CGlWindowBoard::LoadPosture( Ch3PlainData *data, int frame, char *name )
{
	if( data == NULL )return false;

	double tmpV4[ 4 ];
	char nameBoard[32]; nameBoard[0] = 0;
	if( name != NULL )sprintf( nameBoard, "%s", name );
	char namePart[32];

	data->SetNameSpace( "Frame" );
	for( int b=0; b<m_BoardMax; b++ )
	{
		///// 位置
		sprintf( namePart, "%s/Bn%02d/Pos", nameBoard, b );
		if( data->Get( frame, namePart, tmpV4 ) > 3 ){
			SetPos( b, tmpV4 );
		}
		///// 角度
		sprintf( namePart, "%s/Bn%02d/Angle", nameBoard, b );
		if( data->Get( frame, namePart, tmpV4 ) > 3 ){
			SetAngle( b, tmpV4 );
		}

		///// 大きさ
		sprintf( namePart, "%s/Sk%02d/Size", nameBoard, b );
		if( data->Get( frame, namePart, tmpV4 ) > 1 ){
			SetSize( b, tmpV4[ 0 ], tmpV4[ 1 ] );
		}
		///// 拡大縮小
		sprintf( namePart, "%s/Sk%02d/Scale", nameBoard, b );
		if( data->Get( frame, namePart, tmpV4 ) > 1 ){
			SetScale( b, tmpV4[ 0 ], tmpV4[ 1 ] );
		}
		///// テクスチャ
		sprintf( namePart, "%s/Sk%02d/ClipTex", nameBoard, b );
		if( data->Get( frame, namePart, tmpV4 ) > 3 ){
			SetClipTex( b, tmpV4[0], tmpV4[1], tmpV4[2], tmpV4[3] );
		}

		///// 色
		sprintf( namePart, "%s/Sk%02d/Col", nameBoard, b );
		if( data->Get( frame, namePart, tmpV4 ) > 3 ){
			SetColor( b, tmpV4 );
		}
	}

	return true;
}

/*!
	@brief スキンidを拡大縮小する
*/
void CGlWindowBoard::SetScale( int ind, double height, double width )
{
	if( m_Skin == NULL || m_Skin[ ind ].m_ArrPos == NULL ) return;
	double *posData = m_Skin[ ind ].m_ArrPos->GetData();

	posData[ 0 ] *= width; posData[ 1 ] *= height;
	posData[ 4 ] *= width; posData[ 5 ] *= height;
	posData[ 8 ] *= width; posData[ 9 ] *= height;
	posData[ 12 ] *= width; posData[ 13 ] *= height;
	return;
}

/*!
	@brief スキンidの大きさを指定する
*/
void CGlWindowBoard::SetSize( int ind, double height, double width )
{
	if( m_Skin == NULL || m_Skin[ ind ].m_ArrPos == NULL ) return;
	double *posData = m_Skin[ ind ].m_ArrPos->GetData();
	height /= 2.0; width /= 2.0;/////注意

	posData[ 0 ] = -width; posData[ 1 ] = height;
	posData[ 4 ] = -width; posData[ 5 ] = -height;
	posData[ 8 ] = width; posData[ 9 ] = -height;
	posData[ 12 ] = width; posData[ 13 ] = height;
	return;
}

/*!
	@brief スキンidのテクスチャ座標を調整する
*/
void CGlWindowBoard::SetClipTex( int ind, double left, double bottom, double right, double head )
{
	if( m_Skin == NULL || m_Skin[ ind ].m_ArrTex == NULL ) return;
	double *texData = m_Skin[ ind ].m_ArrTex->GetData();
	texData[ 0 ] = left; texData[ 1 ] = 1.0 -head;
	texData[ 4 ] = left; texData[ 5 ] = 1.0 -bottom;
	texData[ 8 ] = right; texData[ 9 ] = 1.0 -bottom;
	texData[ 12 ] = right; texData[ 13 ] = 1.0 -head;
}

/*!
	@brief 描写する
*/
void CGlWindowBoard::Draw()
{
	CGlRenderingImpl *rendering = CGlRendering::Get();
	glMatrixMode( GL_PROJECTION );
	glPushMatrix();
	if( rendering->GetCamera() != NULL )rendering->GetCamera()->SendMatrixProjectionInit();

	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();

	for( int b=0; b<m_BoardMax; b++ ){
		m_Bone[ b ].Draw_recursive();
	}

	glMatrixMode( GL_PROJECTION );
	glPopMatrix();
}

/*!
	@brief ステータス・バーのポリゴンを作る
	@param[in] ind インデックス
*/
bool CGlWindowBoard::MakePolygon( int ind )
{
	if( ind >= m_BoardMax ) return false;

	double left=0.0, bottom=1.0, right=1.0, head=0.0;
	double posData[ 16 ] = {
		-1.0, 1.0, 0.0, 1.0,
		-1.0, -1.0, 0.0, 1.0,
		1.0, -1.0, 0.0, 1.0,
		1.0, 1.0, 0.0, 1.0
	};
	unsigned int indData[ 6 ] = {
		0, 1, 2,
		2, 3, 0
	};
	double nrmData[ 16 ] = {
		0.0, 0.0, 1.0, 1.0,
		0.0, 0.0, 1.0, 1.0,
		0.0, 0.0, 1.0, 1.0,
		0.0, 0.0, 1.0, 1.0
	};
	double texData[ 16 ] = {
		left, head, 0.0, 1.0,		
		left, bottom, 0.0, 1.0,
		right, bottom, 0.0, 1.0,		
		right, head, 0.0, 1.0
	};

	m_Skin[ ind ].MakePolygon( 4, posData, 2, indData, 4, nrmData, 4, texData );
	return true;
}
