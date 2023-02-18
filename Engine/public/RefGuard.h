#pragma once
#include <utility>

/*
 * RAII ����(Resource Acquisition Is Initialization)
 * �����ڰ� ���� resource ������ ���־�� �ϴ� ���� leak �� �����ϱ� ���� �߿��� �������
 * �ش� ���ҽ��� ��� scope�� ���� ��쿡 �ڵ����� �����Ͽ� ȹ���� �ڿ��� �������� ������ ������ �ڵ带 �ۼ��� �� �ִ�.
 *
 * CBase �����͸� ������ �� AddRef, ������ ������ ReleaseRef�� RefCount�� �����ؾ��Ѵ�.
 * �̶� ���α׷����� �Ǽ��� ���۷���ī��Ʈ�� �ø��� �ʰų�, ������ ������ ������ �߻��Ѵ�.
 * ���� RAII������ ������ ���۷��� ī��Ʈ�� �ڵ����� �������ִ� Ŭ������ CRefGuard�� ����Ѵ�.
 * ���۷����� �����ε����� ���� ������ ����ϴ°� ó�� ����ϸ� �ȴ�.
 *
 * ex)
 * �����
 * CGameInstance* pGameInstance = CGameInstance::GatInstance(); // CBaseŬ���� ����
 * Safe_AddRef(pGameInstance); => ���� ���۷��� ī��Ʈ ����
 * ~~~ �ڵ� ~~~
 * Safe_Release(pGameInstance); => ���� ���۷��� ī��Ʈ ����
 * ~~~ ������ Ż�� ~~~
 *
 * �����
 * CRefGuard<CGameInstance> pGameInstance = CGameInstance::GatInstance(); // CBaseŬ���� ����
 * => CRefGuard�� �����ڿ��� CGameInstance�� ���۷��� ī��Ʈ ����
 * ~~~ �ڵ� ~~~
 * ~~~ ������ Ż�� ~~~
 * => CRefGuard�� �Ҹ��ڰ� ȣ��Ǿ� ���۷��� ī��Ʈ ����
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

		// ���� ����, ���� ����
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
