#pragma once

class CGlCamera : public IGlObject
{
protected:
	enum ModeProjection { PERSPECTIVE, FRUSTUM } m_ModeProjection;	//!<プロジェクションの方法
	double m_ParamProjection[ 16 ];	//!<プロジェクション・パラメータ

	CVector3H m_PosCamera;

	enum _ModeCameraRotation { MANUAL, LOOKAT } m_ModeCameraAngle;
	CVector3H m_AngleCamera;
	CVector3H m_PosLookAt;

	bool m_IsEventRepeat;
	Ch3PlainData *m_DataEvent;
	int m_FrameNow, m_FrameMax;
	char m_Command[ 64 ];

public:
	CGlCamera(void){
		m_PosCamera.SetData( 0,0,0 );
		m_AngleCamera.SetData( 0,0,0 );

		m_ModeProjection = PERSPECTIVE;
		m_ParamProjection[ 0 ] = 45.0;
		m_ParamProjection[ 1 ] = 1.5;
		m_ParamProjection[ 2 ] = 10.0;
		m_ParamProjection[ 3 ] = 3000.0;

		m_ModeCameraAngle = MANUAL;

		m_IsEventRepeat = false;
		m_DataEvent = NULL;
		m_FrameNow = 0;
		m_FrameMax = 1;
		m_Command[ 0 ] = 0;
	}
	~CGlCamera(void){
		if( m_DataEvent != NULL )delete m_DataEvent;
	}

	bool IsModeAngleManual(){ return m_ModeCameraAngle == MANUAL; }
	bool IsModeAngleLookAt(){ return m_ModeCameraAngle == LOOKAT; }

	CVector3H *GetPosCamera(){ return &m_PosCamera; }
	CVector3H *GetAngleCamera(){ return &m_AngleCamera; }
	CVector3H *GetPosLookAt(){ return &m_PosLookAt; }

public:
	bool LoadObject( Ch3PlainData *data );
	bool LoadObject( char *locFile ){
		Ch3PlainData data; if( !data.Load(locFile) )return false;
		return LoadObject(&data);
	}
public:
	bool LoadPosture( Ch3PlainData *data, int frame, char *namePrivate=NULL );
	bool SavePosture( Ch3PlainData *data, int fmame, char *namePrivate=NULL );

	void SetEventRepeat( bool b ){ m_IsEventRepeat=b; }
	bool LoadEvent( char *locFile );
	void Command( char *str ){ if(str!=NULL){ strncpy( m_Command, str, 64 ); }else{ m_Command[0]=0; } }
	bool Run();

	void SetProjection_Perspective( double fovy=45.0, double aspect=1.5, double near=10.0, double far=3000.0 );
	void SetProjection_Frustum( double left, double right, double top, double bottom, double near, double far );
	void SetProjection_Matrix3x3( CMatrix *mat, double windowWidth, double windowHeight );
	void GetMatrixProjection( CMatrix4x4 *mat );

	void SetPosCameraGlobal( double x, double y, double z ){ m_PosCamera.SetData(x,y,z); }
	void MovePosCameraGlobal( double x, double y, double z ){ m_PosCamera.AddData(x,y,z); }
	void SetPosCameraGlobal( CVector3H *v ){ m_PosCamera.Copy(v); }
	void MovePosCameraGlobal( CVector3H *v ){ m_PosCamera.Add(v); }

	void MovePosCameraLocal( double x, double y, double z ){ CVector3H v(x,y,z); MovePosCameraLocal(&v); }
	void MovePosCameraLocal( CVector3H *v );
	void MovePosCameraLookAt( double angleHorizon, double angleVertical, double forward );

	void SetAngle( double x, double y, double z ){ m_ModeCameraAngle=MANUAL; m_AngleCamera.SetData(x,y,z); }
	void MoveAngle( double x, double y, double z ){ m_ModeCameraAngle=MANUAL; m_AngleCamera.AddData(x,y,z); }
	void SetAngle( CVector3H *v ){ m_ModeCameraAngle=MANUAL; m_AngleCamera.Copy(v); }
	void MoveAngle( CVector3H *v ){ m_ModeCameraAngle=MANUAL; m_AngleCamera.Add(v); }

	void CopyPosLookAt( CVector3H *v, bool maskX=false, bool maskY=false, bool maskZ=false ){
		double *md=m_PosLookAt.GetData(), *id=v->GetData();
		if( !maskX )md[0] = id[0]; if( !maskY )md[1] = id[1]; if( !maskZ )md[2] = id[2];
	}
	void SetPosLookAtGlobal( double x, double y, double z ){ m_ModeCameraAngle=LOOKAT; m_PosLookAt.SetData(x,y,z); }
	void MovePosLookAtGlobal( double x, double y, double z ){ m_ModeCameraAngle=LOOKAT; m_PosLookAt.AddData(x,y,z); }
	void SetPosLookAtGlobal( CVector3H *v ){ m_ModeCameraAngle=LOOKAT; m_PosLookAt.Copy(v); }
	void MovePosLookAtGlobal( CVector3H *v ){ m_ModeCameraAngle=LOOKAT; m_PosLookAt.Add(v); }

	void MovePosLookAtLocal( double lenHorizon, double lenVertical );

	void SendMatrixProjection(){
		SendMatrixProjectionInit();
		SendMatrixProjectionMove();
	}
	void SendMatrixProjectionInit();
	void SendMatrixProjectionMove();

	void TestPrintPos(){
		m_PosCamera.TestPrint( "Camera Pos" );
	}
	void TestPrintAngle(){
		m_AngleCamera.TestPrint( "Camera Angle" );
	}
};
