
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

#include "pup.h"

TEST_CASE("try hex to color conversion", "[pup::gl1::color]") {
	REQUIRE(pup::gl1::color::from_hex("#ff0000") == pup::gl1::color::r);
	REQUIRE(pup::gl1::color::from_hex("#00ff00") == pup::gl1::color::g);
	REQUIRE(pup::gl1::color::from_hex("#0000ff") == pup::gl1::color::b);

	REQUIRE(pup::gl1::color::from_hex("ffffff") == pup::gl1::color::white);
}

TEST_CASE("rotation works as expected", "[pup::t]") {
	
	SECTION("rot_cw90_2d: 3x3") {
		int data[] = {
			1, 2, 3,
			4, 5, 6,
			7, 8, 9
		};
		int expected1[] = {
			7, 4, 1,
			8, 5, 2,
			9, 6, 3
		};
		int expected2[] = {
			9, 8, 7,
			6, 5, 4,
			3, 2, 1
		};
		int expected3[] = {
			3, 6, 9,
			2, 5, 8,
			1, 4, 7
		};
		pup::t::rot_cw90_2d(data, 3);
		REQUIRE(std::memcmp(data, expected1, 9) == 0);
		pup::t::rot_cw90_2d(data, 3);
		REQUIRE(std::memcmp(data, expected2, 9) == 0);
		pup::t::rot_cw90_2d(data, 3);
		REQUIRE(std::memcmp(data, expected3, 9) == 0);
	}

	SECTION("rot_cw90_2d: 3x3 classic") {
		int t[] = {
			0, 1, 0,
			1, 1, 1,
			0, 0, 0
		};
		int t_expected[] = {
			0, 1, 0,
			0, 1, 1,
			0, 1, 0
		};
		pup::t::rot_cw90_2d(t, 3);
		REQUIRE(std::memcmp(t, t_expected, 9) == 0);

		int l[] = {
			0, 1, 0,
			0, 1, 0,
			1, 1, 0
		};
		int l_expected[] = {
			1, 0, 0,
			1, 1, 1,
			0, 0, 0
		};
		pup::t::rot_cw90_2d(l, 3);
		REQUIRE(std::memcmp(l, l_expected, 9) == 0);
	}

	SECTION("rot_cw90_2d: 2x2") {
		int data[] = {
			1, 2,
			3, 4
		};
		int expected1[] = {
			3, 1,
			4, 2
		};
		int expected2[] = {
			4, 3,
			2, 1
		};
		pup::t::rot_cw90_2d(data, 2);
		REQUIRE(std::memcmp(data, expected1, 2) == 0);
		pup::t::rot_cw90_2d(data, 2);
		REQUIRE(std::memcmp(data, expected2, 2) == 0);
	}

	SECTION("rot_cw90_2d: 4x4") {
		int data[] = {
			1, 2, 3, 4,
			5, 6, 7, 8,
			9, 0, 1, 2,
			3, 4, 5, 6
		};
		int expected[] = {
			3, 9, 5, 1,
			4, 0, 6, 2,
			5, 1, 7, 3,
			6, 2, 8, 4
		};
		pup::t::rot_cw90_2d(data, 4);
		REQUIRE(std::memcmp(data, expected, 16) == 0);
	}

	SECTION("rot_cw90_2d: 5x5 classic") {
		int i[] = {
			0, 0, 0, 0, 0,
			0, 0, 0, 0, 0,
			1, 1, 1, 1, 1,
			0, 0, 0, 0, 0,
			0, 0, 0, 0, 0
		};
		int i_expected[] = {
			0, 0, 1, 0, 0,
			0, 0, 1, 0, 0,
			0, 0, 1, 0, 0,
			0, 0, 1, 0, 0,
			0, 0, 1, 0, 0
		};
		pup::t::rot_cw90_2d(i, 5);
		REQUIRE(std::memcmp(i, i_expected, 25) == 0);
	}

	SECTION("rot_cw90_3d_xz: 3x3x3") {
		int data[] = {
			1, 2, 3,
			4, 5, 6,
			7, 8, 9,
			
			1, 2, 3,
			4, 5, 6,
			7, 8, 9,

			1, 2, 3,
			4, 5, 6,
			7, 8, 9
		};
		int expected[] = {
			7, 4, 1,
			8, 5, 2,
			9, 6, 3,

			7, 4, 1,
			8, 5, 2,
			9, 6, 3,

			7, 4, 1,
			8, 5, 2,
			9, 6, 3
		};
		pup::t::rot_cw90_3d_xz(data, 3);
		REQUIRE(std::memcmp(data, expected, 27) == 0);
	}

	SECTION("rot_cw90_3d_yz: 3x3x3") {
		int data[] = {
			1, 2, 3,
			4, 5, 6,
			7, 8, 9,

			1, 2, 3,
			4, 5, 6,
			7, 8, 9,

			1, 2, 3,
			4, 5, 6,
			7, 8, 9
		};
		int expected1[] = {
			1, 2, 3,
			1, 2, 3,
			1, 2, 3,

			4, 5, 6,
			4, 5, 6,
			4, 5, 6,

			7, 8, 9,
			7, 8, 9,
			7, 8, 9
		};
		int expected2[] = {
			7, 8, 9,
			4, 5, 6,
			1, 2, 3,

			7, 8, 9,
			4, 5, 6,
			1, 2, 3,

			7, 8, 9,
			4, 5, 6,
			1, 2, 3
		};
		pup::t::rot_cw90_3d_yz(data, 3);
		REQUIRE(std::memcmp(data, expected1, 27) == 0);
		pup::t::rot_cw90_3d_yz(data, 3);
		REQUIRE(std::memcmp(data, expected2, 27) == 0);
	}

	SECTION("rot_cw90_3d_xy: 3x3x3") {
		int data[] = {
			1, 2, 3,
			4, 5, 6,
			7, 8, 9,

			1, 2, 3,
			4, 5, 6,
			7, 8, 9,

			1, 2, 3,
			4, 5, 6,
			7, 8, 9
		};
		int expected1[] = {
			1, 1, 1,
			4, 4, 4,
			7, 7, 7,

			2, 2, 2,
			5, 5, 5,
			8, 8, 8,

			3, 3, 3,
			6, 6, 6,
			9, 9, 9
		};
		int expected2[] = {
			3, 2, 1,
			6, 5, 4,
			9, 8, 7,

			3, 2, 1,
			6, 5, 4,
			9, 8, 7,

			3, 2, 1,
			6, 5, 4,
			9, 8, 7
		};
		pup::t::rot_cw90_3d_xy(data, 3);
		REQUIRE(std::memcmp(data, expected1, 27) == 0);
		pup::t::rot_cw90_3d_xy(data, 3);
		REQUIRE(std::memcmp(data, expected2, 27) == 0);
	}
	
}


