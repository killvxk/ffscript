/******************************************************************
* File:        ScriptScopeParser.cpp
* Description: implement a part of ScriptScope class.
*              the parsing script part.
* Author:      Vincent Pham
*
* Copyright (c) 2018 VincentPT.
** Distributed under the MIT License (http://opensource.org/licenses/MIT)
**
*
**********************************************************************/

#include "ScriptScope.h"
#include "ScriptCompiler.h"
#include "Internal.h"
#include "Utils.h"
#include "ScopedCompilingScope.h"
#include "Program.h"
#include "FwdCompositeConstrutorUnit.h"

namespace ffscript {
	const wchar_t* ScriptScope::parseType(const wchar_t* text, const wchar_t* end, ScriptType& type) {
		const wchar_t* c;
		std::string token2;

		ScriptCompiler* scriptCompiler = getCompiler();

		c = scriptCompiler->readType(text, end, type);
		if (type.isUnkownType()) {
			((GlobalScope*)getRoot())->setErrorCompilerChar(text);
			scriptCompiler->setErrorText("unknown data type '" + type.sType() + "'");
			return nullptr;
		}

		return c;
	}

	void ScriptScope::constructObjectForReturning(ExecutableUnitRef& candidateUnitRef, const ScriptType& expectedType) {
		auto scriptCompiler = getCompiler();
		// ser variable to null to don't allow findMatchingConstructor method
		// apply destructor for the return object
		CXOperand* pCXOperand = new CXOperand(this, nullptr, expectedType);
		ExecutableUnitRef variableUnitRef(pCXOperand);
		ParamCastingInfo paramInfo;
		if (scriptCompiler->findMatchingConstructor(variableUnitRef, candidateUnitRef, paramInfo)) {
			auto& constructorUnit = paramInfo.castingFunction;
			auto pVariable = registTempVariable(constructorUnit.get(), -1);
			pCXOperand->setVariable(pVariable);

			candidateUnitRef = constructorUnit;

			//auto typeVoid = scriptCompiler->getTypeManager()->getBasicTypes().TYPE_VOID;
			//if (constructorUnit->getReturnType().iType() == typeVoid) {
			//	auto fwdConstructorUnitRef = make_shared<FwdConstrutorUnit>(constructorUnit);
			//	fwdConstructorUnitRef->setReturnType(expectedType);
			//	//// set it excludedded from destructor to prevent the data of variable is destroyed
			//	//fwdConstructorUnitRef->setMask(fwdConstructorUnitRef->getMask() | UMASK_EXCLUDEFROMDESTRUCTOR);

			//	candidateUnitRef = fwdConstructorUnitRef;

			//	pVariable = nullptr;
			//}
			//else 
			if (constructorUnit->getReturnType() != expectedType) {
				// throw exception here
				throw runtime_error("return type of the constructor must be void");
			}
		}
	}

