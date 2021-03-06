/******************************************************************
* File:        ConditionalOperatorUT.cpp
* Description: Test cases for checking behavior of conditional operator.
* Author:      Vincent Pham
*
* Copyright (c) 2018 VincentPT.
** Distributed under the MIT License (http://opensource.org/licenses/MIT)
**
*
**********************************************************************/
#include "fftest.hpp"

#include <CompilerSuite.h>
#include <ScriptTask.h>
#include <Utils.h>

#include <ExpresionParser.h>
#include "TemplateForTest.hpp"

#include <locale>
#include <codecvt>
#include <string>

using namespace std;
using namespace ffscript;


namespace ffscriptUT
{	
	namespace ConditionalOperatorUT
	{
		FF_TEST_FUNCTION(ConditionalOperator, ConditionalOperatorSimplest1)
		{
			ScriptCompiler scriptCompiler;
			ExpressionParser parser(&scriptCompiler);
			FunctionRegisterHelper funcLibHelper(&scriptCompiler);
			const BasicTypes& basicType = scriptCompiler.getTypeManager()->getBasicTypes();
			scriptCompiler.getTypeManager()->registerBasicTypes(&scriptCompiler);
			importBasicfunction(funcLibHelper);

			list<ExpUnitRef> units;
			EExpressionResult eResult = parser.tokenize(L"1 ? 2 : 3", units);

			FF_EXPECT_TRUE(eResult == EE_SUCCESS, L"parse string to units failed");

			list<ExpressionRef> expList;
			bool res = parser.compile(units, expList);

			FF_EXPECT_TRUE(res, L"compile '1 ? 2 : 3' failed!");
			FF_EXPECT_TRUE(expList.size() == 1, L"compile '1 ? 2 : 3' and retured more than one expression");

			auto expUnit = expList.front()->getRoot();

			FF_EXPECT_EQ(EXP_UNIT_ID_FUNC_CONDITIONAL, expUnit->getType(), L"Compile success but wrong expected");

			auto conditionalExpUnit = dynamic_pointer_cast<DynamicParamFunction>(expUnit);

			auto& params = conditionalExpUnit->getParams();
			FF_EXPECT_EQ(3, (int)params.size(), L"Compile success but wrong expected");
			auto it = params.begin();

			//just call excute function if ensure the unit is const operand.
			int* val1 = (int*)(it++->get()->Execute());
			int* val2 = (int*)(it++->get()->Execute());
			int* val3 = (int*)(it++->get()->Execute());

			FF_EXPECT_EQ(1, *val1, L"Compile success but wrong the order of param wrong");
			FF_EXPECT_EQ(2, *val2, L"Compile success but wrong the order of param wrong");
			FF_EXPECT_EQ(3, *val3, L"Compile success but wrong the order of param wrong");

			//before linking type cast function should be registered
			scriptCompiler.getTypeManager()->registerBasicTypeCastFunctions(&scriptCompiler, funcLibHelper);

			//check linking
			eResult = parser.link(expList.front().get());
			FF_EXPECT_TRUE(EE_SUCCESS == eResult, L"link failed");

			expUnit = expList.front()->getRoot();
			FF_EXPECT_EQ(basicType.TYPE_INT, expUnit->getReturnType().iType(), L"link success but wrong return type for conditional operator");

			//check extracting code
			GlobalScope rootScope(1024, &scriptCompiler);
			ExpUnitExecutor excutor(&rootScope);
			res = excutor.extractCode(&scriptCompiler, expUnit);
			FF_EXPECT_TRUE(res, L"extract code failed");

			excutor.runCode();
			int* iRes = (int*) excutor.getReturnData();
			FF_EXPECT_EQ(2, *iRes, L"wrong result");

			PRINT_TEST_MESSAGE(("expression 1 ? 2 : 3 returned: " + std::to_string(*iRes)).c_str());
		}

		FF_TEST_FUNCTION(ConditionalOperator, ConditionalOperatorSimplest2)
		{
			ScriptCompiler scriptCompiler;
			ExpressionParser parser(&scriptCompiler);
			FunctionRegisterHelper funcLibHelper(&scriptCompiler);
			const BasicTypes& basicType = scriptCompiler.getTypeManager()->getBasicTypes();
			scriptCompiler.getTypeManager()->registerBasicTypes(&scriptCompiler);
			importBasicfunction(funcLibHelper);

			list<ExpUnitRef> units;
			EExpressionResult eResult = parser.tokenize(L"0 ? 2 : 3", units);

			FF_EXPECT_TRUE(eResult == EE_SUCCESS, L"parse string to units failed");

			list<ExpressionRef> expList;
			bool res = parser.compile(units, expList);

			FF_EXPECT_TRUE(res, L"compile '0 ? 2 : 3' failed!");
			FF_EXPECT_TRUE(expList.size() == 1, L"compile '0 ? 2 : 3' and retured more than one expression");

			auto expUnit = expList.front()->getRoot();

			FF_EXPECT_EQ(EXP_UNIT_ID_FUNC_CONDITIONAL, expUnit->getType(), L"Compile success but wrong expected");

			auto conditionalExpUnit = dynamic_pointer_cast<DynamicParamFunction>(expUnit);

			auto& params = conditionalExpUnit->getParams();
			FF_EXPECT_EQ(3, (int)params.size(), L"Compile success but wrong expected");
			auto it = params.begin();

			//just call excute function if ensure the unit is const operand.
			int* val1 = (int*)(it++->get()->Execute());
			int* val2 = (int*)(it++->get()->Execute());
			int* val3 = (int*)(it++->get()->Execute());

			FF_EXPECT_EQ(0, *val1, L"Compile success but wrong the order of param wrong");
			FF_EXPECT_EQ(2, *val2, L"Compile success but wrong the order of param wrong");
			FF_EXPECT_EQ(3, *val3, L"Compile success but wrong the order of param wrong");

			//before linking type cast function should be registered
			scriptCompiler.getTypeManager()->registerBasicTypeCastFunctions(&scriptCompiler, funcLibHelper);

			//check linking
			eResult = parser.link(expList.front().get());
			FF_EXPECT_TRUE(EE_SUCCESS == eResult, L"link failed");

			expUnit = expList.front()->getRoot();
			FF_EXPECT_EQ(basicType.TYPE_INT, expUnit->getReturnType().iType(), L"link success but wrong return type for conditional operator");

			//check extracting code
			GlobalScope rootScope(1024, &scriptCompiler);
			ExpUnitExecutor excutor(&rootScope);
			res = excutor.extractCode(&scriptCompiler, expUnit);
			FF_EXPECT_TRUE(res, L"extract code failed");

			excutor.runCode();
			int* iRes = (int*)excutor.getReturnData();
			FF_EXPECT_EQ(3, *iRes, L"wrong result");

			PRINT_TEST_MESSAGE(("expression 0 ? 2 : 3 returned: " + std::to_string(*iRes)).c_str());
		}

