#include "StdAfx.h"
#include ".\glskin.h"

///// ///// Skin ///// /////
/*!
	@brief �X�L�����R�s�[����
	@param[in] pSkin ���̃X�L��
*/
void CGlSkin::Copy( CGlSkin *pSkin )
{
	/* Primitive */
	m_Primitive = pSkin->m_Primitive;
	/* Pos */
	if( pSkin->m_ArrPos != NULL && m_ArrPos == NULL )
	{
		m_ArrPos = new CArrayVector3H( pSkin->m_ArrPos->GetRows(), pSkin->m_ArrPos->GetData() );
	}
	/* Nrm */
	if( pSkin->m_ArrNrm != NULL && m_ArrNrm == NULL )
	{
		m_ArrNrm = new CArrayVector3H( pSkin->m_ArrNrm->GetRows(), pSkin->m_ArrNrm->GetData() );
	}
	/* Col */
	if( pSkin->m_ArrCol != NULL && m_ArrCol == NULL )
	{
		m_ArrCol = new CArrayVector3H( pSkin->m_ArrCol->GetRows(), pSkin->m_ArrCol->GetData() );
	}
	/* Tex */
	if( pSkin->m_ArrTex != NULL && m_ArrTex == NULL )
	{
		m_ArrTex = new CArrayVector3H( pSkin->m_ArrTex->GetRows(), pSkin->m_ArrTex->GetData() );
	}
	/* Index */
	if( pSkin->m_ArrIndex != NULL && m_ArrIndex == NULL )
	{
		m_ArrIndex = new CUintArrayVector3( pSkin->m_ArrIndex->GetRows(), pSkin->m_ArrIndex->GetData() );
	}
	/* Col */
	if( pSkin->m_ColAmbient != NULL && m_ColAmbient == NULL ){
		m_ColAmbient = new CVector4( pSkin->m_ColAmbient->GetData() );
	}
	if( pSkin->m_ColDiffuse != NULL && m_ColDiffuse == NULL ){
		m_ColDiffuse = new CVector4( pSkin->m_ColDiffuse->GetData() );
	}
	if( pSkin->m_ColSpecular != NULL && m_ColSpecular == NULL ){
		m_ColSpecular = new CVector4( pSkin->m_ColSpecular->GetData() );
	}
	m_ColFilter.Copy( &pSkin->m_ColFilter );

	m_nTexId = pSkin->m_nTexId;
}

/*!
	@brief �����̂̃X�L�������
	@param[in] length ��ӂ̒���
*/
void CGlSkin::MakeCube( double length )
{
	/* Primitive */
	m_Primitive = GL_TRIANGLES;

	/* Pos */
	double cubePos[32] = {
		1.0,1.0,1.0,1.0, 1.0,-1.0,1.0,1.0, 1.0,-1.0,-1.0,1.0, 1.0,1.0,-1.0,1.0,
		-1.0,1.0,1.0,1.0, -1.0,-1.0,1.0,1.0, -1.0,-1.0,-1.0,1.0, -1.0,1.0,-1.0,1.0,
	};
	for( int i=0; i<32; i++ ){	if( i%4==3 )continue;	cubePos[i] *= length;	}
	m_ArrPos = new CArrayVector3H( 8, cubePos );

	/* Nrm */
	double objLength = length * sqrt( (double)3.0 );
	m_ArrNrm = new CArrayVector3H( 8 );
	double *pDataNrm = m_ArrNrm->GetData();
	for( int v=0; v<8; v++ )
	{
		pDataNrm[0] = cubePos[ v*4 ] / objLength;	pDataNrm[1] = cubePos[ v*4 +1 ] / objLength;	pDataNrm[2] = cubePos[ v*4 +2 ] / objLength;	pDataNrm[3] = 1.0;
		pDataNrm += 4;
	}
	
	/* Index */
	GLuint cubeIndex[] = {
		0,1,2, 0,2,3,
		0,4,5, 0,5,1,  1,5,6, 1,6,2,  2,6,7, 2,7,3,  3,7,4, 3,4,0,
		7,6,5, 7,5,4
	};
	m_ArrIndex = new CUintArrayVector3( 12, cubeIndex );

	/* Col */
	double cubeCol[4] = { 0.75, 0.75, 0.75, 1.0 };
	if( m_ColDiffuse == NULL ){		m_ColDiffuse = new CVector4( cubeCol );		}
	if( m_ColSpecular == NULL ){		m_ColSpecular = new CVector4( cubeCol );		}
}

