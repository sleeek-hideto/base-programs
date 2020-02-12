#include "StdAfx.h"
#include "GlLoaderModelMetasequoia.h"

/*!
	@brief 材質の解析を行う
	@param[in] file ファイル・ポインタ
	@param[inout] floorNow 入れ子の階層
*/
bool CGlLoaderModelMetasequoia::RetrieveMaterial( FILE *file, int &floorNow )
{
	int floorStart = floorNow;

	int indexMtl = -1;
	char line[ 1024 ];
	while( fgets( line, 1024, file ) != NULL )
	{
		char *p = line;
		///// 括弧が閉じるかを調べる
		while( *p!=0 ){ if( *p=='}' )floorNow--; if( *p=='{' )floorNow++; p++; }
		if( floorNow < floorStart )return true;

		char nameMat[ 64 ];
		//char strCol[ 64 ], strDif[ 64 ], strAmb[ 64 ], strEmi[ 64 ], strSpc[ 64 ], strPower[ 64 ], strTex[ 64 ], strApl[ 64 ];
		//strCol[0]=0; strDif[0]=0; strAmb[0]=0; strEmi[0]=0; strSpc[0]=0; strPower[0]=0; strTex[0]=0; strApl[0]=0;

		sscanf( line, "%s", nameMat );
		indexMtl++;/////注意
		SetMtl2Index( PickupString( nameMat ), indexMtl );

		p = line;
		while( *p==' ' || *p=='\t' )p++; while( *p!=' ' && *p!='\t' && *p!=0 )p++; while( *p==' ' || *p=='\t' )p++;

		double col[ 4 ], dif=0.0, amb=0.0, emi=0.0, spc=0.0, power=0.0;
		char tex[ 64 ]="", apl[ 64 ]="";
		char nameClass[ 64 ];
		double tmpCol[ 4 ];
		p = PickupClass( p, nameClass, col );
		p = PickupClass( p, nameClass, &dif );
		p = PickupClass( p, nameClass, &amb );
		p = PickupClass( p, nameClass, &emi );
		p = PickupClass( p, nameClass, &spc );
		p = PickupClass( p, nameClass, &power );
		p = PickupClass( p, nameClass, tex );
		p = PickupClass( p, nameClass, apl );

		///// Color Diff
		for( int c=0; c<3; c++ ){ tmpCol[c] = col[c]*dif; }
		tmpCol[3] = 1.0;
		m_DataMaterial.set( indexMtl, "colDiff", 4, tmpCol );

		///// Location Texture
		if( tex[ 0 ] == 0 )continue;
		m_UseTex = true;
		char *locTex = PickupString( tex );
			///// 注意
		char tmpLoc[ 128 ];
		if( m_DirectoryNow[ 0 ] == 0 ){
			sprintf( tmpLoc, "%s", locTex );
		}else{
			sprintf( tmpLoc, "%s%s", m_DirectoryNow, locTex );
		}
		m_DataMaterial.set( indexMtl, "locTex", -1, tmpLoc );
	}
	return true;
}

