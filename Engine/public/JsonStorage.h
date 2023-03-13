#pragma once
#include "Base.h"
#include "JsonLib.h"

BEGIN(Engine)

class ENGINE_DLL CJsonStorage final : public CBase
{
	DECLARE_SINGLETON(CJsonStorage)
private:
	CJsonStorage();
	virtual ~CJsonStorage() = default;

public:
	const Json& FindJson(const string& strJsonPath);
	const Json& FindOrLoadJson(const string& strJsonPath);
	void LoadJson(const string& strJsonPath);

	const Json& LoadJson_ForWork(const string& strJsonPath);

	void SaveJson(const string& strJsonPath);
	void UpdateJson(const string& strJsonPath, const Json& json);

private:
	unordered_map<string, Json> m_mapJson;
	Json m_EmptyJson = Json::object();
	mutex m_mtx;

public:
	virtual void Free() override;

};

END