		FF_TEST_FUNCTION(ConditionalOperator, ConditionalOperatorMostComplex1) 
		{
			ScriptCompiler scriptCompiler;
			GlobalScope rootScope(1024, &scriptCompiler);
			ExpressionParser parser(&scriptCompiler);
			FunctionRegisterHelper funcLibHelper(&scriptCompiler);
			const BasicTypes& basicType = scriptCompiler.getTypeManager()->getBasicTypes();
			scriptCompiler.getTypeManager()->registerBasicTypes(&scriptCompiler);
			importBasicfunction(funcLibHelper);

			list<ExpUnitRef> units;
			scriptCompiler.pushScope(&rootScope);
			EExpressionResult eResult = parser.tokenize(L"y = 1 ? 2 ? 3 : 4 : 5", units);

			FF_EXPECT_TRUE(eResult == EE_SUCCESS, L"parse string to units failed");

			list<ExpressionRef> expList;
			bool res = parser.compile(units, expList);

			FF_EXPECT_TRUE(res, L"compile 'y = 1 ? 2 ? 3 : 4 : 5' failed!");
			FF_EXPECT_TRUE(expList.size() == 1, L"compile 'y = 1 ? 2 ? 3 : 4 : 5' and retured more than one expression");

			auto expUnit = expList.front()->getRoot();

			FF_EXPECT_EQ(EXP_UNIT_ID_OPERATOR_ASSIGNMENT, expUnit->getType(), L"Compile success but wrong expected");;

			auto assigmentUnit = dynamic_pointer_cast<DynamicParamFunction>(expUnit);
			auto& assigmentParams = assigmentUnit->getParams();
			FF_EXPECT_EQ(2, (int)assigmentParams.size(), L"Compile success but wrong expected");

			auto it = assigmentParams.begin();
			FF_EXPECT_EQ(EXP_UNIT_ID_XOPERAND, it++->get()->getType(), L"Compile success but wrong expected");
			FF_EXPECT_EQ(EXP_UNIT_ID_FUNC_CONDITIONAL, it->get()->getType(), L"Compile success but wrong expected");

			auto conditionalExpUnit = dynamic_pointer_cast<DynamicParamFunction>(*it);

			auto& params = conditionalExpUnit->getParams();
			FF_EXPECT_EQ(3, (int)params.size(), L"Compile success but wrong expected");
			it = params.begin();

			//just call excute function if ensure the unit is const operand.
			int* val1 = (int*)(it++->get()->Execute());
			auto subConditionalUnit = *(it++);
			int* val3 = (int*)(it++->get()->Execute());

			FF_EXPECT_EQ(1, *val1, L"Compile success but wrong the order of param wrong");
			FF_EXPECT_EQ(EXP_UNIT_ID_FUNC_CONDITIONAL, subConditionalUnit->getType(), L"Compile success but wrong the order of param wrong");
			FF_EXPECT_EQ(5, *val3, L"Compile success but wrong the order of param wrong");

			auto subConditionalExpUnit = dynamic_pointer_cast<DynamicParamFunction>(subConditionalUnit);

			auto& subConditionalParams = subConditionalExpUnit->getParams();
			FF_EXPECT_EQ(3, (int)subConditionalParams.size(), L"Compile success but wrong expected");
			it = subConditionalParams.begin();

			//just call excute function if ensure the unit is const operand.
			val1 = (int*)(it++->get()->Execute());
			int* val2 = (int*)(it++->get()->Execute());
			val3 = (int*)(it++->get()->Execute());

			FF_EXPECT_EQ(2, *val1, L"Compile success but wrong the order of param wrong");
			FF_EXPECT_EQ(3, *val2, L"Compile success but wrong the order of param wrong");
			FF_EXPECT_EQ(4, *val3, L"Compile success but wrong the order of param wrong");

			//before linking type cast function should be registered
			scriptCompiler.getTypeManager()->registerBasicTypeCastFunctions(&scriptCompiler, funcLibHelper);

			//check linking
			eResult = parser.link(expList.front().get());
			FF_EXPECT_TRUE(EE_SUCCESS == eResult, L"link failed");

			expUnit = expList.front()->getRoot();
			ScriptType expectedType(basicType.TYPE_INT, "int");
			FF_EXPECT_TRUE(expectedType.makeSemiRef() == expUnit->getReturnType(), L"link success but wrong return type for conditional operator");

			//because there is variable 'y'
			//so, we need call update variable for root scope before extract code
			rootScope.updateVariableOffset();

			//check extracting code
			ExpUnitExecutor excutor(&rootScope);
			res = excutor.extractCode(&scriptCompiler, expUnit);
			FF_EXPECT_TRUE(res, L"extract code failed");

			excutor.runCode();
			int* iRes = *(int**)excutor.getReturnData();
			FF_EXPECT_EQ(3, *iRes, L"wrong result");

			auto pVariable = rootScope.findVariable("y");
			int* pY = getVaribleRef<int>(*pVariable);
			FF_EXPECT_EQ(3, *pY, L"wrong result");

			PRINT_TEST_MESSAGE(("expression 'y = 1 ? 2 ? 3 : 4 : 5' returned: " + std::to_string(*iRes)).c_str());
			PRINT_TEST_MESSAGE(("and y = " + std::to_string(*pY)).c_str());
		}

