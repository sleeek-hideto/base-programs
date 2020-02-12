#include "StdAfx.h"
#include "GlSolverIK.h"

void CGlSolverIkTargetChain::LoadFlexibilityBones( Ch3PlainData *data )
{
	data->SetNameSpace( "Bone" );///// 注意かも
	int boneMax = m_Arthro->GetBoneMax();
	for( int b=0; b<boneMax; b++ )
	{
		double vec[ 4 ];
		char namePart[ 128 ];
		sprintf( namePart, "ikFlex" );
		if( data->Get( b, namePart, vec ) > 2 ) SetFlexibilityBone( b, vec );
	}
}

///// ///// 具体的な制約 ///// /////

///// その場所に位置する
///// F = | Pend - Pput |^2 ;
void CGlSolverIkConstraint_Put::Potential(double &valOut)
{
	valOut = 0.0;
	if( m_WeightConstraint == 0.0 ) return;

	int *ends = m_Target->GetChainEnds();
	int endsMax = m_Target->GetChainEndsMax();
	CGlArthro *arthro = m_Target->GetArthro();

	for( int c=0; c<endsMax; c++ ){
		double dis = CVector3H::Distance( arthro->GetBonePosEdge( ends[ c ] ), &m_PosPut );
		valOut += dis * dis;
	}

	valOut *= m_WeightConstraint;
}
///// d F = 2 ( Pend - Pput ) * d Pend ,
///// d Pend = axis_i *cross* ( Pend - P_i ) ;
void CGlSolverIkConstraint_Put::DerPotential(CMatrix *vecOut)
{
	if( m_WeightConstraint == 0.0 ){
		vecOut->SetDataAllZero();
		return;
	}

	int *chain = m_Target->GetChain();
	int chainMax = m_Target->GetChainMax();
	CGlArthro *arthro = m_Target->GetArthro();
	CVector3H *posEnd = arthro->GetBonePosEdge( chain[ chainMax -1 ] );

	CVector3H vCom, vTmp;
	vCom.Sub( posEnd, &m_PosPut );
	vCom.MulConst( 2.0 );

	for( int c=0; c<chainMax; c++ )
	{
		CVector3H *posNode = arthro->GetBonePosCenter( chain[ c ] );
		vTmp.Sub( posEnd, posNode );

		CVector3H derPos;
		derPos.CrossProduct( arthro->GetPartXaxis(c), &vTmp );
		vecOut->SetData( c, 0,
			CVector3H::DotProduct( &vCom, &derPos ) * m_Target->GetFlexibilityXBone( chain[ c ] )
			);
		derPos.CrossProduct( arthro->GetPartYaxis(c), &vTmp );
		vecOut->SetData( c, 1,
			CVector3H::DotProduct( &vCom, &derPos ) * m_Target->GetFlexibilityYBone( chain[ c ] )
			);
		derPos.CrossProduct( arthro->GetPartZaxis(c), &vTmp );
		vecOut->SetData( c, 2,
			CVector3H::DotProduct( &vCom, &derPos ) * m_Target->GetFlexibilityZBone( chain[ c ] )
			);
	}
}

