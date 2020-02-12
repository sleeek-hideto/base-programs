#include "StdAfx.h"
#include "GlSolverPendulum.h"

void CGlSolverPendulumNode::ByLargeStepsInClothSimulation( double amtt, double dert )
{
// (M - h \frac{d f}{d v} - h^2 \frac{d f}{d x} \delta v = h (f_0 + h v_0 \frac{d f}{d x})
// f = f_in + f_ex;
// f_in(1) = -c_damp v^2, f_ex = m_my g + \Sigma (acc_your - acc_my) m_your
// f_in(*) = -c_damp v^2 -{ c_spr x_spr}, x_spr = axis *cross* dirBn * ang
// d f / d x(1) = 0
// d f / d x(*) = -c_spr
// d f / d v(1,2) = -2 c_damp v
	CVector3H vecDfDx, vecDfDv, f, fin, fex;
	CVector3H tmpV0, tmpV1;
	CVector3H angBn;
	angBn.Copy( m_BoneParent->GetAngle() );

	/////�o�l�i�����j���������Y�C
	CVector3H fSpring;

	if( m_ConstantSpring > 0.0 )
	{
		CVector3H axis_x, axis_z, vx, vz, dirBn;

		if( m_BoneParent->m_BoneParent == NULL ){///// �g�b�v���ۂ�
			axis_x.SetData(1,0,0);
			axis_z.SetData(0,0,1);
			dirBn.Copy( m_BoneParent->GetPosLocal() );
		}else{
			axis_x.Copy( m_BoneParent->m_BoneParent->GetAxisX() );
			axis_z.Copy( m_BoneParent->m_BoneParent->GetAxisZ() );
			dirBn.Sub( m_BoneParent->GetPosGlobalEdge(), m_BoneParent->GetPosGlobalCenter() );
		}
		vx.CrossProduct( &axis_x, &dirBn );
		vz.CrossProduct( &axis_z, &dirBn );

		tmpV0.Sub( &angBn, &m_ConstantAngleBaseSpring );
		tmpV0.MulConst( 3.1415/180.0 );
		vx.MulConst( -m_ConstantSpring*tmpV0.GetData()[0] );
		vz.MulConst( -m_ConstantSpring*tmpV0.GetData()[2] );
		fSpring.Add( &vx, &vz );
	}

	//tmpV0.EachProduct( &m_SpatialVelocity, &m_SpatialVelocity );/////��Rv^2�̂Ƃ�
	//fin.MulConst( -m_ConstantDamp, &tmpV0 );/////��Rv^2�̂Ƃ�
	fin.MulConst( -m_ConstantDamp, &m_SpatialVelocity );/////��Rv�̂Ƃ�
	fin.Add( &fSpring );
	fex.SetData( 0, -9.8 * m_ConstantWeight, 0 );
	vecDfDx.SetDataAllZero();
	//vecDfDx.SetData( -m_ConstantSpring, -m_ConstantSpring, -m_ConstantSpring );
	//vecDfDv.MulConst( -2.0 * m_ConstantDamp, &m_SpatialVelocity );/////��Rv^2�̂Ƃ�
	vecDfDv.SetData( -m_ConstantDamp, -m_ConstantDamp, -m_ConstantDamp );/////��Rv�̂Ƃ�

	NodeNeighbor *n = m_BoneNeighbor;
	while( n != NULL )
	{
		CGlSolverPendulumNode *neighbor = (CGlSolverPendulumNode*)n->bone->m_NodeSolver;

		//CVector3H subAcc;
		//subAcc.Sub( &neighbor->m_SpatialAccelerate, &m_SpatialAccelerate );
		//tmpV0.MulConst( neighbor->m_ConstantWeight*m_ConstantSynchronous, &subAcc );

		CVector3H subVel;
		subVel.Sub( &neighbor->m_SpatialVelocity, &m_SpatialVelocity );
		tmpV0.MulConst(
			m_ConstantSynchronous * m_ConstantWeight/(neighbor->m_ConstantWeight + m_ConstantWeight),
			&subVel );

		fex.Add( &tmpV0 );

		n = n->next;
	}

	f.Add( &fin, &fex );

	CVector3H deltaVel;
	for( int i=0; i<3; i++ ){
		//if( m_BoneParent->GetPosLocal()->GetData()[i] == 0.0 )continue;
		deltaVel.SetData( i,
			dert * ( f.GetData()[i] + dert * m_SpatialVelocity.GetData()[i] * vecDfDx.GetData()[i] ) /
			( m_ConstantWeight - dert * vecDfDv.GetData()[i] - dert*dert * vecDfDx.GetData()[i] )
		);
	}

	///// �K���s�l�o�̉����x�Ƒ��x�̐ݒ���s��
	m_TmpSpatialVelocity.Add( &deltaVel, &m_SpatialVelocity );
//	m_TmpSpatialVelocity.MulConst( 1.0-m_ConstantDamp );/////����ł悢���H
	m_TmpSpatialAccelerate.MulConst( 1.0/dert, &deltaVel );
}

