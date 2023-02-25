#pragma once

#include "Transform.h"

BEGIN(Engine)
/*
 * json으로 저장, 불러오기를 지원한다.
 * component 포함 pArg는 json 또는 nullptr 이다.
 * json일 경우 SaveToJson으로 저장한 항목을 전부 불러오면 되고 항목이 빠지는 경우는 없다.
 * nullptr일 경우 Initialize에서 값을 불러오지 않는다.
 */
class ENGINE_DLL CGameObject abstract : public CBase
{
protected:
	CGameObject(ID3D11Device*	pDevice, ID3D11DeviceContext* pContext);
	CGameObject(const CGameObject& rhs);
	virtual ~CGameObject() override;

public:
	static const _tchar*			m_pTransformComTag;

public:
	virtual HRESULT Initialize_Prototype();
	// pArg는 항상 Json* 이다. 변경하기 어렵고, 기존 수업과의 통일을 위해서 유지한다. 추후 수정요함
	virtual HRESULT Initialize(void* pArg);
	/*
	 * object가 생성되고, Tick이 실행되지 전에 한번만 실행되는 함수.
	 * Initialize는 상호작용을 하는 객체가 아직 없을 수 있다.
	 * 따라서 모든 객체가 생성이 완료되고나서 같은 레벨에 있는 다른 오브젝트들과의 상호작용이 필요할 때 사용
	 */
	virtual void BeginTick(){}
	virtual void Tick(_double TimeDelta);
	virtual void Late_Tick(_double TimeDelta);
	virtual void AfterPhysX(){}
	virtual HRESULT Render();
	virtual HRESULT Render_ShadowDepth() { return S_OK; }

	virtual void SaveToJson(OUT Json& json);
	virtual void LoadFromJson(const Json& json);

public: /* imgui */
	// 이 오브젝트가 가지고 있는 component의 Imgui_RenderProtpery함수를 실행하는 함수.
	virtual void Imgui_RenderComponentProperties();

	// 이 오브젝트에서 보여줄 데이터를 imgui로 작성한다.
	virtual void Imgui_RenderProperty();

public:
	void Compute_CamDistance();

public:
	CTransform* GetTransform() const { return m_pTransformCom; }

	_bool IsDeleted() const { return m_bDelete; }
	_bool IsVisible() const { return m_bVisible; }
	_bool IsRemote() const { return m_bRemote; }
	_float GetCamDistance() const { return m_fCamDistance; }

	template<typename T>
	T* GetComponent(const _tchar* pComponentTag);
	template<typename T>
	T* GetComponentChecked(const _tchar* pComponentTag);

	const wstring& GetPrototypeTag() const { return m_strPrototypeTag; }
	void SetPrototypeTag(const wstring& strPrototypeTag) {
		m_strPrototypeTag = strPrototypeTag;
	}

	const string& GetObjectTag() const { return m_strObjectTag; }


	void SetVisible(_bool bVisible) { m_bVisible = bVisible; }
	virtual void SetDelete() { m_bDelete = true; }

protected:
	ID3D11Device*			m_pDevice = nullptr;
	ID3D11DeviceContext*	m_pContext = nullptr;

protected:
	/* 객체들이 사용해야 할 컴포넌트들을 보관한다. */
	map<wstring, class CComponent*>			m_Components;
	class CTransform*								m_pTransformCom = nullptr;
	_bool											m_bCloned = false;
	_bool											m_bDelete = false;
	_bool											m_bVisible = true;
	_bool											m_bRemote = false;

	_float											m_fCamDistance;
	// prototype으로 추가될 때 지정해준다.
	wstring											m_strPrototypeTag;
	string											m_strObjectTag;

protected:
	HRESULT Add_Component(_uint iLevelIndex, const _tchar* pPrototypeTag, const _tchar* pComponentTag, class CComponent** ppOut, void* pArg = nullptr);
	HRESULT Delete_Component(const _tchar* pComponentTag);
	class CComponent* Find_Component(const _tchar* pComponentTag);

public:	
	virtual CGameObject* Clone(void* pArg = nullptr) = 0;
	virtual void Free() override;
};


template <typename T>
T* CGameObject::GetComponent(const _tchar* pComponentTag)
{
	CComponent* pComponent = Find_Component(pComponentTag);
	return dynamic_cast<T*>(pComponent);
}

template <typename T>
T* CGameObject::GetComponentChecked(const _tchar* pComponentTag)
{
	T* pComponent = GetComponent<T>(pComponentTag);
	NULL_CHECK(pComponent);
	return pComponent;
}

END