		FF_TEST_FUNCTION(ConditionalOperator, ConditionalOperatorMostComplex2)
		{
			ScriptCompiler scriptCompiler;
			GlobalScope rootScope(8, &scriptCompiler);
			ExpressionParser parser(&scriptCompiler);
			FunctionRegisterHelper funcLibHelper(&scriptCompiler);
			const BasicTypes& basicType = scriptCompiler.getTypeManager()->getBasicTypes();
			scriptCompiler.getTypeManager()->registerBasicTypes(&scriptCompiler);
			importBasicfunction(funcLibHelper);

			list<ExpUnitRef> units;
			scriptCompiler.pushScope(&rootScope);
			EExpressionResult eResult = parser.tokenize(L"y = 1 ? 2 ? 3 : 4 : 5 ? 6 : 7", units);
			int y = 1 ? 2 ? 3 : 4 : 5 ? 6 : 7;
			FF_EXPECT_TRUE(eResult == EE_SUCCESS, L"parse string to units failed");

			list<ExpressionRef> expList;
			bool res = parser.compile(units, expList);

			FF_EXPECT_TRUE(res, L"compile 'y = 1 ? 2 ? 3 : 4 : 5' failed!");
			FF_EXPECT_TRUE(expList.size() == 1, L"compile 'y = 1 ? 2 ? 3 : 4 : 5' and retured more than one expression");

			auto expUnit = expList.front()->getRoot();

			FF_EXPECT_EQ(EXP_UNIT_ID_OPERATOR_ASSIGNMENT, expUnit->getType(), L"Compile success but wrong expected");;

			auto assigmentUnit = dynamic_pointer_cast<DynamicParamFunction>(expUnit);
			auto& assigmentParams = assigmentUnit->getParams();
			FF_EXPECT_EQ(2, (int)assigmentParams.size(), L"Compile success but wrong expected");

			auto it = assigmentParams.begin();
			FF_EXPECT_EQ(EXP_UNIT_ID_XOPERAND, it++->get()->getType(), L"Compile success but wrong expected");
			FF_EXPECT_EQ(EXP_UNIT_ID_FUNC_CONDITIONAL, it->get()->getType(), L"Compile success but wrong expected");

			auto conditionalExpUnit = dynamic_pointer_cast<DynamicParamFunction>(*it);

			auto& params = conditionalExpUnit->getParams();
			FF_EXPECT_EQ(3, (int)params.size(), L"Compile success but wrong expected");
			it = params.begin();

			//just call excute function if ensure the unit is const operand.
			int* val1 = (int*)(it++->get()->Execute());
			auto subConditionalUnit1 = *(it++);
			auto subConditionalUnit2 = *(it++);

			FF_EXPECT_EQ(1, *val1, L"Compile success but wrong the order of param wrong");
			FF_EXPECT_EQ(EXP_UNIT_ID_FUNC_CONDITIONAL, subConditionalUnit1->getType(), L"Compile success but wrong the order of param wrong");
			FF_EXPECT_EQ(EXP_UNIT_ID_FUNC_CONDITIONAL, subConditionalUnit2->getType(), L"Compile success but wrong the order of param wrong");

			auto subConditionalExpUnit = dynamic_pointer_cast<DynamicParamFunction>(subConditionalUnit1);

			auto& subConditionalParams = subConditionalExpUnit->getParams();
			FF_EXPECT_EQ(3, (int)subConditionalParams.size(), L"Compile success but wrong expected");
			it = subConditionalParams.begin();

			//just call excute function if ensure the unit is const operand.
			val1 = (int*)(it++->get()->Execute());
			int* val2 = (int*)(it++->get()->Execute());
			int* val3 = (int*)(it++->get()->Execute());

			FF_EXPECT_EQ(2, *val1, L"Compile success but wrong the order of param wrong");
			FF_EXPECT_EQ(3, *val2, L"Compile success but wrong the order of param wrong");
			FF_EXPECT_EQ(4, *val3, L"Compile success but wrong the order of param wrong");

			subConditionalExpUnit = dynamic_pointer_cast<DynamicParamFunction>(subConditionalUnit2);

			auto& subConditionalParams2 = subConditionalExpUnit->getParams();
			FF_EXPECT_EQ(3, (int)subConditionalParams2.size(), L"Compile success but wrong expected");
			it = subConditionalParams2.begin();

			//just call excute function if ensure the unit is const operand.
			val1 = (int*)(it++->get()->Execute());
			val2 = (int*)(it++->get()->Execute());
			val3 = (int*)(it++->get()->Execute());

			FF_EXPECT_EQ(5, *val1, L"Compile success but wrong the order of param wrong");
			FF_EXPECT_EQ(6, *val2, L"Compile success but wrong the order of param wrong");
			FF_EXPECT_EQ(7, *val3, L"Compile success but wrong the order of param wrong");
		}