/*!
	@brief ���̃X�L�������
	@param[in] radius ���̔��a
	@param[in] height �������������ɂ��ăv���~�e�B�u����邩
	@param[in] width �����������ɂ��ăv���~�e�B�u����邩
*/
void CGlSkin::MakeSphere( double radius, int height, int width )
{
	/* Primitive */
	m_Primitive = GL_TRIANGLES;

	/* Pos & Nrm */
	int TATE_DIVIDE = height;	const int YOKO_DIVIDE = width;
	const double PAI = 3.1415926535;
	//int VERTEX_MAX = (TATE_DIVIDE-1)*YOKO_DIVIDE + 2;
	int VERTEX_MAX = (TATE_DIVIDE-1)*(YOKO_DIVIDE+1) + 2;

	m_ArrPos = new CArrayVector3H( VERTEX_MAX );
	m_ArrNrm = new CArrayVector3H( VERTEX_MAX );
	double *pPos = m_ArrPos->GetData();
	double *pNrm = m_ArrNrm->GetData();

	///// ��̒��_
	pPos[0]=0.0;	pPos[1]=radius;	pPos[2]=0.0;	pPos[3]=1.0;
	pNrm[0]=0.0;	pNrm[1]=1.0;	pNrm[2]=0.0;	pNrm[3]=1.0;

	///// ���̒��_
	pNrm[VERTEX_MAX*4-4]=pPos[VERTEX_MAX*4-4]=0.0;	pPos[VERTEX_MAX*4-3]=-radius;	pNrm[VERTEX_MAX*4-2]=pPos[VERTEX_MAX*4-2]=0.0;
	pNrm[VERTEX_MAX*4-3]=-1.0;
	pPos[VERTEX_MAX*4-1]=pNrm[VERTEX_MAX*4-1]=1.0;

	pNrm += 4; pPos += 4;
	double X, Y, Z;
	int tate, yoko;
	for(tate=1; tate<TATE_DIVIDE; tate++)
	{
		for(yoko=0; yoko<=YOKO_DIVIDE; yoko++)
		{
			double tmpYoko;
			if( yoko==YOKO_DIVIDE ){ tmpYoko=(double)yoko-0.001; }else{ tmpYoko=(double)yoko; }
			Y = cos( (double)tate * PAI/(double)TATE_DIVIDE );
			X = sin( (double)tate * PAI/(double)TATE_DIVIDE ) * sin( tmpYoko * 2.0 * PAI/(double)YOKO_DIVIDE );
			Z = sin( (double)tate * PAI/(double)TATE_DIVIDE ) * cos( tmpYoko * 2.0 * PAI/(double)YOKO_DIVIDE );
			pNrm[0]=X; pNrm[1]=Y; pNrm[2]=Z; pNrm[3]=1.0;
			pPos[0]=X * radius;	pPos[1]=Y * radius;	pPos[2]=Z * radius; pPos[3]=1.0;
			pNrm += 4;	pPos += 4;
		}
	}

	/* index */
	int i;
	m_ArrIndex = new CUintArrayVector3( YOKO_DIVIDE*2 + (TATE_DIVIDE-2)*YOKO_DIVIDE*2 );
	unsigned int *pI = m_ArrIndex->GetData();

	for(i=0; i<YOKO_DIVIDE; i++)
	{
		pI[0]=0; pI[1]=i+1; pI[2]=i+2;
		pI += 3;
	}
	//pI[0]=0; pI[1]=YOKO_DIVIDE; pI[2]=1; pI += 3;

	for(tate=0; tate<TATE_DIVIDE-2; tate++)
	{
		for(yoko=0; yoko<YOKO_DIVIDE; yoko++)
		{
			pI[0] = tate*(YOKO_DIVIDE+1)+1+yoko;
			pI[2] = pI[3] = tate*(YOKO_DIVIDE+1)+2+yoko;
			pI[1] = pI[4] = (tate+1)*(YOKO_DIVIDE+1)+1+yoko;
			pI[5] = (tate+1)*(YOKO_DIVIDE+1)+2+yoko;
			pI += 6;
		}
		//pI[0] = (tate+1)*YOKO_DIVIDE;
		//pI[2] = pI[3] = tate*YOKO_DIVIDE+1;
		//pI[1] = pI[4] = (tate+2)*YOKO_DIVIDE;
		//pI[5] = (tate+1)*YOKO_DIVIDE+1;
		//pI += 6;
	}

	for(i=0; i<YOKO_DIVIDE; i++)
	{
		pI[0]=VERTEX_MAX-1; pI[1]=VERTEX_MAX-YOKO_DIVIDE+i -1; pI[2]=VERTEX_MAX-YOKO_DIVIDE+i -2;
		pI += 3;
	}
	//pI[0]=VERTEX_MAX-1; pI[1]=VERTEX_MAX-YOKO_DIVIDE-1; pI[2]=VERTEX_MAX-2; pI += 3;

	/* Col */
	double cubeCol[4] = { 0.75, 0.75, 0.75, 1.0 };
	if( m_ColDiffuse == NULL ){		m_ColDiffuse = new CVector4( cubeCol );		}
	if( m_ColSpecular == NULL ){		m_ColSpecular = new CVector4( cubeCol );		}
}

