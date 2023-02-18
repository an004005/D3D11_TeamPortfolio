#pragma once

namespace Engine
{
	class CTag_Finder
	{
	public:
		explicit CTag_Finder(const wchar_t* pTag)
			: m_pTargetTag(pTag)
		{
		}
		~CTag_Finder() {		}
	public:
		template<typename T>
		bool operator()(const T& pair)
		{
			if (0 == lstrcmpW(m_pTargetTag, pair.first))
			{
				return true;
			}

			return false;
		}

	private:
		const wchar_t*		m_pTargetTag = nullptr;
	};


	template <typename T>
	void Safe_Delete(T& pPointer)
	{
		if (nullptr != pPointer)
		{
			delete pPointer;
			pPointer = nullptr;
		}
	}

	template <typename T>
	void Safe_Delete_Array(T& pPointer)
	{
		if (nullptr != pPointer)
		{
			delete [] pPointer;
			pPointer = nullptr;
		}
	}

	template <typename T>
	unsigned long Safe_AddRef(T& pInstance)
	{
		unsigned long	dwRefCnt = 0;

		if (nullptr != pInstance)		
			dwRefCnt = pInstance->AddRef();		

		return dwRefCnt;
	}

	template <typename T>
	unsigned long Safe_Release(T& pInstance) 
	{
		unsigned long	dwRefCnt = 0;
		if (nullptr != pInstance)
		{
			dwRefCnt = pInstance->Release();
			if (0 == dwRefCnt)
				pInstance = nullptr;
		}

		return dwRefCnt;
	}

	inline void wc2c(const wchar_t* szWchar, char* szChar)
	{
		_int len;
		_int slength = lstrlen(szWchar) + 1;
		len = ::WideCharToMultiByte(CP_ACP, 0, szWchar, slength, 0, 0, 0, 0);
		::WideCharToMultiByte(CP_ACP, 0, szWchar, slength, szChar, len, 0, 0);
	}

	inline void c2wc(const char* szChar, wchar_t* szWchar)
	{
		_int len;
		_int slength = static_cast<_int>(strlen(szChar)) + 1;
		len = ::MultiByteToWideChar(CP_ACP, 0, szChar, slength, 0, 0);
		::MultiByteToWideChar(CP_ACP, 0, szChar, slength, szWchar, len);
	}

	inline wstring s2ws(const string& s)
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

	inline string ws2s(const wstring& s)
	{
		_int len;
		_int slength = static_cast<_int>(s.length()) + 1;
		len = ::WideCharToMultiByte(CP_ACP, 0, s.c_str(), slength, 0, 0, 0, 0);
		string r(len, '\0');
		::WideCharToMultiByte(CP_ACP, 0, s.c_str(), slength, &r[0], len, 0, 0);
		return r;
	}
}