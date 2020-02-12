#include "StdAfx.h"
#include ".\HiFrArray.h"

/*!
	@brief ソートのための比較関数
*/
int ChArray_myCompForSort( ChArray *arr1, ChArray *arr2 )
{
	if( arr1->my_name[ 0 ] == 0 )
	{
		if( arr2->my_name[ 0 ] == 0 )
		{
			return arr1->my_index - arr1->my_index;
		}else{
			return 1;
		}
	}else{
		if( arr2->my_name[ 0 ] == 0 )
		{
			return -1;
		}else{
			return strcmp( arr1->my_name, arr2->my_name );
		}
	}
	return 0;
}

/*!
	@brief 番号indexのアレーを探す
	@param[in] index 番号
	@return アレー
*/
ChArray* ChArray::myKeySearch( int index )
{
	ChArray* cur;
	for( cur=stepStart(); cur != NULL; cur=stepNext() )
	{
		if( cur->my_name[0] == 0 && cur->my_index == index )
		{
			return cur;
		}
	}
	return NULL;
}

/*!
	@brief 名前nameのアレーを探す
	@param[in] name 名前
	@return アレー
*/
ChArray* ChArray::myKeySearch( char* name )
{
	ChArray* cur;
	for( cur=stepStart(); cur != NULL; cur=stepNext() )
	{
		if( strcmp( cur->my_name, name ) == 0 )
		{
			return cur;
		}
	}
	return NULL;
}

/*!
	@brief 番号indexのアレーを探す
	@param[in] index 番号
	@param[out] pre 該当するアレーの一つ前
	@param[out] cur indexのアレー
	@param[out] nex 該当するアレーの一つ後
*/
void ChArray::myKeySearch( int index, ChArray* &pre, ChArray* &cur, ChArray* &nex )
{
	pre = NULL;
	for( cur=stepStart(); cur != NULL; cur=stepNext() )
	{
		if( cur->my_name[0] == 0 && cur->my_index == index )
		{
			nex = cur->my_next;
			return;
		}
		pre = cur;
	}
}

/*!
	@brief 名前nameのアレーを探す
	@param[in] name 名前
	@param[out] pre 該当するアレーの一つ前
	@param[out] cur nameのアレー
	@param[out] nex 該当するアレーの一つ後
*/
void ChArray::myKeySearch( char* name, ChArray* &pre, ChArray* &cur, ChArray* &nex )
{
	pre = NULL;
	for( cur=stepStart(); cur != NULL; cur=stepNext() )
	{
		if( strcmp( cur->my_name, name ) == 0 )
		{
			nex = cur->my_next;
			return;
		}
		pre = cur;
	}
}

/*!
	@brief 数inが格納されているアレーを探す
	@param[in] in 数
	@return inのアレー
*/
ChArray* ChArray::myValueSearch( int in )
{
	for( ChArray* cur=stepStart(); cur != NULL; cur=stepNext() )
	{
		if( cur->my_type == 'i' && cur->my_data._int == in )return cur;
	}
	return NULL;
}

/*!
	@brief 数inが格納されているアレーを探す
	@param[in] in 数
	@return inのアレー
*/
ChArray* ChArray::myValueSearch( double in )
{
	for( ChArray* cur=stepStart(); cur != NULL; cur=stepNext() )
	{
		if( cur->my_type == 'd' && cur->my_data._double == in )return cur;
	}
	return NULL;
}

/*!
	@brief 文字列inが格納されているアレーを探す
	@param[in] in 文字列
	@return inのアレー
*/
ChArray* ChArray::myValueSearch( char* in )
{
	for( ChArray* cur=stepStart(); cur != NULL; cur=stepNext() )
	{
		if( cur->my_type == 'c' && strcmp( cur->my_data._char, in )==0 )return cur;
	}
	return NULL;
}

ChArray::~ChArray()
{
	clear();
}

/*!
	@brief すべてのアレーを破棄する
*/
void ChArray::clear()
{
	if( my_type == 'p' )
	{
		ChArray* cur;
		ChArray* tmp;
		for( cur=stepStart(); cur!=NULL; )
		{
			tmp = cur;
			cur = stepNext();
			delete tmp;
		}
	}

	my_type = 0;
	my_name[0] = 0;	my_step = NULL;	my_next = NULL;
}

