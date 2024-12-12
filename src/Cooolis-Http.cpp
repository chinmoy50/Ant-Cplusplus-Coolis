#include "Cooolis-Http.h"

CCooolisHttp::CCooolisHttp()
{
	hSession = WinHttpOpen(
		LIB_HTTP_USER_AGENT,
		WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
		WINHTTP_NO_PROXY_NAME,
		WINHTTP_NO_PROXY_BYPASS,
		0);
}

BOOL CCooolisHttp::ConnectServer(LPCWSTR pswzServerName, INTERNET_PORT nServerPort)
{
	if (hSession == NULL) {
		// Session��ʼ������
		return FALSE;
	}
	hConnect = WinHttpConnect(hSession, pswzServerName, nServerPort, 0);
	if (hConnect != NULL) {
		return TRUE;
	}
	return FALSE;
}

BOOL CCooolisHttp::HttpAddHeaders(LPCWSTR szHeader)
{
	// ������ȴ���0�����
	if (lstrlen(szHeader) > 0) {
		// ���Header
		this->szHeaders.push_back(szHeader);
		return TRUE;
	}
	return FALSE;
}

BOOL CCooolisHttp::HttpAddHeaders(std::vector<std::wstring> szHeaders)
{
	// ֱ�Ӹ�ֵ�ⲿHeaders,����Ľ�ֱ���滻
	this->szHeaders = szHeaders;
	return 0;
}

DWORD CCooolisHttp::HttpGet(LPCWSTR pszServerURI, std::vector<BYTE>& wszResponse)
{
	DWORD dwResponseContentLen = 0; // ��Ӧ���С
	LPVOID lpszResponseBody = NULL; // ��Ӧ����
	// ���ӷ�����
	if (!hConnect) {
		// ���Ӵ���
		return -1;
	}
	// ���ӳɹ�
	// printf("[+] hSession WinHttpConnect Host \n");
	// �����������
	hRequest = WinHttpOpenRequest(
		hConnect,
		TEXT("GET"),   // Request Method
		pszServerURI,  // Request URI
		NULL,
		WINHTTP_NO_REFERER, // û��Referer
		WINHTTP_DEFAULT_ACCEPT_TYPES,
		WINHTTP_FLAG_ESCAPE_DISABLE
	);
	// ����HTTPͷ
	this->SetHeaders();

	// ����HTTP����
	if (this->SendRequest() == FALSE) {
		return -1;
	}
	// ��ȡ��Ӧ ContentLength,���û������,�򷵻� 0
	dwResponseContentLen = this->GetResponseContentLength();
	if (dwResponseContentLen == 0) {
		return dwResponseContentLen;
	}
	// �����ڴ�ռ����ڴ�ŷ�������
	lpszResponseBody = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, dwResponseContentLen);
	// ��HTTP��Ӧ��ȡ����
	if (WinHttpReadData(hRequest, lpszResponseBody, dwResponseContentLen, &dwResponseContentLen) == FALSE) {
		return -1;
	}
	// printf("[+] GET Request Content-Length : %d \n", dwResponseContentLen);
	// ����Ӧ���ݷ���
	for (DWORD i = 0; i < dwResponseContentLen; i++)
	{
		BYTE byS = (BYTE) * ((PCHAR)lpszResponseBody + i);
		wszResponse.push_back(byS);
	}

	// printf("[+] Error %u in WinHttpReadData.\n", GetLastError());
	// �ͷ�ԭ��������ڴ�
	HeapFree(GetProcessHeap(), HEAP_NO_SERIALIZE, lpszResponseBody);
	// �ر�������
	if (hRequest) WinHttpCloseHandle(hRequest);
	// ������ҳ��С
	return dwResponseContentLen;
}

DWORD CCooolisHttp::HttpPost(LPCWSTR pszServerURI, LPVOID pszSendData, DWORD dwSendDataLen, std::vector<BYTE>& wszResponse)
{
	DWORD dwResponseContentLen = 0; // ��Ӧ���С
	LPVOID lpszResponseBody = NULL; // ��Ӧ����
	// ���ӷ�����
	if (!hConnect) {
		// ���Ӵ���
		return -1;
	}
	// ���ӳɹ�
	// printf("[+] hSession WinHttpConnect Host \n");
	// �����������
	hRequest = WinHttpOpenRequest(
		hConnect,
		TEXT("POST"),   // Request Method
		pszServerURI,  // Request URI
		NULL,
		WINHTTP_NO_REFERER, // û��Referer
		WINHTTP_DEFAULT_ACCEPT_TYPES,
		WINHTTP_FLAG_ESCAPE_DISABLE
	);
	// ����HTTPͷ
	this->SetHeaders();

	// ����HTTP����
	if (this->SendRequest(pszSendData, dwSendDataLen) == FALSE) {
		return -1;
	}
	// ��ȡ��Ӧ ContentLength,���û������,�򷵻� 0
	dwResponseContentLen = this->GetResponseContentLength();
	if (dwResponseContentLen == 0) {
		return dwResponseContentLen;
	}
	// �����ڴ�ռ����ڴ�ŷ�������
	lpszResponseBody = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, dwResponseContentLen);
	// ��HTTP��Ӧ��ȡ����
	if (WinHttpReadData(hRequest, lpszResponseBody, dwResponseContentLen, &dwResponseContentLen) == FALSE) {
		return -1;
	}
	// printf("[+] POST Request Content-Length : %d \n", dwResponseContentLen);
	// ����Ӧ���ݷ���
	for (DWORD i = 0; i < dwResponseContentLen; i++)
	{
		BYTE byS = (BYTE) * ((PCHAR)lpszResponseBody + i);
		wszResponse.push_back(byS);

	}
	// �ͷ�ԭ��������ڴ�
	HeapFree(GetProcessHeap(), HEAP_NO_SERIALIZE, lpszResponseBody);
	// �ر�������
	if (hRequest) WinHttpCloseHandle(hRequest);
	// ������ҳ��С
	return dwResponseContentLen;
}

