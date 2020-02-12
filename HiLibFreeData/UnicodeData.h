#pragma once

class CUnicodeData_Node;
class CUnicodeData_Leaf;

class CUnicodeData_Node
{
protected:
	static const int ARR_MAX = 32;
	enum _typeIndex { NUMBER, NAME } m_TypeIndex;	//!<インデックスのタイプ
	union _index {
		wchar_t name[ 32 ];
		int number;
	} m_Index;	//!<インデックス

	CUnicodeData_Node *m_NodeChildTop, *m_NodeChildBottom;
	CUnicodeData_Node *m_NodePrevious, *m_NodeNext;

	///// 高速化のためのキャッシュ
	CUnicodeData_Node **m_CacheNumber;	//!< [ 0 ]はm_CacheNumberMinの値のノード
	int m_CacheNumberMin, m_CacheNumberCnt;

	wchar_t *ShiftNextSpace( wchar_t *str ){
		while( *str != ' ' ){
			if( *str == '\n' || *str == 0 )return NULL;
			str++;
		}
		return ++str;
	}
	wchar_t *ShiftNextColon( wchar_t *str ){
		while( *str != ':' ){
			if( *str == '\n' || *str == 0 )return NULL;
			str++;
		}
		return ++str;
	}

public:
	CUnicodeData_Node(){
		m_TypeIndex = NAME;
		m_NodeChildTop = NULL; m_NodeChildBottom = NULL;
		m_NodePrevious = NULL; m_NodeNext = NULL;

		m_CacheNumber = NULL;
		m_CacheNumberMin = 0;
		m_CacheNumberCnt = 0;
	}
	~CUnicodeData_Node(){
		if( m_CacheNumber != NULL ){
			delete [] m_CacheNumber;
			m_CacheNumber = NULL;
		}
	}

	void DestroyChildren(){
		CUnicodeData_Node *pNodeTmp, *pNode = GetNodeChildTop();
		while( pNode != NULL ){
			pNodeTmp = pNode;
			pNode = pNode->GetNext();
			pNodeTmp->DestroyChildren();
			delete pNodeTmp;
		}
		m_NodeChildTop = NULL;
		m_NodeChildBottom = NULL;
	}

	void SetIndex( wchar_t *name ){
		m_TypeIndex = NAME;
		wcsncpy( m_Index.name, name, 32 );
	}
	void SetIndex( int number ){
		m_TypeIndex = NUMBER;
		m_Index.number = number;
	}
	int GetNumber(){ return m_Index.number; }
	wchar_t *GetName(){ return m_Index.name; }
	bool IndexIsName(){ if(m_TypeIndex==NAME){ return true; } return false; }
	bool IndexIsNumber(){ if(m_TypeIndex==NUMBER){ return true; } return false; }
	bool IndexIsSame( CUnicodeData_Node *node ){ return m_TypeIndex == node->m_TypeIndex; }

	virtual void Copy( CUnicodeData_Node *node ){
		m_TypeIndex = node->m_TypeIndex;
		memcpy( m_Index.name, node->m_Index.name, sizeof( _index ) );
	}

	CUnicodeData_Node *GetNodeChildTop(){ return m_NodeChildTop; }
	CUnicodeData_Node *GetNodeChildBottom(){ return m_NodeChildBottom; }
	void SetPrevious(CUnicodeData_Node *p){ m_NodePrevious=p; }
	CUnicodeData_Node *GetPrevious(){ return m_NodePrevious; }
	void SetNext(CUnicodeData_Node *p){ m_NodeNext=p; }	
	CUnicodeData_Node *GetNext(){ return m_NodeNext; }

	bool LessNumber(int inNum){ if(m_Index.number<inNum){ return true; } return false; }
	bool LessName(wchar_t *inName){ if(wcscmp(m_Index.name, inName)<0){ return true; } return false; }
	bool EqualNumber(int inNum){ if(m_Index.number==inNum){ return true; } return false; }
	bool EqualName(wchar_t *inName){ if(wcscmp(m_Index.name, inName)==0){ return true; } return false; }
	bool MoreNumber(int inNum){ if(m_Index.number>inNum){ return true; } return false; }
	bool MoreName(wchar_t *inName){ if(wcscmp(m_Index.name, inName)>0){ return true; } return false; }

	void MakeCache();
	void MakeCache_Recursive();
	CUnicodeData_Node *SearchNodeChildByCache( int number );

	CUnicodeData_Node *SearchNodeChild( wchar_t *name );
	CUnicodeData_Node *SearchNodeChild( int number );

	CUnicodeData_Node *SetNodeChild( wchar_t *name );		///// Nodeの作成
	CUnicodeData_Node *SetNodeChild( int number );		///// Nodeの作成
	CUnicodeData_Leaf *SetLeafChild( wchar_t *name );		///// Leafの作成
	CUnicodeData_Leaf *SetLeafChild( int number );		///// Leafの作成
	CUnicodeData_Leaf *ScaningSetLeafChild_IndexName( wchar_t *str );///// Leafの作成
//	bool AttachNodeChild( CUnicodeData_Node *node );	///// Leafの貼り付けに
};

