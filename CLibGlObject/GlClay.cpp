#include "StdAfx.h"
#include ".\glclay.h"

/*!
	@brief 正方体を作る、テクスチャも貼れる
	@param r 一辺の長さ
	@param locTex テクスチャの場所
	@return テクスチャID
*/
int CGlClay::MakeSolidCube( double r, char *locTex )
{
	int tex = -1;
	m_SkinLocal.MakeCube( r );

	if( locTex != NULL )
	{
		ChImgC3 img;
		if( img.load( locTex ) )
		{
			img.convertRGBtoBGR();
			img.convertUPtoDOWN();
			tex = m_SkinLocal.MakeTexture( 0, &img );
			m_SkinLocal.CalcTex_FromSphereNormal();
		}
	}
	m_SkinLocalDeform.Copy( &m_SkinLocal );
	m_SkinWorld.Copy( &m_SkinLocal );
	return tex;
}

/*!
	@brief 球を作る、テクスチャも貼れる
	@param r 半径の長さ
	@param locTex テクスチャの場所
	@return テクスチャID
*/
int CGlClay::MakeSolidSphere( double r, char *locTex )
{
	int tex = -1;
	m_SkinLocal.MakeSphere( r );

	if( locTex != NULL )
	{
		ChImgC3 img;
		if( img.load( locTex ) )
		{
			img.convertRGBtoBGR();
			img.convertUPtoDOWN();
			tex = m_SkinLocal.MakeTexture( 0, &img );
			m_SkinLocal.CalcTex_FromSphereNormal();
		}
	}

	m_SkinLocalDeform.Copy( &m_SkinLocal );
	m_SkinWorld.Copy( &m_SkinLocal );
	return tex;
}

