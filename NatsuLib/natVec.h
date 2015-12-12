////////////////////////////////////////////////////////////////////////////////
///	@file	natVec.h
///	@brief	NatsuLib�������
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "natType.h"
#include "natException.h"
#include <type_traits>

////////////////////////////////////////////////////////////////////////////////
///	@addtogroup	NatsuLib��ѧ
///	@brief		NatsuLib��ѧ����
///	@{

#define OPERATORSCALAR(op) template <typename U>\
	natVec2& operator##op(U const& Scalar)\
	{\
		x op static_cast<T>(Scalar);\
		y op static_cast<T>(Scalar);\
		return *this;\
	}

#define OPERATORSELF(op) template <typename U>\
	natVec2& operator##op(natVec2<U> const& v)\
	{\
		x op static_cast<T>(v.x);\
		y op static_cast<T>(v.y);\
		return *this;\
	}

#define TOPERATORSCALAR(op) template <typename U>\
	natVec2 operator##op(U const& Scalar) const\
	{\
		return natVec2(x op static_cast<T>(Scalar), y op static_cast<T>(Scalar));\
	}

#define TOPERATORSELF(op) template <typename U>\
	natVec2 operator##op(natVec2<U> const& v) const\
	{\
		return natVec2(x op static_cast<T>(v.x), y op static_cast<T>(v.y));\
	}

#define TOPERATORSCALARNM(op) template <typename T, typename U>\
natVec2<T> operator##op(U const& Scalar, natVec2<T> const& v)\
{\
	return natVec2<T>(static_cast<T>(Scalar) op v.x, static_cast<T>(Scalar) op v.y);\
}

struct natVec {};

