#include "StdAfx.h"
#include "Ch3PlainData.h"

/*
	@brief キャッシュを作る、今はNumberのみ
*/
void Ch3PlainData_Node::MakeCache()
{
	if( GetNodeChildTop() == NULL || GetNodeChildBottom() == NULL )return;
	if( GetNodeChildTop()->IndexIsNumber() == false )return;

	int numberMax;
	m_CacheNumberMin = GetNodeChildTop()->GetNumber();
	numberMax = GetNodeChildBottom()->GetNumber();
	m_CacheNumberCnt = numberMax - m_CacheNumberMin +1;
	if( m_CacheNumberCnt < 1 )return;

	if( m_CacheNumber != NULL )delete [] m_CacheNumber;
	m_CacheNumber = new Ch3PlainData_Node* [ m_CacheNumberCnt ];
	for( int c=0; c<m_CacheNumberCnt; c++ ){ m_CacheNumber[ c ] = NULL; }
	
	Ch3PlainData_Node *pNode = GetNodeChildTop();
	while( pNode != NULL )
	{
		int numberCur = pNode->GetNumber() - m_CacheNumberMin;
		if( m_CacheNumber[ numberCur ] == NULL )m_CacheNumber[ numberCur ] = pNode;
		pNode = pNode->GetNext();
	}
}

void Ch3PlainData_Node::MakeCache_Recursive()
{
	Ch3PlainData_Node *pNode = GetNodeChildTop();
	while( pNode != NULL )
	{
		if( pNode->GetNodeChildTop() != NULL ){
			pNode->MakeCache();
			pNode->MakeCache_Recursive();
		}
		pNode = pNode->GetNext();
	}
}

Ch3PlainData_Node *Ch3PlainData_Node::SearchNodeChildByCache( int number )
{
	if( m_CacheNumber == NULL )return NULL;
	if( GetNodeChildTop()->IndexIsNumber() == false )return NULL;
	if( number < m_CacheNumberMin || number >= m_CacheNumberMin +m_CacheNumberCnt )return NULL;

	return m_CacheNumber[ number -m_CacheNumberMin ];
}

Ch3PlainData_Node *Ch3PlainData_Node::SearchNodeChild( char *name )
{
	if( GetNodeChildTop() == NULL || GetNodeChildBottom() == NULL )return NULL;
	if( GetNodeChildTop()->IndexIsName() == false )return NULL;

	Ch3PlainData_Node *pNode = GetNodeChildTop();
	while( pNode != NULL )
	{
		if( pNode->EqualName( name ) )return pNode;
		if( pNode->MoreName( name ) )return NULL;
		pNode = pNode->GetNext();
	}
	return NULL;
}