/*!
	@brief クレーの形状をストレージinDataから読み込む
	@param[in] inData ストレージ
*/
bool CGlClay::LoadObject( Ch2PlainData *inData )
{
	char nameShape[ 64 ];
	CVector3H tmpVec;
	double tmpCol0[ 4 ] = { 0, 0, 0, 1 }, tmpCol1[ 4 ], tmpCol2[ 4 ];
	bool isSolidTexSet = false;
	bool isSolidNormalSet = false;

	/* Shape */
	if( inData->get( -1, "claySolidShape", nameShape ) > 1 )
	{
		if( strcmp( nameShape, "Cube" ) == 0 ){
			m_SkinLocal.MakeCube();
		}else if( strcmp( nameShape, "Sphere" ) == 0 ){
			m_SkinLocal.MakeSphere( 1.0, 8, 16 );
		}else if( strcmp( nameShape, "User" ) == 0 ){
			m_SkinLocal.LoadSkinUser( inData, -1, isSolidNormalSet, isSolidTexSet );
			if( !isSolidNormalSet )m_SkinLocal.CalcNormal();
		}else if( strcmp( nameShape, "WaveFront" ) == 0 ){
			//m_SkinLocal.LoadSkinMassWaveFront( inData, -1, isSolidNormalSet, isSolidTexSet );
			//if( !isSolidNormalSet )m_SkinLocal.CalcNormal();
		}
	}
	if( inData->get( -1, "clayAlphaShape", nameShape ) > 1 )
	{
		if( strcmp( nameShape, "Cube" ) == 0 ){
			m_SkinAlpha.MakeCube();
		}else if( strcmp( nameShape, "Sphere" ) == 0 ){
			m_SkinAlpha.MakeSphere();
		}
		m_SkinAlpha.CalcNormal();
	}

	///// texture
	char texLoc[ 128 ];
	if( inData->get( -1, "claySolidTexLoc", texLoc ) > 0
		|| inData->get( -1, "claySolidTex0Loc", texLoc ) > 0 )
	{
		unsigned int nameTex = m_SkinLocal.MakeTexture( 0, texLoc, true );
		m_SkinLocalDeform.SetTexture( 0, nameTex, false );
		if( !isSolidTexSet )m_SkinLocal.CalcTex_FromSphereNormal();
	}
	if( inData->get( -1, "claySolidTex1Loc", texLoc ) > 0 )
	{
		unsigned int nameTex = m_SkinLocal.MakeTexture( 1, texLoc, true );
		m_SkinLocalDeform.SetTexture( 1, nameTex, false );
		m_SkinLocal.CalcTex_FromSphereNormal( 1 );
	}
	if( inData->get( -1, "clayAlphaTexLoc", texLoc ) > 0
		|| inData->get( -1, "clayAlphaTex0Loc", texLoc ) > 0 )
	{
		unsigned int nameTex = m_SkinAlpha.MakeTextureWithAlpha( 0, texLoc, true );
		m_SkinAlphaDeform.SetTexture( 0, nameTex, true );
		m_SkinAlpha.CalcTex_FromSphereNormal();
	}
	if( inData->get( -1, "clayAlphaTex1Loc", texLoc ) > 0 )
	{
		unsigned int nameTex = m_SkinAlpha.MakeTextureWithAlpha( 1, texLoc, true );
		m_SkinAlphaDeform.SetTexture( 1, nameTex, true );
		m_SkinAlpha.CalcTex_FromSphereNormal( 1 );
	}

	/* scale */
	if( inData->get( -1, "claySolidScale", tmpVec.GetData() ) >= 3 ){	SetSolidScale( &tmpVec );	}
	if( inData->get( -1, "clayAlphaScale", tmpVec.GetData() ) >= 3 ){	SetAlphaScale( &tmpVec );	}

	/* pos */
	if( inData->get( -1, "clayPos", tmpVec.GetData() ) >= 3 ){	 SetPos( &tmpVec );	}
	/* angle */
	if( inData->get( -1, "clayAngle", tmpVec.GetData() ) >= 3 ){	SetAngle( &tmpVec );	}

	/* color */
	if( inData->get( -1, "claySolidColorAmbi", tmpCol0 ) == 4
		&& inData->get( -1, "claySolidColorDiff", tmpCol1 ) == 4
		&& inData->get( -1, "claySolidColorSpec", tmpCol2 ) == 4 )
	{
		m_SkinLocal.SetColor( tmpCol0, tmpCol1, tmpCol2 );
	}
	if( inData->get( -1, "clayAlphaColorAmbi", tmpCol0 ) == 4
		&& inData->get( -1, "clayAlphaColorDiff", tmpCol1 ) == 4
		&& inData->get( -1, "clayAlphaColorSpec", tmpCol2 ) == 4 )
	{
		m_SkinAlpha.SetColor( tmpCol0, tmpCol1, tmpCol2 );
	}

	///// スキンの初期化
	m_SkinLocalDeform.Copy( &m_SkinLocal );
	m_SkinWorld.Copy( &m_SkinLocal );
	m_SkinAlphaDeform.Copy( &m_SkinAlpha );

	return true;
}

/*!
	@brief 位置・回転をストレージに保存する
	@param[in] data ストレージ
	@param[in] frame フレーム数
	@param[in] name クレーの名前
*/
bool CGlClay::SavePosture( Ch3PlainData *data, int frame, char *name )
{
	if( data==NULL )return false;
	CalcPosture();

	char nameClay[32]; nameClay[0]=0;
	if( name != NULL )sprintf( nameClay, "%s", name );
	char namePart[32];
	sprintf( namePart, "%s//Pos", nameClay );
	data->Set( frame, namePart, 3, GetPos()->GetData() );
	sprintf( namePart, "%s//Angle", nameClay );
	data->Set( frame, namePart, 3, GetAngle()->GetData() );
	return true;
}

