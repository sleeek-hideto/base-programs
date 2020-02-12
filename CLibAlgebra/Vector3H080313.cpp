#include "StdAfx.h"
#include ".\vector3h.h"

/*
*	@brief ベクトル１、２の足し算を行い、その結果を自身に格納する
*	@param[in] in1 ベクトル１
*	@param[in] in2 ベクトル２
*/
bool CVector3H::Add( CVector3H *in1, CVector3H *in2 )
{
	if( !IsSameSize( in1 ) || !IsSameSize( in2 ) )return false;

	cvAdd( in1->GetCvMat(), in2->GetCvMat(), this->GetCvMat() );
	SetData( 3, 1.0 );
	return true;
}

/*
*	@brief ベクトル１、２の引き算を行い、その結果を自身に格納する
*	@param[in] inLeft ベクトル１
*	@param[in] inRight ベクトル２
*/
bool CVector3H::Sub( CVector3H *inLeft, CVector3H *inRight )
{
	if( !IsSameSize( inLeft ) || !IsSameSize( inRight ) )return false;
	
	cvSub( inLeft->GetCvMat(), inRight->GetCvMat(), this->GetCvMat() );
	SetData( 3, 1.0 );
	return true;
}

/*
*	@brief 行列１、２の掛け算を行い、その結果を自身に格納する
*	@param[in] inLeft 行列１
*	@param[in] inRight 行列２
*/
bool CVector3H::Mul( CMatrix *inLeft, CMatrix *inRight )
{
	cvMatMul( inLeft->GetCvMat(), inRight->GetCvMat(), this->GetCvMat() );
	return true;
}

/*
*	@brief ベクトル１とベクトル２の内積を求め返す
*	@param[in] in1 ベクトル１
*	@param[in] in2 ベクトル２
*/
double CVector3H::DotProduct( CVector3H *in1, CVector3H *in2 )
{
	double *pd1 = in1->GetData(), *pd2 = in2->GetData();
	return pd1[ 0 ] * pd2[ 0 ] + pd1[ 1 ] * pd2[ 1 ] + pd1[ 2 ] * pd2[ 2 ];
}

/*
*	@brief ベクトル左とベクトル右の外積を求め、その結果を自身に格納する
*	@param[in] inLeft ベクトル左
*	@param[in] inRight ベクトル右
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
*	@brief ベクトルを転置し、その結果を自身に格納する
*	@param[in] in1 ベクトル
*/
bool CVector3H::Transpose( CMatrix *in )
{
	cvTranspose( in->GetCvMat(), this->GetCvMat() );
	return true;
}

/*
*	@brief ベクトルに定数を掛け、その結果を自身に格納する
*	@param[in] c 定数
*	@param[in] in ベクトル
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
*	@brief ベクトルの要素ごとに、かける
*	@param[in] in1, in2 ベクトル
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
*	@brief ベクトルを正規化する
*	@param[in] in ベクトル
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
*	@brief ベクトルの長さを求める
*	@param[in] in ベクトル
*	@return 長さ
*/
double CVector3H::Length()
{
	double *pvi = GetData();
	return sqrt( pvi[0] * pvi[0] + pvi[1] * pvi[1] + pvi[2] * pvi[2] );
}

/*
*	@brief ベクトル間の距離を求める
*	@param[in] in1 ベクトル
*	@param[in] in2 ベクトル
*	@return 距離
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
*	@brief 三点の右ねじ方向の法線を求める
*	@param[in] in1 点１
*	@param[in] in2 点２
*	@param[in] in3 点３
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
*	@brief 三点の中心点を求める
*	@param[in] in1 点１
*	@param[in] in2 点２
*	@param[in] in3 点３
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
*	@brief ベクトルの値が下限値low,上限値highを超えないように調整する
*	@param[in] low 下限
*	@param[in] high 上限
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

///// ///// unsafe で、 static な関数達 ///// /////
/*
*	@brief ベクトルをコピーする
*	@param[in] in 元
*	@param[in] out コピー先
*/
void CVector3H::Copy( double *in, double *out )
{
	memcpy( out, in, 3 * sizeof( double ) );
}

