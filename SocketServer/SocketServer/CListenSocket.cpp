// Ŭ���̾�Ʈ�� �޼����� ��ٸ��� ��� ����ϴ� CListenSocket Ŭ����
// ��������� ������Ͽ� ����������!



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

void CListenSocket::OnAccept(int nErrorCode) // ����ڰ� �߰��Ǿ��� ��
{
	CClientSocket* pClient = new CClientSocket;
	CString str;

	if (Accept(*pClient)) {
		pClient->SetListenSocket(this);
		m_ptrClientSocketList.AddTail(pClient);

		CSocketServerDlg* pMain = (CSocketServerDlg*)AfxGetMainWnd();
		str.Format(_T("Client (%d)"), (int)m_ptrClientSocketList.Find(pClient));
		pMain->clientList->AddString(str);
		// Client ����Ʈ�� �߰�

	}
	else {
		delete pClient;
		AfxMessageBox(_T("ERROR : Failed can't accept new Client!"));
	}

	CAsyncSocket::OnAccept(nErrorCode);
}

void CListenSocket::CloseClientSocket(CSocket* pClient) //Client ������ ������ ��
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
				// Client ����Ʈ���� ����
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

void CListenSocket::SendAllMessage(TCHAR* pszMessage) // Client�� ���� �޼����� ���� ��
{
	POSITION pos;
	pos = m_ptrClientSocketList.GetHeadPosition();
	CClientSocket* pClient = NULL;

	while (pos != NULL) {
		pClient = (CClientSocket*)m_ptrClientSocketList.GetNext(pos);
		if (pClient != NULL) {

			int checkLenOfData = pClient->Send(pszMessage, lstrlen(pszMessage) * 2);
			// Ŭ���̾�Ʈ�鿡�� �޽��� ���� 

			if (checkLenOfData != lstrlen(pszMessage) * 2) {
				AfxMessageBox(_T("�Ϻ� �����Ͱ� ���������� ���۵��� ���߽��ϴ�!"));
			}
		}
	}
}