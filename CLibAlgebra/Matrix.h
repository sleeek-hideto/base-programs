#pragma once
#include <iostream>
#include <algorithm>
// CvMat の定義。cv.hより
//typedef struct CvMat
//{
//    int type;	//!<型を指定
//    int step;	//!<一行のバイト数
//
//    ///// for internal use only
//    int* refcount;
//    int hdr_refcount;
//
//    union
//    {
//        uchar* ptr;
//        short* s;
//        int* i;
//        float* fl;
//        double* db;
//    } data;	//!<行列のデータを格納
//
//    union
//    {
//        int rows;
//        int height;
//    };
//
//    union
//    {
//        int cols;
//        int width;
//    };
//}
//CvMat;

/*
*	@brief 任意の行列を表すクラス
*/
class CMatrix
{
protected:
	CvMat *m_Matrix;	//!<CvMatを格納する

	bool IsSameSize( CMatrix *in ){
		if( this->GetRows() == in->GetRows() && this->GetCols() == in->GetCols() )return true;
		return false;
	}

protected:
	CMatrix( ){
		m_Matrix = NULL;
	}
public:
	CMatrix( int rows, int cols, double *data=NULL ){
		m_Matrix = NULL;
		Create( rows, cols, data );
	}
	~CMatrix(void){
		Destroy();
	}
	//代入演算子の定義
	CMatrix& operator=(const CMatrix& mat){
		if (this == &mat) return *this;
		CMatrix(mat).Swap(*this);
		return *this;
	}
	//コピーコンストラクタ
	CMatrix(const CMatrix& mat){
		m_Matrix = cvCreateMat( mat.GetRows(), mat.GetCols(), CV_64F );
		memcpy( m_Matrix->data.db, mat.m_Matrix->data.db, GetRowsCols()*sizeof(double) );
	}
protected:
	void Swap(CMatrix& mat){
		std::swap(m_Matrix, mat.m_Matrix);
	}

protected:
	//コンストラクタ以外で呼ばないこと
	void Create( int rows, int cols, double *data=NULL ){
		if( m_Matrix == NULL )m_Matrix = cvCreateMat( rows, cols, CV_64F );
		SetData( data );
	}
	//デストラクタ以外で呼ばないこと
	void Destroy(){
		if( m_Matrix != NULL ){
			cvReleaseMat( &m_Matrix );
			m_Matrix = NULL;
		}
	}
	/*
protected:
	CMatrix& Copy(const CMatrix& mat){
		Destroy();
		m_Matrix = cvCreateMat( mat.GetRows(), mat.GetCols(), CV_64F );
		memcpy( m_Matrix->data.db, mat.m_Matrix->data.db, GetRowsCols()*sizeof(double) );
		return *this;
	}
	*/
public:
	void Copy( CMatrix *mat ){
		*this = *mat;
	}
public:
	CvMat *GetCvMat()			 {	return m_Matrix;	}
	const CvMat* GetCvMat() const { return m_Matrix;	}
	const int GetRows()		const {	return m_Matrix->rows;	}
	const int GetCols()		const {	return m_Matrix->cols;	}
	const int GetRowsCols()	const {	return m_Matrix->rows * m_Matrix->cols;	}
public:
	void SetDataAllZero(){
		cvSetZero( GetCvMat() );
	}
	/**
	 * @note 非推奨
	 */
	void SetData( double *data ){
		SetDataAllZero();//引数が足りなかったときの処理
		if( data != NULL )memcpy( m_Matrix->data.db, data, GetRowsCols()*sizeof(double) );
	}
	/**
	 * @note 非推奨
	 * matrix = CMatrix(row,col,data);
	 * と利用すること
	 */
	void SetData( int row, int col, double data ){
		m_Matrix->data.db[ row * GetCols() + col ] = data;
	}
	double *GetData( int row=0, int col=0 ){
		return (double*)m_Matrix->data.db + row * GetCols() + col;
	}
	void GetData( double *data ){
		memcpy( data, m_Matrix->data.db, GetRowsCols()*sizeof(double) );
	}

	void SetDataRow( int row, double *data ){
		memcpy( GetData( row ), data, GetCols()*sizeof(double) );
	}
	void SetDataCol( int col, double *data ){
		for( int r=0; r<GetRows(); r++ ){
			m_Matrix->data.db[ r * GetCols() + col ] = data[ r ];
		}
	}
	void GetDataRow( int row, double *data ){
		memcpy( data, GetData( row ), GetCols()*sizeof(double) );
	}
	void GetDataCol( int col, double *data ){
		for( int r=0; r<GetRows(); r++ ){
			data[ r ] = m_Matrix->data.db[ r * GetCols() + col ];
		}
	}

