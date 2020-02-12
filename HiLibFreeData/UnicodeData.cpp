
#include "StdAfx.h"
#include "UnicodeData.h"

/*
	@brief キャッシュを作る、今はNumberのみ
*/
void CUnicodeData_Node::MakeCache()
{
	if( GetNodeChildTop() == NULL || GetNodeChildBottom() == NULL )return;
	if( GetNodeChildTop()->IndexIsNumber() == false )return;

	int numberMax;
	m_CacheNumberMin = GetNodeChildTop()->GetNumber();
	numberMax = GetNodeChildBottom()->GetNumber();
	m_CacheNumberCnt = numberMax - m_CacheNumberMin +1;
	if( m_CacheNumberCnt < 1 )return;

	if( m_CacheNumber != NULL )delete [] m_CacheNumber;
	m_CacheNumber = new CUnicodeData_Node* [ m_CacheNumberCnt ];
	for( int c=0; c<m_CacheNumberCnt; c++ ){ m_CacheNumber[ c ] = NULL; }
	
	CUnicodeData_Node *pNode = GetNodeChildTop();
	while( pNode != NULL )
	{
		int numberCur = pNode->GetNumber();
		if( m_CacheNumber[ numberCur ] == NULL )m_CacheNumber[ numberCur -m_CacheNumberMin ] = pNode;
		pNode = pNode->GetNext();
	}
}

void CUnicodeData_Node::MakeCache_Recursive()
{
	CUnicodeData_Node *pNode = GetNodeChildTop();
	while( pNode != NULL )
	{
		if( pNode->GetNodeChildTop() != NULL ){
			pNode->MakeCache();
			pNode->MakeCache_Recursive();
		}
		pNode = pNode->GetNext();
	}
}

CUnicodeData_Node *CUnicodeData_Node::SearchNodeChildByCache( int number )
{
	if( m_CacheNumber == NULL )return NULL;
	if( GetNodeChildTop()->IndexIsNumber() == false )return NULL;
	if( number < m_CacheNumberMin || number >= m_CacheNumberMin +m_CacheNumberCnt )return NULL;

	return m_CacheNumber[ number -m_CacheNumberMin ];
}

CUnicodeData_Node *CUnicodeData_Node::SearchNodeChild( wchar_t *name )
{
	if( GetNodeChildTop() == NULL || GetNodeChildBottom() == NULL )return NULL;
	if( GetNodeChildTop()->IndexIsName() == false )return NULL;

	CUnicodeData_Node *pNode = GetNodeChildTop();
	while( pNode != NULL )
	{
		if( pNode->EqualName( name ) )return pNode;
		if( pNode->MoreName( name ) )return NULL;
		pNode = pNode->GetNext();
	}
	return NULL;
}

CUnicodeData_Node *CUnicodeData_Node::SearchNodeChild( int number )
{
	if( GetNodeChildTop() == NULL || GetNodeChildBottom() == NULL )return NULL;
	if( GetNodeChildTop()->IndexIsNumber() == false )return NULL;

	CUnicodeData_Node *pNode = SearchNodeChildByCache( number );
	if( pNode != NULL )return pNode;
	pNode = GetNodeChildTop();

	while( pNode != NULL )
	{
		if( pNode->EqualNumber( number ) )return pNode;
		if( pNode->MoreNumber( number ) )return NULL;
		pNode = pNode->GetNext();
	}
	return NULL;
}

CUnicodeData_Node *CUnicodeData_Node::SetNodeChild( wchar_t *name )
{
	CUnicodeData_Node *pNodeNew;

	///// 子供がいないとき
	if( GetNodeChildTop() == NULL ){
		pNodeNew = new CUnicodeData_Node();
		pNodeNew->SetIndex( name );
		m_NodeChildTop = pNodeNew;
		m_NodeChildBottom = pNodeNew;
		pNodeNew->SetPrevious( NULL );
		pNodeNew->SetNext( NULL );
		return pNodeNew;
	}
	
	///// IndexはName型ではなかったときはエラー
	if( GetNodeChildTop()->IndexIsName() == false )return NULL;

	///// 最初の子供より若いとき
	if( GetNodeChildTop()->MoreName( name ) ){
		pNodeNew = new CUnicodeData_Node();
		pNodeNew->SetIndex( name );
		pNodeNew->SetPrevious( NULL );
		pNodeNew->SetNext( GetNodeChildTop() );
		GetNodeChildTop()->SetPrevious( pNodeNew );
		m_NodeChildTop = pNodeNew;
		return pNodeNew;
	}
	///// 最後の子供より老いているとき
	if( GetNodeChildBottom()->LessName( name ) ){
		pNodeNew = new CUnicodeData_Node();
		pNodeNew->SetIndex( name );
		pNodeNew->SetNext( NULL );
		pNodeNew->SetPrevious( GetNodeChildBottom() );
		GetNodeChildBottom()->SetNext( pNodeNew );
		m_NodeChildBottom = pNodeNew;
		return pNodeNew;
	}

	///// 普通に検索
	CUnicodeData_Node *pNode = GetNodeChildTop();
	while( pNode != NULL )
	{
		///// 同じとき
		if( pNode->EqualName( name ) ){
			return pNode;
		}
		if( pNode->MoreName( name ) ){
			pNodeNew = new CUnicodeData_Node();
			pNodeNew->SetIndex( name );
			CUnicodeData_Node *pNodePre = pNode->GetPrevious();				
			pNodePre->SetNext( pNodeNew );
			pNodeNew->SetPrevious( pNodePre );
			pNodeNew->SetNext( pNode );
			pNode->SetPrevious( pNodeNew );
			return pNodeNew;
		}
		pNode = pNode->GetNext();
	}
	return NULL;	///// エラー
}

