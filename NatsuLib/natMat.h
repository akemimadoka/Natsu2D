////////////////////////////////////////////////////////////////////////////////
///	@file	natMat.h
///	@brief	NatsuLib�������
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "natType.h"
#include "natException.h"
#include <type_traits>

#include "natVec.h"

////////////////////////////////////////////////////////////////////////////////
///	@addtogroup	NatsuLib��ѧ
///	@brief		NatsuLib��ѧ����
///	@{

#define OPERATORSCALAR(op) template <typename U>\
	natMat2& operator##op(U const& Scalar)\
	{\
		value[0] op static_cast<T>(Scalar);\
		value[1] op static_cast<T>(Scalar);\
		return *this;\
	}

#define OPERATORSELF(op) template <typename U>\
	natMat2& operator##op(natMat2<U> const& m)\
	{\
		value[0] op static_cast<col_type>(m.value[0]);\
		value[1] op static_cast<col_type>(m.value[1]);\
		return *this;\
	}

#define TOPERATORSCALAR(op) natMat2 operator##op(T const& Scalar) const\
	{\
		return natMat2(value[0] op Scalar, value[1] op Scalar);\
	}

#define TOPERATORSELF(op) template <typename U>\
	natMat2 operator##op(natMat2<U> const& m) const\
	{\
		return natMat2(value[0] op static_cast<col_type>(m.value[0]), value[1] op static_cast<col_type>(m.value[1]));\
	}

#define TOPERATORSCALARNM(op) template <typename T, typename U>\
natMat2<T> operator##op(U const& Scalar, natMat2<T> const& m)\
{\
	return natMat2<T>(static_cast<T>(Scalar) op m[0], static_cast<T>(Scalar) op m[1]);\
}

////////////////////////////////////////////////////////////////////////////////
///	@brief	2�׾���
////////////////////////////////////////////////////////////////////////////////
template <typename T = nFloat>
struct natMat2
{
	typedef natVec2<T> col_type;
	typedef natVec2<T> row_type;

private:
	col_type value[2];

public:
	constexpr nuInt length() const
	{
		return 2u;
	}

	col_type const& operator[](nuInt i) const
	{
		if (i >= length())
		{
			throw natException(_T("natMat2::operator[]"), _T("Out of range"));
		}

		return value[i];
	}

	col_type& operator[](nuInt i)
	{
		return const_cast<col_type&>(static_cast<natMat2 const*>(this)->operator[](i));
	}

	natMat2()
		: natMat2(T(1))
	{
	}

	explicit natMat2(T const& Scalar)
	{
		static_assert(std::is_arithmetic<T>::value || std::is_class<T>::value, "T should be an arithmetic type or a class.");
		value[0] = col_type(Scalar, 0);
		value[1] = col_type(0, Scalar);
	}

	template <typename U>
	explicit natMat2(natMat2<U> const& m)
	{
		static_assert(std::is_arithmetic<T>::value || std::is_class<T>::value, "T should be an arithmetic type or a class.");
		value[0] = static_cast<col_type>(m.value[0]);
		value[1] = static_cast<col_type>(m.value[1]);
	}

	template <typename U>
	explicit natMat2(natMat2<U> && m)
	{
		static_assert(std::is_arithmetic<T>::value || std::is_class<T>::value, "T should be an arithmetic type or a class.");
		value[0] = static_cast<col_type &&>(m.value[0]);
		value[1] = static_cast<col_type &&>(m.value[1]);
	}

	natMat2(
		T const& x1, T const& y1,
		T const& x2, T const& y2)
	{
		static_assert(std::is_arithmetic<T>::value || std::is_class<T>::value, "T should be an arithmetic type or a class.");
		value[0] = col_type(x1, y1);
		value[1] = col_type(x2, y2);
	}

	explicit natMat2(
		natVec2<T> const& v)
	{
		static_assert(std::is_arithmetic<T>::value || std::is_class<T>::value, "T should be an arithmetic type or a class.");
		value[0] = col_type(v.x, 0);
		value[1] = col_type(0, v.y);
	}

	natMat2(
		col_type const& v1,
		col_type const& v2)
	{
		static_assert(std::is_arithmetic<T>::value || std::is_class<T>::value, "T should be an arithmetic type or a class.");
		value[0] = v1;
		value[1] = v2;
	}

