// 클라이언트의 메세지를 기다리며 계속 대기하는 CListenSocket 클래스
// 멤버변수들 헤더파일에 선언해주자!



#include "stdafx.h"
#include "SocketServer.h"
#include "CListenSocket.h"
#include "CClientSocket.h"
#include "SocketServerDlg.h"

CListenSocket::CListenSocket()
{
}

CListenSocket::~CListenSocket()
{
}

void CListenSocket::OnAccept(int nErrorCode) // 사용자가 추가되었을 때
{
	CClientSocket* pClient = new CClientSocket;
	CString str;

	if (Accept(*pClient)) {
		pClient->SetListenSocket(this);
		m_ptrClientSocketList.AddTail(pClient);

		CSocketServerDlg* pMain = (CSocketServerDlg*)AfxGetMainWnd();
		str.Format(_T("Client (%d)"), (int)m_ptrClientSocketList.Find(pClient));
		pMain->clientList->AddString(str);
		// Client 리스트에 추가

	}
	else {
		delete pClient;
		AfxMessageBox(_T("ERROR : Failed can't accept new Client!"));
	}

	CAsyncSocket::OnAccept(nErrorCode);
}

void CListenSocket::CloseClientSocket(CSocket* pClient) //Client 접속이 끊겼을 때
{
	POSITION pos;
	pos = m_ptrClientSocketList.Find(pClient);

	if (pos != NULL) {
		if (pClient != NULL) {
			pClient->ShutDown();
			pClient->Close();
		}

		CSocketServerDlg* pMain = (CSocketServerDlg*)AfxGetMainWnd();
		CString str1, str2;
		UINT indx = 0, posNum;
		pMain->clientList->SetCurSel(0);

		while (indx < pMain->clientList->GetCount()) {
			posNum = (int)m_ptrClientSocketList.Find(pClient);
			pMain->clientList->GetText(indx, str1);
			str2.Format(_T("%d"), posNum);

			if (str1.Find(str2) != -1) {
				// Client 리스트에서 삭제
				AfxMessageBox(str1 + str2);
				pMain->clientList->DeleteString(indx);
				break;
			}
			indx++;
		}

		m_ptrClientSocketList.RemoveAt(pos);
		delete pClient;
	}
}

void CListenSocket::SendAllMessage(TCHAR* pszMessage) // Client가 보낸 메세지가 있을 때
{
	POSITION pos;
	pos = m_ptrClientSocketList.GetHeadPosition();
	CClientSocket* pClient = NULL;

	while (pos != NULL) {
		pClient = (CClientSocket*)m_ptrClientSocketList.GetNext(pos);
		if (pClient != NULL) {

			int checkLenOfData = pClient->Send(pszMessage, lstrlen(pszMessage) * 2);
			// 클라이언트들에게 메시지 전송 

			if (checkLenOfData != lstrlen(pszMessage) * 2) {
				AfxMessageBox(_T("일부 데이터가 정상적으로 전송되지 못했습니다!"));
			}
		}
	}
}