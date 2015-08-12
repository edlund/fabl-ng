
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

#include "pup_snd.h"

namespace pup {
namespace snd {

sound::sound(const boost::filesystem::path& path) :
	chunk_(::Mix_LoadWAV(path.string().c_str()))
{
	if (!chunk_) {
		PUP_ERR(std::runtime_error, boost::str(boost::format(
			"failed to load file \"%1%\""
		) % path));
	}
}

sound::~sound() throw()
{
	::Mix_FreeChunk(chunk_);
}

bool sound::play(int channel, int loop) throw()
{
	return ::Mix_PlayChannel(channel, chunk_, loop) != -1;
}

soundboard::soundboard()
{
}

soundboard::~soundboard() throw()
{
}

void soundboard::load_file(const boost::filesystem::path& path, const std::string& name)
{
	if (!sounds_.insert(sound_map::value_type(
		name,
		sound_ptr(new sound(path)))
	).second) {
		PUP_ERR(std::runtime_error, boost::str(boost::format(
			"name collision, sound with name \"%1%\" already exist"
		) % name));
	}
}

bool soundboard::play(const std::string& name, int channel, int loop)
{
	auto it = sounds_.find(name);
	if (it != sounds_.end()) {
		return it->second->play(channel, loop);
	}
	return false;
}

music::music() :
	track_(nullptr)
{
}

music::~music() throw()
{
	this->stop();
}

void music::start(const boost::filesystem::path& path, int repeat, int fadein)
{
	this->stop();

	if (!boost::filesystem::is_regular_file(path))
		PUP_ERR(std::runtime_error, "given path is not a file");

	if (!(track_ = ::Mix_LoadMUS(path.string().c_str())))
		PUP_ERR(std::runtime_error, ::Mix_GetError());

	::Mix_FadeInMusic(track_, repeat, fadein);
}

void music::stop() throw()
{
	if (track_) {
		::Mix_HaltMusic();
		::Mix_FreeMusic(track_);
		track_ = nullptr;
	}
}

bool music::playing() const throw()
{
	return ::Mix_PlayingMusic() == 1;
}

void music::fadeout(int duration) throw()
{
	if (track_) {
		::Mix_FadeOutMusic(duration);
	}
}

void music::set_volume(int volume)
{
	::Mix_VolumeMusic(volume);
}

int music::get_volume() const throw()
{
	return ::Mix_VolumeMusic(-1);
}

jukebox::jukebox() :
	running_(false),
	random_(false),
	history_lim_(3)
{
	track_ = files_.end();
}

jukebox::~jukebox() throw()
{
}

::Uint32 jukebox::load_dir(const boost::filesystem::path& dir)
{
	if (!boost::filesystem::is_directory(dir)) {
		PUP_ERR(std::runtime_error, boost::str(boost::format(
			"path \"%1%\" is not a directory") % dir.string()
		));
	}
	files_.clear();
	std::copy(
		boost::filesystem::directory_iterator(dir),
		boost::filesystem::directory_iterator(),
		std::back_inserter(files_)
	);
	return this->filter_files();
}

::Uint32 jukebox::load_files(const io::path_list& files)
{
	track_ = files_.end();
	history_.clear();
	files_.clear();
	std::copy(
		files.begin(),
		files.end(),
		std::back_inserter(files_)
	);
	return this->filter_files();
}

void jukebox::update(music& mus)
{
	if (running_ && !mus.playing() && !files_.empty()) {
		if (random_) {
			io::path_list::iterator it;
			do {
				it = pup::random_element(files_.begin(), files_.end());
			} while (
				files_.size() > history_.size() + 1 &&
				std::find(history_.begin(), history_.end(), *it) != history_.end()
			);
			mus.start(*it);
			history_.push_back(*it);
			if (static_cast<int>(history_.size()) == history_lim_) {
				history_.pop_front();
			}
			track_ = it;
		} else {
			if (track_ == files_.end()) {
				track_ = files_.begin();
			}
			auto it = track_++;
			mus.start(*it);
		}
	}
}

::Uint32 jukebox::filter_files()
{
	for (auto it = files_.begin(); it != files_.end(); /* in-loop */) {
		if (
			boost::filesystem::is_regular_file(*it) && (
				it->extension().string() == ".mp3" ||
				it->extension().string() == ".ogg" ||
				it->extension().string() == ".wav"
			)
		) {
			++it;
		} else {
			it = files_.erase(it);
		}
	}
	return files_.size();
}

} // snd
} // pup
