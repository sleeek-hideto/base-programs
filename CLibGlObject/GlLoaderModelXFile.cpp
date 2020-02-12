#include "StdAfx.h"
#include "GlLoaderModelXFile.h"

bool CGlLoaderModelXFile::RetrieveMain( FILE *file , int &floorNow )
{
	int floorStart = floorNow;
	char nameClass[ 64 ], namePrivate[ 64 ];
	while( ScanFile( file, floorNow, nameClass, namePrivate ) != -1 )
	{
		if( strcmp( nameClass, "template" ) == 0 ){
			///// Some Process
		}else if( strcmp( nameClass, "Mesh" ) == 0 ){
			//if( namePrivate[ 0 ] == 0 )strcpy( namePrivate, "Default" );
			RetrieveMesh( file, floorNow );
		}
	}
	return true;
}

bool CGlLoaderModelXFile::RetrieveMesh( FILE *file , int &floorNow )
{
	int floorStart = floorNow;
	char line[ 1024 ];
	int cntPos;

	///// Position
	while( 1 ){
		if( fgets( line, 1024, file ) == NULL )return false;
		if( sscanf( line, "%d;", &cntPos ) == 1 ){
			m_ArrPos = new CArrayVector3H( cntPos );
			break;
		}
	}
	double *pPos = m_ArrPos->GetData();
	for( int c=0; c<cntPos; c++ ){
		if( fgets( line, 1024, file ) == NULL )return false;		
		sscanf( line, "%lf;%lf;%lf", pPos, pPos +1, pPos +2 );
		pPos[ 3 ] = 1.0;
		pPos += 4;
	}

	///// Index
	int amtInd = 0, amtFace = 0;

	while( 1 ){
		if( fgets( line, 1024, file ) == NULL )return false;
		if( sscanf( line, "%d;", &amtFace ) == 1 ){
			m_nAmtMixFace = amtFace;
			break;
		}
	}

	fpos_t posFile;
	fgetpos( file, &posFile );
	//long pSeek = ftell( file );	///// 故障している
	for( int c=0; c<amtFace; c++ ){
		if( fgets( line, 1024, file ) == NULL )return false;
		int cntTmp;
		if( sscanf( line, "%d;", &cntTmp ) == 1 ){
			if( cntTmp == 3 ){
				amtInd++;
			}else if( cntTmp == 4 ){
				amtInd += 2;
			}
		}
	}

	fsetpos( file, &posFile );
	//fseek( file, pSeek, SEEK_SET );	///// 故障している
	m_ArrInd = new CUintArrayVector3( amtInd );
	m_ArrMixFace = new int[ m_nAmtMixFace * 4 ];	///// Data Native
	m_TableMixFace2Ind = new int[ m_nAmtMixFace ];	///// face->ind変換テーブル
	unsigned int *pInd = m_ArrInd->GetData();
	int *pFace = m_ArrMixFace;
	for( int c=0, cntInd=0; c<amtFace; c++ ){
		if( fgets( line, 1024, file ) == NULL )return false;
		unsigned int face;
		sscanf( line, "%d;%d,%d,%d,%d", &face, pFace, pFace +1, pFace +2, pFace +3 );

		m_TableMixFace2Ind[ c ] = cntInd;
		if( face == 4 ){
			pInd[ 0 ] = pFace[ 0 ];	pInd[ 1 ] = pFace[ 1 ];	pInd[ 2 ] = pFace[ 2 ];
			pInd += 3;
			pInd[ 0 ] = pFace[ 2 ];	pInd[ 1 ] = pFace[ 3 ];	pInd[ 2 ] = pFace[ 0 ];
			pInd += 3;
			cntInd += 2;
		}else{
			pInd[ 0 ] = pFace[ 0 ];	pInd[ 1 ] = pFace[ 1 ];	pInd[ 2 ] = pFace[ 2 ];
			pFace[ 3 ] = -1;	///// 注意！三角形か四角形を区別するため
			pInd += 3;
			cntInd++;
		}
		pFace += 4;
	}
	//m_ArrInd->TestPrint( "Index" );

	///// その他
	char nameClass[ 64 ], namePrivate[ 64 ];
	while( ScanFile( file, floorNow, nameClass, namePrivate ) )
	{
		if( floorNow < floorStart )break;

		if( strcmp( nameClass, "MeshMaterialList" ) == 0 ){
			RetrieveMaterialList( file, floorNow );
		}else if( strcmp( nameClass, "MeshNormals" ) == 0 ){
			RetrieveNormal( file, floorNow );
		}else if( strcmp( nameClass, "MeshTextureCoords" ) == 0 ){
			RetrieveCoordsTex( file, floorNow, false );
		}
	}

	return true;
}

