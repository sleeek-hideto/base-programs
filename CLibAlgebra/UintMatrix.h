#pragma once

class CUintMatrix
{
protected:
	typedef unsigned int uint;

	int m_nRows;//!<s‚Ì”
	int m_nCols;//!<—ñ‚Ì”
	unsigned int *m_Data;//!<ƒf[ƒ^‚ðŠi”[‚·‚éêŠ

public:
	CUintMatrix(void){
		m_Data = NULL;
	}
	CUintMatrix( int rows, int cols, unsigned int *data=NULL ){
		Create( rows, cols, data );
	}
	~CUintMatrix(void){
		Destroy();
	}
	void Create( int rows, int cols, unsigned int *data=NULL ){
		m_nRows = rows;
		m_nCols = cols;
		if( m_Data == NULL )m_Data = new uint[ rows * cols ];
		if( data != NULL )memcpy( m_Data, data, rows * cols * sizeof( uint ) );
	}
	void Destroy(){
		if( m_Data != NULL ){
			delete [] m_Data;
			m_Data = NULL;
		}
	}
	const int GetRows()				{	return m_nRows;	}
	const int GetCols()				{	return m_nCols;	}
	const int GetRowsCols()			{	return m_nRows * m_nCols;	}
	void SetData( int row, int col, unsigned int data ){
		m_Data[ row * GetCols() + col ] = data;
	}
	void SetData( unsigned int *data ){
		memcpy( m_Data, data, GetRows() * GetCols() * sizeof( uint ) );
	}
	unsigned int *GetData( int row=0, int col=0 ){
		return m_Data + row * GetCols() + col;
	}

	void TestPrint( char *str=NULL, int num=0 ){
		printf( "CUintMatrix" );
		if( str != NULL )printf( "_%s", str );
		if( num != 0 )printf( "_%d", num );
		printf( " (%d,%d){\n\t", GetRows(), GetCols() ) ;
		int cntMax = GetRowsCols();
		unsigned int *pData = GetData();
		for( int c=0; c<cntMax; c++ ){
			printf( " %d", *pData );
			if( (c+1) % GetCols() == 0 )printf( ";" );
			pData++;
		}
		printf( "\n}\n" );
	}
};

class CUintArrayVector3 : public CUintMatrix
{
public:
	CUintArrayVector3(){
	}
	CUintArrayVector3( int rows, unsigned int *data=NULL ){
		Create( rows, data );
	}

	void Create( int rows, unsigned int *data=NULL ){
		CUintMatrix::Create( rows, 3, data );
	}

	void SetDataRow( int r, unsigned int *data ){
		memcpy( m_Data + r * 3, data, 3 * sizeof( uint ) );
	}
	unsigned int *GetDataRow( int r ){
		return m_Data + r * 3;
	}
};