////////////////////////////////////////////////////////////////////////////////
///	@brief	2ά����
////////////////////////////////////////////////////////////////////////////////
template <typename T = nFloat>
struct natVec2 final
	: natVec
{
	typedef T type;

	union
	{
		struct
		{
			T x, y;
		};
		struct
		{
			T r, g;
		};
	};

	constexpr nuInt length() const
	{
		return 2u;
	}

	T const& operator[](nuInt i) const
	{
		if (i >= length())
		{
			throw natException(_T("natVec2::operator[]"), _T("Out of range"));
		}

		return (&x)[i];
	}

	T& operator[](nuInt i)
	{
		return const_cast<T&>(static_cast<natVec2 const*>(this)->operator[](i));
	}

	template <typename R>
	natVec2<R> call(R(*func)(T)) const
	{
		return natVec2<R>(func(x), func(y));
	}

	natVec2()
		: x(0), y(0)
	{
		static_assert(std::is_arithmetic<T>::value || std::is_class<T>::value, "T should be an arithmetic type or a class.");
	}

	explicit natVec2(T const& Scalar)
		: x(Scalar), y(Scalar)
	{
		static_assert(std::is_arithmetic<T>::value || std::is_class<T>::value, "T should be an arithmetic type or a class.");
	}

	natVec2(T const& S1, T const& S2)
		: x(S1), y(S2)
	{
		static_assert(std::is_arithmetic<T>::value || std::is_class<T>::value, "T should be an arithmetic type or a class.");
	}

	template <typename U>
	explicit natVec2(const U* S)
		: x(static_cast<T>(S[0])), y(static_cast<T>(S[1]))
	{
		static_assert(std::is_arithmetic<T>::value || std::is_class<T>::value, "T should be an arithmetic type or a class.");
	}

	template <typename U>
	explicit natVec2(natVec2<U> && v)
		: x(static_cast<T&&>(v.x)), y(static_cast<T&&>(v.y))
	{
		static_assert(std::is_arithmetic<T>::value || std::is_class<T>::value, "T should be an arithmetic type or a class.");
	}

	template <typename U>
	explicit natVec2(natVec2<U> const& v)
		: x(static_cast<T>(v.x)), y(static_cast<T>(v.y))
	{
		static_assert(std::is_arithmetic<T>::value || std::is_class<T>::value, "T should be an arithmetic type or a class.");
	}

	natVec2(natVec2 const& v) = default;
	natVec2(natVec2 && v) = default;

	natVec2& operator=(natVec2 const& v) = default;
	natVec2& operator=(natVec2 && v) = default;

	template <typename U>
	natVec2& operator=(natVec2<U> && v)
	{
		x = static_cast<T&&>(v.x);
		y = static_cast<T&&>(v.y);

		return *this;
	}

	natVec2& operator++()
	{
		++x; ++y;
		return *this;
	}

	natVec2& operator--()
	{
		--x; --y;
		return *this;
	}

	natVec2 operator++(int)
	{
		natVec2 tResult(*this);
		++*this;
		return tResult;
	}

	natVec2 operator--(int)
	{
		natVec2 tResult(*this);
		--*this;
		return tResult;
	}

	natVec2 operator-() const
	{
		return natVec2(-x, -y);
	}

	natVec2 operator~() const
	{
		return natVec2(~x, ~y);
	}

	template <typename U>
	nBool operator==(natVec2<U> const& v) const
	{
		return (x == v.x) && (y == v.y);
	}

	template <typename U>
	nBool operator!=(natVec2<U> const& v) const
	{
		return !(*this == v);
	}

	OPERATORSCALAR(= );
	OPERATORSELF(= );

	OPERATORSCALAR(+= );
	OPERATORSELF(+= );

	OPERATORSCALAR(-= );
	OPERATORSELF(-= );

	OPERATORSCALAR(*= );
	OPERATORSELF(*= );

	OPERATORSCALAR(/= );
	OPERATORSELF(/= );

	OPERATORSCALAR(%= );
	OPERATORSELF(%= );

	OPERATORSCALAR(&= );
	OPERATORSELF(&= );

	OPERATORSCALAR(|= );
	OPERATORSELF(|= );

	OPERATORSCALAR(^= );
	OPERATORSELF(^= );

	OPERATORSCALAR(<<= );
	OPERATORSELF(<<= );

	OPERATORSCALAR(>>= );
	OPERATORSELF(>>= );

	TOPERATORSCALAR(+);
	TOPERATORSELF(+);

	TOPERATORSCALAR(-);
	TOPERATORSELF(-);

	TOPERATORSCALAR(*);
	TOPERATORSELF(*);

	TOPERATORSCALAR(/ );
	TOPERATORSELF(/ );

	TOPERATORSCALAR(%);
	TOPERATORSELF(%);

	TOPERATORSCALAR(&);
	TOPERATORSELF(&);

	TOPERATORSCALAR(| );
	TOPERATORSELF(| );

	TOPERATORSCALAR(^);
	TOPERATORSELF(^);

	TOPERATORSCALAR(<< );
	TOPERATORSELF(<< );

	TOPERATORSCALAR(>> );
	TOPERATORSELF(>> );
};

TOPERATORSCALARNM(+);

TOPERATORSCALARNM(-);

TOPERATORSCALARNM(*);

TOPERATORSCALARNM(/ );

TOPERATORSCALARNM(%);

TOPERATORSCALARNM(&);

TOPERATORSCALARNM(| );

TOPERATORSCALARNM(^);

TOPERATORSCALARNM(<< );

TOPERATORSCALARNM(>> );

#ifdef OPERATORSCALAR
#	undef OPERATORSCALAR
#endif

#ifdef OPERATORSELF
#	undef OPERATORSELF
#endif

#ifdef TOPERATORSCALAR
#	undef TOPERATORSCALAR
#endif

#ifdef TOPERATORSELF
#	undef TOPERATORSELF
#endif

#ifdef TOPERATORSCALARNM
#	undef TOPERATORSCALARNM
#endif

#define OPERATORSCALAR(op) template <typename U>\
	natVec3& operator##op(U const& Scalar)\
	{\
		x op static_cast<T>(Scalar);\
		y op static_cast<T>(Scalar);\
		z op static_cast<T>(Scalar);\
		return *this;\
	}