/*
*	@brief ベクトル１、２の足し算を行う
*	@param[in] in1 ベクトル１
*	@param[in] in2 ベクトル２
*	@param[out] out 計算結果
*/
void CVector3H::Add( double *in1, double *in2, double *out )
{
	out[ 0 ] = in1[ 0 ] + in2[ 0 ];
	out[ 1 ] = in1[ 1 ] + in2[ 1 ];
	out[ 2 ] = in1[ 2 ] + in2[ 2 ];
}

/*
*	@brief ベクトル１、２の引き算を行う
*	@param[in] inLeft ベクトル１
*	@param[in] inRight ベクトル２
*	@param[out] out 計算結果
*/
void CVector3H::Sub( double *in1, double *in2, double *out )
{
	out[ 0 ] = in1[ 0 ] - in2[ 0 ];
	out[ 1 ] = in1[ 1 ] - in2[ 1 ];
	out[ 2 ] = in1[ 2 ] - in2[ 2 ];
}

/*
*	@brief ベクトル１とベクトル２の内積を求め返す
*	@param[in] inLeft ベクトル１
*	@param[in] inRight ベクトル２
*	@return 内積の値
*/
double CVector3H::DotProduct( double *in1, double *in2 )
{
	return in1[ 0 ] * in2[ 0 ] + in1[ 1 ] * in2[ 1 ] + in1[ 2 ] * in2[ 2 ];
}

/*
*	@brief ベクトル１とベクトル２の外積を求める
*	@param[in] inLeft ベクトル１
*	@param[in] inRight ベクトル２
*	@param[out] out 計算結果
*/
void CVector3H::CrossProduct( double *inLeft, double *inRight, double *out )
{
	out[ 0 ] = inLeft[ 1 ]*inRight[ 2 ] - inRight[ 1 ]*inLeft[ 2 ];
	out[ 1 ] = inLeft[ 2 ]*inRight[ 0 ] - inRight[ 2 ]*inLeft[ 0 ];
	out[ 2 ] = inLeft[ 0 ]*inRight[ 1 ] - inRight[ 0 ]*inLeft[ 1 ];
}

/*
*	@brief ベクトルに定数を掛け、その結果を自身に格納する
*	@param[in] c 定数
*	@param[in] in ベクトル
*	@param[out] out 計算結果
*/
void CVector3H::MulConst( double c, double *in, double *out )
{
	out[ 0 ] = c * in[ 0 ];
	out[ 1 ] = c * in[ 1 ];
	out[ 2 ] = c * in[ 2 ];
}

/*
*	@brief ベクトルを正規化する
*	@param[in] in ベクトル
*	@param[out] out 計算結果
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
*	@brief ベクトルの長さを求める
*	@param[in] in ベクトル
*	@return 長さ
*/
double CVector3H::Length( double *in )
{
	return sqrt( in[0] * in[0] + in[1] * in[1] + in[2] * in[2] );
}

/*
*	@brief ベクトル１、２の距離を求め返す
*	@param[in] in1 ベクトル１
*	@param[in] in2 ベクトル２
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
*	@brief 三点の右ねじ方向の法線を求める
*	@param[in] in1 点１
*	@param[in] in2 点２
*	@param[in] in3 点３
*	@param[out] out 計算結果
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
*	@brief 三点の中心点を求める
*	@param[in] in1 点１
*	@param[in] in2 点２
*	@param[in] in3 点３
*	@param[out] out 計算結果
*/
void CVector3H::Center( double *in1, double *in2, double *in3, double *out )
{
	out[ 0 ] = ( in1[ 0 ] + in2[ 0 ] + in3[ 0 ] ) / 3.0;
	out[ 1 ] = ( in1[ 1 ] + in2[ 1 ] + in3[ 1 ] ) / 3.0;
	out[ 2 ] = ( in1[ 2 ] + in2[ 2 ] + in3[ 2 ] ) / 3.0;
}

/*
*	@brief ベクトルの値が下限値low,上限値highを超えないように調整する
*	@param[in] low 下限
*	@param[in] high 上限
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
	@brief Dir(0,1,0),Nrm(0,0,1)が、vDir,vNrmの値を取るための回転角を求める
	@param[in] vDir 斉次行列
	@param[in] vNrm 斉次行列
	@return 斉次行列へのポインタ
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