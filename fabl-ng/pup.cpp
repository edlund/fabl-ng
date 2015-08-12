
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

namespace pup {

namespace global {

keycode_map keycodes {
	{ "a", ::SDLK_a },
	{ "b", ::SDLK_b },
	{ "c", ::SDLK_c },
	{ "d", ::SDLK_d },
	{ "e", ::SDLK_e },
	{ "f", ::SDLK_f },
	{ "g", ::SDLK_g },
	{ "h", ::SDLK_h },
	{ "i", ::SDLK_i },
	{ "j", ::SDLK_j },
	{ "k", ::SDLK_k },
	{ "l", ::SDLK_l },
	{ "m", ::SDLK_m },
	{ "n", ::SDLK_n },
	{ "o", ::SDLK_o },
	{ "p", ::SDLK_p },
	{ "q", ::SDLK_q },
	{ "r", ::SDLK_r },
	{ "s", ::SDLK_s },
	{ "t", ::SDLK_t },
	{ "u", ::SDLK_u },
	{ "v", ::SDLK_v },
	{ "w", ::SDLK_w },
	{ "x", ::SDLK_x },
	{ "y", ::SDLK_y },
	{ "z", ::SDLK_z },

	{ "0", ::SDLK_0 },
	{ "1", ::SDLK_1 },
	{ "2", ::SDLK_2 },
	{ "3", ::SDLK_3 },
	{ "4", ::SDLK_4 },
	{ "5", ::SDLK_5 },
	{ "6", ::SDLK_6 },
	{ "7", ::SDLK_7 },
	{ "8", ::SDLK_8 },
	{ "9", ::SDLK_9 },

	{ "F1", ::SDLK_F1 },
	{ "F2", ::SDLK_F2 },
	{ "F3", ::SDLK_F3 },
	{ "F4", ::SDLK_F4 },
	{ "F5", ::SDLK_F5 },
	{ "F6", ::SDLK_F6 },
	{ "F7", ::SDLK_F7 },
	{ "F8", ::SDLK_F8 },
	{ "F9", ::SDLK_F9 },
	{ "F10", ::SDLK_F10 },
	{ "F11", ::SDLK_F11 },
	{ "F12", ::SDLK_F12 },

	{ "space", ::SDLK_SPACE },
	{ "return", ::SDLK_RETURN },
	{ "escape", ::SDLK_ESCAPE },
	{ "exclaim", ::SDLK_EXCLAIM },
	{ "backspace", ::SDLK_BACKSPACE },
	{ "tab", ::SDLK_TAB },
	{ "quote", ::SDLK_QUOTE },
	{ "quotedbl", ::SDLK_QUOTEDBL },
	{ "hash", ::SDLK_HASH },
	{ "percent", ::SDLK_PERCENT },
	{ "dollar", ::SDLK_DOLLAR },
	{ "ampersand", ::SDLK_AMPERSAND },
	{ "leftparen", ::SDLK_LEFTPAREN },
	{ "rightparen", ::SDLK_RIGHTPAREN },
	{ "asterisk", ::SDLK_ASTERISK },
	{ "plus", ::SDLK_PLUS },
	{ "minus", ::SDLK_MINUS },
	{ "period", ::SDLK_PERIOD },
	{ "comma", ::SDLK_COMMA },
	{ "slash", ::SDLK_SLASH },
	{ "colon", ::SDLK_COLON },
	{ "semicolon", ::SDLK_SEMICOLON },
	{ "less", ::SDLK_LESS },
	{ "equals", ::SDLK_EQUALS },
	{ "greater", ::SDLK_GREATER },
	{ "question", ::SDLK_QUESTION },
	{ "at", ::SDLK_AT },
	{ "capslock", ::SDLK_CAPSLOCK },

	{ "printscreen", ::SDLK_PRINTSCREEN },
	{ "scrolllock", ::SDLK_SCROLLLOCK },
	{ "pause", ::SDLK_PAUSE },
	{ "insert", ::SDLK_INSERT },
	{ "home", ::SDLK_HOME },
	{ "pageup", ::SDLK_PAGEUP },
	{ "pagedown", ::SDLK_PAGEDOWN },
	{ "delete", ::SDLK_DELETE },
	{ "end", ::SDLK_END },

	{ "right", ::SDLK_RIGHT },
	{ "left", ::SDLK_LEFT },
	{ "down", ::SDLK_DOWN },
	{ "up", ::SDLK_UP },

	{ "numlck_clear", ::SDLK_NUMLOCKCLEAR },
	{ "kp_divide", ::SDLK_KP_DIVIDE },
	{ "kp_multiply", ::SDLK_KP_MULTIPLY },
	{ "kp_minus", ::SDLK_KP_MINUS },
	{ "kp_plus", ::SDLK_KP_PLUS },
	{ "kp_enter", ::SDLK_KP_ENTER },
	{ "kp_0", ::SDLK_KP_0 },
	{ "kp_1", ::SDLK_KP_1 },
	{ "kp_2", ::SDLK_KP_2 },
	{ "kp_3", ::SDLK_KP_3 },
	{ "kp_4", ::SDLK_KP_4 },
	{ "kp_5", ::SDLK_KP_5 },
	{ "kp_6", ::SDLK_KP_6 },
	{ "kp_7", ::SDLK_KP_7 },
	{ "kp_8", ::SDLK_KP_7 },
	{ "kp_9", ::SDLK_KP_9 },
	{ "kp_period", ::SDLK_KP_PERIOD },
	{ "kp_equals", ::SDLK_KP_EQUALS },

	{ "application", ::SDLK_APPLICATION },
	{ "power", ::SDLK_POWER },

	{ "lctrl", ::SDLK_LCTRL },
	{ "lshift", ::SDLK_LSHIFT },
	{ "lalt", ::SDLK_LALT },
	{ "lgui", ::SDLK_LGUI },
	{ "rctrl", ::SDLK_RCTRL },
	{ "rshift", ::SDLK_RSHIFT },
	{ "ralt", ::SDLK_RALT },
	{ "rgui", ::SDLK_RGUI }

};

} // global

boost::program_options::options_description default_program_options()
{
	boost::program_options::options_description opt_desc;
	opt_desc.add_options()
		("help", "print this help message")
		("random-seed", boost::program_options::value<unsigned int>(), "set the random generator seed")
		("opengl-major", boost::program_options::value<int>()
			->default_value(PUP_GL_MAJOR), "major OpenGL version")
		("opengl-minor", boost::program_options::value<int>()
			->default_value(PUP_GL_MINOR), "minor OpenGL version")
		("config-path", boost::program_options::value<std::string>()
			->default_value(PUP_CONFIG_PATH), "set the config file path")
		("base-path", boost::program_options::value<std::string>()
			->default_value(boost::filesystem::current_path().string()), "set the base directory path")
		("tmp-path", boost::program_options::value<std::string>(), "set the temporary directory path")
	;
	return opt_desc;
}

} // pup

