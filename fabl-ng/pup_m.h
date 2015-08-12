
// libPowerUP - Create games with SDL2 and OpenGL
// Copyright(c) 2015, Erik Edlund <erik.edlund@32767.se>
// All rights reserved.
// 
// Redistribution and use in source and binary forms, with
// or without modification, are permitted provided that the
// following conditions are met:
// 
// 1. Redistributions of source code must retain the above
//    copyright notice, this list of conditions and the
//    following disclaimer.
// 
// 2. Redistributions in binary form must reproduce the above
//    copyright notice, this list of conditions and the
//    following disclaimer in the documentation and / or
//    other materials provided with the distribution.
// 
// 3. Neither the name of the copyright holder nor the names
//    of its contributors may be used to endorse or promote
//    products derived from this software without specific
//    prior written permission.
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND
// CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
// INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
// MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED.IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES(INCLUDING,
// BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
// WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
// NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
// SUCH DAMAGE.

#ifndef LIBPUP_PUP_M_H
#define LIBPUP_PUP_M_H

#include "pup_env.h"
#include "pup_core.h"

namespace pup {
namespace m {

namespace bg = boost::geometry;

template <
	class CoordinateType,
	class CoordinateSystem = bg::cs::cartesian
>
class basic_point_2d :
	public bg::model::point<CoordinateType, 2, CoordinateSystem>
{
public:
	typedef CoordinateType value_type;
	typedef CoordinateSystem coordinate_system_type;

	inline basic_point_2d() :
		bg::model::point<CoordinateType, 2, CoordinateSystem>()
	{
		this->x(0);
		this->y(0);
	}

	inline basic_point_2d(const CoordinateType& x, const CoordinateType& y)
		: bg::model::point<CoordinateType, 2, CoordinateSystem>(x, y)
	{}

	inline const CoordinateType& x() const { return this->template get<X>(); }
	inline const CoordinateType& y() const { return this->template get<Y>(); }
	
	inline void x(const CoordinateType& v) { this->template set<X>(v); }
	inline void y(const CoordinateType& v) { this->template set<Y>(v); }

	inline void x_d(const CoordinateType& v) { this->template set<X>(this->x() + v); }
	inline void y_d(const CoordinateType& v) { this->template set<Y>(this->y() + v); }

	template <class U>
	inline void xy(const U& x, const U& y)
	{
		this->x(static_cast<CoordinateType>(x));
		this->y(static_cast<CoordinateType>(y));
	}
};

template <
	class CoordinateType,
	class CoordinateSystem = bg::cs::cartesian
>
class basic_point_3d :
	public bg::model::point<CoordinateType, 3, CoordinateSystem>
{
public:
	typedef CoordinateType value_type;
	typedef CoordinateSystem coordinate_system_type;

	inline basic_point_3d()	:
		bg::model::point<CoordinateType, 3, CoordinateSystem>()
	{
		this->x(0);
		this->y(0);
		this->z(0);
	}

	inline basic_point_3d(const CoordinateType& x, const CoordinateType& y, const CoordinateType& z) :
		bg::model::point<CoordinateType, 3, CoordinateSystem>(x, y, z)
	{}
	
	inline const CoordinateType& x() const { return this->template get<X>(); }
	inline const CoordinateType& y() const { return this->template get<Y>(); }
	inline const CoordinateType& z() const { return this->template get<Z>(); }

	inline void x(const CoordinateType& v) { this->template set<X>(v); }
	inline void y(const CoordinateType& v) { this->template set<Y>(v); }
	inline void z(const CoordinateType& v) { this->template set<Z>(v); }

	inline void x_d(const CoordinateType& v) { this->template set<X>(this->x() + v); }
	inline void y_d(const CoordinateType& v) { this->template set<Y>(this->y() + v); }
	inline void z_d(const CoordinateType& v) { this->template set<Z>(this->z() + v); }

	template <class U>
	inline void xyz(const U& x, const U& y, const U& z)
	{
		this->x(static_cast<CoordinateType>(x));
		this->y(static_cast<CoordinateType>(y));
		this->z(static_cast<CoordinateType>(z));
	}
};

template <class SizeType, std::size_t Dimensions>
class basic_size
{
public:
	inline basic_size() {}

	template <std::size_t K>
	inline const SizeType& get() const
	{
		BOOST_STATIC_ASSERT(K < Dimensions);
		return values_[K];
	}