#define OPERATORSELF(op) template <typename U>\
	natVec3& operator##op(natVec3<U> const& v)\
	{\
		x op static_cast<T>(v.x);\
		y op static_cast<T>(v.y);\
		z op static_cast<T>(v.z);\
		return *this;\
	}

#define TOPERATORSCALAR(op) template <typename U>\
	natVec3 operator##op(U const& Scalar) const\
	{\
		return natVec3(x op static_cast<T>(Scalar), y op static_cast<T>(Scalar), z op static_cast<T>(Scalar));\
	}

#define TOPERATORSELF(op) template <typename U>\
	natVec3 operator##op(natVec3<U> const& v) const\
	{\
		return natVec3(x op static_cast<T>(v.x), y op static_cast<T>(v.y), z op static_cast<T>(v.z));\
	}

#define TOPERATORSCALARNM(op) template <typename T, typename U>\
natVec3<T> operator##op(U const& Scalar, natVec3<T> const& v)\
{\
	return natVec3<T>(static_cast<T>(Scalar) op v.x, static_cast<T>(Scalar) op v.y, static_cast<T>(Scalar) op v.z);\
}

////////////////////////////////////////////////////////////////////////////////
///	@brief	3ά����
////////////////////////////////////////////////////////////////////////////////
template <typename T = nFloat>
struct natVec3 final
	: natVec
{
	typedef T type;

	union
	{
		struct
		{
			T x, y, z;
		};
		struct
		{
			T r, g, b;
		};
	};

	constexpr nuInt length() const
	{
		return 3u;
	}

	T const& operator[](nuInt i) const
	{
		if (i >= length())
		{
			throw natException(_T("natVec3::operator[]"), _T("Out of range"));
		}

		return *(&x + i);
	}

	T& operator[](nuInt i)
	{
		return const_cast<T&>(static_cast<natVec3 const*>(this)->operator[](i));
	}

	template <typename R>
	natVec3<R> call(R(*func)(T)) const
	{
		return natVec3<R>(func(x), func(y), func(z));
	}

	natVec3()
		: x(0), y(0), z(0)
	{
		static_assert(std::is_arithmetic<T>::value || std::is_class<T>::value, "T should be an arithmetic type or a class.");
	}

	explicit natVec3(T const& Scalar)
		: x(Scalar), y(Scalar), z(Scalar)
	{
		static_assert(std::is_arithmetic<T>::value || std::is_class<T>::value, "T should be an arithmetic type or a class.");
	}

	natVec3(T const& s1, T const& s2, T const& s3)
		: x(s1), y(s2), z(s3)
	{
		static_assert(std::is_arithmetic<T>::value || std::is_class<T>::value, "T should be an arithmetic type or a class.");
	}

	template <typename U>
	explicit natVec3(const U* S)
		: x(static_cast<T>(S[0])), y(static_cast<T>(S[1])), z(static_cast<T>(S[2]))
	{
		static_assert(std::is_arithmetic<T>::value || std::is_class<T>::value, "T should be an arithmetic type or a class.");
	}

	template <typename U>
	explicit natVec3(natVec3<U> && v)
		: x(static_cast<T&&>(v.x)), y(static_cast<T&&>(v.y)), z(static_cast<T&&>(v.z))
	{
		static_assert(std::is_arithmetic<T>::value || std::is_class<T>::value, "T should be an arithmetic type or a class.");
	}

	template <typename U>
	explicit natVec3(natVec3<U> const& v)
		: x(static_cast<T>(v.x)), y(static_cast<T>(v.y)), z(static_cast<T>(v.z))
	{
		static_assert(std::is_arithmetic<T>::value || std::is_class<T>::value, "T should be an arithmetic type or a class.");
	}

	natVec3(natVec3 const& v) = default;
	natVec3(natVec3 && v) = default;

	natVec3& operator=(natVec3 const& v) = default;
	natVec3& operator=(natVec3 && v) = default;

	template <typename U>
	natVec3& operator=(natVec3<U> && v)
	{
		x = static_cast<T&&>(v.x);
		y = static_cast<T&&>(v.y);
		z = static_cast<T&&>(v.z);

		return *this;
	}

	natVec3& operator++()
	{
		++x; ++y; ++z;
		return *this;
	}

	natVec3& operator--()
	{
		--x; --y; --z;
		return *this;
	}

	natVec3 operator++(int)
	{
		natVec3 tResult(*this);
		++*this;
		return tResult;
	}

	natVec3 operator--(int)
	{
		natVec3 tResult(*this);
		--*this;
		return tResult;
	}

	natVec3 operator-() const
	{
		return natVec3(-x, -y, -z);
	}

	natVec3 operator~() const
	{
		return natVec3(~x, ~y, ~z);
	}

	template <typename U>
	nBool operator==(natVec3<U> const& v) const
	{
		return (x == v.x) && (y == v.y) && (z == v.z);
	}

	template <typename U>
	nBool operator!=(natVec3<U> const& v) const
	{
		return !(*this == v);
	}

	OPERATORSCALAR(= );
	OPERATORSELF(= );

	OPERATORSCALAR(+= );
	OPERATORSELF(+= );

	OPERATORSCALAR(-= );
	OPERATORSELF(-= );

	OPERATORSCALAR(*= );
	OPERATORSELF(*= );

	OPERATORSCALAR(/= );
	OPERATORSELF(/= );

	OPERATORSCALAR(%= );
	OPERATORSELF(%= );

	OPERATORSCALAR(&= );
	OPERATORSELF(&= );

	OPERATORSCALAR(|= );
	OPERATORSELF(|= );

	OPERATORSCALAR(^= );
	OPERATORSELF(^= );

	OPERATORSCALAR(<<= );
	OPERATORSELF(<<= );

	OPERATORSCALAR(>>= );
	OPERATORSELF(>>= );

	TOPERATORSCALAR(+);
	TOPERATORSELF(+);

	TOPERATORSCALAR(-);
	TOPERATORSELF(-);

	TOPERATORSCALAR(*);
	TOPERATORSELF(*);

	TOPERATORSCALAR(/ );
	TOPERATORSELF(/ );

	TOPERATORSCALAR(%);
	TOPERATORSELF(%);

	TOPERATORSCALAR(&);
	TOPERATORSELF(&);

	TOPERATORSCALAR(| );
	TOPERATORSELF(| );

	TOPERATORSCALAR(^);
	TOPERATORSELF(^);

	TOPERATORSCALAR(<< );
	TOPERATORSELF(<< );

	TOPERATORSCALAR(>> );
	TOPERATORSELF(>> );
};

