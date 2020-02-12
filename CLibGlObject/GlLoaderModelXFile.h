#pragma once

class CGlLoaderModelXFile : public IGlLoaderModel
{
protected:
	int *m_TableHeadVer;	//!<個々のスキンの頂点配列の先頭への番目
	int *m_TableHeadInd;	//!<個々のスキンのインデックス配列の先頭への番目
	int *m_TableCntVer;		//!<個々のスキンの頂点個数
	int *m_TableCntInd;		//!<個々のスキンのインデックス個数

	//char m_DirectoryNow[ 128 ];

	int m_nAmtMixFace;	//<! m_nAmtMixFaceは面の数、m_nAmtSkinsはスキンの数。
	int *m_ArrMixFace;	//<! 面の配列
	int *m_TableMixFace2Ind;	//<!面からインデックスへと変換するテーブル

	int *m_ArrMaterial;	//<! それぞれの面がどのマテリアル・グループに属するか

	bool m_EnableReindexing;

public:
	CGlLoaderModelXFile(void){
		m_TableHeadVer = NULL;
		m_TableHeadInd = NULL;
		m_TableCntVer = NULL;
		m_TableCntInd = NULL;

		m_nAmtMixFace = 0;
		m_ArrMixFace = NULL;
		m_TableMixFace2Ind = NULL;

		m_ArrMaterial = NULL;

		m_EnableReindexing = true;
	}
	~CGlLoaderModelXFile(void){
	}

	void Destroy(){
		if( m_TableHeadVer != NULL ){ delete [] m_TableHeadVer; m_TableHeadVer=NULL; }
		if( m_TableHeadInd != NULL ){ delete [] m_TableHeadInd; m_TableHeadInd=NULL; }
		if( m_TableCntVer != NULL ){ delete [] m_TableCntVer; m_TableCntVer=NULL; }
		if( m_TableCntInd != NULL ){ delete [] m_TableCntInd; m_TableCntInd=NULL; }

		if( m_ArrMixFace != NULL ){ delete [] m_ArrMixFace; m_ArrMixFace=NULL; }
		if( m_TableMixFace2Ind != NULL ){ delete [] m_TableMixFace2Ind; m_TableMixFace2Ind=NULL; }

		if( m_ArrMaterial != NULL ){ delete [] m_ArrMaterial; m_ArrMaterial=NULL; }

		if( m_ArrPos != NULL ){ delete m_ArrPos; m_ArrPos=NULL; }
		if( m_ArrInd != NULL ){ delete m_ArrInd; m_ArrInd=NULL; }
		if( m_ArrNrm != NULL ){ delete m_ArrNrm; m_ArrNrm=NULL; }
		if( m_ArrTex != NULL ){ delete m_ArrTex; m_ArrTex=NULL; }
	}

private:
	int GetAmtSkinsVer(){
		if( m_ArrPos == NULL )return -1;
		return m_ArrPos->GetRows();
	}
	int GetAmtSkinsInd(){
		if( m_ArrInd == NULL )return -1;
		return m_ArrInd->GetRows();
	}

	int GetHeadSkinVer( int skin ){
		return m_TableHeadVer[ skin ];
	}
	int GetHeadSkinInd( int skin ){
		return m_TableHeadInd[ skin ];
	}
	int GetCntSkinPos( int skin ){
		return m_TableCntVer[ skin ];
	}
	int GetCntSkinInd( int skin ){
		return m_TableCntInd[ skin ];
	}
	int GetCntSkinNrm( int skin ){
		if( m_ArrNrm == NULL )return 0;
		return m_TableCntVer[ skin ];
	}
	int GetCntSkinTex( int skin ){
		if( m_ArrTex == NULL )return 0;
		return m_TableCntVer[ skin ];
	}

	double *GetDataSkinPos( int skin ){
		if( m_TableHeadVer == NULL || m_ArrPos == NULL )return NULL;
		int bias = m_TableHeadVer[ skin ];
		return m_ArrPos->GetData( bias );
	}
	unsigned int *GetDataSkinInd( int skin ){
		if( m_TableHeadInd == NULL || m_ArrInd == NULL )return NULL;
		int bias = m_TableHeadInd[ skin ];
		return m_ArrInd->GetData( bias );
	}
	double *GetDataSkinNrm( int skin ){
		if( m_TableHeadVer == NULL || m_ArrNrm == NULL )return NULL;
		int bias = m_TableHeadVer[ skin ];
		return m_ArrNrm->GetData( bias );
	}
	double *GetDataSkinTex( int skin ){
		if( m_TableHeadVer == NULL || m_ArrTex == NULL )return NULL;
		int bias = m_TableHeadVer[ skin ];
		return m_ArrTex->GetData( bias );
	}

	//int ScanFile( FILE *file, int &floorNow, char *nameClass, char *namePrivate ){
	//	char line[ 1024 ];
	//	nameClass[ 0 ] = 0;	namePrivate[ 0 ] = 0;
	//	if( fgets( line, 1024, file ) == NULL )return -1;
	//	if( line[ 0 ] == '\n' )return 0;
	//	char *p = line;
	//	while( *p != 0 ){		if( *p == '{' )floorNow++; if( *p == '}' )floorNow--; p++;		}
	//	p = line; while( *p == ' ' || *p == '\t' )p++;
	//	return sscanf( p, "%s %s", nameClass, namePrivate );
	//}
	//int ScanFile( FILE *file, int &floorNow, int &num ){
	//	char line[ 1024 ];
	//	if( fgets( line, 1024, file ) == NULL )return -1;
	//	if( line[ 0 ] == '\n' )return 0;
	//	char *p = line;
	//	while( *p != 0 ){		if( *p == '{' )floorNow++; if( *p == '}' )floorNow--; p++;		}
	//	p = line; while( *p == ' ' || *p == '\t' )p++;
	//	return sscanf( line, "%d;", &num );
	//}
	//void Trim( char *str ){
	//	int len = (int)strlen( str );
	//	if( str[ len -1 ] == '\n' )str[ len -1 ] = 0;
	//}
	//char *PickupString( char *p ){
	//	char *pStart, *pEnd;
	//	while( *p != '"' ){		if( *p == 0 )return NULL; p++;		}
	//	pStart = ++p;
	//	while( *p != '"' ){		if( *p == 0 )return NULL; p++;		}
	//	pEnd = p;

	//	*pEnd = 0;	return pStart;
	//}

	bool RetrieveMain( FILE *file , int &floorNow );
	bool RetrieveMesh( FILE *file , int &floorNow );
	bool MakeGroupByMaterial();
	bool RetrieveMaterial( FILE *file, int &fllorNow, int indMtl );
	bool RetrieveMaterialList( FILE *file , int &floorNow );
	bool RetrieveNormal( FILE *file , int &floorNow );
	bool RetrieveCoordsTex( FILE *file, int &floorNow, bool coordsIsReverse=false );

	bool MakeDataPolygon( int ind, CGlSkin *skin );

public:
	bool Load( char *locFile );
};
