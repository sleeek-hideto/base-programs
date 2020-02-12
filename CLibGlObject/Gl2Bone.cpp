#include "StdAfx.h"
#include "Gl2Bone.h"

/////
/*!
	@brief ボーンの世界座標だけでなく、ボーンに貼り付けれたスキンのプリミティブの世界座標も計算する。
*/
void CGl2Bone::CalcPosture()
{
	CGlBone::CalcPosture();

	CGl2Skin *pSkin = GetSkin();
	while( pSkin != NULL )
	{
		if( pSkin->m_ArrPos == NULL ){	///// 初期化されていないスキンは飛ばす
			pSkin = (CGl2Skin*)pSkin->m_SkinSibling;
			continue;
		}
		CGl2Skin *pSkinDeformed = pSkin->GetSkinDeformed();
		CGl2Skin *pSkinWorld = pSkin->GetSkinWorld();

		if( pSkinDeformed != NULL && pSkinWorld == NULL )
		{
			pSkinDeformed->m_ArrPos->ArrayMul( pSkin->m_ArrPos, pSkinDeformed->GetMatDeformed() );
			
			CMatrix4x4 matRot;
			matRot.Copy( pSkinDeformed->GetMatDeformed() );
			matRot.SetData( 0, 3, 0.0 );
			matRot.SetData( 1, 3, 0.0 );
			matRot.SetData( 2, 3, 0.0 );
			pSkinDeformed->m_ArrNrm->ArrayMulAndSqrt( pSkin->m_ArrNrm, &matRot );
		}

		if( pSkinDeformed == NULL && pSkinWorld != NULL )
		{
			pSkinWorld->m_ArrPos->ArrayMul( pSkin->m_ArrPos, &m_ProGlobal );
		}

		if( pSkinDeformed != NULL && pSkinWorld != NULL )
		{
			pSkinDeformed->m_ArrPos->ArrayMul( pSkin->m_ArrPos, pSkinDeformed->GetMatDeformed() );
			pSkinWorld->m_ArrPos->ArrayMul( pSkinDeformed->m_ArrPos, &m_ProGlobal );

			CMatrix4x4 matRot;
			matRot.Copy( pSkinDeformed->GetMatDeformed() );
			matRot.SetData( 0, 3, 0.0 );
			matRot.SetData( 1, 3, 0.0 );
			matRot.SetData( 2, 3, 0.0 );
			pSkinDeformed->m_ArrNrm->ArrayMulAndSqrt( pSkin->m_ArrNrm, &matRot );
		}

		pSkin = (CGl2Skin*)pSkin->m_SkinSibling;
	}
}

/*!
	@brief ボーンに貼り付けれたスキンを描画する。ボーンの子も再帰的に描画される
	@param[in] type どのスキン・タイプを描画するか
*/
void CGl2Bone::Draw_recursive( char *type )
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
		double *pAng;
		pAng = m_AngLocal.GetData();
		glRotated( pAng[1], 0.0, 1.0, 0.0 );
		glRotated( pAng[0], 1.0, 0.0, 0.0 );
		glRotated( pAng[2], 0.0, 0.0, 1.0 );	

		double *pPos = m_PosLocal.GetData();
		glTranslated( pPos[0], pPos[1], pPos[2] );

		pAng = m_AngInit.GetData();
		glRotated( pAng[1], 0.0, 1.0, 0.0 );
		glRotated( pAng[0], 1.0, 0.0, 0.0 );
		glRotated( pAng[2], 0.0, 0.0, 1.0 );
	}

	CGl2Skin *pSkin = GetSkin();
	while( pSkin != NULL )
	{
		CGl2Skin *pSkinDeformed = pSkin->GetSkinDeformed();
		CGl2Skin *pSkinWorld = pSkin->GetSkinWorld();

		if( type == NULL || pSkin->IsType( type ) )
		{
			if( pSkinDeformed != NULL ){
				pSkinDeformed->Draw();
			}else{
				pSkin->Draw();
			}
		}
		
		pSkin = pSkin->GetSibling();
	}

	CGlBone *pB = GetBoneChild();
	while( pB != NULL )
	{
		pB->Draw_recursive( type );
		pB = pB->GetBoneSibling();
	}

	glPopMatrix();
}