/*!
	@brief �O�p�`�̏W���̔C�ӂ̃X�L�������
	@param[in] posCnt ���_��
	@param[in] pPos ���_�z��ւ̃|�C���^
	@param[in] indCnt �C���f�b�N�X�̐�
	@param[in] pInd �C���f�b�N�X�z��ւ̃|�C���^
	@param[in] texCnt �e�N�X�`�����W�̐��AposCnt�Ɠ������Ȃ���΂Ȃ�Ȃ�
	@param[in] pTex �e�N�X�`�����W�z��ւ̃|�C���^
*/
void CGlSkin::MakePolygon( int posCnt, double *pPos, int indCnt, unsigned int *pInd, int nrmCnt, double *pNrm, int texCnt, double *pTex )
{
	///// Primitive
	m_Primitive = GL_TRIANGLES;

	///// Pos
	m_ArrPos = new CArrayVector3H( posCnt, pPos );

	///// Index
	m_ArrIndex = new CUintArrayVector3( indCnt, pInd );

	///// Normal
	if( nrmCnt > 0 ){
		m_ArrNrm = new CArrayVector3H( nrmCnt, pNrm );
	}

	///// Tex
	if( texCnt > 0 ){
		m_ArrTex = new CArrayVector3H( texCnt, pTex );
	}
}

/*!
	@brief ���ׂĂ̎O�p�`�̖@���������Ōv�Z����
*/
bool CGlSkin::CalcNormal()
{
	int i;
	if( m_ArrPos == NULL || m_ArrIndex == NULL )return false;
	if( m_ArrNrm == NULL )
	{
		m_ArrNrm = new CArrayVector3H( m_ArrPos->GetRows() );
	}
	m_ArrNrm->SetDataAllZero();

	if( m_Primitive == GL_TRIANGLES ){
		double *pNrm = m_ArrNrm->GetData();
		double *pPos = m_ArrPos->GetData();
		unsigned int *pInd = m_ArrIndex->GetData();
		int indCnt = (int)m_ArrIndex->GetRows();
		double eachFaceNormal[ 3 ];

		for( i=0; i<indCnt; i++ ) ///// �O�p�`�̐�
		{
			CVector3H::Normal( pPos +pInd[0]*4, pPos +pInd[1]*4, pPos +pInd[2]*4, eachFaceNormal);
			CVector3H::Add( pNrm +pInd[0]*4, eachFaceNormal, pNrm +pInd[0]*4 );
			CVector3H::Add( pNrm +pInd[1]*4, eachFaceNormal, pNrm +pInd[1]*4 );
			CVector3H::Add( pNrm +pInd[2]*4, eachFaceNormal, pNrm +pInd[2]*4 );

			pInd += 3;
		}

		for( i=0; i<m_ArrNrm->GetRows(); i++ ) ///// ���_�̐�
		{
			if(pNrm[0]==0.0 && pNrm[1]==0.0 && pNrm[2]==0.0){
				continue;
			}else{
				CVector3H::Normalize( pNrm );
				pNrm[3] = 1.0;
			}
			pNrm += 4;
		}
		return true;
	}
	return false;
}

/*!
	@brief �e�N�X�`�����W�����g�̖@�����狁�߂�B
*/
bool CGlSkin::CalcTex_FromSphereNormal()
{
	if( m_ArrNrm == NULL )return false;

	int cntArr = m_ArrNrm->GetRows();
	if( m_ArrTex == NULL ){
		m_ArrTex = new CArrayVector3H( cntArr );
	}

	double *pNrm = m_ArrNrm->GetData();
	double *pTex = m_ArrTex->GetData();
	for( int t=0; t<cntArr; t++ )
	{
		pTex[1] = 1.0 - acos( pNrm[1] ) / PI;
		if( pNrm[0] == 0.0 && pNrm[2] == 0.0 ){
			pTex[0] = 0.5;
		}else{
			pTex[0] = -atan2( pNrm[0], -pNrm[2] ) / (2.0*PI) + 0.5;
		}
		pNrm += 4;	pTex += 4;
	}
	return true;
}

