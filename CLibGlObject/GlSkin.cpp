#include "StdAfx.h"
#include ".\glskin.h"

///// ///// Skin ///// /////
/*!
	@brief スキンをコピーする
	@param[in] pSkin 元のスキン
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
	@brief 立方体のスキンを作る
	@param[in] length 一辺の長さ
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
	@brief 球のスキンを作る
	@param[in] radius 球の半径
	@param[in] height 高さを何分割にしてプリミティブを作るか
	@param[in] width 幅を何分割にしてプリミティブを作るか
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

	///// 上の頂点
	pPos[0]=0.0;	pPos[1]=radius;	pPos[2]=0.0;	pPos[3]=1.0;
	pNrm[0]=0.0;	pNrm[1]=1.0;	pNrm[2]=0.0;	pNrm[3]=1.0;

	///// 下の頂点
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
	@brief 三角形の集合の任意のスキンを作る
	@param[in] posCnt 頂点数
	@param[in] pPos 頂点配列へのポインタ
	@param[in] indCnt インデックスの数
	@param[in] pInd インデックス配列へのポインタ
	@param[in] texCnt テクスチャ座標の数、posCntと等しくなければならない
	@param[in] pTex テクスチャ座標配列へのポインタ
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
	@brief すべての三角形の法線を自動で計算する
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

		for( i=0; i<indCnt; i++ ) ///// 三角形の数
		{
			CVector3H::Normal( pPos +pInd[0]*4, pPos +pInd[1]*4, pPos +pInd[2]*4, eachFaceNormal);
			CVector3H::Add( pNrm +pInd[0]*4, eachFaceNormal, pNrm +pInd[0]*4 );
			CVector3H::Add( pNrm +pInd[1]*4, eachFaceNormal, pNrm +pInd[1]*4 );
			CVector3H::Add( pNrm +pInd[2]*4, eachFaceNormal, pNrm +pInd[2]*4 );

			pInd += 3;
		}

		for( i=0; i<m_ArrNrm->GetRows(); i++ ) ///// 頂点の数
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
	@brief テクスチャ座標を自身の法線から求める。
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
	@brief 任意の（ユーザ型）スキンをストレージから読み込む
	@param[in] data ストレージ
	@param[in] frame ストレージでのインデックス番号
	@param[out] isNormalSet 法線配列が読み込まれたか
	@param[out] isTexSet テクスチャ座標配列が読み込まれたか
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
	@brief 任意の（ユーザ型）スキンをストレージから読み込む
	@param[in] data ストレージ
	@param[in] nameSpace ネーム・スペース
	@param[out] isNormalSet 法線配列が読み込まれたか
	@param[out] isTexSet テクスチャ座標配列が読み込まれたか
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
	@brief Wavefront(.obj)形式のデータを一つの塊として読み込む
	@param[in] locFile ファイル位置
	@param[out] isNormalSet 法線がセットされたか 
	@param[out] isTexSet テクスチャ座標がセットされたか

bool CGlSkin::LoadSkinMassWaveFront( char *locFile, bool &isNormalSet, bool &isTexSet )
{
	isNormalSet = false;
	isTexSet = false;
	FILE *file = fopen( locFile, "r" );
	if( file == NULL )return false;
	Ch2PlainData dataMtl;

	///// 個数の調査
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
			//char *pStr = line + strlen( type ) + 1;	///// 最初のスペースの次の文字に
			//int lenLine = (int)strlen( pStr );
			//if( pStr[ lenLine -1 ] == '\n' )pStr[ lenLine -1 ] = 0;	///// 改行の除去

			//char locTmp[ 128 ];
			//if( sscanf( line, "%s", locTmp ) < 1 )continue;
			//LoadSkinWaveFrontMtl( locTmp, &dataMtl );
		}
	}

	///// メモリの確保
	if( cntPosMax == 0 || cntIndMax == 0 )return false;
	this->MakePolygon( cntPosMax, NULL, cntIndMax, NULL, cntPosMax, NULL, cntPosMax, NULL );
	double *tablePos=NULL, *tableNrm=NULL, *tableTex=NULL;
	unsigned int *tableInd=NULL;
	if( cntPosMax > 0 )tablePos = new double [ cntPosMax * 3 ];
	if( cntIndMax > 0 )tableInd = new unsigned int[ cntIndMax * 9 ];
	if( cntNrmMax > 0 )tableNrm = new double [ cntNrmMax * 3 ];
	if( cntTexMax > 0 )tableTex = new double [ cntTexMax * 3 ];

	///// 読み込み
	double *tPosCur=tablePos, *tNrmCur=tableNrm, *tTexCur=tableTex;
	unsigned int *tIndCur=tableInd;
	fseek( file, 0, SEEK_SET );
	while( fgets( line, 1024, file ) != NULL )
	{
		type[ 0 ] = 0;
		if( line[ 0 ] == '#' )continue;
		sscanf( line, "%s ", type );
		if( type[ 0 ] == 0 )continue;

		char *pStr = line + strlen( type ) + 1;	///// 最初のスペースの次の文字に
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

	///// 解析
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
	@brief スキンを移動させる
	@param[in] vec 移動方向
*/
void CGlSkin::Shift( CVector3H *vec )
{
	if( m_ArrPos == NULL )return;
	m_ArrPos->ArrayAdd( vec );
}

