/*!*****************************************************************************************
	This file contains macro definition REFLECTABLE that is responsible for static reflection by
	generating metadata for every data member. It also contains template function declaration
	that works with the reflected metadata.

	Let's first look at example:

	struct Person {
		int x = 0;
		float y = 5;

		REFLECTABLE(x, y);
	};

	This will in turn create metadata (which is type FieldData) for each data member.
	It should look something like this.. (many details are hidden for clarify)

	struct Person {
		int x = 0;
		float y = 5;

		struct FieldData<0> {
			int& get()			{ return  x;  }
			const char* name()	{ return "x"; }
		}

		struct FieldData<1> {
			float& get()		{ return  y;  }
			const char* name()	{ return "y"; }
		}
	};

	With the generated metadata, we can iterate through all the data members of a given
	reflectable class, and work with the individual data members via get().

	Reference: https://stackoverflow.com/questions/41453/how-can-i-add-reflection-to-a-c-application
	(goated link)

*******************************************************************************************/

#ifndef CPP_REFLECTION_H
#define CPP_REFLECTION_H

// i dont know why MSVC doesn't like conforming to the standard but ok.
#if ((defined(_MSVC_LANG) && _MSVC_LANG < 202002L) || (!defined(_MSVC_LANG) && __cplusplus < 202002L))
	#error "Requires at least C++20 support. Please ensure your compiler supports C++20."
#elif defined(_MSC_VER) && (!defined(_MSVC_TRADITIONAL) || _MSVC_TRADITIONAL)
	#error "For MSVC, please change project configuration to use standard conforming preprocessor! /Zc:preprocessor"
	#error "Refer to README for simple guide in updating your project configuration."
	#error "Go to project properties > C/C++ > Preprocessor > Use Standard Conforming Preprocessor and set it to Yes (/Zc:preprocessor)."
#else

#include <iostream>
#include <string_view>
#include <concepts>
#include <type_traits>

namespace reflection {
	// For each data member, print it's name and value to stdout. Assumes the data member overloads the operator<<
	template <typename T>
	void print(T&& x);
	
	// Similar to print, but indents for each recursion depth.
	template <typename T>
	void prettyPrint(T&& x);

	// Check if a given class is reflectable.
	template <typename T>
	constexpr bool isReflectable();

	/*!***********************************************************************
	* @brief
	*	This function allows you to retrieve the metadata generated for each
	*	reflected data member.
	*
	*	The function is designed such that template arguments are deduced automatically.
	*
	* @tparam Functor		: The type should ideally be a templated lambda with the following format: [](auto&&){}
	*						  Capture clause doesn't matter here. Only the argument type.
	* @tparam T				: The type of object you want to reflect.
	*
	* @param [in] x			: The object you want to reflect.
	* @param [in] func		: Unary function object, which will be invoked for every reflected data member.
	*						  fieldData.get() to retrieve reference to data member.
	*						  fieldData.name() to retrieve name of data member.
	*
	**************************************************************************/
	template<typename Functor, typename T>
	void visit(Functor&& func, T&& x);

	// similar to visit, but invokes enterFunc when first encountering a reflectable data member before recursing and invokes exitFunc after finishing iterating.
	template<typename Functor, typename Functor2, typename Functor3, typename T>
	void visit(Functor&& func, Functor2&& enterFunc, Functor3&& exitFunc, T&& x);
}

/*!========================================================================
	Implementation details..
========================================================================*/
#define COUNT_ARGS(...) COUNT_ARGS_IMPL(__VA_ARGS__, 32, 31, 30, 29, 28, 27, 26, 25, 24, 23, 22, 21, 20, 19, 18, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0)
#define COUNT_ARGS_IMPL(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, _31, _32, N, ...) N