/*!
	@brief 材質の解析を行う
	@param[in] file ファイル・ポインタ
	@param[in] index スキン・インデックス
	@param[inout] floorNow 入れ子の階層
*/
bool CGlLoaderModelMetasequoia::RetrieveObject( FILE *file, int index, int &floorNow )
{
	if( index < 0 )return false;
	int floorStart = floorNow;

	char line[ 1024 ];
	char nameClass[ 64 ], namePrivate[ 64 ];
	int cntVer=0, cntInd=0;

	while( 1 )
	{
		int state = ScanFile( file, floorNow, nameClass, namePrivate );
		if( floorStart > floorNow )return true;/////重要
		if( state < 0 )return false;
		if( state < 2 )continue;

		if( strcmp( nameClass, "vertex" ) == 0 )
		{
			cntVer = atoi( namePrivate );
			if( cntVer < 1 )return false;
			m_ArrPos[ index ].Create( cntVer );
			if( m_UseTex )m_ArrTex[ index ].Create( cntVer );

			///// 解析
			double *pPos = m_ArrPos[ index ].GetData();
			for( int c=0; c<cntVer; c++ )
			{
				if( fgets( line, 1024, file ) == NULL )return false;
				sscanf( line, "%lf %lf %lf", pPos, pPos +1, pPos +2 );
				pPos[3] = 1.0;
				pPos += 4;
			}
			
		}else if( strcmp( nameClass, "face" ) == 0 ){
			/////頂点を使って面を構築
			fpos_t posFaceFirst;
			fgetpos( file, &posFaceFirst );
			int cntFace = atoi( namePrivate );
			if( cntFace < 1 )return false;

			cntInd = 0;
			for( int c=0; c<cntFace; c++ ){
				int cntPoint;
				if( fgets( line, 1024, file ) == NULL )return false;
				sscanf( line, "%d", &cntPoint );
				if( cntPoint == 3 )cntInd++;
				if( cntPoint == 4 )cntInd += 2;
			}
			if( cntInd < 1 )return false;
			m_ArrInd[ index ].Create( cntInd );

			fsetpos( file, &posFaceFirst );

			unsigned int *pInd = m_ArrInd[ index ].GetData();
			for( int c=0; c<cntFace; c++ )
			{
				if( fgets( line, 1024, file ) == NULL )return false;
				
				int cntPoint;
				sscanf( line, "%d", &cntPoint );
				char *p = line;
				while( *p==' ' || *p=='\t' )p++; while( *p!=' ' && *p!='\t' && *p!=0 )p++; while( *p==' ' || *p=='\t' )p++;
				if( cntPoint == 3 )
				{
					int arrInd[ 3 ], numMtl;
					double arrTex[ 6 ], rowTex[ 3 ] = { 0,0,0 };
					char nameType[ 64 ];
					p = PickupClass( p, nameType, arrInd );
					p = PickupClass( p, nameType, &numMtl );
					p = PickupClass( p, nameType, arrTex );

					if( c == 0 )m_DataSkin.set( index, "useMtl", 1, &numMtl );
					pInd[ 0 ] = (unsigned int)arrInd[ 2 ];
					pInd[ 1 ] = (unsigned int)arrInd[ 1 ];
					pInd[ 2 ] = (unsigned int)arrInd[ 0 ];
					if( m_UseTex )
					{
						rowTex[ 0 ] = arrTex[ 0 ];
						rowTex[ 1 ] = arrTex[ 1 ];
						m_ArrTex[ index ].SetDataRow( pInd[ 2 ], rowTex );
						rowTex[ 0 ] = arrTex[ 2 ];
						rowTex[ 1 ] = arrTex[ 3 ];
						m_ArrTex[ index ].SetDataRow( pInd[ 1 ], rowTex );
						rowTex[ 0 ] = arrTex[ 4 ];
						rowTex[ 1 ] = arrTex[ 5 ];
						m_ArrTex[ index ].SetDataRow( pInd[ 0 ], rowTex );
					}
					pInd += 3;
				}else if( cntPoint == 4 ){
					int arrInd[ 4 ], numMtl;
					double arrTex[ 8 ], rowTex[ 3 ] = { 0,0,0 };
					char nameType[ 64 ];
					p = PickupClass( p, nameType, arrInd );
					p = PickupClass( p, nameType, &numMtl );
					p = PickupClass( p, nameType, arrTex );

					if( c == 0 )m_DataSkin.set( index, "useMtl", 1, &numMtl );
					pInd[ 0 ] = (unsigned int)arrInd[ 2 ];
					pInd[ 1 ] = (unsigned int)arrInd[ 1 ];
					pInd[ 2 ] = (unsigned int)arrInd[ 0 ];
					if( m_UseTex )
					{
						rowTex[ 0 ] = arrTex[ 0 ];
						rowTex[ 1 ] = arrTex[ 1 ];
						m_ArrTex[ index ].SetDataRow( pInd[ 2 ], rowTex );
						rowTex[ 0 ] = arrTex[ 2 ];
						rowTex[ 1 ] = arrTex[ 3 ];
						m_ArrTex[ index ].SetDataRow( pInd[ 1 ], rowTex );
						rowTex[ 0 ] = arrTex[ 4 ];
						rowTex[ 1 ] = arrTex[ 5 ];
						m_ArrTex[ index ].SetDataRow( pInd[ 0 ], rowTex );
					}
					pInd += 3;

					pInd[ 0 ] = (unsigned int)arrInd[ 3 ];
					pInd[ 1 ] = (unsigned int)arrInd[ 2 ];
					pInd[ 2 ] = (unsigned int)arrInd[ 0 ];
					if( m_UseTex )
					{
						rowTex[ 0 ] = arrTex[ 0 ];
						rowTex[ 1 ] = arrTex[ 1 ];
						m_ArrTex[ index ].SetDataRow( pInd[ 2 ], rowTex );
						rowTex[ 0 ] = arrTex[ 4 ];
						rowTex[ 1 ] = arrTex[ 5 ];
						m_ArrTex[ index ].SetDataRow( pInd[ 1 ], rowTex );
						rowTex[ 0 ] = arrTex[ 6 ];
						rowTex[ 1 ] = arrTex[ 7 ];
						m_ArrTex[ index ].SetDataRow( pInd[ 0 ], rowTex );
					}
					pInd += 3;
				}
			}
		}
	}
	return true;
}

