#pragma once

class CGlSolverIkTargetChain
{
protected:
	const static int CHAIN_MAX = 32;
	CGlArthro *m_Arthro;
	int m_Chain[ CHAIN_MAX ], m_ChainMax, m_ChainEnds[ CHAIN_MAX ], m_ChainEndsMax;
	double *m_FlexibilityBones;

public:
	CGlSolverIkTargetChain( CGlArthro *a ){
		m_Arthro = a;
		m_Chain[ 0 ] = -1; m_ChainMax = 0;
		m_ChainEnds[ 0 ] = -1; m_ChainEndsMax = 0;
		if( a==NULL || a->GetBoneMax() < 1 ) return;
		
		int boneMax = a->GetBoneMax();
		m_FlexibilityBones = new double[ boneMax * 3 ];
		for(int b=0; b<boneMax; b++){ m_FlexibilityBones[3*b ]=1.0; m_FlexibilityBones[3*b+1]=1.0; m_FlexibilityBones[3*b+2]=1.0; }
	}
	~CGlSolverIkTargetChain(){
		delete [] m_FlexibilityBones; m_FlexibilityBones = NULL;
	}

	//void SetArthro( CGlArthro *a ){ m_Arthro=a; }
	void SetChain( int *chain, int *ends=NULL ){
		if( chain==NULL )return;
		for(int c=0;;c++){ m_Chain[c]=chain[c]; if(chain[c]<0){ m_ChainMax=c; break; } }
		if( ends==NULL ){
			for(int c=0;;c++){ m_ChainEnds[c]=ends[c]; if(ends[c]<0){ m_ChainEndsMax=c; break; } }
		}else{
			m_ChainEnds[0]=chain[m_ChainMax-1]; m_ChainEnds[1]=-1;
			m_ChainEndsMax=1;
		}
	}
	void SetFlexibilityBone( int b, double *vec ){
		if( b >= m_Arthro->GetBoneMax() ) return;
		memcpy( m_FlexibilityBones +3*b, vec, 3*sizeof(double) );
	}
	void SetFlexibilityBone( int b, double x, double y, double z ){
		m_FlexibilityBones[b*3 ]=x; m_FlexibilityBones[b*3+1]=y; m_FlexibilityBones[b*3+2]=z;
	}
	void LoadFlexibilityBones( Ch3PlainData *data );

	CGlArthro *GetArthro(){ return m_Arthro; }
	int *GetChain(){ return m_Chain; }
	int GetChainMax(){ return m_ChainMax; }
	int *GetChainEnds(){ return m_ChainEnds; }
	int GetChainEndsMax(){ return m_ChainEndsMax; }
	double *GetFlexibilityBone( int b ){
		if( b >= m_Arthro->GetBoneMax() ) return NULL;
		return m_FlexibilityBones + b*3;
	}
	double GetFlexibilityXBone( int b ){ return m_FlexibilityBones[ b*3 ]; }
	double GetFlexibilityYBone( int b ){ return m_FlexibilityBones[ b*3 +1 ]; }
	double GetFlexibilityZBone( int b ){ return m_FlexibilityBones[ b*3 +2 ]; }
};

class IGlSolverIkConstraint
{
protected:
	char m_NamePrivate[ 64 ];
	CGlSolverIkTargetChain *m_Target;
	double m_WeightConstraint;

public:
	IGlSolverIkConstraint *m_Next;

public:
	IGlSolverIkConstraint(){
		m_Target = NULL;
		m_Next = NULL;
	}
	~IGlSolverIkConstraint(){
	}

	void SetTarget( CGlSolverIkTargetChain *t ){ if(t!=NULL) m_Target=t; }
	void SetWeight( double v ){ m_WeightConstraint=v; }

	virtual void Potential(double &valOut)=0;
	virtual void DerPotential(CMatrix *vecOut)=0;
};

///// サンプルの制約
class CGlSolverIkConstraint_Put : public IGlSolverIkConstraint
{	///// その場所に位置する
protected:
	CVector3H m_PosPut;

public:
	CGlSolverIkConstraint_Put(){
		strncpy( m_NamePrivate, "Put", 64 );
	}
	void SetPosPut( CVector3H *pos ){ m_PosPut = *pos; }

	void Potential(double &valOut);
	void DerPotential(CMatrix *vecOut);
};

class CGlSolverIkConstraint_Face : public IGlSolverIkConstraint
{	///// その方向を向く、ただし腹の方向はＺ軸です
protected:
	CVector3H m_VecFace;

public:
	CGlSolverIkConstraint_Face(){
		strncpy( m_NamePrivate, "Face", 64 );
	}
	void SetVecFace( CVector3H *vec ){ m_VecFace = *vec; }

	void Potential(double &valOut);
	void DerPotential(CMatrix *vecOut);
};

class CGlSolverIkConstraint_Surface : public IGlSolverIkConstraint
{	///// 仮想物体の表面に接する、時間がかかる
protected:
	CGlClay *m_Clay;
	double m_RateFace, m_RatePut;

public:
	CGlSolverIkConstraint_Surface(){
		strncpy( m_NamePrivate, "Surface", 64 );
		m_RateFace = 0.5; m_RatePut = 0.5;
	}
	void SetClay( CGlClay *clay ){ m_Clay = clay; }
	void SetRateWeight( double face, double put ){ m_RateFace=face; m_RatePut=put; }

	void Potential(double &valOut);
	void DerPotential(CMatrix *vecOut);
};

class CGlSolverIkManager : public IGlSolverManager
{
protected:
	IGlSolverIkConstraint *m_Constraints;
	CGlArthro *m_Arthro;
	CGlSolverIkTargetChain **m_Targets;
	int m_TargetMax;

	int m_FlowMax;
	double m_AngRange, m_AngStep;

	Ch3PlainData m_Data;

public:
	CGlSolverIkManager(){
		m_Constraints = NULL;
		m_Arthro = NULL;
		m_Targets = NULL;
		m_TargetMax = 0;

		m_FlowMax = 3;
		m_AngRange = 20.0;
		m_AngStep = 1.0;
	}
	~CGlSolverIkManager(){
		if( m_TargetMax > 0 ){
			for( int c=0; c<m_TargetMax; c++ ) delete m_Targets[ c ];
			m_Targets=NULL; m_TargetMax=0;
		}
	}

	void InitArthro( CGlArthro *arthro ){ m_Arthro = arthro; }
	
	void AddConstraint( IGlSolverIkConstraint *constrint );

	bool LoadChains( Ch3PlainData *data );
	bool LoadChains( char *loc ){
		Ch3PlainData data; if( data.Load(loc) ){ return LoadChains(&data); } return false; 
	}

	//void SetWeight( char *name, double val );

	bool Run();
private:
	void ChangeTargetChain( CGlSolverIkTargetChain *target );
	void SetArrAngTarget( CGlSolverIkTargetChain *target, CMatrix *matAng );
	void GetArrAngTarget( CGlSolverIkTargetChain *target, CMatrix *matAng );
	void CalcPot( CGlSolverIkTargetChain *target, CMatrix *matAng, double &valOut );
	void CalcDerPot( CGlSolverIkTargetChain *target, CMatrix *matAng, CMatrix *derOut );
};
