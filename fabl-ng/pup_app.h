
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

#ifndef LIBPUP_PUP_APP_H
#define LIBPUP_PUP_APP_H

#include "pup_env.h"
#include "pup_core.h"

#include "pup_gl1.h"
#include "pup_snd.h"

namespace pup {

// A very simplistic tick-based timer.
class timer
{
public:
	explicit timer(const ::Uint32& tick = ::SDL_GetTicks()) throw() :
		first_tick_(tick),
		current_tick_(tick),
		last_tick_(tick),
		tick_delta_(0)
	{}

	inline void update() throw()
	{
		current_tick_ = ::SDL_GetTicks();
		tick_delta_ = current_tick_ - last_tick_;
		last_tick_ = current_tick_;
	}

	inline ::Uint32 delta() const throw() { return tick_delta_; }
	inline ::Uint32 last() const throw() { return last_tick_; }
	inline ::Uint32 total() const throw() { return last_tick_ - first_tick_; }

	inline double delta_sec() const throw() { return this->delta() / 1000.0; }
	inline double last_sec() const throw() { return this->last() / 1000.0; }
	inline double total_sec() const throw() { return this->total() / 1000.0; }

private:
	::Uint32 first_tick_;
	::Uint32 current_tick_;
	::Uint32 last_tick_;
	::Uint32 tick_delta_;
};

// Detect time intervals.
class interval :
	private boost::noncopyable
{
public:
	explicit interval(timer& t, const ::Uint32 d) throw() :
		timer_(t),
		tick_(t.last()),
		delta_(d)
	{}

	explicit interval(timer& t, const double d) throw() :
		interval(t, static_cast<::Uint32>(d * 1000.0))
	{}

	explicit interval(timer& t, const int d) throw() :
		interval(t, static_cast<::Uint32>(d))
	{}
	
	inline bool test_expired() throw()
	{
		return timer_.last() >= tick_ + delta_;
	}

	inline bool expired() throw()
	{
		if (this->test_expired()) {
			this->renew();
			return true;
		}
		return false;
	}

	inline void renew() throw() { tick_ = timer_.last(); }

	inline void set_delta(double d) throw() { delta_ = static_cast<::Uint32>(d * 1000.0); }
	inline void set_delta(::Uint32 d) throw() { delta_ = d; }
	
	inline double delta_sec() const throw() { return delta_ / 1000.0; }
	inline ::Uint32 delta() const throw() { return delta_; }
	
	inline double remaining_sec() const throw() { return this->remaining() / 1000.0; }
	inline ::Uint32 remaining() const throw()
	{
		::Sint64 diff = static_cast<::Sint32>(tick_)
			+ static_cast<::Sint32>(delta_)
			- static_cast<::Sint32>(timer_.last());
		return static_cast<::Uint32>(diff < 0 ? 0 : diff);
	}
	
private:
	timer& timer_;
	::Uint32 tick_;
	::Uint32 delta_;
	
};

// Responsible for handling key rebindings and dispatching
// key presses to appropriate controller callbacks. Modifiers
// are currently not supported.
class key_dispatcher :
	private boost::noncopyable
{
public:
	typedef boost::function<bool (::SDL_Event& event)> key_function;
	typedef std::map<::SDL_Keycode, key_function> dispatch_map;

	explicit key_dispatcher(application& app);
	~key_dispatcher() throw();

	void add_bindable(key_function func, const std::string& name,
		const std::string& section);

	bool handle(::SDL_Event& event);

private:
	application& app_;
	dispatch_map dispatch_map_;
};

// The controller is responsible for handling application
// events. Each controller queued for use by an application
// will be used for at least one rendered frame.
class controller :
	private boost::noncopyable,
	public reactor
{
public:
	explicit controller(application& app);
	virtual ~controller() throw();

	virtual void init() {}
	virtual void prepare() {}

	virtual bool react(::SDL_Event& event);
	virtual void think() = 0;
	virtual void render() = 0;

	virtual const char* get_name() const throw() = 0;

	virtual application& get_app() { return app_; }
	virtual key_dispatcher& get_key_dispatcher() { return key_dispatcher_; }
	virtual gl1::d3::view& get_view() { return view_; }

protected:
	application& app_;
	key_dispatcher key_dispatcher_;
	gl1::d3::view view_;
};

typedef std::shared_ptr<controller> controller_ptr;
typedef std::queue<controller_ptr> controller_queue;

// The application is responsible for generating events for
// the active controller.
class application :
	private boost::noncopyable
{
public:
	explicit application(::Uint32 sdl_flags, const boost::program_options::variables_map& opt_vm);
	virtual ~application() throw();

	virtual void configure();
	virtual void loop();
	virtual void misc();

	virtual void before_render();
	virtual void after_render();

	virtual void before_loop();
	virtual void after_loop();

	virtual void stop() throw() { loop_ = false; }

	snd::music& get_music() throw() { return *music_; }
	snd::jukebox& get_jukebox() throw() { return *jukebox_; }
	snd::soundboard& get_soundboard() throw() { return *soundboard_; }

	timer& get_timer() throw() { return timer_; }
	interval& get_misc_interval() throw() { return misc_interval_; }
	interval& get_status_interval() throw() { return status_interval_; }
	
	boost::asio::io_service& get_io_service() throw() { return io_service_; }

	boost::property_tree::ptree& get_ptree() throw() { return pt_; }
	boost::program_options::variables_map& get_opt_vm() throw() { return opt_vm_; }
	
	const std::string& get_config_path() { return opt_vm_["config-path"].as<std::string>(); }

	gl1::ft::typewriter& get_typewriter() throw() { return typewriter_; }

	void get_window_size(int* w, int* h);
	void get_window_pos(int* x, int* y);

	virtual controller_ptr get_start_controller() = 0;
	virtual controller_ptr get_current_controller();

	void queue_controller(controller_ptr ctrlr);
	
protected:
	::SDL_Window* window_;
	::SDL_Surface* window_surface_;

	::GLuint vertex_array_id_;
	::SDL_GLContext gl_context_;

	bool loop_;
	bool first_config_;

	::Uint32 frame_delay_;
	::Uint32 frame_count_;
	::Uint32 frames_per_second_;
	::Uint32 rendered_frames_;
	::Uint32 event_tick_lim_;

	int gl_major_;
	int gl_minor_;

	boost::asio::io_service io_service_;

	boost::property_tree::ptree pt_;
	boost::program_options::variables_map opt_vm_;
	
	snd::music* music_;
	snd::jukebox* jukebox_;
	snd::soundboard* soundboard_;

	timer timer_;
	interval misc_interval_;
	interval status_interval_;
	controller_queue controller_queue_;

	gl1::ft::typewriter typewriter_;
};

// Helps reduce the amount of code duplication needed for simple
// ui form controllers.
template <
	class Theme,
	class Form
>
class form_controller :
	public controller
{
public:
	explicit form_controller(application& app) :
		controller(app),
		theme_(app.get_typewriter()),
		form_(*this, theme_)
	{
	}

	virtual ~form_controller() throw()
	{
	}

	virtual bool react(::SDL_Event& event)
	{
		return controller::react(event)?
			true: form_.react(event);
	}

	virtual void think()
	{
		form_.think();
	}

	virtual void render()
	{
		view_.clear();
		view_.look();

		gl1::d2::scoped_screen_coordinate_matrix ssc_matrix;

		form_.render();
	}

protected:
	Theme theme_;
	Form form_;
};

/**
 * Implemented by the user.
 * @{
 */
const char* application_name() throw();
const char* application_version() throw();
/**@}*/

// Run an application.
template <class Application, typename CharT>
int run(int argc, CharT* argv[], boost::program_options::options_description& opt_desc,
	::Uint32 sdl_flags = PUP_SDL_DEFAULT_INIT)
{
	try {
		boost::program_options::variables_map opt_vm;
		boost::program_options::store(
			boost::program_options::parse_command_line(
				argc,
				argv,
				opt_desc
			),
			opt_vm
		);

		boost::program_options::notify(opt_vm);
		Application(sdl_flags, opt_vm).loop();
	}
	catch (const std::exception& e) {
		BOOST_LOG_TRIVIAL(fatal) << boost::format("[%1%] %2%")
			% errno % e.what();
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}

} // pup

#endif
