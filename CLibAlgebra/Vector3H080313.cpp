#include "StdAfx.h"
#include ".\vector3h.h"

/*
*	@brief �x�N�g���P�A�Q�̑����Z���s���A���̌��ʂ����g�Ɋi�[����
*	@param[in] in1 �x�N�g���P
*	@param[in] in2 �x�N�g���Q
*/
bool CVector3H::Add( CVector3H *in1, CVector3H *in2 )
{
	if( !IsSameSize( in1 ) || !IsSameSize( in2 ) )return false;

	cvAdd( in1->GetCvMat(), in2->GetCvMat(), this->GetCvMat() );
	SetData( 3, 1.0 );
	return true;
}

/*
*	@brief �x�N�g���P�A�Q�̈����Z���s���A���̌��ʂ����g�Ɋi�[����
*	@param[in] inLeft �x�N�g���P
*	@param[in] inRight �x�N�g���Q
*/
bool CVector3H::Sub( CVector3H *inLeft, CVector3H *inRight )
{
	if( !IsSameSize( inLeft ) || !IsSameSize( inRight ) )return false;
	
	cvSub( inLeft->GetCvMat(), inRight->GetCvMat(), this->GetCvMat() );
	SetData( 3, 1.0 );
	return true;
}

/*
*	@brief �s��P�A�Q�̊|���Z���s���A���̌��ʂ����g�Ɋi�[����
*	@param[in] inLeft �s��P
*	@param[in] inRight �s��Q
*/
bool CVector3H::Mul( CMatrix *inLeft, CMatrix *inRight )
{
	cvMatMul( inLeft->GetCvMat(), inRight->GetCvMat(), this->GetCvMat() );
	return true;
}

/*
*	@brief �x�N�g���P�ƃx�N�g���Q�̓��ς����ߕԂ�
*	@param[in] in1 �x�N�g���P
*	@param[in] in2 �x�N�g���Q
*/
double CVector3H::DotProduct( CVector3H *in1, CVector3H *in2 )
{
	double *pd1 = in1->GetData(), *pd2 = in2->GetData();
	return pd1[ 0 ] * pd2[ 0 ] + pd1[ 1 ] * pd2[ 1 ] + pd1[ 2 ] * pd2[ 2 ];
}

/*
*	@brief �x�N�g�����ƃx�N�g���E�̊O�ς����߁A���̌��ʂ����g�Ɋi�[����
*	@param[in] inLeft �x�N�g����
*	@param[in] inRight �x�N�g���E
*/
bool CVector3H::CrossProduct( CVector3H *inLeft, CVector3H *inRight )
{
	double *pvt = this->GetData(), *pv1 = inLeft->GetData(), *pv2 = inRight->GetData();
	pvt[0] = pv1[1]*pv2[2] - pv2[1]*pv1[2];
	pvt[1] = pv1[2]*pv2[0] - pv2[2]*pv1[0];
	pvt[2] = pv1[0]*pv2[1] - pv2[0]*pv1[1];
	pvt[3] = 1.0;
	return true;
}

/*
*	@brief �x�N�g����]�u���A���̌��ʂ����g�Ɋi�[����
*	@param[in] in1 �x�N�g��
*/
bool CVector3H::Transpose( CMatrix *in )
{
	cvTranspose( in->GetCvMat(), this->GetCvMat() );
	return true;
}

/*
*	@brief �x�N�g���ɒ萔���|���A���̌��ʂ����g�Ɋi�[����
*	@param[in] c �萔
*	@param[in] in �x�N�g��
*/
bool CVector3H::MulConst( double c, CVector3H *in )
{
	double *pi = in->GetData(), *pt = this->GetData();
	pt[ 0 ] = c * pi[ 0 ];
	pt[ 1 ] = c * pi[ 1 ];
	pt[ 2 ] = c * pi[ 2 ];
	pt[ 3 ] = 1.0;
	return true;
}

/*
*	@brief �x�N�g���̗v�f���ƂɁA������
*	@param[in] in1, in2 �x�N�g��
*/
bool CVector3H::EachProduct( CVector3H *in1, CVector3H *in2 )
{
	double *p1 = in1->GetData(), *p2 = in2->GetData(), *pt = this->GetData();
	pt[ 0 ] = p1[ 0 ] * p2[ 0 ];
	pt[ 1 ] = p1[ 1 ] * p2[ 1 ];
	pt[ 2 ] = p1[ 2 ] * p2[ 2 ];
	pt[ 3 ] = 1.0;
	return true;
}