	ExecutableUnitRef ScriptScope::chooseCandidate(const CandidateCollectionRef& candidates, const ScriptType& expectedType) {
		auto scriptCompiler = getCompiler();
		for (auto it = candidates->begin(); it != candidates->end(); ++it) {
			auto& candidateUnitRef = *it;
			if (candidateUnitRef->getReturnType() == expectedType) {
				if (candidateUnitRef->getType() == EXP_UNIT_ID_CONST || candidateUnitRef->getType() == EXP_UNIT_ID_DEREF) {
					constructObjectForReturning(candidateUnitRef, expectedType);
				}
				return candidateUnitRef;
			}
		}

		ScriptType refVoidType;
		refVoidType.updateType(scriptCompiler, "void");
		refVoidType = refVoidType.makeRef();
		
		for (auto it = candidates->begin(); it != candidates->end(); ++it) {
			auto& candidateUnitRef = *it;
			auto& actualReturnType = candidateUnitRef->getReturnType();	
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
					castingFunction->setSourceCharIndex(candidateUnitRef->getSourceCharIndex());

					applyCastingAndOptimize(candidateUnitRef, castingFunction);
					candidateUnitRef->setReturnType(expectedType);
				}

				// check to construct a object for returning in the function
				if (candidateUnitRef->getType() == EXP_UNIT_ID_DEREF) {						
					// because the deref just only do a shallow copy
					// to return a object to outside, the object must be constructed
					// and not be destructed after out of scope
					constructObjectForReturning(candidateUnitRef, expectedType);
				}

				return candidateUnitRef;
			}
		}

		return nullptr;
	}

	EExpressionResult ScriptScope::parseExpressionInternal(ExpressionParser* pParser, std::list<ExpUnitRef>& unitList, const ScriptType* expectedReturnType) {
		if (unitList.size() == 0) {
			return EE_SUCCESS;
		}
		ExpressionParser& parser = *pParser;

		std::list<ExpressionRef> expList;
		bool res = parser.compile(unitList, expList);
		if (res == false) {
			auto errorUnitRef = parser.getLastErrorUnit();
			if (errorUnitRef) {
				((GlobalScope*)getRoot())->setErrorCompilerCharIndex(errorUnitRef->getSourceCharIndex());
			}
			return EE_TYPE_UNKNOWN;
		}
		return parseExpressionInternal(pParser, expList, expectedReturnType);
	}

	EExpressionResult ScriptScope::parseExpressionInternal(ExpressionParser* pParser, std::list<ExpressionRef>& expList, const ScriptType* expectedReturnType) {
		ExpressionParser& parser = *pParser;
		ScriptCompiler* scriptCompiler = parser.getCompiler();
		EExpressionResult eResult = EE_INCOMPLETED_EXPRESSION;
		if (expList.size() == 1) {
			CandidateCollectionRef candidates = std::make_shared<CandidateCollection>();
			eResult = parser.link(expList.front().get(), candidates);
			if (eResult == EE_SUCCESS) {
				if (expectedReturnType == nullptr) {
					putCommandUnit(candidates->front());
				}
				else {
					auto candidate = chooseCandidate(candidates, *expectedReturnType);
					if (!candidate) {
						scriptCompiler->setErrorText("Cannot cast the return type to '" + expectedReturnType->sType() + "'");
						return EE_TYPE_CONVERSION_ERROR;
					}
					putCommandUnit(candidate);
				}
			}
			else {
				auto errorUnitRef = parser.getLastErrorUnit();
				if (errorUnitRef) {
					((GlobalScope*)getRoot())->setErrorCompilerCharIndex(errorUnitRef->getSourceCharIndex());
				}
			}
		}
		else {
			auto lastUnitIter = expList.end();
			lastUnitIter--;
			CandidateCollectionRef candidates = std::make_shared<CandidateCollection>();
			eResult = EE_SUCCESS;
			for (auto it = expList.begin(); eResult == EE_SUCCESS && it != expList.end(); ++it) {
				eResult = parser.link(it->get(), candidates);
				if (eResult == EE_SUCCESS) {
					if (expectedReturnType == nullptr || it != lastUnitIter) {
						putCommandUnit(candidates->front());
					}
					else {
						auto candidate = chooseCandidate(candidates, *expectedReturnType);
						if (!candidate) {
							scriptCompiler->setErrorText("Cannot cast the return type to '" + expectedReturnType->sType() + "'");
							((GlobalScope*)getRoot())->setErrorCompilerCharIndex(candidates->front()->getSourceCharIndex());
							return eResult;
						}
						putCommandUnit(candidate);
					}
					candidates->clear();
				}
				else {
					auto errorUnitRef = parser.getLastErrorUnit();
					if (errorUnitRef) {
						((GlobalScope*)getRoot())->setErrorCompilerCharIndex(errorUnitRef->getSourceCharIndex());
					}
				}
			}
		}

		return eResult;
	}

	const wchar_t* ScriptScope::parseExpressionInternal(const wchar_t* text, const wchar_t* end, const ScriptType* expectedReturnType) {
		ScriptCompiler* scriptCompiler = getCompiler();
		std::list<ExpUnitRef> unitList;
		EExpressionResult eResult = EE_FAIL;
		const wchar_t* c = nullptr;
		ScopedCompilingScope autoScope(scriptCompiler, this);
		ExpressionParser parser(getCompiler());

		c = parser.readExpression(text, end, eResult, unitList);
		((GlobalScope*)getRoot())->setErrorCompilerChar(parser.getLastCompileChar());
		((GlobalScope*)getRoot())->convertSourceCharIndexToGlobal(text, unitList);

		if (eResult != EE_SUCCESS || c == nullptr) {
			if (c != nullptr && scriptCompiler->getLastError().size() == 0) {
				std::wstring message = L"compile expression '" + std::wstring(text, c - text) + L"' failed";
				scriptCompiler->setErrorText(convertToAscii(message.c_str(), message.size()));
				LOG_COMPILE_MESSAGE(scriptCompiler->getLogger(), MESSAGE_ERROR, message.c_str());
			}
				
			return nullptr;
		}

		eResult = parseExpressionInternal(&parser, unitList, expectedReturnType);
		if (eResult != EE_SUCCESS) {
			c = nullptr;
		}

		return c;
	}

	const wchar_t* ScriptScope::parseDeclaredExpression(const wchar_t* text, const wchar_t* end, const ScriptType* expectedReturnType) {
		auto scriptCompiler = getCompiler();
		std::list<ExpUnitRef> unitList;
		ScopedCompilingScope autoScope(scriptCompiler, this);
		ExpressionParser parser(getCompiler());
		EExpressionResult eResult = EE_FAIL;
		auto c = parser.readExpression(text, end, eResult, unitList);
		auto globalScope = (GlobalScope*)getRoot();
		globalScope->setErrorCompilerChar(parser.getLastCompileChar());
		globalScope->convertSourceCharIndexToGlobal(text, unitList);

		if (eResult != EE_SUCCESS || c == nullptr) {
			return nullptr;
		}
		if (unitList.size() == 0) {
			scriptCompiler->setErrorText("incompleted expression");
			return nullptr;
		}
		if (ScriptCompiler::isCommandBreakSign(*c) == false) {
			scriptCompiler->setErrorText("missing ';'");
			return nullptr;
		}

		std::list<ExpressionRef> expList;
		bool res = parser.compile(unitList, expList);
		if (res == false) {
			auto errorUnitRef = parser.getLastErrorUnit();
			if (errorUnitRef) {
				((GlobalScope*)getRoot())->setErrorCompilerCharIndex(errorUnitRef->getSourceCharIndex());
			}
			return nullptr;
		}

		// root function of expression that initialize as declare must be operator '='
		for (auto expIt = expList.begin(); expIt != expList.end(); expIt++) {
			auto& rootUnit = expIt->get()->getRoot();
			if (rootUnit->getType() != EXP_UNIT_ID_OPERATOR_ASSIGNMENT) {
				scriptCompiler->setErrorText("unexpected token '" + rootUnit->toString() + "'");
				((GlobalScope*)getRoot())->setErrorCompilerCharIndex(rootUnit->getSourceCharIndex());
				return nullptr;
			}

			auto assgimentFunction = dynamic_cast<Function*>(rootUnit.get());
			// take all parameters of operator '='
			auto firstUnit = assgimentFunction->getChild(0);
			auto secondUnit = assgimentFunction->getChild(1);

			if (firstUnit->getType() == EXP_UNIT_ID_XOPERAND) {
				// set first unit with mask UMASK_DECLAREINEXPRESSION to mark it
				// is in a declared expression for further processing
				firstUnit->setMask(firstUnit->getMask() | UMASK_DECLAREINEXPRESSION);

				auto operatorEntry = scriptCompiler->findPredefinedOperator(DEFAULT_COPY_OPERATOR);
				auto defaultAssigmentUnit = new DynamicParamFunction(operatorEntry->name, operatorEntry->operatorType, operatorEntry->priority, operatorEntry->maxParam);

				defaultAssigmentUnit->setSourceCharIndex(rootUnit->getSourceCharIndex());
				// replace operator '=' by operator '<--'
				rootUnit.reset(defaultAssigmentUnit);

				// restore parameters of operator '=' to the new operator
				defaultAssigmentUnit->pushParam(firstUnit);
				defaultAssigmentUnit->pushParam(secondUnit);
			}
		}

		if (parseExpressionInternal(&parser, expList) != EE_SUCCESS) {
			c = nullptr;
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

	CommandConstRefIter ScriptScope::getLastCommandUnitRefIter() const {
		CommandConstRefIter end = _commandBuilder.end();
		if (_commandBuilder.size()) {
			--end;
		}

		return end;
	}

	CommandRefIter ScriptScope::getLastCommandUnitRefIter() {
		CommandRefIter end = _commandBuilder.end();
		if (_commandBuilder.size()) {
			--end;
		}

		return end;
	}

	CommandUnitRef* ScriptScope::getLastCommandUnitRefPtr() {
		if (_commandBuilder.size() == 0) {
			return nullptr;
		}

		return &_commandBuilder.back();
	}

	CommandConstRefIter ScriptScope::getFirstCommandUnitRefIter() const {
		return _commandBuilder.begin();
	}

	CommandRefIter ScriptScope::getFirstCommandUnitRefIter() {
		return _commandBuilder.begin();
	}

	int ScriptScope::getCommandUnitCount() const {
		return (int)_commandBuilder.size();
	}

	void ScriptScope::putCommandUnit(CommandUnitBuilder* commandUnit) {
		_commandBuilder.push_back(CommandUnitRef(commandUnit));
	}

	void ScriptScope::putCommandUnit(const CommandUnitRef& commandUnitRef) {
		_commandBuilder.push_back(commandUnitRef);
	}

	void ScriptScope::insertCommandUnitBefore(CommandConstRefIter commandRefIter, CommandUnitBuilder* commandUnit) {
		_commandBuilder.insert(commandRefIter, CommandUnitRef(commandUnit));
	}

	void ScriptScope::remove(CommandConstRefIter commandRefIter) {
		_commandBuilder.erase(commandRefIter);
	}
}