CUnicodeData_Node *CUnicodeData_Node::SetNodeChild( int number )
{
	CUnicodeData_Node *pNodeNew;

	///// 子供がいないとき
	if( GetNodeChildTop() == NULL ){
		pNodeNew = new CUnicodeData_Node();
		pNodeNew->SetIndex( number );
		m_NodeChildTop = pNodeNew;
		m_NodeChildBottom = pNodeNew;
		pNodeNew->SetPrevious( NULL );
		pNodeNew->SetNext( NULL );
		return pNodeNew;
	}
	
	///// IndexはNumber型ではなかったときはエラー
	if( GetNodeChildTop()->IndexIsNumber() == false )return NULL;

	///// キャッシュがあるならばまず使って検索
	CUnicodeData_Node *pNode = SearchNodeChildByCache( number );
	if( pNode != NULL )return pNode;

	///// 最初の子供より若いとき
	if( GetNodeChildTop()->MoreNumber( number ) ){
		pNodeNew = new CUnicodeData_Node();
		pNodeNew->SetIndex( number );
		pNodeNew->SetPrevious( NULL );
		pNodeNew->SetNext( GetNodeChildTop() );
		GetNodeChildTop()->SetPrevious( pNodeNew );
		m_NodeChildTop = pNodeNew;
		return pNodeNew;
	}
	///// 最後の子供より老いているとき
	if( GetNodeChildBottom()->LessNumber( number ) ){
		pNodeNew = new CUnicodeData_Node();
		pNodeNew->SetIndex( number );
		pNodeNew->SetNext( NULL );
		pNodeNew->SetPrevious( GetNodeChildBottom() );
		GetNodeChildBottom()->SetNext( pNodeNew );
		m_NodeChildBottom = pNodeNew;
		return pNodeNew;
	}

	///// 普通に検索
	pNode = GetNodeChildTop();
	while( pNode != NULL )
	{
		///// 同じとき
		if( pNode->EqualNumber( number ) ){
			return pNode;
		}
		if( pNode->MoreNumber( number ) ){
			pNodeNew = new CUnicodeData_Node();
			pNodeNew->SetIndex( number );
			CUnicodeData_Node *pNodePre = pNode->GetPrevious();				
			pNodePre->SetNext( pNodeNew );
			pNodeNew->SetPrevious( pNodePre );
			pNodeNew->SetNext( pNode );
			pNode->SetPrevious( pNodeNew );
			return pNodeNew;
		}
		pNode = pNode->GetNext();
	}
	return NULL;	///// エラー
}

CUnicodeData_Leaf *CUnicodeData_Node::SetLeafChild( wchar_t *name )
{
	CUnicodeData_Leaf *pLeafNew;
	if( GetNodeChildTop() == NULL ){
		///// 子供がいない時
		pLeafNew = new CUnicodeData_Leaf();
		pLeafNew->SetIndex( name );
		m_NodeChildTop = pLeafNew;
		m_NodeChildBottom = pLeafNew;
		pLeafNew->SetPrevious( NULL );
		pLeafNew->SetNext( NULL );
		return pLeafNew;
	}

	CUnicodeData_Node *pNode = GetNodeChildTop();
	if( pNode->IndexIsName() == false )return NULL;

	///// 最初の子供より若いとき
	if( GetNodeChildTop()->MoreName( name ) ){
		pLeafNew = new CUnicodeData_Leaf();
		pLeafNew->SetIndex( name );
		pLeafNew->SetNext( GetNodeChildTop() );
		GetNodeChildTop()->SetPrevious( pLeafNew );
		m_NodeChildTop = pLeafNew;
		return pLeafNew;
	}
	///// 最後の子供より老いているとき
	if( GetNodeChildBottom()->LessName( name ) ){
		pLeafNew = new CUnicodeData_Leaf();
		pLeafNew->SetIndex( name );
		pLeafNew->SetPrevious( GetNodeChildBottom() );
		GetNodeChildBottom()->SetNext( pLeafNew );
		m_NodeChildBottom = pLeafNew;
		return pLeafNew;
	}

	while( pNode != NULL )
	{
		///// 同じとき
		if( pNode->EqualName( name ) ){
			return (CUnicodeData_Leaf*)pNode;
		}
		if( pNode->MoreName( name ) ){
			pLeafNew = new CUnicodeData_Leaf();
			pLeafNew->SetIndex( name );
			CUnicodeData_Node *pNodePre = pNode->GetPrevious();				
			pNodePre->SetNext( pLeafNew );
			pLeafNew->SetPrevious( pNodePre );
			pLeafNew->SetNext( pNode );
			pNode->SetPrevious( pLeafNew );
			return pLeafNew;
		}
		pNode = pNode->GetNext();
	}
	return NULL;
}