	natMat2 inverse() const
	{
		T OneOverDeterminant = static_cast<T>(1) / (
			+value[0][0] * value[1][1]
			- value[1][0] * value[0][1]);

		return natMat2(
			+value[1][1] * OneOverDeterminant,
			-value[0][1] * OneOverDeterminant,
			-value[1][0] * OneOverDeterminant,
			+value[0][0] * OneOverDeterminant);
	}

	natMat2(natMat2 const& m) = default;
	natMat2(natMat2 && m) = default;

	natMat2& operator=(natMat2 const& m) = default;
	natMat2& operator=(natMat2 && m) = default;

	template <typename U>
	natMat2& operator=(natMat2<U> && m)
	{
		value[0] = static_cast<col_type &&>(m.value[0]);
		value[1] = static_cast<col_type &&>(m.value[1]);

		return *this;
	}

	natMat2& operator++()
	{
		++value[0]; ++value[1];

		return *this;
	}

	natMat2& operator--()
	{
		--value[0]; --value[1];

		return *this;
	}

	natMat2 operator++(int)
	{
		natMat2 tResult(*this);
		++*this;

		return tResult;
	}

	natMat2 operator--(int)
	{
		natMat2 tResult(*this);
		--*this;

		return tResult;
	}

	natMat2 operator-()
	{
		return natMat2(-value[0], -value[1]);
	}

	template <typename U>
	nBool operator==(natMat2<U> const& m) const
	{
		return (value[0] == m.value[0]) && (value[1] == m.value[1]);
	}

	template <typename U>
	nBool operator!=(natMat2<U> const& m) const
	{
		return !(*this == m);
	}

	OPERATORSCALAR(= );
	OPERATORSELF(= );

	OPERATORSCALAR(+= );
	OPERATORSELF(+= );

	OPERATORSCALAR(-= );
	OPERATORSELF(-= );

	OPERATORSCALAR(*= );

	OPERATORSCALAR(/= );

	template <typename U>
	natMat2& operator*=(natMat2<U> const& m)
	{
		return (*this = *this * m);
	}

	template <typename U>
	natMat2& operator/=(natMat2<U> const& m)
	{
		return (*this = *this * m.inverse());
	}

	TOPERATORSCALAR(+);
	TOPERATORSELF(+);

	TOPERATORSCALAR(-);
	TOPERATORSELF(-);

	TOPERATORSCALAR(*);
	TOPERATORSCALAR(/ );

	template <typename U>
	natMat2 operator*(natMat2<U> const& m) const
	{
		return natMat2(
			value[0][0] * static_cast<T>(m.value[0][0]) + value[1][0] * static_cast<T>(m.value[0][1]), value[0][1] * static_cast<T>(m.value[0][0]) + value[1][1] * static_cast<T>(m.value[0][1]),
			value[0][0] * static_cast<T>(m.value[1][0]) + value[1][0] * static_cast<T>(m.value[1][1]), value[0][1] * static_cast<T>(m.value[0][0]) + value[1][1] * static_cast<T>(m.value[1][1]));
	}

	template <typename U>
	natMat2 operator/(natMat2<U> const& m) const
	{
		return natMat2(*this) /= m;
	}
};

TOPERATORSCALARNM(+);

TOPERATORSCALARNM(-);

TOPERATORSCALARNM(*);

TOPERATORSCALARNM(/ );

template <typename T, typename U>
natVec2<T> operator*(natMat2<T> const& m, natVec2<U> const& v)
{
	return natVec2<T>(m[0][0] * v.x + m[1][0] * v.y, m[0][1] * v.x + m[1][1] * v.y);
}

template <typename T, typename U>
natVec2<U> operator*(natVec2<U> const& v, natMat2<T> const& m)
{
	return natVec2<T>(v.x * m[0][0] + v.y * m[0][1], v.x * m[1][0] + v.y * m[1][1]);
}

template <typename T, typename U>
natVec2<T> operator/(natMat2<T> const& m, natVec2<U> const& v)
{
	return m.inverse() * v;
}