/*
*	@brief �x�N�g���𐳋K������
*	@param[in] in �x�N�g��
*/
bool CVector3H::Normalize( CVector3H *in )
{
	double length, *pvi = in->GetData(), *pvt = this->GetData();
	length = pvi[0] * pvi[0] + pvi[1] * pvi[1] + pvi[2] * pvi[2];
	if( length == 0.0 )
	{
		pvt[ 0 ] = 0.0;
		pvt[ 1 ] = 0.0;
		pvt[ 2 ] = 0.0;
		pvt[ 3 ] = 1.0;
		return false;
	}

	length = sqrt( length );
	pvt[ 0 ] = pvi[ 0 ] / length;
	pvt[ 1 ] = pvi[ 1 ] / length;
	pvt[ 2 ] = pvi[ 2 ] / length;
	pvt[ 3 ] = 1.0;
	return true;
}

/*
*	@brief �x�N�g���̒��������߂�
*	@param[in] in �x�N�g��
*	@return ����
*/
double CVector3H::Length()
{
	double *pvi = GetData();
	return sqrt( pvi[0] * pvi[0] + pvi[1] * pvi[1] + pvi[2] * pvi[2] );
}

/*
*	@brief �x�N�g���Ԃ̋��������߂�
*	@param[in] in1 �x�N�g��
*	@param[in] in2 �x�N�g��
*	@return ����
*/
double CVector3H::Distance( CVector3H *in1, CVector3H *in2 )
{
	double amt, tmp;
	double *p1 = in1->GetData(), *p2 = in2->GetData();
	tmp = p1[ 0 ] - p2[ 0 ];
	amt = tmp * tmp;
	tmp = p1[ 1 ] - p2[ 1 ];
	amt += tmp * tmp;
	tmp = p1[ 2 ] - p2[ 2 ];
	amt += tmp * tmp;
	return sqrt( amt );
}

/*
*	@brief �O�_�̉E�˂������̖@�������߂�
*	@param[in] in1 �_�P
*	@param[in] in2 �_�Q
*	@param[in] in3 �_�R
*/
bool CVector3H::Normal( CVector3H *in1, CVector3H *in2, CVector3H *in3 )
{
	CVector3H v31, v21, nrm;
	v31.Sub( in3,in1 );
	v21.Sub( in2,in1 );
	nrm.CrossProduct( &v21, &v31 );
	return Normalize( &nrm );
}

/*
*	@brief �O�_�̒��S�_�����߂�
*	@param[in] in1 �_�P
*	@param[in] in2 �_�Q
*	@param[in] in3 �_�R
*/
bool CVector3H::Center( CVector3H *in1, CVector3H *in2, CVector3H *in3 )
{
	double *pvi1 = in1->GetData(), *pvi2 = in2->GetData(), *pvi3 = in3->GetData();
	double *pvt = this->GetData();

	pvt[ 0 ] = ( pvi1[ 0 ] + pvi2[ 0 ] + pvi3[ 0 ] ) / 3.0;
	pvt[ 1 ] = ( pvi1[ 1 ] + pvi2[ 1 ] + pvi3[ 1 ] ) / 3.0;
	pvt[ 2 ] = ( pvi1[ 2 ] + pvi2[ 2 ] + pvi3[ 2 ] ) / 3.0;
	return true;
}

/*
*	@brief �x�N�g���̒l�������llow,����lhigh�𒴂��Ȃ��悤�ɒ�������
*	@param[in] low ����
*	@param[in] high ���
*/
bool CVector3H::Limit( CVector3H *low, CVector3H *high )
{
	double *pvl = low->GetData(), *pvh = high->GetData(), *pvt = this->GetData();

	if( pvt[ 0 ] < pvl[ 0 ] )pvt[ 0 ] = pvl[ 0 ];
	if( pvt[ 0 ] > pvh[ 0 ] )pvt[ 0 ] = pvh[ 0 ];
	if( pvt[ 1 ] < pvl[ 1 ] )pvt[ 1 ] = pvl[ 1 ];
	if( pvt[ 1 ] > pvh[ 1 ] )pvt[ 1 ] = pvh[ 1 ];
	if( pvt[ 2 ] < pvl[ 2 ] )pvt[ 2 ] = pvl[ 2 ];
	if( pvt[ 2 ] > pvh[ 2 ] )pvt[ 2 ] = pvh[ 2 ];
	return true;
}

