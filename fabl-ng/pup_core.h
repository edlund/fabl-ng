
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

#ifndef LIBPUP_PUP_CORE_H
#define LIBPUP_PUP_CORE_H

#include "pup_env.h"

#define PUP_SDL_DEFAULT_INIT ( \
		SDL_INIT_VIDEO | \
		SDL_INIT_TIMER | \
		SDL_INIT_AUDIO | \
		SDL_INIT_EVENTS  \
	)

#define PUP_2(a, b) a, b
#define PUP_3(a, b, c) a, b, c
#define PUP_4(a, b, c, d) a, b, c, d

#define PUP_2_LOOP(IType, \
	L1, L1Lim, L1next, \
	L2, L2Lim, L2next, \
	Inner \
) \
	for (IType L1; L1Lim; L1next) { \
		for (IType L2; L2Lim; L2next) \
				Inner \
	}

#define PUP_3_LOOP(IType, \
	L1, L1Lim, L1next, \
	L2, L2Lim, L2next, \
	L3, L3Lim, L3next, \
	Inner \
) \
	for (IType L1; L1Lim; L1next) { \
		for (IType L2; L2Lim; L2next) { \
			for (IType L3; L3Lim; L3next) \
				Inner \
		} \
	}

#define PUP_C3f_BLACK PUP_3(0.0f, 0.0f, 0.0f)
#define PUP_C3f_WHITE PUP_3(1.0f, 1.0f, 1.0f)
#define PUP_C3f_R PUP_3(1.0f, 0.0f, 0.0f)
#define PUP_C3f_G PUP_3(0.0f, 1.0f, 0.0f)
#define PUP_C3f_B PUP_3(0.0f, 0.0f, 1.0f)
#define PUP_C3f_Y PUP_3(1.0f, 1.0f, 0.0f) // Yellow
#define PUP_C3f_C PUP_3(0.0f, 1.0f, 1.0f) // Cyan
#define PUP_C3f_M PUP_3(1.0f, 0.0f, 1.0f) // Magenta

#define PUP_SZ3f_0 PUP_3(0.0f, 0.0f, 0.0f)
#define PUP_SZ3f_1 PUP_3(1.0f, 1.0f, 1.0f)

#define PUP_C3f(/* pup::gl::color */ Color) \
	PUP_3((Color).r, (Color).g, (Color).b)

// Use in pup::controller sub-class methods.
#define PUP_KEY_DISPATCH(Class, Callback, Section) \
	key_dispatcher_.add_bindable(boost::bind(&Class::key_##Callback, this, _1), \
		#Callback, Section)

// Trigger an error of the given type.
#define PUP_ERR(Type, String) \
	pup::err<Type>(__FILE__, __LINE__, __FUNCTION__, String)

namespace pup {

enum {
	WD = 0, // width
	HT = 1, // height
	DH = 2  // depth
};

enum {
	X = 0,
	Y = 1,
	Z = 2,
	W = 3
};

enum {
	R = 0,
	G = 1,
	B = 2,
	A = 3
};

typedef std::size_t size_type;

template<class Exception>
void err(const char* file, const int line, const char* func, const std::string& msg)
{
	std::string fmt;

	try {
		fmt.append(boost::str(boost::format("error@%1%:%2%[%3%]: ") % file % line % func));
		fmt.append(msg);
	}
	catch (...) {
		std::fprintf(stderr, "error@%s:%d[%s]\n", file, line, func);
		throw Exception(msg);
	}
	throw Exception(fmt);
}

template <typename Iter, class RandomGenerator>
Iter random_element(Iter begin, Iter end, RandomGenerator& g)
{
	std::uniform_int_distribution<> d(0, std::distance(begin, end) - 1);
	std::advance(begin, d(g));
	return begin;
}

template <typename Iter>
Iter random_element(Iter begin, Iter end)
{
	static std::random_device rd;
	static std::mt19937 mt(rd());
	return random_element(begin, end, mt);
}

inline void do_nothing()
{
}

class reactor
{
public:
	virtual ~reactor() throw() {}

	virtual bool react(::SDL_Event& event) = 0;
	virtual void think() = 0;
	virtual void render() = 0;
};

typedef std::vector<int> int_vector;
typedef std::list<std::string> string_list;
typedef std::vector<std::string> string_vector;
typedef std::queue<std::string> string_queue;

class application;
class controller;
class timer;
class interval;

} // pup

#endif