template <typename T, typename U>
natVec2<T> operator/(natVec2<U> const& v, natMat2<T> const& m)
{
	return v * m.inverse();
}

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
	natMat3& operator##op(U const& Scalar)\
	{\
		value[0] op static_cast<T>(Scalar);\
		value[1] op static_cast<T>(Scalar);\
		value[2] op static_cast<T>(Scalar);\
		return *this;\
	}

#define OPERATORSELF(op) template <typename U>\
	natMat3& operator##op(natMat3<U> const& m)\
	{\
		value[0] op static_cast<col_type>(m.value[0]);\
		value[1] op static_cast<col_type>(m.value[1]);\
		value[2] op static_cast<col_type>(m.value[2]);\
		return *this;\
	}

#define TOPERATORSCALAR(op) natMat3 operator##op(T const& Scalar) const\
	{\
		return natMat3(value[0] op Scalar, value[1] op Scalar, value[2] op Scalar);\
	}

#define TOPERATORSELF(op) template <typename U>\
	natMat3 operator##op(natMat3<U> const& m) const\
	{\
		return natMat3(value[0] op static_cast<col_type>(m.value[0]), value[1] op static_cast<col_type>(m.value[1]), value[2] op static_cast<col_type>(m.value[2]));\
	}

#define TOPERATORSCALARNM(op) template <typename T, typename U>\
natMat3<T> operator##op(U const& Scalar, natMat3<T> const& m)\
{\
	return natMat3<T>(static_cast<T>(Scalar) op m[0], static_cast<T>(Scalar) op m[1], static_cast<T>(Scalar) op m[2]);\
}

////////////////////////////////////////////////////////////////////////////////
///	@brief	3�׾���
////////////////////////////////////////////////////////////////////////////////
template <typename T = nFloat>
struct natMat3
{
	typedef natVec3<T> col_type;
	typedef natVec3<T> row_type;

private:
	col_type value[3];

public:
	constexpr nuInt length() const
	{
		return 3u;
	}

	col_type const& operator[](nuInt i) const
	{
		if (i >= length())
		{
			throw natException(_T("natMat3::operator[]"), _T("Out of range"));
		}

		return value[i];
	}

	col_type& operator[](nuInt i)
	{
		return const_cast<col_type&>(static_cast<natMat3 const*>(this)->operator[](i));
	}

	natMat3()
		: natMat3(T(1))
	{
	}

	explicit natMat3(T const& Scalar)
	{
		static_assert(std::is_arithmetic<T>::value || std::is_class<T>::value, "T should be an arithmetic type or a class.");
		value[0] = col_type(Scalar, 0, 0);
		value[1] = col_type(0, Scalar, 0);
		value[2] = col_type(0, 0, Scalar);
	}

	template <typename U>
	explicit natMat3(natMat3<U> const& m)
	{
		static_assert(std::is_arithmetic<T>::value || std::is_class<T>::value, "T should be an arithmetic type or a class.");
		value[0] = static_cast<col_type>(m.value[0]);
		value[1] = static_cast<col_type>(m.value[1]);
		value[2] = static_cast<col_type>(m.value[2]);
	}

	template <typename U>
	explicit natMat3(natMat3<U> && m)
	{
		static_assert(std::is_arithmetic<T>::value || std::is_class<T>::value, "T should be an arithmetic type or a class.");
		value[0] = static_cast<col_type &&>(m.value[0]);
		value[1] = static_cast<col_type &&>(m.value[1]);
		value[2] = static_cast<col_type &&>(m.value[2]);
	}

	natMat3(
		T const& x1, T const& y1, T const& z1,
		T const& x2, T const& y2, T const& z2,
		T const& x3, T const& y3, T const& z3)
	{
		static_assert(std::is_arithmetic<T>::value || std::is_class<T>::value, "T should be an arithmetic type or a class.");
		value[0] = col_type(x1, y1, z1);
		value[1] = col_type(x2, y2, z2);
		value[2] = col_type(x3, y3, z3);
	}

	explicit natMat3(
		natVec3<T> const& v)
	{
		static_assert(std::is_arithmetic<T>::value || std::is_class<T>::value, "T should be an arithmetic type or a class.");
		value[0] = col_type(v.x, 0, 0);
		value[1] = col_type(0, v.y, 0);
		value[2] = col_type(0, 0, v.z);
	}

