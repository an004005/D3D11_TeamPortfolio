#pragma once

#include "Transform.h"

BEGIN(Engine)
/*
 * json���� ����, �ҷ����⸦ �����Ѵ�.
 * component ���� pArg�� json �Ǵ� nullptr �̴�.
 * json�� ��� SaveToJson���� ������ �׸��� ���� �ҷ����� �ǰ� �׸��� ������ ���� ����.
 * nullptr�� ��� Initialize���� ���� �ҷ����� �ʴ´�.
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
	// pArg�� �׻� Json* �̴�. �����ϱ� ��ư�, ���� �������� ������ ���ؼ� �����Ѵ�. ���� ��������
	virtual HRESULT Initialize(void* pArg);
	/*
	 * object�� �����ǰ�, Tick�� ������� ���� �ѹ��� ����Ǵ� �Լ�.
	 * Initialize�� ��ȣ�ۿ��� �ϴ� ��ü�� ���� ���� �� �ִ�.
	 * ���� ��� ��ü�� ������ �Ϸ�ǰ��� ���� ������ �ִ� �ٸ� ������Ʈ����� ��ȣ�ۿ��� �ʿ��� �� ���
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
	// �� ������Ʈ�� ������ �ִ� component�� Imgui_RenderProtpery�Լ��� �����ϴ� �Լ�.
	virtual void Imgui_RenderComponentProperties();

	// �� ������Ʈ���� ������ �����͸� imgui�� �ۼ��Ѵ�.
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
	/* ��ü���� ����ؾ� �� ������Ʈ���� �����Ѵ�. */
	map<wstring, class CComponent*>			m_Components;
	class CTransform*								m_pTransformCom = nullptr;
	_bool											m_bCloned = false;
	_bool											m_bDelete = false;
	_bool											m_bVisible = true;
	_bool											m_bRemote = false;

	_float											m_fCamDistance;
	// prototype���� �߰��� �� �������ش�.
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