/*!
	@brief �C�ӂ́i���[�U�^�j�X�L�����X�g���[�W����ǂݍ���
	@param[in] data �X�g���[�W
	@param[in] frame �X�g���[�W�ł̃C���f�b�N�X�ԍ�
	@param[out] isNormalSet �@���z�񂪓ǂݍ��܂ꂽ��
	@param[out] isTexSet �e�N�X�`�����W�z�񂪓ǂݍ��܂ꂽ��
*/
bool CGlSkin::LoadSkinUser( Ch2PlainData *data, int frame, bool &isNormalSet, bool &isTexSet )
{
	bool myState = true;
	int posCnt, indCnt, nrmCnt=0, texCnt=0, c;
	if( data->get( -1, "userPosCnt", &posCnt ) == 0
		|| data->get( -1, "userIndCnt", &indCnt ) == 0 )return false;

	data->get( -1, "userNrmCnt", &nrmCnt );
	data->get( -1, "userTexCnt", &texCnt );
	if( nrmCnt > 0 && nrmCnt != posCnt ){
		nrmCnt = posCnt;
		myState = false;
	}
	if( texCnt > 0 && texCnt != posCnt ){
		texCnt = posCnt;
		myState = false;
	}
	this->MakePolygon( posCnt, NULL, indCnt, NULL, nrmCnt, NULL, texCnt, NULL );

	///// pos
	double vec[4];
	for( c=0; c<posCnt; c++ ){
		data->get( c, "pos", vec );
		this->SetPosRow( c, vec );
	}

	///// index
	int tmpIndex[3];
	unsigned int tmpUindex[3];
	for( c=0; c<indCnt; c++ )
	{
		data->get( c, "index", tmpIndex );
		tmpUindex[0] = tmpIndex[0];
		tmpUindex[1] = tmpIndex[1];
		tmpUindex[2] = tmpIndex[2];
		this->SetIndexRow( c, tmpUindex );
	}

	///// normal
	if( nrmCnt > 0 )
	{
		for( c=0; c<nrmCnt; c++ ){
			data->get( c, "normal", vec );
			this->SetNormalRow( c, vec );
		}
		isNormalSet = true;
	}

	///// texture
	if( texCnt > 0 )
	{
		for( c=0; c<posCnt; c++ ){
			data->get( c, "texture", vec );
			this->SetTexRow( c, vec );
		}
		isTexSet = true;
	}
	//m_SkinLocal.m_ArrPos->TestPrint( "posInit" );
	return myState;
}

/*!
	@brief �C�ӂ́i���[�U�^�j�X�L�����X�g���[�W����ǂݍ���
	@param[in] data �X�g���[�W
	@param[in] nameSpace �l�[���E�X�y�[�X
	@param[out] isNormalSet �@���z�񂪓ǂݍ��܂ꂽ��
	@param[out] isTexSet �e�N�X�`�����W�z�񂪓ǂݍ��܂ꂽ��
*/
bool CGlSkin::LoadSkinStorage( Ch3PlainData *data, char *nameSpace, bool &isNormalSet, bool &isTexSet )
{
	bool myState = true;
	int posCnt, indCnt, nrmCnt=0, texCnt=0, c;
	data->SetNameSpace( nameSpace );
	if( data->Get( -1, "posCnt", &posCnt ) == 0
		|| data->Get( -1, "indCnt", &indCnt ) == 0 )return false;

	data->Get( -1, "nrmCnt", &nrmCnt );
	data->Get( -1, "texCnt", &texCnt );
	if( nrmCnt > 0 && nrmCnt != posCnt ){
		nrmCnt = posCnt;
		myState = false;
	}
	if( texCnt > 0 && texCnt != posCnt ){
		texCnt = posCnt;
		myState = false;
	}
	this->MakePolygon( posCnt, NULL, indCnt, NULL, nrmCnt, NULL, texCnt, NULL );

	///// pos
	double vec[4];
	for( c=0; c<posCnt; c++ ){
		data->Get( c, "pos", vec );
		this->SetPosRow( c, vec );
	}

	///// index
	int tmpIndex[3];
	unsigned int tmpUindex[3];
	for( c=0; c<indCnt; c++ )
	{
		if( data->Get( c, "index", tmpIndex ) < 1 && data->Get( c, "idx", tmpIndex ) < 1 ) continue;
		tmpUindex[0] = tmpIndex[0];
		tmpUindex[1] = tmpIndex[1];
		tmpUindex[2] = tmpIndex[2];
		this->SetIndexRow( c, tmpUindex );
	}

	///// normal
	if( nrmCnt > 0 )
	{
		for( c=0; c<nrmCnt; c++ ){
			if( data->Get( c, "normal", vec ) > 0 || data->Get( c, "nor", vec ) > 0 )
				this->SetNormalRow( c, vec );
		}
		isNormalSet = true;
	}

	///// texture
	if( texCnt > 0 )
	{
		for( c=0; c<posCnt; c++ ){
			if( data->Get( c, "texture", vec ) > 0 || data->Get( c, "tex", vec ) > 0 )
				this->SetTexRow( c, vec );
		}
		isTexSet = true;
	}
	//m_SkinLocal.m_ArrPos->TestPrint( "posInit" );
	return myState;
}

