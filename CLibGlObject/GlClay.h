#pragma once

/*
*	@brief 仮想物体・粘土のクラス、下層、上層の二つのスキンを持ちます
*/
class CGlClay : public IGlObjectDraw
{
protected:
	CGl2Bone m_Bone;
	CGl2Skin m_SkinLocal;
	CGl2Skin m_SkinLocalDeform;
	CGl2Skin m_SkinWorld;
	CGl2Skin m_SkinAlpha;
	CGl2Skin m_SkinAlphaDeform;

public:
	CGlClay(){
		m_Bone.AddSkin( &m_SkinLocal, &m_SkinLocalDeform, &m_SkinWorld );
		m_Bone.AddSkin( &m_SkinAlpha, &m_SkinAlphaDeform );

		m_SkinLocal.SetType( "Solid" );
		m_SkinAlpha.SetType( "Alpha" );
	}

	~CGlClay(){
	}

	int MakeSolidCube( double r=1.0, char *locTex=NULL );
	int MakeSolidSphere( double r=1.0, char *locTex=NULL );

	bool LoadObject( Ch2PlainData *inData );
	bool LoadObject( char *loc ){
		Ch2PlainData data;
		data.load( loc );
		return LoadObject( &data );
	}

	bool SavePosture( Ch3PlainData *data, int frame, char *name=NULL );
	bool LoadPosture( Ch3PlainData *data, int frame, char *name=NULL );

	inline void CalcPosture(){	
		m_Bone.CalcPosture_recursive();
		//m_SkinLocal.m_ArrPos->TestPrint( "pos" );
	}
	inline void Draw(){
		///// 描写する順番が大事
		glMatrixMode( GL_MODELVIEW );
		//glLoadIdentity();
		m_Bone.Draw_recursive( "Solid" );
		m_Bone.Draw_recursive( "Alpha" );
	}

	///// あまり必要でない
	double SearchNearestSurface_Point( CVector3H *inPoiPos, CVector3H *outSurPos, CVector3H *outSurNrm );
	double SearchNearestSurface_Line( CVector3H *inPoiPos, CVector3H *inVec, CVector3H *outSurPos, CVector3H *outAsiPos );

	inline void SetSolidScale( CVector3H *v ){
		m_SkinLocal.Scale( v );
	}
	inline void SetAlphaScale( CVector3H *v ){
		m_SkinAlpha.Scale( v );
	}

	inline void SetPos( CVector3H *v ){	m_Bone.SetPos( v );	}
	inline void AddPos( CVector3H *v ){	m_Bone.AddPos( v );	}
	inline void SetPos( double *d ){	m_Bone.SetPos( d );	}
	inline void AddPos( double *d ){	m_Bone.AddPos( d );	}
	inline void SetAngle( CVector3H *v ){	m_Bone.SetAngle( v );	}
	inline void AddAngle( CVector3H *v ){	m_Bone.AddAngle( v );	}

	inline void GetPos( CVector3H *v ){	v->Copy( m_Bone.GetPosGlobalEdge() );	}
	inline CVector3H *GetPos(){	return m_Bone.GetPosGlobalEdge();	}
	inline void GetAngle( CVector3H *v ){	v->Copy( m_Bone.GetAngle() );	}
	inline CVector3H *GetAngle(){	return m_Bone.GetAngle();	}

	inline void SetSolidColor( CVector4 *ambi, CVector4 *diff, CVector4 *spec ){
		m_SkinLocal.SetColor( ambi, diff, spec );
		m_SkinLocalDeform.SetColor( ambi, diff, spec );
	}
	inline void SetAlphaColor( CVector4 *ambi, CVector4 *diff, CVector4 *spec ){
		m_SkinAlpha.SetColor( ambi, diff, spec );
		m_SkinAlphaDeform.SetColor( ambi, diff, spec );
	}

	inline void SetSolidMatDeform( CMatrix4x4 *mat ){ m_SkinLocalDeform.SetMatDeformed( mat ); }
	inline void SetAlphaMatDeform( CMatrix4x4 *mat ){ m_SkinAlphaDeform.SetMatDeformed( mat ); }

	/////デバッグ用
	void TestPrintVertex( char *name ){
		char str[ 128 ];
		if( m_SkinLocal.m_ArrPos!=NULL ){ sprintf( str, "%s:Pos", name ); m_SkinLocal.m_ArrPos->TestPrint( str ); }
		if( m_SkinLocal.m_ArrIndex!=NULL ){ sprintf( str, "%s:Ind", name ); m_SkinLocal.m_ArrIndex->TestPrint( str ); }
		//if( m_SkinLocal.m_ArrNrm!=NULL ){ sprintf( str, "%s:Nrm", name ); m_SkinLocal.m_ArrNrm->TestPrint( str ); }
	}
};
