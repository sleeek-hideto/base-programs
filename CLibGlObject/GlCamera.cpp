#include "StdAfx.h"
#include "GlCamera.h"

bool CGlCamera::LoadObject( Ch3PlainData *data )
{
	if( data == NULL )return false;

	data->SetNameSpace( "Camera" );

	///// Env
	data->Get( -1, "/CamRepeat/Set", &m_IsEventRepeat );

	///// Perspective
	double tmpD16[ 16 ];
	if( data->Get( -1, "/CamPro/Prs", tmpD16 ) > 3 ){
		SetProjection_Perspective( tmpD16[0], tmpD16[1], tmpD16[2], tmpD16[3] );
	}
	if( data->Get( -1, "/CamPro/Fru", tmpD16 ) > 5 ){
		SetProjection_Frustum( tmpD16[0], tmpD16[1], tmpD16[2], tmpD16[3], tmpD16[4], tmpD16[5] );
	}

	return true;
}

bool CGlCamera::LoadPosture( Ch3PlainData *data, int frame, char *namePrivate )
{
	if( data == NULL )return false;

	data->SetNameSpace( "Frame" );

	char strPrivate[ 64 ], name[ 64 ];
	if( namePrivate == NULL ){
		strPrivate[ 0 ] = 0;
	}else{
		sprintf( strPrivate, "%s", namePrivate );
	}
	CVector3H tmpV; double *pV = tmpV.GetData();

	//double tmpD16[ 16 ];
	///// Perspective
	//sprintf( name, "%s/CamPro/Prs", strPrivate );
	//if( data->Get( frame, name, tmpD16 ) > 3 ){
	//	SetProjection_Perspective( tmpD16[0], tmpD16[1], tmpD16[2], tmpD16[3] );
	//}
	//sprintf( name, "%s/CamPro/Fru", strPrivate );
	//if( data->Get( frame, name, tmpD16 ) > 5 ){
	//	SetProjection_Frustum( tmpD16[0], tmpD16[1], tmpD16[2], tmpD16[3], tmpD16[4], tmpD16[5] );
	//}

	///// Camera
	sprintf( name, "%s/Cam/SetPosGlo", strPrivate );
	if( data->Get( frame, name, tmpV.GetData() ) > 2 ){
		SetPosCameraGlobal( &tmpV );
	}
	sprintf( name, "%s/Cam/MovPosGlo", strPrivate );
	if( data->Get( frame, name, tmpV.GetData() ) > 2 ){
		MovePosCameraGlobal( &tmpV );
	}
	sprintf( name, "%s/Cam/MovPosLoc", strPrivate );
	if( data->Get( frame, name, tmpV.GetData() ) > 2 ){
		MovePosCameraLocal( &tmpV );
	}

	sprintf( name, "%s/Fcs/SetPos", strPrivate );
	if( data->Get( frame, name, pV ) > 2 ){
		SetPosLookAtGlobal( pV[0], pV[1], pV[2] );
	}
	sprintf( name, "%s/Cam/MovPosFcs", strPrivate );
	if( data->Get( frame, name, pV ) > 2 ){
		MovePosCameraLookAt( pV[0], pV[1], pV[2] );
	}

	///// Angle
	sprintf( name, "%s/Cam/SetAng", strPrivate );
	if( data->Get( frame, name, tmpV.GetData() ) > 2 ){
		SetAngle( &tmpV );
	}
	sprintf( name, "%s/Cam/MovAng", strPrivate );
	if( data->Get( frame, name, tmpV.GetData() ) > 2 ){
		MoveAngle( &tmpV );
	}
	sprintf( name, "%s/Fcs/AddAng", strPrivate );
	if( data->Get( frame, name, tmpV.GetData() ) > 2 ){
		MovePosLookAtLocal( tmpV.GetData()[0], tmpV.GetData()[1] );
	}

	///// Command
	char com[ 64 ];
	sprintf( name, "%s/Cam/Com", strPrivate );
	if( data->Get( frame, name, com ) > 0 ){
		if( strcmp( com, "Stop" ) == 0 ){
			m_Command[ 0 ] = 0;
		}else{
			sprintf( m_Command, "%s", com );
		}
	}

	return true;
}

bool CGlCamera::SavePosture( Ch3PlainData *data, int frame, char *namePrivate )
{
	if( data == NULL ) return false;

	data->SetNameSpace( "Frame" );

	char strPrivate[ 64 ], name[ 64 ];
	if( namePrivate == NULL ){
		strPrivate[ 0 ] = 0;
	}else{
		sprintf( strPrivate, "%s", namePrivate );
	}

	///// Camera
	sprintf( name, "%s/Cam/SetPosGlo", strPrivate );
	data->Set( frame, name, 3, m_PosCamera.GetData() );

	if( m_ModeCameraAngle == MANUAL )
	{
		sprintf( name, "%s/Cam/SetAng", strPrivate );
		data->Set( frame, name, 3, m_AngleCamera.GetData() );
	}
	if( m_ModeCameraAngle == LOOKAT )
	{
		sprintf( name, "%s/Fcs/SetPos", strPrivate );
		data->Set( frame, name, 3, m_PosLookAt.GetData() );
	}

	return true;
}