/*!
	@brief Wavefront(.obj)�`���̃f�[�^����̉�Ƃ��ēǂݍ���
	@param[in] locFile �t�@�C���ʒu
	@param[out] isNormalSet �@�����Z�b�g���ꂽ�� 
	@param[out] isTexSet �e�N�X�`�����W���Z�b�g���ꂽ��

bool CGlSkin::LoadSkinMassWaveFront( char *locFile, bool &isNormalSet, bool &isTexSet )
{
	isNormalSet = false;
	isTexSet = false;
	FILE *file = fopen( locFile, "r" );
	if( file == NULL )return false;
	Ch2PlainData dataMtl;

	///// ���̒���
	int cntPosMax=0, cntIndMax=0, cntNrmMax=0, cntTexMax=0;
	char line[ 1024 ], type[ 16 ];
	while( fgets( line, 1024, file ) != NULL )
	{
		type[ 0 ] = 0;
		if( line[ 0 ] == '#' )continue;
		sscanf( line, "%s ", type );
		if( type[ 0 ] == 0 )continue;

		if( strcmp( type, "v" ) == 0 ){
			cntPosMax++;
		}else if( strcmp( type, "vn" ) == 0 ){
			cntNrmMax++;
		}else if( strcmp( type, "vt" ) == 0 ){
			cntTexMax++;
		}else if( strcmp( type, "f" ) == 0 ){
			cntIndMax++;
		}else if( strcmp( type, "mtllib" ) == 0 ){
			//char *pStr = line + strlen( type ) + 1;	///// �ŏ��̃X�y�[�X�̎��̕�����
			//int lenLine = (int)strlen( pStr );
			//if( pStr[ lenLine -1 ] == '\n' )pStr[ lenLine -1 ] = 0;	///// ���s�̏���

			//char locTmp[ 128 ];
			//if( sscanf( line, "%s", locTmp ) < 1 )continue;
			//LoadSkinWaveFrontMtl( locTmp, &dataMtl );
		}
	}

	///// �������̊m��
	if( cntPosMax == 0 || cntIndMax == 0 )return false;
	this->MakePolygon( cntPosMax, NULL, cntIndMax, NULL, cntPosMax, NULL, cntPosMax, NULL );
	double *tablePos=NULL, *tableNrm=NULL, *tableTex=NULL;
	unsigned int *tableInd=NULL;
	if( cntPosMax > 0 )tablePos = new double [ cntPosMax * 3 ];
	if( cntIndMax > 0 )tableInd = new unsigned int[ cntIndMax * 9 ];
	if( cntNrmMax > 0 )tableNrm = new double [ cntNrmMax * 3 ];
	if( cntTexMax > 0 )tableTex = new double [ cntTexMax * 3 ];

	///// �ǂݍ���
	double *tPosCur=tablePos, *tNrmCur=tableNrm, *tTexCur=tableTex;
	unsigned int *tIndCur=tableInd;
	fseek( file, 0, SEEK_SET );
	while( fgets( line, 1024, file ) != NULL )
	{
		type[ 0 ] = 0;
		if( line[ 0 ] == '#' )continue;
		sscanf( line, "%s ", type );
		if( type[ 0 ] == 0 )continue;

		char *pStr = line + strlen( type ) + 1;	///// �ŏ��̃X�y�[�X�̎��̕�����
		if( strcmp( type, "v" ) == 0 ){
			if( sscanf( pStr, "%lf %lf %lf", tPosCur, tPosCur +1, tPosCur +2 ) == 3 ){
				tPosCur += 3;
			}
		}else if( strcmp( type, "vn" ) == 0 ){
			if( sscanf( pStr, "%lf %lf %lf", tNrmCur, tNrmCur +1, tNrmCur +2 ) == 3 ){
				tNrmCur += 3;
			}
		}else if( strcmp( type, "vt" ) == 0 ){
			if( sscanf( pStr, "%lf %lf %lf", tTexCur, tTexCur +1, tTexCur +2 ) == 3 ){
				tTexCur += 3;
			}
		}else if( strcmp( type, "f" ) == 0 ){
			tIndCur[ 0 ] = 0;	tIndCur[ 1 ] = 0;	tIndCur[ 2 ] = 0;
			tIndCur[ 3 ] = 0;	tIndCur[ 4 ] = 0;	tIndCur[ 5 ] = 0;
			tIndCur[ 6 ] = 0;	tIndCur[ 7 ] = 0;	tIndCur[ 8 ] = 0;
			if( cntPosMax > 0 && cntNrmMax > 0 && cntTexMax > 0 ){
				sscanf( pStr, "%d/%d/%d %d/%d/%d %d/%d/%d",	///// pos/tex/nrm
					tIndCur, tIndCur +3, tIndCur +6,
					tIndCur +1, tIndCur +4, tIndCur +7,
					tIndCur +2, tIndCur +5, tIndCur +8
					);
				tIndCur[ 0 ]--;	tIndCur[ 1 ]--;	tIndCur[ 2 ]--;
				tIndCur[ 3 ]--;	tIndCur[ 4 ]--;	tIndCur[ 5 ]--;
				tIndCur[ 6 ]--;	tIndCur[ 7 ]--;	tIndCur[ 8 ]--;
				tIndCur += 9;
			}else if( cntNrmMax > 0 ){
				sscanf( pStr, "%d//%d %d//%d %d//%d",
					tIndCur, tIndCur +6,
					tIndCur +1, tIndCur +7,
					tIndCur +2, tIndCur +8
					);
				tIndCur[ 0 ]--;	tIndCur[ 1 ]--;	tIndCur[ 2 ]--;
				tIndCur[ 6 ]--;	tIndCur[ 7 ]--;	tIndCur[ 8 ]--;
				tIndCur += 9;
			}else if( cntTexMax > 0 ){
				sscanf( pStr, "%d/%d %d/%d %d/%d",
					tIndCur, tIndCur +3,
					tIndCur +1, tIndCur +4,
					tIndCur +2, tIndCur +5
					);
				tIndCur[ 0 ]--;	tIndCur[ 1 ]--;	tIndCur[ 2 ]--;
				tIndCur[ 3 ]--;	tIndCur[ 4 ]--;	tIndCur[ 5 ]--;
				tIndCur += 9;
			}else{
				sscanf( pStr, "%d %d %d",
					tIndCur, tIndCur +1, tIndCur +2
					);
				tIndCur[ 0 ]--;	tIndCur[ 1 ]--;	tIndCur[ 2 ]--;
				tIndCur += 9;
			}
		}
	}

	///// ���
	tIndCur = tableInd;
	for( int i=0; i<cntIndMax; i++ )
	{
		this->SetIndexRow( i, tIndCur );
		this->SetPosRow( tIndCur[0], tablePos +tIndCur[0]*3 );
		this->SetPosRow( tIndCur[1], tablePos +tIndCur[1]*3 );
		this->SetPosRow( tIndCur[2], tablePos +tIndCur[2]*3 );

		if( cntTexMax > 0 )
		{
			this->SetTexRow( tIndCur[0], tableTex +tIndCur[3]*3 );
			this->SetTexRow( tIndCur[1], tableTex +tIndCur[4]*3 );
			this->SetTexRow( tIndCur[2], tableTex +tIndCur[5]*3 );
		}

		if( cntNrmMax > 0 )
		{
			this->SetNomalRow( tIndCur[0], tableNrm +tIndCur[6]*3 );
			this->SetNomalRow( tIndCur[1], tableNrm +tIndCur[7]*3 );
			this->SetNomalRow( tIndCur[2], tableNrm +tIndCur[8]*3 );
		}

		tIndCur += 9;
	}

	fclose( file );
	if( tablePos != NULL )delete [] tablePos;
	if( tableInd != NULL )delete [] tableInd;
	if( tableNrm != NULL )delete [] tableNrm;
	if( tableTex != NULL )delete [] tableTex;
	if( cntNrmMax > 0 )isNormalSet = true;
	if( cntTexMax > 0 )isTexSet = true;
	return true;
}
*/