CUnicodeData_Leaf *CUnicodeData_Node::SetLeafChild( int number )
{
	CUnicodeData_Leaf *pLeafNew;
	if( GetNodeChildTop() == NULL ){
		///// 子供がいない時
		pLeafNew = new CUnicodeData_Leaf();
		pLeafNew->SetIndex( number );
		m_NodeChildTop = pLeafNew;
		m_NodeChildBottom = pLeafNew;
		pLeafNew->SetPrevious( NULL );
		pLeafNew->SetNext( NULL );
		return pLeafNew;
	}

	CUnicodeData_Node *pNode = GetNodeChildTop();
	if( pNode->IndexIsNumber() == false )return NULL;

	///// キャッシュがあるならばまず使って検索
	pNode = SearchNodeChildByCache( number );
	if( pNode != NULL )return (CUnicodeData_Leaf*)pNode;

	///// 最初の子供より若いとき
	if( GetNodeChildTop()->MoreNumber( number ) ){
		pLeafNew = new CUnicodeData_Leaf();
		pLeafNew->SetIndex( number );
		pLeafNew->SetNext( GetNodeChildTop() );
		GetNodeChildTop()->SetPrevious( pLeafNew );
		m_NodeChildTop = pLeafNew;
		return pLeafNew;
	}
	///// 最後の子供より老いているとき
	if( GetNodeChildBottom()->LessNumber( number ) ){
		pLeafNew = new CUnicodeData_Leaf();
		pLeafNew->SetIndex( number );
		pLeafNew->SetPrevious( GetNodeChildBottom() );
		GetNodeChildBottom()->SetNext( pLeafNew );
		m_NodeChildBottom = pLeafNew;
		return pLeafNew;
	}

	while( pNode != NULL )
	{
		///// 同じとき
		if( pNode->EqualNumber( number ) ){
			return (CUnicodeData_Leaf*)pNode;
		}
		if( pNode->MoreNumber( number ) ){
			pLeafNew = new CUnicodeData_Leaf();
			pLeafNew->SetIndex( number );
			CUnicodeData_Node *pNodePre = pNode->GetPrevious();				
			pNodePre->SetNext( pLeafNew );
			pLeafNew->SetPrevious( pNodePre );
			pLeafNew->SetNext( pNode );
			pNode->SetPrevious( pLeafNew );
			return pLeafNew;
		}
		pNode = pNode->GetNext();
	}
	return NULL;
}

CUnicodeData_Leaf *CUnicodeData_Node::ScaningSetLeafChild_IndexName( wchar_t *str )
{
	wchar_t name[ 32 ], type[ 16 ];
	int cnt = 0;
	if( swscanf( str, L"%s %s %d:", name, type, &cnt ) < 2 )return NULL;
	if( wcscmp( type, L"CHAR" ) != 0 && cnt != 3 )return NULL;

	wchar_t *pStrData = ShiftNextColon( str );
	CUnicodeData_Leaf *pLeaf = SetLeafChild( name );
	if( pLeaf == NULL )return NULL;

	if( wcscmp( type, L"CHAR" )==0 ){
		pLeaf->SetData( name, pStrData );
	}else if( wcscmp( type, L"INT" )==0 ){
		long arrInt[ ARR_MAX ];
		if( cnt > ARR_MAX )cnt = ARR_MAX;
		if( cnt < 1 )return NULL;
		for( int c=0; c<cnt; c++ ){
			if( pStrData != NULL ){
				arrInt[ c ] = _wtol( pStrData );
			}else{
				arrInt[ c ] = 0; continue;
			}
			pStrData = ShiftNextSpace( pStrData );
		}
		pLeaf->SetData( name, cnt, arrInt );
	}else if( wcscmp( type, L"FLOAT" )==0 ){
		double arrFloat[ ARR_MAX ];
		if( cnt > ARR_MAX )cnt = ARR_MAX;
		if( cnt < 1 )return NULL;
		for( int c=0; c<cnt; c++ ){
			if( pStrData != NULL ){
				arrFloat[ c ] = _wtof( pStrData );
			}else{
				arrFloat[ c ] = 0.0; continue;
			}
			pStrData = ShiftNextSpace( pStrData );
		}
		pLeaf->SetData( name, cnt, arrFloat );
	}else if( wcscmp( type, L"BOOL" )==0 ){
		bool arrBool[ ARR_MAX ];
		if( cnt > ARR_MAX )cnt = ARR_MAX;
		if( cnt < 1 )return NULL;
		for( int c=0; c<cnt; c++ ){
			if( pStrData == NULL ){ arrBool[ c ] = false; continue; }

			if( pStrData[ 0 ] == L'T' || pStrData[ 0 ] == L'Y' || pStrData[ 0 ] == L't' || pStrData[ 0 ] == L'y' ){
				arrBool[ c ] = true;
			}else{
				arrBool[ c ] = false;
			}
			pStrData = ShiftNextSpace( pStrData );
		}
		pLeaf->SetData( name, cnt, arrBool );
	}
	return pLeaf;
}

