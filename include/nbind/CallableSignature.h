// This file is part of nbind, copyright (C) 2014-2015 BusFaster Ltd.
// Released under the MIT license, see LICENSE.

#pragma once

namespace nbind {

// Templated static class for each different function call signature exposed by the
// Node.js plugin. Used to pass arguments and return values between C++ and Node.js.
// Everything must be static because the V8 JavaScript engine wants a single
// function pointer to call, so each template variant is a singleton class.

template <class Signature, typename ReturnType, typename... Args>
class CallableSignature {

public:

	struct FunctionInfo {

		typedef typename Signature::FunctionType FunctionType;

		FunctionInfo(const char *name, FunctionType func):name(name), func(func) {}

		const char *name;
		FunctionType func;

	};

	struct SignatureInfo {
		std::vector<struct FunctionInfo> funcVect;
		typename Signature::Data data;
	};

	static const FunctionInfo &getFunction(unsigned int num) {
		return(signatureStore().funcVect[num]);
	}

	template <typename FunctionType>
	static unsigned int addFunction(const char *name, FunctionType func) {
		auto &funcVect = signatureStore().funcVect;

		funcVect.emplace_back(name, func);

		return(funcVect.size() - 1);
	}

	typedef Caller<
		ReturnType,
		typename emscripten::internal::MapWithIndex<
			TypeList,
			FromWire,
			Args...
		>::type
	> CallWrapper;

	template <typename NanArgs>
	static bool typesAreValid(NanArgs args) {
		typedef Checker<
			typename emscripten::internal::MapWithIndex<
				TypeList,
				CheckWire,
				Args...
			>::type
		> checker;

		return(checker::typesAreValid(args));
	}

protected:

	static SignatureInfo &signatureStore() {
		static SignatureInfo signatureInfo;
		return(signatureInfo);
	}

};

} // namespace