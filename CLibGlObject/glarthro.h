#pragma once

class CGlArthro : public IGlObjectDraw
{
protected:
	int m_nBoneMax;
	int m_nSkinMax;
	CVector3H m_PosRoot;
	CGlBone *m_RootBone;
	CGlBone *m_Bone;
	CGlSkin *m_Skin;

public:
	CGlArthro(void){
		m_nBoneMax = 0;	m_nSkinMax = 0;
		m_RootBone = NULL;	m_Bone = NULL;	m_Skin = NULL;
	}
	~CGlArthro(void){
		if( m_Bone != NULL ){ delete [] m_Bone; m_nBoneMax=0; }
		if( m_Skin != NULL ){ delete [] m_Skin; m_nSkinMax=0; }
	}

	/*!
		@brief ボーンの総数を返す
		@return ボーンの総数
	*/
	inline int GetBoneMax()							{ return m_nBoneMax; }
	inline CGlBone *GetBone( int idx=0 )	{ if(idx>=m_nBoneMax){ return NULL; } return m_Bone +idx; }

	inline void SetBonePosLocal(int id, CVector3H *v){ if(id>=m_nBoneMax)return; m_Bone[id].SetPos(v); }
	inline void SetBonePosLocal(int id, double *d){ if(id>=m_nBoneMax)return; m_Bone[id].SetPos(d); }
	inline void AddBonePosLocal(int id, CVector3H *v){ if(id>=m_nBoneMax)return; m_Bone[id].AddPos(v); }
	inline void AddBonePosLocal(int id, double *d){ if(id>=m_nBoneMax)return; m_Bone[id].AddPos(d); }

	inline bool SetBoneAngle(int id, CVector3H *v)	{ if(id>=m_nBoneMax){ return false; } m_Bone[id].SetAngle(v); return true; }
	inline bool SetBoneAngle(int id, double *d)		{ if(id>=m_nBoneMax){ return false; } m_Bone[id].SetAngle(d); return true; }
	inline void SetBoneAllAngle(double* pF)			{ for(int i=0; i<m_nBoneMax; i++){ m_Bone[i].SetAngle(pF +i*3); } }
	inline bool AddBoneAngle(int id, CVector3H *v)	{ if(id>=m_nBoneMax){ return false; } m_Bone[id].AddAngle(v); return true; }
	inline bool AddBoneAngle(int id, double *d)		{ if(id>=m_nBoneMax){ return false; } m_Bone[id].AddAngle(d); return true; }
	inline bool GetBoneAngle(int id, CVector3H *v)	{ if(id>=m_nBoneMax){ return false; } v->Copy( m_Bone[id].GetAngle() ); return true; }
	inline CVector3H* GetBoneAngle(int id)			{ if(id>=m_nBoneMax){ return NULL; } return m_Bone[id].GetAngle(); }
	inline void GetBoneAngle(int id, double *d)		{ if(id>=m_nBoneMax)return; m_Bone[id].GetAngle(d); }
	inline void GetBoneAllAngle(double* pF)			{ for(int i=0; i<m_nBoneMax; i++){ m_Bone[i].GetAngle(pF +i*3); } }

	inline bool SetBoneAngleInit(int id, CVector3H *v)	{ if(id>=m_nBoneMax){ return false; } m_Bone[id].SetAngleInit(v); return true; }
	inline bool SetBoneAngleInit(int id, double *d)		{ if(id>=m_nBoneMax){ return false; } m_Bone[id].SetAngleInit(d); return true; }
	inline bool AddBoneAngleInit(int id, CVector3H *v)	{ if(id>=m_nBoneMax){ return false; } m_Bone[id].AddAngleInit(v); return true; }
	inline bool AddBoneAngleInit(int id, double *d)		{ if(id>=m_nBoneMax){ return false; } m_Bone[id].AddAngleInit(d); return true; }
	inline CVector3H* GetBoneAngleInit(int id)			{ if(id>=m_nBoneMax){ return NULL; } return m_Bone[id].GetAngleInit(); }

	virtual bool SetRootPosRot_by3Sensor(CVector3H* wristLeft, CVector3H* wristRight, CVector3H *arm, double ctlNrm=-3.0, double ctlDir=1.0);

	/*!
		@brief 多関節オブジェの付け根位置を設定する
		@param[in] v 位置
	*/
	inline void SetRootPos(double *v)						{ m_PosRoot.SetData(v); }
	inline void SetRootPos(CVector3H *v)					{ m_PosRoot.Copy(v); }
	inline void AddRootPos(double *v)						{ m_PosRoot.AddData(v); }
	inline void AddRootPos(CVector3H *v)					{ m_PosRoot.Add(v); }
	inline CVector3H* GetRootPos()							{ return &m_PosRoot; }

	/*!
		@brief インデックスidのボーンの相対位置を返す
		@param[in] id ボーン・インデックス
	*/
	inline CVector3H* GetBonePosLocal(int id)				{ if(id>=m_nBoneMax){ return NULL; } return m_Bone[id].GetPosLocal(); }
	
	/*!
		@brief インデックスidのボーンの根本の絶対位置を返す
		@param[in] id ボーン・インデックス
	*/
	inline CVector3H* GetBonePosEdge(int id)				{ if(id>=m_nBoneMax){ return NULL; } return m_Bone[id].GetPosGlobalEdge(); }
	