/*!
	@brief アレーをコピーする
	@param in 元のアレー
*/
void ChArray::clone( ChArray* in )
{
	if( in->my_type == 0 )return;
	clear();

	if( in->my_type == 'p' )
	{
		for( ChArray* paCur=in->stepStart(); paCur!=NULL; paCur=in->stepNext() )
		{
			ChArray* paNew = (ChArray*) new ChArray();
			ChArray* paEnd = stepEnd();
			if( paEnd != NULL )
			{
				paEnd->my_next = paNew;
			}else{
				myDataPut( paNew );
			}
			paNew->clone( paCur );

			paCur = paCur->my_next;
		}
	}else if( in->my_type == 'c' ){
		my_index = in->my_index;	strcmp( my_name, in->my_name );
		myDataPut( in->my_data._char );
	}else if( in->my_type == 'i' ){
		my_index = in->my_index;	strcmp( my_name, in->my_name );
		myDataPut( in->my_data._int );
	}else if( in->my_type == 'd' ){
		my_index = in->my_index;	strcmp( my_name, in->my_name );
		myDataPut( in->my_data._double );
	}
	//else if( in->my_type == 'D' ){
	//	my_index = in->my_index;	strcmp( my_name, in->my_name );
	//	myDataPut( in->my_data._arr_double, in->my_cntArr );
	//}
}

/*!
	@brief 自身にアレーを繋げる
	@param[in] in アレー
*/
void ChArray::link( ChArray* in )
{
	clear();
	myDataPut( in->my_data._array );
}

/*!
	@brief 番号indexに数字inを格納する
	@param[in] index インデックス
	@param[in] in 数
*/
void ChArray::set( int index, int in )
{
	if( my_type == 0 ){		my_type = 'p';		}
	if( my_type != 'p' )return;

	ChArray* pa = myKeySearch( index );
	if( pa == NULL )
	{
		pa = (ChArray*) new ChArray();
		pa->my_index = index;
		pa->myDataPut( in );
		attachChild( pa );
	}else{
		pa->clear();
		pa->myDataPut( in );
	}
}

/*!
	@brief 番号indexに数字inを格納する
	@param[in] index インデックス
	@param[in] in 数
*/
void ChArray::set( int index, double in )
{
	if( my_type == 0 ){		my_type = 'p';		}
	if( my_type != 'p' )return;

	ChArray* pa = myKeySearch( index );
	if( pa == NULL )
	{
		pa = (ChArray*) new ChArray();
		pa->my_index = index;
		pa->myDataPut( in );
		attachChild( pa );
	}else{
		pa->clear();
		pa->myDataPut( in );
	}
}

/*!
	@brief 番号indexに文字列inを格納する
	@param[in] index インデックス
	@param[in] in 文字列
*/
void ChArray::set( int index, char* in )
{
	if( my_type == 0 ){		my_type = 'p';		}
	if( my_type != 'p' )return;

	ChArray* pa = myKeySearch( index );
	if( pa == NULL )
	{
		pa = (ChArray*) new ChArray();
		pa->my_index = index;
		pa->myDataPut( in );
		attachChild( pa );
	}else{
		pa->clear();
		pa->myDataPut( in );
	}
}

/*!
	@brief 番号indexにアレーinを格納する
	@param[in] index インデックス
	@param[in] in アレー
*/
void ChArray::set( int index, ChArray* in )
{
	if( my_type == 0 ){		my_type = 'p';		}
	if( my_type != 'p' )return;

	ChArray* pa = myKeySearch( index );
	if( pa == NULL )
	{
		in->my_index = index;
		attachChild( in );
	}else{
		unset( index );
		in->my_cntArr = index;
		attachChild( in );
	}
}

/*!
	@brief 番号indexから、浮動小数配列inを格納する
	@param[in] index インデックス
	@param[in] in 浮動小数配列
	@param[in] cnt 配列の要素数
*/
/*
void ChArray::set( int index, double* in, int cnt )
{
	if( my_type == 0 ){		my_type = 'p';		}
	if( my_type != 'p' )return;

	ChArray* pa = myKeySearch( index );
	if( pa == NULL )
	{
		pa = (ChArray*) new ChArray();
		pa->my_index = index;
		pa->myDataPut( in, cnt );
		attachChild( pa );
	}else{
		pa->clear();
		pa->myDataPut( in, cnt );
	}
}
*/

/*!
	@brief 名前nameに数字inを格納する
	@param[in] name 名前
	@param[in] in 数
*/
void ChArray::set( char* name, int in )
{
	if( my_type == 0 ){		my_type = 'p';		}
	if( my_type != 'p' )return;

	ChArray* pa = myKeySearch( name );
	if( pa == NULL )
	{
		pa = (ChArray*) new ChArray();
		strncpy( pa->my_name, name, 15 );
		pa->myDataPut( in );
		attachChild( pa );
	}else{
		pa->clear();
		pa->myDataPut( in );
	}
}

/*!
	@brief 名前nameに数字inを格納する
	@param[in] name 名前
	@param[in] in 数
*/
void ChArray::set( char* name, double in )
{
	if( my_type == 0 ){		my_type = 'p';		}
	if( my_type != 'p' )return;

	ChArray* pa = myKeySearch( name );
	if( pa == NULL )
	{
		pa = (ChArray*) new ChArray();
		strncpy( pa->my_name, name, 15 );
		pa->myDataPut( in );
		attachChild( pa );
	}else{
		pa->clear();
		pa->myDataPut( in );
	}
}