		FF_TEST_FUNCTION(ConditionalOperator, ConditionalOperatorMostComplex3)
		{
			ScriptCompiler scriptCompiler;
			GlobalScope rootScope(8, &scriptCompiler);
			ExpressionParser parser(&scriptCompiler);
			FunctionRegisterHelper funcLibHelper(&scriptCompiler);
			const BasicTypes& basicType = scriptCompiler.getTypeManager()->getBasicTypes();
			scriptCompiler.getTypeManager()->registerBasicTypes(&scriptCompiler);
			importBasicfunction(funcLibHelper);

			list<ExpUnitRef> units;
			scriptCompiler.pushScope(&rootScope);
			EExpressionResult eResult = parser.tokenize(L"y = 1 ? 2 ? 3 : 4 : 5 + 6", units);
			int y = 1 ? 2 ? 3 : 4 : 5 ? 6 : 7;
			FF_EXPECT_TRUE(eResult == EE_SUCCESS, L"parse string to units failed");

			list<ExpressionRef> expList;
			bool res = parser.compile(units, expList);

			FF_EXPECT_TRUE(res, L"compile 'y = 1 ? 2 ? 3 : 4 : 5 + 6' failed!");
			FF_EXPECT_TRUE(expList.size() == 1, L"compile 'y = 1 ? 2 ? 3 : 4 : 5 + 6' and retured more than one expression");

			auto expUnit = expList.front()->getRoot();

			FF_EXPECT_EQ(EXP_UNIT_ID_OPERATOR_ASSIGNMENT, expUnit->getType(), L"Compile success but wrong expected");;

			auto assigmentUnit = dynamic_pointer_cast<DynamicParamFunction>(expUnit);
			auto& assigmentParams = assigmentUnit->getParams();
			FF_EXPECT_EQ(2, (int)assigmentParams.size(), L"Compile success but wrong expected");

			auto it = assigmentParams.begin();
			FF_EXPECT_EQ(EXP_UNIT_ID_XOPERAND, it++->get()->getType(), L"Compile success but wrong expected");
			FF_EXPECT_EQ(EXP_UNIT_ID_FUNC_CONDITIONAL, it->get()->getType(), L"Compile success but wrong expected");

			auto conditionalExpUnit = dynamic_pointer_cast<DynamicParamFunction>(*it);

			auto& params = conditionalExpUnit->getParams();
			FF_EXPECT_EQ(3, (int)params.size(), L"Compile success but wrong expected");
			it = params.begin();

			//just call excute function if ensure the unit is const operand.
			int* val1 = (int*)(it++->get()->Execute());
			auto subConditionalUnit1 = *(it++);
			auto subConditionalUnit2 = *(it++);

			FF_EXPECT_EQ(1, *val1, L"Compile success but wrong the order of param wrong");
			FF_EXPECT_EQ(EXP_UNIT_ID_FUNC_CONDITIONAL, subConditionalUnit1->getType(), L"Compile success but wrong the order of param wrong");
			FF_EXPECT_EQ(EXP_UNIT_ID_OPERATOR_ADD, subConditionalUnit2->getType(), L"Compile success but wrong the order of param wrong");

			auto subConditionalExpUnit = dynamic_pointer_cast<DynamicParamFunction>(subConditionalUnit1);

			auto& subConditionalParams = subConditionalExpUnit->getParams();
			FF_EXPECT_EQ(3, (int)subConditionalParams.size(), L"Compile success but wrong expected");
			it = subConditionalParams.begin();

			//just call excute function if ensure the unit is const operand.
			val1 = (int*)(it++->get()->Execute());
			int* val2 = (int*)(it++->get()->Execute());
			int* val3 = (int*)(it++->get()->Execute());

			FF_EXPECT_EQ(2, *val1, L"Compile success but wrong the order of param wrong");
			FF_EXPECT_EQ(3, *val2, L"Compile success but wrong the order of param wrong");
			FF_EXPECT_EQ(4, *val3, L"Compile success but wrong the order of param wrong");

			subConditionalExpUnit = dynamic_pointer_cast<DynamicParamFunction>(subConditionalUnit2);

			auto& subConditionalParams2 = subConditionalExpUnit->getParams();
			FF_EXPECT_EQ(2, (int)subConditionalParams2.size(), L"Compile success but wrong expected");
			it = subConditionalParams2.begin();

			//just call excute function if ensure the unit is const operand.
			val1 = (int*)(it++->get()->Execute());
			val2 = (int*)(it++->get()->Execute());

			FF_EXPECT_EQ(5, *val1, L"Compile success but wrong the order of param wrong");
			FF_EXPECT_EQ(6, *val2, L"Compile success but wrong the order of param wrong");
		}

