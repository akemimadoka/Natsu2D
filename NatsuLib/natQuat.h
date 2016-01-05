#pragma once
#include "natMat.h"
#include "natVec.h"
#include "natTransform.h"

#define OPERATORSCALAR(op) template <typename U>\
	natQuat& operator##op(U const& Scalar)\
	{\
		x op static_cast<T>(Scalar);\
		y op static_cast<T>(Scalar);\
		z op static_cast<T>(Scalar);\
		w op static_cast<T>(Scalar);\
		return *this;\
	}

#define OPERATORSELF(op) template <typename U>\
	natQuat& operator##op(natQuat<U> const& v)\
	{\
		x op static_cast<T>(v.x);\
		y op static_cast<T>(v.y);\
		z op static_cast<T>(v.z);\
		w op static_cast<T>(v.w);\
		return *this;\
	}

#define TOPERATORSCALAR(op) template <typename U>\
	natQuat operator##op(U const& Scalar) const\
	{\
		return natQuat(w op static_cast<T>(Scalar), x op static_cast<T>(Scalar), y op static_cast<T>(Scalar), z op static_cast<T>(Scalar));\
	}

#define TOPERATORSELF(op) template <typename U>\
	natQuat operator##op(natQuat<U> const& v) const\
	{\
		return natQuat(w op static_cast<T>(v.w), x op static_cast<T>(v.x), y op static_cast<T>(v.y), z op static_cast<T>(v.z));\
	}

#define TOPERATORSCALARNM(op) template <typename T, typename U>\
natQuat<T> operator##op(U const& Scalar, natQuat<T> const& v)\
{\
	return natQuat<T>(static_cast<T>(Scalar) op v.w, static_cast<T>(Scalar) op v.x, static_cast<T>(Scalar) op v.y, static_cast<T>(Scalar) op v.z);\
}

template <typename T = nFloat>
struct natQuat
{
	typedef T type;

	T x, y, z, w;

	constexpr nuInt length() const
	{
		return 4u;
	}

	T const& operator[](nuInt i) const
	{
		if (i >= length())
		{
			throw natException(_T("natQuat::operator[]"), _T("Out of range"));
		}

		return (&x)[i];
	}

	T& operator[](nuInt i)
	{
		return const_cast<T&>(static_cast<natQuat const*>(this)->operator[](i));
	}

	natQuat()
		: x(0), y(0), z(0), w(1)
	{
		static_assert(std::is_arithmetic<T>::value || std::is_class<T>::value, "T should be an arithmetic type or a class.");
	}

	explicit natQuat(T const& Scalar)
		: x(Scalar), y(Scalar), z(Scalar), w(Scalar)
	{
		static_assert(std::is_arithmetic<T>::value || std::is_class<T>::value, "T should be an arithmetic type or a class.");
	}

	natQuat(T const& s, natVec3<T> const& v)
		: x(v.x), y(v.y), z(v.z), w(s)
	{
		static_assert(std::is_arithmetic<T>::value || std::is_class<T>::value, "T should be an arithmetic type or a class.");
	}

	natQuat(T const& sw, T const& sx, T const& sy, T const& sz)
		: x(sx), y(sy), z(sz), w(sw)
	{
		static_assert(std::is_arithmetic<T>::value || std::is_class<T>::value, "T should be an arithmetic type or a class.");
	}

	template <typename U>
	explicit natQuat(const U* S)
		: x(static_cast<T>(S[0])), y(static_cast<T>(S[1])), z(static_cast<T>(S[2])), w(static_cast<T>(S[3]))
	{
		static_assert(std::is_arithmetic<T>::value || std::is_class<T>::value, "T should be an arithmetic type or a class.");
	}

	template <typename U>
	explicit natQuat(natQuat<U> && q)
		: x(static_cast<T&&>(q.x)), y(static_cast<T&&>(q.y)), z(static_cast<T&&>(q.z)), w(static_cast<T&&>(q.w))
	{
		static_assert(std::is_arithmetic<T>::value || std::is_class<T>::value, "T should be an arithmetic type or a class.");
	}

	template <typename U>
	explicit natQuat(natQuat<U> const& q)
		: x(static_cast<T>(q.x)), y(static_cast<T>(q.y)), z(static_cast<T>(q.z)), w(static_cast<T>(q.w))
	{
		static_assert(std::is_arithmetic<T>::value || std::is_class<T>::value, "T should be an arithmetic type or a class.");
	}

