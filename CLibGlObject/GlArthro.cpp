#include "StdAfx.h"
#include ".\glarthro.h"

/*!
	@brief ３つのビーコンから手の付け根位置・方向を求める
	@param[in] wristLeft 手首の左の位置
	@param[in] wristRight 手首の右の位置
	@param[in] arm 手首と肘の真ん中の位置
	@param[in] ctlNrm 腕の垂直方向へのオフセット値。デフォルトで、手の付け根の位置は、wristLeftとwristRightの真ん中となっている
	@param[in] ctlDir 腕の方向へのオフセット値
*/
bool CGlArthro::SetRootPosRot_by3Sensor(CVector3H* wristLeft, CVector3H* wristRight, CVector3H *arm, double ctlNrm, double ctlDir)
{
	CVector3H tmpL, tmpR, nrm, midWrist, tmp_14, dir;

	tmpL.Sub(wristLeft, arm);
	tmpR.Sub(wristRight, arm);
	nrm.CrossProduct(&tmpR, &tmpL);
	nrm.Normalize();//腕の法線を求めた

	tmp_14.Add(wristLeft, wristRight);
	midWrist.MulConst(0.5, &tmp_14);//手首の真ん中
	dir.Sub(&midWrist, arm);
	dir.Normalize();// 腕の方向を求めた

	CVector3H nrmRoot, dirRoot;
	nrmRoot.MulConst(ctlNrm, &nrm);
	dirRoot.MulConst(ctlDir, &dir);

	CVector3H posRoot;
	posRoot.Add(&midWrist, &nrmRoot);//腕の付け根の位置を求めた
	posRoot.Add(&posRoot, &dirRoot);
	SetRootPos(&posRoot); //代入

	// 根本の角度 //
	CVector3H angles;
	angles.CalcAngleFromDirNrm_YXZ( &dir, &nrm );
	SetBoneAngle(0, &angles); //代入
	return true;
}

/*!
	@brief 多関節オブジェの位置と角度をコピーする
	@param[in] tarGet 元の多関節オブジェ
*/
bool CGlArthro::CopyArthroPosAndAngle( CGlArthro *target )
{
	if( target == NULL )return false;
	if( GetBoneMax() != target->GetBoneMax() )return false;

	SetRootPos( target->GetRootPos() );
	SetBonePosLocal( 0, GetBonePosLocal( 0 ) );
	for(int p=0; p<GetBoneMax(); p++)
	{
		m_Bone[ p ].SetAngle( target->GetBoneAngle( p ) );
	}
	CalcPosture();
	return true;
}