/*!
	@brief イベント設定ファイルを読み込む
	@param[in] locFile イベント・ファイル位置
*/
bool CGlCamera::LoadEvent( char *locFile )
{
	if( m_DataEvent != NULL )return false;

	m_DataEvent = new Ch3PlainData;
	if( m_DataEvent->Load( locFile ) == false ){
		delete m_DataEvent;
		m_DataEvent = NULL;
		return false;
	}

	LoadObject( m_DataEvent );
	m_DataEvent->Get( -1, "frameMax", &m_FrameMax );
	m_FrameNow = 0;
	return true;
}

/*!
	@brief イベントを実行・計算する
*/
bool CGlCamera::Run()
{
	if( m_DataEvent == NULL )return false;
	LoadPosture( m_DataEvent, m_FrameNow, NULL );

	///// command
	double valNormal=2.0, valFast=5.0;

	if( strcmp( m_Command, "Right" ) == 0 )MovePosCameraLookAt( valNormal, 0, 0 );
	if( strcmp( m_Command, "Left" ) == 0 )MovePosCameraLookAt( -valNormal, 0, 0 );
	if( strcmp( m_Command, "Up" ) == 0 )MovePosCameraLookAt( 0, valNormal, 0 );
	if( strcmp( m_Command, "Down" ) == 0 )MovePosCameraLookAt( 0, -valNormal, 0 );
	if( strcmp( m_Command, "Front" ) == 0 )MovePosCameraLookAt( 0, 0, valNormal );
	if( strcmp( m_Command, "Back" ) == 0 )MovePosCameraLookAt( 0, 0, -valNormal );
	if( strcmp( m_Command, "FastRight" ) == 0 )MovePosCameraLookAt( valFast, 0, 0 );
	if( strcmp( m_Command, "FastLeft" ) == 0 )MovePosCameraLookAt( -valFast, 0, 0 );
	if( strcmp( m_Command, "FastUp" ) == 0 )MovePosCameraLookAt( 0, valFast, 0 );
	if( strcmp( m_Command, "FastDown" ) == 0 )MovePosCameraLookAt( 0, -valFast, 0 );
	if( strcmp( m_Command, "FastFront" ) == 0 )MovePosCameraLookAt( 0, 0, valFast );
	if( strcmp( m_Command, "FastBack" ) == 0 )MovePosCameraLookAt( 0, 0, -valFast );

	m_FrameNow++;
	if( m_FrameNow >= m_FrameMax ){
		if( !m_IsEventRepeat ){
			delete m_DataEvent; m_DataEvent=NULL;
		}else{
			m_FrameNow = 0;
		}
	}
	return true;
}

/*!
	@brief プロジェクション行列をPerspective型に設定する
	@param[in] fovy 縦方向の角度
	@param[in] aspect 縦横比（横／縦）
	@param[in] near 描写される距離の下限
	@param[in] far 描写される距離の上限
*/
void CGlCamera::SetProjection_Perspective( double fovy, double aspect, double zNear, double zFar )
{
	m_ParamProjection[ 0 ] = fovy;
	m_ParamProjection[ 1 ] = aspect;
	m_ParamProjection[ 2 ] = zNear;
	m_ParamProjection[ 3 ] = zFar;
	m_ModeProjection = PERSPECTIVE;
}

/*!
	@brief プロジェクション行列をFrustum型に設定する
	@param[in] left 描写される左側の位置
	@param[in] right 描写される右側の位置
	@param[in] top 描写される上側の位置
	@param[in] bottom 描写される下側の位置
	@param[in] near 描写される距離の下限
	@param[in] far 描写される距離の上限
*/
void CGlCamera::SetProjection_Frustum( double left, double right, double top, double bottom, double zNear, double zFar )
{
	m_ParamProjection[ 0 ] = left;
	m_ParamProjection[ 1 ] = right;
	m_ParamProjection[ 2 ] = top;
	m_ParamProjection[ 3 ] = bottom;
	m_ParamProjection[ 4 ] = zNear;
	m_ParamProjection[ 5 ] = zFar;
	m_ModeProjection = FRUSTUM;
}

/*!
	@brief プロジェクション行列をカメラ行列3x3に設定する
	@param[in] mat カメラ行列。Open CV で求められる
*/
void CGlCamera::SetProjection_Matrix3x3( CMatrix *mat, double windowWidth, double windowHeight )
{
	if( mat->GetRowsCols() != 9 )return;
	double *pData = mat->GetData();
	m_ParamProjection[ 0 ] = -pData[2]/100.0;
	m_ParamProjection[ 1 ] = (windowWidth - pData[2])/100.0;
	m_ParamProjection[ 2 ] =  (windowHeight - pData[5])/100.0;
	m_ParamProjection[ 3 ] = -pData[5]/100.0;
	m_ParamProjection[ 4 ] = (pData[0] + pData[4])/200.0;
	m_ParamProjection[ 5 ] = (pData[0] + pData[4])/0.2;
	m_ModeProjection = FRUSTUM;
}

