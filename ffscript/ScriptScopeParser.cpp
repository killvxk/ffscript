#include "stdafx.h"
#include "ScriptScope.h"
#include "ScriptCompiler.h"
#include "Internal.h"
#include "Utils.h"
#include "ScopedCompilingScope.h"
#include "Program.h"

namespace ffscript {
	const wchar_t* ScriptScope::parseType(const wchar_t* text, const wchar_t* end, ScriptType& type) {
		const wchar_t* c;
		std::string token2;

		ScriptCompiler* scriptCompiler = getCompiler();


		c = scriptCompiler->readType(text, end, type);
		if (type.isUnkownType()) {
			scriptCompiler->setErrorText("unknow data type '" + type.sType() + "'");
			return nullptr;
		}


		return c;
	}

	ExecutableUnitRef ScriptScope::chooseCandidate(const CandidateCollectionRef& candidates, const ScriptType& expectedType) {
		auto scriptCompiler = getCompiler();
		for (auto it = candidates->begin(); it != candidates->end(); ++it) {
			auto& candidateUnitRef = *it;
			if ((*it)->getReturnType() == expectedType) {
				return candidateUnitRef;
			}
		}

		ScriptType refVoidType;
		refVoidType.updateType(scriptCompiler, "void");
		refVoidType = refVoidType.makeRef();
		
		for (auto it = candidates->begin(); it != candidates->end(); ++it) {
			auto& candidateUnitRef = *it;
			auto& actualReturnType = candidateUnitRef->getReturnType();			
			//if (expectedType != actualReturnType) {
				ParamCastingInfo paramInfo;
				int res = 0;
				if (res = scriptCompiler->findMatchingComposite(expectedType, candidateUnitRef, paramInfo)) {
					;
				}
				else if (res = scriptCompiler->findMatching(refVoidType, expectedType, actualReturnType, paramInfo, true)) {
					;
				}
				if (res) {
					if (paramInfo.castingFunction) {
						auto& castingFunction = paramInfo.castingFunction;
						applyCasting(candidateUnitRef, castingFunction);
						candidateUnitRef->setReturnType(expectedType);
					}					
					return candidateUnitRef;
				}
				
				/*if (expectedType.refLevel() == 0) {
					int castingFunctionId = scriptCompiler->findFunction(expectedType.sType(), actualReturnType.sType());
					if (castingFunctionId < 0) {
						scriptCompiler->setErrorText("cannot casting from " + actualReturnType.sType() + " to " + expectedType.sType());
						return nullptr;
					}

					auto castingFunction = scriptCompiler->createFunctionFromId(castingFunctionId);
					castingFunction->pushParam(candidateUnitRef);
					candidateUnitRef.reset(castingFunction);
					return candidateUnitRef;
				}
				else if (expectedType.origin() == actualReturnType.origin() &&
					expectedType.refLevel() - actualReturnType.refLevel() == 1) {
					int makeRefFunctionId = scriptCompiler->getMakingRefFunction();
					if (makeRefFunctionId < 0) {
						scriptCompiler->setErrorText("cannot casting from " + actualReturnType.sType() + " to " + expectedType.sType());
						return nullptr;
					}

					auto refFunction = scriptCompiler->createFunctionFromId(makeRefFunctionId);
					refFunction->pushParam(dynamic_pointer_cast<ExecutableUnit>(candidateUnitRef));
					candidateUnitRef.reset(refFunction);
					return candidateUnitRef;
				}*/
			//}
		}

		return nullptr;
	}

	EExpressionResult ScriptScope::parseExpressionInternal(std::list<ExpUnitRef>& unitList, const ScriptType* expectedReturnType) {
		ScriptCompiler* scriptCompiler = getCompiler();
		ScopedCompilingScope autoScope(scriptCompiler, this);
		ExpressionParser parser(getCompiler());

		std::list<ExpressionRef> expList;
		bool res = parser.compile(unitList, expList);
		if (res == false) {
			return E_TYPE_UNKNOWN;
		}
		EExpressionResult eResult = E_INCOMPLETED_EXPRESSION;
		if (expList.size() == 1) {
			CandidateCollectionRef candidates = std::make_shared<CandidateCollection>();
			eResult = parser.link(expList.front().get(), candidates);
			if (eResult == E_SUCCESS) {
				if (expectedReturnType == nullptr) {
					putCommandUnit(candidates->front());
				}
				else {
					auto candidate = chooseCandidate(candidates, *expectedReturnType);
					if (!candidate) {
						scriptCompiler->setErrorText("Cannot cast the return type to '" + expectedReturnType->sType() + "'");
						return eResult;
					}
					putCommandUnit(candidate);
				}
			}
		}
		else {
			auto lastUnitIter = expList.end();
			lastUnitIter--;
			CandidateCollectionRef candidates = std::make_shared<CandidateCollection>();
			for (auto it = expList.begin(); it != expList.end(); ++it) {
				eResult = parser.link(it->get(), candidates);
				if (eResult != E_SUCCESS) {
					break;
				}
				if (expectedReturnType == nullptr || it != lastUnitIter) {
					putCommandUnit(candidates->front());
				}
				else {
					auto candidate = chooseCandidate(candidates, *expectedReturnType);
					if (!candidate) {
						scriptCompiler->setErrorText("Cannot cast the return type to '" + expectedReturnType->sType() + "'");
						return eResult;
					}
					putCommandUnit(candidate);
				}
				candidates->clear();
			}
		}

		return eResult;
	}