void CGlSolverPendulumNode::InitVelocityAndAccelerate()
{
	if( m_BoneParent == NULL )return;
	m_SpatialVelocity.SetDataAllZero();
	m_SpatialAccelerate.SetDataAllZero();

	m_PreSpatialPos.Copy( m_BoneParent->GetPosGlobalEdge() );
}

/*!
	@brief ���x�A�����x�����̓�������v�Z����
*/
void CGlSolverPendulumNode::SetVelocityAndAccelerate( double amt, double dert )
{
	if( m_BoneParent == NULL )return;

	CVector3H velNew;
	velNew.Sub( m_BoneParent->GetPosGlobalEdge(), &m_PreSpatialPos );
	velNew.MulConst( 1.0/dert );
	m_SpatialAccelerate.Sub( &velNew, &m_SpatialVelocity );
	m_SpatialAccelerate.MulConst( 1.0/dert );
	m_SpatialVelocity.Copy( &velNew );

	m_PreSpatialPos.Copy( m_BoneParent->GetPosGlobalEdge() );
}

/*!
	@brief �P�X�e�b�v��̃m�[�h�̓������A���݂̑��x�A�����x����v�Z����
	@param[in] ammt ����܂ł̌o�ߎ���
	@param[in] dert �P�X�e�b�v�̎���
*/
void CGlSolverPendulumNode::CalcAngleNextStep( double amtt, double dert )
{
	ByLargeStepsInClothSimulation(amtt,dert);

	///// ���x�E�����x�̕ω�����p�x�̕ω������߂�
	///// (\delta deltaPos)(=c)
	/////	= (axis_x \cross posBn)(=a) \delta theta_x + (axis_z \cross posBn)(=b) \delta theta_z ;
	///// L = (c - s a - t b )^2, L���ŏ��ɂ���s,t?
	///// d L / d s = (c - s a - t b) a = 0, a \dot b = 0
	///// a \dot b = 0 =>
	///// s = a \dot c / |a|^2 ; t = b \dot c / |b|^2 ;
	CVector3H axis_x, axis_z, va, vb, deltaPos, posBn;
	double theta_x, theta_z, tmpl;
	CVector3H angBone;
	angBone.Copy( m_BoneParent->GetAngle() );

	if( m_BoneParent->m_BoneParent == NULL ){///// �g�b�v���ۂ�
		axis_x.SetData(1,0,0);
		axis_z.SetData(0,0,1);
		posBn.Copy( m_BoneParent->GetPosLocal() );

		deltaPos.MulConst( dert, &m_TmpSpatialVelocity );
	}else{
		axis_x.Copy( m_BoneParent->m_BoneParent->GetAxisX() );
		axis_z.Copy( m_BoneParent->m_BoneParent->GetAxisZ() );
		posBn.Sub( m_BoneParent->GetPosGlobalEdge(), m_BoneParent->GetPosGlobalCenter() );

		CGlSolverPendulumNode *n = (CGlSolverPendulumNode*)m_BoneParent->m_BoneParent->m_NodeSolver;
		if( n!=NULL ){
			deltaPos.Sub( &m_TmpSpatialVelocity, &n->m_SpatialVelocity );
			deltaPos.MulConst( dert );
		}
	}
	va.CrossProduct( &axis_x, &posBn );
	vb.CrossProduct( &axis_z, &posBn );

	tmpl = va.Length();
	double damp_x = sqrt( abs( cos( angBone.GetData()[0]*3.1415/180.0 ) ) );
	double damp_z = sqrt( abs( cos( angBone.GetData()[2]*3.1415/180.0 ) ) );
	if( tmpl == 0.0 ){
		theta_x = 0.0;
	}else{
		theta_x = CVector3H::DotProduct( &va, &deltaPos ) / (tmpl*tmpl);
		theta_x *= 180.0 / 3.14159265;
		if( theta_x < -15.0 ) theta_x = -15.0;
		if( theta_x > 15.0 ) theta_x = 15.0;
		theta_x *= damp_x;
	}
	tmpl = vb.Length();
	if( tmpl == 0.0 ){
		theta_z = 0.0;
	}else{
		theta_z = CVector3H::DotProduct( &vb, &deltaPos ) / (tmpl*tmpl);
		theta_z *= 180.0 / 3.14159265;
		if( theta_z < -15.0 ) theta_z = -15.0;
		if( theta_z > 15.0 ) theta_z = 15.0;
		theta_z *= damp_z;
	}

	m_TmpAngleVelocity.SetData(
		(1.0-m_ConstantDamp)*theta_x/dert,
		0.0,
		(1.0-m_ConstantDamp)*theta_z/dert );

	//m_TmpSpatialAccelerate.TestPrint( "Acc" );
	//m_TmpSpatialVelocity.TestPrint( "Vel" );
	//m_TmpAngleVelocity.TestPrint( "AngVel" );
}