	natMat3(
		col_type const& v1,
		col_type const& v2,
		col_type const& v3)
	{
		static_assert(std::is_arithmetic<T>::value || std::is_class<T>::value, "T should be an arithmetic type or a class.");
		value[0] = v1;
		value[1] = v2;
		value[2] = v3;
	}

	natMat3 inverse() const
	{
		T OneOverDeterminant = static_cast<T>(1) / (
			+value[0][0] * (value[1][1] * value[2][2] - value[2][1] * value[1][2])
			- value[1][0] * (value[0][1] * value[2][2] - value[2][1] * value[0][2])
			+ value[2][0] * (value[0][1] * value[1][2] - value[1][1] * value[0][2]));

		return natMat3(
			+value[1][1] * value[2][2] - value[2][1] * value[1][2] * OneOverDeterminant, -value[1][0] * value[2][2] - value[2][0] * value[1][2] * OneOverDeterminant, +value[1][0] * value[2][1] - value[2][0] * value[1][1] * OneOverDeterminant,
			-value[0][1] * value[2][2] - value[2][1] * value[0][2] * OneOverDeterminant, +value[0][0] * value[2][2] - value[2][0] * value[0][2] * OneOverDeterminant, -value[0][0] * value[2][1] - value[2][0] * value[0][1] * OneOverDeterminant,
			+value[0][1] * value[1][2] - value[1][1] * value[0][2] * OneOverDeterminant, -value[0][0] * value[1][2] - value[1][0] * value[0][2] * OneOverDeterminant, +value[0][0] * value[1][1] - value[1][0] * value[0][1] * OneOverDeterminant);
	}

	natMat3(natMat3 const& m) = default;
	natMat3(natMat3 && m) = default;

	natMat3& operator=(natMat3 const& m) = default;
	natMat3& operator=(natMat3 && m) = default;

	template <typename U>
	natMat3& operator=(natMat3<U> && m)
	{
		value[0] = static_cast<col_type &&>(m.value[0]);
		value[1] = static_cast<col_type &&>(m.value[1]);
		value[2] = static_cast<col_type &&>(m.value[2]);

		return *this;
	}

	natMat3& operator++()
	{
		++value[0]; ++value[1]; ++value[2];

		return *this;
	}

	natMat3& operator--()
	{
		--value[0]; --value[1]; --value[2];

		return *this;
	}

	natMat3 operator++(int)
	{
		natMat3 tResult(*this);
		++*this;

		return tResult;
	}

	natMat3 operator--(int)
	{
		natMat3 tResult(*this);
		--*this;

		return tResult;
	}

	natMat3 operator-()
	{
		return natMat3(-value[0], -value[1], -value[2]);
	}

	template <typename U>
	nBool operator==(natMat2<U> const& m) const
	{
		return (value[0] == m.value[0]) && (value[1] == m.value[1]) && (value[2] == m.value[2]);
	}

	template <typename U>
	nBool operator!=(natMat2<U> const& m) const
	{
		return !(*this == m);
	}

	OPERATORSCALAR(= );
	OPERATORSELF(= );

	OPERATORSCALAR(+= );
	OPERATORSELF(+= );

	OPERATORSCALAR(-= );
	OPERATORSELF(-= );

	OPERATORSCALAR(*= );

	OPERATORSCALAR(/= );

	template <typename U>
	natMat3& operator*=(natMat3<U> const& m)
	{
		return (*this = *this * m);
	}

	template <typename U>
	natMat3& operator/=(natMat3<U> const& m)
	{
		return (*this = *this * m.inverse());
	}

	TOPERATORSCALAR(+);
	TOPERATORSELF(+);

	TOPERATORSCALAR(-);
	TOPERATORSELF(-);

	TOPERATORSCALAR(*);
	TOPERATORSCALAR(/ );