	template <std::size_t K>
	inline void set(const SizeType& value)
	{
		BOOST_STATIC_ASSERT(K < Dimensions);
		values_[K] = value;
	}

private:
	SizeType values_[Dimensions];
};

template <class SizeType>
class basic_size_2d :
	public basic_size<SizeType, 2>
{
public:
	typedef SizeType value_type;

	inline basic_size_2d() :
		basic_size<SizeType, 2>()
	{
		this->w(0);
		this->h(0);
	}

	inline basic_size_2d(const SizeType& wd, const SizeType& ht) :
		basic_size<SizeType, 2>()
	{
		this->w(wd);
		this->h(ht);
	}

	inline const SizeType& w() const { return this->template get<WD>(); }
	inline const SizeType& h() const { return this->template get<HT>(); }

	inline void w(const SizeType& v) { this->template set<WD>(v); }
	inline void h(const SizeType& v) { this->template set<HT>(v); }

	inline void w_d(const SizeType& v) { this->template set<WD>(this->w() + v); }
	inline void h_d(const SizeType& v) { this->template set<HT>(this->h() + v); }

	template <class U>
	inline void wh(const U& w, const U& h)
	{
		this->w(static_cast<SizeType>(w));
		this->h(static_cast<SizeType>(h));
	}
};

template <class SizeType>
class basic_size_3d :
	public basic_size<SizeType, 3>
{
public:
	typedef SizeType value_type;

	inline basic_size_3d() :
		basic_size<SizeType, 3>()
	{
		this->w(0);
		this->h(0);
		this->d(0);
	}

	inline basic_size_3d(const SizeType& wd, const SizeType& ht, const SizeType& dt) :
		basic_size<SizeType, 3>()
	{
		this->w(wd);
		this->h(ht);
		this->d(dt);
	}

	inline const SizeType& w() const { return this->template get<WD>(); }
	inline const SizeType& h() const { return this->template get<HT>(); }
	inline const SizeType& d() const { return this->template get<DH>(); }

	inline void w(const SizeType& v) { this->template set<WD>(v); }
	inline void h(const SizeType& v) { this->template set<HT>(v); }
	inline void d(const SizeType& v) { this->template set<DH>(v); }

	inline void w_d(const SizeType& v) { this->template set<WD>(this->w() + v); }
	inline void h_d(const SizeType& v) { this->template set<HT>(this->h() + v); }
	inline void d_d(const SizeType& v) { this->template set<DH>(this->d() + v); }

	template <class U>
	inline void whd(const U& w, const U& h, const U& d)
	{
		this->w(static_cast<SizeType>(w));
		this->h(static_cast<SizeType>(h));
		this->d(static_cast<SizeType>(d));
	}
};

typedef basic_point_2d<::GLfloat> fpoint_2d;
typedef basic_point_3d<::GLfloat> fpoint_3d;

typedef basic_point_2d<::GLdouble> dpoint_2d;
typedef basic_point_3d<::GLdouble> dpoint_3d;

typedef basic_point_2d<::GLint> spoint_2d;
typedef basic_point_3d<::GLint> spoint_3d;

typedef basic_size_2d<::GLfloat> fsize_2d;
typedef basic_size_3d<::GLfloat> fsize_3d;

typedef basic_size_2d<::GLdouble> dsize_2d;
typedef basic_size_3d<::GLdouble> dsize_3d;

typedef basic_size_2d<::GLint> ssize_2d;
typedef basic_size_3d<::GLint> ssize_3d;

template <class Point2d, typename T>
inline void orbit2d(Point2d& p, const T A, const T r)
{
	p.x(r * std::sin(A));
	p.y(r * std::cos(A));
}

template <class Point3d, typename T>
inline void orbit3d(Point3d& e, Point3d& c, const T A, const T B, const T r)
{
	e.x(c.x() + (+r) * -std::sin(A) * +std::cos(B));
	e.y(c.y() + (+r) * -std::sin(B));
	e.z(c.z() + (-r) * +std::cos(A) * +std::cos(B));
}

template <class Point3d>
inline typename Point3d::value_type magnitude(const Point3d& p)
{
	return std::sqrt(
		p.x() * p.x() +
		p.y() * p.y() +
		p.z() * p.z()
	);
}

template <class Point3d>
inline void normalize(Point3d& p)
{
	typename Point3d::value_type m = magnitude(p);

	if (m != 0.0) {
		p.x(p.x() / m);
		p.y(p.y() / m);
		p.z(p.z() / m);
	}
}

#define CK_INDEX_FROM_ND(V, P) \
	do { \
		if ((V) >= (P)) \
			PUP_ERR(std::out_of_range, #V " >= " #P); \
	} while (0)

template <typename T>
inline T index_from_2d(const T& x, const T& y,
	const T& px, const T& py)
{
	CK_INDEX_FROM_ND(x, px);
	CK_INDEX_FROM_ND(y, py);
	return y * px + x;
}

template <typename T>
inline T index_from_3d(const T& x, const T& y, const T& z,
	const T& px, const T& py, const T& pz)
{
	CK_INDEX_FROM_ND(x, px);
	CK_INDEX_FROM_ND(y, py);
	CK_INDEX_FROM_ND(z, pz);
	return y * pz * px + z * px + x;
}

template <typename T>
inline T index_from_4d(const T& x, const T& y, const T& z, const T& w,
	const T& px, const T& py, const T& pz, const T& pw)
{
	CK_INDEX_FROM_ND(x, px);
	CK_INDEX_FROM_ND(y, py);
	CK_INDEX_FROM_ND(z, pz);
	CK_INDEX_FROM_ND(w, pw);
	return w * py * pz * px + y * pz * px + z * px + x;
}

#undef CK_INDEX_FROM

} // m
} // pup

#endif
