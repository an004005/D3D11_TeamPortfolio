#pragma once

// #define D3DCOLOR_ARGB(a,r,g,b) \
//     ((D3DCOLOR)((((a)&0xff)<<24)|(((r)&0xff)<<16)|(((g)&0xff)<<8)|((b)&0xff)))
//
// #define D3DCOLOR_RGBA(a,r,g,b) \
//     ((D3DCOLOR)((((r)&0xff)<<24)|(((g)&0xff)<<16)|(((b)&0xff)<<8)|((a)&0xff)))

#define D3DCOLOR_BGRA(b, g, r, a) \
    ((D3DCOLOR)((((b)&0xff)<<24)|(((g)&0xff)<<16)|(((r)&0xff)<<8)|((a)&0xff)))

#define GET_INSTANCE(CLASSNAME)	[](){											\
	CLASSNAME*	pInstance = CLASSNAME::GetInstance();							\
	if(nullptr == pInstance) {													\
	char	szMessage[MAX_PATH] = "";											\
	strcpy_s(szMessage, typeid(CLASSNAME).name());								\
	strcat_s(szMessage, "is nullptr");											\
	MessageBoxA(0, szMessage, nullptr, MB_OK);}									\
	else {																		\
	pInstance->AddRef();}														\
	return pInstance;															\
	}();

#define RELEASE_INSTANCE(CLASSNAME)	[](){										\
	CLASSNAME*	pInstance = CLASSNAME::GetInstance();							\
	if(nullptr == pInstance) {													\
	char	szMessage[MAX_PATH] = "";											\
	strcpy_s(szMessage, typeid(CLASSNAME).name());								\
	strcat_s(szMessage, "is nullptr");											\
	MessageBoxA(0, szMessage, nullptr, MB_OK);}									\
	else {																		\
	pInstance->Release();}														\
	}();

#define BEGIN(NAMESPACE)  namespace NAMESPACE {
#define END				  }

#ifdef ENGINE_EXPORTS
#define ENGINE_DLL _declspec(dllexport) 
#else
#define ENGINE_DLL _declspec(dllimport) 
#endif

#define			MSG_BOX(_message)			MessageBox(NULL, TEXT(_message), L"System Message", MB_OK)
#define         MSG_BOX_CHECK(_message)      MessageBox(NULL, TEXT(_message), L"System Message", MB_YESNO)

#define NO_COPY(CLASSNAME)										\
		private:												\
		CLASSNAME(const CLASSNAME&) = delete;					\
		CLASSNAME& operator = (const CLASSNAME&) = delete;		

#define DECLARE_SINGLETON(CLASSNAME)							\
		NO_COPY(CLASSNAME)										\
		private:												\
		static CLASSNAME*	m_pInstance;						\
		public:													\
		static CLASSNAME*	GetInstance( void );				\
		static unsigned long DestroyInstance( void );			

#define IMPLEMENT_SINGLETON(CLASSNAME)							\
		CLASSNAME*	CLASSNAME::m_pInstance = NULL;				\
		CLASSNAME*	CLASSNAME::GetInstance( void )	{			\
			if(NULL == m_pInstance) {							\
				m_pInstance = new CLASSNAME;					\
			}													\
			return m_pInstance;									\
		}														\
		unsigned long CLASSNAME::DestroyInstance( void ) {		\
			unsigned long	dwRefCnt = 0;						\
			if(nullptr != m_pInstance)	{						\
				dwRefCnt = m_pInstance->Release();				\
				if(0 == dwRefCnt)								\
					m_pInstance = nullptr;						\
			}													\
			return dwRefCnt;									\
		}

#define TAG_LEN (256)

// debug모드에서만 체크할 경우 사용
#define Assert(expression) assert(expression)
#define AssertMsg(expression, message) Assert((expression) && (message))

// release에서도 체크해야할 경우 사용
#define NULL_CHECK(_ptr) if((_ptr) == 0){__debugbreak();}
#define FAILED_CHECK(_hr) if(((HRESULT)(_hr)) < 0){__debugbreak();}

// switch에서 break하지 않고 다음 case로 가야할 때 가독성을 위해서 사용
#define FALLTHROUGH 

// switch에서 default에 가면 안되는 경우 사용
#ifdef _DEBUG
# define NODEFAULT   Assert(0);
#else
# define NODEFAULT   __assume(0);
#endif

#define DECLARE_CREATE(ClassName)													\
static ClassName* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)		\
{																					\
 	auto		pInstance = new ClassName(pDevice, pContext);					\
	if (FAILED(pInstance->Initialize_Prototype()))									\
	{																				\
		MSG_BOX("Failed to Created : "#ClassName);									\
		Safe_Release(pInstance);													\
	}																				\
	return pInstance;																\
}

#define DECLARE_CLONE(ClassName)\
virtual CGameObject* Clone(void* pArg = nullptr) override\
{\
	auto		pInstance = new ClassName(*this);\
	if (FAILED(pInstance->Initialize(pArg)))\
	{\
		MSG_BOX("Failed to Cloned : "#ClassName);\
		Safe_Release(pInstance);\
	}\
	return pInstance;\
}