	template <typename U>
	natMat3 operator*(natMat3<U> const& m) const
	{
		return natMat3(
			value[0][0] * static_cast<T>(m.value[0][0]) + value[1][0] * static_cast<T>(m.value[0][1]) + value[2][0] * static_cast<T>(m.value[0][2]),
			value[0][1] * static_cast<T>(m.value[0][0]) + value[1][1] * static_cast<T>(m.value[0][1]) + value[2][1] * static_cast<T>(m.value[0][2]),
			value[0][2] * static_cast<T>(m.value[0][0]) + value[1][2] * static_cast<T>(m.value[0][1]) + value[2][2] * static_cast<T>(m.value[0][2]),
			value[0][0] * static_cast<T>(m.value[1][0]) + value[1][0] * static_cast<T>(m.value[1][1]) + value[2][0] * static_cast<T>(m.value[1][2]),
			value[0][1] * static_cast<T>(m.value[1][0]) + value[1][1] * static_cast<T>(m.value[1][1]) + value[2][1] * static_cast<T>(m.value[1][2]),
			value[0][2] * static_cast<T>(m.value[1][0]) + value[1][2] * static_cast<T>(m.value[1][1]) + value[2][2] * static_cast<T>(m.value[1][2]),
			value[0][0] * static_cast<T>(m.value[2][0]) + value[1][0] * static_cast<T>(m.value[2][1]) + value[2][0] * static_cast<T>(m.value[2][2]),
			value[0][1] * static_cast<T>(m.value[2][0]) + value[1][1] * static_cast<T>(m.value[2][1]) + value[2][1] * static_cast<T>(m.value[2][2]),
			value[0][2] * static_cast<T>(m.value[2][0]) + value[1][2] * static_cast<T>(m.value[2][1]) + value[2][2] * static_cast<T>(m.value[2][2])
			);
	}

	template <typename U>
	natMat3 operator/(natMat3<U> const& m) const
	{
		return natMat3(*this) /= m;
	}
};

TOPERATORSCALARNM(+);

TOPERATORSCALARNM(-);

TOPERATORSCALARNM(*);

TOPERATORSCALARNM(/ );

template <typename T, typename U>
natVec3<T> operator*(natMat3<T> const& m, natVec3<U> const& v)
{
	return natVec3<T>(
		m[0][0] * v.x + m[1][0] * v.y + m[2][0] * v.z,
		m[0][1] * v.x + m[1][1] * v.y + m[2][1] * v.z,
		m[0][2] * v.x + m[1][2] * v.y + m[2][2] * v.z
		);
}

template <typename T, typename U>
natVec3<U> operator*(natVec3<U> const& v, natMat3<T> const& m)
{
	return natVec3<T>(
		v.x * m[0][0] + v.y * m[0][1] + v.z * m[0][2],
		v.x * m[1][0] + v.y * m[1][1] + v.z * m[1][2],
		v.x * m[2][0] + v.y * m[2][1] + v.z * m[2][2]
		);
}

template <typename T, typename U>
natVec3<T> operator/(natMat3<T> const& m, natVec3<U> const& v)
{
	return m.inverse() * v;
}

template <typename T, typename U>
natVec3<T> operator/(natVec3<U> const& v, natMat3<T> const& m)
{
	return v * m.inverse();
}

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
	natMat4& operator##op(U const& Scalar)\
	{\
		value[0] op static_cast<T>(Scalar);\
		value[1] op static_cast<T>(Scalar);\
		value[2] op static_cast<T>(Scalar);\
		value[3] op static_cast<T>(Scalar);\
		return *this;\
	}

#define OPERATORSELF(op) template <typename U>\
	natMat4& operator##op(natMat4<U> const& m)\
	{\
		value[0] op static_cast<col_type>(m.value[0]);\
		value[1] op static_cast<col_type>(m.value[1]);\
		value[2] op static_cast<col_type>(m.value[2]);\
		value[3] op static_cast<col_type>(m.value[3]);\
		return *this;\
	}

#define TOPERATORSCALAR(op) natMat4 operator##op(T const& Scalar) const\
	{\
		return natMat4(value[0] op Scalar, value[1] op Scalar, value[2] op Scalar, value[3] op Scalar);\
	}

#define TOPERATORSELF(op) template <typename U>\
	natMat4 operator##op(natMat4<U> const& m) const\
	{\
		return natMat4(value[0] op static_cast<col_type>(m.value[0]), value[1] op static_cast<col_type>(m.value[1]), value[2] op static_cast<col_type>(m.value[2]), value[3] op static_cast<col_type>(m.value[3]));\
	}

#define TOPERATORSCALARNM(op) template <typename T, typename U>\
natMat4<T> operator##op(U const& Scalar, natMat4<T> const& m)\
{\
	return natMat4<T>(static_cast<T>(Scalar) op m[0], static_cast<T>(Scalar) op m[1], static_cast<T>(Scalar) op m[2], static_cast<T>(Scalar) op m[3]);\
}

