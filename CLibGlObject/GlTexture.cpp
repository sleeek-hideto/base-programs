#include "StdAfx.h"
#include "GlTexture.h"

CGlTextureImpl *CGlTexture::m_Impl = NULL;

/*
	@brief 同じ名前のイメージを探す
	@param[in] loc イメージの場所
	@return 同じ名前のイメージのテクスチャIDを返す、ない場合は-1
*/
int CGlTextureImpl::SearchSameLocImg( char *loc )
{
	for( int tid=0; tid<MAX_TEX; tid++ )
	{
		if( strcmp( m_ArrBox[ tid ].loc, loc ) == 0 ){
			return tid;
		}
	}
	return -1;
}

/*
	@brief 空のテクスチャ・ボックスを得る
	@return テクスチャID。空きがない場合は-1
*/
int CGlTextureImpl::GetEmptyBox()
{
	for( int tid=0; tid<MAX_TEX; tid++ )
	{
		if( m_ArrBox[ tid ].name == 0 )return tid;
	}
	return -1;
}

/*
	@brief テクスチャを作る
	@param[in] img イメージ
	@return 新しく作られたテクスチャIDを返す
*/
int CGlTextureImpl::MakeTexture( ChImgC3* img )
{
	int tid = GetEmptyBox();
	if( tid < 0 )return -1;

	ChImgC3 *imgNew = new ChImgC3;
	imgNew->clone( img );
	m_ArrBox[ tid ].img = imgNew;
	m_ArrBox[ tid ].loc[ 0 ] = 0;

	unsigned int tname;
	glGenTextures( 1, &tname );
	if( tname == 0 )return -1;
	m_ArrBox[ tid ].name = tname;

	glBindTexture( GL_TEXTURE_2D, tname );
	glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB8, img->getWidth(), img->getHeight(), 0, GL_RGB, GL_UNSIGNED_BYTE, img->getOrigin() );
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	if( m_bEnableClamp )
	{	/* カートーンでは、クランプ */
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	}else{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	}
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	return tid;
}

/*
	@brief アルファ付きテクスチャを作る
	@param[in] img イメージ
	@return 新しく作られたテクスチャIDを返す
*/
int CGlTextureImpl::MakeTextureWithAlpha( ChImgC4* img )
{
	int tid = GetEmptyBox();
	if( tid < 0 )return -1;

	ChImgC4 *imgNew = new ChImgC4;
	imgNew->clone( img );
	m_ArrBox[ tid ].img = imgNew;
	m_ArrBox[ tid ].loc[ 0 ] = 0;

	unsigned int tname;
	glGenTextures( 1, &tname );
	if( tname == 0 )return -1;
	m_ArrBox[ tid ].name = tname;

	glBindTexture( GL_TEXTURE_2D, tname );
	glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA8, img->getWidth(), img->getHeight(), 0, GL_RGBA, GL_UNSIGNED_BYTE, img->getOrigin() );
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	if( m_bEnableClamp )
	{	/* カートーンでは、クランプ */
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	}else{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	}
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	return tid;
}

/*
	@brief テクスチャを作る
	@param[in] locImg イメージの場所
	@return 新しく作られたテクスチャIDを返す
*/
int CGlTextureImpl::MakeTexture( char *locImg )
{
	int tid = SearchSameLocImg( locImg );
	if( tid >= 0 )return tid;

	tid = GetEmptyBox();
	if( tid < 0 )return -1;

	ChImgC3 *imgNew = new ChImgC3;
	if( imgNew->load( locImg ) == false )return -1;
	m_ArrBox[ tid ].img = imgNew;
	strcpy( m_ArrBox[ tid ].loc, locImg );

	if( m_bImgReverseRGB2BGR )imgNew->convertRGBtoBGR();
	if( m_bImgReverseUpDown )imgNew->convertUPtoDOWN();

	unsigned int tname;
	glGenTextures( 1, &tname );
	if( tname == 0 )return -1;
	m_ArrBox[ tid ].name = tname;

	glBindTexture( GL_TEXTURE_2D, tname );
	glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB8, imgNew->getWidth(), imgNew->getHeight(), 0, GL_RGB, GL_UNSIGNED_BYTE, imgNew->getOrigin() );
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	if( m_bEnableClamp )
	{	/* カートーンでは、クランプ */
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	}else{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	}
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	return tid;
}