/*!
	@brief 多関節オブジェの形状の定義データをストレージから読み込む
	@param[in] data ストレージ
	@param[in] drawSkin スキンの設定を読み込むかどうか
*/
bool CGlArthro::LoadObject( Ch2PlainData *data )
{	///// サンプル
	if(data == NULL)return false;
	if( m_nBoneMax > 0 && m_Bone != NULL ) delete [] m_Bone;
	if( m_nSkinMax > 0 && m_Skin != NULL ) delete [] m_Skin;

	data->get( -1, "BoneMax", &m_nBoneMax );
	data->get( -1, "SkinMax", &m_nSkinMax );
	if( m_nBoneMax < 1){ fprintf(stderr,"ERROR: illegal Bone Max\n"); return false; }
	if( m_nSkinMax < 1){ fprintf(stderr,"ERROR: illegal Skin Max\n"); return false; }

	m_Bone = new CGlBone[ m_nBoneMax ];
	m_Skin = new CGlSkin[ m_nSkinMax ];

	bool enableWholeColor = false;
	double wholeColAmbi[ 4 ] = { 0.1, 0.1, 0.1, 1.0 };
	double wholeColDiff[ 4 ] = { 0.1, 0.1, 0.1, 1.0 };
	double wholeColSpec[ 4 ] = { 0.1, 0.1, 0.1, 1.0 };
	if( data->get( -1, "wholeColorAmbi", wholeColAmbi ) > 3 )enableWholeColor = true;
	if( data->get( -1, "wholeColorDiff", wholeColDiff ) > 3 )enableWholeColor = true;
	if( data->get( -1, "wholeColorSpec", wholeColSpec ) > 3 )enableWholeColor = true;

	IGlLoaderModel *loader = NULL;
	char locModel[ 128 ];
	if( loader == NULL && data->get( -1, "loadWaveFront", locModel ) > 0 ){
		loader = new CGlLoaderModelWaveFront();
		loader->Load( locModel );
	}
	if( loader == NULL && data->get( -1, "loadXFile", locModel ) > 0 ){
		loader = new CGlLoaderModelXFile();
		loader->Load( locModel );
	}
	if( loader == NULL && data->get( -1, "loadMetasequoia", locModel ) > 0 ){
		loader = new CGlLoaderModelMetasequoia();
		loader->Load( locModel );
	}

	for(int b=0; b<m_nBoneMax; b++)
	{
		double posBone[3];
		if( data->get( b, "pos", posBone ) < 1 )return false;
		m_Bone[b].SetPos( posBone );

		double angle[3];
		if( data->get(b, "angleInit", angle) >= 3 ){
			m_Bone[b].SetAngleInit( angle );
			CVector3H posBoneOld( posBone ), posBoneNew;
			CMatrix4x4 matRot; matRot.SetRotYXZTran( angle[0], angle[1], angle[2] );
			posBoneNew.Mul( &matRot, &posBoneOld );
			m_Bone[b].SetPos( &posBoneNew );
		}

		double limit[6];
		if( data->get(b, "limit", limit ) == 6 ){
			m_Bone[b].SetAngleLimit(limit, limit+3);
		}

		int cons[32], conCnt;
		conCnt = data->get(b, "children", cons);
		for( int c=0; c<conCnt; c++ ){
			if( cons[ c ] >= m_nBoneMax )continue;
			m_Bone[b].AddBoneChild( &m_Bone[ cons[ c ] ] );
		}

		int skins[32], skinsCnt;
		skinsCnt = data->get(b, "skins", skins );
		for( int s=0; s<skinsCnt; s++ ){
			if( skins[ s ] >= m_nSkinMax )continue;
			m_Bone[b].AddSkin( &m_Skin[ skins[ s ] ] );
		}
	}

	for(int s=0; s<m_nSkinMax; s++)
	{
		bool isNormalMade = false;
		bool isTexMade = false;
		char nameShape[32];
		nameShape[ 0 ] = 0;	///// 注意：毎回、文字列を消す
		data->get(s, "skinShape", nameShape);
		if(strcmp(nameShape, "Sphere")==0){
			m_Skin[s].MakeSphere( 1.0, 6, 16 );
		}else if(strcmp(nameShape, "Cube")==0){
			m_Skin[s].MakeCube();
		}else if(strcmp(nameShape, "User")==0){
			m_Skin[s].LoadSkinUser( data, s, isNormalMade, isTexMade );
			if( !isNormalMade )m_Skin[s].CalcNormal();
		}else if( strcmp( nameShape, "Loader" ) == 0 ){
			if( loader != NULL ){
				char nameGroup[ 64 ];
				int indexGroup;
				if( data->get( s, "skinLoaderName", nameGroup ) > 0 ){
					loader->MakeSkin( nameGroup, m_Skin +s );
				}else if( data->get( s, "skinLoaderIndex", &indexGroup ) > 0 ){
					loader->MakeSkin( indexGroup, m_Skin +s );
				}
			}
		}else{
			continue;
		}

		///// scale and shift
		double scal[3] = {0,0,0};
		if(data->get( s, "skinScale", scal ) < 3 ){
			//scal[0]=1.0; scal[1]=1.0; scal[2]=1.0;
		}else{
			m_Skin[s].Scale( scal[0], scal[1], scal[2] );
		}

		double skinShift[3] = {0,0.0};
		if( data->get( s, "skinShift", skinShift ) < 3 ){
			//skinShift[0]=0.0;	skinShift[1]=0.0;	skinShift[2]=0.0;
		}else{
			m_Skin[s].Shift( skinShift[0], skinShift[1], skinShift[2] );
		}
		
		///// angle
		double skinAngle[3] = {0,0,0};
		if( data->get( s, "skinAngle", skinShift ) > 2 ){
			m_Skin[s].Rotate( skinShift[0], skinShift[1], skinShift[2] );
		}

		///// texture
		char locTex[ 128 ];
		int isClamped = 0;
		int nameTex;
		if( data->get( s, "skinTexMake", locTex ) > 0
			|| data->get( s, "skinTexClampMake", locTex ) > 0, isClamped++ )
		{
			ChImgC3 img;
			if( img.load( locTex ) )
			{
				img.convertUPtoDOWN();
				img.convertRGBtoBGR();
				m_Skin[s].MakeTexture( &img, isClamped == 1 );
				if( !isTexMade )m_Skin[ s ].CalcTex_FromSphereNormal();
			}
		}else if( data->get( s, "skinTexCopy", &nameTex ) > 0 ){
			m_Skin[ s ].SetTexture( m_Skin[ nameTex ].GetTexture() );
			if( !isTexMade )m_Skin[ s ].CalcTex_FromSphereNormal();
		}

		///// color
		double colAmbi[ 4 ], colDiff[ 4 ], colSpec[ 4 ];
		if( data->get( s, "skinColAmbi", colAmbi ) > 3
			&& data->get( s, "skinColDiff", colDiff ) > 3
			&& data->get( s, "skinColSpec", colSpec ) > 3 )
		{
			m_Skin[ s ].SetColor( colAmbi, colDiff, colSpec );
		}else if( enableWholeColor ){
			m_Skin[ s ].SetColor( wholeColAmbi, wholeColDiff, wholeColSpec );
		}

		///// enable option
		char yesorno[ 64 ];
		yesorno[ 0 ] = 0;
		if( data->get( s, "skinEnableAutoNormal", yesorno ) > 0 ){
			if( strcmp( yesorno, "yes" ) == 0
				|| strcmp( yesorno, "YES" ) == 0
				|| strcmp( yesorno, "Yes" ) == 0 ){
				m_Skin[ s ].CalcNormal();
			}
		}
		yesorno[ 0 ] = 0;
		if( data->get( s, "skinDisableColor", yesorno ) > 0 ){
			if( strcmp( yesorno, "yes" ) == 0
				|| strcmp( yesorno, "YES" ) == 0
				|| strcmp( yesorno, "Yes" ) == 0 ){
					m_Skin[ s ].UnsetColor();
			}
		}
	}

	/////拡張テクスチャ
	int texExpNum = 0;
	if( data->get( -1, "texExpNum", &texExpNum ) > 0 )
	{
		char nameTexExp[ 64 ], locTex[ 64 ];
		for( int n=0; n<texExpNum; n++ ){
			sprintf( nameTexExp, "texExp%02d", n );
			if( data->get( -1, nameTexExp, locTex ) > 0 ){
				CGlTexture::Get()->MakeTexture( locTex );
			}
		}
	}

	m_RootBone = m_Bone;
	if( loader != NULL ){
		loader->Destroy();
		delete loader;
	}
	return true;
}