////////////////////////////////////////////////////////////////////////////////
///	@brief	4�׾���
////////////////////////////////////////////////////////////////////////////////
template <typename T = nFloat>
struct natMat4
{
	typedef natVec4<T> col_type;
	typedef natVec4<T> row_type;

private:
	col_type value[4];

public:
	constexpr nuInt length() const
	{
		return 4u;
	}

	col_type const& operator[](nuInt i) const
	{
		if (i >= length())
		{
			throw natException(_T("natMat4::operator[]"), _T("Out of range"));
		}

		return value[i];
	}

	col_type& operator[](nuInt i)
	{
		return const_cast<col_type&>(static_cast<natMat4 const*>(this)->operator[](i));
	}

	natMat4()
		: natMat4(T(1))
	{
	}

	explicit natMat4(T const& Scalar)
	{
		static_assert(std::is_arithmetic<T>::value || std::is_class<T>::value, "T should be an arithmetic type or a class.");
		value[0] = col_type(Scalar, 0, 0, 0);
		value[1] = col_type(0, Scalar, 0, 0);
		value[2] = col_type(0, 0, Scalar, 0);
		value[3] = col_type(0, 0, 0, Scalar);
	}

	template <typename U>
	explicit natMat4(natMat4<U> const& m)
	{
		static_assert(std::is_arithmetic<T>::value || std::is_class<T>::value, "T should be an arithmetic type or a class.");
		value[0] = static_cast<col_type>(m.value[0]);
		value[1] = static_cast<col_type>(m.value[1]);
		value[2] = static_cast<col_type>(m.value[2]);
		value[3] = static_cast<col_type>(m.value[3]);
	}

	template <typename U>
	explicit natMat4(natMat4<U> && m)
	{
		static_assert(std::is_arithmetic<T>::value || std::is_class<T>::value, "T should be an arithmetic type or a class.");
		value[0] = static_cast<col_type &&>(m.value[0]);
		value[1] = static_cast<col_type &&>(m.value[1]);
		value[2] = static_cast<col_type &&>(m.value[2]);
		value[3] = static_cast<col_type &&>(m.value[3]);
	}

	natMat4(
		T const& x1, T const& y1, T const& z1, T const& w1,
		T const& x2, T const& y2, T const& z2, T const& w2,
		T const& x3, T const& y3, T const& z3, T const& w3,
		T const& x4, T const& y4, T const& z4, T const& w4)
	{
		static_assert(std::is_arithmetic<T>::value || std::is_class<T>::value, "T should be an arithmetic type or a class.");
		value[0] = col_type(x1, y1, z1, w1);
		value[1] = col_type(x2, y2, z2, w2);
		value[2] = col_type(x3, y3, z3, w3);
		value[3] = col_type(x4, y4, z4, w4);
	}

	explicit natMat4(
		natVec4<T> const& v)
	{
		static_assert(std::is_arithmetic<T>::value || std::is_class<T>::value, "T should be an arithmetic type or a class.");
		value[0] = col_type(v.x, 0, 0, 0);
		value[1] = col_type(0, v.y, 0, 0);
		value[2] = col_type(0, 0, v.z, 0);
		value[3] = col_type(0, 0, 0, v.w);
	}

	natMat4(
		col_type const& v1,
		col_type const& v2,
		col_type const& v3,
		col_type const& v4)
	{
		static_assert(std::is_arithmetic<T>::value || std::is_class<T>::value, "T should be an arithmetic type or a class.");
		value[0] = v1;
		value[1] = v2;
		value[2] = v3;
		value[3] = v4;
	}