bool CGlLoaderModelXFile::MakeGroupByMaterial()
{
	///// Grouping
	for( int g=0; g<m_nAmtSkins; g++ )
	{
		bool isNoSkin = true;
		int faceMin, faceMax;
		for( int a=0; a<m_nAmtMixFace; a++ ){
			if( m_ArrMaterial[ a ] == g ){
				faceMin = a;
				isNoSkin = false;
				break;
			}			
		}
		for( int a=m_nAmtMixFace-1; a>0; a-- ){
			if( m_ArrMaterial[ a ] == g ){
				faceMax = a;
				isNoSkin = false;
				break;
			}
		}
		if( isNoSkin ){
			m_TableHeadInd[ g ] = 0;
			m_TableCntInd[ g ] = 0;
			m_TableHeadVer[ g ] = 0;
			m_TableCntVer[ g ] = 0;
			continue;
		}

		int grpIndHead, grpIndMax, grpIndCnt;
		grpIndHead = m_TableMixFace2Ind[ faceMin ];
		if( faceMax == m_nAmtMixFace -1 ){
			grpIndMax = GetAmtSkinsInd() -1;
		}else{
			grpIndMax = m_TableMixFace2Ind[ faceMax +1 ] -1;
		}
		m_TableHeadInd[ g ] = grpIndHead;
		m_TableCntInd[ g ] = grpIndCnt = grpIndMax - grpIndHead +1;

		unsigned int *pIndData = m_ArrInd->GetData( grpIndHead );
		unsigned int verMin = pIndData[ 0 ];
		unsigned int verMax = pIndData[ 0 ];
		for( int i=0; i<grpIndCnt; i++ )
		{
			if( verMin > pIndData[ 0 ] )verMin = pIndData[ 0 ];
			if( verMax < pIndData[ 0 ] )verMax = pIndData[ 0 ];
			if( verMin > pIndData[ 1 ] )verMin = pIndData[ 1 ];
			if( verMax < pIndData[ 1 ] )verMax = pIndData[ 1 ];
			if( verMin > pIndData[ 2 ] )verMin = pIndData[ 2 ];
			if( verMax < pIndData[ 2 ] )verMax = pIndData[ 2 ];
			pIndData += 3;
		}
		m_TableHeadVer[ g ] = verMin;
		m_TableCntVer[ g ] = verMax - verMin +1;

		///// skin index bind ! material index
		m_DataSkin.set( g, "useMtl", 1, &g );
	}

	return true;
}

bool CGlLoaderModelXFile::RetrieveMaterial( FILE *file, int &floorNow, int indMtl )
{
	int floorStart = floorNow;
	char line[ 1024 ];
	double elem[ 4 ] = { 0,0,0,1 };

	if( fgets( line, 1024, file ) == NULL )return false;
	sscanf( line, "%lf;%lf;%lf;%lf", elem, elem +1, elem +2, elem +3 );
	m_DataMaterial.set( indMtl, "colDiff", 4, elem );

	if( fgets( line, 1024, file ) == NULL )return false;
	sscanf( line, "%lf", elem );
	m_DataMaterial.set( indMtl, "numSpec", 1, elem );

	elem[ 3 ] = 1.0;
	if( fgets( line, 1024, file ) == NULL )return false;
	sscanf( line, "%lf;%lf;%lf", elem, elem +1, elem +2 );
	m_DataMaterial.set( indMtl, "colSpec", 4, elem );

	if( fgets( line, 1024, file ) == NULL )return false;
	sscanf( line, "%lf;%lf;%lf", elem, elem +1, elem +2 );
	m_DataMaterial.set( indMtl, "colAmbi", 4, elem );

	char nameClass[ 64 ], namePrivate[ 64 ];
	while( ScanFile( file, floorNow, nameClass, namePrivate ) )
	{
		if( floorNow < floorStart )break;
		if( strcmp( nameClass, "TextureFilename" ) == 0 ){
			fgets( line, 1024, file );
			char *tmpLoc0 = PickupString( line );

			///// 注意
			char tmpLoc1[ 128 ];
			if( m_DirectoryNow[ 0 ] == 0 ){
				sprintf( tmpLoc1, "%s", tmpLoc0 );
			}else{
				sprintf( tmpLoc1, "%s%s", m_DirectoryNow, tmpLoc0 );
			}
			m_DataMaterial.set( indMtl, "locTex", -1, tmpLoc1 );
		}
	}

	return true;
}

