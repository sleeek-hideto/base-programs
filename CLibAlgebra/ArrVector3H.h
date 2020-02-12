#pragma once
#include "matrix.h"

/*
*	@brief 斉次座標の配列を格納するクラス
*/
class CArrayVector3H : public CMatrix
{
private:

public:
	CArrayVector3H(void):CMatrix( 1 , 4 ){
	}
	CArrayVector3H( int rows, double *data=NULL ):CMatrix( rows , 4 , data ){
		//Create( rows, data );
	}

	~CArrayVector3H(void){
	}

public:
	void Create( int rows, double *data=NULL ){
		//CMatrix::Create( rows, 4, data );
		*this = CArrayVector3H(rows,data);
	}
	void SetDataRow( int row, double *data ){
		memcpy( GetData( row ), data, 3*sizeof(double) );
		SetData( row, 3, 1.0 );
	}
	void AddDataRow( int row, double *dataIn ){
		double *data = GetData( row );
		data[ 0 ] += dataIn[ 0 ]; data[ 1 ] += dataIn[ 1 ]; data[ 2 ] += dataIn[ 2 ]; data[ 3 ] = 1.0;
	}

	bool ArrayAdd( CArrayVector3H *arr, CVector3H *vec );
	bool ArrayAdd( CVector3H *vec ){
		return ArrayAdd( this, vec );
	}
	bool ArraySub( CArrayVector3H *arr, CVector3H *vec );
	bool ArraySub( CVector3H *vec ){
		return ArraySub( this, vec );
	}
	bool ArrayMul( CArrayVector3H *arr, CMatrix4x4 *mat );
	bool ArrayMul( CMatrix4x4 *mat ){
		CArrayVector3H tmpArr;
		tmpArr.Copy( this );
		return ArrayMul( &tmpArr, mat );
	}
	bool ArraySqrt( CArrayVector3H *arr );
	bool ArraySqrt(){
		return ArraySqrt( this );
	}
	bool ArrayMulAndSqrt( CArrayVector3H *arr, CMatrix4x4 *mat );
	bool ArrayMulAndSqrt( CMatrix4x4 *mat ){
		CArrayVector3H tmpArr;
		tmpArr.Copy( this );
		return ArrayMulAndSqrt( &tmpArr, mat );	
	}
};
