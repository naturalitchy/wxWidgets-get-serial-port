#include "get-serial-port.h"


GetSerialPort::GetSerialPort()
: portName() {

}

GetSerialPort::~GetSerialPort() {

}

int GetSerialPort::getCOMPort() {

	HRESULT hres;				// ���� �Ǵ� ��� �����ϴ� �� ���Ǵ� 32��Ʈ ���Դϴ�.

	// Initialize COM.
	/*
	ȣ�� �����忡�� ����� COM ���̺귯���� �ʱ�ȭ�ϰ� �������� ���ü� ���� �����ϸ� �ʿ��� ��� �����忡 ���� �� ����Ʈ�� ����ϴ�.
	Windows ��Ÿ�� API�� ����ϰų� COM �� Windows ��Ÿ�� ���� ��Ҹ� ��� ����Ϸ��� ���
	Windows::Foundation::Initialize�� ȣ���Ͽ� CoInitializeEx ��� �����带 �ʱ�ȭ�ؾ� �մϴ�.
	Windows::Foundation::Initialize�� COM ���� ��ҿ� ����ϱ⿡ ����մϴ�.
	*/
	hres = CoInitializeEx(0, COINIT_MULTITHREADED);
	if (FAILED(hres)) {
		std::cout << "Failed to initialize COM library. "
			<< "Error code = 0x"
			<< std::hex << hres << std::endl;
		return 0;              // Program has failed.
	}

	// Initialize 
	hres = CoInitializeSecurity(
		NULL,
		-1,      // COM negotiates service                  
		NULL,    // Authentication services
		NULL,    // Reserved
		RPC_C_AUTHN_LEVEL_DEFAULT,    // authentication
		RPC_C_IMP_LEVEL_IMPERSONATE,  // Impersonation
		NULL,             // Authentication info 
		EOAC_NONE,        // Additional capabilities
		NULL              // Reserved
	);


	if (FAILED(hres)) {
		std::cout << "Failed to initialize security. " << "Error code = 0x" << std::hex << hres << std::endl;
		CoUninitialize();
		return 0;          // Program has failed.
	}

	// Obtain the initial locator to Windows Management
	// on a particular host computer.
	IWbemLocator *pLoc = 0;

	hres = CoCreateInstance(
		CLSID_WbemLocator,
		0,
		CLSCTX_INPROC_SERVER,
		IID_IWbemLocator, (LPVOID *)&pLoc);

	if (FAILED(hres)) {
		std::cout << "Failed to create IWbemLocator object. " << "Error code = 0x" << std::hex << hres << std::endl;
		CoUninitialize();
		return 0;       // Program has failed.
	}

	IWbemServices *pSvc = 0;

	// Connect to the root\cimv2 namespace with the
	// current user and obtain pointer pSvc
	// to make IWbemServices calls.

	hres = pLoc->ConnectServer(
		_bstr_t(L"ROOT\\CIMV2"), // WMI namespace
		NULL,                    // User name
		NULL,                    // User password
		0,                       // Locale
		NULL,                    // Security flags                 
		0,                       // Authority       
		0,                       // Context object
		&pSvc                    // IWbemServices proxy
	);

	if (FAILED(hres)) {
		std::cout << "Could not connect. Error code = 0x" << std::hex << hres << std::endl;
		pLoc->Release();
		CoUninitialize();
		return 0;                // Program has failed.
	}

	std::cout << "Connected to ROOT\\CIMV2 WMI namespace" << std::endl;

	// Set the IWbemServices proxy so that impersonation
	// of the user (client) occurs.
	hres = CoSetProxyBlanket(
		pSvc,                         // the proxy to set
		RPC_C_AUTHN_WINNT,            // authentication service
		RPC_C_AUTHZ_NONE,             // authorization service
		NULL,                         // Server principal name
		RPC_C_AUTHN_LEVEL_CALL,       // authentication level
		RPC_C_IMP_LEVEL_IMPERSONATE,  // impersonation level
		NULL,                         // client identity 
		EOAC_NONE                     // proxy capabilities     
	);

	if (FAILED(hres)) {
		std::cout << "Could not set proxy blanket. Error code = 0x" << std::hex << hres << std::endl;
		pSvc->Release();
		pLoc->Release();
		CoUninitialize();
		return 0;               // Program has failed.
	}

	// Use the IWbemServices pointer to make requests of WMI. 
	// Make requests here:

	// For example, query for all the running processes
	IEnumWbemClassObject* pEnumerator = NULL;
	hres = pSvc->ExecQuery(
		bstr_t("WQL"),
		//bstr_t("SELECT * FROM Win32_PnPEntity"),
		bstr_t("SELECT * FROM Win32_SerialPort"),
		WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,
		NULL,
		&pEnumerator);

	if (FAILED(hres)) {
		std::cout << "Query for processes failed. " << "Error code = 0x" << std::hex << hres << std::endl;
		pSvc->Release();
		pLoc->Release();
		CoUninitialize();
		return 0;               // Program has failed.
	} else {
		IWbemClassObject *pclsObj;
		ULONG uReturn = 0;

		while (pEnumerator) {
			hres = pEnumerator->Next(WBEM_INFINITE, 1, &pclsObj, &uReturn);

			if (0 == uReturn) {
				break;
			}

			VARIANT vtProp;

			// Get the value of the Name property
			hres = pclsObj->Get(L"Name", 0, &vtProp, 0, 0);
			std::wcout << "Process Name : " << vtProp.bstrVal << std::endl;
			assert(vtProp.bstrVal != nullptr);
			std::wstring wstr(vtProp.bstrVal, SysStringLen(vtProp.bstrVal));
			std::string str(wstr.begin(), wstr.end());
			int findNum = str.find("Arduino Mega 2560");
			if (findNum != -1) {
				printf(" > find Success! \n");
			}
			printf("findNum = %d \n", findNum);
			StaticVariable::autoComPortName = str;

			std::cout << "StaticVariable::comPortName(getSerialPort) = " << StaticVariable::autoComPortName << std::endl;
			VariantClear(&vtProp);
			pclsObj->Release();
			pclsObj = NULL;

			return 1;
		}
	}

	// Cleanup
	// ========
	pSvc->Release();
	pLoc->Release();
	pEnumerator->Release();

	CoUninitialize();


}