///// その方向を向く
///// F = | Vend - Vface |^2, Vend = Vaxis_z
void CGlSolverIkConstraint_Face::Potential(double &valOut)
{
	valOut = 0.0;
	if( m_WeightConstraint == 0.0 ) return;

	int *ends = m_Target->GetChainEnds();
	int endsMax = m_Target->GetChainEndsMax();
	CGlArthro *arthro = m_Target->GetArthro();

	for( int c=0; c<endsMax; c++ ){
		double dis = CVector3H::Distance( arthro->GetPartZaxis( ends[ c ] ), &m_VecFace );
		valOut += dis * dis;
	}

	valOut *= m_WeightConstraint;
}
///// d F = 2 ( Vend - Vface ) * d Vend,
///// d Vend = axis_i *cross* Vend
void CGlSolverIkConstraint_Face::DerPotential(CMatrix *vecOut)
{
	if( m_WeightConstraint == 0.0 ){
		vecOut->SetDataAllZero();
		return;
	}

	int *chain = m_Target->GetChain();
	int chainMax = m_Target->GetChainMax();
	CGlArthro *arthro = m_Target->GetArthro();
	CVector3H *vecEnd = arthro->GetPartZaxis( chain[ chainMax -1 ] );

	CVector3H vCom, vTmp;
	vCom.Sub( vecEnd, &m_VecFace );
	vCom.MulConst( 2.0 );

	for( int c=0; c<chainMax; c++ )
	{
		CVector3H derVec;
		derVec.CrossProduct( arthro->GetPartXaxis(c), vecEnd );
		vecOut->SetData( c, 0,
			CVector3H::DotProduct( &vCom, &derVec ) * m_Target->GetFlexibilityXBone( chain[ c ] )
			);
		derVec.CrossProduct( arthro->GetPartYaxis(c), vecEnd );
		vecOut->SetData( c, 1,
			CVector3H::DotProduct( &vCom, &derVec ) * m_Target->GetFlexibilityYBone( chain[ c ] )
			);
		derVec.CrossProduct( arthro->GetPartZaxis(c), vecEnd );
		vecOut->SetData( c, 2,
			CVector3H::DotProduct( &vCom, &derVec ) * m_Target->GetFlexibilityZBone( chain[ c ] )
			);
	}
}

///// 仮想物体の表面に接する
///// F = Rput * | Pend - Psur |^2 + Rface * | Vend - Vsur |^2;
void CGlSolverIkConstraint_Surface::Potential(double &valOut)
{
	valOut = 0.0;
	if( m_WeightConstraint == 0.0 ) return;

	int *ends = m_Target->GetChainEnds();
	int endsMax = m_Target->GetChainEndsMax();
	CGlArthro *arthro = m_Target->GetArthro();

	for( int c=0; c<endsMax; c++ ){
		CVector3H posSur, nrmSur;
		CVector3H *posEnd = arthro->GetBonePosEdge( ends[ c ] );
		CVector3H *vecEnd = arthro->GetPartZaxis( ends[ c ] );
		m_Clay->SearchNearestSurface_Point( posEnd, &posSur, &nrmSur );

		double lenPos = CVector3H::Distance( posEnd, &posSur );
		double lenVec = CVector3H::Distance( vecEnd, &nrmSur );
		valOut += m_RatePut * lenPos * lenPos + m_RateFace * lenVec * lenVec;
	}

	valOut *= m_WeightConstraint;
}
///// d F = Rput * 2 ( Pend - Psur ) * (1) + Rface * 2 ( Vend - Vsur ) * (2)
///// (1) = (Nsur * d Pend) Nsur
	// d Pend = axis_i *cross* ( Pend - P_i )
	// d Psur = d Pend - (Nsur * dPend) Nsur