		FF_TEST_FUNCTION(ConditionalOperator, ConditionalOperatorMostComplex4)
		{
			ScriptCompiler scriptCompiler;
			GlobalScope rootScope(8, &scriptCompiler);
			ExpressionParser parser(&scriptCompiler);
			FunctionRegisterHelper funcLibHelper(&scriptCompiler);
			const BasicTypes& basicType = scriptCompiler.getTypeManager()->getBasicTypes();
			scriptCompiler.getTypeManager()->registerBasicTypes(&scriptCompiler);
			importBasicfunction(funcLibHelper);

			list<ExpUnitRef> units;
			scriptCompiler.pushScope(&rootScope);
			EExpressionResult eResult = parser.tokenize(L"y = 1 ? 2 + 3 : 5 ? 6 : 7", units);
			FF_EXPECT_TRUE(eResult == EE_SUCCESS, L"parse string to units failed");

			list<ExpressionRef> expList;
			bool res = parser.compile(units, expList);

			FF_EXPECT_TRUE(res, L"compile 'y = 1 ? 2 + 4 : 5' failed!");
			FF_EXPECT_TRUE(expList.size() == 1, L"compile 'y = 1 ? 2 ? 3 : 4 : 5' and retured more than one expression");

			auto expUnit = expList.front()->getRoot();

			FF_EXPECT_EQ(EXP_UNIT_ID_OPERATOR_ASSIGNMENT, expUnit->getType(), L"Compile success but wrong expected");;

			auto assigmentUnit = dynamic_pointer_cast<DynamicParamFunction>(expUnit);
			auto& assigmentParams = assigmentUnit->getParams();
			FF_EXPECT_EQ(2, (int)assigmentParams.size(), L"Compile success but wrong expected");

			auto it = assigmentParams.begin();
			FF_EXPECT_EQ(EXP_UNIT_ID_XOPERAND, it++->get()->getType(), L"Compile success but wrong expected");
			FF_EXPECT_EQ(EXP_UNIT_ID_FUNC_CONDITIONAL, it->get()->getType(), L"Compile success but wrong expected");

			auto conditionalExpUnit = dynamic_pointer_cast<DynamicParamFunction>(*it);

			auto& params = conditionalExpUnit->getParams();
			FF_EXPECT_EQ(3, (int)params.size(), L"Compile success but wrong expected");
			it = params.begin();

			//just call excute function if ensure the unit is const operand.
			int* val1 = (int*)(it++->get()->Execute());
			auto subConditionalUnit1 = *(it++);
			auto subConditionalUnit2 = *(it++);

			FF_EXPECT_EQ(1, *val1, L"Compile success but wrong the order of param wrong");
			FF_EXPECT_EQ(EXP_UNIT_ID_OPERATOR_ADD, subConditionalUnit1->getType(), L"Compile success but wrong the order of param wrong");
			FF_EXPECT_EQ(EXP_UNIT_ID_FUNC_CONDITIONAL, subConditionalUnit2->getType(), L"Compile success but wrong the order of param wrong");

			auto subConditionalExpUnit = dynamic_pointer_cast<DynamicParamFunction>(subConditionalUnit1);

			auto& subConditionalParams = subConditionalExpUnit->getParams();
			FF_EXPECT_EQ(2, (int)subConditionalParams.size(), L"Compile success but wrong expected");
			it = subConditionalParams.begin();

			//just call excute function if ensure the unit is const operand.
			val1 = (int*)(it++->get()->Execute());
			int* val2 = (int*)(it++->get()->Execute());

			FF_EXPECT_EQ(2, *val1, L"Compile success but wrong the order of param wrong");
			FF_EXPECT_EQ(3, *val2, L"Compile success but wrong the order of param wrong");

			subConditionalExpUnit = dynamic_pointer_cast<DynamicParamFunction>(subConditionalUnit2);

			auto& subConditionalParams2 = subConditionalExpUnit->getParams();
			FF_EXPECT_EQ(3, (int)subConditionalParams2.size(), L"Compile success but wrong expected");
			it = subConditionalParams2.begin();

			//just call excute function if ensure the unit is const operand.
			val1 = (int*)(it++->get()->Execute());
			val2 = (int*)(it++->get()->Execute());
			int* val3 = (int*)(it++->get()->Execute());

			FF_EXPECT_EQ(5, *val1, L"Compile success but wrong the order of param wrong");
			FF_EXPECT_EQ(6, *val2, L"Compile success but wrong the order of param wrong");
			FF_EXPECT_EQ(7, *val3, L"Compile success but wrong the order of param wrong");
		}

		FF_TEST_FUNCTION(ConditionalOperator, ConditionalOperatorMostComplex5)
		{
			ScriptCompiler scriptCompiler;
			GlobalScope rootScope(8, &scriptCompiler);
			ExpressionParser parser(&scriptCompiler);
			FunctionRegisterHelper funcLibHelper(&scriptCompiler);
			const BasicTypes& basicType = scriptCompiler.getTypeManager()->getBasicTypes();
			scriptCompiler.getTypeManager()->registerBasicTypes(&scriptCompiler);
			importBasicfunction(funcLibHelper);

			list<ExpUnitRef> units;
			scriptCompiler.pushScope(&rootScope);
			EExpressionResult eResult = parser.tokenize(L"y = 0 + 1 ? 2 ? 3 : 4 : 5 ? 6 : 7", units);
			int y = 1 ? 2 ? 3 : 4 : 5 ? 6 : 7;
			FF_EXPECT_TRUE(eResult == EE_SUCCESS, L"parse string to units failed");

			list<ExpressionRef> expList;
			bool res = parser.compile(units, expList);

			FF_EXPECT_TRUE(res, L"compile 'y = 0 + 1 ? 2 ? 3 : 4 : 5 ? 6 : 7' failed!");
			FF_EXPECT_TRUE(expList.size() == 1, L"compile 'y = 0 + 1 ? 2 ? 3 : 4 : 5 ? 6 : 7' and retured more than one expression");

			auto expUnit = expList.front()->getRoot();

			FF_EXPECT_EQ(EXP_UNIT_ID_OPERATOR_ASSIGNMENT, expUnit->getType(), L"Compile success but wrong expected");;

			auto assigmentUnit = dynamic_pointer_cast<DynamicParamFunction>(expUnit);
			auto& assigmentParams = assigmentUnit->getParams();
			FF_EXPECT_EQ(2, (int)assigmentParams.size(), L"Compile success but wrong expected");

			auto it = assigmentParams.begin();
			FF_EXPECT_EQ(EXP_UNIT_ID_XOPERAND, it++->get()->getType(), L"Compile success but wrong expected");
			FF_EXPECT_EQ(EXP_UNIT_ID_FUNC_CONDITIONAL, it->get()->getType(), L"Compile success but wrong expected");

			auto conditionalExpUnit = dynamic_pointer_cast<DynamicParamFunction>(*it);

			auto& params = conditionalExpUnit->getParams();
			FF_EXPECT_EQ(3, (int)params.size(), L"Compile success but wrong expected");
			it = params.begin();

			//just call excute function if ensure the unit is const operand.
			auto additionalUnit = *(it++);
			auto subConditionalUnit1 = *(it++);
			auto subConditionalUnit2 = *(it++);

			FF_EXPECT_EQ(EXP_UNIT_ID_OPERATOR_ADD, additionalUnit->getType(), L"Compile success but wrong the order of param wrong");
			FF_EXPECT_EQ(EXP_UNIT_ID_FUNC_CONDITIONAL, subConditionalUnit1->getType(), L"Compile success but wrong the order of param wrong");
			FF_EXPECT_EQ(EXP_UNIT_ID_FUNC_CONDITIONAL, subConditionalUnit2->getType(), L"Compile success but wrong the order of param wrong");

			auto& addParams = dynamic_pointer_cast<DynamicParamFunction>(additionalUnit)->getParams();
			FF_EXPECT_EQ(2, (int)addParams.size(), L"Compile success but wrong expected");

			it = addParams.begin();
			int* val1 = (int*)(it++->get()->Execute());
			int* val2 = (int*)(it++->get()->Execute());
			FF_EXPECT_EQ(0, *val1, L"Compile success but wrong the order of param wrong");
			FF_EXPECT_EQ(1, *val2, L"Compile success but wrong the order of param wrong");

			auto subConditionalExpUnit = dynamic_pointer_cast<DynamicParamFunction>(subConditionalUnit1);

			auto& subConditionalParams = subConditionalExpUnit->getParams();
			FF_EXPECT_EQ(3, (int)subConditionalParams.size(), L"Compile success but wrong expected");
			it = subConditionalParams.begin();

			//just call excute function if ensure the unit is const operand.
			val1 = (int*)(it++->get()->Execute());
			val2 = (int*)(it++->get()->Execute());
			int* val3 = (int*)(it++->get()->Execute());

			FF_EXPECT_EQ(2, *val1, L"Compile success but wrong the order of param wrong");
			FF_EXPECT_EQ(3, *val2, L"Compile success but wrong the order of param wrong");
			FF_EXPECT_EQ(4, *val3, L"Compile success but wrong the order of param wrong");

			subConditionalExpUnit = dynamic_pointer_cast<DynamicParamFunction>(subConditionalUnit2);

			auto& subConditionalParams2 = subConditionalExpUnit->getParams();
			FF_EXPECT_EQ(3, (int)subConditionalParams2.size(), L"Compile success but wrong expected");
			it = subConditionalParams2.begin();

			//just call excute function if ensure the unit is const operand.
			val1 = (int*)(it++->get()->Execute());
			val2 = (int*)(it++->get()->Execute());
			val3 = (int*)(it++->get()->Execute());

			FF_EXPECT_EQ(5, *val1, L"Compile success but wrong the order of param wrong");
			FF_EXPECT_EQ(6, *val2, L"Compile success but wrong the order of param wrong");
			FF_EXPECT_EQ(7, *val3, L"Compile success but wrong the order of param wrong");
		}

