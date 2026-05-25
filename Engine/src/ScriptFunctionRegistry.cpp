#include "ScriptFunctionRegistry.h"

#include <algorithm>

namespace buki
{
	ScriptFunctionRegistry& ScriptFunctionRegistry::Get()
	{
		static ScriptFunctionRegistry instance;
		return instance;
	}

	void ScriptFunctionRegistry::RegisterStaticVoidFunction(
		const std::string& scriptTypeName,
		const std::string& functionName,
		const std::function<void()>& callback)
	{
		if (scriptTypeName.empty() || functionName.empty() || !callback)
		{
			return;
		}

		for (StaticVoidFunctionInfo& existing : m_Functions)
		{
			if (existing.scriptTypeName == scriptTypeName &&
				existing.functionName == functionName)
			{
				existing.callback = callback;
				return;
			}
		}

		StaticVoidFunctionInfo info;
		info.scriptTypeName = scriptTypeName;
		info.functionName = functionName;
		info.callback = callback;
		m_Functions.push_back(std::move(info));

		std::sort(m_Functions.begin(), m_Functions.end(),
			[](const StaticVoidFunctionInfo& a, const StaticVoidFunctionInfo& b)
			{
				if (a.scriptTypeName != b.scriptTypeName)
				{
					return a.scriptTypeName < b.scriptTypeName;
				}
				return a.functionName < b.functionName;
			});
	}

	const StaticVoidFunctionInfo* ScriptFunctionRegistry::FindStaticVoidFunction(
		const std::string& scriptTypeName,
		const std::string& functionName) const
	{
		for (const StaticVoidFunctionInfo& info : m_Functions)
		{
			if (info.scriptTypeName == scriptTypeName &&
				info.functionName == functionName)
			{
				return &info;
			}
		}

		return nullptr;
	}

	std::vector<StaticVoidFunctionInfo> ScriptFunctionRegistry::GetAllStaticVoidFunctions() const
	{
		return m_Functions;
	}

	std::vector<StaticVoidFunctionInfo> ScriptFunctionRegistry::GetFunctionsForScript(
		const std::string& scriptTypeName) const
	{
		std::vector<StaticVoidFunctionInfo> result;

		for (const StaticVoidFunctionInfo& info : m_Functions)
		{
			if (info.scriptTypeName == scriptTypeName)
			{
				result.push_back(info);
			}
		}

		return result;
	}

	void ScriptFunctionRegistry::Clear()
	{
		m_Functions.clear();
	}
}