/*!
	@brief �X�L�����ړ�������
	@param[in] vec �ړ�����
*/
void CGlSkin::Shift( CVector3H *vec )
{
	if( m_ArrPos == NULL )return;
	m_ArrPos->ArrayAdd( vec );
}

/*!
	@brief �X�L�����ړ�������
	@param[in] x X�ړ���
	@param[in] y Y�ړ���
	@param[in] z Z�ړ���
*/
void CGlSkin::Shift( double x, double y, double z )
{
	if( m_ArrPos == NULL )return;
	CVector3H vec( x, y, z );
	m_ArrPos->ArrayAdd( &vec );
}

/*!
	@brief �X�L�����g��E�k������
	@param[in] vec �g��E�k����
*/
void CGlSkin::Scale( CVector3H *vec )
{
	if( m_ArrPos == NULL )return;
	double data[16] = {
		vec->GetData()[0],0,0,0,
		0,vec->GetData()[1],0,0,
		0,0,vec->GetData()[2],0,
		0,0,0,1.0
	};
	CMatrix4x4 matScale( data );
	m_ArrPos->ArrayMul( &matScale );
}

/*!
	@brief �X�L�����g��E�k��������
	@param[in] x X�g��E�k����
	@param[in] y Y�g��E�k����
	@param[in] z Z�g��E�k����
*/
void CGlSkin::Scale( double x, double y, double z )
{
	if( m_ArrPos == NULL )return;
	double data[16] = { x,0,0,0, 0,y,0,0, 0,0,z,0, 0,0,0,1.0 };
	CMatrix4x4 matScale( data );
	m_ArrPos->ArrayMul( &matScale );
}

