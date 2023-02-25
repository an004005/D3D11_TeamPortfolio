#pragma once

#include "Base.h"

/* 우리가 앞으로 사용할 컴포넌트(텍스쳐, 트랜스폼, 정점,인덱스버퍼, 모델) 들의 부모가 되는 클래스이다. */
/* 클래스 다형성을 이용하여 하나의 템플릿컨테이너안에 담아두도록 저으이한다. */

BEGIN(Engine)

class ENGINE_DLL CComponent abstract : public CBase
{
protected:
	CComponent(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CComponent(const CComponent& rhs);
	virtual ~CComponent() = default;

public:
	virtual HRESULT Initialize_Prototype();
	// pArg는 항상 Json* 이다. 변경하기 어렵고, 기존 수업과의 통일을 위해서 유지한다. 추후 수정요함
	virtual HRESULT Initialize(void* pArg);

#ifdef _DEBUG
public:
	virtual HRESULT Render() { return S_OK;  };
#endif // 
	// add prototype 할 때 복사된다.
	void SetPrototypeTag(const _tchar* pPrototypeTag) { m_pPrototypeTag = pPrototypeTag; }
	const _tchar* GetPrototypeTag() const { return m_pPrototypeTag; }
	virtual void SaveToJson(OUT Json& json){}
	virtual void LoadFromJson(const Json& json) {}

	// 보통 GameObject에서 clone할 때 지정해준다.
	void SetOwner(class CGameObject* pOwner) { m_pOwner = pOwner; }
	class CGameObject* TryGetOwner();
	_bool IsCloned() const { return m_isCloned; }

public: /* imgui */
	// 이 컴포넌트에서 보여줄 데이터를 imgui로 작성한다.
	virtual void Imgui_RenderProperty(){}

public:
	virtual CComponent* Clone(void* pArg = nullptr) = 0;
	virtual void Free() override;

protected:
	ID3D11Device*				m_pDevice = nullptr;
	ID3D11DeviceContext*		m_pContext = nullptr;
	_bool						m_isCloned = false;
	const _tchar* m_pPrototypeTag = nullptr;

private:
	// 순환참조문제로 reference count는 올리지 않는다. 주의
	class CGameObject* m_pOwner = nullptr;
};

END