/*!
	@brief 名前nameに文字列inを格納する
	@param[in] name 名前
	@param[in] in 文字列
*/
void ChArray::set( char* name, char* in )
{
	if( my_type == 0 ){		my_type = 'p';		}
	if( my_type != 'p' )return;

	ChArray* pa = myKeySearch( name );
	if( pa == NULL )
	{
		pa = (ChArray*) new ChArray();
		strncpy( pa->my_name, name, 15 );
		pa->myDataPut( in );
		attachChild( pa );
	}else{
		pa->clear();
		pa->myDataPut( in );
	}
}

/*!
	@brief 名前nameにアレーinを格納する
	@param[in] name 名前
	@param[in] in アレー
*/
void ChArray::set( char* name, ChArray* in )
{
	if( my_type == 0 ){		my_type = 'p';		}
	if( my_type != 'p' )return;

	ChArray* pa = myKeySearch( name );
	if( pa == NULL )
	{
		strncpy( in->my_name, name, 15 );
		attachChild( in );
	}else{
		unset( name );
		strncpy( in->my_name, name, 15 );
		attachChild( in );
	}
}

/*!
	@brief 名前nameに浮動小数配列inを格納する
	@param[in] name 名前
	@param[in] in 浮動小数配列
	@param[in] cnt 配列の要素数
*/
/*
void ChArray::set( char* name, double* in, int cnt )
{
	if( my_type == 0 ){		my_type = 'p';		}
	if( my_type != 'p' )return;

	ChArray* pa = myKeySearch( name );
	if( pa == NULL )
	{
		pa = (ChArray*) new ChArray();
		strncpy( pa->my_name, name, 15 );
		pa->myDataPut( in, cnt );
		attachChild( pa );
	}else{
		pa->clear();
		pa->myDataPut( in, cnt );
	}
}
*/

/*!
	@brief インデックスindexに格納されている数を返す
	@param[in] index インデックス
	@return 数
*/
int ChArray::getInt( int index )
{
	ChArray* pa = myKeySearch( index );
	if( pa == NULL || pa->my_type != 'i' )return 0;
	return pa->my_data._int;
}

/*!
	@brief インデックスindexに格納されている数を返す
	@param[in] index インデックス
	@return 数
*/
double ChArray::getDouble( int index )
{
	ChArray* pa = myKeySearch( index );
	if( pa == NULL || pa->my_type != 'd' )return 0.0;
	return pa->my_data._double;
}

/*!
	@brief インデックスindexに格納されている文字列を返す
	@param[in] index インデックス
	@return 文字列へのポインタ
*/
char* ChArray::getChar( int index )
{
	ChArray* pa = myKeySearch( index );
	if( pa == NULL || pa->my_type != 'c' )return NULL;
	return pa->my_data._char;
}

/*!
	@brief インデックスindexに格納されているアレーを返す
	@param[in] index インデックス
	@return アレーへのポインタ
*/
ChArray* ChArray::getArray( int index )
{
	ChArray* pa = myKeySearch( index );
	if( pa == NULL || pa->my_type != 'p' )return 0;
	return pa;
}

/*!
	@brief インデックスindexに格納されている数の配列を返す
	@param[in] index インデックス
	@return 数の配列
*/
/*
double* ChArray::getArrDouble( int index )
{
	ChArray* pa = myKeySearch( index );
	if( pa == NULL || pa->my_type != 'D' )return 0;
	return pa->my_data._arr_double;
}
*/

/*!
	@brief 名前nameに格納されている数を返す
	@param[in] name 名前
	@return 数
*/
int ChArray::getInt( char* name )
{
	ChArray* pa = myKeySearch( name );
	if( pa == NULL || pa->my_type != 'i' )return 0;
	return pa->my_data._int;
}

/*!
	@brief 名前nameに格納されている数を返す
	@param[in] name 名前
	@return 数
*/
double ChArray::getDouble( char* name )
{
	ChArray* pa = myKeySearch( name );
	if( pa == NULL || pa->my_type != 'd' )return 0;
	return pa->my_data._double;
}

/*!
	@brief 名前nameに格納されている文字列を返す
	@param[in] name 名前
	@return 文字列へのポインタ
*/
char* ChArray::getChar( char* name )
{
	ChArray* pa = myKeySearch( name );
	if( pa == NULL || pa->my_type != 'c' )return NULL;
	return pa->my_data._char;
}

/*!
	@brief 名前nameに格納されているアレーを返す
	@param[in] name 名前
	@return アレーへのポインタ
*/
ChArray* ChArray::getArray( char* name )
{
	ChArray* pa = myKeySearch( name );
	if( pa == NULL || pa->my_type != 'p' )return NULL;
	return pa;
}

