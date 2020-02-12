#include "StdAfx.h"
#include ".\arrvector3h.h"

/*
*	@brief ベクトル列arrの各ベクトルに対し、ベクトルvecを足し、その結果を自身に格納する
*	@param[in] arr ベクトル列
*	@param[in] vec ベクトル
*/
bool CArrayVector3H::ArrayAdd( CArrayVector3H *arr, CVector3H *vec )
{
	const int Rows = GetRows();
	int Step = 4;
	double *dataArr = arr->GetData();
	double *dataMe = this->GetData();
	double *dataVec = vec->GetData();
	for( int r=0; r<Rows; r++ )
	{
		dataMe[ 0 ] = dataArr[ 0 ] + dataVec[ 0 ];
		dataMe[ 1 ] = dataArr[ 1 ] + dataVec[ 1 ];
		dataMe[ 2 ] = dataArr[ 2 ] + dataVec[ 2 ];
		dataMe[ 3 ] = 1.0;
		dataArr += Step;
		dataMe += Step;
	}
	return true;
}

/*
*	@brief ベクトル列arrの各ベクトルに対し、ベクトルvecを引き、その結果を自身に格納する
*	@param[in] arr ベクトル列
*	@param[in] vec ベクトル
*/
bool CArrayVector3H::ArraySub( CArrayVector3H *arr, CVector3H *vec )
{
	const int Rows = GetRows();
	int Step = 4;
	double *dataArr = arr->GetData();
	double *dataMe = this->GetData();
	double *dataVec = vec->GetData();
	for( int r=0; r<Rows; r++ )
	{
		dataMe[ 0 ] = dataArr[ 0 ] - dataVec[ 0 ];
		dataMe[ 1 ] = dataArr[ 1 ] - dataVec[ 1 ];
		dataMe[ 2 ] = dataArr[ 2 ] - dataVec[ 2 ];
		dataMe[ 3 ] = 1.0;
		dataArr += Step;
		dataMe += Step;
	}
	return true;
}

/*
*	@brief ベクトル列arrの各ベクトルに対し、４×４行列を掛け、その結果を自身に格納する
*	@param[in] arr ベクトル列
*	@param[in] mat 行列
*/
bool CArrayVector3H::ArrayMul( CArrayVector3H *arr, CMatrix4x4 *mat )
{	///// (v4o)^T = (mat * v4i)^T => v4o^T = v4i^T * mat^T
	cvGEMM( arr->GetCvMat(), mat->GetCvMat(), 1.0, NULL, 0.0, this->GetCvMat(), CV_GEMM_B_T );
	return true;
}

/*
*	@brief ベクトル列arrの各ベクトルに対し正規化後、その結果を自身に格納する
*	@param[in] arr ベクトル列
*	@param[in] mat 行列
*/
bool CArrayVector3H::ArraySqrt( CArrayVector3H *arr )
{
	int c, cnt = GetRows();
	double *data = GetData();
	for( c=0; c<cnt; c++ )
	{
		double len = sqrt( data[0]*data[0] + data[1]*data[1] + data[2]*data[2] );
		if( len == 0.0 ){
			data += 4;
			continue;
		}
		data[0] /= len;
		data[1] /= len;
		data[2] /= len;
		data[3] = 1.0;
		data += 4;
	}
	return true;
}

/*
*	@brief ベクトル列arrの各ベクトルに対し、４×４行列を掛け、正規化後、その結果を自身に格納する
*	@param[in] arr ベクトル列
*	@param[in] mat 行列
*/
bool CArrayVector3H::ArrayMulAndSqrt( CArrayVector3H *arr, CMatrix4x4 *mat )
{
	cvGEMM( arr->GetCvMat(), mat->GetCvMat(), 1.0, NULL, 0.0, this->GetCvMat(), CV_GEMM_B_T );

	int c, cnt = GetRows();
	double len, *data = GetData();
	for( c=0; c<cnt; c++ )
	{
		len = sqrt( data[0]*data[0] + data[1]*data[1] + data[2]*data[2] );
		if( len == 0.0 ){
			data += 4;
			continue;
		}
		data[0] /= len;
		data[1] /= len;
		data[2] /= len;
		data += 4;
	}
	return true;
}
