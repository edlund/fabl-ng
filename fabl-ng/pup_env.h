
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

#ifndef LIBPUP_PUP_ENV_H
#define LIBPUP_PUP_ENV_H

#if defined(_WIN32) || defined(_WIN64) || defined(__WINRT__)
#define PUP_WIN
#ifdef _MSC_VER
#define PUP_WIN_MSC
#endif
#include <sdkddkver.h>
#else
#define PUP_NIX
#endif

#include <boost/algorithm/string.hpp>
#include <boost/asio.hpp>
#include <boost/core/noncopyable.hpp>
#include <boost/crc.hpp>
#include <boost/cstdint.hpp>
#include <boost/filesystem.hpp>
#include <boost/format.hpp>
#include <boost/foreach.hpp>
#include <boost/function.hpp>
#include <boost/geometry.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/log/trivial.hpp>
#include <boost/program_options.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/random.hpp>
#include <boost/regex.hpp>
#include <boost/shared_array.hpp>
#include <boost/type_traits.hpp>

#ifdef PUP_WIN
#include <tchar.h>
#ifdef PUP_WIN_MSC
#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "SDL2.lib")
#pragma comment(lib, "SDL2main.lib")
#pragma comment(lib, "SDL2_mixer.lib")
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glu32.lib")
#pragma comment(lib, "glew32.lib")
#pragma comment(lib, "freetype.lib")
#define _CRT_SECURE_NO_WARNINGS
#endif
#define OS_MAIN(C, V) _tmain(int C, _TCHAR* V[])
#else
#define OS_MAIN(C, V) main(int C, char* V[])
#endif

#ifndef PUP_GL_MAJOR
#define PUP_GL_MAJOR 1
#endif
#ifndef PUP_GL_MINOR
#define PUP_GL_MINOR 5
#endif

#include <cmath>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <ctime>

#include <algorithm>
#include <exception>
#include <memory>

#include <fstream>
#include <functional>
#include <iomanip>
#include <iostream>
#include <list>
#include <map>
#include <random>
#include <set>
#include <sstream>
#include <string>
#include <vector>
#include <queue>

#ifdef PUP_WIN
#include <GL/glew.h>
#include <GL/GLU.h>
#else
#include <GL/glew.h>
#include <GL/glu.h>
#endif

#include <catch.hpp>

#include <SDL.h>
#include <SDL_mixer.h>
#include <SDL_opengl.h>

#include <ft2build.h>

#include FT_FREETYPE_H
#include FT_GLYPH_H
#include FT_OUTLINE_H
#include FT_TRIGONOMETRY_H

#define PUP_CONFIG_PATH "../resources/config.ini"

#endif
