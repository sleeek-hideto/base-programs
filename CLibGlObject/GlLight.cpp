#include "StdAfx.h"
#include "GlLight.h"

/*!
	@brief ライトの名前を設定をする
	@param[in] ライトの番号
*/
void CGlLight::SetName( unsigned int name )
{
	switch( name )
	{
	case 0: m_LightName = GL_LIGHT0; break;
	case 1: m_LightName = GL_LIGHT1; break;
	case 2: m_LightName = GL_LIGHT2; break;
	case 3: m_LightName = GL_LIGHT3; break;
	}
}

/*!
	@brief 環境光の色の設定
*/
void CGlLight::SetColAmbi( double *col )
{
	if( col == NULL ){
		m_ColAmbi[0] = 0.0;
		m_ColAmbi[1] = 0.0;
		m_ColAmbi[2] = 0.0;
		m_ColAmbi[3] = 1.0;
		return;
	}

	m_ColAmbi[0] = (float)col[0];
	m_ColAmbi[1] = (float)col[1];
	m_ColAmbi[2] = (float)col[2];
	m_ColAmbi[3] = (float)col[3];
}

/*!
	@brief 環境光の色の設定
*/
void CGlLight::SetColAmbi( float r, float g, float b, float a )
{
	m_ColAmbi[0] = r;
	m_ColAmbi[1] = g;
	m_ColAmbi[2] = b;
	m_ColAmbi[3] = a;
}

/*!
	@brief 拡散光の色の設定
*/
void CGlLight::SetColDiff( double *col )
{
	if( col == NULL ){
		m_ColDiff[0] = 0.0;
		m_ColDiff[1] = 0.0;
		m_ColDiff[2] = 0.0;
		m_ColDiff[3] = 1.0;
		return;
	}

	m_ColDiff[0] = (float)col[0];
	m_ColDiff[1] = (float)col[1];
	m_ColDiff[2] = (float)col[2];
	m_ColDiff[3] = (float)col[3];
}

/*!
	@brief 拡散光の色の設定
*/
void CGlLight::SetColDiff( float r, float g, float b, float a )
{
	m_ColDiff[0] = r;
	m_ColDiff[1] = g;
	m_ColDiff[2] = b;
	m_ColDiff[3] = a;
}

/*!
	@brief 鏡面反射光の色の設定
*/
void CGlLight::SetColSpec( double *col )
{
	if( col == NULL ){
		m_ColSpec[0] = 0.0;
		m_ColSpec[1] = 0.0;
		m_ColSpec[2] = 0.0;
		m_ColSpec[3] = 1.0;
		return;
	}

	m_ColSpec[0] = (float)col[0];
	m_ColSpec[1] = (float)col[1];
	m_ColSpec[2] = (float)col[2];
	m_ColSpec[3] = (float)col[3];
}

/*!
	@brief 鏡面反射光の色の設定
*/
void CGlLight::SetColSpec( float r, float g, float b, float a )
{
	m_ColSpec[0] = r;
	m_ColSpec[1] = g;
	m_ColSpec[2] = b;
	m_ColSpec[3] = a;
}

bool CGlLight::LoadObject( Ch3PlainData *data )
{
	if( data == NULL )return false;

	data->SetNameSpace( "Frame" );

	data->Get( -1, "/LgtRepeat/Set", &m_IsEventRepeat );

	return LoadPosture( data, -1, NULL );
}

bool CGlLight::LoadPosture( Ch3PlainData *data, int frame, char *namePrivate )
{
	if( data == NULL )return false;

	data->SetNameSpace( "Frame" );

	char strPrivate[ 64 ], name[ 64 ];
	if( namePrivate == NULL ){
		strPrivate[ 0 ] = 0;
	}else{
		sprintf( strPrivate, "%s", namePrivate );
	}

	double tmpD4[ 4 ] = { 0,0,0,1.0 };

	sprintf( name, "%s/LgtPos/Set", strPrivate );
	if( data->Get( frame, name, tmpD4 ) > 2 ){
		SetPos( tmpD4 );
	}

	sprintf( name, "%s/LgtColAmbi/Set", strPrivate );
	if( data->Get( frame, name, tmpD4 ) > 3 ){
		SetColAmbi( tmpD4 );
	}
	sprintf( name, "%s/LgtColDiff/Set", strPrivate );
	if( data->Get( frame, name, tmpD4 ) > 3 ){
		SetColDiff( tmpD4 );
	}
	sprintf( name, "%s/LgtColSpec/Set", strPrivate );
	if( data->Get( frame, name, tmpD4 ) > 3 ){
		SetColSpec( tmpD4 );
	}

	return true;
}

bool CGlLight::SavePosture( Ch3PlainData *data, int frame, char *namePrivate )
{
	if( data == NULL )return false;

	char strPrivate[ 64 ], name[ 64 ];
	if( namePrivate == NULL ){
		strPrivate[ 0 ] = 0;
	}else{
		sprintf( strPrivate, "%s", namePrivate );
	}

	sprintf( name, "%s/LgtPos/Set", strPrivate );
	data->Set( frame, name, 3, m_Pos );

	sprintf( name, "%s/LgtColAmbi/Set", strPrivate );
	data->Set( frame, name, 4, m_ColAmbi );
	sprintf( name, "%s/LgtColDiff/Set", strPrivate );
	data->Set( frame, name, 4, m_ColDiff );
	sprintf( name, "%s/LgtColSpec/Set", strPrivate );
	data->Set( frame, name, 4, m_ColSpec );

	return true;
}

bool CGlLight::LoadEvent( char *locFile )
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

bool CGlLight::Run()
{
	if( m_DataEvent == NULL )return false;
	LoadPosture( m_DataEvent, m_FrameNow, NULL );

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

void CGlLight::TurnOn()
{
	m_IsOn = true;

	glEnable( GL_LIGHTING );
	//glEnable(GL_LIGHT0);
	glLightfv( m_LightName, GL_POSITION, m_Pos );
	glLightfv( m_LightName, GL_AMBIENT, m_ColAmbi );
	glLightfv( m_LightName, GL_DIFFUSE, m_ColDiff );
	glLightfv( m_LightName, GL_SPECULAR, m_ColSpec );
	glLightf( m_LightName, GL_SPOT_CUTOFF, 180.0 );
}

void CGlLight::TurnOff()
{
	m_IsOn = false;

	glDisable( GL_LIGHTING );
	glDisable( m_LightName );
}
