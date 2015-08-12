
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

#ifndef LIBPUP_PUP_T_H
#define LIBPUP_PUP_T_H

#include "pup_env.h"
#include "pup_core.h"

namespace pup {
namespace t {

// Overall thought: There is probably a smarter way to do
// this. The solutions found below are probably very slow,
// but should work okay for rotation of smaller arrays.

// Rotate the given square matrix 90 degrees clock-wise.
template <typename T, typename U>
inline void rot_cw90_2d(T* m, const U& s)
{
	if (s < 2) {
		// nothing to do
	} else if (s == 2) {
		std::swap(m[0], m[3]);
		std::swap(m[1], m[3]);
		std::swap(m[0], m[2]);
	} else {
		const U size = s * s;
		for (U y = 0; y < s; ++y) {
			for (U x = 0; x < s - y; ++x) {
				std::swap(
					m[y * s + x],
					m[size - (x * s + y) - 1]
				);
			}
		}
		// Every column is now reversed from top to bottom
		// and must be reversed to form the desired result.
		for (U x = 0; x < s; ++x) {
			for (U y = 0; y < s / 2; ++y) {
				std::swap(
					m[s * y + x],
					m[size - (s * y + (s - x - 1)) - 1]
				);
			}
		}
	}
}

template <typename T, typename U>
inline void rot_ccw90_2d(T* m, const U& s)
{
	for (U i = 0; i < 3; ++i)
		rot_cw90_2d(m, s);
}

// Rotate every layer in the given cube matrix 90 degrees
// clock-wise in the {x, z} planes.
template <typename T, typename U>
inline void rot_cw90_3d_xz(T* m, const U& s)
{
	for (U z = 0; z < s; ++z) {
		rot_cw90_2d(m + s * s * z, s);
	}
}

template <typename T, typename U>
inline void rot_ccw90_3d_xz(T* m, const U& s)
{
	for (U i = 0; i < 3; ++i)
		rot_cw90_3d_xz(m, s);
}

// Rotate every layer in the given cube matrix 90 degrees
// clock-wise in the {y, z} planes.
template <typename T, typename U>
inline void rot_cw90_3d_yz(T* m, const U& s)
{
	T* n = new T[s * s * s];
	U i;
	i = 0;
	PUP_3_LOOP(U,
		x = 0, x < s, ++x,
		y = 0, y < s, ++y,
		z = 0, z < s, ++z,
		{ n[i++] = m[s * s * y + s * z + x]; }
	);
	rot_cw90_3d_xz(n, s);
	i = 0;
	PUP_3_LOOP(U,
		x = 0, x < s, ++x,
		y = 0, y < s, ++y,
		z = 0, z < s, ++z,
		{ m[s * s * y + s * z + x] = n[i++]; }
	);
	delete[] n;
}

template <typename T, typename U>
inline void rot_ccw90_3d_yz(T* m, const U& s)
{
	for (U i = 0; i < 3; ++i)
		rot_cw90_3d_yz(m, s);
}


// Rotate every layer in the given cube matrix 90 degrees
// clock-wise in the {x, y} planes.
template <typename T, typename U>
inline void rot_cw90_3d_xy(T* m, const U& s)
{
	T* n = new T[s * s * s];
	U i;
	i = 0;
	PUP_3_LOOP(U,
		z = 0, z < s, ++z,
		y = 0, y < s, ++y,
		x = 0, x < s, ++x,
		{ n[i++] = m[s * s * y + s * z + x]; }
	);
	rot_cw90_3d_xz(n, s);
	i = 0;
	PUP_3_LOOP(U,
		z = 0, z < s, ++z,
		y = 0, y < s, ++y,
		x = 0, x < s, ++x,
		{ m[s * s * y + s * z + x] = n[i++]; }
	);
	delete[] n;
}

template <typename T, typename U>
inline void rot_ccw90_3d_xy(T* m, const U& s)
{
	for (U i = 0; i < 3; ++i)
		rot_cw90_3d_xy(m, s);
}

} // t
} // pup

#endif