Ch3PlainData_Node *Ch3PlainData_Node::SearchNodeChild( int number )
{
	if( GetNodeChildTop() == NULL || GetNodeChildBottom() == NULL )return NULL;
	if( GetNodeChildTop()->IndexIsNumber() == false )return NULL;

	Ch3PlainData_Node *pNode = SearchNodeChildByCache( number );
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

Ch3PlainData_Node *Ch3PlainData_Node::SetNodeChild( char *name )
{
	Ch3PlainData_Node *pNodeNew;

	///// 子供がいないとき
	if( GetNodeChildTop() == NULL ){
		pNodeNew = new Ch3PlainData_Node();
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
		pNodeNew = new Ch3PlainData_Node();
		pNodeNew->SetIndex( name );
		pNodeNew->SetPrevious( NULL );
		pNodeNew->SetNext( GetNodeChildTop() );
		GetNodeChildTop()->SetPrevious( pNodeNew );
		m_NodeChildTop = pNodeNew;
		return pNodeNew;
	}
	///// 最後の子供より老いているとき
	if( GetNodeChildBottom()->LessName( name ) ){
		pNodeNew = new Ch3PlainData_Node();
		pNodeNew->SetIndex( name );
		pNodeNew->SetNext( NULL );
		pNodeNew->SetPrevious( GetNodeChildBottom() );
		GetNodeChildBottom()->SetNext( pNodeNew );
		m_NodeChildBottom = pNodeNew;
		return pNodeNew;
	}

	///// 普通に検索
	Ch3PlainData_Node *pNode = GetNodeChildTop();
	while( pNode != NULL )
	{
		///// 同じとき
		if( pNode->EqualName( name ) ){
			return pNode;
		}
		if( pNode->MoreName( name ) ){
			pNodeNew = new Ch3PlainData_Node();
			pNodeNew->SetIndex( name );
			Ch3PlainData_Node *pNodePre = pNode->GetPrevious();				
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

Ch3PlainData_Node *Ch3PlainData_Node::SetNodeChild( int number )
{
	Ch3PlainData_Node *pNodeNew;

	///// 子供がいないとき
	if( GetNodeChildTop() == NULL ){
		pNodeNew = new Ch3PlainData_Node();
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
	Ch3PlainData_Node *pNode = SearchNodeChildByCache( number );
	if( pNode != NULL )return pNode;

	///// 最初の子供より若いとき
	if( GetNodeChildTop()->MoreNumber( number ) ){
		pNodeNew = new Ch3PlainData_Node();
		pNodeNew->SetIndex( number );
		pNodeNew->SetPrevious( NULL );
		pNodeNew->SetNext( GetNodeChildTop() );
		GetNodeChildTop()->SetPrevious( pNodeNew );
		m_NodeChildTop = pNodeNew;
		return pNodeNew;
	}
	///// 最後の子供より老いているとき
	if( GetNodeChildBottom()->LessNumber( number ) ){
		pNodeNew = new Ch3PlainData_Node();
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
			pNodeNew = new Ch3PlainData_Node();
			pNodeNew->SetIndex( number );
			Ch3PlainData_Node *pNodePre = pNode->GetPrevious();				
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

Ch3PlainData_Leaf *Ch3PlainData_Node::SetLeafChild( char *name )
{
	Ch3PlainData_Leaf *pLeafNew;
	if( GetNodeChildTop() == NULL ){
		///// 子供がいない時
		pLeafNew = new Ch3PlainData_Leaf();
		pLeafNew->SetIndex( name );
		m_NodeChildTop = pLeafNew;
		m_NodeChildBottom = pLeafNew;
		pLeafNew->SetPrevious( NULL );
		pLeafNew->SetNext( NULL );
		return pLeafNew;
	}

	Ch3PlainData_Node *pNode = GetNodeChildTop();
	if( pNode->IndexIsName() == false )return NULL;

	///// 最初の子供より若いとき
	if( GetNodeChildTop()->MoreName( name ) ){
		pLeafNew = new Ch3PlainData_Leaf();
		pLeafNew->SetIndex( name );
		pLeafNew->SetNext( GetNodeChildTop() );
		GetNodeChildTop()->SetPrevious( pLeafNew );
		m_NodeChildTop = pLeafNew;
		return pLeafNew;
	}
	///// 最後の子供より老いているとき
	if( GetNodeChildBottom()->LessName( name ) ){
		pLeafNew = new Ch3PlainData_Leaf();
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
			return (Ch3PlainData_Leaf*)pNode;
		}
		if( pNode->MoreName( name ) ){
			pLeafNew = new Ch3PlainData_Leaf();
			pLeafNew->SetIndex( name );
			Ch3PlainData_Node *pNodePre = pNode->GetPrevious();				
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

Ch3PlainData_Leaf *Ch3PlainData_Node::SetLeafChild( int number )
{
	Ch3PlainData_Leaf *pLeafNew;
	if( GetNodeChildTop() == NULL ){
		///// 子供がいない時
		pLeafNew = new Ch3PlainData_Leaf();
		pLeafNew->SetIndex( number );
		m_NodeChildTop = pLeafNew;
		m_NodeChildBottom = pLeafNew;
		pLeafNew->SetPrevious( NULL );
		pLeafNew->SetNext( NULL );
		return pLeafNew;
	}

	Ch3PlainData_Node *pNode = GetNodeChildTop();
	if( pNode->IndexIsNumber() == false )return NULL;

	///// キャッシュがあるならばまず使って検索
	pNode = SearchNodeChildByCache( number );
	if( pNode != NULL )return (Ch3PlainData_Leaf*)pNode;

	///// 最初の子供より若いとき
	if( GetNodeChildTop()->MoreNumber( number ) ){
		pLeafNew = new Ch3PlainData_Leaf();
		pLeafNew->SetIndex( number );
		pLeafNew->SetNext( GetNodeChildTop() );
		GetNodeChildTop()->SetPrevious( pLeafNew );
		m_NodeChildTop = pLeafNew;
		return pLeafNew;
	}
	///// 最後の子供より老いているとき
	if( GetNodeChildBottom()->LessNumber( number ) ){
		pLeafNew = new Ch3PlainData_Leaf();
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
			return (Ch3PlainData_Leaf*)pNode;
		}
		if( pNode->MoreNumber( number ) ){
			pLeafNew = new Ch3PlainData_Leaf();
			pLeafNew->SetIndex( number );
			Ch3PlainData_Node *pNodePre = pNode->GetPrevious();				
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

Ch3PlainData_Leaf *Ch3PlainData_Node::ScaningSetLeafChild_IndexName( char *str )
{
	char *pStrData = ShiftNextColon( str );
	if( pStrData == NULL ) return NULL;
	pStrData[ -1 ] = 0;

	char name[ 32 ], type[ 16 ];
	int cnt = 0;
	if( sscanf( str, "%s %s %d:", name, type, &cnt ) < 2 ) return NULL;

	Ch3PlainData_Leaf *pLeaf = SetLeafChild( name );
	if( pLeaf == NULL ) return NULL;

	if( strcmp( type, "CHAR" )==0 || strcmp( type, "C" )==0 ){
		pLeaf->SetData( name, pStrData );
	}else if( strcmp( type, "INT" )==0 || strcmp( type, "I" )==0 ){
		int arrInt[ ARR_MAX ];
		if( cnt > ARR_MAX )cnt = ARR_MAX;
		if( cnt < 1 )return NULL;
		for( int c=0; c<cnt; c++ ){
			if( pStrData != NULL ){
				arrInt[ c ] = atoi( pStrData );
			}else{
				arrInt[ c ] = 0; continue;
			}
			pStrData = ShiftNextSpace( pStrData );
		}
		pLeaf->SetData( name, cnt, arrInt );
	}else if( strcmp( type, "FLOAT" )==0 || strcmp( type, "F" )==0 ){
		float arrFloat[ ARR_MAX ];
		if( cnt > ARR_MAX )cnt = ARR_MAX;
		if( cnt < 1 )return NULL;
		for( int c=0; c<cnt; c++ ){
			if( pStrData != NULL ){
				arrFloat[ c ] = (float)atof( pStrData );
			}else{
				arrFloat[ c ] = 0.0f; continue;
			}
			pStrData = ShiftNextSpace( pStrData );
		}
		pLeaf->SetData( name, cnt, arrFloat );
	}else if( strcmp( type, "BOOL" )==0 || strcmp( type, "B" )==0 ){
		bool arrBool[ ARR_MAX ];
		if( cnt > ARR_MAX )cnt = ARR_MAX;
		if( cnt < 1 )return NULL;
		for( int c=0; c<cnt; c++ ){
			if( pStrData == NULL ){ arrBool[ c ] = false; continue; }

			if( pStrData[ 0 ] == 'T' || pStrData[ 0 ] == 'Y' || pStrData[ 0 ] == 't' || pStrData[ 0 ] == 'y' ){
				arrBool[ c ] = true;
			}else{
				arrBool[ c ] = false;
			}
			pStrData = ShiftNextSpace( pStrData );
		}
		pLeaf->SetData( name, cnt, arrBool );
	}else if( strcmp( type, "DEL" )==0 ){
		pLeaf->DelData();
	}
	return pLeaf;
}

Ch3PlainData_Leaf *Ch3PlainData_Node::CopyLeafChild( Ch3PlainData_Leaf *leaf )
{
	Ch3PlainData_Leaf *leafNew = NULL;
	if( leaf->IndexIsName() ){
		leafNew->SetLeafChild( leaf->GetName() );
	}else if( leaf->IndexIsNumber() ){
		leafNew->SetLeafChild( leaf->GetNumber() );
	}
	if( leafNew == NULL ) return NULL;

	leafNew->Copy( leaf );
	return leafNew;
}

/*
bool Ch3PlainData_Node::AttachNodeChild( Ch3PlainData_Node *nodeIn )
{
	if( GetNodeChildTop() == NULL ){
		///// 子供がいない時
		m_NodeChildTop = nodeIn;
		m_NodeChildBottom = nodeIn;
		nodeIn->SetPrevious( NULL );
		nodeIn->SetNext( NULL );
		return true;
	}else{
		Ch3PlainData_Node *pNode = GetNodeChildTop();
		if( pNode->IndexIsSame( nodeIn ) == false )return false;

		if( nodeIn->IndexIsName() )
		{
			char *nameIn = nodeIn->GetName();

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
					Ch3PlainData_Node *pNodePre = pNode->GetPrevious();				
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
					Ch3PlainData_Node *pNodePre = pNode->GetPrevious();				
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
void Ch3PlainData_Leaf::SetData( char *name, char *data ){
	m_TypeIndex = NAME;
	m_TypeData = CHAR;
	strncpy( m_Index.name, name, 32 );
	if( data == NULL ){ m_Data.string[ 0 ]=0; m_Cnt=0; return; }
	strncpy( m_Data.string, data, 128 );
	m_Cnt = (int)strlen( m_Data.string );
}
void Ch3PlainData_Leaf::SetData( char *name, int cnt, int *data ){
	m_TypeIndex = NAME;
	m_TypeData = INT;
	strncpy( m_Index.name, name, 32 );
	if( cnt > ARR_MAX )cnt = ARR_MAX;
	if( cnt < -1 )return;
	m_Cnt = cnt;
	if( cnt > 0 ) memcpy( m_Data.arrInt, data, cnt*sizeof(int) );
}
void Ch3PlainData_Leaf::SetData( char *name, int cnt, float *data ){
	m_TypeIndex = NAME;
	m_TypeData = FLOAT;
	strncpy( m_Index.name, name, 32 );
	if( cnt > ARR_MAX )cnt = ARR_MAX;
	if( cnt < -1 )return;
	m_Cnt = cnt;
	if( cnt > 0 ) memcpy( m_Data.arrFloat, data, cnt*sizeof(float) );
}
void Ch3PlainData_Leaf::SetData( char *name, int cnt, double *data ){
	m_TypeIndex = NAME;
	m_TypeData = FLOAT;
	strncpy( m_Index.name, name, 32 );
	if( cnt > ARR_MAX )cnt = ARR_MAX;
	if( cnt < -1 )return;
	m_Cnt = cnt;
	for( int c=0; c<cnt; c++ ){ m_Data.arrFloat[c] = (float)data[c]; }
}
void Ch3PlainData_Leaf::SetData( char *name, int cnt, bool *data ){
	m_TypeIndex = NAME;
	m_TypeData = BOOL;
	strncpy( m_Index.name, name, 32 );
	if( cnt > ARR_MAX )cnt = ARR_MAX;
	if( cnt < -1 ) return;
	m_Cnt = cnt;
	if( cnt > 0 ) memcpy( m_Data.arrBool, data, cnt*sizeof(bool) );
}
void Ch3PlainData_Leaf::SetData( int number, char *data ){
	m_TypeIndex = NUMBER;
	m_TypeData = CHAR;
	m_Index.number = number;
	if( data == NULL ){ m_Data.string[ 0 ]=0; m_Cnt=0; return; }
	strncpy( m_Data.string, data, 128 );
	m_Cnt = (int)strlen( m_Data.string );
}
void Ch3PlainData_Leaf::SetData( int number, int cnt, int *data ){
	m_TypeIndex = NUMBER;
	m_TypeData = INT;
	m_Index.number = number;
	if( cnt > ARR_MAX )cnt = ARR_MAX;
	if( cnt < -1 )return;
	m_Cnt = cnt;
	if( cnt > 0 ) memcpy( m_Data.arrInt, data, cnt*sizeof(int) );
}
void Ch3PlainData_Leaf::SetData( int number, int cnt, float *data ){
	m_TypeIndex = NUMBER;
	m_TypeData = FLOAT;
	m_Index.number = number;
	if( cnt > ARR_MAX )cnt = ARR_MAX;
	if( cnt < -1 )return;
	m_Cnt = cnt;
	if( cnt > 0 ) memcpy( m_Data.arrFloat, data, cnt*sizeof(float) );
}
void Ch3PlainData_Leaf::SetData( int number, int cnt, double *data ){
	m_TypeIndex = NUMBER;
	m_TypeData = FLOAT;
	m_Index.number = number;
	if( cnt > ARR_MAX )cnt = ARR_MAX;
	if( cnt < -1 )return;
	m_Cnt = cnt;
	for( int c=0; c<cnt; c++ ){ m_Data.arrFloat[c] = (float)data[c]; }
}
void Ch3PlainData_Leaf::SetData( int number, int cnt, bool *data ){
	m_TypeIndex = NUMBER;
	m_TypeData = BOOL;
	m_Index.number = number;
	if( cnt > ARR_MAX )cnt = ARR_MAX;
	if( cnt < -1 )return;
	m_Cnt = cnt;
	if( cnt > 0 ) memcpy( m_Data.arrBool, data, cnt*sizeof(bool) );
}

void Ch3PlainData_Leaf::DelData(){
//	m_TypeIndex = NAME;
	m_TypeData = CHAR;
//	strncpy( m_Index.name, name, 32 );
	m_Cnt = 0;
	strncpy( m_Data.string, "#DEL#", 128 );
}

int Ch3PlainData_Leaf::GetData( char *data )
{
	if( m_TypeData != CHAR )return 0;
	strncpy( data, m_Data.string, m_Cnt +1 );
	return m_Cnt;
}
int Ch3PlainData_Leaf::GetData( int *data )
{
	if( m_TypeData != INT )return 0;
	memcpy( data, m_Data.arrInt, m_Cnt * sizeof(int) );
	return m_Cnt;
}
int Ch3PlainData_Leaf::GetData( float *data )
{
	if( m_TypeData != FLOAT )return 0;
	memcpy( data, m_Data.arrFloat, m_Cnt * sizeof(float) );
	return m_Cnt;
}
int Ch3PlainData_Leaf::GetData( double *data )
{
	if( m_TypeData != FLOAT )return 0;
	for( int c=0; c<m_Cnt; c++ ){ data[c] = (double)m_Data.arrFloat[c]; }
	return m_Cnt;
}
int Ch3PlainData_Leaf::GetData( bool *data )
{
	if( m_TypeData != BOOL )return 0;
	memcpy( data, m_Data.arrBool, m_Cnt * sizeof(bool) );
	return m_Cnt;
}

void Ch3PlainData_Leaf::PrintData( FILE *file )
{
	if( m_Cnt <= 0 ) return;

	if( m_TypeIndex == NAME ){
		fprintf( file, " %s", m_Index.name );
	}else if( m_TypeIndex == NUMBER ){
		fprintf( file, " %d", m_Index.number );
	}

	if( m_TypeData == CHAR ){
		fprintf( file, " CHAR %d:%s", m_Cnt, m_Data.string );
	}else if( m_TypeData == INT ){
		fprintf( file, " INT %d:%d", m_Cnt, m_Data.arrInt[ 0 ] );
		for( int c=1; c<m_Cnt; c++ ){
			fprintf( file, " %d", m_Data.arrInt[ c ] );
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

/////////////////////////////////////////////////
void Ch3PlainData::SetNameSpace( char *name )
{
	m_NodeNameSpaceNow = m_NodeTop.SetNodeChild( name );
}

void Ch3PlainData::GetNameSpace( int number )
{
	m_NodeNameSpaceNow = m_NodeTop.SetNodeChild( number );
}

void Ch3PlainData::Set(int num, char *name, int cnt, int *dataIn)
{
	Ch3PlainData_Node *pNode;
	Ch3PlainData_Leaf *pLeaf;
	pNode = m_NodeNameSpaceNow->SetNodeChild( num );
	if( pNode == NULL )return;
	pLeaf = pNode->SetLeafChild( name );
	pLeaf->SetData( name, cnt, dataIn );
}

void Ch3PlainData::Set(int num, char *name, int cnt, float *dataIn)
{
	Ch3PlainData_Node *pNode;
	Ch3PlainData_Leaf *pLeaf;
	pNode = m_NodeNameSpaceNow->SetNodeChild( num );
	if( pNode == NULL )return;
	pLeaf = pNode->SetLeafChild( name );
	pLeaf->SetData( name, cnt, dataIn );
}

void Ch3PlainData::Set(int num, char *name, int cnt, double *dataIn)
{
	Ch3PlainData_Node *pNode;
	Ch3PlainData_Leaf *pLeaf;
	pNode = m_NodeNameSpaceNow->SetNodeChild( num );
	if( pNode == NULL )return;
	pLeaf = pNode->SetLeafChild( name );
	pLeaf->SetData( name, cnt, dataIn );
}

void Ch3PlainData::Set(int num, char *name, char *dataIn)
{
	Ch3PlainData_Node *pNode;
	Ch3PlainData_Leaf *pLeaf;
	pNode = m_NodeNameSpaceNow->SetNodeChild( num );
	if( pNode == NULL )return;
	pLeaf = pNode->SetLeafChild( name );
	pLeaf->SetData( name, dataIn );
}

void Ch3PlainData::Set(int num, char *name, int cnt, bool *dataIn)
{
	Ch3PlainData_Node *pNode;
	Ch3PlainData_Leaf *pLeaf;
	pNode = m_NodeNameSpaceNow->SetNodeChild( num );
	if( pNode == NULL )return;
	pLeaf = pNode->SetLeafChild( name );
	pLeaf->SetData( name, cnt, dataIn );
}

void Ch3PlainData::Del(int num, char *name)
{
	Ch3PlainData_Node *pNode;
	Ch3PlainData_Leaf *pLeaf;
	pNode = m_NodeNameSpaceNow->SetNodeChild( num );
	if( pNode == NULL )return;
	pLeaf = pNode->SetLeafChild( name );
	pLeaf->DelData();
}

int Ch3PlainData::Get(int num, char *name, int *dataOut)
{
	Ch3PlainData_Node *pNode;
	Ch3PlainData_Leaf *pLeaf;
	pNode = m_NodeNameSpaceNow->SearchNodeChild( num );
	if( pNode == NULL )return 0;
	pLeaf = (Ch3PlainData_Leaf*)pNode->SearchNodeChild( name );
	if( pLeaf == NULL )return 0;
	return pLeaf->GetData( dataOut );
}

int Ch3PlainData::Get(int num, char *name, float *dataOut)
{
	Ch3PlainData_Node *pNode;
	Ch3PlainData_Leaf *pLeaf;
	pNode = m_NodeNameSpaceNow->SearchNodeChild( num );
	if( pNode == NULL )return 0;
	pLeaf = (Ch3PlainData_Leaf*)pNode->SearchNodeChild( name );
	if( pLeaf == NULL )return 0;
	return pLeaf->GetData( dataOut );
}

int Ch3PlainData::Get(int num, char *name, double *dataOut)
{
	Ch3PlainData_Node *pNode;
	Ch3PlainData_Leaf *pLeaf;
	pNode = m_NodeNameSpaceNow->SearchNodeChild( num );
	if( pNode == NULL )return 0;
	pLeaf = (Ch3PlainData_Leaf*)pNode->SearchNodeChild( name );
	if( pLeaf == NULL )return 0;
	return pLeaf->GetData( dataOut );
}

int Ch3PlainData::Get(int num, char *name, char *dataOut)
{
	Ch3PlainData_Node *pNode;
	Ch3PlainData_Leaf *pLeaf;
	pNode = m_NodeNameSpaceNow->SearchNodeChild( num );
	if( pNode == NULL )return 0;
	pLeaf = (Ch3PlainData_Leaf*)pNode->SearchNodeChild( name );
	if( pLeaf == NULL )return 0;
	return pLeaf->GetData( dataOut );
}

int Ch3PlainData::Get(int num, char *name, bool *dataOut)
{
	Ch3PlainData_Node *pNode;
	Ch3PlainData_Leaf *pLeaf;
	pNode = m_NodeNameSpaceNow->SearchNodeChild( num );
	if( pNode == NULL )return 0;
	pLeaf = (Ch3PlainData_Leaf*)pNode->SearchNodeChild( name );
	if( pLeaf == NULL )return 0;
	return pLeaf->GetData( dataOut );
}

bool Ch3PlainData::Save(char *fileName)
{
	FILE *file = fopen( fileName, "w" );
	if( file == NULL )return false;

	Ch3PlainData_Node *pNode1 = m_NodeTop.GetNodeChildTop(), *pNode2;
	Ch3PlainData_Leaf *pLeaf;
	while( pNode1 != NULL )
	{	///// ネーム・スペース
		if( pNode1->GetName()[ 0 ] == 0 ){
			fprintf( file, "#no namespace\n" );
		}else{
			fprintf( file, "namespace %s {\n", pNode1->GetName() );
		}

		pNode2 = pNode1->GetNodeChildTop();
		while( pNode2 != NULL )
		{	///// インデックス番号
			int indexNode2 = pNode2->GetNumber();
			pLeaf = (Ch3PlainData_Leaf*)pNode2->GetNodeChildTop();
			while( pLeaf != NULL ){
				fprintf( file, "%d", indexNode2 );
				pLeaf->PrintData( file );
				pLeaf = (Ch3PlainData_Leaf*)pLeaf->GetNext();
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

bool Ch3PlainData::Load(char *fileName)
{
	FILE *file = fopen( fileName, "r" );
	if( file == NULL )return false;

	char line[ 1024 ];
	int level = 0;
	char *pC;
	while( fgets( line, 1024, file ) != NULL )
	{
		pC = line;
		while( *pC != 0 ){
			if( *pC == '{' ){ level++; }
			if( *pC == '}' ){
				level--;
				if( level == 0 ){
					m_NodeNameSpaceNow = m_NodeTop.SetNodeChild( "" );
				}
			}
			if( *pC == '#' ){ *pC = 0; break; }
			if( *pC == '\n' ){ *pC = 0; break; }
			pC++;
		}
		if( line[ 0 ] == 0 )continue;

		///// ネーム・スペース
		char nameClass[ 64 ], namePrivate[ 64 ];
		nameClass[ 0 ] = 0;
		namePrivate[ 0 ] = 0;
		sscanf( line, "%s %s", nameClass, namePrivate );
		if( strcmp( nameClass, "namespace" ) == 0 ){
			int clen = (int)strlen( namePrivate );
			for( int c=0; c<clen; c++ ){
				if( namePrivate[c]=='{' || namePrivate[c]==' ' ) namePrivate[c]=0;
			}
			m_NodeNameSpaceNow = m_NodeTop.SetNodeChild( namePrivate );
			continue;
		}

		///// ノード
		int number = 0;
		if( sscanf( line, "%d", &number ) > 0 )
		{
			Ch3PlainData_Node *pNode = m_NodeNameSpaceNow->SetNodeChild( number );
			if( pNode == NULL )continue;

			pC = line;
			while( *pC != ' ' && *pC != 0 )pC++;
			pNode->ScaningSetLeafChild_IndexName( pC );
		}
	}

	fclose( file );
	return true;
}

bool Ch3PlainData::Retrieve(char *str)
{
	if( str == NULL )return false;

	char line[ 1024 ];
	char *strBreak, *pC;
	int level = 0;
	while( ( strBreak = strchr( str, '\n' ) ) != NULL )
	{
		int len = (int)(strBreak - str);
		if( len < 1 ){ str++; continue; }
		if( len > 1024 ){ len = 1024; }
		strncpy( line, str, len );
		line[ len ] = 0;

		pC = line;
		while( *pC != 0 ){
			if( *pC == '{' ){ level++; }
			if( *pC == '}' ){
				level--;
				if( level == 0 ){
					m_NodeNameSpaceNow = m_NodeTop.SetNodeChild( "" );
				}
			}
			if( *pC == '#' ){ *pC = 0; break; }
			if( *pC == '\n' ){ *pC = 0; break; }
			pC++;
		}
		if( line[ 0 ] == 0 )continue;

		///// ネーム・スペース
		char nameClass[ 64 ], namePrivate[ 64 ];
		nameClass[ 0 ] = 0;
		namePrivate[ 0 ] = 0;
		sscanf( line, "%s %s", nameClass, namePrivate );
		if( strcmp( nameClass, "namespace" ) == 0 ){
			int clen = (int)strlen( namePrivate );
			for( int c=0; c<clen; c++ ){
				if( namePrivate[c]=='{' || namePrivate[c]==' ' ) namePrivate[c]=0;
			}
			m_NodeNameSpaceNow = m_NodeTop.SetNodeChild( namePrivate );

			str = ++strBreak;
			continue;
		}

		///// ノード
		int number = 0;
		if( sscanf( line, "%d", &number ) > 0 )
		{
			Ch3PlainData_Node *pNode = m_NodeNameSpaceNow->SetNodeChild( number );
			if( pNode == NULL )continue;

			pC = line;
			while( *pC != ' ' && *pC != 0 )pC++;
			pNode->ScaningSetLeafChild_IndexName( pC );
		}

		str = ++strBreak;///// 次の行に移動
	}

	return true;
}

bool Ch3PlainData::Copy( Ch3PlainData *dataOrg, char **namespaces )
{
	Ch3PlainData_Node *pNode1 = dataOrg->m_NodeTop.GetNodeChildTop(), *pNode2;
	Ch3PlainData_Leaf *pLeaf;
	SetNameSpace( "" );

	while( pNode1 != NULL )
	{	///// ネーム・スペース
		if( namespaces != NULL ){
			int arrMax = sizeof( namespaces );
			for( int a=0; a<arrMax; a++ ){
				if( strcmp( pNode1->GetName(), namespaces[ a ] ) == 0 ){
					this->SetNameSpace( namespaces[ a ] );
					goto NameIsIncluded;
				}
			}
			continue;
		}
NameIsIncluded:

		pNode2 = pNode1->GetNodeChildTop();
		while( pNode2 != NULL )
		{	///// 番号
			int indexNode2 = pNode2->GetNumber();
			Ch3PlainData_Node *pNodeAdd2 = this->m_NodeNameSpaceNow->SetNodeChild( indexNode2 );
			if( pNodeAdd2 == NULL ) continue;

			pLeaf = (Ch3PlainData_Leaf*)pNode2->GetNodeChildTop();
			while( pLeaf != NULL ){
				pNodeAdd2->CopyLeafChild( pLeaf );
				pLeaf = (Ch3PlainData_Leaf*)pLeaf->GetNext();///// Name (Leaf)
			}

			pNode2 = pNode2->GetNext();///// Number
		}

		pNode1 = pNode1->GetNext();///// NameSpace
	}

	return true;
}

bool Ch3PlainData::ForeachStart( int num )
{
	Ch3PlainData_Node *pNode;
	pNode = m_NodeNameSpaceNow->SearchNodeChild( num );
	if( pNode == NULL )return false;
	m_LeafIterate = (Ch3PlainData_Leaf*)pNode->GetNodeChildTop();
	if( m_LeafIterate == NULL )return false;
	return true;
}

int Ch3PlainData::ForeachGet( char *nameOut, void *dataOut, char *typeOut )
{
	if( m_LeafIterate == NULL )return 0;
	int cnt = m_LeafIterate->GetCnt();
	if( cnt < 1 )return 0;
	
	strncpy( nameOut, m_LeafIterate->GetName(), 32 );

	if( m_LeafIterate->DataIsCHAR() ){
		if( typeOut != NULL )strncpy( typeOut, "CHAR", 8 );
		m_LeafIterate->GetData( (char*)dataOut );
	}else if( m_LeafIterate->DataIsINT() ){
		if( typeOut != NULL )strncpy( typeOut, "INT", 8 );
		m_LeafIterate->GetData( (int*)dataOut );
	}else if( m_LeafIterate->DataIsFLOAT() ){
		if( typeOut != NULL )strncpy( typeOut, "FLOAT", 8 );
		m_LeafIterate->GetData( (float*)dataOut );
	}else if( m_LeafIterate->DataIsBOOL() ){
		if( typeOut != NULL )strncpy( typeOut, "BOOL", 8 );
		m_LeafIterate->GetData( (bool*)dataOut );
	}

	return cnt;
}