/*!
	@brief 多関節オブジェの形状の定義データをストレージに読み込む
	@param[in] data ストレージ
*/
bool CGlArthro::LoadObjectBones( Ch3PlainData *data )
{
	if( data == NULL ) return false;
	if( m_nBoneMax != 0 && m_Bone != NULL ) delete [] m_Bone;

	data->SetNameSpace( "Bone" );
	data->Get( -1, "BoneMax", &m_nBoneMax );
	if( m_nBoneMax < 1){ fprintf(stderr,"ERROR: illegal Bone Max\n"); return false; }
	m_Bone = new CGlBone[ m_nBoneMax ];

	m_RootBone = m_Bone;

	/////骨の設定
	for(int b=0; b<m_nBoneMax; b++)
	{
		double posBone[ 3 ];
		if( data->Get( b, "bonePos", posBone ) < 1 )return false;
		m_Bone[ b ].SetPos( posBone );

		double angle[ 3 ];
		if( data->Get(b, "boneAng", angle) >= 3 ){
			m_Bone[ b ].SetAngleInit( angle );
		}

		double limit[ 6 ];
		if( data->Get(b, "boneAngLimit", limit ) == 6 ){
			m_Bone[ b ].SetAngleLimit(limit, limit+3);
		}

		int cons[ 32 ], conCnt;
		conCnt = data->Get(b, "children", cons);
		for( int c=0; c<conCnt; c++ ){
			if( cons[ c ] >= m_nBoneMax )continue;
			m_Bone[ b ].AddBoneChild( &m_Bone[ cons[ c ] ] );
		}
	}

	return true;
}

