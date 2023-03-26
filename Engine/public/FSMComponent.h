#pragma once
#include "Base.h"

BEGIN(Engine)

/*-------------------
 *    FSM_TRANSITION
 --------------------*/
// FSM�� Transition(���º�������)�� �����ϴ� ����ü
typedef struct ENGINE_DLL tagFSM_Transition : public CBase
{
	// �� ���� ������ �̸�
	const char* pTransitionName = nullptr;
	// ���� �޼��� �̵��� ���� �̸�
	const char* pNextStateName = nullptr;
	// �� ���� ������ ����, �������� ���� ����ȴ�.
	_uint iPriority = 0;
	// �� ���� ���� Ȯ�� �Լ�
	std::function<_bool()> Predicator = nullptr;

	tagFSM_Transition() = delete;
	tagFSM_Transition(const char* pTransitionName, const char* pNextStateName)
		: pTransitionName(pTransitionName), pNextStateName(pNextStateName){}

	virtual void Free() override{}

} FSM_TRANSITION;

/*-------------------
 *    FSM_STATE
 --------------------*/
// FSM�� ���¸� �����ϴ� ����ü
typedef struct ENGINE_DLL tagFSM_State : public CBase
{
	// ���� �̸�
	const char* pStateName;
	// �� ���¸� ������ �� �����ϴ� �Լ�
	std::function<void()> OnStart = nullptr;
	// �� ���¸� �ݺ��� �� �����ϴ� �Լ�
	std::function<void(_double)> Tick = nullptr;
	// �� ���¸� ������ �����ϴ� �Լ�
	std::function<void()> OnExit = nullptr;

	// �� ���¿��� �ٸ� ���·� ���̵� ���ǵ�
	vector<FSM_TRANSITION*> Transitions;

	virtual void Free() override
	{
		for (auto pTransition : Transitions)
			Safe_Release(pTransition);
		Transitions.clear();
	}

	tagFSM_State(const char* pStateName) : pStateName(pStateName){}

} FSM_STATE;

/*-------------------
 *    CFSMComponent
 --------------------*/
// ���� component�� �����ʿ� ���ٰ� �����Ǽ� CBase�� ������
class ENGINE_DLL CFSMComponent : public CBase
{
protected:
	CFSMComponent();
	virtual ~CFSMComponent() = default;

public:
	void Tick(_double TimeDelta);

	// CFSMComponentBuilder�� ���� FMS�� ����� �̸� pArg�� �����ش�.
	virtual HRESULT Initialize(class CFSMComponentBuilder* pBuilder);
	virtual void Imgui_RenderProperty();

	const char* GetCurStateName() const { return m_pCurStateName; }
	void SetState(const char* pStateName) { m_pCurStateName = pStateName; }

private:
	void StateHistoryUpdate(const char* pNextStateName);

private:
	cmap<FSM_STATE*> m_mapState;
	const char* m_pCurStateName; // set init node at initialize

	// for debug
	const char* m_pLastTransitionName = nullptr;
	list<string> m_strDebugQue;
	_uint m_iDebugQueSize = 10;
	bool m_bStoreHistory = false;
	// for debug

public:
	static CFSMComponent* Create(class CFSMComponentBuilder* pBuilder);
	virtual void Free() override;
};

/*-------------------
 *    CFSMComponentBuilder
 --------------------*/
/* ����
 * 	m_pAbilityC_FSM = CFSMComponentBuilder()
		.InitState("Idle")
		.AddState("Idle")
			.AddTransition("Idle to Intro", "Intro")
				.Predicator([this]()
				{
					return !m_bAbilityC && !m_bAbility && m_pController->KeyDown(CController::C);
				})

		.AddState("Intro")
			.OnStart([this]()
			{
				CWeapon* pCurrentWeapon = m_WeaponSocket[static_cast<_uint>(m_eCurWeaponType)];
				pCurrentWeapon->SetVisible(false);
				m_bAbilityC = true;
				m_bAbility = true;
				pCurrentWeapon->PlayAnimSocket_FPS("AnimSocket_UpperBody", m_pFPS_AbilityC_Intro);
				pCurrentWeapon->PlayAnimSocket_TPS("AnimSocket_UpperBody", m_pTPS_AbilityC_Intro);
			})
			.AddTransition("Intro to Loop", "Loop")
				.Predicator([this]()
				{
					return m_pFPS_AbilityC_Intro->GetPlayRatio() > 0.9f;
				})

		.AddState("Loop")
			.OnStart([this]
			{
				CWeapon* pCurrentWeapon = m_WeaponSocket[static_cast<_uint>(m_eCurWeaponType)];
				pCurrentWeapon->PlayAnimSocket_FPS("AnimSocket_UpperBody", m_pFPS_AbilityC_Loop);
				pCurrentWeapon->PlayAnimSocket_TPS("AnimSocket_UpperBody", m_pTPS_AbilityC_Loop);
			})
			.AddTransition("Loop to Outro", "Outro")
				.Predicator([this] {return m_pController->KeyUp(CController::C); })

		.AddState("Outro")
			.OnStart([this]
			{
				CWeapon* pCurrentWeapon = m_WeaponSocket[static_cast<_uint>(m_eCurWeaponType)];
				pCurrentWeapon->PlayAnimSocket_FPS("AnimSocket_UpperBody", m_pFPS_AbilityC_Outro);
				pCurrentWeapon->PlayAnimSocket_TPS("AnimSocket_UpperBody", m_pTPS_AbilityC_Outro);
			})
			.OnExit([this]
			{
				CWeapon* pCurrentWeapon = m_WeaponSocket[static_cast<_uint>(m_eCurWeaponType)];
				m_bAbilityC = false;
				m_bAbility = false;
				pCurrentWeapon->SetVisible(true);
				pCurrentWeapon->EquipFast();
			})
			.AddTransition("Outro to Idle", "Idle")
				.Predicator([this]
				{
					return m_pFPS_AbilityC_Outro->GetPlayRatio() > 0.9f;
				})
		.Build();
 */
