#pragma once

/*
*	@brief �X�P���g���E���f���̃{�[�������̃N���X
*/
class CGlBone
{
public:
	CGlBone *m_BoneParent, *m_BoneSibling, *m_BoneChild;	//!<���̃{�[���̐e�A�Z��A�q��
	CGlSkin *m_Skin;	//!<�ێ�����X�L���i�̐擪�փ|�C���^�j
	CVector3H m_PosLocal;	//!<���[�J�����W
	CVector3H m_AngInit, m_AngLocal, m_AngHigher, m_AngLower;	//!<�{�[���̏�����]�p�A�{�[���̉�]�p�A��]������E�A��]�������E
	CVector3H m_PosGlobalCenter, m_PosGlobalEdge;	//!<�{�[���̌��_�̃O���[�o�����W�A�{�[���̖��[�̃O���[�o�����W
	CVector3H m_AxisX, m_AxisY, m_AxisZ;	//!<�O���[�o�����W�ɂ�����A�w�A�x�A�y���̌�������
	CMatrix4x4 m_ProLocal, m_ProGlobal;	//!<���[�J�����W����O���[�o�����W�ւƕϊ�����s��

	IGlSolverNode *m_NodeSolver;
	void SetSolver(IGlSolverNode *s){ if(m_NodeSolver!=NULL)m_NodeSolver=s; }
public:
	CGlBone(){
		m_BoneParent = NULL;
		m_BoneSibling = NULL;
		m_BoneChild = NULL;
		m_Skin = NULL;

		m_PosLocal.SetData( 0.0, 0.0, 0.0 );
		m_AngInit.SetData( 0.0, 0.0, 0.0 );
		m_AngLocal.SetData( 0.0, 0.0, 0.0 );
		m_AngHigher.SetData( 180.0, 180.0, 180.0 );
		m_AngLower.SetData( -180.0, -180.0, -180.0 );

		m_NodeSolver = NULL;
	}
	~CGlBone(){
		if(m_NodeSolver!=NULL){ m_NodeSolver->Destroy(); delete m_NodeSolver; m_NodeSolver=NULL; }
	}

	virtual void Draw_recursive( char *type=NULL );
	virtual void Draw_oneBone( char *type=NULL );
	virtual void CalcPosture();
	virtual void CalcPosture_recursive();

	inline void SetPos( CVector3H *v ){	m_PosLocal.Copy( v ); }
	inline void SetPos( double *p ){ m_PosLocal.SetData( p ); }
	inline void AddPos( CVector3H *v ){	m_PosLocal.Add( v ); }
	inline void AddPos( double *p ){ m_PosLocal.AddData( p ); }
	inline CVector3H *GetPosLocal(){ return &m_PosLocal; }
	inline CVector3H *GetPosGlobalCenter(){ return &m_PosGlobalCenter; }
	inline CVector3H *GetPosGlobalEdge(){ return &m_PosGlobalEdge; }

	void SetAngle( CVector3H *v );
	void SetAngle( double *d );
	void AddAngle( CVector3H *v );
	void AddAngle( double *d );
	inline CVector3H *GetAngle(){	return &m_AngLocal;	}
	inline void GetAngle( double *d ){	m_AngLocal.GetData( d );	}
	inline void SetAngleInit( CVector3H *v ){	m_AngInit.Copy( v );	}
	inline void SetAngleInit( double *d ){	m_AngInit.SetData( d );	}
	inline void AddAngleInit( CVector3H *v ){	m_AngInit.Add( v );	}
	inline void AddAngleInit( double *d ){	m_AngInit.AddData( d );	}
	inline CVector3H *GetAngleInit(){	return &m_AngInit;	}

	/*!
		@brief �{�[������蓾��p�x�̏���Ɖ�����ݒ肷��
		@param[in] low �����̊p�x
		@param[in] higt ����̊p�x
	*/
	inline void SetAngleLimit( CVector3H *low, CVector3H *high ){
		m_AngLower.Copy( low );	m_AngHigher.Copy( high );
	}
	inline void SetAngleLimit( double *low, double *high ){
		m_AngLower.SetData( low );	m_AngHigher.SetData( high );
	}
	inline void GetAngleLimit( CVector3H *low, CVector3H *high ){
		low->Copy( &m_AngLower ); high->Copy( &m_AngHigher );
	}
	inline void GetAngleLimit( double *low, double *high ){
		m_AngLower.GetData( low );	m_AngHigher.GetData( high );
	}

	/*!
		@brief ����X�����A���A���E���W�n�łǂ̕����Ɍ����Ă��邩��Ԃ�
	*/
	inline CVector3H *GetAxisX(){	return &m_AxisX;	}
	inline CVector3H *GetAxisY(){	return &m_AxisY;	}
	inline CVector3H *GetAxisZ(){	return &m_AxisZ;	}

	inline void AddBoneSibling( CGlBone *bone ){
		bone->m_BoneParent = m_BoneParent;
		bone->m_BoneSibling = m_BoneSibling;
		m_BoneSibling = bone;
	}
	inline void AddBoneChild( CGlBone *bone ){
		if( m_BoneChild == NULL ){
			bone->m_BoneParent = this;
			m_BoneChild = bone;
		}else{
			m_BoneChild->AddBoneSibling( bone );
		}
	}
	inline void ClearSkin(){ m_Skin = NULL; }
	inline void AddSkin( CGlSkin *skin ){
		skin->m_BoneParent = this;
		skin->m_SkinSibling = m_Skin;
		m_Skin = skin;
	}

	inline CGlBone* GetBoneChild(){
		return m_BoneChild;
	}
	inline CGlBone* GetBoneSibling(){
		return m_BoneSibling;
	}
	inline CGlSkin* GetSkin(){
		return m_Skin;
	}
};

/*
*	@brief �X�P���g���E���f���̃{�[�������̎������̃N���X�B�X�̒��_�ʒu�܂Ōv�Z����A�����Ԃ�������
*/
/*
class CGlBoneDeform : public CGlBone
{
public:
	CGlBoneDeform(){
	}

	inline void AddBoneSibling( CGlBoneDeform *bone ){
		bone->m_BoneParent = m_BoneParent;
		bone->m_BoneSibling = m_BoneSibling;
		m_BoneSibling = bone;
	}
	inline void AddBoneChild( CGlBoneDeform *bone ){
		if( m_BoneChild == NULL ){
			bone->m_BoneParent = this;
			m_BoneChild = bone;
		}else{
			m_BoneChild->AddBoneSibling( bone );
		}
	}

	inline CGlBoneDeform* GetBoneChild(){
		return (CGlBoneDeform*)m_BoneChild;
	}
	inline CGlBoneDeform* GetBoneSibling(){
		return (CGlBoneDeform*)m_BoneSibling;
	}
	inline CGlSkinDeform* GetSkin(){
		return (CGlSkinDeform*)m_Skin;
	}

	void CalcPosture();
	void Draw_recursive( char *type=NULL );

	//
	//	@brief ���[�J�����W���i�[�����X�L���ƁA���[���h���W���i�[�����X�L������������
	//	@param[in] skinLocal ���[�J�����W���i�[�����X�L��
	//	@param[in] skinWorld ���[���h���W���i�[�����X�L��
	//
	inline void AddSkin( CGlSkinDeform *skinLocal, CGlSkinDeform *skinDeformed=NULL, CGlSkinDeform *skinWorld=NULL ){
		CGlBone::AddSkin( skinLocal );
		if( skinDeformed != NULL )skinLocal->SetSkinDeformed( skinDeformed );
		if( skinWorld != NULL )skinLocal->SetSkinWorld( skinWorld );
	}
};
*/