#pragma once

/*
	@brief U3dTrackerから位置情報を取ってくる、シングルトン・クラス
*/
class CDevU3dTrackerImpl
{
protected:
	bool m_isConnected;
	int u3d_data[10][3];
	SOCKET soc;
	char szServer[32];

public:
	CDevU3dTrackerImpl(){ 
		m_isConnected = false;
		strcpy(szServer, "localhost");
	}
	~CDevU3dTrackerImpl(){
		CloseConnection();
	}

public:
	bool OpenConnection( int port=20000 );
	void CloseConnection();
	bool IsConnected();

	bool UpdatePos();
	bool GetPos_Right( unsigned int num, double *pPos, bool isUpdated=true );
	bool GetPos_Right( unsigned int num, float *pPos, bool isUpdated=true );
};

class CDevU3dTracker
{
protected:
	static CDevU3dTrackerImpl *m_Impl;

public:
	static CDevU3dTrackerImpl *Get(){
		if( m_Impl == NULL )m_Impl = new CDevU3dTrackerImpl;
		return m_Impl;
	}
	static void Destroy(){
		if( m_Impl != NULL ){ delete m_Impl; m_Impl = NULL; }
	}
};