/*!
	@brief 位置・回転をストレージから読み込む
	@param[in] data ストレージ
	@param[in] frame フレーム数
	@param[in] name クレーの名前
*/
bool CGlClay::LoadPosture( Ch3PlainData *data, int frame, char *name )
{
	if(data==NULL)return false;

	///// 位置・回転
	CVector3H vec;
	char nameClay[32]; nameClay[0]=0;
	if( name != NULL )sprintf( nameClay, "%s", name );
	char namePart[ 128 ];
	sprintf( namePart, "%s//Pos", nameClay );
	if( data->Get( frame, namePart, vec.GetData() ) >= 3 ){
		SetPos( &vec );
	}
	sprintf( namePart, "%s//Angle", nameClay );
	if( data->Get( frame, namePart, vec.GetData() ) >= 3 ){
		SetAngle( &vec );
	}

	///// デフォーム
	double matData[ 16 ];
	sprintf( namePart, "%s/Solid/Deform", nameClay );
	if( data->Get( frame, namePart, matData ) == 16 ){
		m_SkinLocalDeform.SetMatDeformed( matData );
	}
	sprintf( namePart, "%s/Alpha/Deform", nameClay );
	if( data->Get( frame, namePart, matData ) == 16 ){
		m_SkinAlphaDeform.SetMatDeformed( matData );
	}

	CalcPosture();
	return true;
}

/*!
	@brief ある点inPoiPosに最も近い、クレーの表面上の点を探す
	@param[in] inPoiPos ある点
	@param[out] outSurPos ある点に最も近い、表面上の点の位置
	@param[out] outSurNrm ある点に最も近い、表面上の点の法線
	@return ある点と表面上の点との距離
*/
double CGlClay::SearchNearestSurface_Point( CVector3H *inPoiPos, CVector3H *outSurPos, CVector3H *outSurNrm )
{
	CalcPosture();
	double nearestDis;
	double *nearestTriangle[3];
	double posCenter[3];

	unsigned int *pInd = m_SkinWorld.m_ArrIndex->GetData();
	double *pPos = m_SkinWorld.m_ArrPos->GetData();

	CVector3H::Center( pPos + 4*pInd[0], pPos + 4*pInd[1], pPos + 4*pInd[2], posCenter );
	nearestDis = CVector3H::Distance( inPoiPos->GetData(), posCenter );
	nearestTriangle[ 0 ] = pPos + 4*pInd[0];
	nearestTriangle[ 1 ] = pPos + 4*pInd[1];
	nearestTriangle[ 2 ] = pPos + 4*pInd[2];

	for(int i=0; i<m_SkinWorld.m_ArrIndex->GetRows(); i++)
	{
		double tmpDis;
		CVector3H::Center( pPos + 4*pInd[0], pPos + 4*pInd[1], pPos + 4*pInd[2], posCenter );
		tmpDis = CVector3H::Distance( inPoiPos->GetData(), posCenter );
		if(tmpDis < nearestDis)
		{
			nearestDis = tmpDis;
			nearestTriangle[0] = pPos + 4*pInd[0];
			nearestTriangle[1] = pPos + 4*pInd[1];
			nearestTriangle[2] = pPos + 4*pInd[2];
		}
		pInd += 3;
	}

	double posSurface[3];
	double nrmSurface[3];
	double plane[4]; // aX + bY + cZ + d = 0; 0:a, 1:b, 2:c, 3:d=1.0or-1.0;
	double tmp;
	CVector3H::Normal( nearestTriangle[0], nearestTriangle[1], nearestTriangle[2], nrmSurface );
	CVector3H::Normalize( nrmSurface );
	CVector3H::Center( nearestTriangle[0], nearestTriangle[1], nearestTriangle[2], posSurface );
	tmp = CVector3H::DotProduct( nrmSurface, posSurface );

	if(tmp == 0.0)
	{
		posSurface[0]=0.0; posSurface[1]=0.0; posSurface[2]=0.0;
		nrmSurface[0]=0.0; nrmSurface[1]=0.0; nrmSurface[2]=0.0;
	}else if(tmp < 0){
		CVector3H::MulConst(-1.0/tmp, nrmSurface, plane);
		plane[3] = 1.0;
	}else{
		CVector3H::MulConst(1.0/tmp, nrmSurface, plane);
		plane[3]=-1.0;
	}

	if( outSurPos != NULL ){		outSurPos->SetData( posSurface );		}
	if( outSurNrm != NULL ){		outSurNrm->SetData( nrmSurface );		}
	tmp = ( CVector3H::DotProduct( plane, inPoiPos->GetData() ) + plane[3] ) / CVector3H::Length( plane );
//	printf("[poi:(%3.3f,%3.3f,%3.3f),sur:(%3.3f,%3.3f,%3.3f),nrm:(%3.3f,%3.3f,%3.3f)]", posPoint[0],posPoint[1],posPoint[2], posSurface[0],posSurface[1],posSurface[2], nrmSurface[0],nrmSurface[1],nrmSurface[2]); }
	return tmp;
}

