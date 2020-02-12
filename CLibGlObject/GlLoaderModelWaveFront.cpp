#include "StdAfx.h"
#include "GlLoaderModelWaveFront.h"

/*
	@brief WaveFront マテリアル・データをファイルから読み込む
	@param[in] ファイル位置
*/
bool CGlLoaderModelWaveFront::LoadWaveFrontMtl( char *locFile )
{
	char loc[ 128 ];
	if( m_DirectoryNow[ 0 ] == 0 ){
		sprintf( loc, "%s", locFile );
	}else{
		sprintf( loc, "%s%s", m_DirectoryNow, locFile );
	}

	FILE *file = fopen( loc, "r" );
	if( file == NULL )return false;

	int indMtl = -1;
	char line[ 1024 ], type[ 16 ];
	while( fgets( line, 1024, file ) != NULL )
	{
		type[ 0 ] = 0;
		if( line[ 0 ] == '#' || line[ 0 ] == '\n' )continue;
		sscanf( line, "%s ", type );
		if( type[ 0 ] == 0 )continue;

		char *pStr = ShiftNextNoSpace( line );	///// 最初のスペースの次の文字に
		int lenLine = (int)strlen( pStr );
		if( pStr[ lenLine -1 ] == '\n' )pStr[ lenLine -1 ] = 0;	///// 改行の除去

		if( strcmp( type, "newmtl" ) == 0 ){
			indMtl++;
			char nameMtl[ 64 ];
			sscanf( pStr, "%s", nameMtl );
			SetMtl2Index( nameMtl, indMtl );
		}else if( strcmp( type, "Kd" ) == 0 ){
			double valDiff[ 3 ];
			if( sscanf( pStr, "%lf %lf %lf", valDiff, valDiff +1, valDiff +2 ) == 3 ){
				m_DataMaterial.set( indMtl, "colDiff", 3, valDiff );
			}
		}else if( strcmp( type, "Ka" ) == 0 ){
			double valAmbi[ 3 ];
			if( sscanf( pStr, "%lf %lf %lf", valAmbi, valAmbi +1, valAmbi +2 ) == 3 ){
				m_DataMaterial.set( indMtl, "colAmbi", 3, valAmbi );
			}
		}else if( strcmp( type, "Ks" ) == 0 ){
			double valSpec[ 3 ];
			if( sscanf( pStr, "%lf %lf %lf", valSpec, valSpec +1, valSpec +2 ) == 3 ){
				m_DataMaterial.set( indMtl, "colSpec", 3, valSpec );
			}
		}else if( strcmp( type, "map_Kd" ) == 0 ){
			char locTmp[ 128 ];
			if( sscanf( pStr, "%s", locTmp ) > 0 ){
				char tmpLoc1[ 128 ];	///// 注意：カレント・ディレクトリ
				if( m_DirectoryNow[ 0 ] == 0 ){
					sprintf( tmpLoc1, "%s", locTmp );
				}else{
					sprintf( tmpLoc1, "%s%s", m_DirectoryNow, locTmp );
				}
				m_DataMaterial.set( indMtl, "locTex", -1, tmpLoc1 );
			}
		}
	}
	fclose( file );
	return true;
}

