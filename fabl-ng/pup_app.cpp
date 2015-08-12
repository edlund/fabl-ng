
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

key_dispatcher::key_dispatcher(application& app) :
	app_(app)
{
}

key_dispatcher::~key_dispatcher() throw()
{
}

void key_dispatcher::add_bindable(key_function func, const std::string& name,
	const std::string& section)
{
	std::string i = boost::str(boost::format("%1%.%2%") % section % name);
	std::string key = app_.get_ptree().get<std::string>(i);
	auto pair = global::keycodes.find(key);
	if (pair == global::keycodes.end()) {
		PUP_ERR(std::runtime_error, boost::str(boost::format("binding %1%.%2% specifies invalid key %3%")
			% section % name % key));
	}

	dispatch_map_.insert(dispatch_map::value_type(pair->second, func));
}

bool key_dispatcher::handle(::SDL_Event& event)
{
	if (event.type == SDL_KEYDOWN) {
		auto pair = dispatch_map_.find(event.key.keysym.sym);
		if (pair != dispatch_map_.end())
			return pair->second(event);
	}
	return false;
}

controller::controller(application& app) :
	app_(app),
	key_dispatcher_(app)
{
}

controller::~controller() throw()
{
}

bool controller::react(::SDL_Event& event)
{
	return key_dispatcher_.handle(event);
}

application::application(
	::Uint32 sdl_flags,
	const boost::program_options::variables_map& opt_vm
) :
	window_(nullptr),
	window_surface_(nullptr),
	loop_(true),
	first_config_(true),
	frame_delay_(0),
	frame_count_(0),
	frames_per_second_(0),
	rendered_frames_(0),
	event_tick_lim_(64),
	opt_vm_(opt_vm),
	music_(nullptr),
	jukebox_(nullptr),
	soundboard_(nullptr),
	misc_interval_(timer_, 250),
	status_interval_(timer_, 1000)
{
	if (::SDL_Init(sdl_flags) < 0)
		PUP_ERR(std::runtime_error, ::SDL_GetError());
	
	if (::Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
		PUP_ERR(std::runtime_error, ::Mix_GetError());
	
	gl_major_ = opt_vm_["opengl-major"].as<int>();
	gl_minor_ = opt_vm_["opengl-minor"].as<int>();

	::SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, gl_major_);
	::SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, gl_minor_);
	
	window_ = ::SDL_CreateWindow(
		application_name(),
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		640,
		480,
		SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL
	);
	
	if (!window_)
		PUP_ERR(std::runtime_error, ::SDL_GetError());
	
	window_surface_ = ::SDL_GetWindowSurface(window_);
	gl_context_ = ::SDL_GL_CreateContext(window_);

	if (!gl_context_)
		PUP_ERR(std::runtime_error, ::SDL_GetError());
	
	::glewExperimental = GL_TRUE;
	GLenum glew_err = ::glewInit();
	if (glew_err != GLEW_OK) {
		PUP_ERR(std::runtime_error, reinterpret_cast<const char*>
			(::glewGetErrorString(glew_err)));
	}

	::glGenVertexArrays(1, &vertex_array_id_);
	::glBindVertexArray(vertex_array_id_);

	music_ = new snd::music();
	jukebox_ = new snd::jukebox();
	soundboard_ = new snd::soundboard();
	if (!(music_ && jukebox_ && soundboard_)) {
		PUP_ERR(std::runtime_error, "out of memory");
	}

	::SDL_version linked;
	::SDL_GetVersion(&linked);
	
	std::printf("%s(v.%s), SDL(v.%d.%d), OpenGL(v.%d.%d)\n",
		application_name(),
		application_version(),
		linked.major,
		linked.minor,
		gl_major_,
		gl_minor_
	);
}

application::~application() throw()
{
	delete music_;
	delete jukebox_;
	delete soundboard_;

	::glDeleteVertexArrays(1, &vertex_array_id_);
	::SDL_DestroyWindow(window_);
	::Mix_Quit();
	::SDL_Quit();
}

