#pragma once
#include "Base.h"
#include "Animation.h"

BEGIN(Engine)
class CModel;
class CGameObject;
class CDoOnce;

struct ENGINE_DLL CAnimTransition : public CBase		// Ʈ������(����)
{
	string m_strName;									// Ʈ������ �̸�
	string m_strNextStateName;							// �Ѿ State �̸�
	_float m_fTransitionDuration = 0.f;					// ���� �ӵ�
	std::function<_bool()> m_Predicator = nullptr;		// Predicator�� True�� ��ȯ�ϸ� �����
	_uint m_iPriority = 0;								// �켱����

	// Ʈ������ ������
	CAnimTransition(const string& strName, const string& strNextStateName, _float fTransitionDuration,
		const std::function<_bool()>& Predicator = nullptr, _uint Priority = 0);

public:
	CAnimTransition(const string& szTransitionName, const string& szNextAnimName)
		:m_strName(szTransitionName), m_strNextStateName(szNextAnimName) {}
	virtual void Free() override {}
};

// ������Ʈ
struct ENGINE_DLL CAnimState : public CBase
{
	string m_strName;									// ������Ʈ �̸�
	vector<CAnimTransition*> m_Transitions;				// ���� ������Ʈ�� ������ Ʈ�������� ����
	CAnimation* m_Animation = nullptr;
	std::function<void(void)> m_StartEvent = nullptr;		// �ִϸ��̼��� ���۵� �� �߻��ϴ� �̺�Ʈ, ������ �����Ű�� ����
	std::function<_bool(void)> m_OptionalEvent = nullptr;	// �Ű������� ���� �������� �Ѿ �� �߻��ϴ� �̺�Ʈ, ������ �����Ű�� ����
	std::function<void(void)> m_FinishEvent = nullptr;		// �ִϸ��̼��� ����� �� �߻��ϴ� �̺�Ʈ, ������ �����Ű�� ����
	_bool m_bOptionalEvent = true;

public:
	CAnimState(const string& szStateName) :m_strName(szStateName) {};
	virtual void Free() override
	{
		for (auto pTransition : m_Transitions)
			Safe_Release(pTransition);
		m_Transitions.clear();
	}	
};

// ������Ʈ�ӽ�
class ENGINE_DLL CAnimationStateMachine : public CBase
{
protected:
	CAnimationStateMachine();
	virtual ~CAnimationStateMachine() = default;

public:
	virtual HRESULT	Initialize(class CASMBuilder* pBuilder);
	void Tick(_double TimeDelta, _bool bUpdateBone = true);													// ������Ʈ�ӽ� ������Ʈ
	void SetCurState(CAnimState* pCurState) { m_pCurState = pCurState; }									// ���� ���� ����
	void SetCurState(const string& stateName) { m_pCurState = m_mapStates[stateName]; }
	CAnimState* GetState(const string& stateName);															// Ư�� ���� ��������
	void Imgui_RenderState();

	CAnimState* GetCurState() { return m_pCurState; }					// ���� ���� ��������
	CAnimState* GetPreState() { return m_pPreState; }					// ���� ���� ��������
	_float GetTransitionDuration() { return m_fTransitionDuration; }	// Ʈ�������� Duration �������� (�� ����ð�)
	_float GetCurTransitionTime() { return m_fCurTransitionTime; }		// Ʈ�������� Time �������� (����ð�)
	_bool  isLerping() { return (1.f > (m_fCurTransitionTime / m_fTransitionDuration)) ? true : false ; }

private:
	string m_szCurStateName = "";

	string m_strName = typeid(*this).name();	// ������Ʈ�ӽ� �̸�
	CAnimState* m_pCurState = nullptr;			// ���� ����
	CAnimState* m_pPreState = nullptr;			// ���� ����

	_float m_fPreStatePlayAt = 0.f;

	_float m_fTransitionDuration = 0.f;			// Ʈ�������� Duration
	_float m_fCurTransitionTime = 0.f;			// ���� �������� �ִϸ��̼��� ����ð�
	_float m_fBlend = 0.f;
	_bool  m_bLerp = false;

	unordered_map<string, CAnimState*> m_mapStates;

#ifdef _DEBUG
	list<string> m_DebugQue;
	_uint m_iDebugQueSize = 15;
	bool m_bStoreHistory = false;

	string m_strFromState;
	string m_strTransition;
	string m_strToState;
#endif

public:
	static CAnimationStateMachine* Create(class CASMBuilder* pBuilder);
	virtual void Free() override
	{
		for (auto& PairState : m_mapStates)
			Safe_Release(PairState.second);
		m_mapStates.clear();
	}
};

