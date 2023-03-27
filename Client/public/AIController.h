#pragma once
#include "Controller.h"

BEGIN(Client)

class AICommand
{
using CallbackType = std::function<void()>;
public:
	// AICommand(CallbackType&& callback, _float fDuration)
	// 	: m_Callback(std::move(callback))
	// 	, m_fDuration(fDuration)
	// {
	// }

	template<typename T, typename Ret, typename... Args>
	AICommand(const string& strName, _float fDuration, T* owner, Ret(T::* memFunc)(Args...), Args&&... args)
	{
		m_Callback = [owner, memFunc, args...]()
		{
			(owner->*memFunc)(args...);
		};
		m_fDuration = fDuration;
		m_strName = strName;
	}

	AICommand(const AICommand& other);
	AICommand(AICommand&& other) noexcept;
	AICommand& operator=(const AICommand& other);
	AICommand& operator=(AICommand&& other) noexcept;

	_bool Execute(_double TimeDelta)
	{
		m_Callback();
		m_fDuration -= (_float)TimeDelta;
		return m_fDuration <= 0.f;
	}
	_float GetDuration() const { return m_fDuration; }
	const string& GetName() const { return m_strName; }
	void SetFinish() { m_fDuration = 0.f; }

private:
	CallbackType m_Callback;
	_float m_fDuration = 0.f;
	string m_strName;
};

class CAIController abstract : public CController
{
public:
	enum DISTANCE { DIS_NEAR, DIS_MIDDLE, DIS_FAR, DIS_OUTSIDE, DIS_END };

protected:
	CAIController(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CAIController(const CAIController& rhs);
	virtual ~CAIController() override = default;

public:
	virtual void Tick(_double TimeDelta) override;
	virtual void AI_Tick(_double TimeDelta) = 0;
	virtual void Imgui_RenderProperty() override;
	void SetTarget(class CScarletCharacter* pTarget);

	_float GetTurnRemain() const { return m_fTurnRemain; }

	// duration = 0 �̸� �ѹ��� ������
	// OnStart, OnExit �ʿ��ϸ� �����
	template<typename T, typename Ret, typename... Args>
	void AddCommand(const string& strName, _float fDuration, Ret(T::*memFunc)(Args...), Args... args)
	{
		T* pController = dynamic_cast<T*>(this);
		m_Commands.push_back(AICommand(strName, fDuration, pController, memFunc, std::forward<Args>(args)...));
	}

	_bool IsCommandRunning();
	void ClearCommands();
	virtual void Invalidate() override;


	// �⺻ command �Լ�, Ŀ�ǵ� �Լ��� ���ڴ� ���۷��� ��� ����!
	void Move(EMoveAxis eAxis);
	void TurnToTarget(_float fSpeedRatio = 1.f);
	void Input(EHandleInput eInput);
	void Move_TurnToTarget(EMoveAxis eAxis, _float fSpeedRatio = 1.f);
	void Move_TurnToTargetStop(EMoveAxis eAxis, _float fSpeedRatio = 1.f);
	void TurnToTargetStop(_float fSpeedRatio = 1.f);
	void Wait();

	DISTANCE CurrentDistance() const { return m_eDistance; }
	EHandleInput GetAIInput() const { return m_eInput; }
	void SetActive(_bool bActive) { m_bActive = bActive; }

protected:
	class CScarletCharacter* m_pTarget = nullptr;
//	class CScarletCharacter* m_pUseObj = nullptr;
	list<AICommand> m_Commands;
	_float m_fTurnRemain = 0.f;

	// command ������ ����� TimeDelta
	_float m_fTimeDelta = 0.f;

	// IMGUI ���� �� �Ÿ� ���¸� ���ϱ� ���� enum
	DISTANCE m_eDistance = DIS_END;

	_float m_fTtoM_Distance = 0.f;

	//Move�� �÷��̾� �տ��� ���ߴ� �Ÿ�
	_float	m_fNearestTargetDist = 2.f;

	EHandleInput m_eInput = HANDLE_END;

	_bool m_bActive = true;
	
	//UI ������ ����
	_bool	m_bDetectTarget = false;
public:
	virtual void Free() override;
};

END