class ENGINE_DLL CFSMComponentBuilder
{
public:
	CFSMComponentBuilder& InitState(const char* szNodeName)
	{
		Assert(m_pInitStateName == nullptr);
		m_pInitStateName = szNodeName;
		return *this;
	}
	
	CFSMComponentBuilder& AddState(const char* pNodeName)
	{
		Assert(m_mapStates.find(pNodeName) == m_mapStates.end());
		m_pBuildTransition = nullptr;
		m_pBuildState = new FSM_STATE(pNodeName);
		m_mapStates.emplace(pNodeName, m_pBuildState);
		return *this;
	}
	CFSMComponentBuilder& OnStart(const std::function<void()>& onStart)
	{
		Assert(m_pBuildState != nullptr);
		Assert(m_pBuildState->OnStart == nullptr);
		m_pBuildState->OnStart = onStart;
		return *this;
	}
	
	template<typename T>
	CFSMComponentBuilder& OnStart(T* obj, void (T::*memFunc)())
	{
		Assert(m_pBuildState != nullptr);
		Assert(m_pBuildState->OnStart == nullptr);
		m_pBuildState->OnStart = [obj, memFunc]()
		{
			(obj->*memFunc)();
		};
		return *this;
	}
	
	CFSMComponentBuilder& Tick(const std::function<void(_double)>& tick)
	{
		Assert(m_pBuildState != nullptr);
		Assert(m_pBuildState->Tick == nullptr);
		m_pBuildState->Tick = tick;
		return *this;
	}
	
	template<typename T>
	CFSMComponentBuilder& Tick(T* obj, void (T::*memFunc)(_double))
	{
		Assert(m_pBuildState != nullptr);
		Assert(m_pBuildState->Tick == nullptr);
		m_pBuildState->Tick = [obj, memFunc](_double TimeDelta)
		{
			(obj->*memFunc)(TimeDelta);
		};
		return *this;
	}
	
	CFSMComponentBuilder& OnExit(const std::function<void()>& onExit)
	{
		Assert(m_pBuildState != nullptr);
		Assert(m_pBuildState->OnExit == nullptr);
		m_pBuildState->OnExit = onExit;
		return *this;
	}
	
	template<typename T>
	CFSMComponentBuilder& OnExit(T* obj, void (T::*memFunc)())
	{
		Assert(m_pBuildState != nullptr);
		Assert(m_pBuildState->OnExit == nullptr);
		m_pBuildState->OnExit = [obj, memFunc]()
		{
			(obj->*memFunc)();
		};
		return *this;
	}

	CFSMComponentBuilder& AddTransition(const char* pTransitionName, const char* pNextStateName)
	{
		Assert(m_pBuildState != nullptr);
		m_pBuildTransition = new FSM_TRANSITION(pTransitionName, pNextStateName);
		m_pBuildState->Transitions.push_back(m_pBuildTransition);
		return *this;
	}

	CFSMComponentBuilder& Priority(_uint iPriority)
	{
		Assert(m_pBuildTransition != nullptr);
		m_pBuildTransition->iPriority = iPriority;
		return *this;
	}

	CFSMComponentBuilder& Predicator(std::function<_bool()> Predicator)
	{
		Assert(m_pBuildTransition != nullptr);
		Assert(m_pBuildTransition->Predicator == nullptr);
		m_pBuildTransition->Predicator = Predicator;
		return *this;
	}

	template<typename T>
	CFSMComponentBuilder& Predicator(T* obj, bool (T::*memFunc)())
	{
		Assert(m_pBuildTransition != nullptr);
		Assert(m_pBuildTransition->Predicator == nullptr);
		m_pBuildTransition->Predicator = [obj, memFunc]()
		{
			return (obj->*memFunc)();
		};
		return *this;
	}
	
	CFSMComponent* Build()
	{
		assert(m_pInitStateName != nullptr);

		for (auto& NodePair : m_mapStates)
		{
			std::sort(NodePair.second->Transitions.begin(), NodePair.second->Transitions.end(), 
				[](const FSM_TRANSITION* left, const FSM_TRANSITION* right)
			{
				return left->iPriority < right->iPriority; // ��������
			});
		}
	
		return CFSMComponent::Create(this);
	}
	
	cmap<FSM_STATE*>& GetStates()
	{
		return m_mapStates;
	}
	
	const char* GetInitStateName() const
	{
		return m_pInitStateName;
	}


private:
	cmap<FSM_STATE*> m_mapStates;
	const char* m_pInitStateName = nullptr;

	FSM_STATE* m_pBuildState = nullptr;
	FSM_TRANSITION* m_pBuildTransition = nullptr;
};

END