void CGlSolverPendulumNode::ApplyNextStep( double amtt, double dert )
{
	CVector3H tmpV0;
	tmpV0.MulConst( dert, &m_TmpAngleVelocity );
	m_BoneParent->AddAngle( &tmpV0 );/////�p�x�𑫂���

	/////�o�l�i�ȈՂQ�j
	tmpV0.Sub( &m_ConstantAngleBaseSpring, m_BoneParent->GetAngle() );
	tmpV0.MulConst( m_ConstantSpringEasy );
	m_BoneParent->AddAngle( &tmpV0 );/////�p�x�̒���

	//m_SpatialAccelerate.Copy( &m_TmpSpatialAccelerate );/////�X�V
	//m_SpatialVelocity.Copy( &m_TmpSpatialVelocity );
}

///// ///// Manager ///// /////
/*!
	@brief �쓮�^�̃\���o�[�E�m�[�h���P���
	@param[in] idx �{�[���ԍ�
*/
bool CGlSolverPendulumManager::MakeNodeDriven( unsigned int idx )
{
	if( m_Arthro == NULL || m_Arthro->GetBoneMax() <= (int)idx )return false;
	CGlBone *tgtBone = m_Arthro->GetBone(idx);

	if( tgtBone->m_NodeSolver != NULL )return false;
	CGlSolverPendulumNode *tgtNodeSolver = new CGlSolverPendulumNode();
	tgtNodeSolver->m_BoneParent = tgtBone;
	tgtBone->m_NodeSolver = tgtNodeSolver;
	if( strcmp( tgtNodeSolver->m_Type, "Pendulum" ) != 0 )return false;

	tgtNodeSolver->m_ModeMove = CGlSolverPendulumNode::Driven;

	return true;
}