	natMat4 inverse() const
	{
		T Coef00 = value[2][2] * value[3][3] - value[3][2] * value[2][3];
		T Coef02 = value[1][2] * value[3][3] - value[3][2] * value[1][3];
		T Coef03 = value[1][2] * value[2][3] - value[2][2] * value[1][3];

		T Coef04 = value[2][1] * value[3][3] - value[3][1] * value[2][3];
		T Coef06 = value[1][1] * value[3][3] - value[3][1] * value[1][3];
		T Coef07 = value[1][1] * value[2][3] - value[2][1] * value[1][3];

		T Coef08 = value[2][1] * value[3][2] - value[3][1] * value[2][2];
		T Coef10 = value[1][1] * value[3][2] - value[3][1] * value[1][2];
		T Coef11 = value[1][1] * value[2][2] - value[2][1] * value[1][2];

		T Coef12 = value[2][0] * value[3][3] - value[3][0] * value[2][3];
		T Coef14 = value[1][0] * value[3][3] - value[3][0] * value[1][3];
		T Coef15 = value[1][0] * value[2][3] - value[2][0] * value[1][3];

		T Coef16 = value[2][0] * value[3][2] - value[3][0] * value[2][2];
		T Coef18 = value[1][0] * value[3][2] - value[3][0] * value[1][2];
		T Coef19 = value[1][0] * value[2][2] - value[2][0] * value[1][2];

		T Coef20 = value[2][0] * value[3][1] - value[3][0] * value[2][1];
		T Coef22 = value[1][0] * value[3][1] - value[3][0] * value[1][1];
		T Coef23 = value[1][0] * value[2][1] - value[2][0] * value[1][1];

		natVec4<T> Fac0(Coef00, Coef00, Coef02, Coef03);
		natVec4<T> Fac1(Coef04, Coef04, Coef06, Coef07);
		natVec4<T> Fac2(Coef08, Coef08, Coef10, Coef11);
		natVec4<T> Fac3(Coef12, Coef12, Coef14, Coef15);
		natVec4<T> Fac4(Coef16, Coef16, Coef18, Coef19);
		natVec4<T> Fac5(Coef20, Coef20, Coef22, Coef23);

		natVec4<T> Vec0(value[1][0], value[0][0], value[0][0], value[0][0]);
		natVec4<T> Vec1(value[1][1], value[0][1], value[0][1], value[0][1]);
		natVec4<T> Vec2(value[1][2], value[0][2], value[0][2], value[0][2]);
		natVec4<T> Vec3(value[1][3], value[0][3], value[0][3], value[0][3]);

		natVec4<T> Inv0(Vec1 * Fac0 - Vec2 * Fac1 + Vec3 * Fac2);
		natVec4<T> Inv1(Vec0 * Fac0 - Vec2 * Fac3 + Vec3 * Fac4);
		natVec4<T> Inv2(Vec0 * Fac1 - Vec1 * Fac3 + Vec3 * Fac5);
		natVec4<T> Inv3(Vec0 * Fac2 - Vec1 * Fac4 + Vec2 * Fac5);

		natVec4<T> SignA(+1, -1, +1, -1);
		natVec4<T> SignB(-1, +1, -1, +1);
		natMat4 Inverse(Inv0 * SignA, Inv1 * SignB, Inv2 * SignA, Inv3 * SignB);

		natVec4<T> Row0(Inverse[0][0], Inverse[1][0], Inverse[2][0], Inverse[3][0]);

		natVec4<T> Dot0(value[0] * Row0);
		T Dot1 = (Dot0.x + Dot0.y) + (Dot0.z + Dot0.w);

		T OneOverDeterminant = static_cast<T>(1) / Dot1;

		return Inverse * OneOverDeterminant;
	}

	natMat4(natMat4 const& m) = default;
	natMat4(natMat4 && m) = default;

	natMat4& operator=(natMat4 const& m) = default;
	natMat4& operator=(natMat4 && m) = default;

	template <typename U>
	natMat4& operator=(natMat4<U> && m)
	{
		value[0] = static_cast<col_type &&>(m.value[0]);
		value[1] = static_cast<col_type &&>(m.value[1]);
		value[2] = static_cast<col_type &&>(m.value[2]);
		value[3] = static_cast<col_type &&>(m.value[3]);

		return *this;
	}

	natMat4& operator++()
	{
		++value[0]; ++value[1]; ++value[2]; ++value[3];

		return *this;
	}

	natMat4& operator--()
	{
		--value[0]; --value[1]; --value[2]; --value[3];

		return *this;
	}

	natMat4 operator++(int)
	{
		natMat4 tResult(*this);
		++*this;

		return tResult;
	}

	natMat4 operator--(int)
	{
		natMat4 tResult(*this);
		--*this;

		return tResult;
	}

