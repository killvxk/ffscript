/******************************************************************
* File:        CodeUpdater.h
* Description: declare CodeUpdater class. A class used to store
*              update tasks. Behavior of update task are various,
*              created in compiling time, and run in extraction code
*              time.
* Author:      Vincent Pham
*
* (C) Copyright 2018, The ffscript project, All rights reserved.
** Distributed under the MIT License (http://opensource.org/licenses/MIT)
**
*
**********************************************************************/

#pragma once
#include <list>
#include <memory>
#include <map>
#include "ffscript.h"

namespace ffscript {

	class Executor;
	class CommandUnit;
	class IfCommandBuilder;
	class CallScriptFuntion;
	class CallScriptFuntion2;
	class ScriptScope;
	class Program;
	class CallCreateLambda;
	
	class CodeUpdater
	{
		std::list<DFunctionRef> _updateLaterList;
		std::map<CommandUnit*, Executor*> _commandExecutorMap;
		ScriptScope* _ownerScope;
	public:
		CodeUpdater(ScriptScope* ownerScope);
		virtual ~CodeUpdater();
		void addUpdateLaterTask(DFunction* task);
		void runUpdate();
		void clear();
		void setUpdateInfo(CommandUnit* commandUnit, Executor* executor);
		void saveUpdateInfo(CommandUnit* commandUnit, Executor* executor);
		Executor* findUpdateInfo(CommandUnit* commandUnit) const;

		static CodeUpdater* getInstance(const ScriptScope* scope);

	public:
		static void updateScriptFunction(Program* program, CallScriptFuntion* command, int functionId);
		static void updateScriptFunction(Program* program, CallScriptFuntion2* command, int functionId);
		static void updateScriptFunctionObject(Program* program, RuntimeFunctionInfo* runtimeInfo, int functionId);
		static void updateLamdaScriptFunctionObject(Program* program, CallCreateLambda* createLambdaInst, int functionId);
	};
}