class CUnicodeData_Leaf : public CUnicodeData_Node
{
protected:
	enum _typeData { CHAR, INT, FLOAT, BOOL } m_TypeData;	//!<データのタイプ
	union _data {	/////256byte
		wchar_t string[ 128 ];
		long arrInt[ ARR_MAX ];
		double arrFloat[ ARR_MAX ];
		bool arrBool[ ARR_MAX ];
	} m_Data;	//!<データ
	int m_Cnt;	//!<個数

public:
	CUnicodeData_Leaf(){
		m_TypeData = CHAR;
		m_TypeIndex = NAME;
		m_Cnt = 0;
	}
	~CUnicodeData_Leaf(){
	}

	void Copy( CUnicodeData_Leaf *node ){
		m_TypeIndex = node->m_TypeIndex;
		memcpy( m_Index.name, node->m_Index.name, sizeof( _index ) );
		m_TypeData = node->m_TypeData;
		memcpy( m_Data.string, node->m_Data.string, sizeof( _data ) );
	}

	int GetCnt(){ return m_Cnt; }
	bool DataIsINT(){ if(m_TypeData==INT){ return true; } return false; }
	bool DataIsFLOAT(){ if(m_TypeData==FLOAT){ return true; } return false; }
	bool DataIsCHAR(){ if(m_TypeData==CHAR){ return true; } return false; }
	bool DataIsBOOL(){ if(m_TypeData==BOOL){ return true; } return false; }

	void SetData( wchar_t *name, wchar_t *data );
	void SetData( wchar_t *name, int cnt, int *data );
	void SetData( wchar_t *name, int cnt, long *data );
	void SetData( wchar_t *name, int cnt, float *data );
	void SetData( wchar_t *name, int cnt, double *data );
	void SetData( wchar_t *name, int cnt, bool *data );
	void SetData( int number, wchar_t *data );
	void SetData( int number, int cnt, int *data );
	void SetData( int number, int cnt, long *data );
	void SetData( int number, int cnt, float *data );
	void SetData( int number, int cnt, double *data );
	void SetData( int number, int cnt, bool *data );

	int GetData( wchar_t *data );
	int GetData( int *data );
	int GetData( long *data );
	int GetData( float *data );
	int GetData( double *data );
	int GetData( bool *data );
	wchar_t *GetDataChar(){ return m_Data.string; }
	long *GetDataInt(){ return m_Data.arrInt; }
	double *GetDataFloat(){ return m_Data.arrFloat; }
	bool *GetDataBool(){ return m_Data.arrBool; }

	void PrintData( FILE *file );
	//void ScanIndexNameString( wchar_t *str );
	//void ScanIndexNumberString( wchar_t *str );
};

class CUnicodeData
{
protected:
	CUnicodeData_Node m_NodeTop;
	CUnicodeData_Node *m_NodeNameSpaceNow;

	CUnicodeData_Leaf *m_LeafIterate;

public:
	CUnicodeData(void){
		m_LeafIterate = NULL;

		Create();
	}
	~CUnicodeData(void){
		Destroy();
	}

	void Create(){
		m_NodeNameSpaceNow = m_NodeTop.SetNodeChild( L"" );		
	}
	void Destroy(){
		m_NodeTop.DestroyChildren();
	}
	void MakeCache(){
		CUnicodeData_Node *pNode = m_NodeTop.GetNodeChildTop();
		while( pNode != NULL ){
			pNode->MakeCache();
			pNode = pNode->GetNext();
		}
	}

	void SetNameSpace( wchar_t *name );
	void GetNameSpace( int number );
	void Set(int num, wchar_t *name, int cnt, int *dataIn);
	void Set(int num, wchar_t *name, int cnt, long *dataIn);
	void Set(int num, wchar_t *name, int cnt, float *dataIn);
	void Set(int num, wchar_t *name, int cnt, double *dataIn);
	void Set(int num, wchar_t *name, wchar_t *dataIn);
	void Set(int num, wchar_t *name, int cnt, bool *dataIn);
	int Get(int num, wchar_t *name, int *dataOut);
	int Get(int num, wchar_t *name, long *dataOut);
	int Get(int num, wchar_t *name, float *dataOut);
	int Get(int num, wchar_t *name, double *dataOut);
	int Get(int num, wchar_t *name, wchar_t *dataOut);
	int Get(int num, wchar_t *name, bool *dataOut);
	bool Save(wchar_t *fileName);
	bool Load(wchar_t *fileName);
	bool Retrieve(wchar_t *str);

	bool ForeachStart( int num );
	int ForeachGet( wchar_t *nameOut, void *dataOut, wchar_t *typeOut=NULL );
};