bool CGlLoaderModelWaveFront::Load( char *locFile )
{
	///// カレント・ディレクトリの設定
	strcpy( m_DirectoryNow, locFile );
	int lenLoc = (int)strlen( m_DirectoryNow );
	char *pC = m_DirectoryNow +lenLoc -1;
	for( int c=0; c<lenLoc; c++ ){
		if( *pC == '/' || *pC == '\\' ){
			pC[ 1 ] = 0;
			break;
		}
		pC--;
	}

	///// ファイル・読み込み
	FILE *file = fopen( locFile, "r" );
	if( file == NULL )return false;

	///// 個数の調査
	int cntPosWhole=0, cntIndWhole=0, cntNrmWhole=0, cntTexWhole=0;
	int cntGroupWhole = 0;
	char line[ 1024 ], type[ 16 ];
	while( fgets( line, 1024, file ) != NULL )
	{
		type[ 0 ] = 0;
		if( line[ 0 ] == '#' )continue;
		sscanf( line, "%s ", type );
		if( type[ 0 ] == 0 )continue;

		char *pStr = ShiftNextNoSpace( line );
		int lenLine = (int)strlen( pStr );
		if( pStr[ lenLine -1 ] == '\n' )pStr[ lenLine -1 ] = 0;	///// 改行の除去

		if( strcmp( type, "g" ) == 0 ){
			char groupName[ 128 ];
			if( sscanf( pStr, "%s", groupName ) > 0 ){
				cntGroupWhole++;
			}
		}else if( strcmp( type, "v" ) == 0 ){
			cntPosWhole++;
		}else if( strcmp( type, "vn" ) == 0 ){
			cntNrmWhole++;
		}else if( strcmp( type, "vt" ) == 0 ){
			cntTexWhole++;
		}else if( strcmp( type, "f" ) == 0 ){
			int cnt = CountSpace( pStr );
			if( cnt == 2 ){
				cntIndWhole++;
			}else{
				cntIndWhole += 2;
			}
		}else if( strcmp( type, "mtllib" ) == 0 ){
			char locTmp[ 128 ];
			if( sscanf( pStr, "%s", locTmp ) < 1 )continue;
			LoadWaveFrontMtl( locTmp );
		}else if( strcmp( type, "usemtl" ) == 0 ){
			//char nameMtl[ 128 ];
			//if( sscanf( pStr, "%s", nameMtl ) < 0 )continue; 
		}
	}

	///// グループ数から
	if( cntGroupWhole < 1 )return false;
	m_nAmtSkins = cntGroupWhole;
	m_TableHeadVer = new int[ m_nAmtSkins ];
	m_TableHeadInd = new int[ m_nAmtSkins ];
	m_TableCntVer = new int[ m_nAmtSkins ];
	m_TableCntInd = new int[ m_nAmtSkins ];

	///// 「一時」メモリの確保
	if( cntPosWhole == 0 || cntIndWhole == 0 )return false;
	double *tablePos=NULL, *tableNrm=NULL, *tableTex=NULL;
	unsigned int *tableInd=NULL;
	if( cntPosWhole > 0 )tablePos = new double [ cntPosWhole * 4 ];
	if( cntIndWhole > 0 )tableInd = new unsigned int[ cntIndWhole * 9 ];
	if( cntNrmWhole > 0 )tableNrm = new double [ cntNrmWhole * 4 ];
	if( cntTexWhole > 0 )tableTex = new double [ cntTexWhole * 4 ];

	///// 再・読み込み
	cntGroupWhole = -1;
	cntIndWhole = 0;
	double *tPosCur=tablePos, *tNrmCur=tableNrm, *tTexCur=tableTex;
	unsigned int *tIndCur=tableInd;
	fseek( file, 0, SEEK_SET );
	while( fgets( line, 1024, file ) != NULL )
	{
		type[ 0 ] = 0;
		if( line[ 0 ] == '#' )continue;
		sscanf( line, "%s ", type );
		if( type[ 0 ] == 0 )continue;

		char *pStr = ShiftNextNoSpace( line );
		int lenLine = (int)strlen( pStr );
		if( pStr[ lenLine -1 ] == '\n' )pStr[ lenLine -1 ] = 0;	///// 改行の除去

		if( strcmp( type, "v" ) == 0 ){
			if( sscanf( pStr, "%lf %lf %lf", tPosCur, tPosCur +1, tPosCur +2 ) == 3 ){
				tPosCur[ 3 ] = 1.0;
				tPosCur += 4;
			}
		}else if( strcmp( type, "vn" ) == 0 ){
			if( sscanf( pStr, "%lf %lf %lf", tNrmCur, tNrmCur +1, tNrmCur +2 ) == 3 ){
				tNrmCur[ 3 ] = 1.0;
				tNrmCur += 4;
			}
		}else if( strcmp( type, "vt" ) == 0 ){
			if( sscanf( pStr, "%lf %lf %lf", tTexCur, tTexCur +1, tTexCur +2 ) == 3 ){
				tTexCur[ 3 ] = 1.0;
				tTexCur += 4;
			}
		}else if( strcmp( type, "f" ) == 0 ){
			unsigned int tmpInd[ 12 ];

			if( cntPosWhole > 0 && cntNrmWhole > 0 && cntTexWhole > 0 ){
				int cntScan = sscanf( pStr, "%d/%d/%d %d/%d/%d %d/%d/%d %d/%d/%d",	///// pos/tex/nrm
					tmpInd   , tmpInd +1, tmpInd +2,
					tmpInd +3, tmpInd +4, tmpInd +5,
					tmpInd +6, tmpInd +7, tmpInd +8,
					tmpInd +9, tmpInd +10, tmpInd +11 );

				///// 0>1>2
				tIndCur[ 0 ] = tmpInd[ 0 ] -1;	tIndCur[ 1 ] = tmpInd[ 3 ] -1;	tIndCur[ 2 ] = tmpInd[ 6 ] -1;
				tIndCur[ 3 ] = tmpInd[ 1 ] -1;	tIndCur[ 4 ] = tmpInd[ 4 ] -1;	tIndCur[ 5 ] = tmpInd[ 7 ] -1;
				tIndCur[ 6 ] = tmpInd[ 2 ] -1;	tIndCur[ 7 ] = tmpInd[ 5 ] -1;	tIndCur[ 8 ] = tmpInd[ 8 ] -1;
				tIndCur += 9;
				///// 2>3>0
				if( cntScan == 12 ){
					tIndCur[ 0 ] = tmpInd[ 6 ] -1;	tIndCur[ 1 ] = tmpInd[ 9 ] -1;	tIndCur[ 2 ] = tmpInd[ 0 ] -1;
					tIndCur[ 3 ] = tmpInd[ 7 ] -1;	tIndCur[ 4 ] = tmpInd[ 10 ] -1;	tIndCur[ 5 ] = tmpInd[ 1 ] -1;
					tIndCur[ 6 ] = tmpInd[ 8 ] -1;	tIndCur[ 7 ] = tmpInd[ 11 ] -1;	tIndCur[ 8 ] = tmpInd[ 2 ] -1;
					tIndCur += 9;
				}
			}else if( cntNrmWhole > 0 ){
				int cntScan = sscanf( pStr, "%d//%d %d//%d %d//%d %d//%d",	///// pos//nrm
					tmpInd   , tmpInd +2,
					tmpInd +3, tmpInd +5,
					tmpInd +6, tmpInd +8,
					tmpInd +9, tmpInd +11 );

				///// 0>1>2
				tIndCur[ 0 ] = tmpInd[ 0 ] -1;	tIndCur[ 1 ] = tmpInd[ 3 ] -1;	tIndCur[ 2 ] = tmpInd[ 6 ] -1;
				tIndCur[ 3 ] = 0;	tIndCur[ 4 ] = 0;	tIndCur[ 5 ] = 0;
				tIndCur[ 6 ] = tmpInd[ 2 ] -1;	tIndCur[ 7 ] = tmpInd[ 5 ] -1;	tIndCur[ 8 ] = tmpInd[ 8 ] -1;
				tIndCur += 9;
				///// 2>3>0
				if( cntScan == 8 ){
					tIndCur[ 0 ] = tmpInd[ 6 ] -1;	tIndCur[ 1 ] = tmpInd[ 9 ] -1;	tIndCur[ 2 ] = tmpInd[ 0 ] -1;
					tIndCur[ 3 ] = 0;	tIndCur[ 4 ] = 0;	tIndCur[ 5 ] = 0;
					tIndCur[ 6 ] = tmpInd[ 8 ] -1;	tIndCur[ 7 ] = tmpInd[ 11 ] -1;	tIndCur[ 8 ] = tmpInd[ 2 ] -1;
					tIndCur += 9;
				}
			}else if( cntTexWhole > 0 ){
				int cntScan = sscanf( pStr, "%d/%d %d/%d %d/%d %d/%d",	///// pos/tex
					tmpInd   , tmpInd +1,
					tmpInd +3, tmpInd +4,
					tmpInd +6, tmpInd +7,
					tmpInd +9, tmpInd +10 );

				///// 0>1>2
				tIndCur[ 0 ] = tmpInd[ 0 ] -1;	tIndCur[ 1 ] = tmpInd[ 3 ] -1;	tIndCur[ 2 ] = tmpInd[ 6 ] -1;
				tIndCur[ 3 ] = tmpInd[ 1 ] -1;	tIndCur[ 4 ] = tmpInd[ 4 ] -1;	tIndCur[ 5 ] = tmpInd[ 7 ] -1;
				tIndCur[ 6 ] = 0;	tIndCur[ 7 ] = 0;	tIndCur[ 8 ] = 0;
				tIndCur += 9;
				///// 2>3>0
				if( cntScan == 8 ){
					tIndCur[ 0 ] = tmpInd[ 6 ] -1;	tIndCur[ 1 ] = tmpInd[ 9 ] -1;	tIndCur[ 2 ] = tmpInd[ 0 ] -1;
					tIndCur[ 3 ] = tmpInd[ 7 ] -1;	tIndCur[ 4 ] = tmpInd[ 10 ] -1;	tIndCur[ 5 ] = tmpInd[ 1 ] -1;
					tIndCur[ 6 ] = 0;	tIndCur[ 7 ] = 0;	tIndCur[ 8 ] = 0;
					tIndCur += 9;
				}
			}else{
				int cntScan = sscanf( pStr, "%d/%d %d/%d %d/%d %d/%d",	///// pos/tex
					tmpInd   ,
					tmpInd +3,
					tmpInd +6,
					tmpInd +9 );

				///// 0>1>2
				tIndCur[ 0 ] = tmpInd[ 0 ] -1;	tIndCur[ 1 ] = tmpInd[ 3 ] -1;	tIndCur[ 2 ] = tmpInd[ 6 ] -1;
				tIndCur[ 3 ] = 0;	tIndCur[ 4 ] = 0;	tIndCur[ 5 ] = 0;
				tIndCur[ 6 ] = 0;	tIndCur[ 7 ] = 0;	tIndCur[ 8 ] = 0;
				tIndCur += 9;
				///// 2>3>0
				if( cntScan == 4 ){
					tIndCur[ 0 ] = tmpInd[ 6 ] -1;	tIndCur[ 1 ] = tmpInd[ 9 ] -1;	tIndCur[ 2 ] = tmpInd[ 0 ] -1;
					tIndCur[ 3 ] = 0;	tIndCur[ 4 ] = 0;	tIndCur[ 5 ] = 0;
					tIndCur[ 6 ] = 0;	tIndCur[ 7 ] = 0;	tIndCur[ 8 ] = 0;
					tIndCur += 9;
				}
			}
			cntIndWhole++;///// count index を更新

		}else if( strcmp( type, "g" ) == 0 ){
			///// グループ・インデックスの頭を登録
			char groupName[ 128 ];
			if( sscanf( pStr, "%s", groupName ) > 0 ){
				cntGroupWhole++;
				SetGrp2Index( groupName, cntGroupWhole );
				m_TableHeadInd[ cntGroupWhole ] = cntIndWhole;
			}
		}else if( strcmp( type, "usemtl" ) == 0 ){
			///// 使用するマテリアル・インデックスを登録
			char nameMtl[ 128 ];
			int indexMtl;
			if( sscanf( pStr, "%s", nameMtl ) < 0 )continue;
			indexMtl = Mtl2Index( nameMtl );
			if( indexMtl < 0 )continue;
			m_DataSkin.set( cntGroupWhole, "useMtl", 1, &indexMtl );
		}
	}

	///// データの並び替え
	tIndCur = tableInd;
	m_ArrPos = new CArrayVector3H( cntPosWhole );
	m_ArrInd = new CUintArrayVector3( cntIndWhole );
	if( cntNrmWhole > 0 ){
		m_ArrNrm = new CArrayVector3H( cntPosWhole );
	}
	if( cntTexWhole > 0 ){
		m_ArrTex = new CArrayVector3H( cntPosWhole );
	}

	for( int i=0; i<cntIndWhole; i++ )
	{	///// Pos
		m_ArrPos->SetDataRow( tIndCur[0], tablePos +tIndCur[0]*4 );
		m_ArrPos->SetDataRow( tIndCur[1], tablePos +tIndCur[1]*4 );
		m_ArrPos->SetDataRow( tIndCur[2], tablePos +tIndCur[2]*4 );
		///// Ind
		m_ArrInd->SetDataRow( i, tIndCur );

		if( cntTexWhole > 0 )
		{	///// Tex
			m_ArrTex->SetDataRow( tIndCur[0], tableTex +tIndCur[3]*4 );
			m_ArrTex->SetDataRow( tIndCur[1], tableTex +tIndCur[4]*4 );
			m_ArrTex->SetDataRow( tIndCur[2], tableTex +tIndCur[5]*4 );
		}

		if( cntNrmWhole > 0 )
		{	///// Nrm
			m_ArrNrm->SetDataRow( tIndCur[0], tableNrm +tIndCur[6]*4 );
			m_ArrNrm->SetDataRow( tIndCur[1], tableNrm +tIndCur[7]*4 );
			m_ArrNrm->SetDataRow( tIndCur[2], tableNrm +tIndCur[8]*4 );
		}

		tIndCur += 9;
	}

	///// dataPosのそれぞれの個数の解析
	///// インデックスの振りなおしもする
	for( int g=0; g<GetAmtSkins(); g++ )
	{
		int grpIndStart, grpIndCnt;
		if( g == m_nAmtSkins -1 ){
			grpIndStart = GetHeadSkinInd( g );
			m_TableCntInd[ g ] = grpIndCnt = GetAmtSkinsInd() - grpIndStart;
		}else{
			grpIndStart = GetHeadSkinInd( g );
			m_TableCntInd[ g ] = grpIndCnt = GetHeadSkinInd( g +1 ) - grpIndStart;
		}

		unsigned int *pIndData = m_ArrInd->GetData( grpIndStart );
		unsigned int cntMin = pIndData[ 0 ];
		unsigned int cntMax = pIndData[ 0 ];
		for( int i=0; i<grpIndCnt; i++ )
		{
			if( cntMin > pIndData[ 0 ] )cntMin = pIndData[ 0 ];
			if( cntMax < pIndData[ 0 ] )cntMax = pIndData[ 0 ];
			if( cntMin > pIndData[ 1 ] )cntMin = pIndData[ 1 ];
			if( cntMax < pIndData[ 1 ] )cntMax = pIndData[ 1 ];
			if( cntMin > pIndData[ 2 ] )cntMin = pIndData[ 2 ];
			if( cntMax < pIndData[ 2 ] )cntMax = pIndData[ 2 ];
			pIndData += 3;
		}
		int grpPosStart, grpPosCnt;
		m_TableHeadVer[ g ] = grpPosStart = cntMin;
		m_TableCntVer[ g ] = grpPosCnt = cntMax - cntMin +1;
	}

	///// 後始末
	fclose( file );
	if( tablePos != NULL )delete [] tablePos;
	if( tableInd != NULL )delete [] tableInd;
	if( tableNrm != NULL )delete [] tableNrm;
	if( tableTex != NULL )delete [] tableTex;

	return true;
}