/*!
	@brief 名前nameに格納されている数の配列を返す
	@param[in] name 名前
	@return 数の配列へのポインタ
*/
/*
double* ChArray::getArrDouble( char* name )
{
	ChArray* pa = myKeySearch( name );
	if( pa == NULL || pa->my_type != 'D' )return 0;
	return pa->my_data._arr_double;
}
*/

/*!
	@brief アレーの最後尾に数inを格納する
	@param[in] in 数
*/
void ChArray::push( int in ){		my_type = 'p';	set( getMaxIndex(), in );		}

/*!
	@brief アレーの最後尾に数inを格納する
	@param[in] in 数
*/
void ChArray::push( double in ){		my_type = 'p';	set( getMaxIndex(), in );		}

/*!
	@brief アレーの最後尾に文字列inを格納する
	@param[in] in 文字列
*/
void ChArray::push( char* in ){		my_type = 'p';	set( getMaxIndex(), in );		}

/*!
	@brief 自身のアレーの最後尾にアレーinを格納する
	@param[in] in アレー
*/
void ChArray::push( ChArray* in ){		my_type = 'p';	set( getMaxIndex(), in );		}

/*!
	@brief アレーの最後尾に数の配列inを格納する
	@param[in] in 数の配列
*/
//void ChArray::push( double* in, int cnt ){		my_type = 'p';	set( getMaxIndex(), in, cnt );		}

/*!
	@brief インデックスindexの要素を削除する
	@param[in] index インデックス
*/
void ChArray::unset( int index )
{
	if( my_type != 'p' )return;
	ChArray *pre, *cur, *nex;
	myKeySearch( index, pre, cur, nex );
	if( cur == NULL )return;
	delete cur;
	if( pre == NULL ){
		my_data._array = nex;
	}else{
		pre->my_next = nex;
	}
}

/*!
	@brief 名前nameの要素を削除する
	@param[in] name 名前
*/
void ChArray::unset( char* name )
{
	if( my_type != 'p' )return;
	ChArray *pre, *cur, *nex;
	myKeySearch( name, pre, cur, nex );
	if( cur == NULL )return;
	delete cur;
	if( pre == NULL ){
		my_data._array = nex;
	}else{
		pre->my_next = nex;
	}
}

/*!
	@brief アレーの要素数を返す
	@return 要素数
*/
int ChArray::count()
{
	int cnt = 0;
	for( ChArray* cur=stepStart(); cur!=NULL; cur=stepNext() ){		cnt++;		}
	return cnt;
}

/*!
	@brief 文字列strをキーkeyによって、切り出します
	@param[in] key キー
	@param[in] str 文字列
	@param[in] type 切り出す種類。i:int,c:char,d:doubleのいずれかを入力
*/
bool ChArray::split( char* key, char* str, char type )
{
	size_t lenKey = strlen( key );
	if( lenKey < 1 )return false;
	size_t lenStr = strlen( str );
	if( lenStr < 2 )return false;

	clear();
	my_type = 'p';

	int cnt = 0;
	char* pc = (char*) new char [ lenStr +1 ];
	strcpy( pc, str );

		ChArray* paNew = (ChArray*) new ChArray();
		if( type == 'd' ){
			paNew->my_type = 'd';
			paNew->my_data._double = atof( pc );
		}else if( type == 'c' ){
			paNew->my_type = 'c';
			paNew->my_data._char = pc;
		}else if( type == 'i' ){
			paNew->my_type = 'i';
			paNew->my_data._int = atoi( pc );
		}else{
			return false;
		}
		paNew->my_index = cnt;
		cnt++;

	my_data._array = paNew;
	ChArray* pArrEnd = paNew;

	while( pc != NULL )
	{
		pc = strstr( pc, key );
		if( pc == NULL )break;

		paNew = (ChArray*) new ChArray();
		ChArray* paNew = (ChArray*) new ChArray();
		if( type == 'd' ){
			paNew->my_type = 'd';
			paNew->my_data._double = atof( pc +lenKey );
		}else if( type == 'c' ){
			paNew->my_type = 'c';
			paNew->my_data._char = pc +lenKey;
		}else if( type == 'i' ){
			paNew->my_type = 'i';
			paNew->my_data._int = atoi( pc +lenKey );
		}
		paNew->my_index = cnt;
		cnt++;
		///// update my_next;
		pArrEnd->my_next = paNew;
		pArrEnd = paNew;

		*pc = 0;	pc += lenKey;
	}

	return true;
}

