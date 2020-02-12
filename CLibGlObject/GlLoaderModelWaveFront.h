#pragma once
#include "IGlLoaderModel.h"

class CGlLoaderModelWaveFront : public IGlLoaderModel
{
protected:
	int *m_TableHeadVer;	//!<個々のスキンの頂点配列の先頭への番目
	int *m_TableHeadInd;	//!<個々のスキンのインデックス配列の先頭への番目
	int *m_TableCntVer;		//!<個々のスキンの頂点個数
	int *m_TableCntInd;		//!<個々のスキンのインデックス個数

	//char m_DirectoryNow[ 128 ];

	bool m_EnableReindexing;

public:
	CGlLoaderModelWaveFront(void){
		m_TableHeadVer = NULL;
		m_TableHeadInd = NULL;
		m_TableCntVer = NULL;
		m_TableCntInd = NULL;

		m_EnableReindexing = true;
	}
	~CGlLoaderModelWaveFront(void){
	}

	void Destroy(){
		if( m_TableHeadVer != NULL ){ delete [] m_TableHeadVer; m_TableHeadVer=NULL; }
		if( m_TableHeadInd != NULL ){ delete [] m_TableHeadInd; m_TableCntInd=NULL; }
		if( m_TableCntVer != NULL ){ delete [] m_TableCntVer; m_TableCntVer=NULL; }
		if( m_TableCntInd != NULL ){ delete [] m_TableCntInd; m_TableCntInd=NULL; }

		if( m_ArrPos != NULL ){ delete m_ArrPos; m_ArrPos=NULL; }
		if( m_ArrInd != NULL ){ delete m_ArrInd; m_ArrInd=NULL; }
		if( m_ArrNrm != NULL ){ delete m_ArrNrm; m_ArrNrm=NULL; }
		if( m_ArrTex != NULL ){ delete m_ArrTex; m_ArrTex=NULL; }
	}

private:
	//char *ShiftNextNoSpace( char *str ){
	//	while(str[0]!=' ' && str[0]!='\t' && str[0]!='\n' && str[0]!=0)str++;
	//	while(str[0]==' ' || str[0]=='\t')str++;
	//	return str;
	//}
	int CountSpace( char *str ){
		int cnt = 0;
		while( 1 ){
			while( str[0] != ' ' && str[0] != '\t' ){
				if( str[0] == 0 || str[0] == '\n' )return cnt;
				str++;
			}
			cnt++;
			while( str[0] == ' ' || str[0] == '\t' )str++;
		}
		return cnt;
	}

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

protected:
	bool LoadWaveFrontMtl( char *locFile );
	bool MakeDataPolygon( int ind, CGlSkin *skin );
public:
	bool Load( char *locFile );
};