/*
bool CUnicodeData_Node::AttachNodeChild( CUnicodeData_Node *nodeIn )
{
	if( GetNodeChildTop() == NULL ){
		///// 子供がいない時
		m_NodeChildTop = nodeIn;
		m_NodeChildBottom = nodeIn;
		nodeIn->SetPrevious( NULL );
		nodeIn->SetNext( NULL );
		return true;
	}else{
		CUnicodeData_Node *pNode = GetNodeChildTop();
		if( pNode->IndexIsSame( nodeIn ) == false )return false;

		if( nodeIn->IndexIsName() )
		{
			wchar_t *nameIn = nodeIn->GetName();

			///// 最初の子供より若いとき
			if( GetNodeChildTop()->MoreName( nameIn ) ){
				nodeIn->SetPrevious( NULL );
				nodeIn->SetNext( GetNodeChildTop() );
				GetNodeChildTop()->SetPrevious( nodeIn );
				m_NodeChildTop = nodeIn;
				return true;
			}
			///// 最後の子供より老いているとき
			if( GetNodeChildBottom()->LessName( nameIn ) ){
				nodeIn->SetNext( NULL );
				nodeIn->SetPrevious( GetNodeChildBottom() );
				GetNodeChildBottom()->SetNext( nodeIn );
				m_NodeChildBottom = nodeIn;
				return true;
			}

			while( pNode != NULL )
			{
				///// 同じとき
				if( pNode->EqualName( nameIn ) ){
					pNode->Copy( nodeIn );
					return true;
				}
				if( pNode->MoreName( nameIn ) ){
					CUnicodeData_Node *pNodePre = pNode->GetPrevious();				
					pNodePre->SetNext( nodeIn );
					nodeIn->SetPrevious( pNodePre );
					nodeIn->SetNext( pNode );
					pNode->SetPrevious( nodeIn );
					return true;
				}
				pNode = pNode->GetNext();
			}
		}
		if( nodeIn->IndexIsNumber() )
		{
			int numberIn = nodeIn->GetNumber();

			///// 最初の子供より若いとき
			if( GetNodeChildTop()->MoreNumber( numberIn ) ){
				nodeIn->SetPrevious( NULL );
				nodeIn->SetNext( GetNodeChildTop() );
				GetNodeChildTop()->SetPrevious( nodeIn );
				m_NodeChildTop = nodeIn;
				return true;
			}
			///// 最後の子供より老いているとき
			if( GetNodeChildBottom()->LessNumber( numberIn ) ){
				nodeIn->SetNext( NULL );
				nodeIn->SetPrevious( GetNodeChildBottom() );
				GetNodeChildBottom()->SetNext( nodeIn );
				m_NodeChildBottom = nodeIn;
				return true;
			}

			while( pNode != NULL )
			{
				///// 同じとき
				if( pNode->EqualNumber( numberIn ) ){
					pNode->Copy( nodeIn );
					return true;
				}
				if( pNode->MoreNumber( numberIn ) ){
					CUnicodeData_Node *pNodePre = pNode->GetPrevious();				
					pNodePre->SetNext( nodeIn );
					nodeIn->SetPrevious( pNodePre );
					nodeIn->SetNext( pNode );
					pNode->SetPrevious( nodeIn );
					return true;
				}
				pNode = pNode->GetNext();
			}
		}
	}
	return false;
}
*/