	natQuat(natQuat const&) = default;
	natQuat(natQuat &&) = default;

	natQuat(natVec3<T> const& u, natVec3<T> const& v);

	natQuat(natVec3<T> const& eulerAngle)
	{
		natVec3<T> c = eulerAngle * T(0.5);
		natVec3<T> s = c;
		T(*pFunc)(T) = cos;
		c.call(pFunc);
		pFunc = sin;
		s.call(pFunc);

		w = c.x * c.y * c.z + s.x * s.y * s.z;
		x = s.x * c.y * c.z - c.x * s.y * s.z;
		y = c.x * s.y * c.z + s.x * c.y * s.z;
		z = c.x * c.y * s.z - s.x * s.y * c.z;
	}

	natQuat& operator=(natQuat const& v) = default;
	natQuat& operator=(natQuat && v) = default;

	template <typename U>
	natQuat& operator=(natQuat<U> && v)
	{
		x = static_cast<T&&>(v.x);
		y = static_cast<T&&>(v.y);
		z = static_cast<T&&>(v.z);
		w = static_cast<T&&>(v.w);

		return *this;
	}

	natQuat& operator++()
	{
		++x; ++y; ++z; ++w;
		return *this;
	}

	natQuat& operator--()
	{
		--x; --y; --z; --w;
		return *this;
	}

	natQuat operator++(int)
	{
		natQuat tResult(*this);
		++*this;
		return tResult;
	}

	natQuat operator--(int)
	{
		natQuat tResult(*this);
		--*this;
		return tResult;
	}

	natQuat operator-() const
	{
		return natQuat(-w, -x, -y, -z);
	}

	natQuat operator~() const
	{
		return natQuat(~w, ~x, ~y, ~z);
	}

	template <typename U>
	nBool operator==(natQuat<U> const& v) const
	{
		return (x == v.x) && (y == v.y) && (z == v.z) && (w == v.w);
	}

	template <typename U>
	nBool operator!=(natQuat<U> const& v) const
	{
		return !(*this == v);
	}

	natQuat conjugate() const
	{
		return natQuat(w, -x, -y, -z);
	}

	natQuat inverse() const;

	template <typename U>
	U cast() const;

	template <>
	natMat3<T> cast<natMat3<T>>() const
	{
		natMat3<T> Result;
		T qxx(x * x);
		T qyy(y * y);
		T qzz(z * z);
		T qxz(x * z);
		T qxy(x * y);
		T qyz(y * z);
		T qwx(w * x);
		T qwy(w * y);
		T qwz(w * z);

		Result[0][0] = T(1) - T(2) * (qyy + qzz);
		Result[0][1] = T(2) * (qxy + qwz);
		Result[0][2] = T(2) * (qxz - qwy);

		Result[1][0] = T(2) * (qxy - qwz);
		Result[1][1] = T(1) - T(2) * (qxx + qzz);
		Result[1][2] = T(2) * (qyz + qwx);

		Result[2][0] = T(2) * (qxz + qwy);
		Result[2][1] = T(2) * (qyz - qwx);
		Result[2][2] = T(1) - T(2) * (qxx + qyy);
		return Result;
	}