// Applies macro for each variadic argument
#define  APPLY_MACRO_TO_EACH_1(macro, index, a)																												macro(a, index)
#define  APPLY_MACRO_TO_EACH_2(macro, index, a, b) 																											macro(a, index)  APPLY_MACRO_TO_EACH_1(macro, index + 1, b)
#define  APPLY_MACRO_TO_EACH_3(macro, index, a, b, c) 																										macro(a, index)  APPLY_MACRO_TO_EACH_2(macro, index + 1, b, c)
#define  APPLY_MACRO_TO_EACH_4(macro, index, a, b, c, d) 																									macro(a, index)  APPLY_MACRO_TO_EACH_3(macro, index + 1, b, c, d)
#define  APPLY_MACRO_TO_EACH_5(macro, index, a, b, c, d, e) 																								macro(a, index)  APPLY_MACRO_TO_EACH_4(macro, index + 1, b, c, d, e)
#define  APPLY_MACRO_TO_EACH_6(macro, index, a, b, c, d, e, f) 																								macro(a, index)  APPLY_MACRO_TO_EACH_5(macro, index + 1, b, c, d, e, f)
#define  APPLY_MACRO_TO_EACH_7(macro, index, a, b, c, d, e, f, g) 																							macro(a, index)  APPLY_MACRO_TO_EACH_6(macro, index + 1, b, c, d, e, f, g)
#define  APPLY_MACRO_TO_EACH_8(macro, index, a, b, c, d, e, f, g, h)																						macro(a, index)  APPLY_MACRO_TO_EACH_7(macro, index + 1, b, c, d, e, f, g, h)
#define  APPLY_MACRO_TO_EACH_9(macro, index, a, b, c, d, e, f, g, h, i)																						macro(a, index)  APPLY_MACRO_TO_EACH_8(macro, index + 1, b, c, d, e, f, g, h, i)
#define APPLY_MACRO_TO_EACH_10(macro, index, a, b, c, d, e, f, g, h, i, j)																					macro(a, index)  APPLY_MACRO_TO_EACH_9(macro, index + 1, b, c, d, e, f, g, h, i, j)
#define APPLY_MACRO_TO_EACH_11(macro, index, a, b, c, d, e, f, g, h, i, j, k)																				macro(a, index) APPLY_MACRO_TO_EACH_10(macro, index + 1, b, c, d, e, f, g, h, i, j, k)
#define APPLY_MACRO_TO_EACH_12(macro, index, a, b, c, d, e, f, g, h, i, j, k, l)																			macro(a, index) APPLY_MACRO_TO_EACH_11(macro, index + 1, b, c, d, e, f, g, h, i, j, k, l)
#define APPLY_MACRO_TO_EACH_13(macro, index, a, b, c, d, e, f, g, h, i, j, k, l, m)																			macro(a, index) APPLY_MACRO_TO_EACH_12(macro, index + 1, b, c, d, e, f, g, h, i, j, k, l, m)
#define APPLY_MACRO_TO_EACH_14(macro, index, a, b, c, d, e, f, g, h, i, j, k, l, m, n)																		macro(a, index) APPLY_MACRO_TO_EACH_13(macro, index + 1, b, c, d, e, f, g, h, i, j, k, l, m, n)
#define APPLY_MACRO_TO_EACH_15(macro, index, a, b, c, d, e, f, g, h, i, j, k, l, m, n, o)																	macro(a, index) APPLY_MACRO_TO_EACH_14(macro, index + 1, b, c, d, e, f, g, h, i, j, k, l, m, n, o)
#define APPLY_MACRO_TO_EACH_16(macro, index, a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p)																macro(a, index) APPLY_MACRO_TO_EACH_15(macro, index + 1, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p)
#define APPLY_MACRO_TO_EACH_17(macro, index, a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q)																macro(a, index) APPLY_MACRO_TO_EACH_16(macro, index + 1, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q)
#define APPLY_MACRO_TO_EACH_18(macro, index, a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r)															macro(a, index) APPLY_MACRO_TO_EACH_17(macro, index + 1, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r)
#define APPLY_MACRO_TO_EACH_19(macro, index, a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s)														macro(a, index) APPLY_MACRO_TO_EACH_18(macro, index + 1, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s)
#define APPLY_MACRO_TO_EACH_20(macro, index, a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t)													macro(a, index) APPLY_MACRO_TO_EACH_19(macro, index + 1, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t)
#define APPLY_MACRO_TO_EACH_21(macro, index, a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u)													macro(a, index) APPLY_MACRO_TO_EACH_20(macro, index + 1, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u)
#define APPLY_MACRO_TO_EACH_22(macro, index, a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v)												macro(a, index) APPLY_MACRO_TO_EACH_21(macro, index + 1, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v)
#define APPLY_MACRO_TO_EACH_23(macro, index, a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, w)											macro(a, index) APPLY_MACRO_TO_EACH_22(macro, index + 1, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, w)
#define APPLY_MACRO_TO_EACH_24(macro, index, a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, w, x)										macro(a, index) APPLY_MACRO_TO_EACH_23(macro, index + 1, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, w, x)
#define APPLY_MACRO_TO_EACH_25(macro, index, a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, w, x, y)										macro(a, index) APPLY_MACRO_TO_EACH_24(macro, index + 1, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, w, x, y)
#define APPLY_MACRO_TO_EACH_26(macro, index, a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, w, x, y, z)									macro(a, index) APPLY_MACRO_TO_EACH_25(macro, index + 1, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, w, x, y, z)
#define APPLY_MACRO_TO_EACH_27(macro, index, a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, w, x, y, z, aa)								macro(a, index) APPLY_MACRO_TO_EACH_26(macro, index + 1, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, w, x, y, z, aa)
#define APPLY_MACRO_TO_EACH_28(macro, index, a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, w, x, y, z, aa, bb)							macro(a, index) APPLY_MACRO_TO_EACH_27(macro, index + 1, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, w, x, y, z, aa, bb)
#define APPLY_MACRO_TO_EACH_29(macro, index, a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, w, x, y, z, aa, bb, cc)						macro(a, index) APPLY_MACRO_TO_EACH_28(macro, index + 1, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, w, x, y, z, aa, bb, cc)
#define APPLY_MACRO_TO_EACH_30(macro, index, a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, w, x, y, z, aa, bb, cc, dd)					macro(a, index) APPLY_MACRO_TO_EACH_29(macro, index + 1, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, w, x, y, z, aa, bb, cc, dd)
#define APPLY_MACRO_TO_EACH_31(macro, index, a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, w, x, y, z, aa, bb, cc, dd, ee)				macro(a, index) APPLY_MACRO_TO_EACH_30(macro, index + 1, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, w, x, y, z, aa, bb, cc, dd, ee)
#define APPLY_MACRO_TO_EACH_32(macro, index, a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, w, x, y, z, aa, bb, cc, dd, ee, ff)			macro(a, index) APPLY_MACRO_TO_EACH_31(macro, index + 1, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, w, x, y, z, aa, bb, cc, dd, ee, ff)