		FF_TEST_FUNCTION(ConditionalOperator, CheckExcution1)
		{
			CompilerSuite compiler;

			//the code does not contain any global scope'code and only a variable
			//so does not need global memory
			compiler.initialize(1024);
			GlobalScopeRef rootScope = compiler.getGlobalScope();

			auto pVariableX = rootScope->registVariable("x");
			auto pVariableY = rootScope->registVariable("y");
			auto pVariableZ = rootScope->registVariable("z");

			auto& basicTypes = compiler.getTypeManager()->getBasicTypes();

			pVariableX->setDataType(ScriptType(basicTypes.TYPE_INT,"int"));
			pVariableY->setDataType(ScriptType(basicTypes.TYPE_INT,"int"));
			pVariableZ->setDataType(ScriptType(basicTypes.TYPE_INT,"int"));

			const wchar_t*  scriptCode = L"x + 1 ? 3 + (y ? -3 : 3) ? 100 : 101 : z ? 102 : 103";
			auto excutor = compiler.compileExpression(scriptCode);
			FF_EXPECT_NE(nullptr, excutor, L"Compile conditional expression failed");

			setVariableValue<int>(*pVariableX, 0);
			setVariableValue<int>(*pVariableY, 0);
			setVariableValue<int>(*pVariableZ, 0);

			excutor->runCode();
			int* res = (int*)excutor->getReturnData();

			FF_EXPECT_TRUE(*res == 100, L"program can run but return wrong value");
		}

		FF_TEST_FUNCTION(ConditionalOperator, CheckExcution2)
		{
			CompilerSuite compiler;

			//the code does not contain any global scope'code and only a variable
			//so does not need global memory
			compiler.initialize(1024);
			GlobalScopeRef rootScope = compiler.getGlobalScope();

			auto pVariableX = rootScope->registVariable("x");
			auto pVariableY = rootScope->registVariable("y");
			auto pVariableZ = rootScope->registVariable("z");

			auto& basicTypes = compiler.getTypeManager()->getBasicTypes();

			pVariableX->setDataType(ScriptType(basicTypes.TYPE_INT,"int"));
			pVariableY->setDataType(ScriptType(basicTypes.TYPE_INT,"int"));
			pVariableZ->setDataType(ScriptType(basicTypes.TYPE_INT,"int"));

			const wchar_t*  scriptCode = L"x + 1 ? 3 + (y ? (-3) : (3 + 1)) ? 100 : 101 : z ? 102 : 103";
			auto excutor = compiler.compileExpression(scriptCode);
			FF_EXPECT_NE(nullptr, excutor, L"Compile conditional expression failed");

			setVariableValue<int>(*pVariableX, 0);
			setVariableValue<int>(*pVariableY, 0);
			setVariableValue<int>(*pVariableZ, 0);

			excutor->runCode();
			int* res = (int*)excutor->getReturnData();

			FF_EXPECT_TRUE(*res == 100, L"program can run but return wrong value");
		}

		FF_TEST_FUNCTION(ConditionalOperator, CheckExcution3)
		{
			CompilerSuite compiler;

			//the code does not contain any global scope'code and only a variable
			//so does not need global memory
			compiler.initialize(1024);
			GlobalScopeRef rootScope = compiler.getGlobalScope();

			auto pVariableX = rootScope->registVariable("x");
			auto pVariableY = rootScope->registVariable("y");
			auto pVariableZ = rootScope->registVariable("z");

			auto& basicTypes = compiler.getTypeManager()->getBasicTypes();

			pVariableX->setDataType(ScriptType(basicTypes.TYPE_INT,"int"));
			pVariableY->setDataType(ScriptType(basicTypes.TYPE_INT,"int"));
			pVariableZ->setDataType(ScriptType(basicTypes.TYPE_INT,"int"));

			const wchar_t*  scriptCode = L"x + 1 ? 3 + (y ? (-3) : (3 + 1)) ? 100 : 101 : z ? 102 : 103";
			auto excutor = compiler.compileExpression(scriptCode);
			FF_EXPECT_NE(nullptr, excutor, L"Compile conditional expression failed");

			setVariableValue<int>(*pVariableX, 0);
			setVariableValue<int>(*pVariableY, 1);
			setVariableValue<int>(*pVariableZ, 0);

			excutor->runCode();
			int* res = (int*)excutor->getReturnData();

			FF_EXPECT_TRUE(*res == 101, L"program can run but return wrong value");
		}