	explicit natQuat(natMat3<T> const& m)
	{
		T fourXSquaredMinus1 = m[0][0] - m[1][1] - m[2][2];
		T fourYSquaredMinus1 = m[1][1] - m[0][0] - m[2][2];
		T fourZSquaredMinus1 = m[2][2] - m[0][0] - m[1][1];
		T fourWSquaredMinus1 = m[0][0] + m[1][1] + m[2][2];

		nuInt biggestIndex = 0u;
		T fourBiggestSquaredMinus1 = fourWSquaredMinus1;
		if (fourXSquaredMinus1 > fourBiggestSquaredMinus1)
		{
			fourBiggestSquaredMinus1 = fourXSquaredMinus1;
			biggestIndex = 1u;
		}
		if (fourYSquaredMinus1 > fourBiggestSquaredMinus1)
		{
			fourBiggestSquaredMinus1 = fourYSquaredMinus1;
			biggestIndex = 2u;
		}
		if (fourZSquaredMinus1 > fourBiggestSquaredMinus1)
		{
			fourBiggestSquaredMinus1 = fourZSquaredMinus1;
			biggestIndex = 3u;
		}

		T biggestVal = sqrt(fourBiggestSquaredMinus1 + T(1)) * T(0.5);
		T mult = static_cast<T>(0.25) / biggestVal;

		switch (biggestIndex)
		{
		case 0u:
			w = biggestVal;
			x = (m[1][2] - m[2][1]) * mult;
			y = (m[2][0] - m[0][2]) * mult;
			z = (m[0][1] - m[1][0]) * mult;
			break;
		case 1u:
			w = (m[1][2] - m[2][1]) * mult;
			x = biggestVal;
			y = (m[0][1] + m[1][0]) * mult;
			z = (m[2][0] + m[0][2]) * mult;
			break;
		case 2u:
			w = (m[2][0] - m[0][2]) * mult;
			x = (m[0][1] + m[1][0]) * mult;
			y = biggestVal;
			z = (m[1][2] + m[2][1]) * mult;
			break;
		case 3u:
			w = (m[0][1] - m[1][0]) * mult;
			x = (m[2][0] + m[0][2]) * mult;
			y = (m[1][2] + m[2][1]) * mult;
			z = biggestVal;
			break;

		default:
			throw natException(_T("natQuat::natQuat"), _T("Internal error"));
		}
	}

	OPERATORSCALAR(= );
	OPERATORSELF(= );

	OPERATORSCALAR(+= );
	OPERATORSELF(+= );

	OPERATORSCALAR(-= );
	OPERATORSELF(-= );

	OPERATORSCALAR(*= );

	template <typename U>
	natQuat<T>& operator*=(natQuat<U> const& r)
	{
		natQuat<T> const p(*this);
		natQuat<T> const q(r);

		this->w = p.w * q.w - p.x * q.x - p.y * q.y - p.z * q.z;
		this->x = p.w * q.x + p.x * q.w + p.y * q.z - p.z * q.y;
		this->y = p.w * q.y + p.y * q.w + p.z * q.x - p.x * q.z;
		this->z = p.w * q.z + p.z * q.w + p.x * q.y - p.y * q.x;

		return *this;
	}

	OPERATORSCALAR(/= );

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

	template <typename U>
	natQuat<T> operator*(natQuat<U> const& r) const
	{
		return natQuat<T>(*this) *= r;
	}

	TOPERATORSCALAR(/ );

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

template <typename T, typename U>
natQuat<T> operator*(U const& Scalar, natQuat<T> const& v)
{
	return v * Scalar;
}

TOPERATORSCALARNM(/ );

TOPERATORSCALARNM(%);

TOPERATORSCALARNM(&);

TOPERATORSCALARNM(| );

TOPERATORSCALARNM(^);

TOPERATORSCALARNM(<< );

TOPERATORSCALARNM(>> );

namespace natTransform
{
	template <typename T>
	T dot(natQuat<T> const& x, natQuat<T> const& y)
	{
		return x.x * y.x + x.y * y.y + x.z * y.z + x.w * y.w;
	}

	template <typename T>
	T length(natQuat<T> const& q)
	{
		return sqrt(dot(q, q));
	}

	template <typename T>
	natQuat<T> normalize(natQuat<T> const& q)
	{
		T len = length(q);
		if (len <= T(0))
		{
			return natQuat<T>();
		}

		T oneOverLen = T(1) / len;
		return natQuat<T>(q.w * oneOverLen, q.x * oneOverLen, q.y * oneOverLen, q.z * oneOverLen);
	}

	template <typename T>
	natQuat<T> cross(natQuat<T> const& q1, natQuat<T> const& q2)
	{
		return natQuat<T>(
			q1.w * q2.w - q1.x * q2.x - q1.y * q2.y - q1.z * q2.z,
			q1.w * q2.x + q1.x * q2.w + q1.y * q2.z - q1.z * q2.y,
			q1.w * q2.y + q1.y * q2.w + q1.z * q2.x - q1.x * q2.z,
			q1.w * q2.z + q1.z * q2.w + q1.x * q2.y - q1.y * q2.x);
	}