/*!
	@brief 多関節オブジェの形状のスキンデータをストレージに読み込む
	@param[in] data ストレージ
*/
bool CGlArthro::LoadObjectSkins( Ch3PlainData *data )
{
	if( data == NULL ) return false;
	if( m_nBoneMax < 1 ) return false;
	if( m_nSkinMax != 0 && m_Skin != NULL ) delete [] m_Skin;

	///// リセット
	m_PosRoot.SetData( 0,0,0 );

	data->SetNameSpace( "Skin" );
	data->Get( -1, "SkinMax", &m_nSkinMax );
	if( m_nSkinMax < 1){ fprintf(stderr,"ERROR: illegal Skin Max\n"); return false; }
	m_Skin = new CGlSkin[ m_nSkinMax ];

	/////皮の貼り付け
	data->SetNameSpace( "Bone" );
	for(int b=0; b<m_nBoneMax; b++)
	{
		m_Bone[ b ].ClearSkin();/////注意

		int skins[ 32 ], skinsCnt;
		skinsCnt = data->Get(b, "skins", skins );
		for( int s=0; s<skinsCnt; s++ ){
			if( skins[ s ] >= m_nSkinMax )continue;
			m_Bone[ b ].AddSkin( &m_Skin[ skins[ s ] ] );
		}
	}

	/////骨の位置の計算
	CalcPosture();

	/////皮の設定
	data->SetNameSpace( "Skin" );
	double wholeScale[ 3 ] = { 1,1,1 };
	data->Get( -1, "wholeScale", wholeScale );

	IGlLoaderModel *loader = NULL;
	bool trueOrFalse;
	char locModel[ 128 ];
	if( loader == NULL && data->Get( -1, "loadWaveFront", locModel ) > 0 ){
		loader = new CGlLoaderModelWaveFront();
		loader->Load( locModel );
	}
	if( loader == NULL && data->Get( -1, "loadXFile", locModel ) > 0 ){
		loader = new CGlLoaderModelXFile();
		loader->Load( locModel );
	}
	if( loader == NULL && data->Get( -1, "loadMetasequoia", locModel ) > 0 ){
		CGlLoaderModelMetasequoia* loaderMQ = new CGlLoaderModelMetasequoia();
		loader = loaderMQ;
		if( data->Get( -1, "loadTextureUnified", &trueOrFalse ) > 0 ){
			loaderMQ->InitSetTextureUnified( trueOrFalse );
		}
		loaderMQ->Load( locModel );
	}

	for(int s=0; s<m_nSkinMax; s++)
	{
		bool isNormalMade = false;
		bool isTexMade = false;
		char nameShape[ 32 ];
		nameShape[ 0 ] = 0;	/////注意：毎回、文字列を消す
		data->Get(s, "skinShape", nameShape);
		if(strcmp(nameShape, "Sphere")==0){
			m_Skin[ s ].MakeSphere( 1.0, 6, 16 );
		}else if(strcmp(nameShape, "Cube")==0){
			m_Skin[ s ].MakeCube();
		}else if(strcmp(nameShape, "User")==0){
			char tmpNameSpace[ 64 ];	sprintf( tmpNameSpace, "usr%02d", s );
			m_Skin[ s ].LoadSkinStorage( data, tmpNameSpace, isNormalMade, isTexMade );
			m_Skin[ s ].CalcNormal();
		}else if( strcmp( nameShape, "Loader" ) == 0 ){
			if( loader != NULL )
			{
				char nameGroup[ 64 ];
				int indexGroup;
				if( data->Get( s, "skinLoaderName", nameGroup ) > 0 ){
					loader->MakeSkin( nameGroup, m_Skin +s );
				}else if( data->Get( s, "skinLoaderIndex", &indexGroup ) > 0 ){
					loader->MakeSkin( indexGroup, m_Skin +s );
				}
				isTexMade = true;/////テクスチャ座標は生成される

				/////全体の縮尺はコチラで
				m_Skin[ s ].Scale( wholeScale[0], wholeScale[1], wholeScale[2] );
				
				/////皮の位置の調整
				CMatrix4x4 matInvRot;	matInvRot.Invert( &m_Skin[ s ].m_BoneParent->m_ProGlobal );
				m_Skin[ s ].Project( &matInvRot );
			}
		}else{
			continue;
		}

		/////Scale and Shift and Rotate and Color
		double scal[ 3 ] = { 1,1,1 };
		if(data->Get( s, "skinScale", scal ) > 2 ){
			m_Skin[ s ].Scale( scal[0], scal[1], scal[2] );
		}
		double skinShift[ 3 ] = { 0,0.0 };
		if( data->Get( s, "skinShift", skinShift ) > 2 ){
			m_Skin[ s ].Shift( skinShift[0], skinShift[1], skinShift[2] );
		}
		double skinAngle[3] = { 0,0,0 };
		if( data->Get( s, "skinRotate", skinShift ) > 2 ){
			m_Skin[s].Rotate( skinShift[0], skinShift[1], skinShift[2] );
		}
		double skinColor[4] = { 1,1,1,1 };
		if( data->Get( s, "skinColor", skinColor ) > 2 ){
			m_Skin[s].SetColorFilter( skinColor );
		}

		/////Texture
		char locTex[ 128 ];
		int isClamped = 0;
		int nameTex;
		if( data->Get( s, "skinTexMake", locTex ) > 0
			|| data->Get( s, "skinTexClampMake", locTex ) > 0, isClamped++ )
		{
			ChImgC3 img;
			if( img.load( locTex ) )
			{
				img.convertUPtoDOWN();
				img.convertRGBtoBGR();
				m_Skin[s].MakeTexture( &img, isClamped == 1 );
				if( !isTexMade )m_Skin[ s ].CalcTex_FromSphereNormal();
			}
		}else if( data->Get( s, "skinTexCopy", &nameTex ) > 0 ){
			m_Skin[ s ].SetTexture( m_Skin[ nameTex ].GetTexture() );
			if( !isTexMade )m_Skin[ s ].CalcTex_FromSphereNormal();
		}

		/////Enable option
		bool enableAuto;
		if( data->Get( s, "skinEnableAutoNormal", &enableAuto ) > 0 ){
			if( enableAuto ) m_Skin[ s ].CalcNormal();
		}
		if( data->Get( s, "skinEnableAutoColor", &enableAuto ) > 0 ){
			if( !enableAuto ) m_Skin[ s ].UnsetColor();
		}
	}

	/////拡張テクスチャ
	data->SetNameSpace( "TextureExt" );
	int texExpMax = 0;
	if( data->Get( -1, "texExpMax", &texExpMax ) > 0 )
	{
		char locTex[ 128 ];
		for( int t=0; t<texExpMax; t++ ){
			if( data->Get( t, "locTex", locTex ) > 0 ){
				CGlTexture::Get()->MakeTexture( locTex );
			}
		}
	}

	if( loader != NULL ){
		loader->Destroy();
		delete loader;
	}

	return true;
}