///// ///// unsafe �ŁA static �Ȋ֐��B ///// /////
/*
*	@brief �x�N�g�����R�s�[����
*	@param[in] in ��
*	@param[in] out �R�s�[��
*/
void CVector3H::Copy( double *in, double *out )
{
	memcpy( out, in, 3 * sizeof( double ) );
}

/*
*	@brief �x�N�g���P�A�Q�̑����Z���s��
*	@param[in] in1 �x�N�g���P
*	@param[in] in2 �x�N�g���Q
*	@param[out] out �v�Z����
*/
void CVector3H::Add( double *in1, double *in2, double *out )
{
	out[ 0 ] = in1[ 0 ] + in2[ 0 ];
	out[ 1 ] = in1[ 1 ] + in2[ 1 ];
	out[ 2 ] = in1[ 2 ] + in2[ 2 ];
}

/*
*	@brief �x�N�g���P�A�Q�̈����Z���s��
*	@param[in] inLeft �x�N�g���P
*	@param[in] inRight �x�N�g���Q
*	@param[out] out �v�Z����
*/
void CVector3H::Sub( double *in1, double *in2, double *out )
{
	out[ 0 ] = in1[ 0 ] - in2[ 0 ];
	out[ 1 ] = in1[ 1 ] - in2[ 1 ];
	out[ 2 ] = in1[ 2 ] - in2[ 2 ];
}

/*
*	@brief �x�N�g���P�ƃx�N�g���Q�̓��ς����ߕԂ�
*	@param[in] inLeft �x�N�g���P
*	@param[in] inRight �x�N�g���Q
*	@return ���ς̒l
*/
double CVector3H::DotProduct( double *in1, double *in2 )
{
	return in1[ 0 ] * in2[ 0 ] + in1[ 1 ] * in2[ 1 ] + in1[ 2 ] * in2[ 2 ];
}

/*
*	@brief �x�N�g���P�ƃx�N�g���Q�̊O�ς����߂�
*	@param[in] inLeft �x�N�g���P
*	@param[in] inRight �x�N�g���Q
*	@param[out] out �v�Z����
*/
void CVector3H::CrossProduct( double *inLeft, double *inRight, double *out )
{
	out[ 0 ] = inLeft[ 1 ]*inRight[ 2 ] - inRight[ 1 ]*inLeft[ 2 ];
	out[ 1 ] = inLeft[ 2 ]*inRight[ 0 ] - inRight[ 2 ]*inLeft[ 0 ];
	out[ 2 ] = inLeft[ 0 ]*inRight[ 1 ] - inRight[ 0 ]*inLeft[ 1 ];
}

/*
*	@brief �x�N�g���ɒ萔���|���A���̌��ʂ����g�Ɋi�[����
*	@param[in] c �萔
*	@param[in] in �x�N�g��
*	@param[out] out �v�Z����
*/
void CVector3H::MulConst( double c, double *in, double *out )
{
	out[ 0 ] = c * in[ 0 ];
	out[ 1 ] = c * in[ 1 ];
	out[ 2 ] = c * in[ 2 ];
}

/*
*	@brief �x�N�g���𐳋K������
*	@param[in] in �x�N�g��
*	@param[out] out �v�Z����
*/
bool CVector3H::Normalize( double *in, double *out )
{
	double length;
	length = in[0] * in[0] + in[1] * in[1] + in[2] * in[2];
	if( length == 0.0 )
	{
		out[ 0 ] = 0.0;
		out[ 1 ] = 0.0;
		out[ 2 ] = 0.0;
		return false;
	}

	length = sqrt( length );
	out[ 0 ] = in[ 0 ] / length;
	out[ 1 ] = in[ 1 ] / length;
	out[ 2 ] = in[ 2 ] / length;
	return true;
}

/*
*	@brief �x�N�g���̒��������߂�
*	@param[in] in �x�N�g��
*	@return ����
*/
double CVector3H::Length( double *in )
{
	return sqrt( in[0] * in[0] + in[1] * in[1] + in[2] * in[2] );
}

