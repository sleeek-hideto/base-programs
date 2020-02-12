#pragma once

/*
*	@brief 斉次座標のクラス
*/
class CVector3H : public CMatrix
{
private:
	double* m_dData[4];
private:
	void Init(){
		for( int i=0; i<4; i++ ){ m_dData[ i ] = m_Matrix->data.db +i; }
		m_Matrix->data.db[3] = 1.0;
	}

	//!<使うな！以下の関数
	void SetDataRow( int row, double *data ){}
	void SetDataCol( int col, double *data ){}
	void SwapDataRow( int r1, int r2 ){}
	void SwapDataCol( int c1, int c2 ){}
	bool Invert( CMatrix &in ){	return false; }

public:
	CVector3H(void):CMatrix(4,1){
		Init();
	}
	CVector3H( double *data ):CMatrix( 4,1,data){
		Init();
	}
	CVector3H( double x, double y, double z ):CMatrix(4,1){
		SetData( x, y, z );
		Init();
	}
	//コピーコンストラクタ
	CVector3H( const CVector3H& vec ):CMatrix(vec){
		m_Matrix->data.db[3] = 1.0;//念のため
	}
	//キャスト
	CVector3H( const CMatrix& mat ):CMatrix(mat){
		m_Matrix->data.db[3] = 1.0;
	}
	
	void Copy( CVector3H *vec ){
		*this = *vec;
		m_Matrix->data.db[3] = 1.0;
	}

	void SetDataAllZero(){
		CMatrix::SetDataAllZero();
		m_Matrix->data.db[3] = 1.0;
	}
	void SetData( double *data ){
		CMatrix::SetData(data);
		m_Matrix->data.db[3] = 1.0;
	}
	void SetData( int row, double data ){
		m_Matrix->data.db[row] = data;
	}
	void SetData( double x, double y, double z ){
		double *data = m_Matrix->data.db;
		data[ 0 ] = x;	data[ 1 ] = y;	data[ 2 ] = z;	data[ 3 ] = 1.0;
	}
	double* GetData(int row=0){
		return (double*)m_Matrix->data.db + row;
	}
	void GetData( double *data ){
		//memcpy( data, GetData(), 3*sizeof(double) );
		memcpy( data , m_Matrix->data.db, 3 * sizeof(double) );
	}
	void GetData( float *fdata ){
		double *ddata = GetData();
		fdata[0] = (float)ddata[0]; fdata[1] = (float)ddata[1]; fdata[2] = (float)ddata[2];
	}
	void AddData( double *p ){
		double *data = GetData();
		data[ 0 ] += p[ 0 ]; data[ 1 ] += p[ 1 ]; data[ 2 ] += p[ 2 ]; data[ 3 ] = 1.0;
	}
	void AddData( double x, double y, double z ){
		double *data = GetData();
		data[ 0 ] += x;	data[ 1 ] += y;	data[ 2 ] += z;	data[ 3 ] = 1.0;
	}

	CVector3H* Add( CVector3H *in1, CVector3H *in2 );
	CVector3H* Add( CVector3H *in ){
		return Add( this, in );
	}
	CVector3H* Sub( CVector3H *inLeft, CVector3H *inRight );
	CVector3H* Sub( CVector3H *in ){
		return Sub( this, in );
	}
	CVector3H* Mul( CMatrix *inLeft, CMatrix *inRight );
	static double DotProduct( CVector3H *in1, CVector3H *in2 );
	CVector3H* CrossProduct( CVector3H *inLeft, CVector3H *inRight );
	CVector3H* Transpose( CMatrix *in );
	CVector3H* MulConst( double c, CVector3H *in );
	CVector3H* MulConst( double c ){
		return MulConst( c, this );
	}
	bool EachProduct( CVector3H *in1, CVector3H *in2 );

	bool Normalize( CVector3H *in );
	bool Normalize(){
		return Normalize( this );
	}
	double Length();
	static double Distance( CVector3H *in1, CVector3H *in2 );
	bool Normal( CVector3H *in1, CVector3H *in2, CVector3H *in3 );
	bool Center( CVector3H *in1, CVector3H *in2, CVector3H *in3 );
	bool Limit( CVector3H *low, CVector3H *high );

	///// unsafe な static 関数
	static void Copy( double *in, double *out );
	static void Add( double *in1, double *in2, double *out );
	static void Sub( double *inLeft, double *inRight, double *out );
	static double DotProduct( double *in1, double *in2 );
	static void CrossProduct( double *inLeft, double *inRight, double *out );
	static void MulConst( double c, double *in, double *out );

	static bool Normalize( double *in, double *out );
	static bool Normalize( double *inout ){
		return Normalize( inout, inout );
	}
	static double Length( double *in );
	static double Distance( double *in1, double *in2 );
	static bool Normal( double *in1, double *in2, double *in3, double *out );
	static void Center( double *in1, double *in2, double *in3, double *out );
	static void Limit( double *inout, double *low, double *high );

	///// とりあえずここに置いておこう
	virtual void CalcAngleFromDirNrm_YXZ( CVector3H* dir, CVector3H* nrm );
	virtual void CalcAngleFromDir_YXZ( CVector3H* dir );
};