void application::configure()
{
	boost::filesystem::path config(this->get_config_path());
	boost::filesystem::path config_orig(PUP_CONFIG_PATH "-orig");

	if (!boost::filesystem::exists(config)) {
		if (!boost::filesystem::exists(config_orig)) {
			PUP_ERR(std::runtime_error, "can not find original config file");
		}
		boost::filesystem::copy_file(config_orig, config);
		BOOST_LOG_TRIVIAL(warning) << boost::format("failed to load config \"%1%\""
			" copying original config")	% config.string();
	}

	pt_.clear();
	boost::property_tree::ini_parser::read_ini(config.string(), pt_);

	unsigned int vsync = pt_.get<unsigned int>("graphics.vsync");
	unsigned int seed = opt_vm_.count("random-seed")? opt_vm_["random-seed"].as<unsigned int>():
		pt_.get<unsigned int>("general.random_seed")? pt_.get<unsigned int>("general.random_seed"):
			static_cast<unsigned int>(std::time(nullptr));
	
	std::srand(seed);

	::SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	::SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
	::SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 4);

	music_->set_volume(pt_.get<int>("sound.music_volume"));

	if (::SDL_GL_SetSwapInterval(vsync == 1) < 0) {
		BOOST_LOG_TRIVIAL(warning) << boost::format("failed to set v-sync: %1%")
			% ::SDL_GetError();
	}

	if (first_config_) {
		first_config_ = false;

		::glShadeModel(GL_SMOOTH);
#ifdef PUP_NIX
		::glEnable(GL_LINE_SMOOTH);
		::glEnable(GL_POLYGON_SMOOTH);
#endif
		::glClearDepth(1.0f);

		::glEnable(GL_DEPTH_TEST);
		::glDepthFunc(GL_LEQUAL);
		::glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		::glEnable(GL_CULL_FACE);

		::glEnable(GL_BLEND);
		::glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		::glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

		::glEnable(GL_LIGHTING);
		::glEnable(GL_COLOR_MATERIAL);
	}

	int width = pt_.get<int>("graphics.window_width");
	int height = pt_.get<int>("graphics.window_height");

	if (width < 1 || height < 1)
		PUP_ERR(std::runtime_error, "invalid window dimensions");

	::SDL_SetWindowSize(window_, width, height);
	::SDL_SetWindowPosition(window_, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
	::glViewport(0, 0, width, height);
	
	::Uint32 fullscreen = pt_.get<bool>("graphics.window_fullscreen")?
		SDL_WINDOW_FULLSCREEN: 0;
	
	if (::SDL_SetWindowFullscreen(window_, fullscreen) != 0) {
		PUP_ERR(std::runtime_error, boost::str(
			boost::format("failed to set fullscreen: %1%")
				% ::SDL_GetError()
		));
	}
	
	const float fov = pt_.get<float>("graphics.fov");
	const float ratio = static_cast<float>(width) / static_cast<float>(height);
	const float z_near = pt_.get<float>("graphics.z_near");
	const float z_far = pt_.get<float>("graphics.z_far");

	::glMatrixMode(GL_PROJECTION);
	::glLoadIdentity();
	::gluPerspective(
		fov,
		ratio,
		z_near,
		z_far
	);

	::glMatrixMode(GL_MODELVIEW);
	::glLoadIdentity();

	::glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	::glClear(GL_COLOR_BUFFER_BIT);

	BOOST_LOG_TRIVIAL(info)	<< boost::format(
		"pup::application::configure()\n"
		"\tseed=%1%\n"
		"\tw=%2%, h=%3%\n"
		"\tfov=%4%, ratio=%5%, vsync=%6%, fullscreen=%7%\n"
	)
		% seed
		% width
		% height
		% fov
		% ratio
		% vsync
		% (fullscreen != 0);
}

void application::loop()
{
	this->configure();
	this->queue_controller(this->get_start_controller());
	
	this->before_loop();

	::SDL_Event event;

	while (loop_) {
		timer_.update();

		if (controller_queue_.empty() || !controller_queue_.front().get())
			PUP_ERR(std::runtime_error, "missing controller");
		
		controller_queue_.front()->prepare();

		while (
			::SDL_PollEvent(&event) != 0 &&
			::SDL_GetTicks() - timer_.last() <= event_tick_lim_
		) {
			if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
				this->stop();
				break;
			}
			if (controller_queue_.front()->react(event))
				break;
		}
		controller_queue_.front()->think();

		this->before_render();
		controller_queue_.front()->render();
		this->after_render();

		if (misc_interval_.expired())
			this->misc();
		if (controller_queue_.size() > 1)
			controller_queue_.pop();
	}

	this->after_loop();
}

void application::misc()
{
	jukebox_->update(*music_);
}

void application::before_render()
{
}

void application::after_render()
{
	::SDL_GL_SwapWindow(window_);
	
	frame_count_++;
	frames_per_second_ = static_cast<::Uint32>(
		++rendered_frames_ / timer_.total_sec()
	);
	
	if (status_interval_.test_expired()) {
		::SDL_SetWindowTitle(
			window_,
			boost::str(boost::format("%1% [approx_fps=%2%, avg_fps=%3%, frames=%4%, ctrlr=%5%]")
				% application_name()
				% frame_count_
				% frames_per_second_
				% rendered_frames_
				% controller_queue_.front()->get_name()
			).c_str()
		);

		frame_count_ = 0;
		status_interval_.renew();
	}

	if (frame_delay_)
		::SDL_Delay(frame_delay_);
}

void application::before_loop()
{
}

void application::after_loop()
{
}

void application::get_window_size(int* w, int* h)
{
	::SDL_GetWindowSize(window_, w, h);
}

void application::get_window_pos(int* x, int* y)
{
	::SDL_GetWindowPosition(window_, x, y);
}

controller_ptr application::get_current_controller()
{
	return controller_queue_.front();
}

void application::queue_controller(controller_ptr ctrlr)
{
	controller_queue_.push(ctrlr);
	ctrlr->init();
}

} // pup
