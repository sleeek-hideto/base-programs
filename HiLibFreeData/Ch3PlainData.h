#pragma once

class Ch3PlainData_Node;
class Ch3PlainData_Leaf;

class Ch3PlainData_Node
{
protected:
	static const int ARR_MAX = 32;
	enum _typeIndex { NUMBER, NAME } m_TypeIndex;	//!<インデックスのタイプ
	union _index {
		char name[ 32 ];
		int number;
	} m_Index;	//!<インデックス

	Ch3PlainData_Node *m_NodeChildTop, *m_NodeChildBottom;
	Ch3PlainData_Node *m_NodePrevious, *m_NodeNext;

	///// 高速化のためのキャッシュ
	Ch3PlainData_Node **m_CacheNumber;	//!< [ 0 ]はm_CacheNumberMinの値のノード
	int m_CacheNumberMin, m_CacheNumberCnt;

	char *ShiftNextSpace( char *str ){
		while( *str != ' ' ){
			if( *str == '\n' || *str == 0 )return NULL;
			str++;
		}
		return ++str;
	}
	char *ShiftNextColon( char *str ){
		while( *str != ':' ){
			if( *str == '\n' || *str == 0 )return NULL;
			str++;
		}
		return ++str;
	}

public:
	Ch3PlainData_Node(){
		m_TypeIndex = NAME;
		m_NodeChildTop = NULL; m_NodeChildBottom = NULL;
		m_NodePrevious = NULL; m_NodeNext = NULL;

		m_CacheNumber = NULL;
		m_CacheNumberMin = 0;
		m_CacheNumberCnt = 0;
	}
	~Ch3PlainData_Node(){
		if( m_CacheNumber != NULL ){
			delete [] m_CacheNumber;
			m_CacheNumber = NULL;
		}
	}

	void DestroyChildren(){
		Ch3PlainData_Node *pNodeTmp, *pNode = GetNodeChildTop();
		while( pNode != NULL ){
			pNodeTmp = pNode;
			pNode = pNode->GetNext();
			pNodeTmp->DestroyChildren();
			delete pNodeTmp;
		}
		m_NodeChildTop = NULL;
		m_NodeChildBottom = NULL;
	}

	void SetIndex( char *name ){
		m_TypeIndex = NAME;
		strncpy( m_Index.name, name, 32 );
	}
	void SetIndex( int number ){
		m_TypeIndex = NUMBER;
		m_Index.number = number;
	}
	int GetNumber(){ return m_Index.number; }
	char *GetName(){ return m_Index.name; }
	bool IndexIsName(){ if(m_TypeIndex==NAME){ return true; } return false; }
	bool IndexIsNumber(){ if(m_TypeIndex==NUMBER){ return true; } return false; }
	bool IndexIsSame( Ch3PlainData_Node *node ){ return m_TypeIndex == node->m_TypeIndex; }

	virtual void Copy( Ch3PlainData_Node *node ){
		m_TypeIndex = node->m_TypeIndex;
		memcpy( m_Index.name, node->m_Index.name, sizeof( _index ) );
	}

	Ch3PlainData_Node *GetNodeChildTop(){ return m_NodeChildTop; }
	Ch3PlainData_Node *GetNodeChildBottom(){ return m_NodeChildBottom; }
	void SetPrevious(Ch3PlainData_Node *p){ m_NodePrevious=p; }
	Ch3PlainData_Node *GetPrevious(){ return m_NodePrevious; }
	void SetNext(Ch3PlainData_Node *p){ m_NodeNext=p; }	
	Ch3PlainData_Node *GetNext(){ return m_NodeNext; }

	bool LessNumber(int inNum){ if(m_Index.number<inNum){ return true; } return false; }
	bool LessName(char *inName){ if(strcmp(m_Index.name, inName)<0){ return true; } return false; }
	bool EqualNumber(int inNum){ if(m_Index.number==inNum){ return true; } return false; }
	bool EqualName(char *inName){ if(strcmp(m_Index.name, inName)==0){ return true; } return false; }
	bool MoreNumber(int inNum){ if(m_Index.number>inNum){ return true; } return false; }
	bool MoreName(char *inName){ if(strcmp(m_Index.name, inName)>0){ return true; } return false; }

	void MakeCache();
	void MakeCache_Recursive();
	Ch3PlainData_Node *SearchNodeChildByCache( int number );

	Ch3PlainData_Node *SearchNodeChild( char *name );
	Ch3PlainData_Node *SearchNodeChild( int number );

