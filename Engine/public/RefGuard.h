#pragma once
#include <utility>

/*
 * RAII 패턴(Resource Acquisition Is Initialization)
 * 개발자가 직접 resource 관리를 해주어야 하는 언어에서 leak 을 방지하기 위한 중요한 기법으로
 * 해당 리소스의 사용 scope이 끝날 경우에 자동으로 해제하여 획득한 자원이 해제됨을 보장해 안전한 코드를 작성할 수 있다.
 *
 * CBase 포인터를 참조할 때 AddRef, 참조가 끝나면 ReleaseRef로 RefCount를 관리해야한다.
 * 이때 프로그래머의 실수로 레퍼런스카운트를 올리지 않거나, 내리지 않으면 문제가 발생한다.
 * 따라서 RAII패턴을 구현한 레퍼런스 카운트를 자동으로 관리해주는 클래스인 CRefGuard를 사용한다.
 * 오퍼레이터 오버로딩으로 기존 포인터 사용하는것 처럼 사용하면 된다.
 *
 * ex)
 * 사용전
 * CGameInstance* pGameInstance = CGameInstance::GatInstance(); // CBase클래스 참조
 * Safe_AddRef(pGameInstance); => 직접 레퍼런스 카운트 증가
 * ~~~ 코드 ~~~
 * Safe_Release(pGameInstance); => 직접 레퍼런스 카운트 감소
 * ~~~ 스코프 탈출 ~~~
 *
 * 사용후
 * CRefGuard<CGameInstance> pGameInstance = CGameInstance::GatInstance(); // CBase클래스 참조
 * => CRefGuard의 생성자에서 CGameInstance의 레퍼런스 카운트 증가
 * ~~~ 코드 ~~~
 * ~~~ 스코프 탈출 ~~~
 * => CRefGuard의 소멸자가 호출되어 레퍼런스 카운트 감소
 *
 */
namespace Engine
{
	template <typename T>
	class ENGINE_DLL CRefGuard final
	{
	public:
		CRefGuard() = default;
		CRefGuard(CRefGuard<T>& RefGuard);
		CRefGuard(T*&& pBase);
		~CRefGuard();

		// 동적 생성, 삭제 방지
		void* operator new(size_t sz) = delete;
		void* operator new(size_t sz, int id) = delete;
		void operator delete(void* p) = delete;

		T* operator =(const CRefGuard<T>& rhs) = delete;

		T* operator ->() { return m_pBase; }
		T& operator*() { return *m_pBase; }

		T* get() { return m_pBase; }

	private:
		T* m_pBase;
	};

	// operator ==
	template<typename T>
	bool operator== (const CRefGuard<T>& left, const CRefGuard<T>& right)
	{
		return left.get() == right.get();
	}

	template<typename T>
	bool operator== (const CRefGuard<T>& left, nullptr_t)
	{
		return left.get() == nullptr;
	}
	template<typename T>
	bool operator== (nullptr_t, const CRefGuard<T>& right)
	{
		return nullptr == right.get();
	}

	template<typename T>
	bool operator== (void* left, const CRefGuard<T>& right)
	{
		return left == right.get();
	}
	template<typename T>
	bool operator== (const CRefGuard<T>& left, void* right)
	{
		return left.get() == right;
	}

	// operator !=
	template<typename T>
	bool operator!= (const CRefGuard<T>& left, const CRefGuard<T>& right)
	{
		return left.get() != right.get();
	}
	template<typename T>
	bool operator!= (const CRefGuard<T>& left, nullptr_t)
	{
		return left.get() != nullptr;
	}
	template<typename T>
	bool operator!= (nullptr_t, const CRefGuard<T>& right)
	{
		return nullptr != right.get();
	}
	template<typename T>
	bool operator!= (void* left, const CRefGuard<T>& right)
	{
		return left != right.get();
	}
	template<typename T>
	bool operator!= (const CRefGuard<T>& left, void* right)
	{
		return left.get() != right;
	}
}