// Dispatch to the correct APPLY_MACRO according to argument count.
#define GET_MACRO(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, _31, _32, NAME, ...) NAME
#define APPLY_MACRO_TO_EACH(macro, ...) GET_MACRO(__VA_ARGS__, APPLY_MACRO_TO_EACH_32, APPLY_MACRO_TO_EACH_31, APPLY_MACRO_TO_EACH_30, APPLY_MACRO_TO_EACH_29, APPLY_MACRO_TO_EACH_28, APPLY_MACRO_TO_EACH_27, APPLY_MACRO_TO_EACH_26, APPLY_MACRO_TO_EACH_25, APPLY_MACRO_TO_EACH_24, APPLY_MACRO_TO_EACH_23, APPLY_MACRO_TO_EACH_22, APPLY_MACRO_TO_EACH_21, APPLY_MACRO_TO_EACH_20, APPLY_MACRO_TO_EACH_19, APPLY_MACRO_TO_EACH_18, APPLY_MACRO_TO_EACH_17, APPLY_MACRO_TO_EACH_16, APPLY_MACRO_TO_EACH_15, APPLY_MACRO_TO_EACH_14, APPLY_MACRO_TO_EACH_13, APPLY_MACRO_TO_EACH_12, APPLY_MACRO_TO_EACH_11, APPLY_MACRO_TO_EACH_10, APPLY_MACRO_TO_EACH_9, APPLY_MACRO_TO_EACH_8, APPLY_MACRO_TO_EACH_7, APPLY_MACRO_TO_EACH_6, APPLY_MACRO_TO_EACH_5, APPLY_MACRO_TO_EACH_4, APPLY_MACRO_TO_EACH_3, APPLY_MACRO_TO_EACH_2, APPLY_MACRO_TO_EACH_1)(macro, 0, __VA_ARGS__)

/*!***********************************************************************
* @brief
*	The macro responsible for static reflection. It generates metadata for every data member.
*	Please take a look at the Reflection.hpp file level documentation if you want
*	to understand the implementation details behind reflection.
*
* @param [in] ...					: Variadic argument list, containing all of the data members you want to reflect.
*
**************************************************************************/
#define REFLECTABLE(...) \
static int constexpr _reflection_fields_n = COUNT_ARGS(__VA_ARGS__); \
friend reflection::query; \
static_assert(_reflection_fields_n < 32, "Reflection does not support more than 32 data members."); \
template<int N, typename Object> \
struct FieldData {}; \
APPLY_MACRO_TO_EACH(REFLECT_EACH, __VA_ARGS__) \

#define REFLECT_EACH(dataMember, index) \
\
template<typename Object> \
struct FieldData<index, Object> \
{ \
	using type = std::remove_cvref_t<decltype(std::decay_t<Object>::dataMember)>; \
\
    Object&& self; \
\
    constexpr FieldData(Object&& self) : self(self) {} \
    \
    constexpr decltype(std::remove_cvref_t<Object>::dataMember)& get() const requires(std::is_const_v<Object>) \
    {\
        return self.dataMember; \
    }\
    constexpr decltype(std::remove_cvref_t<Object>::dataMember) const& get() const requires(!std::is_const_v<Object>) \
    {\
        return self.dataMember; \
    }\
    constexpr char const* name() const \
    {\
        return #dataMember; \
    }\
	constexpr decltype(auto) getPointerToMember() { \
		return &std::decay_t<Object>::dataMember;	\
	}\
}; \