	void SwapRow( int r1, int r2 ){
		double tmp, *pr1 = GetData( r1, 0 ), *pr2 = GetData( r2, 0 );
		for( int c=0; c<GetCols(); c++ ){
			tmp = *pr1;		*pr1 = *pr2;		*pr2 = tmp;
			pr1++;
			pr2++;
		}
	}
	void SwapCol( int c1, int c2 ){
		double tmp, *pc1 = GetData( 0, c1 ), *pc2 = GetData( 0, c2 );
		for( int r=0; r<GetRows(); r++ )
		{
			tmp = *pc1;		*pc1 = *pc2;		*pc2 = tmp;
			*pc1 += GetCols();
			*pc2 += GetCols();
		}
	}

	bool Add( CMatrix *in1, CMatrix *in2 );
	bool Sub( CMatrix *inLeft, CMatrix *inRight );
	bool Mul( CMatrix *inLeft, CMatrix *inRight );
	//bool Mul_MxMt( CMatrix &inLeft, CMatrix &inRight );
	bool Invert( CMatrix *in );
	//bool DotProduct( CMatrix &in1, CMatrix &in2 );
	//bool CrossProduct( CMatrix &inLeft, CMatrix &inRight );
	static double Det( CMatrix *in );
	bool Transpose( CMatrix *in );
	bool MulConst( double c, CMatrix *in );

	void TestPrint( char *str=NULL, int num=0 ){
		printf( "CMatrix" );
		if( str != NULL )printf( "_%s", str );
		if( num != 0 )printf( "_%d", num );
		printf( " (%d,%d){\n\t", GetRows(), GetCols() ) ;
		int cntMax = GetRowsCols();
		double *pData = GetData();
		for( int c=0; c<cntMax; c++ ){
			printf( " %5.5f", *pData );
			if( (c+1) % GetCols() == 0 )printf( ";" );
			pData++;
		}
		printf( "\n}\n" );
	}
};

extern CMatrix& operator+= ( CMatrix& lhs , const CMatrix& rhs );

extern CMatrix operator+  ( const CMatrix& lhs , const CMatrix& rhs);

extern CMatrix& operator-= ( CMatrix& lhs , const CMatrix& rhs );

extern CMatrix operator-  ( const CMatrix& lhs , const CMatrix& rhs );

//extern CMatrix& operator*= ( CMatrix& lhs , const CMatrix& rhs);

extern CMatrix operator*  ( const CMatrix& lhs , const CMatrix& rhs );

extern CMatrix& operator*= ( CMatrix& lhs , double rhs);

extern CMatrix operator*  ( const CMatrix& lhs , double rhs);

extern CMatrix operator*  ( double lhs , const CMatrix& rhs );

class CVector4 : public CMatrix
{	//!<４行１列のベクトルを表すクラス。色情報の格納に使う
private:
	double m_Data[ 4 ];

	void Create( double *data ){
		//CMatrix::Create( 4, 1, data );

		if( m_Matrix == NULL ){
			m_Matrix = cvCreateMatHeader( 4, 1, CV_64F );
			m_Matrix->data.db = m_Data;
			m_Matrix->step = sizeof( double );
			//cvSetData( m_Matrix, m_Data, sizeof( double ) );
		}
		if( data != NULL ){
			memcpy( m_Data, data, 4 * sizeof( double ) );
		}else{
			m_Data[ 0 ] = 0.0;	m_Data[ 1 ] = 0.0;	m_Data[ 2 ] = 0.0;	m_Data[ 3 ] = 1.0;
		}
	}

public:
	CVector4(){
		Create( NULL );
	}
	
	CVector4( double *data ){
		Create( data );
	}
	CVector4( double r, double g, double b, double a ){
		Create( NULL );
		SetData( r, g, b, a );
	}

	void Copy( CVector4 *vec4 ){
		memcpy( GetData(), vec4->GetData(), 4*sizeof(double) );
	}

	void SetData( double *data ){
		memcpy( m_Matrix->data.db, data, 4*sizeof(double) );
	}
	void SetData( int row, double data ){
		m_Matrix->data.db[ row ] = data;
	}
	void SetData( double r, double g, double b, double a ){
		double *pData = m_Matrix->data.db;
		pData[ 0 ] = r;	pData[ 1 ] = g;	pData[ 2 ] = b;	pData[ 3 ] = a;
	}
	double *GetData( int row=0 ){
		return (double*)m_Matrix->data.db + row;
	}
	void GetData( double *data ){
		memcpy( data, m_Matrix->data.db, 4*sizeof(double) );
	}
};