bool CGlLoaderModelXFile::RetrieveMaterialList( FILE *file , int &floorNow )
{
	int floorStart = floorNow;
	char line[ 1024 ];
	if( GetAmtSkinsInd() < 1 )return false;

	///// load material group
	int amtFace = 0;
	while( 1 ){
		int state = ScanFile( file, floorNow, m_nAmtSkins );

		if( state == 1 ){
			m_TableHeadVer = new int[ m_nAmtSkins ];
			m_TableHeadInd = new int[ m_nAmtSkins ];
			m_TableCntVer = new int[ m_nAmtSkins ];
			m_TableCntInd = new int[ m_nAmtSkins ];
			break;
		}else if( state == -1 )return false;
	}
	while( 1 ){
		if( ScanFile( file, floorNow, amtFace ) != 0 )break;
	}
	if( amtFace < 1 )return false;

	m_ArrMaterial = new int[ amtFace ];
	int *pList = m_ArrMaterial;
	for( int a=0; a<amtFace; a++ )
	{
		if( fgets( line, 1024, file ) == NULL )return false;
		sscanf( line, "%d", pList );
		pList++;
	}

	///// Material
	int indMtl = 0;
	char nameClass[ 64 ], namePrivate[ 64 ];
	while( ScanFile( file, floorNow, nameClass, namePrivate ) )
	{
		if( floorNow < floorStart )break;

		if( strcmp( nameClass, "Material" ) == 0 ){
			RetrieveMaterial( file, floorNow, indMtl );
			indMtl++;
		}
	}

	return true;
}

bool CGlLoaderModelXFile::RetrieveNormal( FILE *file , int &floorNow )
{
	int floorStart = floorNow;
	char line[ 1024 ];
	int cntVec;
	double *dataVec;

	if( GetAmtSkinsVer() < 1 )return false;
	if( GetAmtSkinsInd() < 1 )return false;

	///// Normal Vector
	while( 1 ){
		if( fgets( line, 1024, file ) == NULL )return false;
		if( sscanf( line, "%d;", &cntVec ) == 1 ){
			dataVec = new double [ cntVec * 4 ];
			break;
		}
	}
	double *pVec = dataVec;
	for( int c=0; c<cntVec; c++ ){
		if( fgets( line, 1024, file ) == NULL )return false;
		sscanf( line, "%lf;%lf;%lf", pVec, pVec +1, pVec +2 );
		pVec[ 3 ] = 1.0;
		pVec += 4;
	}

	///// Normal Index
	m_ArrNrm = new CArrayVector3H( GetAmtSkinsVer() );
	m_ArrNrm->SetDataAllZero();	///// 0 で初期化する
	int amtFace;

	while( 1 ){
		if( fgets( line, 1024, file ) == NULL )return false;
		if( sscanf( line, "%d;", &amtFace ) == 1 )break;
	}

	unsigned int *pInd = m_ArrInd->GetData();
	for( int c=0; c<amtFace; c++ ){
		if( fgets( line, 1024, file ) == NULL )return false;
		unsigned int face, tmpInd[ 4 ];
		sscanf( line, "%d;%d,%d,%d,%d", &face, tmpInd, tmpInd +1, tmpInd +2, tmpInd +3 );

		if( face == 4 ){
			m_ArrNrm->AddDataRow( pInd[ 0 ], dataVec +tmpInd[ 0 ] );
			m_ArrNrm->AddDataRow( pInd[ 1 ], dataVec +tmpInd[ 1 ] );
			m_ArrNrm->AddDataRow( pInd[ 2 ], dataVec +tmpInd[ 2 ] );
			pInd += 3;
			m_ArrNrm->AddDataRow( pInd[ 0 ], dataVec +tmpInd[ 2 ] );
			m_ArrNrm->AddDataRow( pInd[ 1 ], dataVec +tmpInd[ 3 ] );
			m_ArrNrm->AddDataRow( pInd[ 2 ], dataVec +tmpInd[ 0 ] );
			pInd += 3;
		}else{
			m_ArrNrm->AddDataRow( pInd[ 0 ], dataVec +tmpInd[ 0 ] );
			m_ArrNrm->AddDataRow( pInd[ 1 ], dataVec +tmpInd[ 1 ] );
			m_ArrNrm->AddDataRow( pInd[ 2 ], dataVec +tmpInd[ 2 ] );
			pInd += 3;
		}
	}
	m_ArrNrm->ArraySqrt();
	//m_ArrNrm->TestPrint( "Normal Calc" );

	delete [] dataVec;
	return true;
}