/*
*	@brief �x�N�g���P�A�Q�̋��������ߕԂ�
*	@param[in] in1 �x�N�g���P
*	@param[in] in2 �x�N�g���Q
*/
double CVector3H::Distance( double *in1, double *in2 )
{
	double amt, tmp;
	tmp = in1[ 0 ] - in2[ 0 ];
	amt = tmp * tmp;
	tmp = in1[ 1 ] - in2[ 1 ];
	amt += tmp * tmp;
	tmp = in1[ 2 ] - in2[ 2 ];
	amt += tmp * tmp;
	return sqrt( amt );
}

/*
*	@brief �O�_�̉E�˂������̖@�������߂�
*	@param[in] in1 �_�P
*	@param[in] in2 �_�Q
*	@param[in] in3 �_�R
*	@param[out] out �v�Z����
*/
bool CVector3H::Normal( double *in1, double *in2, double *in3, double *out )
{
	double v31[ 3 ], v21[ 3 ], nrm[ 3 ];
	CVector3H::Sub( in3, in1, v31 );
	CVector3H::Sub( in2, in1, v21 );
	CVector3H::CrossProduct( v21, v31, nrm );
	return CVector3H::Normalize( nrm, out );
}

/*
*	@brief �O�_�̒��S�_�����߂�
*	@param[in] in1 �_�P
*	@param[in] in2 �_�Q
*	@param[in] in3 �_�R
*	@param[out] out �v�Z����
*/
void CVector3H::Center( double *in1, double *in2, double *in3, double *out )
{
	out[ 0 ] = ( in1[ 0 ] + in2[ 0 ] + in3[ 0 ] ) / 3.0;
	out[ 1 ] = ( in1[ 1 ] + in2[ 1 ] + in3[ 1 ] ) / 3.0;
	out[ 2 ] = ( in1[ 2 ] + in2[ 2 ] + in3[ 2 ] ) / 3.0;
}

/*
*	@brief �x�N�g���̒l�������llow,����lhigh�𒴂��Ȃ��悤�ɒ�������
*	@param[in] low ����
*	@param[in] high ���
*/
void CVector3H::Limit( double *inout, double *low, double *high )
{
	if( inout[ 0 ] < low[ 0 ] )inout[ 0 ] = low[ 0 ];
	if( inout[ 0 ] > high[ 0 ] )inout[ 0 ] = high[ 0 ];
	if( inout[ 1 ] < low[ 1 ] )inout[ 1 ] = low[ 1 ];
	if( inout[ 1 ] > high[ 1 ] )inout[ 1 ] = high[ 1 ];
	if( inout[ 2 ] < low[ 2 ] )inout[ 2 ] = low[ 2 ];
	if( inout[ 2 ] > high[ 2 ] )inout[ 2 ] = high[ 2 ];
}

/*!
	@brief Dir(0,1,0),Nrm(0,0,1)���AvDir,vNrm�̒l����邽�߂̉�]�p�����߂�
	@param[in] vDir �Ď��s��
	@param[in] vNrm �Ď��s��
	@return �Ď��s��ւ̃|�C���^
*/
void CVector3H::CalcAngleFromDirNrm_YXZ( CVector3H* vDir, CVector3H* vNrm )
{
	double *dir = vDir->GetData(), *nrm=vNrm->GetData();
	double radianTo360 = 180.0 / 3.14159265;
	double angleX, angleY, angleZ, cosX, sinX, cosY, sinY;
	if(nrm[0] == 0.0 || nrm[2] == 0.0){
		if(nrm[1]>0){ angleX = -90.0; }else{ angleX = 90.0; }
		sinX = sin(angleX / radianTo360); cosX = cos(angleX / radianTo360);
		angleZ = 90.0;
		angleY = radianTo360 * atan2( dir[2], -dir[0] );
	}else{
		angleX = radianTo360 * asin(-nrm[1]);
		sinX = sin(angleX/ radianTo360); cosX = cos(angleX / radianTo360);
		angleY = radianTo360 * atan2( nrm[0], nrm[2]);
		sinY = sin(angleY / radianTo360); cosY = cos(angleY / radianTo360);
		angleZ = radianTo360 * atan2( -cosX * (dir[0]*cosY - dir[2]*sinY), dir[1] );
	}
	SetData( angleX, angleY, angleZ );
}