// ��Http�ļ������ڴ�
LPVOID CCooolisHttp::ReadHttpFile(LPCWSTR pszServerURI, DWORD& dwResponseLength)
{
	// DWORD dwResponseContentLen = 0; // ��Ӧ���С
	LPVOID lpszResponseBody = NULL; // ��Ӧ����
	// ���ӷ�����
	if (!hConnect) {
		// ���Ӵ���
		return NULL;
	}
	// ���ӳɹ�
	// printf("[+] hSession WinHttpConnect Host \n");
	// �����������
	hRequest = WinHttpOpenRequest(
		hConnect,
		TEXT("GET"),   // Request Method
		pszServerURI,  // Request URI
		NULL,
		WINHTTP_NO_REFERER, // û��Referer
		WINHTTP_DEFAULT_ACCEPT_TYPES,
		WINHTTP_FLAG_ESCAPE_DISABLE | WINHTTP_FLAG_SECURE
	);
	// ����HTTPͷ
	this->SetHeaders();

	// ����HTTP����
	if (this->SendRequest() == FALSE) {
		return NULL;
	}
	// ��ȡ��Ӧ ContentLength,���û������,�򷵻� 0
	dwResponseLength = this->GetResponseContentLength();
	if (dwResponseLength == 0) {
		return NULL;
	}
	// �����ڴ�ռ����ڴ�ŷ�������
	lpszResponseBody = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, dwResponseLength);
	// ��HTTP��Ӧ��ȡ����
	if (WinHttpReadData(hRequest, lpszResponseBody, dwResponseLength, &dwResponseLength) == FALSE) {
		return NULL;
	}
	
	// �ر�������
	if (hRequest) WinHttpCloseHandle(hRequest);
	// ������ҳ��С
	return lpszResponseBody;
}

VOID CCooolisHttp::SetHeaders()
{
	INT nHeaderCount = this->szHeaders.size();
	// wprintf(TEXT("[+] Header Size : %d \n"), nHeaderCount);
	if (nHeaderCount <= 0) {
		return VOID();
	}
	for (INT i = 0; i < nHeaderCount; i++)
	{
		WinHttpAddRequestHeaders(hRequest,
			szHeaders[i].data(),
			szHeaders[i].length(),
			WINHTTP_ADDREQ_FLAG_ADD | WINHTTP_ADDREQ_FLAG_REPLACE);
	}
	return VOID();
}

DWORD CCooolisHttp::GetResponseContentLength()
{
	DWORD dwResponseLength = 0;
	DWORD dwCch = sizeof(DWORD);
	WinHttpQueryHeaders(hRequest,
		WINHTTP_QUERY_CONTENT_LENGTH | WINHTTP_QUERY_FLAG_NUMBER,
		WINHTTP_HEADER_NAME_BY_INDEX,
		&dwResponseLength,
		&dwCch,
		WINHTTP_NO_HEADER_INDEX);
	return dwResponseLength;
}

BOOL CCooolisHttp::SendRequest()
{
	BOOL bIsSend = FALSE;


	if (hRequest == NULL)
		return FALSE;


	DWORD dwFlags = SECURITY_FLAG_IGNORE_UNKNOWN_CA |
		SECURITY_FLAG_IGNORE_CERT_WRONG_USAGE |
		SECURITY_FLAG_IGNORE_CERT_CN_INVALID |
		SECURITY_FLAG_IGNORE_CERT_DATE_INVALID;
	// ����HTTPS
	WinHttpSetOption(hRequest, WINHTTP_OPTION_SECURITY_FLAGS, &dwFlags, sizeof(dwFlags));
	WinHttpSetOption(hRequest, WINHTTP_OPTION_CLIENT_CERT_CONTEXT, WINHTTP_NO_CLIENT_CERT_CONTEXT, 0);

	// ����HTTP����
	bIsSend = WinHttpSendRequest(
		hRequest,
		WINHTTP_NO_ADDITIONAL_HEADERS,
		0,
		WINHTTP_NO_REQUEST_DATA,
		NULL,
		0,
		0);
	if (bIsSend) {
		// ��ʼ������Ӧ
		WinHttpReceiveResponse(hRequest, NULL);
		return TRUE;
	}
	return bIsSend;
}

BOOL CCooolisHttp::SendRequest(LPVOID pswzSendData, DWORD dwSendDataLen)
{
	// �������д���󳤶�,Ĭ�ϲ������κ�����
	if (dwSendDataLen == 0) {
		return this->SendRequest();
	}
	// �Ƿ�����ɹ�
	BOOL bIsSend = FALSE;
	if (hRequest == NULL)
		return FALSE;
	// ����HTTP����
	bIsSend = WinHttpSendRequest(
		hRequest,
		WINHTTP_NO_ADDITIONAL_HEADERS,
		0,
		pswzSendData,
		dwSendDataLen,
		dwSendDataLen,
		0);
	if (bIsSend) {
		// ��ʼ������Ӧ
		WinHttpReceiveResponse(hRequest, NULL);
		return TRUE;
	}
	return bIsSend;
}