/*!
	@brief テクスチャ座標をもとに、材質の解析を行う
	@param[in] file ファイル・ポインタ
	@param[in] index スキン・インデックス
	@param[inout] floorNow 入れ子の階層
*/
bool CGlLoaderModelMetasequoia::RetrieveObject_TextureNonUnified( FILE *file, int index, int &floorNow )
{
	if( index < 0 )return false;
	int floorStart = floorNow;

	char line[ 1024 ];
	char nameClass[ 64 ], namePrivate[ 64 ];
	int cntVer=0, cntInd=0;
	
	CArrayVector3H tmpArrPos;

	while( 1 )
	{
		int state = ScanFile( file, floorNow, nameClass, namePrivate );
		if( floorStart > floorNow )return true;/////重要
		if( state < 0 )return false;
		if( state < 2 )continue;

		if( strcmp( nameClass, "vertex" ) == 0 )
		{
			cntVer = atoi( namePrivate );
			if( cntVer < 1 )return false;
			tmpArrPos.Create( cntVer );

			///// 解析
			double *pPos = tmpArrPos.GetData();
			for( int c=0; c<cntVer; c++ )
			{
				if( fgets( line, 1024, file ) == NULL )return false;
				sscanf( line, "%lf %lf %lf", pPos, pPos +1, pPos +2 );
				pPos[3] = 1.0;
				pPos += 4;
			}
			
		}else if( strcmp( nameClass, "face" ) == 0 ){
			/////まず、インデックスを調べて、三角面がいくつ必要かを数える
			fpos_t posFaceFirst;
			fgetpos( file, &posFaceFirst );
			int cntFace = atoi( namePrivate );
			if( cntFace < 1 )return false;

			cntInd = 0;
			for( int c=0; c<cntFace; c++ ){
				int cntPoint;
				if( fgets( line, 1024, file ) == NULL )return false;
				sscanf( line, "%d", &cntPoint );
				if( cntPoint == 3 )cntInd++;
				if( cntPoint == 4 )cntInd += 2;
			}
			if( cntInd < 1 )return false;
			m_ArrInd[ index ].Create( cntInd );/////インデックス配列だけでなく、
			m_ArrPos[ index ].Create( cntInd*3 );/////頂点配列
			if( m_UseTex )m_ArrTex[ index ].Create( cntInd*3 );/////テクスチャ座標配列

			fsetpos( file, &posFaceFirst );

			int numInd = 0;
			unsigned int *pInd = m_ArrInd[ index ].GetData();
			for( int c=0; c<cntFace; c++ )
			{
				if( fgets( line, 1024, file ) == NULL )return false;
				
				int cntPoint;
				sscanf( line, "%d", &cntPoint );
				char *p = line;
				while( *p==' ' || *p=='\t' )p++; while( *p!=' ' && *p!='\t' && *p!=0 )p++; while( *p==' ' || *p=='\t' )p++;
				if( cntPoint == 3 )
				{
					int arrInd[ 3 ], numMtl;
					double arrTex[ 6 ], rowTex[ 3 ] = { 0,0,0 };
					char nameType[ 64 ];
					p = PickupClass( p, nameType, arrInd );
					p = PickupClass( p, nameType, &numMtl );
					p = PickupClass( p, nameType, arrTex );

					if( c == 0 )m_DataSkin.set( index, "useMtl", 1, &numMtl );
					pInd[ 0 ] = (unsigned int)numInd +2;
					pInd[ 1 ] = (unsigned int)numInd +1;
					pInd[ 2 ] = (unsigned int)numInd;
					m_ArrPos[ index ].SetDataRow( numInd, tmpArrPos.GetData( arrInd[0] ) );
					m_ArrPos[ index ].SetDataRow( numInd +1, tmpArrPos.GetData( arrInd[1] ) );
					m_ArrPos[ index ].SetDataRow( numInd +2, tmpArrPos.GetData( arrInd[2] ) );

					if( m_UseTex )
					{
						rowTex[ 0 ] = arrTex[ 0 ];
						rowTex[ 1 ] = arrTex[ 1 ];
						m_ArrTex[ index ].SetDataRow( numInd, rowTex );
						rowTex[ 0 ] = arrTex[ 2 ];
						rowTex[ 1 ] = arrTex[ 3 ];
						m_ArrTex[ index ].SetDataRow( numInd +1, rowTex );
						rowTex[ 0 ] = arrTex[ 4 ];
						rowTex[ 1 ] = arrTex[ 5 ];
						m_ArrTex[ index ].SetDataRow( numInd +2, rowTex );
					}

					numInd += 3;
					pInd += 3;
				}else if( cntPoint == 4 ){
					int arrInd[ 4 ], numMtl;
					double arrTex[ 8 ], rowTex[ 3 ] = { 0,0,0 };
					char nameType[ 64 ];
					p = PickupClass( p, nameType, arrInd );
					p = PickupClass( p, nameType, &numMtl );
					p = PickupClass( p, nameType, arrTex );

					if( c == 0 )m_DataSkin.set( index, "useMtl", 1, &numMtl );
					pInd[ 0 ] = (unsigned int)numInd +2;
					pInd[ 1 ] = (unsigned int)numInd +1;
					pInd[ 2 ] = (unsigned int)numInd;
					m_ArrPos[ index ].SetDataRow( numInd, tmpArrPos.GetData( arrInd[0] ) );
					m_ArrPos[ index ].SetDataRow( numInd +1, tmpArrPos.GetData( arrInd[1] ) );
					m_ArrPos[ index ].SetDataRow( numInd +2, tmpArrPos.GetData( arrInd[2] ) );

					if( m_UseTex )
					{
						rowTex[ 0 ] = arrTex[ 0 ];
						rowTex[ 1 ] = arrTex[ 1 ];
						m_ArrTex[ index ].SetDataRow( numInd, rowTex );
						rowTex[ 0 ] = arrTex[ 2 ];
						rowTex[ 1 ] = arrTex[ 3 ];
						m_ArrTex[ index ].SetDataRow( numInd +1, rowTex );
						rowTex[ 0 ] = arrTex[ 4 ];
						rowTex[ 1 ] = arrTex[ 5 ];
						m_ArrTex[ index ].SetDataRow( numInd +2, rowTex );
					}

					numInd += 3;
					pInd += 3;

					/////二個目の三角
					pInd[ 0 ] = (unsigned int)numInd +2;
					pInd[ 1 ] = (unsigned int)numInd +1;
					pInd[ 2 ] = (unsigned int)numInd;
					m_ArrPos[ index ].SetDataRow( numInd, tmpArrPos.GetData( arrInd[0] ) );
					m_ArrPos[ index ].SetDataRow( numInd +1, tmpArrPos.GetData( arrInd[2] ) );
					m_ArrPos[ index ].SetDataRow( numInd +2, tmpArrPos.GetData( arrInd[3] ) );

					if( m_UseTex )
					{
						rowTex[ 0 ] = arrTex[ 0 ];
						rowTex[ 1 ] = arrTex[ 1 ];
						m_ArrTex[ index ].SetDataRow( numInd, rowTex );
						rowTex[ 0 ] = arrTex[ 4 ];
						rowTex[ 1 ] = arrTex[ 5 ];
						m_ArrTex[ index ].SetDataRow( numInd +1, rowTex );
						rowTex[ 0 ] = arrTex[ 6 ];
						rowTex[ 1 ] = arrTex[ 7 ];
						m_ArrTex[ index ].SetDataRow( numInd +2, rowTex );
					}
					pInd += 3;
				}
			}
		}
	}
	return true;
}