	template <typename T>
	natQuat<T> mix(natQuat<T> const& x, natQuat<T> const& y, T const& a)
	{
		T cosTheta = dot(x, y);

		if (cosTheta > T(1) - std::numeric_limits<T>::epsilon())
		{
			return natQuat<T>(
				mix(x.w, y.w, a),
				mix(x.x, y.x, a),
				mix(x.y, y.y, a),
				mix(x.z, y.z, a));
		}
		else
		{
			T angle = acos(cosTheta);
			return (sin((T(1) - a) * angle) * x + sin(a * angle) * y) / sin(angle);
		}
	}

	template <typename T>
	natQuat<T> lerp(natQuat<T> const& x, natQuat<T> const& y, T const& a)
	{
#ifdef _DEBUG
		if (a < T(0) || a > T(1))
			throw natException(_T("natTransform::lerp"), _T("Out of range"));
#endif
		return x * (T(1) - a) + (y * a);
	}

	template <typename T>
	natQuat<T> slerp(natQuat<T> const& x, natQuat<T> const& y, T const& a)
	{
		T cosTheta = dot(x, y);

		natQuat<T> z = cosTheta < T(0) ? -y : y;

		if (cosTheta < T(0))
		{
			cosTheta = -cosTheta;
		}

		if (cosTheta > T(1) - std::numeric_limits<T>::epsilon())
		{
			return natQuat<T>(
				mix(x.w, z.w, a),
				mix(x.x, z.x, a),
				mix(x.y, z.y, a),
				mix(x.z, z.z, a));
		}
		else
		{
			T angle = acos(cosTheta);
			return (sin((T(1) - a) * angle) * x + sin(a * angle) * z) / sin(angle);
		}
	}

	template <typename T>
	natQuat<T> rotate(natQuat<T> const& q, T const& angle, natVec3<T> const& v)
	{
		static_assert(std::is_floating_point<T>::value, "T should be an floating point type");

		natVec3<T> tmp(v);

		T len = tmp.length();
		if (abs(len - T(1)) > std::numeric_limits<T>::epsilon())
		{
			T oneOverLen = static_cast<T>(1) / len;
			tmp *= oneOverLen;
		}

		T const Sin = sin(angle * T(0.5));
		tmp *= Sin;

		return q * natQuat<T>(cos(angle * T(0.5)), tmp.x, tmp.y, tmp.z);
	}

	template <typename T>
	T roll(natQuat<T> const& q)
	{
		return T(atan2(T(2) * (q.x * q.y + q.w * q.z), q.w * q.w + q.x * q.x - q.y * q.y - q.z * q.z));
	}

	template <typename T>
	T pitch(natQuat<T> const& q)
	{
		return T(atan2(T(2) * (q.y * q.z + q.w * q.x), q.w * q.w - q.x * q.x - q.y * q.y + q.z * q.z));
	}

	template <typename T>
	T yaw(natQuat<T> const& q)
	{
		return asin(T(-2) * (q.x * q.z - q.w * q.y));
	}

	template <typename T>
	natVec3<T> eulerAngles(natQuat<T> const& x)
	{
		return natVec3<T>(pitch(x), yaw(x), roll(x));
	}

	template <typename T>
	T angle(natQuat<T> const& x)
	{
		return acos(x.w) * T(2);
	}

	template <typename T>
	natVec3<T> axis(natQuat<T> const& x)
	{
		T tmp1 = static_cast<T>(1) - x.w * x.w;
		if (tmp1 <= static_cast<T>(0))
			return natVec3<T>(0, 0, 1);
		T tmp2 = static_cast<T>(1) / sqrt(tmp1);

		return natVec3<T>(x.x * tmp2, x.y * tmp2, x.z * tmp2);
	}

	template <typename T>
	natQuat<T> angleAxis(T const& angle, natVec3<T> const& v)
	{
		T const s = sin(angle * T(0.5));

		return natQuat<T>(
			cos(angle * T(0.5)),
			v.x * s,
			v.y * s,
			v.z * s);
	}
}

template <typename T>
natQuat<T>::natQuat(natVec3<T> const& u, natVec3<T> const& v)
{
	const natVec3<T> LocalW(natTransform::cross(u, v));
	const T Dot = natTransform::dot(u, v);

	*this = natTransform::normalize(natQuat(T(1) + Dot, LocalW.x, LocalW.y, LocalW.z));
}

template <typename T>
natQuat<T> natQuat<T>::inverse() const
{
	return conjugate() / natTransform::dot(*this, *this);
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