/*
	@brief アルファ付きテクスチャを作る
	@param[in] locImg イメージの場所
	@return 新しく作られたテクスチャIDを返す
*/
int CGlTextureImpl::MakeTextureWithAlpha( char *locImg, unsigned char r, unsigned char g, unsigned char b )
{
	int tid = SearchSameLocImg( locImg );
	if( tid >= 0 )return tid;

	tid = GetEmptyBox();
	if( tid < 0 )return -1;

	ChImgC4 *imgNew = new ChImgC4;
	if( imgNew->load( locImg ) == false )return -1;
	m_ArrBox[ tid ].img = imgNew;
	strcpy( m_ArrBox[ tid ].loc, locImg );

	imgNew->setTransparent( r, g, b );
	if( m_bImgReverseRGB2BGR )imgNew->convertRGBAtoBGRA();
	if( m_bImgReverseUpDown )imgNew->convertUPtoDOWN();

	unsigned int tname;
	glGenTextures( 1, &tname );
	if( tname == 0 )return -1;
	m_ArrBox[ tid ].name = tname;

	glBindTexture( GL_TEXTURE_2D, tname );
	glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA8, imgNew->getWidth(), imgNew->getHeight(), 0, GL_RGBA, GL_UNSIGNED_BYTE, imgNew->getOrigin() );
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	if( m_bEnableClamp )
	{	/* カートーンでは、クランプ */
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	}else{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	}
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	return tid;
}

/*!
	@brief アルファ付きテクスチャを作る
	@param[in] locImg イメージの場所
	@param[in] locAlpha アルファ・イメージの場所
	@return 新しく作られたテクスチャIDを返す
*/
int CGlTextureImpl::MakeTextureWithAlpha( char *locImg, char *locAlpha )
{
	int tid = SearchSameLocImg( locImg );
	if( tid >= 0 )return tid;

	tid = GetEmptyBox();
	if( tid < 0 )return -1;

	ChImgC4 *imgNew = new ChImgC4;
	if( imgNew->load( locImg ) == false )return -1;
	m_ArrBox[ tid ].img = imgNew;
	strcpy( m_ArrBox[ tid ].loc, locImg );

	///// アルファの設定
	ChImgC1 imgAlpha;
	unsigned char *pi, *pa;
	if( imgAlpha.load( locAlpha ) )
	{
		for( int h=0; h<imgAlpha.getHeight(); h++ )
		{
			pi = imgNew->getPixel( h );
			pa = imgAlpha.getPixel( h );
			for( int w=0; w<imgAlpha.getWidth(); w++ ){
				pi[ 3 ] = pa[ 0 ];
				pi += 4; pa += 1;
			}
		}
	}else{
		for( int h=0; h<imgAlpha.getHeight(); h++ )
		{
			pi = imgNew->getPixel( h );
			for( int w=0; w<imgAlpha.getWidth(); w++ ){
				pi[ 3 ] = 255;
				pi += 4;
			}
		}		
	}

	if( m_bImgReverseRGB2BGR )imgNew->convertRGBAtoBGRA();
	if( m_bImgReverseUpDown )imgNew->convertUPtoDOWN();

	unsigned int tname;
	glGenTextures( 1, &tname );
	if( tname == 0 )return -1;
	m_ArrBox[ tid ].name = tname;

	glBindTexture( GL_TEXTURE_2D, tname );
	glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA8, imgNew->getWidth(), imgNew->getHeight(), 0, GL_RGBA, GL_UNSIGNED_BYTE, imgNew->getOrigin() );
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	if( m_bEnableClamp )
	{	/* カートーンでは、クランプ */
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	}else{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	}
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	return tid;
}