///////////////////////// L e a f /////////////////////////////
void CUnicodeData_Leaf::SetData( wchar_t *name, wchar_t *data ){
	m_TypeIndex = NAME;
	m_TypeData = CHAR;
	wcsncpy( m_Index.name, name, 32 );
	wcsncpy( m_Data.string, data, 128 );
	m_Cnt = (int)wcslen( m_Data.string );
}
void CUnicodeData_Leaf::SetData( wchar_t *name, int cnt, int *data ){
	m_TypeIndex = NAME;
	m_TypeData = INT;
	wcsncpy( m_Index.name, name, 32 );
	if( cnt > ARR_MAX )cnt = ARR_MAX;
	if( cnt < 0 )return;
	m_Cnt = cnt;
	for( int c=0; c<cnt; c++ ){ m_Data.arrInt[c]=(long)data[c]; }
}
void CUnicodeData_Leaf::SetData( wchar_t *name, int cnt, long *data ){
	m_TypeIndex = NAME;
	m_TypeData = INT;
	wcsncpy( m_Index.name, name, 32 );
	if( cnt > ARR_MAX )cnt = ARR_MAX;
	if( cnt < 0 )return;
	m_Cnt = cnt;
	memcpy( m_Data.arrInt, data, cnt*sizeof(long) );
}
void CUnicodeData_Leaf::SetData( wchar_t *name, int cnt, float *data ){
	m_TypeIndex = NAME;
	m_TypeData = FLOAT;
	wcsncpy( m_Index.name, name, 32 );
	if( cnt > ARR_MAX )cnt = ARR_MAX;
	if( cnt < 0 )return;
	m_Cnt = cnt;
	for( int c=0; c<cnt; c++ ){ m_Data.arrFloat[c] = (double)data[c]; }
}
void CUnicodeData_Leaf::SetData( wchar_t *name, int cnt, double *data ){
	m_TypeIndex = NAME;
	m_TypeData = FLOAT;
	wcsncpy( m_Index.name, name, 32 );
	if( cnt > ARR_MAX )cnt = ARR_MAX;
	if( cnt < 0 )return;
	m_Cnt = cnt;
	memcpy( m_Data.arrFloat, data, cnt*sizeof(double) );
}
void CUnicodeData_Leaf::SetData( wchar_t *name, int cnt, bool *data ){
	m_TypeIndex = NAME;
	m_TypeData = BOOL;
	wcsncpy( m_Index.name, name, 32 );
	if( cnt > ARR_MAX )cnt = ARR_MAX;
	if( cnt < 0 )return;
	m_Cnt = cnt;
	memcpy( m_Data.arrBool, data, cnt*sizeof(bool) );
}
void CUnicodeData_Leaf::SetData( int number, wchar_t *data ){
	m_TypeIndex = NUMBER;
	m_TypeData = CHAR;
	m_Index.number = number;
	wcsncpy( m_Data.string, data, 128 );
	m_Cnt = (int)wcslen( m_Data.string );
}
void CUnicodeData_Leaf::SetData( int number, int cnt, int *data ){
	m_TypeIndex = NUMBER;
	m_TypeData = INT;
	m_Index.number = number;
	if( cnt > ARR_MAX )cnt = ARR_MAX;
	if( cnt < 0 )return;
	m_Cnt = cnt;
	for( int c=0; c<cnt; c++ ){ m_Data.arrInt[c] = (long)data[c]; }
}
void CUnicodeData_Leaf::SetData( int number, int cnt, long *data ){
	m_TypeIndex = NUMBER;
	m_TypeData = INT;
	m_Index.number = number;
	if( cnt > ARR_MAX )cnt = ARR_MAX;
	if( cnt < 0 )return;
	m_Cnt = cnt;
	memcpy( m_Data.arrInt, data, cnt*sizeof(long) );
}
void CUnicodeData_Leaf::SetData( int number, int cnt, float *data ){
	m_TypeIndex = NUMBER;
	m_TypeData = FLOAT;
	m_Index.number = number;
	if( cnt > ARR_MAX )cnt = ARR_MAX;
	if( cnt < 0 )return;
	m_Cnt = cnt;
	for( int c=0; c<cnt; c++ ){ m_Data.arrFloat[c] = (double)data[c]; }
}
void CUnicodeData_Leaf::SetData( int number, int cnt, double *data ){
	m_TypeIndex = NUMBER;
	m_TypeData = FLOAT;
	m_Index.number = number;
	if( cnt > ARR_MAX )cnt = ARR_MAX;
	if( cnt < 0 )return;
	m_Cnt = cnt;
	memcpy( m_Data.arrFloat, data, cnt*sizeof(double) );
}
void CUnicodeData_Leaf::SetData( int number, int cnt, bool *data ){
	m_TypeIndex = NUMBER;
	m_TypeData = BOOL;
	m_Index.number = number;
	if( cnt > ARR_MAX )cnt = ARR_MAX;
	if( cnt < 0 )return;
	m_Cnt = cnt;
	memcpy( m_Data.arrBool, data, cnt*sizeof(bool) );
}

int CUnicodeData_Leaf::GetData( wchar_t *data )
{
	if( m_TypeData != CHAR )return 0;
	wcsncpy( data, m_Data.string, m_Cnt +1 );
	return m_Cnt;
}
int CUnicodeData_Leaf::GetData( int *data )
{
	if( m_TypeData != INT )return 0;
	for( int c=0; c<m_Cnt; c++ ){ data[c] = (int)m_Data.arrInt[c]; }
	return m_Cnt;
}
int CUnicodeData_Leaf::GetData( long *data )
{
	if( m_TypeData != INT )return 0;
	memcpy( data, m_Data.arrInt, m_Cnt * sizeof(long) );
	return m_Cnt;
}
int CUnicodeData_Leaf::GetData( float *data )
{
	if( m_TypeData != FLOAT )return 0;
	for( int c=0; c<m_Cnt; c++ ){ data[c] = (float)m_Data.arrFloat[c]; }
	return m_Cnt;
}
int CUnicodeData_Leaf::GetData( double *data )
{
	if( m_TypeData != FLOAT )return 0;
	memcpy( data, m_Data.arrFloat, m_Cnt * sizeof(double) );
	return m_Cnt;
}
int CUnicodeData_Leaf::GetData( bool *data )
{
	if( m_TypeData != BOOL )return 0;
	memcpy( data, m_Data.arrBool, m_Cnt * sizeof(bool) );
	return m_Cnt;
}