		FF_TEST_FUNCTION(ConditionalOperator, CheckExcution4)
		{
			CompilerSuite compiler;

			//the code does not contain any global scope'code and only a variable
			//so does not need global memory
			compiler.initialize(1024);
			GlobalScopeRef rootScope = compiler.getGlobalScope();

			auto pVariableX = rootScope->registVariable("x");
			auto pVariableY = rootScope->registVariable("y");
			auto pVariableZ = rootScope->registVariable("z");

			auto& basicTypes = compiler.getTypeManager()->getBasicTypes();

			pVariableX->setDataType(ScriptType(basicTypes.TYPE_INT,"int"));
			pVariableY->setDataType(ScriptType(basicTypes.TYPE_INT,"int"));
			pVariableZ->setDataType(ScriptType(basicTypes.TYPE_INT,"int"));

			const wchar_t*  scriptCode = L"x + 1 ? 3 + (y ? length(\"test string\") ? -3 : 0 : (3 + 1)) ? 100 : 101 : z ? 102 : 103";
			auto excutor = compiler.compileExpression(scriptCode);
			FF_EXPECT_NE(nullptr, excutor, L"Compile conditional expression failed");

			setVariableValue<int>(*pVariableX, 0);
			setVariableValue<int>(*pVariableY, 1);
			setVariableValue<int>(*pVariableZ, 0);

			excutor->runCode();
			int* res = (int*)excutor->getReturnData();

			FF_EXPECT_TRUE(*res == 101, L"program can run but return wrong value");
		}

		FF_TEST_FUNCTION(ConditionalOperator, CheckExcution5)
		{
			CompilerSuite compiler;

			//the code does not contain any global scope'code and only a variable
			//so does not need global memory
			compiler.initialize(1024);
			GlobalScopeRef rootScope = compiler.getGlobalScope();

			auto pVariableX = rootScope->registVariable("x");
			auto pVariableY = rootScope->registVariable("y");
			auto pVariableZ = rootScope->registVariable("z");

			auto& basicTypes = compiler.getTypeManager()->getBasicTypes();

			pVariableX->setDataType(ScriptType(basicTypes.TYPE_INT,"int"));
			pVariableY->setDataType(ScriptType(basicTypes.TYPE_INT,"int"));
			pVariableZ->setDataType(ScriptType(basicTypes.TYPE_INT,"int"));

			const wchar_t*  scriptCode = L"x + 1 ? 3 + (y ? length(\"\") ? -3 : 0 : (3 + 1)) ? 100 : 101 : z ? 102 : 103";
			auto excutor = compiler.compileExpression(scriptCode);
			FF_EXPECT_NE(nullptr, excutor, L"Compile conditional expression failed");

			setVariableValue<int>(*pVariableX, 0);
			setVariableValue<int>(*pVariableY, 1);
			setVariableValue<int>(*pVariableZ, 0);

			excutor->runCode();
			int* res = (int*)excutor->getReturnData();

			FF_EXPECT_TRUE(*res == 100, L"program can run but return wrong value");
		}

		FF_TEST_FUNCTION(ConditionalOperator, CheckExcution6)
		{
			CompilerSuite compiler;

			//the code does not contain any global scope'code and only a variable
			//so does not need global memory
			compiler.initialize(1024);
			GlobalScopeRef rootScope = compiler.getGlobalScope();

			auto pVariableX = rootScope->registVariable("x");
			auto pVariableY = rootScope->registVariable("y");
			auto pVariableZ = rootScope->registVariable("z");

			auto& basicTypes = compiler.getTypeManager()->getBasicTypes();

			pVariableX->setDataType(ScriptType(basicTypes.TYPE_INT,"int"));
			pVariableY->setDataType(ScriptType(basicTypes.TYPE_INT,"int"));
			pVariableZ->setDataType(ScriptType(basicTypes.TYPE_INT,"int"));

			const wchar_t*  scriptCode = L"x + 1 ? 3 + (y ? length(\"\") ? -3 : 0 : (3 + 1)) ? 100 : 101 : z ? 102 : 103";
			auto excutor = compiler.compileExpression(scriptCode);
			FF_EXPECT_NE(nullptr, excutor, L"Compile conditional expression failed");

			setVariableValue<int>(*pVariableX, -1);
			setVariableValue<int>(*pVariableY, 1);
			setVariableValue<int>(*pVariableZ, 1);

			excutor->runCode();
			int* res = (int*)excutor->getReturnData();

			FF_EXPECT_TRUE(*res == 102, L"program can run but return wrong value");
		}

		FF_TEST_FUNCTION(ConditionalOperator, CheckExcution7)
		{
			CompilerSuite compiler;

			//the code does not contain any global scope'code and only a variable
			//so does not need global memory
			compiler.initialize(1024);
			GlobalScopeRef rootScope = compiler.getGlobalScope();

			auto pVariableX = rootScope->registVariable("x");
			auto pVariableY = rootScope->registVariable("y");
			auto pVariableZ = rootScope->registVariable("z");

			auto& basicTypes = compiler.getTypeManager()->getBasicTypes();

			pVariableX->setDataType(ScriptType(basicTypes.TYPE_INT,"int"));
			pVariableY->setDataType(ScriptType(basicTypes.TYPE_INT,"int"));
			pVariableZ->setDataType(ScriptType(basicTypes.TYPE_INT,"int"));

			const wchar_t*  scriptCode = L"x + 1 ? 3 + (y ? length(\"\") ? -3 : 0 : (3 + 1)) ? 100 : 101 : z ? 102 : 103";
			auto excutor = compiler.compileExpression(scriptCode);
			FF_EXPECT_NE(nullptr, excutor, L"Compile conditional expression failed");

			setVariableValue<int>(*pVariableX, -1);
			setVariableValue<int>(*pVariableY, 1);
			setVariableValue<int>(*pVariableZ, 0);

			excutor->runCode();
			int* res = (int*)excutor->getReturnData();

			FF_EXPECT_TRUE(*res == 103, L"program can run but return wrong value");
		}

