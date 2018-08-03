#pragma once

#include <type_traits>

#define DECLARE_CLASS_INVOKER_TEMPLATE2(className, classOwner, ...) \
class className<__VA_ARGS__> { \
public: \
	classOwner<__VA_ARGS__>* mOwner; \
	className(classOwner<__VA_ARGS__>* pOwner) { \
		mOwner = pOwner; \
	} \
	inline void call(void* pReturnVal, void* params[]); \
}

template<typename T>
struct make_pointer {
	typedef typename std::remove_reference<T>::type* ptr_type;
};

template<typename... Types>
struct correct_types;

template<>
struct correct_types<> {};

template<typename T1, typename... Types>
struct correct_types<T1, Types...> {
	typedef correct_types<Types...> sub;
	typedef typename make_pointer<T1>::ptr_type ptr_type;
	typedef typename std::conditional<std::is_reference<T1>::value, ptr_type, T1>::type T;
};