/*!
	@brief デプス・テクスチャを作成する
	@param[in] width, height 幅、高さ
	@return 新しく作られたテクスチャIDを返す
*/
int CGlTextureImpl::MakeTextureDepth( int width, int height )
{
	int tid = GetEmptyBox();
	if( tid < 0 )return -1;

	//ChImgF1 *imgNew = new ChImgF1;
	ChImgC3 *imgNew = new ChImgC3;	
	imgNew->create( height, width );
	m_ArrBox[ tid ].img = imgNew;
	m_ArrBox[ tid ].loc[ 0 ] = 0;
	unsigned char rgb[3] = { 254,254,254 };
	imgNew->setAll( rgb );

	unsigned int tname;
	glGenTextures( 1, &tname );
	if( tname == 0 )return -1;
	m_ArrBox[ tid ].name = tname;

	glBindTexture( GL_TEXTURE_2D, tname );
	///// channel 1だとうまくいかない？
	//glTexImage2D( GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, imgNew->getOrigin() );
	glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB8, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, imgNew->getOrigin() );
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
/*
	///// 書き込むポリゴンのテクスチャ座標値のＲとテクスチャとの比較を行うようにする
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);
	///// もしＲの値がテクスチャの値以下なら真、つまり日向
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
	///// 比較の結果を輝度値として得る
	glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE, GL_LUMINANCE);
*/
	return tid;
}

/*!
	@brief テクスチャの一部を上書きする
	@param[in] id テクスチャID
	@param[in] imgSub 上書きするサブ・イメージ
	@param[in] offsetw, offseth オフセット
*/
bool CGlTextureImpl::OverDrawSubImage( int id, ChImageChar* imgSub, int offsetw, int offseth )
{
	ChImageChar *imgMain = (ChImageChar*)GetImg( id );
	if( imgMain == NULL )return false;
	if( imgMain->getChannel() != imgSub->getChannel() )return false;

	glBindTexture( GL_TEXTURE_2D, GetName( id ) );
	if( imgMain->getChannel() == 3 ){
		glTexSubImage2D( GL_TEXTURE_2D, 0, offsetw, offseth, imgSub->getWidth(), imgSub->getHeight(), GL_RGB, GL_UNSIGNED_BYTE, imgSub->getOrigin() );
	}else if( imgMain->getChannel() == 4 ){
		glTexSubImage2D( GL_TEXTURE_2D, 0, offsetw, offseth, imgSub->getWidth(), imgSub->getHeight(), GL_RGBA, GL_UNSIGNED_BYTE, imgSub->getOrigin() );
	}
	return true;
}

/*!
	@brief テクスチャの一部を上書きする
	@param[in] fromId 上書き元のテクスチャID
	@param[in] toId 上書き先のテクスチャID
	@param[in] fromx, fromy オフセット
	@param[in] tox, toy オフセット
	@param[in] width, height イメージの大きさ
*/
bool CGlTextureImpl::OverDrawSubImage( int fromId, int toId, int fromx, int fromy, int tox, int toy, int width, int height )
{
	ChImageChar *imgFrom = (ChImageChar*)GetImg( fromId );
	ChImageChar *imgTo = (ChImageChar*)GetImg( toId );
	if( imgFrom == NULL || imgTo == NULL )return false;
	if( imgFrom->getChannel() != imgTo->getChannel() )return false;

	glBindTexture( GL_TEXTURE_2D, GetName( fromId ) );
	if( imgFrom->getChannel() == 3 ){
		ChImgC3 imgc3;
		imgc3.trim( imgFrom, fromy, fromx, height, width );
		glTexSubImage2D( GL_TEXTURE_2D, 0, tox, toy, imgc3.getWidth(), imgc3.getHeight(), GL_RGB, GL_UNSIGNED_BYTE, imgc3.getOrigin() );
	}else if( imgTo->getChannel() == 4 ){
		ChImgC4 imgc4;
		imgc4.trim( imgFrom, fromy, fromx, height, width );
		glTexSubImage2D( GL_TEXTURE_2D, 0, tox, toy, imgc4.getWidth(), imgc4.getHeight(), GL_RGBA, GL_UNSIGNED_BYTE, imgc4.getOrigin() );
	}
	return true;
}

