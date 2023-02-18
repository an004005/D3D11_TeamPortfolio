#pragma once
#include "Base.h"

BEGIN(Engine)
class CAnimation;

class CAnimationManager : public CBase
{
	DECLARE_SINGLETON(CAnimationManager)
private:
	CAnimationManager() = default;
	virtual ~CAnimationManager() = default;

public:
	HRESULT LoadAnimation(const char* pAnimPath);
	HRESULT LoadAnimations(const char* pAnimDirPath);
	CAnimation* GetAnimation(const string& strAnimName);
	CAnimation* GetAnimation(const char* pAnimName);

private:
	unordered_map<string, CAnimation*> m_mapAnims;

public:
	virtual void Free() override;
};

END