/*!
	@brief �\���o�[�E�m�[�h���P���
	@param[in] idx �{�[���ԍ�
	@param[in] neighbor �ߐڂ���{�[�����w�肷��ANULL���Ƃ��ׂĂ�parent,children�ɂȂ�
*/
bool CGlSolverPendulumManager::MakeNodeFloaty( unsigned int idx, int *neighbor )
{
	if( m_Arthro == NULL || (unsigned int)m_Arthro->GetBoneMax() <= idx )return false;
	CGlBone *tgtBone = m_Arthro->GetBone(idx);

	if( tgtBone->m_NodeSolver != NULL )return false;
	CGlSolverPendulumNode *tgtNodeSolver = new CGlSolverPendulumNode();
	tgtNodeSolver->m_BoneParent = tgtBone;
	tgtBone->m_NodeSolver = tgtNodeSolver;
	if( strcmp( tgtNodeSolver->m_Type, "Pendulum" ) != 0 )return false;

	tgtNodeSolver->m_ModeMove = CGlSolverPendulumNode::Floaty;

	///// �e������ꍇ�͂��ׂɒǉ�
	if( tgtBone->m_BoneParent != NULL ){
		tgtNodeSolver->AddNeighbor( tgtBone->m_BoneParent );
	}

	if( neighbor != NULL )
	{
		for( unsigned int i=0; i<8; i++ ){
			if( neighbor[i] < 0 )break;
			tgtNodeSolver->AddNeighbor( m_Arthro->GetBone( i ) );
		}
	}else{
		CGlBone *rlyBone = tgtBone->GetBoneChild();
		while( rlyBone != NULL ){
			tgtNodeSolver->AddNeighbor( rlyBone );
			rlyBone = rlyBone->GetBoneSibling();
		}
	}
	return true;
}

/*!
	@brief �����萔�̎w��
*/
bool CGlSolverPendulumManager::SetNodeDamp( unsigned int idx, double v )
{
	CGlSolverPendulumNode *tgtNodeSolver = GetNodeSolver( idx );
	if( tgtNodeSolver == NULL )return false;

	tgtNodeSolver->m_ConstantDamp = v;
	return true;
}
/*!
	@brief �����萔�̎w��
*/
bool CGlSolverPendulumManager::SetNodeWeight( unsigned int idx, double v )
{
	CGlSolverPendulumNode *tgtNodeSolver = GetNodeSolver( idx );
	if( tgtNodeSolver == NULL )return false;

	tgtNodeSolver->m_ConstantWeight = v;
	return true;
}

/*!
	@brief �o�l�̎w��
*/
bool CGlSolverPendulumManager::SetNodeSpring( unsigned int idx, double v, double angX, double angY, double angZ )
{
	CGlSolverPendulumNode *tgtNodeSolver = GetNodeSolver( idx );
	if( tgtNodeSolver == NULL )return false;

	tgtNodeSolver->m_ConstantSpring = v;
	tgtNodeSolver->m_ConstantAngleBaseSpring.SetData( angX,angY,angZ );
	return true;
}

/*!
	@brief �ȈՃo�l�̎w��
*/
bool CGlSolverPendulumManager::SetNodeSpringEasy( unsigned int idx, double v, double angX, double angY, double angZ )
{
	CGlSolverPendulumNode *tgtNodeSolver = GetNodeSolver( idx );
	if( tgtNodeSolver == NULL )return false;

	tgtNodeSolver->m_ConstantSpringEasy = v;
	tgtNodeSolver->m_ConstantAngleBaseSpring.SetData( angX,angY,angZ );
	return true;
}

/*!
	@brief �����萔�̎w��
*/
bool CGlSolverPendulumManager::SetNodeSynchronous( unsigned int idx, double v )
{
	CGlSolverPendulumNode *tgtNodeSolver = GetNodeSolver( idx );
	if( tgtNodeSolver == NULL )return false;

	tgtNodeSolver->m_ConstantSynchronous = v;
	return true;
}