		FF_TEST_FUNCTION(ConditionalOperator, CheckExcution8)
		{
			CompilerSuite compiler;

			//the code does not contain any global scope'code and only a variable
			//so does not need global memory
			compiler.initialize(1024);
			GlobalScopeRef rootScope = compiler.getGlobalScope();

			auto& basicTypes = compiler.getTypeManager()->getBasicTypes();

			const wchar_t*  scriptCode = L"1 ? 2 * length(\"1\") ? 4 : 5 : 6";
			auto excutor = compiler.compileExpression(scriptCode);
			FF_EXPECT_NE(nullptr, excutor, L"Compile conditional expression failed");

			excutor->runCode();
			int* res = (int*)excutor->getReturnData();

			FF_EXPECT_TRUE(*res == 4, L"program can run but return wrong value");
		}

		FF_TEST_FUNCTION(ConditionalOperator, CheckExcution9)
		{
			CompilerSuite compiler;

			//the code does not contain any global scope'code and only a variable
			//so does not need global memory
			compiler.initialize(1024);
			GlobalScopeRef rootScope = compiler.getGlobalScope();

			auto pVariableX = rootScope->registVariable("x");
			auto pVariableY = rootScope->registVariable("y");
			auto pVariableZ = rootScope->registVariable("z");

			auto& basicTypes = compiler.getTypeManager()->getBasicTypes();

			pVariableX->setDataType(ScriptType(basicTypes.TYPE_INT,"int"));
			pVariableY->setDataType(ScriptType(basicTypes.TYPE_INT,"int"));
			pVariableZ->setDataType(ScriptType(basicTypes.TYPE_INT,"int"));

			//const wchar_t*  scriptCode = L"x + 1 ? 3 + (y ? 2*length(\"test string\") ? -3 : 0 : (3 + 1)) ? 100 : 101 : z ? 102 : 103";
			const wchar_t*  scriptCode = L"x + 1 ? 3 + (y ? 2*length(\"1\") ? -3 : 0 : 2) ? 100 : 101 :102";
			auto excutor = compiler.compileExpression(scriptCode);
			FF_EXPECT_NE(nullptr, excutor, L"Compile conditional expression failed");

			setVariableValue<int>(*pVariableX, 0);
			setVariableValue<int>(*pVariableY, 1);
			setVariableValue<int>(*pVariableZ, 0);

			excutor->runCode();
			int* res = (int*)excutor->getReturnData();

			FF_EXPECT_TRUE(*res == 101, L"program can run but return wrong value");
		}

		FF_TEST_FUNCTION(ConditionalOperator, CheckCompileExcution1)
		{
			CompilerSuite compiler;

			//the code does not contain any global scope'code and only a variable
			//so does not need global memory
			compiler.initialize(1024);
			GlobalScopeRef rootScope = compiler.getGlobalScope();

			auto pVariableX = rootScope->registVariable("x");
			auto pVariableY = rootScope->registVariable("y");
			auto pVariableZ = rootScope->registVariable("z");

			auto& basicTypes = compiler.getTypeManager()->getBasicTypes();

			pVariableX->setDataType(ScriptType(basicTypes.TYPE_INT,"int"));
			pVariableY->setDataType(ScriptType(basicTypes.TYPE_INT,"int"));
			pVariableZ->setDataType(ScriptType(basicTypes.TYPE_INT,"int"));

			const wchar_t*  scriptCode = L"x + 1 ? 3 + y ? -3 : 3) ? 100 : 101 : z ? 102 : 103";
			auto excutor = compiler.compileExpression(scriptCode);
			FF_EXPECT_EQ(nullptr, excutor, L"Compile conditional expression must failed");
		}

		FF_TEST_FUNCTION(ConditionalOperator, CheckExcutionInsufficientMemory)
		{
			CompilerSuite compiler;

			//the code does not contain any global scope'code and only a variable
			//so does not need global memory
			compiler.initialize(128);
			GlobalScopeRef rootScope = compiler.getGlobalScope();

			auto pVariableX = rootScope->registVariable("x");
			auto pVariableY = rootScope->registVariable("y");
			auto pVariableZ = rootScope->registVariable("z");

			auto& basicTypes = compiler.getTypeManager()->getBasicTypes();

			pVariableX->setDataType(ScriptType(basicTypes.TYPE_INT,"int"));
			pVariableY->setDataType(ScriptType(basicTypes.TYPE_INT,"int"));
			pVariableZ->setDataType(ScriptType(basicTypes.TYPE_INT,"int"));

			const wchar_t*  scriptCode = L"x + 1 ? 3 + (y ? length(\"\") ? -3 : 0 : (3 + 1)) ? 100 : 101 : z ? 102 : 103";
			auto excutor = compiler.compileExpression(scriptCode);
			FF_EXPECT_NE(nullptr, excutor, L"Compile conditional expression failed");

			setVariableValue<int>(*pVariableX, -1);
			setVariableValue<int>(*pVariableY, 1);
			setVariableValue<int>(*pVariableZ, 0);

			try
			{
				excutor->runCode();

				int* res = (int*)excutor->getReturnData();
				FF_EXPECT_TRUE(*res == 103, L"program can run but return wrong value");
			}
			catch (const std::exception& e)
			{
				//string errorMsg = e.what();

				//std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
				////::string narrow = converter.to_bytes(wide_utf16_source_string);
				//std::wstring wErrorMsg = converter.from_bytes(errorMsg);

				//FF_EXPECT_TRUE(false, wErrorMsg.c_str());

				PRINT_TEST_MESSAGE(e.what());
			}
		}
	};
}