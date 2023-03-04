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

	// duration = 0 이면 한번만 실행함
	// OnStart, OnExit 필요하면 만들기
	template<typename T, typename Ret, typename... Args>
	void AddCommand(const string& strName, _float fDuration, Ret(T::*memFunc)(Args...), Args... args)
	{
		T* pController = dynamic_cast<T*>(this);
		m_Commands.push_back(AICommand(strName, fDuration, pController, memFunc, std::forward<Args>(args)...));
	}

	_bool IsCommandRunning();
	void ClearCommands();


	// 기본 command 함수, 커맨드 함수의 인자는 레퍼런스 사용 금지!
	void Move(EMoveAxis eAxis);
	void TurnToTarget(_float fSpeedRatio = 1.f);
	void Input(EHandleInput eInput);
	void Move_TurnToTarget(EMoveAxis eAxis, _float fSpeedRatio = 1.f);

protected:
	class CScarletCharacter* m_pTarget = nullptr;
	list<AICommand> m_Commands;
	_float m_fTurnRemain = 0.f;

	// command 내에서 사용할 TimeDelta
	_float m_fTimeDelta = 0.f;

public:
	virtual void Free() override;
};

END