	/*!
		@brief インデックスidのボーンの先端の絶対位置を返す
		@param[in] id ボーン・インデックス
	*/
	inline CVector3H* GetBonePosCenter(int id)				{ if(id>=m_nBoneMax){ return NULL; } return m_Bone[id].GetPosGlobalCenter(); }
	inline CVector3H* GetPartXaxis(int id)					{ if(id>=m_nBoneMax){ return NULL; } return m_Bone[id].GetAxisX(); }
	inline CVector3H* GetPartYaxis(int id)					{ if(id>=m_nBoneMax){ return NULL; } return m_Bone[id].GetAxisY(); }
	inline CVector3H* GetPartZaxis(int id)					{ if(id>=m_nBoneMax){ return NULL; } return m_Bone[id].GetAxisZ(); }

	bool CopyArthroPosAndAngle( CGlArthro *target );

	///// Skin
	inline int GetSkinMax(){ return m_nSkinMax; }
	inline void ShiftSkinPos( int id, CVector3H *v ){ if(id>=m_nSkinMax)return; m_Skin[id].Shift(v); }

	///// Skin Texture
	unsigned int GetSkinTextureId( int id ){
		if( id>=m_nSkinMax )return 0;
		return m_Skin[ id ].GetTexture();
	}

	bool LoadObject( Ch2PlainData *data );
	bool LoadObject( char* loc ){
		Ch2PlainData data; if( !data.load( loc ) )return false;
		return LoadObject( &data );
	}

	bool LoadObject( Ch3PlainData *data ){
		if( LoadObjectBones(data) && LoadObjectSkins(data) ){
			data->SetNameSpace("Control"); double v[3]; if(data->Get(-1,"addRootPos",v)>2)AddRootPos(v);
			return true;
		}
		return false;
	}
	bool LoadObjectBones( Ch3PlainData *data );
	bool LoadObjectSkins( Ch3PlainData *data );
	bool SaveObject( Ch3PlainData *data );

	bool LoadPosture( Ch3PlainData *data, int frame, char *name=NULL );
	bool SavePosture( Ch3PlainData *data, int frame, char *name=NULL );

///// スキンに色をつける
	void SetBoneColor( int id, CVector4 *col ){
		if(m_nBoneMax<=id)return;
		CGlSkin *sk=m_Bone[id].GetSkin(); while(sk!=NULL){sk->SetColorFilter(col); sk=sk->GetSibling(); }
	}
	void SetBoneColor( int id, double *col ){
		if(m_nBoneMax<=id)return;
		CGlSkin *sk=m_Bone[id].GetSkin(); while(sk!=NULL){sk->SetColorFilter(col); sk=sk->GetSibling(); }
	}
	void SetSkinColor( int id, CVector4 *col ){ if(m_nSkinMax<=id)return; m_Skin[id].SetColorFilter(col); }
	void SetSkinColor( int id, double *col ){ if(m_nSkinMax<=id)return; m_Skin[id].SetColorFilter(col); }
	void SetAllSkinColor( CVector4 *col ){ for(int i=0; i<m_nSkinMax; i++){ m_Skin[i].SetColorFilter(col); } }
	void SetAllSkinColor( double *col ){ for(int i=0; i<m_nSkinMax; i++){ m_Skin[i].SetColorFilter(col); } }

	void CalcPosture(){
		m_RootBone->CalcPosture_recursive();
		for(int b=0; b<m_nBoneMax; b++){///// これでよいかな
			m_Bone[b].m_PosGlobalCenter.Add( &m_PosRoot );
			m_Bone[b].m_PosGlobalEdge.Add( &m_PosRoot );
		}
	}
	void Draw(){
		glMatrixMode( GL_MODELVIEW );
		glPushMatrix();
		//if( isLoadedIdentity ) glLoadIdentity();
		if( m_RootBone != NULL ){
			double *d=m_PosRoot.GetData(); glTranslated(d[0],d[1],d[2]);
			m_RootBone->Draw_recursive();
		}
		glPopMatrix();
	}
	void DrawOneBone( int id ){
		glMatrixMode( GL_MODELVIEW );
		glPushMatrix();
		//if( isLoadedIdentity ) glLoadIdentity();
		double *d=m_PosRoot.GetData(); glTranslated(d[0],d[1],d[2]);
		m_Bone[ id ].Draw_oneBone();
		glPopMatrix();
	}

///// For Debug
	void TestPrintBonePos( int id ){
		m_Bone[ id ].m_PosLocal.TestPrint( "Len", id );
	}
	void TestPrintBoneAngInit( int id ){
		m_Bone[ id ].m_AngInit.TestPrint( "Ang", id );
	}
	void TestPrintSkinPos( int id ){
		m_Skin[ id ].m_ArrPos->TestPrint( "Pos", id );
	}
	void TestPrintSkinInd( int id ){
		m_Skin[ id ].m_ArrIndex->TestPrint( "Index", id );
	}
	void TestPrintSkinNrm( int id ){
		m_Skin[ id ].m_ArrNrm->TestPrint( "Nrm", id );
	}
	void TestPrintAllSkinMinMax();
};