/*!
	@brief アレーの要素をキーkeyを挟んで統合した文字列を作る
	@param[in] key キー
	@param[out] str 文字列
*/
bool ChArray::implode( char* key, char* str )
{
	bool isFirst = true;
	for( ChArray* cur=stepStart(); cur!=NULL; cur=stepNext() )
	{
		if( isFirst )
		{
			switch( cur->my_type )
			{
			case 'i':	sprintf( str, "%d", cur->my_data._int );	break;
			case 'd':	sprintf( str, "%f", cur->my_data._double );	break;
			case 'c':	sprintf( str, "%s", cur->my_data._char );	break;
			case 'p':	sprintf( str, "Array", cur->my_data._array );	break;
			}
			isFirst = false;
		}else{
			switch( cur->my_type )
			{
			case 'i':	sprintf( str, "%s%s%d", str, key, cur->my_data._int );	break;
			case 'd':	sprintf( str, "%s%s%f", str, key, cur->my_data._double );	break;
			case 'c':	sprintf( str, "%s%s%s", str, key, cur->my_data._char );	break;
			case 'p':	sprintf( str, "%s%sArray", str, key, cur->my_data._array );	break;
			}
		}
	}
	return true;
}

/*!
	@brief 要素をすべて出力する
	@param[in] rank タブを何個付けるか。0のままで
*/
void ChArray::print_r( int rank )
{
	if( rank > 10 )return;
	for( int t=0; t<rank; t++ ){	printf( "\t" );		}
	printf( "Array(\n" );

	if( my_type == 'p' )
	{
		for( ChArray* cur=stepStart(); cur!=NULL; cur=stepNext() )
		{
			if( cur->my_type == 'p' )
			{
				for( int t=0; t<rank; t++ ){	printf( "\t" );		}
				if( cur->my_index != 0 ){	printf( "[%d]->\n" , cur->my_index );	}
				else if( cur->my_name[0] != 0 ){	printf( "[%s]->\n", cur->my_name );	}
				cur->print_r( rank+1 );
				continue;
			}

			for( int t=0; t<rank; t++ ){	printf( "\t" );		}
			if( cur->my_name[0] != 0 ){		printf( "[%s]->", cur->my_name );		}
			else{		printf( "[%d]->", cur->my_index );		}

			if( cur->my_type == 'i' ){		printf( "%d\n", cur->my_data._int );		}
			else if( cur->my_type == 'd' ){		printf( "%f\n", cur->my_data._double );		}
			else if( cur->my_type == 'c' ){		printf( "%s\n", cur->my_data._char );		}
		}
	}else{
		if( my_type == 'i' ){		printf( "%d\n", my_data._int );		}
		else if( my_type == 'd' ){		printf( "%f\n", my_data._double );		}
		else if( my_type == 'c' ){		printf( "%s\n", my_data._char );		}
	}

	for( int t=0; t<rank; t++ ){	printf( "\t" );		}
	printf( ")\n" );
}

/*!
	@brief ソートする
*/
bool ChArray::sort()
{
	if( my_type != 'p' )return false;

	int tableMax = count();
	ChArray **tableArr = new ChArray* [ tableMax ];

	/* テーブルの構築 */
	int cnt = 0;
	for( ChArray* cur=stepStart(); cur!=NULL; )
	{
		ChArray *tmp = cur;
		cur=stepNext();

		tableArr[ cnt ] = tmp;
		tmp->my_next = NULL;
		cnt++;
	}

	/* ソート */
	qsort( tableArr, tableMax, sizeof(ChArray*),
		(int (*)(const void*,const void*))ChArray_myCompForSort );

	/* 再登録 */
	my_data._array = NULL;
	for( int c=0; c<tableMax; c++ )
	{
		ChArray *cur = tableArr[ c ];
		if( cur->my_name[ 0 ] == 0 )
		{
			set( cur->my_index, cur );
		}else{
			set( cur->my_name, cur );
		}
	}

	return true;
}

/*!
	@brief 整数が格納されたcsv形式のファイルを読み込んで、アレーを作る
	@param[in] loc ファイルの位置
*/
bool ChArray::loadCsvInt( char *loc )
{
	FILE *fp = fopen( loc, "r" );
	if( fp == NULL )return false;

	clear();
	my_type = 'p';
	int anArrIndex = -1;
	ChArray *anArr;

	char line[ 1024 ];
	while( fgets( line, 1023, fp ) )
	{
		anArrIndex++;
		if( strlen( line ) < 1 || line[0] == '\n' )continue;
		anArr = new ChArray();
		anArr->split( ",", line, 'i' );
		set( anArrIndex, anArr );
//		anArr->print_r();
//		this->print_r();
	}
	return true;
}


/*!
	@brief 浮動小数が格納されたcsv形式のファイルを読み込んで、アレーを作る
	@param[in] loc ファイルの位置
*/
bool ChArray::loadCsvDouble( char *loc )
{
	FILE *fp = fopen( loc, "r" );
	if( fp == NULL )return false;

	clear();
	my_type = 'p';
	int anArrIndex = -1;
	ChArray *anArr;

	char line[ 1024 ];
	while( fgets( line, 1023, fp ) )
	{
		anArrIndex++;
		if( strlen( line ) < 1 || line[0] == '\n' )continue;
		anArr = new ChArray();
		anArr->split( ",", line, 'd' );
		set( anArrIndex, anArr );
	}
	return true;
}