/*!
	@brief スキンを移動させる
	@param[in] x X移動量
	@param[in] y Y移動量
	@param[in] z Z移動量
*/
void CGlSkin::Shift( double x, double y, double z )
{
	if( m_ArrPos == NULL )return;
	CVector3H vec( x, y, z );
	m_ArrPos->ArrayAdd( &vec );
}

/*!
	@brief スキンを拡大・縮小する
	@param[in] vec 拡大・縮小量
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
	@brief スキンを拡大・縮小させる
	@param[in] x X拡大・縮小量
	@param[in] y Y拡大・縮小量
	@param[in] z Z拡大・縮小量
*/
void CGlSkin::Scale( double x, double y, double z )
{
	if( m_ArrPos == NULL )return;
	double data[16] = { x,0,0,0, 0,y,0,0, 0,0,z,0, 0,0,0,1.0 };
	CMatrix4x4 matScale( data );
	m_ArrPos->ArrayMul( &matScale );
}

/*!
	@brief スキンを回転する
	@param[in] vec 角度
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
	@brief スキンを回転させる
	@param[in] x,y,z 角度
*/
void CGlSkin::Rotate( double x, double y, double z )
{
	if( m_ArrPos == NULL )return;
	CMatrix4x4 matRot;
	matRot.SetRotYXZTran( x, y, z );
	m_ArrPos->ArrayMul( &matRot );
}

/*!
	@brief スキンを行列matで投影する
	@param[in] mat 行列4x4
*/
void CGlSkin::Project( CMatrix4x4 *mat )
{
	if( m_ArrPos == NULL )return;
	m_ArrPos->ArrayMul( mat );
}

/*!
	@brief スキンを描画する
	@param[in] type どのスキン・タイプを描画するか
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
	///// カラー・フィルター
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
	{	/* 2次のテクスチャ */
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
	@brief テクスチャをスキンに貼る。テクスチャはすべてのスキンに共有される
	@param[in] pImg テクスチャ画像
	@return テクスチャ番号を返す
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
	@brief アルファ付きテクスチャをスキンに貼る。テクスチャはすべてのスキンに共有される
	@param[in] pImg アルファ付きテクスチャ画像
	@return テクスチャ番号を返す
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
	@brief テクスチャをスキンに貼る。テクスチャはすべてのスキンに共有される
	@param[in] pImg テクスチャ画像の場所
	@return テクスチャ番号を返す
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
	@brief アルファ付きテクスチャをスキンに貼る。テクスチャはすべてのスキンに共有される
	@param[in] pImg テクスチャ画像の場所
	@param[in] r,g,b 透過にする色
	@return テクスチャ番号を返す
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
	@brief スキンの色を決める
	@param[in] ambi スキンの環境色を決める
	@param[in] diff スキンの拡散色を決める
	@param[in] spec スキンの光沢色を決める
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