	natMat4 operator-()
	{
		return natMat4(-value[0], -value[1], -value[2], -value[3]);
	}

	template <typename U>
	nBool operator==(natMat2<U> const& m) const
	{
		return (value[0] == m.value[0]) && (value[1] == m.value[1]) && (value[2] == m.value[2]) && (value[3] == m.value[3]);
	}

	template <typename U>
	nBool operator!=(natMat2<U> const& m) const
	{
		return !(*this == m);
	}

	OPERATORSCALAR(= );
	OPERATORSELF(= );

	OPERATORSCALAR(+= );
	OPERATORSELF(+= );

	OPERATORSCALAR(-= );
	OPERATORSELF(-= );

	OPERATORSCALAR(*= );

	OPERATORSCALAR(/= );

	template <typename U>
	natMat4& operator*=(natMat4<U> const& m)
	{
		return (*this = *this * m);
	}

	template <typename U>
	natMat4& operator/=(natMat4<U> const& m)
	{
		return (*this = *this * m.inverse());
	}

	TOPERATORSCALAR(+);
	TOPERATORSELF(+);

	TOPERATORSCALAR(-);
	TOPERATORSELF(-);

	TOPERATORSCALAR(*);
	TOPERATORSCALAR(/);

	template <typename U>
	natMat4 operator*(natMat4<U> const& m) const
	{
		col_type const SrcA0 = value[0];
		col_type const SrcA1 = value[1];
		col_type const SrcA2 = value[2];
		col_type const SrcA3 = value[3];

		col_type const SrcB0 = m.value[0];
		col_type const SrcB1 = m.value[1];
		col_type const SrcB2 = m.value[2];
		col_type const SrcB3 = m.value[3];

		return natMat4(
			SrcA0 * SrcB0[0] + SrcA1 * SrcB0[1] + SrcA2 * SrcB0[2] + SrcA3 * SrcB0[3],
			SrcA0 * SrcB1[0] + SrcA1 * SrcB1[1] + SrcA2 * SrcB1[2] + SrcA3 * SrcB1[3],
			SrcA0 * SrcB2[0] + SrcA1 * SrcB2[1] + SrcA2 * SrcB2[2] + SrcA3 * SrcB2[3],
			SrcA0 * SrcB3[0] + SrcA1 * SrcB3[1] + SrcA2 * SrcB3[2] + SrcA3 * SrcB3[3]);
	}

	template <typename U>
	natMat4 operator/(natMat4<U> const& m) const
	{
		return natMat4(*this) /= m;
	}
};

TOPERATORSCALARNM(+);

TOPERATORSCALARNM(-);

TOPERATORSCALARNM(*);

TOPERATORSCALARNM(/ );

template <typename T, typename U>
natVec4<T> operator*(natMat4<T> const& m, natVec4<U> const& v)
{
	return natVec4<T>(
		m[0][0] * v.x + m[1][0] * v.y + m[2][0] * v.z + m[3][0] * v.w,
		m[0][1] * v.x + m[1][1] * v.y + m[2][1] * v.z + m[3][1] * v.w,
		m[0][2] * v.x + m[1][2] * v.y + m[2][2] * v.z + m[3][2] * v.w,
		m[0][3] * v.x + m[1][3] * v.y + m[2][3] * v.z + m[3][3] * v.w
		);
}

template <typename T, typename U>
natVec4<U> operator*(natVec4<U> const& v, natMat4<T> const& m)
{
	return natVec4<T>(
		v.x * m[0][0] + v.y * m[0][1] + v.z * m[0][2] + v.w * m[0][3],
		v.x * m[1][0] + v.y * m[1][1] + v.z * m[1][2] + v.w * m[1][3],
		v.x * m[2][0] + v.y * m[2][1] + v.z * m[2][2] + v.w * m[2][3],
		v.x * m[3][0] + v.y * m[3][1] + v.z * m[3][2] + v.w * m[3][3]
		);
}

template <typename T, typename U>
natVec4<T> operator/(natMat4<T> const& m, natVec4<U> const& v)
{
	return m.inverse() * v;
}

template <typename T, typename U>
natVec4<T> operator/(natVec4<U> const& v, natMat4<T> const& m)
{
	return v * m.inverse();
}

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

/// @}