/*!
	@brief 文字列が格納されたcsv形式のファイルを読み込んで、アレーを作る
	@param[in] loc ファイルの位置
*/
/*
bool ChArray::loadCsvChar( char *loc )
{
	FILE *fp = fopen( loc, "r" );
	if( fp == NULL )return false;

	clear();
	my_type = 'p';
	int anArrIndex = -1;
	ChArray *anArr;

	char line[ 1024 ];
	while( fgets( line, 1023, fp ) )
	{
		anArrIndex++;
		if( strlen( line ) < 1 || line[0] == '\n' )continue;
		anArr = new ChArray();
		anArr->split( ",", line, 'c' );
		set( anArrIndex, anArr );
	}
	return true;
}
*/

/*!
	@brief アレーin1,in2を統合する
	@param[in] in1 アレー
	@param[in] in2 アレー
*/
bool ChArray::merge( ChArray* in1, ChArray* in2 )
{
	clear();
	my_type = 'p';

	int cnt = 0;
	if( in1->my_type ='p' )
	{
		for( ChArray* paIn=in1->stepStart(); paIn != NULL; paIn=in1->stepNext() )
		{
			if( paIn->my_name[ 0 ] != 0 )
			{
				if( paIn->my_type == 'i' ){		this->set( paIn->my_name, paIn->my_data._int );		}
				else if( paIn->my_type == 'd' ){		this->set( paIn->my_name, paIn->my_data._double );		}
				else if( paIn->my_type == 'c' ){		this->set( paIn->my_name, paIn->my_data._char );		}
				else if( paIn->my_type == 'p' ){		this->set( paIn->my_name, paIn->my_data._array );		}
			}else{
				if( paIn->my_type == 'i' ){		this->set( cnt, paIn->my_data._int );		}
				else if( paIn->my_type == 'd' ){		this->set( cnt, paIn->my_data._double );		}
				else if( paIn->my_type == 'c' ){		this->set( cnt, paIn->my_data._char );		}
				else if( paIn->my_type == 'p' ){		this->set( cnt, paIn->my_data._array );		}
				cnt++;
			}
		}
	}
	if( in2->my_type ='p' )
	{
		for( ChArray* paIn=in2->stepStart(); paIn != NULL; paIn=in2->stepNext() )
		{
			if( paIn->my_name[ 0 ] != 0 )
			{
				if( paIn->my_type == 'i' ){		this->set( paIn->my_name, paIn->my_data._int );		}
				else if( paIn->my_type == 'd' ){		this->set( paIn->my_name, paIn->my_data._double );		}
				else if( paIn->my_type == 'c' ){		this->set( paIn->my_name, paIn->my_data._char );		}
				else if( paIn->my_type == 'p' ){		this->set( paIn->my_name, paIn->my_data._array );		}
			}else{
				if( paIn->my_type == 'i' ){		this->set( cnt, paIn->my_data._int );		}
				else if( paIn->my_type == 'd' ){		this->set( cnt, paIn->my_data._double );		}
				else if( paIn->my_type == 'c' ){		this->set( cnt, paIn->my_data._char );		}
				else if( paIn->my_type == 'p' ){		this->set( cnt, paIn->my_data._array );		}
				cnt++;
			}
		}
	}
	return true;
}

/*!
	@brief アレーの要素のインデックスを0から順に付け直す
	@param[in] in アレー
*/
bool ChArray::unique( ChArray* in )
{
	clear();
	my_type = 'p';
	
	int cnt = 0;

	for( ChArray* pa=in->stepStart(); pa != NULL; pa=in->stepNext() )
	{
		switch( pa->my_type )
		{
		case 'i':
			if( myValueSearch( pa->my_data._int ) == NULL )
			{
				if( pa->my_name[ 0 ] != 0 )
				{
					set( pa->my_name, pa->my_data._int );
				}else{
					set( cnt++, pa->my_data._int );
				}
			}
			break;
		case 'd':
			if( myValueSearch( pa->my_data._double ) == NULL )
			{
				if( pa->my_name[ 0 ] != 0 )
				{
					set( pa->my_name, pa->my_data._double );
				}else{
					set( cnt++, pa->my_data._double );
				}
			}
			break;
		case 'c':
			if( myValueSearch( pa->my_data._char ) == NULL )
			{
				if( pa->my_name[ 0 ] != 0 )
				{
					set( pa->my_name, pa->my_data._char );
				}else{
					set( cnt++, pa->my_data._char );
				}
			}
			break;
		}
	}
	return true;
}

/*!
	@brief foreach処理を行う際の、要素ポインタの初期化
*/
ChArray* ChArray::stepStart()
{
	if( my_type != 'p' )return NULL;

	my_step = my_data._array;
	return my_step;
}