class ENGINE_DLL CAnimationInstance abstract : public CBase
{
protected:
	CAnimationInstance() = default;
	virtual ~CAnimationInstance() = default;

public:
	virtual HRESULT Initialize(CModel* pModel, CGameObject* pTargetObject);
	virtual void UpdateTargetState(_double TimeDelta) = 0;
	virtual void Tick(_double TimeDelta){}
	virtual void Imgui_RenderState(){}

public:
	void PlayAnimationSocket(const string& strSocName, CAnimation* pAnimation, _float fPlayAt = 0.f);
	void StopAnimationSocket(const string& strSocName);
	CAnimation* GetSocketAnimation(const string& strSocName);
	

protected:
	_bool CheckFinishedAnimSocket();

protected:
	CModel* m_pModel = nullptr;
	CGameObject* m_pTargetObject = nullptr;

	unordered_map<string, list<CAnimation*>> m_mapAnimSocket;


public:
	virtual void Free() override;
};

class ENGINE_DLL CASMBuilder
{
public:
	CASMBuilder& InitState(const string& szNodeName)
	{
		Assert(m_pInitAnimStateName == "");
		m_pInitAnimStateName = szNodeName;
		return *this;
	}

	CASMBuilder& AddState(const string& szNodeName)
	{
		m_pBuildAnimTransition = nullptr;
		m_pBuildAnimState = new CAnimState(szNodeName);
		m_mapStates.emplace(szNodeName, m_pBuildAnimState);
		return *this;
	}

	CASMBuilder& SetAnimation(CAnimation& Anim)
	{
		Assert(m_pBuildAnimState != nullptr);
		Assert(m_pBuildAnimState->m_Animation == nullptr);
		m_pBuildAnimState->m_Animation = &Anim;
		return *this;
	}

	CASMBuilder& AddTransition(const string& szTransitionName, const string& szNextAnimName)
	{
		Assert(m_pBuildAnimState != nullptr);
		m_pBuildAnimTransition = new CAnimTransition(szTransitionName, szNextAnimName);
		m_pBuildAnimState->m_Transitions.push_back(m_pBuildAnimTransition);
		return *this;
	}

	CASMBuilder& Priority(_uint iPriority)
	{
		Assert(m_pBuildAnimTransition != nullptr);
		m_pBuildAnimTransition->m_iPriority = iPriority;
		return *this;
	}

	CASMBuilder& Predicator(std::function<_bool()> Predicator)
	{
		Assert(m_pBuildAnimTransition != nullptr);
		Assert(m_pBuildAnimTransition->m_Predicator == nullptr);
		m_pBuildAnimTransition->m_Predicator = Predicator;
		return *this;
	}

	CASMBuilder& StartEvent(std::function<void(void)> StartEvent)
	{
		Assert(m_pBuildAnimState != nullptr);
		Assert(m_pBuildAnimState->m_StartEvent == nullptr);
		m_pBuildAnimState->m_StartEvent = StartEvent;
		return *this;
	}

	CASMBuilder& FinishEvent(std::function<void(void)> FinishEvent)
	{
		Assert(m_pBuildAnimState != nullptr);
		Assert(m_pBuildAnimState->m_FinishEvent == nullptr);
		m_pBuildAnimState->m_FinishEvent = FinishEvent;
		return *this;
	}

	CASMBuilder& OptionalEvent(std::function<_bool(void)> OptionalEvent)
	{
		Assert(m_pBuildAnimState != nullptr);
		Assert(m_pBuildAnimState->m_OptionalEvent == nullptr);
		m_pBuildAnimState->m_OptionalEvent = OptionalEvent;
		return *this;
	}

	CASMBuilder& Duration(_float fDuration)
	{
		Assert(m_pBuildAnimTransition != nullptr);
		m_pBuildAnimTransition->m_fTransitionDuration = fDuration;
		return *this;
	}

	CAnimationStateMachine* Build()
	{
		Assert(m_pInitAnimStateName != "");

		for (auto& Pair : m_mapStates)
		{
			std::sort(Pair.second->m_Transitions.begin(), Pair.second->m_Transitions.end(),
				[](const CAnimTransition* left, const CAnimTransition* right)
			{
				return left->m_iPriority < right->m_iPriority;;
			});
		}

		return CAnimationStateMachine::Create(this);
	}

	unordered_map<string, CAnimState*>& GetAnimState()
	{
		return m_mapStates;
	}

	CAnimState& GetInitAnimState()
	{
		return *m_mapStates[m_pInitAnimStateName];
	}

	const string& GetInitAnimStateName() const
	{
		return m_pInitAnimStateName;
	}

private:
	unordered_map<string, CAnimState*> m_mapStates;			// ������ �߰��� �ִϸ��̼ǵ�
	string				m_pInitAnimStateName = "";			// �ֻ��� �ִϸ��̼� ������Ʈ �̸�
	CAnimState*			m_pBuildAnimState = nullptr;		// �ִϸ��̼� ������Ʈ
	CAnimTransition*	m_pBuildAnimTransition = nullptr;	// �ִϸ��̼� Ʈ������
};

END