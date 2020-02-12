#include "StdAfx.h"
#include "DevU3dTracker.h"

#pragma comment(lib , "WSock32.lib")

CDevU3dTrackerImpl *CDevU3dTracker::m_Impl=NULL;

/*!
	@brief ポートを開いて、U3Dと通信を行う
	@param[in] port ポート番号
*/
bool CDevU3dTrackerImpl::OpenConnection(int port)
{
	WORD wVersionRequested = MAKEWORD(1,1);
	WSADATA wsaData;
	HOSTENT *lpHost;
	SOCKADDR_IN saddr;
	int nRet;

	// WinSockの初期化
	nRet = WSAStartup(wVersionRequested, &wsaData);
	if (wsaData.wVersion != wVersionRequested) {	
		fprintf(stderr,"\n Wrong version\n");
		return false;
	}

	// ホスト名からホスト情報を取得（Find the Server）
	lpHost = gethostbyname(szServer);
    if (lpHost == NULL) {
        fprintf(stderr,"\n%s: %d\n", "gethostbyname()", WSAGetLastError());
        return false;
    }

	// Create a TCP/IP stream socket
	soc = socket(AF_INET, SOCK_STREAM, 0);
	if (soc == INVALID_SOCKET) {
		fprintf(stderr, "\n%s: %d\n", "socket()", WSAGetLastError());
		return false;
	}

	// Fill in the address structure
	memset(&saddr, 0, sizeof(SOCKADDR_IN));
	saddr.sin_family = lpHost->h_addrtype;
	saddr.sin_port = htons(port);
	saddr.sin_addr = *((LPIN_ADDR)*lpHost->h_addr_list);
										// ^ Server's address
	
	// connect to the server
	if (connect(soc, (SOCKADDR *)&saddr, sizeof(saddr)) == SOCKET_ERROR) {
		fprintf(stderr,"\n%s: %d\n", "socket()", WSAGetLastError());
		closesocket(soc);
		WSACleanup();
		return false;
	}
	
	m_isConnected = true;
    fprintf(stderr,"connected\n");
	return true;
}

/*!
	@brief U3Dと通信を終了する
*/
void CDevU3dTrackerImpl::CloseConnection(void)
{
	if( !m_isConnected )return;

	closesocket(soc);
	m_isConnected = false;
	// Release WinSock
	WSACleanup();
}

/*!
	@brief U3Dと接続されているかどうかを調べる
*/
bool CDevU3dTrackerImpl::IsConnected()
{
	return m_isConnected;
}

/*!
	@brief ビーコンのすべての位置を問い合わせる。ビーコン位置は関数get1PositionForRight()で獲得する
*/
bool CDevU3dTrackerImpl::UpdatePos()
{
	if( !m_isConnected )return false;
	char a = 0x00;
	send(soc, &a, 1, 0);	// send 1 character   
    recv(soc, (char *)u3d_data, sizeof(int) * 10 * 3, 0);	// receive 3D positions 
	return true;
}

/*!
	@brief num番のビーコン位置を獲得する
	@param[in] num 番
	@param[out] pPos ビーコン位置
	@param[in] isUpdated ビーコンの位置を問い合わせるかどうか
*/
bool CDevU3dTrackerImpl::GetPos_Right( unsigned int num, double *pPos, bool isUpdated )
{
	if( !m_isConnected )return false;
	if( isUpdated )UpdatePos();
	pPos[0] = (double)u3d_data[num][0] / 10.0;
	pPos[1] = (double)u3d_data[num][1] / 10.0;
	pPos[2] = (double)u3d_data[num][2] / 10.0;
	return true;
}

/*!
	@brief num番のビーコン位置を獲得する
	@param[in] num 番
	@param[out] pPos ビーコン位置
	@param[in] isUpdated ビーコンの位置を問い合わせるかどうか
*/
bool CDevU3dTrackerImpl::GetPos_Right( unsigned int num, float *pPos, bool isUpdated )
{
	if( !m_isConnected )return false;
	if( isUpdated )UpdatePos();
	pPos[0] = (float)( u3d_data[num][0] / 10.0 );
	pPos[1] = (float)( u3d_data[num][1] / 10.0 );
	pPos[2] = (float)( u3d_data[num][2] / 10.0 );
	return true;
}