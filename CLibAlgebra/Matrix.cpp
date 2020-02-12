#include "StdAfx.h"
#include ".\matrix.h"

/*
*	@brief 行列１、２の足し算を行い、その結果を自身に格納する
*	@param[in] in1 行列１
*	@param[in] in2 行列２
*/
bool CMatrix::Add( CMatrix *in1, CMatrix *in2 )
{
	if( !IsSameSize( in1 ) || !IsSameSize( in2 ) )return false;

	cvAdd( in1->GetCvMat(), in2->GetCvMat(), this->GetCvMat() );
	return true;
}

/*
*	@brief 行列１、２の引き算を行い、その結果を自身に格納する
*	@param[in] inLeft 行列１
*	@param[in] inRight 行列２
*/
bool CMatrix::Sub( CMatrix *inLeft, CMatrix *inRight )
{
	if( !IsSameSize( inLeft ) || !IsSameSize( inRight ) )return false;
	
	cvSub( inLeft->GetCvMat(), inRight->GetCvMat(), this->GetCvMat() );
	return true;
}

/*
*	@brief 行列１、２の掛け算を行い、その結果を自身に格納する
*	@param[in] inLeft 行列１
*	@param[in] inRight 行列２
*/
bool CMatrix::Mul( CMatrix *inLeft, CMatrix *inRight )
{
	cvMatMul( inLeft->GetCvMat(), inRight->GetCvMat(), this->GetCvMat() );
	return true;
}

/*
*	@brief 行列１と行列２の転置の掛け算を行い、その結果を自身に格納する
*	@param[in] inLeft 行列１
*	@param[in] inRight 行列２
*/
/*
bool CMatrix::Mul_MxMt( CMatrix &inLeft, CMatrix &inRight )
{
	cvGEMM( inLeft.GetCvMat(), inRight.GetCvMat(), 1.0, NULL, 0.0, this->GetCvMat(), CV_GEMM_B_T );
	return true;
}
*/

/*
*	@brief 行列の逆行列あるいは擬似逆行列を求め、その結果を自身に格納する
*	@param[in] in 行列
*/
bool CMatrix::Invert( CMatrix *in )
{
	cvInvert( in->GetCvMat(), this->GetCvMat() );
	return true;
}

/*
*	@brief 行列積を求め、その結果を自身に格納する
*	@param[in] in1 行列
*	@return 行列積
*/
double CMatrix::Det( CMatrix *in )
{
	return cvDet( in->GetCvMat() );
}

/*
*	@brief 転置行列、その結果を自身に格納する
*	@param[in] in1 行列
*/
bool CMatrix::Transpose( CMatrix *in )
{
	cvTranspose( in->GetCvMat(), this->GetCvMat() );
	return true;
}

/*
*	@brief 行列に定数を掛け、その結果を自身に格納する
*	@param[in] c 定数
*	@param[in] in 行列
*/
bool CMatrix::MulConst( double c, CMatrix *in )
{
	cvConvertScale( in->GetCvMat(), this->GetCvMat(), c, 0 );

	//int cntMax = GetRows() * GetCols();
	//double *pmi = in.GetData(), *pmt = this->GetData();
	//for( int c=0; c<cntMax; c++ )
	//{
	//	*pmt = c * *pmi;
	//	pmi++;
	//	pmt++;
	//}
	return true;
}

CMatrix& operator+= ( CMatrix& lhs , const CMatrix& rhs ){
	cvAdd( lhs.GetCvMat(), rhs.GetCvMat(), lhs.GetCvMat() );
	return lhs;
}

CMatrix  operator+  ( const CMatrix& lhs , const CMatrix& rhs){
	CMatrix mat(lhs);
	return mat += rhs;
}

CMatrix& operator-= ( CMatrix& lhs , const CMatrix& rhs ){
	cvSub( lhs.GetCvMat() , rhs.GetCvMat() , lhs.GetCvMat() );
	return lhs;
}

CMatrix operator-  ( const CMatrix& lhs , const CMatrix& rhs ){
	CMatrix mat(lhs);
	return mat -= rhs;
}

CMatrix operator*  ( const CMatrix& lhs , const CMatrix& rhs ){
	CMatrix mat(lhs);
	cvMatMul( lhs.GetCvMat(), rhs.GetCvMat(), mat.GetCvMat() );
	return mat;
}

CMatrix& operator*= ( CMatrix& lhs , double rhs){
	cvConvertScale( lhs.GetCvMat(), lhs.GetCvMat(), rhs, 0 );	
	return lhs;
}

CMatrix operator*  ( const CMatrix& lhs , double rhs){
	CMatrix mat(lhs);
	return mat *= rhs;
}

CMatrix operator*  ( double lhs , const CMatrix& rhs){
	return rhs * lhs;
}