void CUnicodeData_Leaf::PrintData( FILE *file )
{
	char line[ 1024 ];
	wchar_t wline[ 1024 ];

	if( m_TypeIndex == NAME ){
		swprintf( wline, L" %s", m_Index.name );
		wcstombs( line, wline, 1024 );
		fputs( line, file );
	}else if( m_TypeIndex == NUMBER ){
		fprintf( file, " %d", m_Index.number );
	}

	if( m_TypeData == CHAR ){
		swprintf( wline, L" CHAR %d:%s", m_Cnt, m_Data.string );
		wcstombs( line, wline, 1024 );
		fputs( line, file );
	}else if( m_TypeData == INT ){
		fprintf( file, " INT %ld:%ld", m_Cnt, m_Data.arrInt[ 0 ] );
		for( int c=1; c<m_Cnt; c++ ){
			fprintf( file, " %ld", m_Data.arrInt[ c ] );
		}
	}else if( m_TypeData == FLOAT ){
		fprintf( file, " FLOAT %d:%5.5f", m_Cnt, m_Data.arrFloat[ 0 ] );
		for( int c=1; c<m_Cnt; c++ ){
			fprintf( file, " %5.5f", m_Data.arrFloat[ c ] );
		}
	}else if( m_TypeData == BOOL ){
		fprintf( file, " BOOL %d:", m_Cnt );
		if( m_Data.arrBool[ 0 ] ){ fprintf( file, "True" ); }else{ fprintf( file, "False" ); }
		for( int c=1; c<m_Cnt; c++ ){
			if( m_Data.arrBool[ c ] ){ fprintf( file, " True" ); }else{ fprintf( file, " False" ); }
		}
	}

	fprintf( file, "\n" );
}

/*
void CUnicodeData_Leaf::ScanIndexNameString( wchar_t *str )
{
	wchar_t name[ 32 ], type[ 16 ];
	int cnt = 0;
	if( swscanf( str, L"%s %s %d:", name, type, &cnt ) != 3 )return;

	wchar_t *pStrData = ShiftNextColon( str );

	if( wcscmp( type, L"CHAR" )==0 ){
		m_TypeIndex = NAME;
		m_TypeData = CHAR;
		wcsncpy( m_Index.name, name, 32 );
		if( cnt < 0 )cnt = 128;
		wcsncpy( m_Data.string, pStrData, cnt );
		m_Cnt = (int)strlen( m_Data.string );
	}else if( wcscmp( type, L"INT" )==0 ){
		m_TypeIndex = NAME;
		m_TypeData = INT;
		wcsncpy( m_Index.name, name, 32 );
		if( cnt > ARR_MAX )cnt = ARR_MAX;
		if( cnt < 1 )return;
		for( int c=0; c<cnt; c++ ){
			if( pStrData != NULL ){
				m_Data.arrInt[ c ] = atoi( pStrData );
			}else{
				m_Data.arrInt[ c ] = 0;
			}
			pStrData = ShiftNextSpace( pStrData );
		}
		m_Cnt = cnt;
	}else if( wcscmp( type, L"FLOAT" )==0 ){
		m_TypeIndex = NAME;
		m_TypeData = FLOAT;
		wcsncpy( m_Index.name, name, 32 );
		if( cnt > ARR_MAX )cnt = ARR_MAX;
		if( cnt < 1 )return;
		for( int c=0; c<cnt; c++ ){
			if( pStrData != NULL ){
				m_Data.arrFloat[ c ] = (float)atof( pStrData );
			}else{
				m_Data.arrFloat[ c ] = 0.0;
			}
			pStrData = ShiftNextSpace( pStrData );
		}
		m_Cnt = cnt;
	}
}
*/
/////////////////////////////////////////////////
void CUnicodeData::SetNameSpace( wchar_t *name )
{
	m_NodeNameSpaceNow = m_NodeTop.SetNodeChild( name );
}

void CUnicodeData::GetNameSpace( int number )
{
	m_NodeNameSpaceNow = m_NodeTop.SetNodeChild( number );
}

void CUnicodeData::Set(int num, wchar_t *name, int cnt, int *dataIn)
{
	CUnicodeData_Node *pNode;
	CUnicodeData_Leaf *pLeaf;
	pNode = m_NodeNameSpaceNow->SetNodeChild( num );
	if( pNode == NULL )return;
	pLeaf = pNode->SetLeafChild( name );
	pLeaf->SetData( name, cnt, dataIn );
}

void CUnicodeData::Set(int num, wchar_t *name, int cnt, long *dataIn)
{
	CUnicodeData_Node *pNode;
	CUnicodeData_Leaf *pLeaf;
	pNode = m_NodeNameSpaceNow->SetNodeChild( num );
	if( pNode == NULL )return;
	pLeaf = pNode->SetLeafChild( name );
	pLeaf->SetData( name, cnt, dataIn );
}

void CUnicodeData::Set(int num, wchar_t *name, int cnt, float *dataIn)
{
	CUnicodeData_Node *pNode;
	CUnicodeData_Leaf *pLeaf;
	pNode = m_NodeNameSpaceNow->SetNodeChild( num );
	if( pNode == NULL )return;
	pLeaf = pNode->SetLeafChild( name );
	pLeaf->SetData( name, cnt, dataIn );
}

void CUnicodeData::Set(int num, wchar_t *name, int cnt, double *dataIn)
{
	CUnicodeData_Node *pNode;
	CUnicodeData_Leaf *pLeaf;
	pNode = m_NodeNameSpaceNow->SetNodeChild( num );
	if( pNode == NULL )return;
	pLeaf = pNode->SetLeafChild( name );
	pLeaf->SetData( name, cnt, dataIn );
}

void CUnicodeData::Set(int num, wchar_t *name, wchar_t *dataIn)
{
	CUnicodeData_Node *pNode;
	CUnicodeData_Leaf *pLeaf;
	pNode = m_NodeNameSpaceNow->SetNodeChild( num );
	if( pNode == NULL )return;
	pLeaf = pNode->SetLeafChild( name );
	pLeaf->SetData( name, dataIn );
}