	const wchar_t* ScriptScope::parseExpressionInternal(const wchar_t* text, const wchar_t* end,  std::wstring& expression, const ScriptType* expectedReturnType) {		
		ScriptCompiler* scriptCompiler = getCompiler();
		std::list<ExpUnitRef> unitList;
		EExpressionResult eResult = E_FAIL;
		const wchar_t* c = nullptr;
		{
			ScopedCompilingScope autoScope(scriptCompiler, this);
			ExpressionParser parser(getCompiler());

			c = parser.readExpression(text, end, eResult, unitList);
			if (eResult != E_SUCCESS || c == nullptr) {
				return nullptr;
			}
			expression = std::wstring(text, c - text);
		}

		eResult = parseExpressionInternal(unitList, expectedReturnType);
		if (eResult != E_SUCCESS) {
			c = nullptr;
		}

		return c;
	}

	const wchar_t* ScriptScope::parseExpression(const wchar_t* text, const wchar_t* end, const ScriptType* expectedReturnType) {
		std::wstring expressionString;
		const wchar_t* c = parseExpressionInternal(text, end, expressionString, expectedReturnType);
		if (c == nullptr && !expressionString.empty()) {
			ScriptCompiler* scriptCompiler = getCompiler();
			std::string lastError = scriptCompiler->getLastError();
			std::string expression = convertToAscii(expressionString.c_str());

			scriptCompiler->setErrorText("compile '" + expression + "' failed with error:" + lastError);
		}

		return c;
	}

	//void ScriptScope::setBeginExpression(CommandConstRefIter expressionIter) {
	//	_beginExpression = expressionIter;
	//}

	//void ScriptScope::setEndExpression(CommandConstRefIter expressionIter) {
	//	_endExpression = expressionIter;
	//}

	//CommandConstRefIter ScriptScope::getBeginExpression() const {
	//	return _beginExpression;
	//}

	//CommandConstRefIter ScriptScope::getEndExpression() const {
	//	return _endExpression;
	//}

	CommandConstRefIter ScriptScope::getLastExpression() const {
		CommandConstRefIter end = _commandBuilder.end();
		if (_commandBuilder.size()) {
			--end;
		}

		return end;
	}

	CommandRefIter ScriptScope::getLastExpression() {
		CommandRefIter end = _commandBuilder.end();
		if (_commandBuilder.size()) {
			--end;
		}

		return end;
	}

	CommandUnitRef* ScriptScope::getLastCommandUnit() {
		if (_commandBuilder.size() == 0) {
			return nullptr;
		}

		return &_commandBuilder.back();
	}

	CommandConstRefIter ScriptScope::getFirstExpression() const {
		return _commandBuilder.begin();
	}

	CommandRefIter ScriptScope::getFirstExpression() {
		return _commandBuilder.begin();
	}

	int ScriptScope::getExpressionCount() const {
		return (int)_commandBuilder.size();
	}

	void ScriptScope::putCommandUnit(CommandUnit* commandUnit) {
		_commandBuilder.push_back(CommandUnitRef(commandUnit));
	}

	void ScriptScope::putCommandUnit(const CommandUnitRef& commandUnitRef) {
		_commandBuilder.push_back(commandUnitRef);
	}

	void ScriptScope::insertCommandUnitBefore(CommandConstRefIter commandRefIter, CommandUnit* commandUnit) {
		_commandBuilder.insert(commandRefIter, CommandUnitRef(commandUnit));
	}

	void ScriptScope::remove(CommandConstRefIter commandRefIter) {
		_commandBuilder.erase(commandRefIter);
	}
}