bool CGlLoaderModelXFile::RetrieveCoordsTex( FILE *file , int &floorNow, bool coordsIsReverse )
{
///// MeshTextureCoords {
/////  4238;
/////  0.000000;0.000000;
/////  ...
///// }

	int floorStart = floorNow;
	char line[ 1024 ];
	int cntTex;

	if( GetAmtSkinsVer() < 1 )return false;

	///// Coordinate Texture
	m_ArrTex = new CArrayVector3H( GetAmtSkinsVer() );
	m_ArrTex->SetDataAllZero();

	while( 1 ){
		if( fgets( line, 1024, file ) == NULL )return false;
		if( sscanf( line, "%d;", &cntTex ) == 1 )break;
	}

	double *pTex = m_ArrTex->GetData();
	for( int c=0; c<cntTex; c++ ){
		if( fgets( line, 1024, file ) == NULL )return false;
		sscanf( line, "%lf;%lf", pTex, pTex +1 );
		if( coordsIsReverse ){
			pTex[ 1 ] = 1.0 - pTex[ 1 ];
		}
		pTex[ 2 ] = 0.0;
		pTex[ 3 ] = 1.0;
		pTex += 4;
	}

	return true;
}

bool CGlLoaderModelXFile::Load( char *locFile )
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

	FILE *file = fopen( locFile, "r" );
	setvbuf( file, NULL, _IONBF, 0 );
	if( file == NULL )return false;

	char line[ 1024 ];
	fgets( line, 1024, file );
	Trim( line );
	if( strcmp( line, "xof 0302txt 0064" ) != 0 )return false;

	int floorNow = 0;
	RetrieveMain( file, floorNow );

	MakeGroupByMaterial();

	if( m_EnableDebug )
	{
		printf( "(amtver,amtind)=(%d,%d)\n", GetAmtSkinsVer(), GetAmtSkinsInd() );
		//m_ArrPos->TestPrint( "Pos", s );
		//m_ArrInd->TestPrint( "Index", s );
		//m_ArrTex->TestPrint( "CoordTex", s );
		for( int s=0; s<m_nAmtSkins; s++ ){
			printf( "skin[%d] (head,cnt)=(%d,%d)\n", s, m_TableHeadInd[ s ], m_TableCntInd[ s ] );
		}
	}

	bool state = true;
	if( floorNow != 0 )state = false;
	fclose( file );
	return state;
}

/*
	@brief スキンにポリゴンを構築する。ローダでも必要となるので、先にここに作っておく
	@param ind グループ・インデックス
	@param skin スキン
*/
bool CGlLoaderModelXFile::MakeDataPolygon( int ind, CGlSkin *skin )
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