namespace reflection {
	// class that has access to reflected private data members
	struct query {
		template <typename T>
		constexpr static bool reflectable() {
			return requires {
				std::remove_cvref_t<T>::_reflection_fields_n;
			};
		}

		template <typename T>
		constexpr static auto getNumberOfFields() {
			return std::remove_cvref_t<T>::_reflection_fields_n;
		}

		// Get a specific FieldData of object type T, index N.
		template<int N, typename T>
		static auto getFieldData(T&& object) {
			return typename std::remove_cvref_t<T>::template FieldData<N, T>(std::forward<T>(object));
		}
	};

	template <typename T>
	constexpr bool isReflectable() {
		return query::reflectable<T>();
	}

	template <typename T>
	constexpr auto getNumberOfFields() {
		return query::getNumberOfFields<T>();
	}

	template <typename T, typename Functor, std::size_t... ints>
	void forEach(T&& x, Functor callback, std::integer_sequence<std::size_t, ints...>) {
		(callback(query::getFieldData<ints>(std::forward<T>(x))), ...);	// Using the comma operator fold expression to invoke callback function for each field data.
	}

	template<typename T, typename Functor>
	void iterateThroughMember(T&& x, Functor func) {
		forEach(std::forward<T>(x), func, std::make_integer_sequence<std::size_t, query::getNumberOfFields<T>()>());
	}

	template<typename Functor, typename T>
	void visit(Functor&& func, T&& x) {
		visit(std::forward<Functor>(func), []{}, []{}, std::forward<T>(x));
	}

	template<typename Functor, typename Functor2, typename Functor3, typename T>
	void visit(Functor&& func, Functor2&& enterFunc, Functor3&& exitFunc, T&& x) {
		static_assert(isReflectable<T>(), "Class provided is not reflectable! Did you forget to provide the REFLECTABLE macro?");

		iterateThroughMember(std::forward<T>(x), [&]<typename U>(U&& fieldData) {
			if constexpr (isReflectable<typename std::remove_cvref_t<U>::type>()) {
				_internal_visit(std::forward<Functor>(func), std::forward<Functor2>(enterFunc), std::forward<Functor3>(exitFunc), std::forward<U>(fieldData).get());
			}
			else {
				func(std::forward<U>(fieldData));
			}
		});
	}

	template<typename Functor, typename Functor2, typename Functor3, typename T>
	void _internal_visit(Functor&& func, Functor2&& enterFunc, Functor3&& exitFunc, T&& x) {
		enterFunc();

		iterateThroughMember(std::forward<T>(x), [&]<typename U>(U && fieldData) {
			if constexpr (isReflectable<typename std::remove_cvref_t<U>::type>()) {
				_internal_visit(std::forward<Functor>(func), std::forward<Functor2>(enterFunc), std::forward<Functor3>(exitFunc), std::forward<U>(fieldData).get());
			}
			else {
				func(std::forward<U>(fieldData));
			}
		});

		exitFunc();
	}

	template <typename T>
	void print(T&& x) {
		static_assert(isReflectable<T>(), "Class provided is not reflectable! Did you forget to provide the REFLECTABLE macro?");

		visit([](auto fieldData) {
				
		}, std::forward<T>(x));
	}

	template <typename T>
	void prettyPrint(T&& x) {
		static_assert(isReflectable<T>(), "Class provided is not reflectable! Did you forget to provide the REFLECTABLE macro?");

		int recursionDepth = 0;

		std::cout << "{\n";

		visit(
			[&recursionDepth](auto fieldData) {
				for (int i = 0; i < recursionDepth + 1; ++i) {
					std::cout << "    ";
				}

				std::cout << fieldData.name() << " = " << fieldData.get() << ",\n";
			}, 
			[&recursionDepth]() {
				++recursionDepth;

				for (int i = 0; i < recursionDepth; ++i) {
					std::cout << "    ";
				}
				std::cout << "{\n";

			},
			[&recursionDepth]() {
				for (int i = 0; i < recursionDepth; ++i) {
					std::cout << "    ";
				}
				std::cout << "},\n";

				--recursionDepth;
			},
			
			std::forward<T>(x)
		);

		std::cout << "}\n";
	}
}

#endif
#endif // CPP_REFLECTION_H