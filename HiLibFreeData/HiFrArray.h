#pragma once

/* PHP の Array 似のクラス */
class ChArray
{
public:
	char my_type;	///// 0:NULL, p:parent, i:int, d:double, c:char
	int my_index;
	char my_name[16];
	union UNION_my_data
	{
		int _int;
		double _double;
		char* _char;
		ChArray* _array;
	}my_data;
	ChArray* my_next;
	int my_cntArr;
	/* temp data */
	ChArray* my_step;

	void myDataPut( int in ){		my_type='i';	my_data._int = in;		}
	void myDataPut( double in ){		my_type='d';	my_data._double = in;		}
	void myDataPut( char* in )
	{
		my_type='c';
		size_t len = strlen( in );	my_data._char = (char*) new char[ len +1 ]; strcpy( my_data._char, in );
	}
	void myDataPut( ChArray* in )
	{
		my_type='p';
		ChArray* paNew = (ChArray*)new ChArray();	paNew->clone( in );	my_data._array = paNew;
	}

	int myGetInt(){	if( my_type=='i' )return my_data._int;	return 0;	}
	double myGetDouble(){	if( my_type=='d' )return my_data._double;	return 0.0;	}
	char* myGetChar(){	if( my_type=='c' )return my_data._char;	return NULL;	}

	ChArray* myKeySearch( int index );
	ChArray* myKeySearch( char* name );
	void myKeySearch( int index, ChArray* &pre, ChArray* &cur, ChArray* &nex );
	void myKeySearch( char* name, ChArray* &pre, ChArray* &cur, ChArray* &nex );
	ChArray* myValueSearch( int in );
	ChArray* myValueSearch( double in );
	ChArray* myValueSearch( char* in );

	/*!
		@brief インデックスの最高値＋１を求める
	*/
	int getMaxIndex(){
		int index = 0;
		for( ChArray* cur=stepStart(); cur!=NULL; cur=stepNext() )
		{
			if( cur->my_name[0] == 0 && cur->my_index >= index ){	index = cur->my_index +1;	}
		}
		return index;
	}

public:
	ChArray(void)
	{
		my_type = 0;	my_index = 0;	my_name[0] = 0;	my_data._array = 0;	my_step = NULL;	my_next = NULL;
		my_cntArr = 0;
	}
	~ChArray(void);
	void clear();

	void clone( ChArray* in );
	void link( ChArray* in );

	void set( int index, int in );
	void set( int index, double in );
	void set( int index, char* in );
	void set( int index, ChArray* in );
	void set( int index, double* in, int cnt ){
		for( int i=0; i<cnt; i++ ){ set( index+i, in[i] ); }
	}

	void set( char* name, int in );
	void set( char* name, double in );
	void set( char* name, char* in );
	void set( char* name, ChArray* in );

	void push( int in );
	void push( double in );
	void push( char* in );
	void push( ChArray* in );
	void push( double* in, int cnt ){
		for( int i=0; i<cnt; i++ ){ push( in[i] ); }
	}

	int getMyInt(){		if( my_type='i' )return my_data._int;	return 0;		}
	double getMyDouble(){		if( my_type='d' )return my_data._double;	return 0.0;		}
	char* getMyChar(){		if( my_type='c' )return my_data._char;	return NULL;		}
	ChArray* getMyArray(){		if( my_type='a' )return my_data._array;	return NULL;		}

	int getInt( int index );
	double getDouble( int index );
	char* getChar( int index );
	ChArray* getArray( int index );
	void getArrDouble( int index, double *out, int cnt ){
		for( int i=0; i<cnt; i++ ){ out[i]=getDouble( index+i ); }
	}
	int getInt( char* name );
	double getDouble( char* name );
	char* getChar( char* name );
	ChArray* getArray( char* name );

	void unset( int index );
	void unset( char* name );

	int count();

	bool split( char* key, char* str, char type='c' );
	bool implode( char* key, char* str );
	void print_r( int rank=0 );
	bool loadCsvInt( char *loc );
	bool loadCsvDouble( char *loc );
	//bool loadCsvChar( char *loc );

	bool sort();

	ChArray* in_array( int v ){		return myValueSearch( v );		}
	ChArray* in_array( double v ){		return myValueSearch( v );		}
	ChArray* in_array( char* v ){		return myValueSearch( v );		}
	bool merge( ChArray* in1, ChArray* in2 );
	bool unique( ChArray* in );

	ChArray* stepStart();
	ChArray* stepNext();
	ChArray* stepEnd();

private:
	bool attachChild( ChArray* pa );
};

class ChArrayData : public ChArray
{
private:
	char *myScanNextColon(char *str){
		while(str[0]!=':' && str[0]!='\n' && str[0]!='\0'){ str++; } return ++str;
	}
	char *myScanNextSpace(char *str){
		while(str[0]!=' ' && str[0]!='\n' && str[0]!='\0'){ str++; } return ++str;
	}
	void myTrim(char *str){
		int len=(int)strlen(str)-1;
		while( len>=0 && (str[len]=='\n' || str[len]=='\r') ){ str[len]=0; len--; }
	}
public:
	bool save( char *loc );
	bool load( char *loc );

	void set( int index, char* name, int* in, int cnt );
	void set( int index, char* name, double* in, int cnt );
	void set( int index, char* name, char* in );

	int get( int index, char* name, int* out );
	int get( int index, char* name, double* out );
	int get( int index, char* name, char* out );
};

