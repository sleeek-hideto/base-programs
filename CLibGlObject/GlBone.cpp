#include "StdAfx.h"
#include ".\glbone.h"

/*!
	@brief ボーンに貼り付けれたスキンを描画する。ボーンの子も再帰的に描画される
	@param[in] type どのスキン・タイプを描画するか
*/
void CGlBone::Draw_recursive( char* type )
{
	glPushMatrix();

	if( m_BoneParent == NULL ){
		double *pPos = m_PosLocal.GetData();
		glTranslated( pPos[0], pPos[1], pPos[2] );

		double *pAngLoc = m_AngLocal.GetData();
		double *pAngIni = m_AngInit.GetData();
		glRotated( pAngLoc[1] + pAngIni[1], 0.0, 1.0, 0.0 );
		glRotated( pAngLoc[0] + pAngIni[0], 1.0, 0.0, 0.0 );
		glRotated( pAngLoc[2] + pAngIni[2], 0.0, 0.0, 1.0 );

	}else{
		/*
		double *pAng = m_AngLocal.GetData();
		glRotated( pAng[1], 0.0, 1.0, 0.0 );
		glRotated( pAng[0], 1.0, 0.0, 0.0 );
		glRotated( pAng[2], 0.0, 0.0, 1.0 );
		
		double *pPos = m_PosLocal.GetData();
		glTranslated( pPos[0], pPos[1], pPos[2] );
		
		pAng = m_AngInit.GetData();
		glRotated( pAng[1], 0.0, 1.0, 0.0 );
		glRotated( pAng[0], 1.0, 0.0, 0.0 );
		glRotated( pAng[2], 0.0, 0.0, 1.0 );
		*/
		double *pAngLoc = m_AngLocal.GetData();
		double *pAngIni = m_AngInit.GetData();
		glRotated( pAngIni[1], 0.0, 1.0, 0.0 );
		glRotated( pAngIni[0], 1.0, 0.0, 0.0 );
		glRotated( pAngIni[2], 0.0, 0.0, 1.0 );

		glRotated( pAngLoc[1], 0.0, 1.0, 0.0 );
		glRotated( pAngLoc[0], 1.0, 0.0, 0.0 );
		glRotated( pAngLoc[2], 0.0, 0.0, 1.0 );
				
		double *pPos = m_PosLocal.GetData();
		glTranslated( pPos[0], pPos[1], pPos[2] );

	}

	CGlSkin *pS = GetSkin();
	while( pS != NULL )
	{
		pS->Draw( type );	//種類ごとによる描き分けをする
		pS = pS->GetSibling();
	}

	CGlBone *pB = GetBoneChild();
	while( pB != NULL )
	{
		pB->Draw_recursive( type );
		pB = pB->GetBoneSibling();
	}

	glPopMatrix();
}

/*!
	@brief ボーンに貼り付けれたスキンを描画する。再帰的には描写されない。
		この関数を呼び出す前に CalcPosture() を呼び出して個々の骨の世界座標を計算させること！
	@param[in] type どのスキン・タイプを描画するか
*/
void CGlBone::Draw_oneBone( char* type )
{
	glPushMatrix();

	glMultTransposeMatrixd( m_ProGlobal.GetData() );

	CGlSkin *pS = GetSkin();
	while( pS != NULL )
	{
		pS->Draw( type );	//種類ごとによる描き分けをする
		pS = pS->GetSibling();
	}

	glPopMatrix();
}

