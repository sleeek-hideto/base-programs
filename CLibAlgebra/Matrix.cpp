#include "StdAfx.h"
#include ".\matrix.h"

/*
*	@brief s—ñ‚PA‚Q‚Ì‘«‚µŽZ‚ðs‚¢A‚»‚ÌŒ‹‰Ê‚ðŽ©g‚ÉŠi”[‚·‚é
*	@param[in] in1 s—ñ‚P
*	@param[in] in2 s—ñ‚Q
*/
bool CMatrix::Add( CMatrix *in1, CMatrix *in2 )
{
	if( !IsSameSize( in1 ) || !IsSameSize( in2 ) )return false;

	cvAdd( in1->GetCvMat(), in2->GetCvMat(), this->GetCvMat() );
	return true;
}

/*
*	@brief s—ñ‚PA‚Q‚Ìˆø‚«ŽZ‚ðs‚¢A‚»‚ÌŒ‹‰Ê‚ðŽ©g‚ÉŠi”[‚·‚é
*	@param[in] inLeft s—ñ‚P
*	@param[in] inRight s—ñ‚Q
*/
bool CMatrix::Sub( CMatrix *inLeft, CMatrix *inRight )
{
	if( !IsSameSize( inLeft ) || !IsSameSize( inRight ) )return false;
	
	cvSub( inLeft->GetCvMat(), inRight->GetCvMat(), this->GetCvMat() );
	return true;
}

/*
*	@brief s—ñ‚PA‚Q‚ÌŠ|‚¯ŽZ‚ðs‚¢A‚»‚ÌŒ‹‰Ê‚ðŽ©g‚ÉŠi”[‚·‚é
*	@param[in] inLeft s—ñ‚P
*	@param[in] inRight s—ñ‚Q
*/
bool CMatrix::Mul( CMatrix *inLeft, CMatrix *inRight )
{
	cvMatMul( inLeft->GetCvMat(), inRight->GetCvMat(), this->GetCvMat() );
	return true;
}

/*
*	@brief s—ñ‚P‚Æs—ñ‚Q‚Ì“]’u‚ÌŠ|‚¯ŽZ‚ðs‚¢A‚»‚ÌŒ‹‰Ê‚ðŽ©g‚ÉŠi”[‚·‚é
*	@param[in] inLeft s—ñ‚P
*	@param[in] inRight s—ñ‚Q
*/
/*
bool CMatrix::Mul_MxMt( CMatrix &inLeft, CMatrix &inRight )
{
	cvGEMM( inLeft.GetCvMat(), inRight.GetCvMat(), 1.0, NULL, 0.0, this->GetCvMat(), CV_GEMM_B_T );
	return true;
}
*/

/*
*	@brief s—ñ‚Ì‹ts—ñ‚ ‚é‚¢‚Í‹[Ž—‹ts—ñ‚ð‹‚ßA‚»‚ÌŒ‹‰Ê‚ðŽ©g‚ÉŠi”[‚·‚é
*	@param[in] in s—ñ
*/
bool CMatrix::Invert( CMatrix *in )
{
	cvInvert( in->GetCvMat(), this->GetCvMat() );
	return true;
}

/*
*	@brief s—ñÏ‚ð‹‚ßA‚»‚ÌŒ‹‰Ê‚ðŽ©g‚ÉŠi”[‚·‚é
*	@param[in] in1 s—ñ
*	@return s—ñÏ
*/
double CMatrix::Det( CMatrix *in )
{
	return cvDet( in->GetCvMat() );
}

/*
*	@brief “]’us—ñA‚»‚ÌŒ‹‰Ê‚ðŽ©g‚ÉŠi”[‚·‚é
*	@param[in] in1 s—ñ
*/
bool CMatrix::Transpose( CMatrix *in )
{
	cvTranspose( in->GetCvMat(), this->GetCvMat() );
	return true;
}

/*
*	@brief s—ñ‚É’è”‚ðŠ|‚¯A‚»‚ÌŒ‹‰Ê‚ðŽ©g‚ÉŠi”[‚·‚é
*	@param[in] c ’è”
*	@param[in] in s—ñ
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