/*!
	@brief ある点inPoiPosを通る直線に最も近い、クレーの表面上の点を探す
	@param[in] inPoiPos ある点
	@param[in] inVec 直線の方向
	@param[out] outSurPos 直線に最も近い、表面上の点の位置
	@param[out] outAsiPos 直線に最も近くなる、直線上の点
	@return 直線が表面上の点と最も近くなった際の距離
*/
double CGlClay::SearchNearestSurface_Line( CVector3H *inPoiPos, CVector3H *inVec, CVector3H *outSurPos, CVector3H *outAsiPos )
{
	CalcPosture();
	double DisCut = 0.03;	///// 検索打ち切りの閾値
	double *pPos = m_SkinWorld.m_ArrPos->GetData();
	double disOri2Asi;
	double posSur[3], posAsi[3];
	double tmpAsi[3], tmp3[3], tmpDis;
	inVec->Normalize();
	double minDis = CVector3H::Distance( pPos, inPoiPos->GetData() );
	CVector3H::Copy( pPos, posSur );
	CVector3H::Copy( inPoiPos->GetData(), posAsi );
//printf( "\n%f,%f,%f>", vecLine[0], vecLine[1], vecLine[2] );

	for( int i=0; i<m_SkinWorld.m_ArrPos->GetRows(); i++ )
	{
		CVector3H::Sub( pPos, inPoiPos->GetData(), tmp3 );
		disOri2Asi = CVector3H::DotProduct( tmp3, inVec->GetData() );
		CVector3H::MulConst( disOri2Asi, inVec->GetData(), tmp3 );
		CVector3H::Add( inPoiPos->GetData(), tmp3, tmpAsi );
		tmpDis = CVector3H::Distance( pPos, tmpAsi );

		if( tmpDis < minDis )
		{
			//printf( "(%f,%f,%f)", pVer[0], pVer[1], pVer[2] );
			minDis = tmpDis;
			CVector3H::Copy( pPos, posSur );
			CVector3H::Copy( tmpAsi, posAsi );
			if( tmpDis < DisCut ){		return 0.0;		}
		}
		pPos += 4;
	}

	if( outSurPos != NULL ){		outSurPos->SetData( posSur );		}
	if( outAsiPos != NULL ){		outAsiPos->SetData( posAsi );		}
	return minDis;
}

///*!
//	@brief ファイルからイメージを読み込み、透明テクスチャを貼る。Color(r,g,b)が透明になる
//	@param[in] id スキンＩＤ
//	@param[in] loc ファイル位置
//	@param[in] r,g,b 透過色の指定
//*/
//unsigned int CGlClay::SetTextureAlpha_Sphere( char *loc, unsigned char r, unsigned char g, unsigned char b ){
//	ChImgC4 img;
//	if( img.load( loc ) == false )return -1;
//	img.setTransparent( r,g,b );
//	img.convertRGBAtoBGRA();
//	img.convertUPtoDOWN();
//	int tex = m_SkinLocal.MakeTextureWithAlpha( 0, &img );
//	m_SkinLocal.CalcTex_FromSphereNormal();
//	return tex;
//}
