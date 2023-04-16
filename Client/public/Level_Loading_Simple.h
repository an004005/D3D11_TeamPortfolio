#pragma once
#include "Client_Defines.h"
#include "LoadingLevel.h"
#include "GameObject.h"

BEGIN(Engine)
class CRenderer;
class CModel;
END

BEGIN(Client)

class CLoadingModel final : public CGameObject
{
private:
	CLoadingModel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CLoadingModel(const CLoadingModel& rhs);
	virtual ~CLoadingModel() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_double TimeDelta) override;
	virtual void Late_Tick(_double TimeDelta) override;
	virtual HRESULT Render() override;
	virtual void SaveToJson(OUT Json& json) override;
	virtual void LoadFromJson(const Json& json) override;
	virtual void Imgui_RenderProperty() override;

	void SetModel(const wstring& strTag);
	void SetPlayAnim(const string& strAnim);
	CModel* GetModel() { return m_pModel; }

private:
	CRenderer*				m_pRendererCom = nullptr;
	CModel*					m_pModel = nullptr;

	_float      m_fLerpTime = 0.f;
	_float      m_fDuration = 0.f;

private:
	string		m_strAnimName = "";
	wstring		m_wstrModelTag = L"";
	_int			m_iModelIndex = 0;
public:
	static CLoadingModel* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

class CLevel_Loading_Simple final : public CLoadingLevel
{
private:
	CLevel_Loading_Simple(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, std::function<CLevel*()>&& LevelCreator);
	virtual ~CLevel_Loading_Simple() = default;

public:
	virtual HRESULT Initialize() override;
	virtual void Tick(_double TimeDelta) override;
	virtual void Late_Tick(_double TimeDelta) override;
	virtual HRESULT Render() override;


private:
	class CEffectSystem* pRect = nullptr;


public:
	virtual void Free() override;
	template<typename LEVEL>
	static CLevel_Loading_Simple* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	{
		CLevel_Loading_Simple*		pInstance = new CLevel_Loading_Simple(pDevice, pContext, [pDevice, pContext]()
		{
			return LEVEL::Create(pDevice, pContext);
		});

		if (FAILED(pInstance->Initialize()))
		{
			MSG_BOX("Failed to Created : CLevel_Loading_Simple");
			Safe_Release(pInstance);
		}
		return pInstance;
	}

};

END