/*!
	@brief �m�[�h�̐ݒ���X�g���[�W����ǂݍ���
*/
bool CGlSolverPendulumManager::LoadConfig( Ch3PlainData *data )
{
	data->SetNameSpace( "Bone" );
	int BoneMax = 0;
	if( data->Get( -1, "BoneMax", &BoneMax ) < 1 || BoneMax < 1 )return false;

	data->SetNameSpace( "Dynamics" );
	double tmpD4[ 4 ] = { 0,0,0,0 };
	int tmpI16[ 16 ];
	int cnt;
	for( int b=0; b<BoneMax; b++ )
	{
		if( data->Get( b, "dynMakeDriven", tmpI16 ) > 0 ){
			MakeNodeDriven( (unsigned int)b );
		}
		cnt = data->Get( b, "dynMakeFloaty", tmpI16 );
		if( cnt > 0 ){
			if( tmpI16[ 0 ] < 0 ){
				MakeNodeFloaty( (unsigned int)b, NULL );
			}else{
				if( cnt >= 16 ) cnt = 15;
				tmpI16[ cnt ] = -1;
				MakeNodeFloaty( (unsigned int)b, tmpI16 );
			}
		}

		if( data->Get( b, "dynSetDamp", tmpD4 ) > 0 ){
			SetNodeDamp( (unsigned int)b, tmpD4[ 0 ] );
		}
		if( data->Get( b, "dynSetWeight", tmpD4 ) > 0 ){
			SetNodeWeight( (unsigned int)b, tmpD4[ 0 ] );
		}
		if( data->Get( b, "dynSetSync", tmpD4 ) > 0 ){
			SetNodeSynchronous( (unsigned int)b, tmpD4[ 0 ] );
		}
		/////�ǂ��炩�������
		if( data->Get( b, "dynSetSpringEasy", tmpD4 ) > 0 ){
			SetNodeSpringEasy( (unsigned int)b, tmpD4[0], tmpD4[1], tmpD4[2], tmpD4[3] );
		}else if( data->Get( b, "dynSetSpring", tmpD4 ) > 0 ){
			SetNodeSpring( (unsigned int)b, tmpD4[0], tmpD4[1], tmpD4[2], tmpD4[3] );
		}
	}

	return true;
}

/*
	@brief �_�C�i�~�N�X���P�t���[�����i�߂�
*/
bool CGlSolverPendulumManager::Run()
{
	if( m_Arthro == NULL )return false;
	m_Arthro->CalcPosture();

	///// ���x�Ɖ����x�̍X�V
	int BoneMax = m_Arthro->GetBoneMax();
	for( int b=0; b<BoneMax; b++ )
	{
		CGlSolverPendulumNode *tgtNodeSolver = GetNodeSolver( b );
		if( tgtNodeSolver != NULL ){
			if( m_StepRun == 0 ){
				tgtNodeSolver->InitVelocityAndAccelerate();
			}else{
				tgtNodeSolver->SetVelocityAndAccelerate( m_TimeAmount, m_TimeInterval );
			}
		}
	}

	///// Floaty �̃m�[�h�̌v�Z
	for( int b=0; b<BoneMax; b++ )
	{
		CGlSolverPendulumNode *tgtNodeSolver = GetNodeSolver( b );
		if( tgtNodeSolver != NULL && tgtNodeSolver->IsNodeDriven()==false ){
			tgtNodeSolver->CalcAngleNextStep( m_TimeAmount, m_TimeInterval );
		}
	}

	///// �m�[�h�̍X�V
	for( int b=0; b<BoneMax; b++ )
	{
		CGlSolverPendulumNode *tgtNodeSolver = GetNodeSolver( b );
		if( tgtNodeSolver != NULL && tgtNodeSolver->IsNodeDriven()==false ){
			tgtNodeSolver->ApplyNextStep( m_TimeAmount, m_TimeInterval );

			if( m_Debug )
			{
				char namePart[ 64 ];
				sprintf( namePart, "Bn%02dAng", b );
				m_DataDebug.Set( (int)m_StepRun, namePart, 3, tgtNodeSolver->m_TmpAngleVelocity.GetData() );
				sprintf( namePart, "Bn%02dVel", b );
				m_DataDebug.Set( (int)m_StepRun, namePart, 3, tgtNodeSolver->m_SpatialVelocity.GetData() );
				sprintf( namePart, "Bn%02dAcc", b );
				m_DataDebug.Set( (int)m_StepRun, namePart, 3, tgtNodeSolver->m_SpatialAccelerate.GetData() );
			}
		}
	}

	m_StepRun++;
	m_TimeAmount += m_TimeInterval;
	return true;
}