/*!
	@brief 多関節オブジェの形状の定義データをストレージに読み込む
	@param[in] data ストレージ
*/
bool CGlArthro::SaveObject( Ch3PlainData *data )
{
	if( data == NULL ) return false;

	data->SetNameSpace( "Bone" );
	data->Set( -1, "BoneMax", 1, &m_nBoneMax );
	data->SetNameSpace( "Skin" );
	data->Set( -1, "SkinMax", 1, &m_nSkinMax );

	/////骨の保存
	data->SetNameSpace( "Bone" );
	for(int b=0; b<m_nBoneMax; b++)
	{
		data->Set( b, "bonePos", 3, m_Bone[ b ].GetPosLocal()->GetData() );
		data->Set( b, "boneAng", 3, m_Bone[ b ].GetAngleInit()->GetData() );

		double limit[ 6 ] = { -180,-180,-180, 180,180,180 };
		m_Bone[ b ].GetAngleLimit( limit, limit +3 );
		data->Set(b, "boneAngLimit", 6, limit );
	}

	/////皮の保存
	//for(int s=0; s<m_nSkinMax; s++)
	//{
	//}
	return true;
}

/*!
	@brief 多関節オブジェの位置・角度をストレージから読み込む
	@param[in] data ストレージ
	@param[in] frame フレーム数
	@param[in] name 名前を入れる
*/
bool CGlArthro::LoadPosture( Ch3PlainData *data, int frame, char *name )
{
	if( data == NULL )return false;

	data->SetNameSpace( "Frame" );
	///// 位置の変更
	char nameArthro[ 32 ]; nameArthro[ 0 ] = 0;
	if( name != NULL )sprintf( nameArthro, "%s", name );
	char namePart[ 32 ];
	double vec[ 4 ];

	sprintf( namePart, "%s/Bn00/Pos", nameArthro );
	if( data->Get( frame, namePart, vec ) >= 3 ){
		SetBonePosLocal( 0, vec );
	}
	sprintf( namePart, "%s/Bn00/AddPos", nameArthro );
	if( data->Get( frame, namePart, vec ) >= 3 ){
		AddBonePosLocal( 0, vec );
	}

	for( int b=0; b<GetBoneMax(); b++ )
	{
		sprintf( namePart, "%s/Bn%02d/Angle", nameArthro, b );
		if( data->Get( frame, namePart, vec ) >= 3 ){
			SetBoneAngle( b, vec );
		}
		sprintf( namePart, "%s/Bn%02d/AddAngle", nameArthro, b );
		if( data->Get( frame, namePart, vec ) >= 3 ){
			AddBoneAngle( b, vec );
		}
	}
	//CalcPosture();

	///// 色の変更
	double color[ 4 ] = { 1,1,1,1 };
	sprintf( namePart, "%s/SkAll/Col", nameArthro );
	if( data->Get( frame, namePart, color ) > 2 ){
		SetAllSkinColor( color );
	}

	///// テクスチャの変更
	CGlTextureImpl *texture = CGlTexture::Get();
	for( int s=0; s<GetSkinMax(); s++ )
	{
		int texFrom = m_Skin[ s ].GetTexture();
		int subTex[ 7 ];///// texTo, fromx, fromy, tox, toy, width, height の順番とする
		sprintf( namePart, "%s/Sk%02d/SubTex", nameArthro, s );
		if( data->Get( frame, namePart, subTex ) > 6 && texFrom >= 0 ){
			texture->OverDrawSubImage( texFrom, subTex[0],subTex[1],subTex[2],subTex[3],subTex[4],subTex[5],subTex[6] );
		}

		char tmpLoc[ 64 ];
		sprintf( namePart, "%s/Sk%02d/ChgTex", nameArthro, s );
		if( data->Get( frame, namePart, tmpLoc ) > 0 ){
			texFrom = texture->GetIndexTex( tmpLoc );
			if( texFrom >= 0 )m_Skin[ s ].SetTexture( texFrom );
		}
	}
	return true;
}