///// (2) = axis_i *cross* Vend
void CGlSolverIkConstraint_Surface::DerPotential(CMatrix *vecOut)
{
	if( m_WeightConstraint == 0.0 ){
		vecOut->SetDataAllZero();
		return;
	}

	int *chain = m_Target->GetChain();
	int chainMax = m_Target->GetChainMax();
	CGlArthro *arthro = m_Target->GetArthro();
	CVector3H *posEnd = arthro->GetBonePosEdge( chain[ chainMax -1 ] );
	CVector3H *vecEnd = arthro->GetPartZaxis( chain[ chainMax -1 ] );
	CVector3H posSur, nrmSur;
	m_Clay->SearchNearestSurface_Point( posEnd, &posSur, &nrmSur );

	CVector3H subPendPsur; subPendPsur.Sub( posEnd, &posSur );
	CVector3H subVendVsur; subVendVsur.Sub( vecEnd, &nrmSur );

	for( int c=0; c<chainMax; c++ )
	{
		CVector3H derPosEnd_x, derPosEnd_y, derPosEnd_z;
		CVector3H vTmp, v1, v2;
		CVector3H *posNode = arthro->GetBonePosCenter( chain[ c ] );
		vTmp.Sub( posEnd, posNode );

		derPosEnd_x.CrossProduct( arthro->GetPartXaxis(c), &vTmp );
		derPosEnd_y.CrossProduct( arthro->GetPartYaxis(c), &vTmp );
		derPosEnd_z.CrossProduct( arthro->GetPartZaxis(c), &vTmp );

		double valPutX, valPutY, valPutZ;
		v1.MulConst( CVector3H::DotProduct(&nrmSur, &derPosEnd_x), &nrmSur );
		valPutX = m_RatePut * 2.0 * CVector3H::DotProduct( &subPendPsur, &v1 );
		v1.MulConst( CVector3H::DotProduct(&nrmSur, &derPosEnd_y), &nrmSur );
		valPutY = m_RatePut * 2.0 * CVector3H::DotProduct( &subPendPsur, &v1 );
		v1.MulConst( CVector3H::DotProduct(&nrmSur, &derPosEnd_z), &nrmSur );
		valPutZ = m_RatePut * 2.0 * CVector3H::DotProduct( &subPendPsur, &v1 );

		double valFaceX, valFaceY, valFaceZ;
		v2.CrossProduct( arthro->GetPartXaxis(c), vecEnd );
		valFaceX = m_RateFace * 2.0 * CVector3H::DotProduct( &subVendVsur, &v2 );
		v2.CrossProduct( arthro->GetPartYaxis(c), vecEnd );
		valFaceY = m_RateFace * 2.0 * CVector3H::DotProduct( &subVendVsur, &v2 );
		v2.CrossProduct( arthro->GetPartZaxis(c), vecEnd );
		valFaceZ = m_RateFace * 2.0 * CVector3H::DotProduct( &subVendVsur, &v2 );

		vecOut->SetData( c, 0, (valPutX + valFaceX) * m_WeightConstraint );
		vecOut->SetData( c, 1, (valPutY + valFaceY) * m_WeightConstraint );
		vecOut->SetData( c, 2, (valPutZ + valFaceZ) * m_WeightConstraint );
	}
}

///// ///// ///// マネージャー
void CGlSolverIkManager::AddConstraint( IGlSolverIkConstraint *constraint )
{
	if( m_Constraints == NULL ){
		m_Constraints = constraint; constraint->m_Next = NULL;
		return;
	}

	constraint->m_Next = m_Constraints; m_Constraints = constraint; 
}

bool CGlSolverIkManager::LoadChains( Ch3PlainData *data )
{
	if( m_Arthro == NULL ) return false;

	data->SetNameSpace( "IK" );
	if( data->Get( -1, "targetMax", &m_TargetMax ) == 0 || m_TargetMax < 1 ) return false;
	m_Targets = new CGlSolverIkTargetChain* [ m_TargetMax ];

	for( int t=0; t<m_TargetMax; t++ )
	{
		m_Targets[ t ] = new CGlSolverIkTargetChain( m_Arthro );

		int chain[ 32 ], ends[ 32 ];
		data->Get( t, "ikChain", chain );
		if( data->Get( t, "ikEnds", ends ) > 0 ){
			m_Targets[ t ]->SetChain( chain, ends );
		}else{
			m_Targets[ t ]->SetChain( chain, NULL );
		}
	}

	for( int t=0; t<m_TargetMax; t++ )
	{
		m_Targets[ t ]->LoadFlexibilityBones( data );
	}

	return true;
}

