#pragma once
#include "ffscript.h"
#include <list>

#include "function/CdeclFunction2.hpp"
#include "function/CdeclFunction3.hpp"

class DFunction2;
namespace ffscript {

	class FunctionFactory;
	class ScriptCompiler;

	struct PredefinedOperator {
		const char* name;
		const char* functionParams;
		const char* returnType;
		DFunction2* nativeFunction;
	};

	struct FactoryItem
	{
		char* functionName;
		char* functionParams;
		FunctionFactory* factory;
		bool autoDelete;
	};

	class FFSCRIPT_API FunctionRegisterHelper
	{
		ScriptCompiler* _scriptCompiler;
	public:
		FunctionRegisterHelper(ScriptCompiler* scriptCompiler);
		virtual ~FunctionRegisterHelper();

		int registFunction(const std::string& name, const std::string& functionParams, FunctionFactory* factory, bool autoDelete = true);
		int registDynamicFunction(const std::string& name, FunctionFactory* factory, bool autoDelete = true);
		int registPredefinedOperators(const std::string& name, const std::string& functionParams, const std::string& returnType, DFunction2*);		
		void addFactory(FunctionFactory* factory, bool autoDelete = true);
		int registFunctions(FactoryItem* factories, int n);
		int registPredefinedOperators(PredefinedOperator* operators, int n);
		ScriptCompiler* getSriptCompiler() const;
	};


	template <class Rt, class... Types>
	DFunction2* createFunctionCdecl1(Rt(_cdecl *f)(Types...)) {
		return new CdeclFunction2<Rt, Types...>(f);
	}

	template <class Rt, class... Types>
	DFunction2* createFunctionCdecl2(Rt(_cdecl *f)(Types...)) {
		return new CCdelFunction3<Rt, Types...>(f);
	}

	template <class Rt, class... Types>
	FunctionFactory* createUserFunctionCdecl1(ScriptCompiler* scriptCompiler, const char* rt, Rt(_cdecl *f)(Types...)) {
		return new BasicFunctionFactory<sizeof...(Types)>(EXP_UNIT_ID_USER_FUNC, FUNCTION_PRIORITY_USER_FUNCTION, rt, createFunctionCdecl1<Rt, Types...>(f), scriptCompiler);
	}

	template <class ...Args>
	struct ArgumentFunctionCounter {
		static constexpr int count = sizeof...(Args);
	};
}