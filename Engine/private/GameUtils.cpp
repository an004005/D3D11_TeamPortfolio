#include "stdafx.h"
#include "..\public\GameUtils.h"
#include "GameInstance.h"
#include <sstream>
#include <dirent.h>
#include "Shlwapi.h"
#include <random>

void CGameUtils::wc2c(const wchar_t* szWchar, char* szChar)
{
	_int len;
	_int slength = lstrlen(szWchar) + 1;
	len = ::WideCharToMultiByte(CP_ACP, 0, szWchar, slength, 0, 0, 0, 0);
	::WideCharToMultiByte(CP_ACP, 0, szWchar, slength, szChar, len, 0, 0);
}

void CGameUtils::c2wc(const char* szChar, wchar_t* szWchar)
{
	_int len;
	_int slength = static_cast<_int>(strlen(szChar)) + 1;
	len = ::MultiByteToWideChar(CP_ACP, 0, szChar, slength, 0, 0);
	::MultiByteToWideChar(CP_ACP, 0, szChar, slength, szWchar, len);
}

wstring CGameUtils::s2ws(const string& s)
{
	_int len;
	_int slength = static_cast<_int>(s.length()) + 1;
	len = ::MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, 0, 0);
	wchar_t* buf = new wchar_t[len];
	::MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, buf, len);
	wstring ret(buf);
	delete[] buf;
	return ret;
}

string CGameUtils::ws2s(const wstring& s)
{
	_int len;
	_int slength = static_cast<_int>(s.length()) + 1;
	len = ::WideCharToMultiByte(CP_ACP, 0, s.c_str(), slength, 0, 0, 0, 0);
	string r(len, '\0');
	::WideCharToMultiByte(CP_ACP, 0, s.c_str(), slength, &r[0], len, 0, 0);
	return r;
}

_bool CGameUtils::IsTexture(const _tchar* pFilePath)
{
	_tchar			szExt[MAX_PATH] = TEXT("");
	_wsplitpath_s(pFilePath, nullptr, 0, nullptr, 0, nullptr, 0, szExt, MAX_PATH);
	wstring wstrExt = szExt;
	std::transform(wstrExt.begin(), wstrExt.end(), wstrExt.begin(), ::tolower);
	return wstrExt == L".dds" || wstrExt == L".jpg" || wstrExt == L".png" || wstrExt == L".bmp";
}

_bool CGameUtils::IsModel(const _tchar* pFilePath)
{
	_tchar			szExt[MAX_PATH] = TEXT("");
	_wsplitpath_s(pFilePath, nullptr, 0, nullptr, 0, nullptr, 0, szExt, MAX_PATH);
	wstring wstrExt = szExt;
	std::transform(wstrExt.begin(), wstrExt.end(), wstrExt.begin(), ::tolower);
	return wstrExt == L".fbx";
}

_bool CGameUtils::CheckExt(const _tchar* pFilePath, const vector<wstring>& Exts)
{
	_tchar			szExt[MAX_PATH] = TEXT("");
	_wsplitpath_s(pFilePath, nullptr, 0, nullptr, 0, nullptr, 0, szExt, MAX_PATH);
	wstring wstrExt = szExt;
	std::transform(wstrExt.begin(), wstrExt.end(), wstrExt.begin(), ::tolower);

	for (const auto& ext : Exts)
	{
		if (wstrExt == ext)
			return true;
	}

	return false;
}

_bool CGameUtils::CheckExt(const char* pFilePath, const vector<string>& Exts)
{
	char			szExt[MAX_PATH] = "";
	_splitpath_s(pFilePath, nullptr, 0, nullptr, 0, nullptr, 0, szExt, MAX_PATH);
	string strExt = szExt;
	std::transform(strExt.begin(), strExt.end(), strExt.begin(), ::tolower);

	for (const auto& ext : Exts)
	{
		if (strExt == ext)
			return true;
	}

	return false;
}

_bool CGameUtils::FileExist(const _tchar* pFilePath)
{
	FILE *file;
	if (0 == _wfopen_s(&file, pFilePath, L"r")) 
	{
        fclose(file);
        return true;
    }

    return false;
}

vector<string> CGameUtils::Split(const string& str, char Delimiter)
{
    istringstream iss(str);             // istringstream에 str을 담는다.
    string buffer;                      // 구분자를 기준으로 절삭된 문자열이 담겨지는 버퍼
 
    vector<string> result;
 
    // istringstream은 istream을 상속받으므로 getline을 사용할 수 있다.
    while (getline(iss, buffer, Delimiter))
	{
        result.push_back(buffer);               // 절삭된 문자열을 vector에 저장
    }
 
    return result;
}

void CGameUtils::ListFilesRecursive(const std::string& DirPath, std::function<void(const std::string&)> fileEncounter)
{
    if (auto dir = opendir(DirPath.c_str()))
	{
        while (auto f = readdir(dir))
		{
            if (!f->d_name || f->d_name[0] == '.')
				continue;

            if (f->d_type == DT_DIR) 
                ListFilesRecursive(DirPath + f->d_name + "/", fileEncounter);

            if (f->d_type == DT_REG)
                fileEncounter(DirPath + f->d_name);
        }
        closedir(dir);
    }
}

void CGameUtils::ListFiles(const std::string& DirPath, std::function<void(const std::string&)> fileEncounter)
{
    if (auto dir = opendir(DirPath.c_str()))
	{
        while (auto f = readdir(dir))
		{
            if (!f->d_name || f->d_name[0] == '.')
				continue;

            if (f->d_type == DT_REG)
                fileEncounter(DirPath + f->d_name);
        }
        closedir(dir);
    }
}