/*!
	@brief 自身だけのボーンの世界座標系の位置・方向などを計算する。必ず_recursiveから呼び出すこと
*/
void CGlBone::CalcPosture()
{
	if( m_BoneParent == NULL )
	{
		double *angLoc = m_AngLocal.GetData();
		double *angIni = m_AngInit.GetData();
		m_ProLocal.SetRotYXZTran( angLoc[0]+angIni[0], angLoc[1]+angIni[1], angLoc[2]+angIni[2], &m_PosLocal );
		m_ProGlobal.Copy( &m_ProLocal );

		m_PosGlobalCenter.SetData( 0.0, 0.0, 0.0 );
		m_PosGlobalEdge.Copy( &m_PosLocal );
	}else{
		double *angLoc = m_AngLocal.GetData();
		double *angIni = m_AngInit.GetData();
		CMatrix4x4 proTmp, matRot;
		/*
		proTmp.SetRotYXZxTran( angLoc[0], angLoc[1], angLoc[2], &m_PosLocal );
		matRot.SetRotYXZTran( angIni[0], angIni[1], angIni[2] );
		m_ProLocal.Mul( &proTmp, &matRot );
		*/
		proTmp.SetRotYXZTran( angIni[0], angIni[1], angIni[2] );
		matRot.SetRotYXZxTran( angLoc[0], angLoc[1], angLoc[2], &m_PosLocal );
		m_ProLocal.Mul( &proTmp, &matRot );
		
		m_ProGlobal.Mul( &m_BoneParent->m_ProGlobal, &m_ProLocal );

		m_PosGlobalCenter.Copy( &m_BoneParent->m_PosGlobalEdge );
		m_ProGlobal.GetDataCol( 3, m_PosGlobalEdge.GetData() );
	}

	m_ProGlobal.GetCol( 0, &m_AxisX );
	m_ProGlobal.GetCol( 1, &m_AxisY );
	m_ProGlobal.GetCol( 2, &m_AxisZ );
/*
	m_AxisX.SetData( m_ProGlobal.GetData( 0 ) );
	m_AxisY.SetData( m_ProGlobal.GetData( 1 ) );
	m_AxisZ.SetData( m_ProGlobal.GetData( 2 ) );
*/
}

/*!
	@brief 自身のボーンおよび子の世界座標系の位置・方向などを再帰的に求める。
*/
void CGlBone::CalcPosture_recursive()
{
	CalcPosture();

	CGlBone *pBone = GetBoneChild();
	while( pBone != NULL )
	{
		pBone->CalcPosture_recursive();
		pBone = pBone->GetBoneSibling();
	}
}

/*!
	@brief 角度を設定する
	@param v 角度ベクトル
*/
void CGlBone::SetAngle( CVector3H *v )
{
	m_AngLocal.Copy( v );
	double *a=m_AngLocal.GetData();
	if( a[0] > 180.0 )a[0] -= 360.0; if( a[0] < -180.0 )a[0] += 360.0;
	if( a[1] > 180.0 )a[1] -= 360.0; if( a[1] < -180.0 )a[1] += 360.0;
	if( a[2] > 180.0 )a[2] -= 360.0; if( a[2] < -180.0 )a[2] += 360.0;
	m_AngLocal.Limit( &m_AngLower, &m_AngHigher );
}
void CGlBone::SetAngle( double *d )
{
	m_AngLocal.SetData( d );
	double *a=m_AngLocal.GetData();
	if( a[0] > 180.0 )a[0] -= 360.0; if( a[0] < -180.0 )a[0] += 360.0;
	if( a[1] > 180.0 )a[1] -= 360.0; if( a[1] < -180.0 )a[1] += 360.0;
	if( a[2] > 180.0 )a[2] -= 360.0; if( a[2] < -180.0 )a[2] += 360.0;
	m_AngLocal.Limit( &m_AngLower, &m_AngHigher  );
}

/*!
	@brief 角度を足す
	@param v 角度ベクトル
*/
void CGlBone::AddAngle( CVector3H *v )
{
	m_AngLocal.Add( v );
	double *a=m_AngLocal.GetData();
	if( a[0] > 180.0 )a[0] -= 360.0; if( a[0] < -180.0 )a[0] += 360.0;
	if( a[1] > 180.0 )a[1] -= 360.0; if( a[1] < -180.0 )a[1] += 360.0;
	if( a[2] > 180.0 )a[2] -= 360.0; if( a[2] < -180.0 )a[2] += 360.0;
	m_AngLocal.Limit( &m_AngLower, &m_AngHigher );
}

/*!
	@brief 角度を足す
	@param d 角度ベクトル
*/
void CGlBone::AddAngle( double *d )
{
	m_AngLocal.AddData( d );
	double *a=m_AngLocal.GetData();
	if( a[0] > 180.0 )a[0] -= 360.0; if( a[0] < -180.0 )a[0] += 360.0;
	if( a[1] > 180.0 )a[1] -= 360.0; if( a[1] < -180.0 )a[1] += 360.0;
	if( a[2] > 180.0 )a[2] -= 360.0; if( a[2] < -180.0 )a[2] += 360.0;
	m_AngLocal.Limit( &m_AngLower, &m_AngHigher );
}