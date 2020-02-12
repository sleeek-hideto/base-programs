#include "StdAfx.h"
#include ".\matrix.h"

/*
*	@brief �s��P�A�Q�̑����Z���s���A���̌��ʂ����g�Ɋi�[����
*	@param[in] in1 �s��P
*	@param[in] in2 �s��Q
*/
bool CMatrix::Add( CMatrix *in1, CMatrix *in2 )
{
	if( !IsSameSize( in1 ) || !IsSameSize( in2 ) )return false;

	cvAdd( in1->GetCvMat(), in2->GetCvMat(), this->GetCvMat() );
	return true;
}

/*
*	@brief �s��P�A�Q�̈����Z���s���A���̌��ʂ����g�Ɋi�[����
*	@param[in] inLeft �s��P
*	@param[in] inRight �s��Q
*/
bool CMatrix::Sub( CMatrix *inLeft, CMatrix *inRight )
{
	if( !IsSameSize( inLeft ) || !IsSameSize( inRight ) )return false;
	
	cvSub( inLeft->GetCvMat(), inRight->GetCvMat(), this->GetCvMat() );
	return true;
}

/*
*	@brief �s��P�A�Q�̊|���Z���s���A���̌��ʂ����g�Ɋi�[����
*	@param[in] inLeft �s��P
*	@param[in] inRight �s��Q
*/
bool CMatrix::Mul( CMatrix *inLeft, CMatrix *inRight )
{
	cvMatMul( inLeft->GetCvMat(), inRight->GetCvMat(), this->GetCvMat() );
	return true;
}

/*
*	@brief �s��P�ƍs��Q�̓]�u�̊|���Z���s���A���̌��ʂ����g�Ɋi�[����
*	@param[in] inLeft �s��P
*	@param[in] inRight �s��Q
*/
/*
bool CMatrix::Mul_MxMt( CMatrix &inLeft, CMatrix &inRight )
{
	cvGEMM( inLeft.GetCvMat(), inRight.GetCvMat(), 1.0, NULL, 0.0, this->GetCvMat(), CV_GEMM_B_T );
	return true;
}
*/

/*
*	@brief �s��̋t�s�񂠂邢�͋[���t�s������߁A���̌��ʂ����g�Ɋi�[����
*	@param[in] in �s��
*/
bool CMatrix::Invert( CMatrix *in )
{
	cvInvert( in->GetCvMat(), this->GetCvMat() );
	return true;
}

/*
*	@brief �s��ς����߁A���̌��ʂ����g�Ɋi�[����
*	@param[in] in1 �s��
*	@return �s���
*/
double CMatrix::Det( CMatrix *in )
{
	return cvDet( in->GetCvMat() );
}

/*
*	@brief �]�u�s��A���̌��ʂ����g�Ɋi�[����
*	@param[in] in1 �s��
*/
bool CMatrix::Transpose( CMatrix *in )
{
	cvTranspose( in->GetCvMat(), this->GetCvMat() );
	return true;
}

/*
*	@brief �s��ɒ萔���|���A���̌��ʂ����g�Ɋi�[����
*	@param[in] c �萔
*	@param[in] in �s��
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