TOPERATORSCALARNM(+);

TOPERATORSCALARNM(-);

TOPERATORSCALARNM(*);

TOPERATORSCALARNM(/ );

TOPERATORSCALARNM(%);

TOPERATORSCALARNM(&);

TOPERATORSCALARNM(| );

TOPERATORSCALARNM(^);

TOPERATORSCALARNM(<< );

TOPERATORSCALARNM(>> );

#ifdef OPERATORSCALAR
#	undef OPERATORSCALAR
#endif

#ifdef OPERATORSELF
#	undef OPERATORSELF
#endif

#ifdef TOPERATORSCALAR
#	undef TOPERATORSCALAR
#endif

#ifdef TOPERATORSELF
#	undef TOPERATORSELF
#endif

#ifdef TOPERATORSCALARNM
#	undef TOPERATORSCALARNM
#endif

#define OPERATORSCALAR(op) template <typename U>\
	natVec4& operator##op(U const& Scalar)\
	{\
		x op static_cast<T>(Scalar);\
		y op static_cast<T>(Scalar);\
		z op static_cast<T>(Scalar);\
		w op static_cast<T>(Scalar);\
		return *this;\
	}

#define OPERATORSELF(op) template <typename U>\
	natVec4& operator##op(natVec4<U> const& v)\
	{\
		x op static_cast<T>(v.x);\
		y op static_cast<T>(v.y);\
		z op static_cast<T>(v.z);\
		w op static_cast<T>(v.w);\
		return *this;\
	}

