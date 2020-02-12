#include "StdAfx.h"
#include ".\matrix4x4.h"

/*
*	@brief ‚wA‚xA‚y•ûŒü‚ÉƒXƒP[ƒ‹‚³‚¹‚és—ñ‚ğ‹‚ß‚é
*	@param[in] scaleX ‚w•ûŒü‚ÌkÚ
*	@param[in] scaleY ‚x•ûŒü‚ÌkÚ
*	@param[in] scaleZ ‚y•ûŒü‚ÌkÚ
*/
void CMatrix4x4::SetScale(double scaleX, double scaleY, double scaleZ )
{
	SetDataAllZero();
	double *pData = GetData();
	pData[ 0 ] = scaleX;
	pData[ 5 ] = scaleY;
	pData[ 10 ] = scaleZ;
	pData[ 15 ] = 1.0;
}

/*
*	@brief ‚w²‚ğ‰ñ“]‚³‚¹‚½‚Ì‰ñ“]s—ñ‚ğ‹‚ß‚é
*	@param[in] angX Šp“x
*/
void CMatrix4x4::SetRotX(double angX)
{
	SetDataAllZero();
	double cosX = cos(angX * PI / 180.0);
	double sinX = sin(angX * PI / 180.0);
	double* pRot = GetData();
	pRot[0]=1.0;
	pRot[4*1+1] = cosX;	pRot[4*1+2] = -sinX;
	pRot[4*2+1] = sinX;	pRot[4*2+2] = cosX;
	pRot[15]=1.0;
}

/*
*	@brief ‚x²‚ğ‰ñ“]‚³‚¹‚½‚Ì‰ñ“]s—ñ‚ğ‹‚ß‚é
*	@param[in] angY Šp“x
*/
void CMatrix4x4::SetRotY(double angY)
{
	SetDataAllZero();
	double cosY = cos(angY * PI / 180.0);
	double sinY = sin(angY * PI / 180.0);
	double* pRot = GetData();
	pRot[0] = cosY;	pRot[2] = sinY;
	pRot[4*1+1] = 1.0;
	pRot[4*2+0] = -sinY;	pRot[4*2+2] = cosY;
	pRot[15] = 1.0;
}

/*
*	@brief ‚y²‚ğ‰ñ“]‚³‚¹‚½‚Ì‰ñ“]s—ñ‚ğ‹‚ß‚é
*	@param[in] angX Šp“x
*/
void CMatrix4x4::SetRotZ(double angZ)
{
	SetDataAllZero();
	double cosZ = cos(angZ * PI / 180.0);
	double sinZ = sin(angZ * PI / 180.0);
	double* pRot = GetData();
	pRot[0] = cosZ;	pRot[1] = -sinZ;
	pRot[4*1] = sinZ;	pRot[4*1+1] = cosZ;
	pRot[4*2+2] = 1.0;
	pRot[15] = 1.0;
}

/*
*	@brief •Àis—ñ‚ğ‹‚ß‚é
*	@param[in] vt ˆÚ“®—Ê
*/
void CMatrix4x4::SetTran(CVector3H *vt)
{
	SetDataAllZero();
	double *pTran = vt->GetData();
	double *pD = GetData();
	pD[0] = 1.0;
	pD[5] = 1.0;
	pD[10] = 1.0;
	pD[15] = 1.0;
	pD[3] = pTran[0];	pD[7] = pTran[1];	pD[11] = pTran[2];
}

/*
	@brief ‚x²A‚w²A‚y²‚ğ‰ñ“]‚³‚¹AŸ‚É•Ài‚µ‚½Û‚Ì•ÏŠ·s—ñ‚ğ‹‚ß‚é
	@param[in] angX Šp“x
	@param[in] angY Šp“x
	@param[in] angZ Šp“x
	@param[in] vt ˆÚ“®s—ñ
*/
void CMatrix4x4::SetRotYXZTran(double angX, double angY, double angZ, CVector3H *vt)
{
	double cosX = cos(angX * PI / 180.0);
	double sinX = sin(angX * PI / 180.0);
	double cosY = cos(angY * PI / 180.0);
	double sinY = sin(angY * PI / 180.0);
	double cosZ = cos(angZ * PI / 180.0);
	double sinZ = sin(angZ * PI / 180.0);
	double *pD = GetData();
	pD[0] = cosY*cosZ + sinY*sinX*sinZ;	pD[1] = -cosY*sinZ + sinY*sinX*cosZ;	pD[2] = sinY*cosX;
	pD[4] = cosX*sinZ;	pD[5] = cosX*cosZ;	pD[6] = -sinX;
	pD[8] = -sinY*cosZ + cosY*sinX*sinZ;	pD[9] = sinY*sinZ + cosY*sinX*cosZ;	pD[10] = cosY*cosX;
	pD[12] = 0.0;	pD[13] = 0.0;	pD[14] = 0.0;	pD[15] = 1.0;
	if( vt != NULL )
	{
		double *pTran = vt->GetData();
		pD[3] = pTran[0]; pD[7] = pTran[1];	pD[11] = pTran[2];
	}else{
		pD[3] = 0.0; pD[7] = 0.0;	pD[11] = 0.0;
	}
}

/*
	@brief •Ài‚³‚¹A‚»‚ÌŸ‚É‚x²A‚w²A‚y²‚ğ‰ñ“]‚³‚¹‚½Û‚Ì•ÏŠ·s—ñ‚ğ‹‚ß‚é
	@param[in] angX Šp“x
	@param[in] angY Šp“x
	@param[in] angZ Šp“x
	@param[in] vt ˆÚ“®s—ñ
*/
void CMatrix4x4::SetRotYXZxTran(double angX, double angY, double angZ, CVector3H *vt)
{
	CMatrix4x4 matRot, matTran;
	matTran.SetTran( vt );
	matRot.SetRotYXZTran( angX, angY, angZ );
	Mul( &matRot, &matTran );
}