/*!
	@brief foreach処理を行う際の、要素ポインタを次に動かす
*/
ChArray* ChArray::stepNext()
{
	if( my_type != 'p' )return NULL;

	if( my_step != NULL )
	{
		my_step = my_step->my_next;
		return my_step;
	}
	return NULL;
}

/*!
	@brief foreach処理を行う際の、要素ポインタが最後かどうかを調べる
*/
ChArray* ChArray::stepEnd()
{
	if( my_type != 'p' )return NULL;

	ChArray* pTmp = (ChArray*)my_data._array;
	if( pTmp == NULL )return NULL;
	while( pTmp->my_next != NULL )
	{
		pTmp = pTmp->my_next;
	}
	return pTmp;
}

/*!
	@brief アレーを要素の最後に加える
	@param[in] pa アレーへのポインタ
*/
bool ChArray::attachChild( ChArray* pa )
{
	if( my_type == 0 ){		my_type = 'p';		}
	if( my_type != 'p' )return false;

	ChArray *paEnd = stepEnd();
	if( paEnd == NULL )
	{
		my_data._array = pa;
	}else{
		paEnd->my_next = pa;
	}
	return true;
}

/*!
	@brief インデックス＋ネーム形式のアレーをファイルに保存する
	@param[in] loc ファイルの位置
*/
bool ChArrayData::save( char *loc )
{
	if( my_type != 'p' )return false;

	char buf[ 4096 ], *pbuf;
	FILE *fp = fopen( loc, "w" );
	if( fp == NULL )return false;

	int arrIndex, arrCount;
	char arrName[ 64 ], arrType, strType[ 8 ];
	for( ChArray* cur1=this->stepStart(); cur1!=NULL; cur1=this->stepNext() )
	{
		if( cur1->my_type != 'p' )continue;
		if( cur1->my_name[0] != 0 )
		{
			printf( "! WARNING : can not save element Array[ %s ]\n", cur1->my_name[0] );
			continue;
		}
		arrIndex = cur1->my_index;

		for( ChArray* cur2=cur1->stepStart(); cur2!=NULL; cur2=cur1->stepNext() )
		{
			if( cur2->my_type != 'p' )continue;
			if( cur2->my_name[0] == 0 )
			{
				printf( "! WARNING : can not save element Array[ %d ][ %d ]\n", arrIndex, cur2->my_index );
				continue;
			}
			strcpy( arrName, cur2->my_name );

			arrCount = cur2->count();
			arrType = cur2->stepStart()->my_type;
			switch( arrType )
			{
			case 'i':	sprintf( strType, "INT" ); break;
			case 'd':	sprintf( strType, "FLOAT" ); break;
			case 'c':	sprintf( strType, "CHAR" ); arrCount=-1; break;
			default:	strType[0] = 0;
			}
			if( strType[0] == 0 )continue;

			/* ヘッダー書き込み */
			int lenHeader = sprintf( buf, "%d %s %s %d:", arrIndex, arrName, strType, arrCount );
			pbuf = buf + lenHeader;

			ChArray *cur3End = cur2->stepEnd();
			for( ChArray* cur3=cur2->stepStart(); cur3!=NULL; cur3=cur2->stepNext() )
			{
				int lenElement;

				if( cur3->my_type != arrType )
				{
					lenElement = sprintf( pbuf, "%d", 0 );
				}else{
					/* 一要素書き込み */
					switch( arrType )
					{
					case 'i':	lenElement = sprintf( pbuf, "%d", cur3->my_data._int ); break;
					case 'd':	lenElement = sprintf( pbuf, "%6.6f", cur3->my_data._double ); break;
					case 'c':	lenElement = sprintf( pbuf, "%s", cur3->my_data._char ); break;
					}
				}
				pbuf += lenElement;
				
				if( cur3 != cur3End ){
					sprintf( pbuf, " " );
					pbuf++;
				}
			}
			
			fprintf( fp, "%s\n", buf );
		}
	}

	fclose( fp );
	return true;
}

