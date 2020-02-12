#include "StdAfx.h"
#include "GlBillBoard.h"

/*!
	@brief ����������A�{�[�h�̐����w�肷��
	@param[in] cntBoard �{�[�h�̐�
*/
void CGlBillBoard::Create( int cntBoard )
{
	if( cntBoard < 1 )return;
	if( m_Bone != NULL )delete [] m_Bone;
	if( m_Skin != NULL )delete [] m_Skin;

	m_BoardMax = cntBoard;
	m_Bone = new CGl2Bone[ cntBoard ];
	m_Skin = new CGl2Skin[ cntBoard ];

	double col[4] = { 1.0, 1.0, 1.0, 1.0 };
	for( int c=0; c<cntBoard; c++ ){
		m_Bone[ c ].AddSkin( m_Skin +c );
		m_Skin[ c ].SetColorFilter( col );
	}
}

/*!
	@brief �X�e�[�^�X�E�o�[�����
	@param[in] ind �{�[�h�E�C���f�b�N�X
	@param[in] locFile �e�N�X�`���摜�̏ꏊ
*/
bool CGlBillBoard::MakeObject( int id, char *locFile )
{
	if( MakePolygon( id ) == false )return false;
	if( m_Skin[ id ].MakeTexture( 0, locFile ) < 0 )return false;
	
	return true;
}

/*!
	@brief �X�e�[�^�X�E�o�[�����
	@param[in] ind �{�[�h�E�C���f�b�N�X
	@param[in] img �e�N�X�`���摜
*/
bool CGlBillBoard::MakeObject( int id, ChImgC3 *img )
{
	if( MakePolygon( id ) == false )return false;
	if( m_Skin[ id ].MakeTexture( 0, img ) < 0 )return false;
	return true;
}

/*!
	@brief �A���t�@�t���̃X�e�[�^�X�E�o�[�����
	@param[in] ind �{�[�h�E�C���f�b�N�X
	@param[in] locFile �e�N�X�`���摜�̏ꏊ
	@param[in] r,g,b �����F�̎w��
*/
bool CGlBillBoard::MakeObjectWithAlpha( int id, char *locFile, unsigned char r, unsigned char g, unsigned char b )
{
	if( MakePolygon( id ) == false )return false;
	if( m_Skin[ id ].MakeTextureWithAlpha( 0, locFile, false, r,g,b ) < 0 )return false;
	return true;
}

/*!
	@brief �A���t�@�t���X�e�[�^�X�E�o�[�����
	@param[in] ind �{�[�h�E�C���f�b�N�X
	@param[in] locFile �C���[�W�̏ꏊ
	@param[in] locALpha �A���t�@�摜�̏ꏊ
*/
bool CGlBillBoard::MakeObjectWithAlpha( int id, char *locFile, char *locAlpha )
{
	if( MakePolygon( id ) == false )return false;
	if( m_Skin[ id ].MakeTextureWithAlpha( 0, locFile, locAlpha ) < 0 )return false;
	return true;
}

/*!
	@brief �A���t�@�t���X�e�[�^�X�E�o�[�����
	@param[in] ind �{�[�h�E�C���f�b�N�X
	@param[in] img �e�N�X�`���摜
*/
bool CGlBillBoard::MakeObjectWithAlpha( int id, ChImgC4 *img )
{
	if( MakePolygon( id ) == false )return false;
	if( m_Skin[ id ].MakeTextureWithAlpha( 0, img ) < 0 )return false;
	return true;
}

/*!
	@brief �X�g���[�Wdata����r���E�{�[�h���\�z����
*/
bool CGlBillBoard::LoadObject( Ch3PlainData *data )
{
	data->SetNameSpace( "Board" );
	if( data->Get( -1, "boardMax", &m_BoardMax ) == 0 ) return false;

	Create( m_BoardMax );

	///// make object
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

		///// �ʒu
		double tmpV4[ 4 ] = { 0,0,0,1 };
		if( data->Get( b, "boardPos", tmpV4 ) > 3 ){
			SetPos( b, tmpV4 );
		}

		///// �傫��
		if( data->Get( b, "boardSize", tmpV4 ) > 1 ){
			SetSize( b, tmpV4[ 0 ], tmpV4[ 1 ] );
		}
		///// �g��k��
		if( data->Get( b, "boardScale", tmpV4 ) > 1 ){
			SetScale( b, tmpV4[ 0 ], tmpV4[ 1 ] );
		}
		///// �e�N�X�`���E�N���b�v
		if( data->Get( b, "boardClipTex", tmpV4 ) > 3 ){
			SetClipTex( b, tmpV4[0], tmpV4[1], tmpV4[2], tmpV4[3] );
		}

		///// �F
		if( data->Get( b, "boardCol", tmpV4 ) > 3 ){
			SetColor( b, tmpV4 );
		}
	}
	return true;
}