/*!
	@brief �X�L������]����
	@param[in] vec �p�x
*/
void CGlSkin::Rotate( CVector3H *vec )
{
	if( m_ArrPos == NULL )return;
	double *datVec = vec->GetData();
	CMatrix4x4 matRot;
	matRot.SetRotYXZTran( datVec[0], datVec[1], datVec[2] );
	m_ArrPos->ArrayMul( &matRot );
}

/*!
	@brief �X�L������]������
	@param[in] x,y,z �p�x
*/
void CGlSkin::Rotate( double x, double y, double z )
{
	if( m_ArrPos == NULL )return;
	CMatrix4x4 matRot;
	matRot.SetRotYXZTran( x, y, z );
	m_ArrPos->ArrayMul( &matRot );
}

/*!
	@brief �X�L�����s��mat�œ��e����
	@param[in] mat �s��4x4
*/
void CGlSkin::Project( CMatrix4x4 *mat )
{
	if( m_ArrPos == NULL )return;
	m_ArrPos->ArrayMul( mat );
}

/*!
	@brief �X�L����`�悷��
	@param[in] type �ǂ̃X�L���E�^�C�v��`�悷�邩
*/
void CGlSkin::Draw( char *type )
{
	if( m_ArrIndex == NULL )return;

	CGlTextureImpl *texturing = CGlTexture::Get();
	bool isLightOff = false;

	if( type != NULL && strcmp( m_sType, type ) != 0 )return;

	if( m_nTexId >= 0 ){
		texturing->BindUnit( m_nTexId );
	}

	if( m_bAlphaBlend == true )
	{
		//glDisable(GL_CULL_FACE);
		glEnable(GL_BLEND);
		glDepthMask(GL_FALSE);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}

	if( m_ColAmbient == NULL && m_ColDiffuse == NULL && m_ColSpecular == NULL )
	{
		glDisable( GL_LIGHTING );
		glDisable( GL_LIGHT0 );
		isLightOff = true;
	}
	if( m_ColAmbient != NULL )
	{
		float tmpCol[4];
		double *pCol = m_ColAmbient->GetData();
		tmpCol[0] = (float)pCol[0];	tmpCol[1] = (float)pCol[1];	tmpCol[2] = (float)pCol[2];	tmpCol[3] = (float)pCol[3];
		glMaterialfv(GL_FRONT, GL_AMBIENT, tmpCol );
	}
	if( m_ColDiffuse != NULL )
	{
		float tmpCol[4];
		double *pCol = m_ColDiffuse->GetData();
		tmpCol[0] = (float)pCol[0];	tmpCol[1] = (float)pCol[1];	tmpCol[2] = (float)pCol[2];	tmpCol[3] = (float)pCol[3];
		glMaterialfv(GL_FRONT, GL_DIFFUSE, tmpCol );
	}
	if( m_ColSpecular != NULL )
	{
		float tmpCol[4];
		double *pCol = m_ColSpecular->GetData();
		tmpCol[0] = (float)pCol[0];	tmpCol[1] = (float)pCol[1];	tmpCol[2] = (float)pCol[2];	tmpCol[3] = (float)pCol[3];
		glMaterialfv(GL_FRONT, GL_SPECULAR, tmpCol );
		glMaterialf(GL_FRONT, GL_SHININESS, 20.0f);
	}
	///// �J���[�E�t�B���^�[
	glColor4dv( m_ColFilter.GetData() );

	if( m_ArrPos != NULL )
	{
		glEnableClientState(GL_VERTEX_ARRAY); glVertexPointer( 4, GL_DOUBLE, 0, m_ArrPos->GetData() );
	}
	if( m_ArrNrm != NULL )
	{
		glEnableClientState(GL_NORMAL_ARRAY); glNormalPointer( GL_DOUBLE, 32, m_ArrNrm->GetData() );
	}
	if( m_ArrCol != NULL )
	{
		glEnableClientState(GL_COLOR_ARRAY); glColorPointer( 4, GL_DOUBLE, 0, m_ArrCol->GetData() );
	}
	if( m_ArrTex != NULL && m_nTexId >= 0 )
	{	/* 2���̃e�N�X�`�� */
		glEnableClientState(GL_TEXTURE_COORD_ARRAY); glTexCoordPointer( 2, GL_DOUBLE, 32, m_ArrTex->GetData() );
		//m_ArrTex->TestPrint( "Tex", m_nTexId );
	}

	if( m_ArrIndex != NULL )
	{
		glDrawElements( m_Primitive, m_ArrIndex->GetRowsCols(), GL_UNSIGNED_INT, m_ArrIndex->GetData() );
	}

	if( m_nTexId >= 0 ){
		texturing->UnbindUnit();
	}

	if( m_bAlphaBlend == true )
	{
		//glEnable(GL_CULL_FACE);
		glDisable(GL_BLEND);
		glDepthMask(GL_TRUE);
	}

	if( isLightOff == true )
	{
		glEnable( GL_LIGHTING );
		glEnable( GL_LIGHT0 );
	}
}