/*!
	@brief 多関節オブジェの位置・角度をストレージへ書き出す
	@param[in] data ストレージ
	@param[in] frame フレーム数
	@param[in] name 名前、NULLでもよい
*/
bool CGlArthro::SavePosture( Ch3PlainData *data, int frame, char *name )
{
	if(data == NULL) return false;

	CalcPosture();
	char nameArthro[32]; nameArthro[ 0 ] = 0;
	if( name != NULL )sprintf( nameArthro, "%s", name );
	char namePart[32];

	data->SetNameSpace( "Frame" );
	for(int b=0; b<GetBoneMax(); b++)
	{
		sprintf( namePart, "%s/Bn%02d/Angle", nameArthro, b );
		data->Set( frame, namePart, 3, GetBoneAngle(b)->GetData() );
		sprintf( namePart, "%s/Bn%02d/Pos", nameArthro, b );
		data->Set( frame, namePart, 3, GetBonePosEdge(b)->GetData() );
	}
	return true;
}

void CGlArthro::TestPrintAllSkinMinMax()
{
	for(int s=0; s<GetSkinMax(); s++ )
	{
		double xMin, yMin, zMin, xMax, yMax, zMax;
		double *posData = m_Skin[ s ].m_ArrPos->GetData();
		int posCnt = m_Skin[ s ].m_ArrPos->GetRows();

		xMin = posData[ 0 ]; xMax = posData[ 0 ];
		yMin = posData[ 1 ]; yMax = posData[ 1 ];
		zMin = posData[ 2 ]; zMax = posData[ 2 ];
		for( int c=0; c<posCnt; c++ )
		{
			if( xMin > posData[ 0 ] )xMin = posData[ 0 ];
			if( xMax < posData[ 0 ] )xMax = posData[ 0 ];
			if( yMin > posData[ 1 ] )yMin = posData[ 1 ];
			if( yMax < posData[ 1 ] )yMax = posData[ 1 ];
			if( zMin > posData[ 2 ] )zMin = posData[ 2 ];
			if( zMax < posData[ 2 ] )zMax = posData[ 2 ];
			posData += 4;
		}

		printf( "Skin[ %d ]\n\tPosMin(x,y,z)=(%3.3f,%3.3f,%3.3f), PosMax(x,y,z)=(%3.3f,%3.3f,%3.3f)\n",
			s, xMin,yMin,zMin, xMax,yMax,zMax );
	}
}