/*!
	@brief �X�g���[�Wdata�̃t���[��frame�A���Oname�̃f�[�^��ǂݍ���
*/
bool CGlBillBoard::LoadPosture( Ch3PlainData *data, int frame, char *name )
{
	if( data == NULL )return false;

	double tmpV4[ 4 ];
	char nameBoard[32]; nameBoard[0] = 0;
	if( name != NULL )sprintf( nameBoard, "%s", name );
	char namePart[32];

	data->SetNameSpace( "Frame" );
	for( int b=0; b<m_BoardMax; b++ )
	{
		///// �ʒu
		sprintf( namePart, "%s/Bn%02d/Pos", nameBoard, b );
		if( data->Get( frame, namePart, tmpV4 ) > 3 ){
			SetPos( b, tmpV4 );
		}

		///// �傫��
		sprintf( namePart, "%s/Sk%02d/Size", nameBoard, b );
		if( data->Get( frame, namePart, tmpV4 ) > 1 ){
			SetSize( b, tmpV4[ 0 ], tmpV4[ 1 ] );
		}
		///// �g��k��
		sprintf( namePart, "%s/Sk%02d/Scale", nameBoard, b );
		if( data->Get( frame, namePart, tmpV4 ) > 1 ){
			SetScale( b, tmpV4[ 0 ], tmpV4[ 1 ] );
		}
		///// �e�N�X�`���E�N���b�v
		sprintf( namePart, "%s/Sk%02d/ClipTex", nameBoard, b );
		if( data->Get( frame, namePart, tmpV4 ) > 3 ){
			SetClipTex( b, tmpV4[0], tmpV4[1], tmpV4[2], tmpV4[3] );
		}

		///// �F
		sprintf( namePart, "%s/Sk%02d/Col", nameBoard, b );
		if( data->Get( frame, namePart, tmpV4 ) > 3 ){
			SetColor( b, tmpV4 );
		}
	}
	return true;
}

/*!
	@brief �X�L��id���g��k������
*/
void CGlBillBoard::SetScale( int id, double height, double width )
{
	if( m_Skin == NULL || m_Skin[ id ].m_ArrPos == NULL ) return;
	double *posData = m_Skin[ id ].m_ArrPos->GetData();

	posData[ 0 ] *= width; posData[ 1 ] *= height;
	posData[ 4 ] *= width; posData[ 5 ] *= height;
	posData[ 8 ] *= width; posData[ 9 ] *= height;
	posData[ 12 ] *= width; posData[ 13 ] *= height;
	return;
}

/*!
	@brief �X�L��id�̑傫�����w�肷��
*/
void CGlBillBoard::SetSize( int id, double height, double width )
{
	if( m_Skin == NULL || m_Skin[ id ].m_ArrPos == NULL ) return;
	double *posData = m_Skin[ id ].m_ArrPos->GetData();
	height /= 2.0; width /= 2.0;/////����

	posData[ 0 ] = -width; posData[ 1 ] = height;
	posData[ 4 ] = -width; posData[ 5 ] = -height;
	posData[ 8 ] = width; posData[ 9 ] = -height;
	posData[ 12 ] = width; posData[ 13 ] = height;
	return;
}

/*!
	@brief �X�L��id�̃e�N�X�`�����W�𒲐�����
*/
void CGlBillBoard::SetClipTex( int id, double left, double bottom, double right, double head )
{
	if( m_Skin == NULL || m_Skin[ id ].m_ArrTex == NULL ) return;
	double *texData = m_Skin[ id ].m_ArrTex->GetData();
	texData[ 0 ] = left; texData[ 1 ] = 1.0 -head;
	texData[ 4 ] = left; texData[ 5 ] = 1.0 -bottom;
	texData[ 8 ] = right; texData[ 9 ] = 1.0 -bottom;
	texData[ 12 ] = right; texData[ 13 ] = 1.0 -head;
}

/*!
	@brief �`�ʂ���
*/
void CGlBillBoard::Draw()
{
	CVector3H angCam;
	CGlCamera *camera = CGlRendering::Get()->GetCamera();

	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();

	for( int b=0; b<m_BoardMax; b++ )
	{
		angCam.SetData( 0,0,0 );

		if( camera != NULL )
		{
			if( camera->IsModeAngleManual() ){
				angCam.Copy( camera->GetAngleCamera() );
				//angCam.MulConst( -1.0 );	///// ����Ŗ{���ɂ悢�̂�
			}else if( camera->IsModeAngleLookAt() ){
				CVector3H dir, nrm, tmp, axisY( 0,1,0 );
				nrm.Sub( camera->GetPosCamera(), m_Bone[ b ].GetPosLocal() );
				//nrm.Sub( camera->GetPosCamera(), camera->GetPosLookAt() );
				nrm.Normalize();
				double len = nrm.GetData()[ 1 ];
				tmp.MulConst( len, &nrm );
				dir.Sub( &axisY, &tmp );
				angCam.CalcAngleFromDirNrm_YXZ( &dir, &nrm );
			}
		}

		m_Bone[ b ].SetAngle( &angCam );
		m_Bone[ b ].Draw_recursive();
	}
}

/*!
	@brief �{�[�h���\�z����
*/
bool CGlBillBoard::MakePolygon( int id )
{
	if( id >= m_BoardMax ) return false;

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

	m_Skin[ id ].MakePolygon( 4, posData, 2, indData, 4, nrmData, 4, texData );
	return true;
}