/*!
	@brief スキンskinに対して、インデックスのスキンindexの頂点データのみを構築する
	@param[in] index インデックス
	@param[inout] skin 対象となるスキン
*/
bool CGlLoaderModelMetasequoia::MakeDataPolygon( int index, CGlSkin *skin )
{
	if( index < -1 || index >= m_nAmtSkins )return false;
	if( skin == NULL )return false;

	///// 頂点データだけは確保
	skin->MakePolygon(
		GetCntSkinPos( index ), GetDataSkinPos( index ),
		GetCntSkinInd( index ), GetDataSkinInd( index ),
		GetCntSkinNrm( index ), GetDataSkinNrm( index ),
		GetCntSkinTex( index ), GetDataSkinTex( index )
		);

	if( GetDataSkinNrm( index ) == NULL )skin->CalcNormal();

	return true;
}

/*!
	@brief メタセコイアのデータを読み込む
	@param[in] locFile ファイルの位置
*/
bool CGlLoaderModelMetasequoia::Load( char *locFile )
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
	if( strcmp( line, "Metasequoia Document" ) != 0 )return false;

	fpos_t posFileFirst;
	fgetpos( file, &posFileFirst );

	char nameClass[ 64 ], namePrivate[ 64 ];
	///// データの「個数」の解析
	int floorNow = 0;
	m_nAmtSkins = 0;
	while( ScanFile( file, floorNow, nameClass, namePrivate ) >= 0 )
	{
		if( strcmp( nameClass, "Material" ) == 0 ){
			RetrieveMaterial( file, floorNow );
		}else if( strcmp( nameClass, "Object" ) == 0 ){
			SetGrp2Index( PickupString( namePrivate ), m_nAmtSkins );
			m_nAmtSkins++;
		}
	}
	if( m_nAmtSkins == 0 )return false;
	m_ArrPos = new CArrayVector3H[ m_nAmtSkins ];
	m_ArrInd = new CUintArrayVector3[ m_nAmtSkins ];
	if( m_UseTex )m_ArrTex = new CArrayVector3H[ m_nAmtSkins ];
	//m_ArrNrm = new CArrayVector3H[ m_nAmtSkins ];

	///// データの「中身」の解析
	fsetpos( file, &posFileFirst );
	floorNow = 0;
	while( ScanFile( file, floorNow, nameClass, namePrivate ) >= 0 )
	{
		if( strcmp( nameClass, "Object" ) == 0 ){
			if( m_IsTextureUnified ){
				RetrieveObject( file, Grp2Index( PickupString( namePrivate ) ), floorNow );
			}else{
				RetrieveObject_TextureNonUnified( file, Grp2Index( PickupString( namePrivate ) ), floorNow );
			}
		}
	}

	fclose( file );
	return true;
}

