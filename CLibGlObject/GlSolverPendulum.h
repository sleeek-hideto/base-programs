#pragma once

/////Using "Inverse Kinematics Positioning
////	Using Nonlinear Programming for Highly Articulated Figures", by Jianmin Zhao, Norman I, Badler. ACM SIG 1994.
/////Using "Large Steps in Cloth Simulation", by David Baraff, Andrew Witkin. ACM SIG 1998.

class CGlSolverPendulumNode : public IGlSolverNode
{
private:
	typedef struct _NodeNeighbor {
		_NodeNeighbor *next;
		CGlBone *bone;
	} NodeNeighbor;

	inline double Radian2Degree( double ra ){ return ra*180.0/3.14159265; }

	void ByLargeStepsInClothSimulation( double amtt, double dert );

public:
	CVector3H m_TmpAngleVelocity;//!<Šp‘¬“x
	CVector3H m_TmpSpatialVelocity, m_TmpSpatialAccelerate;
	CVector3H m_PreSpatialPos;
	//CVector3H m_AngleVelocity;
	CVector3H m_SpatialVelocity, m_SpatialAccelerate;
	double m_ConstantDamp, m_ConstantSpring, m_ConstantWeight, m_ConstantSynchronous;
	double m_ConstantSpringEasy;
	CVector3H m_ConstantAngleBaseSpring;

	enum _ModeMove { Driven, Floaty } m_ModeMove;

	CGlBone *m_BoneParent;
	NodeNeighbor *m_BoneNeighbor;

	CGlSolverPendulumNode(void){
		sprintf( m_Type, "Pendulum" );
		m_ConstantDamp = 1.0; m_ConstantSpring = 0.0; m_ConstantWeight = 1.0; m_ConstantSynchronous = 1.0;
		m_ConstantSpringEasy = 0.0;
		m_BoneNeighbor = NULL;

		m_SpatialVelocity.SetDataAllZero();
		m_SpatialAccelerate.SetDataAllZero();
	}
	~CGlSolverPendulumNode(void){
	}

	bool IsNodeDriven(){ if(m_ModeMove==Driven)return true; return false; }

	void Destroy(){
		if( m_BoneNeighbor != NULL ){
			NodeNeighbor *n = m_BoneNeighbor;
			while( n!=NULL ){ NodeNeighbor *d = n; n=n->next; delete d; }
		}
	}

	void AddNeighbor( CGlBone *b ){
		NodeNeighbor *n = new NodeNeighbor; n->bone=b; n->next=m_BoneNeighbor; m_BoneNeighbor=n;
	}

	void InitVelocityAndAccelerate();
	void SetVelocityAndAccelerate( double amt, double dert );

	void CalcAngleNextStep( double amtt, double dert );
	void ApplyNextStep( double amtt, double dert );
};

class CGlSolverPendulumManager : public IGlSolverManager
{
private:
	long m_StepRun;
	double m_TimeAmount, m_TimeInterval;
	CGlArthro *m_Arthro;

	Ch3PlainData m_DataDebug;

	CGlSolverPendulumNode *GetNodeSolver( unsigned int idx ){
		if( m_Arthro == NULL || idx >= (unsigned int)m_Arthro->GetBoneMax() )return NULL;
		CGlSolverPendulumNode *n = (CGlSolverPendulumNode*)m_Arthro->GetBone(idx)->m_NodeSolver;
		if( n==NULL || strcmp(n->m_Type,"Pendulum")!=0 )return NULL;
		return n;
	}

public:
	bool m_Debug;

	CGlSolverPendulumManager(void){
		m_Debug = false;
		m_StepRun = 0;
		m_TimeInterval = 0.1;
		m_TimeAmount = 0.0;
		m_Arthro = NULL;
	}
	~CGlSolverPendulumManager(void){
	}

	void InitSetArthro( CGlArthro *arthro ){ m_Arthro=arthro; }
	bool MakeNodeDriven( unsigned int idx );
	bool MakeNodeFloaty( unsigned int idx, int *neighbor );

	void SetInterval( double t ){ m_TimeInterval=t; }
	bool SetNodeDamp( unsigned int idx, double v );
	bool SetNodeWeight( unsigned int idx, double v );
	bool SetNodeSpring( unsigned int idx, double v, double angX=0.0, double angY=0.0, double angZ=0.0 );
	bool SetNodeSpringEasy( unsigned int idx, double v, double angX=0.0, double angY=0.0, double angZ=0.0 );
	bool SetNodeSynchronous( unsigned int idx, double v );

	bool LoadConfig( Ch3PlainData *data );
	bool LoadConfig( char *loc ){
		Ch3PlainData data; if( data.Load(loc) ){ return LoadConfig(&data); } return false;
	}

	bool Run();

	void DebugStart(){ m_Debug = true; }
	void DebugFinish(){ m_Debug = false; m_DataDebug.Save( "SolverPendulum.dat" ); }
};
