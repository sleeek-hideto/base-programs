#include "StdAfx.h"
#include "GlTexture.h"

CGlTextureImpl *CGlTexture::m_Impl = NULL;

/*
	@brief �������O�̃C���[�W��T��
	@param[in] loc �C���[�W�̏ꏊ
	@return �������O�̃C���[�W�̃e�N�X�`��ID��Ԃ��A�Ȃ��ꍇ��-1
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
	@brief ��̃e�N�X�`���E�{�b�N�X�𓾂�
	@return �e�N�X�`��ID�B�󂫂��Ȃ��ꍇ��-1
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
	@brief �e�N�X�`�������
	@param[in] img �C���[�W
	@return �V�������ꂽ�e�N�X�`��ID��Ԃ�
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
	{	/* �J�[�g�[���ł́A�N�����v */
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
	@brief �A���t�@�t���e�N�X�`�������
	@param[in] img �C���[�W
	@return �V�������ꂽ�e�N�X�`��ID��Ԃ�
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
	{	/* �J�[�g�[���ł́A�N�����v */
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
	@brief �e�N�X�`�������
	@param[in] locImg �C���[�W�̏ꏊ
	@return �V�������ꂽ�e�N�X�`��ID��Ԃ�
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
	{	/* �J�[�g�[���ł́A�N�����v */
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
	@brief �A���t�@�t���e�N�X�`�������
	@param[in] locImg �C���[�W�̏ꏊ
	@return �V�������ꂽ�e�N�X�`��ID��Ԃ�
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
	{	/* �J�[�g�[���ł́A�N�����v */
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
	@brief �A���t�@�t���e�N�X�`�������
	@param[in] locImg �C���[�W�̏ꏊ
	@param[in] locAlpha �A���t�@�E�C���[�W�̏ꏊ
	@return �V�������ꂽ�e�N�X�`��ID��Ԃ�
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

	///// �A���t�@�̐ݒ�
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
	{	/* �J�[�g�[���ł́A�N�����v */
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
	@brief �f�v�X�E�e�N�X�`�����쐬����
	@param[in] width, height ���A����
	@return �V�������ꂽ�e�N�X�`��ID��Ԃ�
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
	///// channel 1���Ƃ��܂������Ȃ��H
	//glTexImage2D( GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, imgNew->getOrigin() );
	glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB8, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, imgNew->getOrigin() );
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
/*
	///// �������ރ|���S���̃e�N�X�`�����W�l�̂q�ƃe�N�X�`���Ƃ̔�r���s���悤�ɂ���
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);
	///// �����q�̒l���e�N�X�`���̒l�ȉ��Ȃ�^�A�܂����
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
	///// ��r�̌��ʂ��P�x�l�Ƃ��ē���
	glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE, GL_LUMINANCE);
*/
	return tid;
}

/*!
	@brief �e�N�X�`���̈ꕔ���㏑������
	@param[in] id �e�N�X�`��ID
	@param[in] imgSub �㏑������T�u�E�C���[�W
	@param[in] offsetw, offseth �I�t�Z�b�g
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
	@brief �e�N�X�`���̈ꕔ���㏑������
	@param[in] fromId �㏑�����̃e�N�X�`��ID
	@param[in] toId �㏑����̃e�N�X�`��ID
	@param[in] fromx, fromy �I�t�Z�b�g
	@param[in] tox, toy �I�t�Z�b�g
	@param[in] width, height �C���[�W�̑傫��
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

	//glReadPixels( 0, 0, width,height, GL_DEPTH_COMPONENT, GL_FLOAT, timg->getOrigin() );/////�l��0����1�܂�
	//glTexSubImage2D( GL_TEXTURE_2D, 0, 0,0, width,height, GL_DEPTH_COMPONENT, GL_FLOAT, timg->getOrigin() );
	//glReadPixels( 0, 0, width,height, GL_RGB, GL_UNSIGNED_BYTE, timg->getOrigin() );/////�l��0����1�܂�
	//glTexSubImage2D( GL_TEXTURE_2D, 0, 0,0, width,height, GL_RGB, GL_UNSIGNED_BYTE, timg->getOrigin() );

	//timg->save( "lgtScr.jpg" );

	return true;
}

/*!
	@brief �㏑�����ꂽ�C���[�W����������
	@param[in] id �e�N�X�`��ID
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
	@brief ���j�b�g��0�ɐݒ肵�Aid�̃e�N�X�`�������j�b�g0�ɐڑ�������
	@param[in] id �e�N�X�`��ID
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
	@brief ���j�b�g��1�ɐݒ肵�Aid�̃e�N�X�`�������j�b�g1�ɐڑ�������
	@param[in] id �e�N�X�`��ID
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
	@brief ���j�b�g��2�ɐݒ肵�Aid�̃e�N�X�`�������j�b�g2�ɐڑ�������
	@param[in] id �e�N�X�`��ID
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
	@brief ���j�b�g��3�ɐݒ肵�Aid�̃e�N�X�`�������j�b�g3�ɐڑ�������
	@param[in] id �e�N�X�`��ID
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
	@brief ���݂̃��j�b�g����e�N�X�`���̐ڑ�����������
*/
void CGlTextureImpl::UnbindUnit()
{
	glBindTexture( GL_TEXTURE_2D, 0 );
	glDisable(GL_TEXTURE_2D);
}