/*!
	@brief プロジェクション行列を獲得する、転置している
	@param[out] mat プロジェクション行列
*/
void CGlCamera::GetMatrixProjection( CMatrix4x4 *mat )
{
	glGetDoublev(GL_PROJECTION_MATRIX, mat->GetData() );
}

/*
	@brief カメラの向きで移動させる
*/
void CGlCamera::MovePosCameraLocal( CVector3H *v )
{
	double *vecData = v->GetData();
	if( vecData[ 0 ] == 0.0 && vecData[ 1 ] == 0.0 && vecData[ 2 ] == 0.0 )return;

	CMatrix4x4 matRot;
	CVector3H vecGlobal;
	double *angData = m_AngleCamera.GetData();
	matRot.SetRotYXZTran( angData[ 0 ], angData[ 1 ], angData[ 2 ] );
	vecGlobal.Mul( &matRot, v );
	MovePosCameraGlobal( &vecGlobal );
}

/*!
	@brief カメラをLookAtの点を中心に動かす
	@param[in] angleHorizon 横に何度動かす
	@param[in] angleVertical 縦に何度動かす
	@param[in] lengthZ 前後にどれだけ動かす
*/
void CGlCamera::MovePosCameraLookAt( double angleHorizon, double angleVertical, double forward )
{
	double nowHorizon, nowVertical, nowRadius;
	CVector3H vecCam;
	vecCam.Sub( &m_PosCamera, &m_PosLookAt );
	nowRadius = vecCam.Length();
	vecCam.Normalize();

	double *xyz = vecCam.GetData();
	nowVertical = acos( xyz[ 1 ] );
	if( abs( xyz[ 1 ] ) > 0.999 ){
		nowHorizon = 0.0;
	}else{
		nowHorizon = atan2( xyz[ 0 ], -xyz[ 2 ] );
	}

	nowHorizon += angleHorizon*PI/180.0;
	nowVertical += angleVertical*PI/180.0;
	if( nowVertical > PI ) nowVertical = PI;
	if( nowVertical < -PI ) nowVertical = -PI;
	nowRadius -= forward;
	xyz = vecCam.GetData();
	xyz[ 0 ] = nowRadius * sin(nowHorizon) * sin(nowVertical);
	xyz[ 1 ] = nowRadius * cos(nowVertical);
	xyz[ 2 ] = - nowRadius * cos(nowHorizon) * sin(nowVertical);
	m_PosCamera.Add( &m_PosLookAt, &vecCam );
}

void CGlCamera::MovePosLookAtLocal( double lenHorizon, double lenVertical )
{
	if( lenHorizon == 0.0 && lenVertical == 0.0 )return;

	CVector3H vecHorizon, vecVertical, vecAdd(0,0,0);
	CVector3H vecLook, vecLookTmp, vecAxisY(0,1,0);
	double lenFoot;
	vecLook.Sub( &m_PosLookAt, &m_PosCamera );
	vecLook.Normalize();
	lenFoot = vecLook.GetData()[1];
	vecLookTmp.MulConst( lenFoot, &vecLook );
	vecVertical.Sub( &vecAxisY, &vecLookTmp );
	vecVertical.Normalize();

	vecHorizon.CrossProduct( &vecLook, &vecVertical );

	vecVertical.MulConst( lenVertical );
	vecHorizon.MulConst( -lenHorizon );
	vecAdd.Add( &vecVertical, &vecHorizon );

	m_PosCamera.Add( &vecAdd );
	m_PosLookAt.Add( &vecAdd );
}

void CGlCamera::SendMatrixProjectionInit()
{
	double *pData = m_ParamProjection;
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();

	if( m_ModeProjection == FRUSTUM )
	{
		glFrustum( pData[0], pData[1], pData[2], pData[3], pData[4], pData[5]);
	}else if( m_ModeProjection == PERSPECTIVE )
	{
		gluPerspective( pData[0], pData[1], pData[2], pData[3] );
	}
}

void CGlCamera::SendMatrixProjectionMove()
{
	if( m_ModeCameraAngle == MANUAL )
	{
		///// 移動してからカメラを回転させる
		double *pData = m_AngleCamera.GetData();
		glRotated( -pData[1], 0.0, 1.0, 0.0 );
		glRotated( -pData[0], 1.0, 0.0, 0.0 );
		glRotated( -pData[2], 0.0, 0.0, 1.0 );
		pData = m_PosCamera.GetData();
		glTranslated( -pData[0], -pData[1], -pData[2] );

		///// カメラを回転させてから移動する
		//pData = m_AngleCamera.GetData();
		//glRotated( pData[1], 0.0, 1.0, 0.0 );
		//glRotated( pData[0], 1.0, 0.0, 0.0 );
		//glRotated( pData[2], 0.0, 0.0, 1.0 );
		//pData = m_PosCamera.GetData();
		//glTranslated( -pData[0], -pData[1], -pData[2] );
	}else if( m_ModeCameraAngle == LOOKAT ){
		double *cam = m_PosCamera.GetData();
		double *look = m_PosLookAt.GetData();
		gluLookAt(
		cam[0], cam[1], cam[2],
		look[0], look[1], look[2],
		0.0, 1.0, 0.0 );
	}
}