	Ch3PlainData_Node *SetNodeChild( char *name );		///// Nodeの作成
	Ch3PlainData_Node *SetNodeChild( int number );		///// Nodeの作成
	Ch3PlainData_Leaf *SetLeafChild( char *name );		///// Leafの作成
	Ch3PlainData_Leaf *SetLeafChild( int number );		///// Leafの作成
	Ch3PlainData_Leaf *ScaningSetLeafChild_IndexName( char *str );///// Leafの作成

	Ch3PlainData_Leaf *CopyLeafChild( Ch3PlainData_Leaf *leaf );///// Leafをコピー
};

class Ch3PlainData_Leaf : public Ch3PlainData_Node
{
protected:
	enum _typeData { CHAR, INT, FLOAT, BOOL } m_TypeData;	//!<データのタイプ
	union _data {
		char string[ 128 ];
		int arrInt[ 32 ];
		float arrFloat[ 32 ];
		bool arrBool[ 32 ];
	} m_Data;	//!<データ
	int m_Cnt;	//!<個数

public:
	Ch3PlainData_Leaf(){
		m_TypeData = CHAR;
		m_TypeIndex = NAME;
		m_Cnt = 0;
	}
	~Ch3PlainData_Leaf(){
	}

	void Copy( Ch3PlainData_Leaf *node ){
		m_TypeIndex = node->m_TypeIndex;
		memcpy( m_Index.name, node->m_Index.name, sizeof( _index ) );
		m_TypeData = node->m_TypeData;
		m_Cnt = node->m_Cnt;
		memcpy( m_Data.string, node->m_Data.string, sizeof( _data ) );
	}

	int GetCnt(){ return m_Cnt; }
	bool DataIsINT(){ if(m_TypeData==INT){ return true; } return false; }
	bool DataIsFLOAT(){ if(m_TypeData==FLOAT){ return true; } return false; }
	bool DataIsCHAR(){ if(m_TypeData==CHAR){ return true; } return false; }
	bool DataIsBOOL(){ if(m_TypeData==BOOL){ return true; } return false; }

	void SetData( char *name, char *data );
	void SetData( char *name, int cnt, int *data );
	void SetData( char *name, int cnt, float *data );
	void SetData( char *name, int cnt, double *data );
	void SetData( char *name, int cnt, bool *data );
	void SetData( int number, char *data );
	void SetData( int number, int cnt, int *data );
	void SetData( int number, int cnt, float *data );
	void SetData( int number, int cnt, double *data );
	void SetData( int number, int cnt, bool *data );
	void DelData();

	int GetData( char *data );
	int GetData( int *data );
	int GetData( float *data );
	int GetData( double *data );
	int GetData( bool *data );
	char *GetDataChar(){ return m_Data.string; }
	int *GetDataInt(){ return m_Data.arrInt; }
	float *GetDataFloat(){ return m_Data.arrFloat; }
	bool *GetDataBool(){ return m_Data.arrBool; }

	void PrintData( FILE *file );
	//void ScanIndexNameString( char *str );
	//void ScanIndexNumberString( char *str );
};

class Ch3PlainData
{
protected:
	Ch3PlainData_Node m_NodeTop;
	Ch3PlainData_Node *m_NodeNameSpaceNow;

	Ch3PlainData_Leaf *m_LeafIterate;

public:
	Ch3PlainData(void){
		m_LeafIterate = NULL;

		Create();
	}
	~Ch3PlainData(void){
		Destroy();
	}

	void Create(){
		m_NodeNameSpaceNow = m_NodeTop.SetNodeChild( "" );		
	}
	void Destroy(){
		m_NodeTop.DestroyChildren();
	}
	void MakeCache(){
		Ch3PlainData_Node *pNode = m_NodeTop.GetNodeChildTop();
		while( pNode != NULL ){
			pNode->MakeCache();
			pNode = pNode->GetNext();
		}
	}

	void SetNameSpace( char *name );
	void GetNameSpace( int number );
	void Set(int num, char *name, int cnt, int *dataIn);
	void Set(int num, char *name, int cnt, float *dataIn);
	void Set(int num, char *name, int cnt, double *dataIn);
	void Set(int num, char *name, char *dataIn);
	void Set(int num, char *name, int cnt, bool *dataIn);
	void Del(int num, char *name);
	int Get(int num, char *name, int *dataOut);
	int Get(int num, char *name, float *dataOut);
	int Get(int num, char *name, double *dataOut);
	int Get(int num, char *name, char *dataOut);
	int Get(int num, char *name, bool *dataOut);
	bool Save(char *fileName);
	bool Load(char *fileName);
	bool Retrieve(char *str);
	bool Copy( Ch3PlainData *dataOrg, char **namespaces=NULL );

	bool ForeachStart( int num );
	int ForeachGet( char *nameOut, void *dataOut, char *typeOut=NULL );
};