/*!
	@brief インデックス＋ネーム形式のアレーをファイルから読み込む
	@param[in] loc ファイルの位置
*/
bool ChArrayData::load( char *loc )
{
	FILE *fp = fopen( loc, "r" );
	if( fp == NULL )return false;

	clear();
	my_type = 'p';

	int index, cnt;
	char buf[ 4096 ], *pbuf, name[ 64 ], type[ 16 ];
	while( fgets( buf, 4095, fp ) )
	{
		if(buf[0]=='\n' || buf[0]=='#' || buf[0]=='\t' || sscanf(buf, "%d %s %s %d:", &index, name, type, &cnt)==0 )continue;

		ChArray *arrInd = myKeySearch( index );
		if( arrInd == NULL ){
			arrInd = new ChArray();
			ChArray::set( index, arrInd );
		}

		pbuf = myScanNextColon( buf );
		if(strcmp(type, "INT")==0){
			ChArray *arrName = new ChArray();
			for( int c=0; c<cnt; c++ )
			{
				arrName->set( c, atoi( pbuf ) );
				pbuf = myScanNextSpace( pbuf );
			}
			arrInd->set( name, arrName );
		}else if(strcmp(type, "FLOAT")==0){
			ChArray *arrName = new ChArray();
			for( int c=0; c<cnt; c++ )
			{
				arrName->set( c, atof( pbuf ) );
				pbuf = myScanNextSpace( pbuf );
			}
			arrInd->set( name, arrName );
		}else if(strcmp(type, "CHAR")==0){
			ChArray *arrName = new ChArray();
			myTrim( pbuf );
			arrName->set( 0, pbuf );
			arrInd->set( name, arrName );
		}
	}
	return true;
}

/*!
	@brief 二階層のアレーのインデックスindex,名前nameに数を格納する
	@param[in] index インデックス
	@param[in] name 名前
	@param[in] in 数
	@param[in] cnt 個数
*/
void ChArrayData::set( int index, char* name, int* in, int cnt )
{
	ChArray *arrInd = myKeySearch( index );
	if( arrInd == NULL )
	{
		arrInd = new ChArray();
		ChArray::set( index, arrInd );
	}

	ChArray *arrName = arrInd->myKeySearch( name );
	if( arrName == NULL )
	{
		arrName = new ChArray();
		arrInd->set( name, arrName );
	}

	for( int c=0; c<cnt; c++ )
	{
		arrName->set( c, in[ c ] );
	}
}

/*!
	@brief 二階層のアレーのインデックスindex,名前nameに数を格納する
	@param[in] index インデックス
	@param[in] name 名前
	@param[in] in 数
	@param[in] cnt 個数
*/
void ChArrayData::set( int index, char* name, double* in, int cnt )
{
	ChArray *arrInd = myKeySearch( index );
	if( arrInd == NULL )
	{
		arrInd = new ChArray();
		ChArray::set( index, arrInd );
	}

	ChArray *arrName = arrInd->myKeySearch( name );
	if( arrName == NULL )
	{
		arrName = new ChArray();
		arrInd->set( name, arrName );
	}

	for( int c=0; c<cnt; c++ )
	{
		arrName->set( c, in[ c ] );
	}
}

/*!
	@brief 二階層のアレーのインデックスindex,名前nameに文字列を格納する
	@param[in] index インデックス
	@param[in] name 名前
	@param[in] in 文字列
*/
void ChArrayData::set( int index, char* name, char* in )
{
	ChArray *arrInd = myKeySearch( index );
	if( arrInd == NULL )
	{
		arrInd = new ChArray();
		ChArray::set( index, arrInd );
	}

	ChArray *arrName = arrInd->myKeySearch( name );
	if( arrName == NULL )
	{
		arrName = new ChArray();
		arrInd->set( name, arrName );
	}

	arrName->set( 0, in );
}

/*!
	@brief 二階層のアレーのインデックスindex,名前nameから数を獲得する
	@param[in] index インデックス
	@param[in] name 名前
	@param[out] out 数
	@return 個数
*/
int ChArrayData::get( int index, char* name, int* out )
{
	ChArray *arrInd = myKeySearch( index );
	if( arrInd == NULL )return 0;

	ChArray *arrName = arrInd->myKeySearch( name );
	if( arrName == NULL )return 0;

	int cnt = arrName->count();
	for( int c=0; c<cnt; c++ )
	{
		out[ c ] = arrName->getInt( c );
	}
	return cnt;
}

/*!
	@brief 二階層のアレーのインデックスindex,名前nameから数を獲得する
	@param[in] index インデックス
	@param[in] name 名前
	@param[out] out 数
	@return 個数
*/
int ChArrayData::get( int index, char* name, double* out )
{
	ChArray *arrInd = myKeySearch( index );
	if( arrInd == NULL )return 0;

	ChArray *arrName = arrInd->myKeySearch( name );
	if( arrName == NULL )return 0;

	int cnt = arrName->count();
	for( int c=0; c<cnt; c++ )
	{
		out[ c ] = arrName->getDouble( c );
	}
	return cnt;
}

/*!
	@brief 二階層のアレーのインデックスindex,名前nameから文字列を獲得する
	@param[in] index インデックス
	@param[in] name 名前
	@param[out] out 文字列
	@return 文字数
*/
int ChArrayData::get( int index, char* name, char* out )
{
	ChArray *arrInd = myKeySearch( index );
	if( arrInd == NULL )return 0;

	ChArray *arrName = arrInd->myKeySearch( name );
	if( arrName == NULL )return 0;

	return sprintf( out, "%s", arrName->getChar( 0 ) );
}