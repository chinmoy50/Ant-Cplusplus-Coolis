#pragma once
#include <Windows.h>
#include <Winhttp.h>
#include <iostream>
#include <vector>

#define LIB_HTTP_USER_AGENT L"Mozilla/5.02 (Linux; Android 4.1.1; Nexus 7 Build/JRO03D) AppleWebKit/535.19 (KHTML, like Gecko) Chrome/18.0.1025.166 Safari"


class CCooolisHttp {
public:
	CCooolisHttp();
	virtual BOOL ConnectServer(LPCWSTR pswzServerName, INTERNET_PORT nServerPort);
	virtual BOOL HttpAddHeaders(LPCWSTR szHeader); // ���HTTPͷ
	virtual BOOL HttpAddHeaders(std::vector<std::wstring> szHeaders); // ��Ӷ��HTTPͷ
	// ����GET����
	virtual DWORD HttpGet(
		LPCWSTR pszServerURI,
		std::vector<BYTE>& wszResponse
	);
	// ����POST����
	virtual DWORD HttpPost(
		LPCWSTR pszServerURI,
		LPVOID pszSendData,
		DWORD dwSendDataLen,
		std::vector<BYTE>& wszResponse
	);
	// ��Http Server��ȡ�ļ����ڴ�
	virtual LPVOID ReadHttpFile(LPCWSTR pszServerURI, DWORD & dwResponseLength);

private:
	HINTERNET   hSession = NULL;
	HINTERNET	hConnect = NULL;
	HINTERNET	hRequest = NULL;
	std::vector<std::wstring> szHeaders; // ����ͷ
	VOID SetHeaders(); // ��������ͷ
	DWORD GetResponseContentLength(); // ��ȡ��Ӧ���ݳ���
	BOOL SendRequest(); // ��������
	BOOL SendRequest(LPVOID pswzSendData, DWORD dwSendDataLen); // ���ʹ�����������

};