void CUnicodeData::Set(int num, wchar_t *name, int cnt, bool *dataIn)
{
	CUnicodeData_Node *pNode;
	CUnicodeData_Leaf *pLeaf;
	pNode = m_NodeNameSpaceNow->SetNodeChild( num );
	if( pNode == NULL )return;
	pLeaf = pNode->SetLeafChild( name );
	pLeaf->SetData( name, cnt, dataIn );
}

int CUnicodeData::Get(int num, wchar_t *name, int *dataOut)
{
	CUnicodeData_Node *pNode;
	CUnicodeData_Leaf *pLeaf;
	pNode = m_NodeNameSpaceNow->SearchNodeChild( num );
	if( pNode == NULL )return 0;
	pLeaf = (CUnicodeData_Leaf*)pNode->SearchNodeChild( name );
	if( pLeaf == NULL )return 0;
	return pLeaf->GetData( dataOut );
}

int CUnicodeData::Get(int num, wchar_t *name, long *dataOut)
{
	CUnicodeData_Node *pNode;
	CUnicodeData_Leaf *pLeaf;
	pNode = m_NodeNameSpaceNow->SearchNodeChild( num );
	if( pNode == NULL )return 0;
	pLeaf = (CUnicodeData_Leaf*)pNode->SearchNodeChild( name );
	if( pLeaf == NULL )return 0;
	return pLeaf->GetData( dataOut );
}

int CUnicodeData::Get(int num, wchar_t *name, float *dataOut)
{
	CUnicodeData_Node *pNode;
	CUnicodeData_Leaf *pLeaf;
	pNode = m_NodeNameSpaceNow->SearchNodeChild( num );
	if( pNode == NULL )return 0;
	pLeaf = (CUnicodeData_Leaf*)pNode->SearchNodeChild( name );
	if( pLeaf == NULL )return 0;
	return pLeaf->GetData( dataOut );
}

int CUnicodeData::Get(int num, wchar_t *name, double *dataOut)
{
	CUnicodeData_Node *pNode;
	CUnicodeData_Leaf *pLeaf;
	pNode = m_NodeNameSpaceNow->SearchNodeChild( num );
	if( pNode == NULL )return 0;
	pLeaf = (CUnicodeData_Leaf*)pNode->SearchNodeChild( name );
	if( pLeaf == NULL )return 0;
	return pLeaf->GetData( dataOut );
}

int CUnicodeData::Get(int num, wchar_t *name, wchar_t *dataOut)
{
	CUnicodeData_Node *pNode;
	CUnicodeData_Leaf *pLeaf;
	pNode = m_NodeNameSpaceNow->SearchNodeChild( num );
	if( pNode == NULL )return 0;
	pLeaf = (CUnicodeData_Leaf*)pNode->SearchNodeChild( name );
	if( pLeaf == NULL )return 0;
	return pLeaf->GetData( dataOut );
}

int CUnicodeData::Get(int num, wchar_t *name, bool *dataOut)
{
	CUnicodeData_Node *pNode;
	CUnicodeData_Leaf *pLeaf;
	pNode = m_NodeNameSpaceNow->SearchNodeChild( num );
	if( pNode == NULL )return 0;
	pLeaf = (CUnicodeData_Leaf*)pNode->SearchNodeChild( name );
	if( pLeaf == NULL )return 0;
	return pLeaf->GetData( dataOut );
}

bool CUnicodeData::Save(wchar_t *fileName)
{
	setlocale( LC_ALL, "jpn" );

	FILE *file = _wfopen( fileName, L"w" );
	if( file == NULL )return false;

	CUnicodeData_Node *pNode1 = m_NodeTop.GetNodeChildTop(), *pNode2;
	CUnicodeData_Leaf *pLeaf;
	while( pNode1 != NULL )
	{	///// ネーム・スペース
		if( pNode1->GetName()[ 0 ] == 0 ){
			fprintf( file, "#no namespace\n" );
		}else{
			char line[ 1024 ];
			wchar_t wline[ 1024 ];
			swprintf( wline, L"namespace %s{\n", pNode1->GetName() );
			wcstombs( line, wline, 1024 );
			fputs( line, file );
		}

		pNode2 = pNode1->GetNodeChildTop();
		while( pNode2 != NULL )
		{	///// インデックス番号
			int indexNode2 = pNode2->GetNumber();
			pLeaf = (CUnicodeData_Leaf*)pNode2->GetNodeChildTop();
			while( pLeaf != NULL ){
				fprintf( file, "%d", indexNode2 );
				pLeaf->PrintData( file );
				pLeaf = (CUnicodeData_Leaf*)pLeaf->GetNext();
			}
			pNode2 = pNode2->GetNext();
		}

		if( pNode1->GetName()[ 0 ] == 0 ){
			fprintf( file, "\n" );
		}else{
			fprintf( file, "}\n" );
		}

		pNode1 = pNode1->GetNext();
	}

	fclose( file );
	return true;
}

