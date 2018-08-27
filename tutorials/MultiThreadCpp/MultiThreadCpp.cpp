// CoActionRecursive.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "../TutorialCommon.h"
#include <chrono>
#include <thread>

using namespace std::chrono;
using namespace std;

// native function that is real implementation of 'println' in the script
void println(const RawString& rs) {
	wcout << rs.elms << endl;
}

void importApplicationLibrary(ScriptCompiler* scriptCompiler) {
	FunctionRegisterHelper fb(scriptCompiler);

	// register 'println' function
	registerFunction
		<void, const RawString&> // native function prototype
		(fb, // register helper object
			println, // native function
			"println", //script function name
			"void", // return type of the script function
			"String&" // parameter type of the function
			);
}

int main(int argc, char* argv[])
{
	CompilerSuite compiler;

	// initialize compiler, import system library and set the stack size of global context is 1024
	compiler.initialize(1024);
	auto& rootScope = compiler.getGlobalScope();
	auto scriptCompiler = rootScope->getCompiler();

	// import custom libraries
	// String library
	includeRawStringToCompiler(scriptCompiler);

	// import application specific library
	importApplicationLibrary(scriptCompiler);

	// tell compiler that from now, any the function that will be registered
	// is a user function, that mean it will be clean each time compileProgram is executed
	scriptCompiler->beginUserLib();

	const char* scriptFile = "MultiThreadCpp.c955";
	auto script = readCodeFromUtf8File(scriptFile);
	compiler.setPreprocessor(std::make_shared<DefaultPreprocessor>());
	auto program = compiler.compileProgram(script.c_str(), script.c_str() + script.size());

	if (program == nullptr) {
		int line, column;
		compiler.getLastCompliedPosition(line, column);

		string errorMsg("error at line = ");
		errorMsg.append(std::to_string(line + 1));
		errorMsg.append(", column = ");
		errorMsg.append(std::to_string(column + 1));
		errorMsg.append("\n");
		errorMsg.append(scriptCompiler->getLastError());

		cout << "Compile program " << scriptFile << " error" << endl;
		cout << errorMsg << endl;

		return -1;
	}

	int xFunctionId = scriptCompiler->findFunction("X", "long");
	int yFunctionId = scriptCompiler->findFunction("Y", "long");
	if (xFunctionId < 0) {
		cout << "function X(long) is not found in the script" << endl;
	}
	if (yFunctionId < 0) {
		cout << "function Y(long) is not found in the script" << endl;
	}
	if(xFunctionId < 0 || yFunctionId< 0) {
		return -1;
	}
	cout << "+----------------------------------------------------------+" << endl;
	cout << "|            Compile program successfully!                 |" << endl;
	cout << "+----------------------------------------------------------+" << endl << endl;

	auto scriptProgram = rootScope->detachScriptProgram(program);
	if (scriptProgram) {
		// run the code that place in global scope
		scriptProgram->runGlobalCode();

		void* returnFromMain = nullptr;
		long long xn_1 = -1;
		long long yn_1 = -1;
		long long n = 7;
		bool hasReturnValOfX = true, hasReturnValOfY = true;

		high_resolution_clock::time_point t1 = high_resolution_clock::now();

		// call X(long) function from thread 1...
		thread th1([&hasReturnValOfX, &xn_1, n, scriptProgram, xFunctionId]() {
			try {
				// ...with a separated context from global context
				ScriptTask scriptTask(scriptProgram->getProgram());

				// pass parameter to X(long)
				ScriptParamBuffer functionParamterBuffer;
				functionParamterBuffer.addParam(n - 1);
				scriptTask.runFunction(xFunctionId, &functionParamterBuffer);

				void* res = scriptTask.getTaskResult();
				if (res) {
					xn_1 = *(long long*)res;
				}
				else {
					hasReturnValOfX = false;
				}
			}
			catch (std::exception& e) {
				cout << "An exception has been occured while running the script: " << e.what() << endl;
				hasReturnValOfX = false;
			}
		});

		// call Y(long) function from thread 2...
		thread th2([&hasReturnValOfY, &yn_1, n, scriptProgram, yFunctionId]() {
			try {
				// ...with a separated context from global context
				ScriptTask scriptTask(scriptProgram->getProgram());
				// pass parameter to Y(long)
				ScriptParamBuffer functionParamterBuffer;
				functionParamterBuffer.addParam(n - 1);
				scriptTask.runFunction(yFunctionId, &functionParamterBuffer);

				void* res = scriptTask.getTaskResult();
				if (res) {
					yn_1 = *(long long*)res;
				}
				else {
					hasReturnValOfY = false;
				}
			}
			catch (std::exception& e) {
				cout << "An exception has been occured while running the script: " << e.what() << endl;
				hasReturnValOfY = false;
			}
		});

		th1.join();
		th2.join();

		if (!hasReturnValOfX || !hasReturnValOfY) {
			cout << "Function X(long) or Y(long) does not have valid return type" << endl;
			return -1;
		}

		long long Xn = xn_1 + yn_1;
		long long Yn = 2 * xn_1 * yn_1;

		cout << "X(" << n << ") = " << Xn << endl;
		cout << "Y(" << n << ") = " << Yn << endl;

		high_resolution_clock::time_point t2 = high_resolution_clock::now();

		duration<double> time_span = duration_cast<duration<double>>(t2 - t1);
		cout << endl;
		cout << "+----------------------------------------------------------+" << endl;
		cout << "|              time consume: " << time_span.count() << "s                   |" << endl;
		cout << "+----------------------------------------------------------+" << endl;

		// clean up global memory generated by runGlobalCode method
		scriptProgram->cleanupGlobalMemory();

		delete scriptProgram;
	}

    return 0;
}