/*
	@brief スキンにポリゴンを構築する。ローダでも必要となるので、先にここに作っておく
	@param ind グループ・インデックス
	@param skin スキン
*/
bool CGlLoaderModelWaveFront::MakeDataPolygon( int ind, CGlSkin *skin )
{
	if( skin == NULL )return false;

	///// 頂点データだけは確保
	skin->MakePolygon(
		GetCntSkinPos( ind ), GetDataSkinPos( ind ),
		GetCntSkinInd( ind ), GetDataSkinInd( ind ),
		GetCntSkinNrm( ind ), GetDataSkinNrm( ind ),
		GetCntSkinTex( ind ), GetDataSkinTex( ind )
		);

	///// Reindexing
	if( m_EnableReindexing )
	{
		unsigned int verMin = m_TableHeadVer[ ind ];
		int indCnt = m_TableCntInd[ ind ];
		if( verMin != 0 )
		{
			unsigned int *pData = skin->m_ArrIndex->GetData();
			for( int i=0; i<indCnt; i++ )
			{
				pData[ 0 ] -= verMin;
				pData[ 1 ] -= verMin;
				pData[ 2 ] -= verMin;
				pData += 3;
			}
		}
	}

	if( GetDataSkinNrm( ind ) == NULL )skin->CalcNormal();

	return true;
}