bool CGlTextureImpl::OverDrawSubImageDepth( int id )
{
	//ChImgF1 *timg = (ChImgF1*)GetImg( id );
	ChImgC3 *timg = (ChImgC3*)GetImg( id );
	if( timg == NULL )return false;
	int width = timg->getWidth();
	int height = timg->getHeight();

	unsigned int tname = GetName( id );
	if( tname == 0 )return false;

	glBindTexture( GL_TEXTURE_2D, tname );
	glReadBuffer( GL_BACK );
	//glCopyTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, 0,0, width,height, 0 );
	//glCopyTexImage2D( GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, 0,0, width,height, 0 );
	glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0, width, height);

	//glReadPixels( 0, 0, width,height, GL_DEPTH_COMPONENT, GL_FLOAT, timg->getOrigin() );/////値は0から1まで
	//glTexSubImage2D( GL_TEXTURE_2D, 0, 0,0, width,height, GL_DEPTH_COMPONENT, GL_FLOAT, timg->getOrigin() );
	//glReadPixels( 0, 0, width,height, GL_RGB, GL_UNSIGNED_BYTE, timg->getOrigin() );/////値は0から1まで
	//glTexSubImage2D( GL_TEXTURE_2D, 0, 0,0, width,height, GL_RGB, GL_UNSIGNED_BYTE, timg->getOrigin() );

	//timg->save( "lgtScr.jpg" );

	return true;
}

/*!
	@brief 上書きされたイメージを消去する
	@param[in] id テクスチャID
*/
bool CGlTextureImpl::ClearSubImage( int id )
{
	ChImageChar *img = (ChImageChar*)GetImg( id );
	if( img == NULL )return false;

	glBindTexture( GL_TEXTURE_2D, GetName( id ) );
	if( img->getChannel() == 3 ){
		glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB8, img->getWidth(), img->getHeight(), 0, GL_RGB, GL_UNSIGNED_BYTE, img->getOrigin() );
	}else if( img->getChannel() == 4 ){
		glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA8, img->getWidth(), img->getHeight(), 0, GL_RGBA, GL_UNSIGNED_BYTE, img->getOrigin() );
	}
	return true;
}

/*
	@brief ユニットを0に設定し、idのテクスチャをユニット0に接続させる
	@param[in] id テクスチャID
*/
void CGlTextureImpl::BindUnit0( int id )
{
	if( id < 0 )return;
	glActiveTexture( GL_TEXTURE0 );
	glEnable( GL_TEXTURE_2D );
	glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );
	glBindTexture(GL_TEXTURE_2D, GetName( id ) );
}

/*
	@brief ユニットを1に設定し、idのテクスチャをユニット1に接続させる
	@param[in] id テクスチャID
*/

void CGlTextureImpl::BindUnit1( int id )
{
	if( id < 0 )return;
	glActiveTexture( GL_TEXTURE1 );
	glEnable( GL_TEXTURE_2D );
	glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );
	glBindTexture(GL_TEXTURE_2D, GetName( id ) );
}

/*
	@brief ユニットを2に設定し、idのテクスチャをユニット2に接続させる
	@param[in] id テクスチャID
*/
void CGlTextureImpl::BindUnit2( int id )
{
	if( id < 0 )return;
	glActiveTexture( GL_TEXTURE2 );
	glEnable( GL_TEXTURE_2D );
	glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );
	glBindTexture(GL_TEXTURE_2D, GetName( id ) );
}

/*
	@brief ユニットを3に設定し、idのテクスチャをユニット3に接続させる
	@param[in] id テクスチャID
*/
void CGlTextureImpl::BindUnit3( int id )
{
	if( id < 0 )return;
	glActiveTexture( GL_TEXTURE3 );
	glEnable( GL_TEXTURE_2D );
	glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );
	glBindTexture(GL_TEXTURE_2D, GetName( id ) );
}

/*
	@brief 現在のユニットからテクスチャの接続を解除する
*/
void CGlTextureImpl::UnbindUnit()
{
	glBindTexture( GL_TEXTURE_2D, 0 );
	glDisable(GL_TEXTURE_2D);
}