bool CGlSolverIkManager::Run()
{
	///// 各節の位置の再計算
	m_Arthro->CalcPosture();

	for( int t=0; t<m_TargetMax; t++ )
	{
		CGlSolverIkTargetChain *targetNow = m_Targets[ t ];
		ChangeTargetChain( targetNow );

		int chainMax = targetNow->GetChainMax();
		CMatrix angNow( chainMax, 3 );
		GetArrAngTarget( targetNow, &angNow );

		for( int f=0; f<m_FlowMax; f++ )
		{
			CMatrix angDer( chainMax, 3 );
			/////傾きを計算して
			CalcDerPot( targetNow, &angNow, &angDer );

			/////傾きの最大値を探す
			double valMax=0.0, *data=angDer.GetData();
			for( int i=0; i<chainMax*3; i++ ){
				if( valMax < *data ) valMax = *data;
				data++;
			}
			if( valMax == 0.0 ) break;

			/////角度を調べる
			double v=-m_AngRange/2.0;
			CMatrix angTest( chainMax, 3 ), bestAng( chainMax, 3 ), vecTmp( chainMax, 3 );
			double bestPot, valTest;
			vecTmp.MulConst( v/valMax, &angDer );
			bestAng.Add( &angNow, &vecTmp );
			CalcPot( targetNow, &bestAng, bestPot );

			for( v+=m_AngStep; v<m_AngRange/2.0; v+=m_AngStep )
			{
				vecTmp.MulConst( v/valMax, &angDer );
				angTest.Add( &angNow, &vecTmp );
				CalcPot( targetNow, &angTest, valTest );
				if( bestPot > valTest ){
					bestPot = valTest;
					bestAng = angTest;
				}
			}

			angNow = bestAng;
		}
	}

	return true;
}

///// プライベートな関数
void CGlSolverIkManager::ChangeTargetChain( CGlSolverIkTargetChain *target )
{
	IGlSolverIkConstraint *constraint = m_Constraints;
	while( constraint != NULL )
	{
		constraint->SetTarget( target );
		constraint = constraint->m_Next;
	}
}

void CGlSolverIkManager::SetArrAngTarget( CGlSolverIkTargetChain *target, CMatrix *matAng )
{
	CGlArthro *arthro = target->GetArthro();
	int *chain = target->GetChain();
	int chainMax = target->GetChainMax();

	for( int c=0; c<chainMax; c++ ){
		arthro->SetBoneAngle( chain[ c ], matAng->GetData( c ) );
	}
	arthro->CalcPosture();
}

void CGlSolverIkManager::GetArrAngTarget( CGlSolverIkTargetChain *target, CMatrix *matAng )
{
	CGlArthro *arthro = target->GetArthro();
	int *chain = target->GetChain();
	int chainMax = target->GetChainMax();

	for( int c=0; c<chainMax; c++ ){
		arthro->GetBoneAngle( chain[ c ], matAng->GetData( c ) );
	}
}

void CGlSolverIkManager::CalcPot( CGlSolverIkTargetChain *target, CMatrix *matAng, double &valOut )
{
	CGlArthro *arthro = target->GetArthro();
	int *chain = target->GetChain();
	int chainMax = target->GetChainMax();

	///// 角度を代入
	SetArrAngTarget( target, matAng );

	///// ポテンシャルを計算
	double amtPot=0, tmpPot;
	IGlSolverIkConstraint *constraint = m_Constraints;
	while( constraint != NULL )
	{
		constraint->Potential( tmpPot );
		amtPot += tmpPot;
		constraint = constraint->m_Next;
	}
	valOut = amtPot;
}

void CGlSolverIkManager::CalcDerPot( CGlSolverIkTargetChain *target, CMatrix *matAng, CMatrix *derOut )
{
	CGlArthro *arthro = target->GetArthro();
	int *chain = target->GetChain();
	int chainMax = target->GetChainMax();

	///// 角度を代入
	SetArrAngTarget( target, matAng );

	///// 微分を計算
	derOut->SetDataAllZero();
	CMatrix tmpDer( chainMax, 3 );
	IGlSolverIkConstraint *constraint = m_Constraints;
	while( constraint != NULL )
	{
		constraint->DerPotential( &tmpDer );
		derOut->Add( derOut, &tmpDer );
		constraint = constraint->m_Next;
	}
}