bool CUnicodeData::Load(wchar_t *fileName)
{
	setlocale( LC_ALL, "jpn" );

	FILE *file = _wfopen( fileName, L"r" );
	if( file == NULL )return false;

	char line[ 1024 ];
	int level = 0;
	wchar_t wline[ 1024 ], *pC;
	while( fgets( line, 1024, file ) != NULL )
	{
		if( mbstowcs( wline, line, 1024 ) == 0 )continue;///// マルチバイトからワイド文字へ
		pC = wline;
		while( *pC != 0 ){
			if( *pC == L'{' ){ level++; }
			if( *pC == L'}' ){
				level--;
				if( level == 0 ){
					m_NodeNameSpaceNow = m_NodeTop.SetNodeChild( L"" );
				}
			}
			if( *pC == L'#' ){ *pC = 0; break; }
			if( *pC == L'\n' ){ *pC = 0; break; }
			pC++;
		}
		if( line[ 0 ] == 0 )continue;

		///// ネーム・スペース
		wchar_t nameClass[ 64 ], namePrivate[ 64 ];
		nameClass[ 0 ] = 0;
		namePrivate[ 0 ] = 0;
		swscanf( wline, L"%s %s", nameClass, namePrivate );
		if( wcscmp( nameClass, L"namespace" ) == 0 ){
			m_NodeNameSpaceNow = m_NodeTop.SetNodeChild( namePrivate );
			continue;
		}

		///// ノード
		int number = 0;
		if( swscanf( wline, L"%d", &number ) > 0 )
		{
			CUnicodeData_Node *pNode = m_NodeNameSpaceNow->SetNodeChild( number );
			if( pNode == NULL )continue;

			pC = wline;
			while( *pC != L' ' && *pC != 0 )pC++;
			pNode->ScaningSetLeafChild_IndexName( pC );
		}
	}

	fclose( file );
	return true;
}

bool CUnicodeData::Retrieve(wchar_t *str)
{
	if( str == NULL )return false;

	wchar_t line[ 1024 ];
	wchar_t *strBreak, *pC;
	int level = 0;
	while( ( strBreak = wcschr( str, L'\n' ) ) != NULL )
	{
		int len = (int)(strBreak - str);
		if( len < 1 ){ str++; continue; }
		if( len > 1024 ){ len = 1024; }
		wcsncpy( line, str, len );

		pC = line;
		while( *pC != 0 ){
			if( *pC == L'{' ){ level++; }
			if( *pC == L'}' ){
				level--;
				if( level == 0 ){
					m_NodeNameSpaceNow = m_NodeTop.SetNodeChild( L"" );
				}
			}
			if( *pC == L'#' ){ *pC = 0; break; }
			if( *pC == L'\n' ){ *pC = 0; break; }
			pC++;
		}
		if( line[ 0 ] == 0 )continue;

		///// ネーム・スペース
		wchar_t nameClass[ 64 ], namePrivate[ 64 ];
		nameClass[ 0 ] = 0;
		namePrivate[ 0 ] = 0;
		swscanf( line, L"%s %s", nameClass, namePrivate );
		if( wcscmp( nameClass, L"namespace" ) == 0 ){
			m_NodeTop.SetNodeChild( namePrivate );
			continue;
		}

		///// ノード
		int number = 0;
		if( swscanf( line, L"%d", &number ) > 0 )
		{
			CUnicodeData_Node *pNode = m_NodeNameSpaceNow->SetNodeChild( number );
			if( pNode == NULL )continue;

			pC = line;
			while( *pC != L' ' && *pC != 0 )pC++;
			pNode->ScaningSetLeafChild_IndexName( pC );
		}

		str = ++strBreak;///// 次の行に移動
	}

	return true;
}

bool CUnicodeData::ForeachStart( int num )
{
	CUnicodeData_Node *pNode;
	pNode = m_NodeNameSpaceNow->SearchNodeChild( num );
	if( pNode == NULL )return false;
	m_LeafIterate = (CUnicodeData_Leaf*)pNode->GetNodeChildTop();
	if( m_LeafIterate == NULL )return false;
	return true;
}

int CUnicodeData::ForeachGet( wchar_t *nameOut, void *dataOut, wchar_t *typeOut )
{
	if( m_LeafIterate == NULL )return 0;
	int cnt = m_LeafIterate->GetCnt();
	if( cnt < 1 )return 0;
	
	wcsncpy( nameOut, m_LeafIterate->GetName(), 32 );

	if( m_LeafIterate->DataIsCHAR() ){
		if( typeOut != NULL )wcsncpy( typeOut, L"CHAR", 8 );
		m_LeafIterate->GetData( (wchar_t*)dataOut );
	}else if( m_LeafIterate->DataIsINT() ){
		if( typeOut != NULL )wcsncpy( typeOut, L"INT", 8 );
		m_LeafIterate->GetData( (int*)dataOut );
	}else if( m_LeafIterate->DataIsFLOAT() ){
		if( typeOut != NULL )wcsncpy( typeOut, L"FLOAT", 8 );
		m_LeafIterate->GetData( (float*)dataOut );
	}else if( m_LeafIterate->DataIsBOOL() ){
		if( typeOut != NULL )wcsncpy( typeOut, L"BOOL", 8 );
		m_LeafIterate->GetData( (bool*)dataOut );
	}

	return cnt;
}