_uint CGameUtils::CountFiles(const std::string& DirPath)
{
	_uint iCnt = 0;
    if (auto dir = opendir(DirPath.c_str()))
	{
        while (auto f = readdir(dir))
		{
            if (!f->d_name || f->d_name[0] == '.')
				continue;

			if (f->d_type == DT_REG)
				++iCnt;
        }
        closedir(dir);
    }
	return iCnt;
}

string CGameUtils::GetFileName(const string& filePath)
{
	char szFileName[MAX_PATH]{};
	_splitpath_s(filePath.c_str(), nullptr, 0, nullptr, 0, szFileName, MAX_PATH, nullptr, 0);
	return szFileName;
}

void CGameUtils::WriteCStrFile(HANDLE hFile, const char* pStr)
{
	DWORD dwByte = 0;
	const _uint iStrlen = static_cast<_uint>(strlen(pStr) + 1); // include \0
	WriteFile(hFile, &iStrlen, sizeof(_uint), &dwByte, nullptr);
	WriteFile(hFile, pStr, iStrlen, &dwByte, nullptr);
}

void CGameUtils::ReadCStrFile(HANDLE hFile, OUT char* pStr, _uint iSize)
{
	DWORD dwByte = 0;
	_uint iStrlen = 0;
	ReadFile(hFile, &iStrlen, sizeof(_uint), &dwByte, nullptr);
	Assert(iSize >= iStrlen);
	ReadFile(hFile, pStr, iStrlen, &dwByte, nullptr);
}

string CGameUtils::ReadStrFile(HANDLE hFile)
{
	DWORD dwByte = 0;
	_uint iStrlen = 0;
	ReadFile(hFile, &iStrlen, sizeof(_uint), &dwByte, nullptr);

	string str;
	if (iStrlen <= 1024)
	{
		char buff[1024];
		ReadFile(hFile, buff, iStrlen, &dwByte, nullptr);
		str = buff;
	}
	else
	{
		char* buff = new char[iStrlen];
		ReadFile(hFile, buff, iStrlen, &dwByte, nullptr);
		str = buff;
		delete[] buff;
	}

	return str;
}

void CGameUtils::WriteStrFile(HANDLE hFile, const string& str)
{
	WriteCStrFile(hFile, str.c_str());
}

void CGameUtils::GetPickingRay(_float4& vRayOrigin, _float4& vRayDir)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	const POINT ptMouse = CGameUtils::GetClientCursor();

	const _float fWidth = static_cast<_float>(WINCX);
	const _float fHeight = static_cast<_float>(WINCY);

	const _matrix matViewInv = pGameInstance->Get_TransformMatrix_Inverse(CPipeLine::D3DTS_VIEW);
	const _float4x4 matPrj = pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ);
	
	const _float fViewX = (+2.0f * ptMouse.x / fWidth - 1.0f) / matPrj._11;
	const _float fViewY = (-2.0f * ptMouse.y / fHeight + 1.0f) / matPrj._22;

	vRayOrigin = _float4(0.0f, 0.0f, 0.0f, 1.f);
	vRayDir = _float4(fViewX, fViewY, 1.f, 0.f);
	_vector _vRayOrigin = XMLoadFloat4(&vRayOrigin);
	_vector _vRayDir = XMLoadFloat4(&vRayDir);

	_vRayOrigin = XMVector4Transform(_vRayOrigin, matViewInv);
	_vRayDir = XMVector4Normalize(XMVector4Transform(_vRayDir, matViewInv));

	XMStoreFloat4(&vRayOrigin, _vRayOrigin);
	XMStoreFloat4(&vRayDir, _vRayDir);
}

POINT CGameUtils::GetClientCursor()
{
	POINT pt;
	GetCursorPos(&pt);
	ScreenToClient(CLIENT_HWND, &pt);
	return pt;
}

_float CGameUtils::GetRandFloat(_float fMin, _float fMax)
{
	if (fMin >= fMax) 
		return fMin;
	
	float f = (rand() % 10000) * 0.0001f;
	
	return (f * (fMax - fMin)) + fMin;
}

_float3 CGameUtils::GetRandVector3(_float3 vMin, _float3 vMax)
{
	return _float3::Lerp(vMin, vMax, GetRandFloat(0.f, 1.f));
}

_float3 CGameUtils::GetRandVector3Sphere(_float3 vOrigin, _float fRadius)
{
	_float3 vRt;
	const _vector vRand = fRadius * XMVector3NormalizeEst(XMVectorSet(GetRandFloat(-1.f, 1.f), GetRandFloat(-1.f, 1.f), GetRandFloat(-1.f, 1.f), 0.f));
	XMStoreFloat3(&vRt, vRand);
	vRt += vOrigin;
	return vRt;
}

string CGameUtils::GetRandomString(_uint iLen)
{
    std::string str(iLen, 0);
    static const char alphanum[] =
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<> dis(0, sizeof(alphanum) - 2);
    for (int i = 0; i < iLen; ++i) 
	{
        str[i] = alphanum[dis(gen)];
    }
    return str;
}

void CGameUtils::HideCursor()
{
	while (::ShowCursor(FALSE) >= 0);
}

void CGameUtils::ShowCursor()
{
	while (::ShowCursor(TRUE) < 0);
}
