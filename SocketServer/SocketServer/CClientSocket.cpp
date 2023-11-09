// 클라이언트의 추가 등 클라이언트와 관련한 작업을 수행하는 CCientSocket
// OnReceive : 소켓에서 데이터를 수신할 때 실행되며, 수신된 데이터를 처리하고 다른 클라이언트의 메시지를 전달합니다.

#include "stdafx.h"
#include "SocketServer.h"
#include "CClientSocket.h"
#include "CListenSocket.h"
#include "SocketServerDlg.h"


// CClientSocket
int index = 0;
CString alias[100][2];

CClientSocket::CClientSocket()
{
}

CClientSocket::~CClientSocket()
{
}


// CClientSocket 멤버 함수
void CClientSocket::SetListenSocket(CAsyncSocket* pSocket) // 새로 접속한 소켓 값을 넣는다
{
	m_pListenSocket = pSocket;
}
// CClientSocket 멤버 함수


void CClientSocket::OnClose(int nErrorCode) // 연결이 종료되었을 때 소켓을 닫는다
{
	CSocket::OnClose(nErrorCode);

	CListenSocket* pServerSocket = (CListenSocket*)m_pListenSocket;
	pServerSocket->CloseClientSocket(this);
}

void CClientSocket::OnReceive(int nErrorCode) // 소켓에 데이터가 들어왔을 때
{
	int i, check = 0;
	CString strTmp = _T(""), strIPAddress = _T("");
	UINT uPortNumber = 0;
	TCHAR strBuffer[1024]; // 데이터 저장
	::ZeroMemory(strBuffer, sizeof(strBuffer));

	GetPeerName(strIPAddress, uPortNumber);
	// ip주소와 포트번호를 받고
	if (Receive(strBuffer, sizeof(strBuffer)) > 0) {
		CSocketServerDlg* pMain = (CSocketServerDlg*)AfxGetMainWnd();
		
		CString portStr;
		portStr.Format(L"%d", uPortNumber);

		for (i = index - 1; i >= 0; i--) {
			if (portStr == alias[i][0]) { // 사용자가 지정한 이름과 매칭
				strTmp.Format(_T("[%s]: %s"), alias[i][1], strBuffer);
				break;
			}
		}

		if (i == -1) {
			strTmp.Format(_T("[%s:%d]: %s"), strIPAddress, uPortNumber, strBuffer);
		}
		// 출력하고 싶은 모양대로 가공하고

		if (strTmp.Find(L"alias:") != -1) { // 이름 설정하는 신호였다면
			alias[index][0] = portStr; // 이름 저장
			CString temp = strBuffer;
			temp.Delete(0, 6);
			alias[index][1] = temp;
			index++;
			check = 1;
		}

		if (check == 0) { // 이름 설정하는 신호가 아니라면 (메시지 전송)
			int cnt = pMain->m_List.GetCount();
			pMain->m_List.InsertString(cnt, strTmp);

			CListenSocket* pServerSocket = (CListenSocket*)m_pListenSocket;

			TCHAR *tChr = (TCHAR*)(LPCTSTR)strTmp;
			pServerSocket->SendAllMessage(tChr);
		}
		else {
			check = 0;
		}
	}

	CSocket::OnReceive(nErrorCode); // 소켓 추가 동작을 처리
}