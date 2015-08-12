
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

#ifndef LIBPUP_PUP_SND_H
#define LIBPUP_PUP_SND_H

#include "pup_env.h"
#include "pup_core.h"

#include "pup_io.h"

namespace pup {
namespace snd {

// Play a sound.
class sound :
	private boost::noncopyable
{
public:
	sound(const boost::filesystem::path& path);
	~sound() throw();

	bool play(int channel = -1, int loop = 0) throw();

private:
	::Mix_Chunk* chunk_;
};

typedef std::shared_ptr<sound> sound_ptr;

class soundboard :
	private boost::noncopyable
{
public:
	typedef std::map<std::string, sound_ptr> sound_map;

	soundboard();
	~soundboard() throw();

	void load_file(const boost::filesystem::path& path, const std::string& name);

	bool play(const std::string& name, int channel = -1, int loop = 0);

private:
	sound_map sounds_;
};

// Play music.
class music	:
	private boost::noncopyable
{
public:
	music();
	~music() throw();

	void start(const boost::filesystem::path& path, int repeat = 1, int fadein = 0);
	void stop() throw();

	bool playing() const throw();

	void fadeout(int duration) throw();

	void set_volume(int volume);
	int get_volume() const throw();

private:
	::Mix_Music* track_;
};

typedef std::shared_ptr<music> music_ptr;

class jukebox :
	private boost::noncopyable
{
public:
	jukebox();
	~jukebox() throw();

	::Uint32 load_dir(const boost::filesystem::path& dir);
	::Uint32 load_files(const io::path_list& files);

	void update(music& mus);

	inline void start() throw() { running_ = true; }
	inline void stop() throw() { running_ = false; }

	inline void set_random(const bool r) throw() { random_ = r; }

	inline void start_mix() throw()
	{
		this->set_random(true);
		this->start();
	}

	inline void stop_mix() throw()
	{
		this->stop();
		this->set_random(false);
	}

private:
	::Uint32 filter_files();

	bool running_;
	bool random_;
	int history_lim_;
	io::path_list history_;
	io::path_list files_;
	io::path_list::iterator track_;
};

} // snd
} // pup

#endif
