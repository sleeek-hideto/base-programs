#pragma once

class CGlLight : public IGlObject
{
	GLenum m_LightName;

	float m_Pos[4];
	float m_ColAmbi[4], m_ColDiff[4], m_ColSpec[4];	//!<ƒ‰ƒCƒg‚ÌŠgŽUŒõA‹¾–Ê”½ŽËŒõ‚ÌF
	bool m_IsOn;

	bool m_IsEventRepeat;
	Ch3PlainData *m_DataEvent;
	int m_FrameNow, m_FrameMax;

	float m_MatPro[ 16 ];

public:
	CGlLight(void){
		m_LightName = GL_LIGHT0;

		m_Pos[0]=0.0; m_Pos[1]=1000.0; m_Pos[2]=1000.0; m_Pos[3]=1.0;

		m_ColAmbi[0]=0.0; m_ColAmbi[1]=0.0; m_ColAmbi[2]=0.0; m_ColAmbi[3]=1.0;
		m_ColDiff[0]=1.0; m_ColDiff[1]=1.0; m_ColDiff[2]=1.0; m_ColDiff[3]=1.0;
		m_ColSpec[0]=0.0; m_ColSpec[1]=0.0; m_ColSpec[2]=0.0; m_ColSpec[3]=1.0;

		m_IsOn = true;

		m_IsEventRepeat = false;
		m_DataEvent = NULL;
		m_FrameNow = 0; m_FrameMax = 1;
	}
	~CGlLight(void){
		if( m_DataEvent != NULL ){ delete m_DataEvent; m_DataEvent=NULL; }
	}

	void SetName( unsigned int name );

	void SetPos( CVector3H *pos, bool isInfinite=false ){
		if( pos==NULL )return;
		double *d=pos->GetData(); m_Pos[0]=(float)d[0]; m_Pos[1]=(float)d[1]; m_Pos[2]=(float)d[2];
		if( isInfinite )m_Pos[3]=0.0;
	}
	void SetPos( double *pos, bool isInfinite=false ){
		if( pos==NULL )return;
		m_Pos[0]=(float)pos[0]; m_Pos[1]=(float)pos[1]; m_Pos[2]=(float)pos[2];
		if( isInfinite )m_Pos[3]=0.0;
	}
	void SetPos( float x, float y, float z, bool isInfinite=false ){
		m_Pos[0]=x; m_Pos[1]=y; m_Pos[2]=z;
		if( isInfinite )m_Pos[3]=0.0;
	}
	void AddPos( CVector3H *pos ){
		if( pos==NULL )return;
		double *d=pos->GetData(); m_Pos[0]+=(float)d[0]; m_Pos[1]+=(float)d[1]; m_Pos[2]+=(float)d[2];
	}
	void AddPos( double *pos ){
		if( pos==NULL )return;
		m_Pos[0]+=(float)pos[0]; m_Pos[1]+=(float)pos[1]; m_Pos[2]+=(float)pos[2];
	}
	void AddPos( float x, float y, float z ){
		m_Pos[0]+=x; m_Pos[1]+=y; m_Pos[2]+=z;
	}

	float *GetPos(){ return m_Pos; }

	float *GetMatPro(){ return m_MatPro; }

	void SetColAmbi( double *col );
	void SetColAmbi( float r, float g, float b, float a );
	void SetColDiff( double *col );
	void SetColDiff( float r, float g, float b, float a );
	void SetColSpec( double *col );
	void SetColSpec( float r, float g, float b, float a );

	bool LoadObject( Ch3PlainData *data );
	bool LoadObject( char *locFile ){
		Ch3PlainData data; if( !data.Load(locFile) )return false;
		return LoadObject(&data);
	}

	bool LoadPosture( Ch3PlainData *data, int frame, char *namePrivate=NULL );
	bool SavePosture( Ch3PlainData *data, int frame, char *namePrivate=NULL );

	void SetEventRepeat( bool b ){ m_IsEventRepeat=b; }
	bool LoadEvent( char *locFile );
	bool Run();

	void TurnOn();
	void TurnOff();
	//void Illuminate();
};