/*!
	@brief �e�N�X�`�����X�L���ɓ\��B�e�N�X�`���͂��ׂẴX�L���ɋ��L�����
	@param[in] pImg �e�N�X�`���摜
	@return �e�N�X�`���ԍ���Ԃ�
*/
int CGlSkin::MakeTexture( ChImgC3 *pImg, bool isClamped )
{
	CGlTextureImpl *texturing = CGlTexture::Get();
	texturing->EnableClamp( isClamped );
	m_nTexId = texturing->MakeTexture( pImg );
	if( m_nTexId < 0 )return -1;

	if( m_nTexId >= 0 && m_ArrTex == NULL ){
		m_ArrTex = new CArrayVector3H( m_ArrPos->GetRows() );
	}

	return m_nTexId;
}

/*!
	@brief �A���t�@�t���e�N�X�`�����X�L���ɓ\��B�e�N�X�`���͂��ׂẴX�L���ɋ��L�����
	@param[in] pImg �A���t�@�t���e�N�X�`���摜
	@return �e�N�X�`���ԍ���Ԃ�
*/
int CGlSkin::MakeTextureWithAlpha( ChImgC4 *pImg, bool isClamped )
{
	CGlTextureImpl *texturing = CGlTexture::Get();
	texturing->EnableClamp( isClamped );
	m_nTexId = texturing->MakeTextureWithAlpha( pImg );
	if( m_nTexId < 0 )return -1;

	if( m_nTexId >= 0 && m_ArrTex == NULL ){
		m_ArrTex = new CArrayVector3H( m_ArrPos->GetRows() );
	}

	m_bAlphaBlend = true;

	return m_nTexId;
}

/*!
	@brief �e�N�X�`�����X�L���ɓ\��B�e�N�X�`���͂��ׂẴX�L���ɋ��L�����
	@param[in] pImg �e�N�X�`���摜�̏ꏊ
	@return �e�N�X�`���ԍ���Ԃ�
*/
int CGlSkin::MakeTexture( char *locFile, bool isClamped )
{
	CGlTextureImpl *texturing = CGlTexture::Get();
	texturing->EnableClamp( isClamped );
	m_nTexId = texturing->MakeTexture( locFile );
	if( m_nTexId < 0 )return -1;

	if( m_nTexId >= 0 && m_ArrTex == NULL ){
		m_ArrTex = new CArrayVector3H( m_ArrPos->GetRows() );
	}

	return m_nTexId;
}

/*!
	@brief �A���t�@�t���e�N�X�`�����X�L���ɓ\��B�e�N�X�`���͂��ׂẴX�L���ɋ��L�����
	@param[in] pImg �e�N�X�`���摜�̏ꏊ
	@param[in] r,g,b ���߂ɂ���F
	@return �e�N�X�`���ԍ���Ԃ�
*/
int CGlSkin::MakeTextureWithAlpha( char *locFile, bool isClamped, unsigned char r, unsigned char g, unsigned char b )
{
	CGlTextureImpl *texturing = CGlTexture::Get();
	texturing->EnableClamp( isClamped );
	m_nTexId = texturing->MakeTextureWithAlpha( locFile, r,g,b );
	if( m_nTexId < 0 )return -1;

	if( m_nTexId >= 0 && m_ArrTex == NULL ){
		m_ArrTex = new CArrayVector3H( m_ArrPos->GetRows() );
	}

	m_bAlphaBlend = true;

	return m_nTexId;
}

/*!
	@brief �X�L���̐F�����߂�
	@param[in] ambi �X�L���̊��F�����߂�
	@param[in] diff �X�L���̊g�U�F�����߂�
	@param[in] spec �X�L���̌���F�����߂�
*/
void CGlSkin::SetColor( double *ambi, double *diff, double *spec )
{
	m_bAlphaBlend = false;

	if( ambi != NULL )
	{	
		if( m_ColAmbient == NULL )m_ColAmbient = new CVector4();
		m_ColAmbient->SetData( ambi );
		if( ambi[3] < 1.0 )m_bAlphaBlend = true;
	}

	if( diff != NULL )
	{	
		if( m_ColDiffuse == NULL )m_ColDiffuse = new CVector4();
		m_ColDiffuse->SetData( diff );
		if( diff[3] < 1.0 )m_bAlphaBlend = true;
	}

	if( spec != NULL )
	{
		if( m_ColSpecular == NULL )m_ColSpecular = new CVector4();
		m_ColSpecular->SetData( spec );
		if( spec[3] < 1.0 )m_bAlphaBlend = true;
	}
}
