#pragma once

BEGIN(Engine)
class ENGINE_DLL CGameUtils
{
private:
	CGameUtils() = default;

public:
	static void wc2c(const wchar_t* szWchar, char* szChar);
	static void c2wc(const char* szChar, wchar_t* szWchar);
	static wstring s2ws(const string& s);
	static string ws2s(const wstring& s);

	static _bool IsTexture(const _tchar* pFilePath);
	static _bool IsModel(const _tchar* pFilePath);
	// ext는 .이 포함된 소문자 포멧으로 사용한다. : ex) .dds, .png 등
	static _bool CheckExt(const _tchar* pFilePath, const vector<wstring>& Exts);
	static _bool CheckExt(const char* pFilePath, const vector<string>& Exts);
	static _bool FileExist(const _tchar* pFilePath);
	static vector<string> Split(const string& str, char Delimiter);
	static void ListFilesRecursive(const std::string& DirPath, std::function<void(const std::string &)> fileEncounter);
	static void ListFiles(const std::string& DirPath, std::function<void(const std::string &)> fileEncounter);
	static _uint CountFiles(const std::string& DirPath);
	static string GetFileName(const string& filePath);

	static void WriteCStrFile(HANDLE hFile, const char* pStr);
	static void ReadCStrFile(HANDLE hFile, OUT char* pStr, _uint iSize);
	static void WriteStrFile(HANDLE hFile, const string& str);
	static string ReadStrFile(HANDLE hFile);

	static void GetPickingRay(OUT _float4& vRayOrigin, OUT _float4& vRayDir);
	static POINT GetClientCursor();

	static _float GetRandFloat(_float fMin = 0.f, _float fMax = 1.f);
	static _float3 GetRandVector3(_float3 vMin, _float3 vMax);
	static _float3 GetRandVector3Sphere(_float3 vOrigin, _float fRadius);

	static string GetRandomString(_uint iLen = 15);

	static _bool GetPosFromRayCast(_float4& vPos);

	static void HideCursor();
	static void ShowCursor();
};
END