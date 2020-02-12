#pragma once

class CGlLoaderModelMetasequoia : public IGlLoaderModel
{
protected:
	bool m_UseTex;//!<テクスチャを使うかどうか
	bool m_IsTextureUnified;

public:
	CGlLoaderModelMetasequoia(void){
		m_UseTex = false;
		m_IsTextureUnified = true;
	}
	~CGlLoaderModelMetasequoia(void){
	}

	void Destroy(){
		if( m_ArrPos != NULL ){ delete [] m_ArrPos; m_ArrPos=NULL; }
		if( m_ArrInd != NULL ){ delete [] m_ArrInd; m_ArrInd=NULL; }
		if( m_ArrNrm != NULL ){ delete [] m_ArrNrm; m_ArrNrm=NULL; }
		if( m_ArrTex != NULL ){ delete [] m_ArrTex; m_ArrTex=NULL; }
	}

protected:
	char *PickupClass( char *pStr, char *nameClass, char *outStr ){
		if( pStr==NULL || *pStr==0 )return 0;
		while( *pStr==' ' || *pStr=='\t' )pStr++;
		char *pStart, *p=pStr;
		while( *p!='(' ){ if( *p==0 )return 0; p++; }
		*p=0; p++; pStart=p;
		strcpy( nameClass, pStr );
		while( *p!=')' ){ if( *p==0 )return 0; p++; }
		*p=0; p++;
		if( outStr!=NULL)strcpy( outStr, pStart );
		return p;
	}
	char *PickupClass( char *pStr, char *nameClass, int *outArr ){
		if( pStr==NULL || *pStr==0 )return 0;
		while( *pStr==' ' || *pStr=='\t' )pStr++;
		char *pStart, *pEnd, *p=pStr;
		while( *p!='(' ){ if( *p==0 )return 0; p++; }
		*p=0; p++; pStart=p;
		strcpy( nameClass, pStr );
		while( *p!=')' ){ if( *p==0 )return 0; p++; }
		*p=0; p++; pEnd=p;
		p=pStart;
		int c = 0;
		while( *p!=0 ){ 
			outArr[c++]=atof(p);
			while( *p!=' ' && *p!=0 )p++;
			while( *p==' ' )p++;
		}
		return pEnd;
	}
	char *PickupClass( char *pStr, char *nameClass, double *outArr ){
		if( *pStr==0 )return 0;
		while( *pStr==' ' || *pStr=='\t' )pStr++;
		char *pStart, *pEnd, *p=pStr;
		while( *p!='(' ){ if( *p==0 )return 0; p++; }
		*p=0; p++; pStart=p;
		strcpy( nameClass, pStr );
		while( *p!=')' ){ if( *p==0 )return 0; p++; }
		*p=0; p++; pEnd=p;
		p=pStart;
		int c = 0;
		while( *p!=0 ){ 
			outArr[c++]=atof(p);
			while( *p!=' ' && *p!=0 )p++;
			while( *p==' ' )p++;
		}
		return pEnd;
	}

	int GetCntSkinPos( int skin ){ if( m_ArrPos==NULL )return 0; return m_ArrPos[ skin ].GetRows(); }
	int GetCntSkinInd( int skin ){ if( m_ArrInd==NULL )return 0; return m_ArrInd[ skin ].GetRows(); }
	int GetCntSkinNrm( int skin ){ if( m_ArrNrm==NULL )return 0; return m_ArrNrm[ skin ].GetRows(); }
	int GetCntSkinTex( int skin ){ if( m_ArrTex==NULL )return 0; return m_ArrTex[ skin ].GetRows(); }

	double *GetDataSkinPos( int skin ){ if( m_ArrPos==NULL )return NULL; return m_ArrPos[ skin ].GetData(); }
	unsigned int *GetDataSkinInd( int skin ){ if( m_ArrInd==NULL )return NULL; return m_ArrInd[ skin ].GetData(); }
	double *GetDataSkinNrm( int skin ){ if( m_ArrNrm==NULL )return NULL; return m_ArrNrm[ skin ].GetData(); }
	double *GetDataSkinTex( int skin ){ if( m_ArrTex==NULL )return NULL; return m_ArrTex[ skin ].GetData(); }

	bool RetrieveMaterial( FILE *file, int &floorNow );
	bool RetrieveObject( FILE *file, int index, int &floorNow );
	bool RetrieveObject_TextureNonUnified( FILE *file, int index, int &floorNow );

	bool MakeDataPolygon( int index, CGlSkin *skin );

public:
	void InitSetTextureUnified( bool b ){ m_IsTextureUnified = b; }
	bool Load( char *locFile );
	//bool MakeSkin( int index, CGlSkin *skin );
	//bool MakeSkin( char *name, CGlSkin *skin );
};