#define TOPERATORSCALAR(op) template <typename U>\
	natVec4 operator##op(U const& Scalar) const\
	{\
		return natVec4(x op static_cast<T>(Scalar), y op static_cast<T>(Scalar), z op static_cast<T>(Scalar), w op static_cast<T>(Scalar));\
	}

#define TOPERATORSELF(op) template <typename U>\
	natVec4 operator##op(natVec4<U> const& v) const\
	{\
		return natVec4(x op static_cast<T>(v.x), y op static_cast<T>(v.y), z op static_cast<T>(v.z), w op static_cast<T>(v.w));\
	}

#define TOPERATORSCALARNM(op) template <typename T, typename U>\
natVec4<T> operator##op(U const& Scalar, natVec4<T> const& v)\
{\
	return natVec4<T>(static_cast<T>(Scalar) op v.x, static_cast<T>(Scalar) op v.y, static_cast<T>(Scalar) op v.z, static_cast<T>(Scalar) op v.w);\
}

////////////////////////////////////////////////////////////////////////////////
///	@brief	4ά����
////////////////////////////////////////////////////////////////////////////////
template <typename T = nFloat>
struct natVec4 final
	: natVec
{
	typedef T type;

	union
	{
		struct
		{
			T x, y, z, w;
		};
		struct
		{
			T r, g, b, a;
		};
	};

	constexpr nuInt length() const
	{
		return 4u;
	}

	T const& operator[](nuInt i) const
	{
		if (i >= length())
		{
			throw natException(_T("natVec4::operator[]"), _T("Out of range"));
		}

		return (&x)[i];
	}

	T& operator[](nuInt i)
	{
		return const_cast<T&>(static_cast<natVec4 const*>(this)->operator[](i));
	}

	template <typename R>
	natVec4<R> call(R(*func)(T)) const
	{
		return natVec4<R>(func(x), func(y), func(z), func(w));
	}

	natVec4()
		: x(0), y(0), z(0), w(0)
	{
		static_assert(std::is_arithmetic<T>::value || std::is_class<T>::value, "T should be an arithmetic type or a class.");
	}

	explicit natVec4(T const& Scalar)
		: x(Scalar), y(Scalar), z(Scalar), w(Scalar)
	{
		static_assert(std::is_arithmetic<T>::value || std::is_class<T>::value, "T should be an arithmetic type or a class.");
	}

	natVec4(T const& s1, T const& s2, T const& s3, T const& s4)
		: x(s1), y(s2), z(s3), w(s4)
	{
		static_assert(std::is_arithmetic<T>::value || std::is_class<T>::value, "T should be an arithmetic type or a class.");
	}

	template <typename U>
	explicit natVec4(const U* S)
		: x(static_cast<T>(S[0])), y(static_cast<T>(S[1])), z(static_cast<T>(S[2])), w(static_cast<T>(S[3]))
	{
		static_assert(std::is_arithmetic<T>::value || std::is_class<T>::value, "T should be an arithmetic type or a class.");
	}

	template <typename U>
	explicit natVec4(natVec4<U> && v)
		: x(static_cast<T&&>(v.x)), y(static_cast<T&&>(v.y)), z(static_cast<T&&>(v.z)), w(static_cast<T&&>(v.w))
	{
		static_assert(std::is_arithmetic<T>::value || std::is_class<T>::value, "T should be an arithmetic type or a class.");
	}

	template <typename U>
	explicit natVec4(natVec4<U> const& v)
		: x(static_cast<T>(v.x)), y(static_cast<T>(v.y)), z(static_cast<T>(v.z)), w(static_cast<T>(v.w))
	{
		static_assert(std::is_arithmetic<T>::value || std::is_class<T>::value, "T should be an arithmetic type or a class.");
	}

	natVec4(natVec4 const& v) = default;
	natVec4(natVec4 && v) = default;

	natVec4& operator=(natVec4 const& v) = default;
	natVec4& operator=(natVec4 && v) = default;

	template <typename U>
	natVec4& operator=(natVec4<U> && v)
	{
		x = static_cast<T&&>(v.x);
		y = static_cast<T&&>(v.y);
		z = static_cast<T&&>(v.z);
		w = static_cast<T&&>(v.w);

		return *this;
	}

	natVec4& operator++()
	{
		++x; ++y; ++z; ++w;
		return *this;
	}

	natVec4& operator--()
	{
		--x; --y; --z; --w;
		return *this;
	}

	natVec4 operator++(int)
	{
		natVec4 tResult(*this);
		++*this;
		return tResult;
	}

	natVec4 operator--(int)
	{
		natVec4 tResult(*this);
		--*this;
		return tResult;
	}

	natVec4 operator-() const
	{
		return natVec4(-x, -y, -z, -w);
	}

	natVec4 operator~() const
	{
		return natVec4(~x, ~y, ~z, ~w);
	}

	template <typename U>
	nBool operator==(natVec4<U> const& v) const
	{
		return (x == v.x) && (y == v.y) && (z == v.z) && (w == v.w);
	}

	template <typename U>
	nBool operator!=(natVec4<U> const& v) const
	{
		return !(*this == v);
	}

	OPERATORSCALAR(= );
	OPERATORSELF(= );

	OPERATORSCALAR(+= );
	OPERATORSELF(+= );

	OPERATORSCALAR(-= );
	OPERATORSELF(-= );

	OPERATORSCALAR(*= );
	OPERATORSELF(*= );

	OPERATORSCALAR(/= );
	OPERATORSELF(/= );

	OPERATORSCALAR(%= );
	OPERATORSELF(%= );

	OPERATORSCALAR(&= );
	OPERATORSELF(&= );

	OPERATORSCALAR(|= );
	OPERATORSELF(|= );

	OPERATORSCALAR(^= );
	OPERATORSELF(^= );

	OPERATORSCALAR(<<= );
	OPERATORSELF(<<= );

	OPERATORSCALAR(>>= );
	OPERATORSELF(>>= );

	TOPERATORSCALAR(+);
	TOPERATORSELF(+);

	TOPERATORSCALAR(-);
	TOPERATORSELF(-);

	TOPERATORSCALAR(*);
	TOPERATORSELF(*);

	TOPERATORSCALAR(/ );
	TOPERATORSELF(/ );

	TOPERATORSCALAR(%);
	TOPERATORSELF(%);

	TOPERATORSCALAR(&);
	TOPERATORSELF(&);

	TOPERATORSCALAR(| );
	TOPERATORSELF(| );

	TOPERATORSCALAR(^);
	TOPERATORSELF(^);

	TOPERATORSCALAR(<< );
	TOPERATORSELF(<< );

	TOPERATORSCALAR(>> );
	TOPERATORSELF(>> );
};

TOPERATORSCALARNM(+);

TOPERATORSCALARNM(-);

TOPERATORSCALARNM(*);

TOPERATORSCALARNM(/ );

TOPERATORSCALARNM(%);

TOPERATORSCALARNM(&);

TOPERATORSCALARNM(| );

TOPERATORSCALARNM(^);

TOPERATORSCALARNM(<< );

TOPERATORSCALARNM(>> );

#ifdef OPERATORSCALAR
#	undef OPERATORSCALAR
#endif

#ifdef OPERATORSELF
#	undef OPERATORSELF
#endif

#ifdef TOPERATORSCALAR
#	undef TOPERATORSCALAR
#endif

#ifdef TOPERATORSELF
#	undef TOPERATORSELF
#endif

#ifdef TOPERATORSCALARNM
#	undef TOPERATORSCALARNM
#endif

///	@}