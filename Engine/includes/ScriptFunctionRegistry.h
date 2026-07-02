#pragma once

#include <functional>
#include <string>
#include <vector>

namespace buki
{
	struct StaticVoidFunctionInfo
	{
		std::string scriptTypeName;
		std::string functionName;
		std::function<void()> callback;
	};

	class ScriptFunctionRegistry
	{
	public:
		static ScriptFunctionRegistry& Get();

		void RegisterStaticVoidFunction(
			const std::string& scriptTypeName,
			const std::string& functionName,
			const std::function<void()>& callback);

		const StaticVoidFunctionInfo* FindStaticVoidFunction(
			const std::string& scriptTypeName,
			const std::string& functionName) const;

		std::vector<StaticVoidFunctionInfo> GetAllStaticVoidFunctions() const;
		std::vector<StaticVoidFunctionInfo> GetFunctionsForScript(const std::string& scriptTypeName) const;

		std::vector<std::string> GetFunctionsScriptsName() const;
		std::vector<std::string> GetFunctionsForScriptName(const std::string& scriptTypeName) const;

		void Clear();

	private:
		std::vector<StaticVoidFunctionInfo> m_Functions;
	};
}
