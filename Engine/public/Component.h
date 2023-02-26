#pragma once

#include "Base.h"

/* �츮�� ������ ����� ������Ʈ(�ؽ���, Ʈ������, ����,�ε�������, ��) ���� �θ� �Ǵ� Ŭ�����̴�. */
/* Ŭ���� �������� �̿��Ͽ� �ϳ��� ���ø������̳ʾȿ� ��Ƶε��� �������Ѵ�. */

BEGIN(Engine)

class ENGINE_DLL CComponent abstract : public CBase
{
protected:
	CComponent(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CComponent(const CComponent& rhs);
	virtual ~CComponent() = default;

public:
	virtual HRESULT Initialize_Prototype();
	// pArg�� �׻� Json* �̴�. �����ϱ� ��ư�, ���� �������� ������ ���ؼ� �����Ѵ�. ���� ��������
	virtual HRESULT Initialize(void* pArg);

#ifdef _DEBUG
public:
	virtual HRESULT Render() { return S_OK;  };
#endif // 
	// add prototype �� �� ����ȴ�.
	void SetPrototypeTag(const _tchar* pPrototypeTag) { m_pPrototypeTag = pPrototypeTag; }
	const _tchar* GetPrototypeTag() const { return m_pPrototypeTag; }
	virtual void SaveToJson(OUT Json& json){}
	virtual void LoadFromJson(const Json& json) {}

	// ���� GameObject���� clone�� �� �������ش�.
	void SetOwner(class CGameObject* pOwner) { m_pOwner = pOwner; }
	class CGameObject* TryGetOwner();
	_bool IsCloned() const { return m_isCloned; }

public: /* imgui */
	// �� ������Ʈ���� ������ �����͸� imgui�� �ۼ��Ѵ�.
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
	// ��ȯ���������� reference count�� �ø��� �ʴ´�. ����
	class CGameObject* m_pOwner = nullptr;
};

END