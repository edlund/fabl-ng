
/* FABL-ng - FAlling BLocks Next Generation
 * Copyright 2015 (C) Erik Edlund <erik.edlund@32767.se>
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef FABL_FABL_H
#define FABL_FABL_H

#include "pup.h"

#include "sqlite3.h"
#include "SQLiteCpp/SQLiteCpp.h"

#define APPLICATION_VERSION "1.1.0(BoringBlock)"
#define APPLICATION_NAME "FallingBlocks_NextGeneration"

// FABL_VERSION % 100 is the patch level
// FABL_VERSION / 100 % 1000 is the minor version
// FABL_VERSION / 100000 is the major version
#define FABL_VERSION 100100

#define FABL_MAJOR (FABL_VERSION / 100000)
#define FABL_MINOR (FABL_VERSION / 100 % 1000)
#define FABL_PATCH (FABL_VERSION % 100)

namespace fabl {

class blockfall;

class mainmenu_controller;
class game_controller;
class gameover_controller;

namespace ng {

enum block_state {
	ST_HOLLOW,
	ST_FILLED
};

class block
{
public:
	explicit block() throw() :
		x(0),
		y(0),
		z(0),
		unique_(0),
		st_(ST_HOLLOW)
	{}
	
	pup::gl1::rgb& get_color() throw() { return col_; }
	const pup::gl1::rgb& get_color() const throw() { return col_; }
	void set_color(const pup::gl1::rgb& col) throw() { col_ = col; }
	
	void set_state(const block_state st) throw() { st_ = st; }
	block_state get_state() const throw() { return st_; }
	
	bool need_render() const throw() { return st_ == ST_FILLED; }
	bool is_solid() const throw() { return st_ == ST_FILLED; }
	
	void set_unique(::Uint32 id) throw() { unique_ = id; }
	::Uint32 get_unique() const throw() { return unique_; }
	
	void clear() throw()
	{
		unique_ = 0;
		st_ = ST_HOLLOW;
	}

	::Uint32 x;
	::Uint32 y;
	::Uint32 z;

private:
	::Uint32 unique_;
	block_state st_;
	pup::gl1::rgb col_;
};

typedef std::set<ng::block*> block_addr_set;
typedef std::map<ng::block*, block_addr_set> block_addr_map;

enum shape_rotation {
	RT_XZ,
	RT_XY,
	RT_YZ
};

class shape :
	private boost::noncopyable
{
public:
	typedef boost::function<void ()> rotation_function;
	typedef std::list<rotation_function> rotation_list;

	explicit shape(
		const ::Uint32 id,
		const ::Uint32 side,
		const ::Uint32 highdice,
		const ::Uint32 lifts,
		const bool rotatable,
		const std::string& name,
		const bool shallow = false
	);
	virtual ~shape() throw();

	block& get_block(::Uint32 x, ::Uint32 y, ::Uint32 z);
	block* get_blocks() { return blocks_; }

	pup::gl1::d3::point& get_position() { return pos_; }
	
	::Uint32 get_id() const { return id_; }
	::Uint32 get_side() const { return side_; }
	::Uint32 get_volume() const { return side_ * side_ * side_; }

	::Uint32& get_lifts() { return lifts_; }

	std::string& get_name() { return name_; }

	void set_id(::Uint32 id);

	void set_name(const std::string& n) { name_ = n; }
	
	void set_color(const pup::gl1::rgb& col);
	const pup::gl1::rgb& get_color() const { return col_; }
	

	bool get_rotatable() const { return rotatable_; }
	void set_rotatable(bool r) { rotatable_ = r; }

	int get_highdice() const { return highdice_; }

	shape* clone(const bool shallow = false) const;

	void freeze_to(block* blocks, ::Uint32 x_l, ::Uint32 y_l, ::Uint32 z_l);

#define SHAPE_ROTATION(Dim, Dir) \
	inline void rotate_##Dim##_##Dir() \
	{ \
		if (rotatable_) \
			pup::t::rot_##Dir##90_3d_##Dim(blocks_, side_); \
	}
	
	SHAPE_ROTATION(xz, cw) // rotate_xz_cw
	SHAPE_ROTATION(xz, ccw) // rotate_xz_ccw

	SHAPE_ROTATION(xy, cw) // rotate_xy_cw
	SHAPE_ROTATION(xy, ccw) // rotate_xy_ccw

	SHAPE_ROTATION(yz, cw) // rotate_yz_cw
	SHAPE_ROTATION(yz, ccw) // rotate_yz_ccw

#undef SHAPE_ROTATION
	
	void random_rotation()
	{
		rotation_function rotfun = *pup::random_element(
			rotations_.begin(),
			rotations_.end()
		);

		rotfun();
	}

	inline bool operator==(const shape& rhs)
	{
		return id_ == rhs.id_;
	}

protected:
	::Uint32 id_;
	std::string name_;
	rotation_list rotations_;
	bool rotatable_;
	bool shallow_;
	::Uint32 side_;
	::Uint32 highdice_;
	::Uint32 lifts_;
	block* blocks_;
	pup::gl1::d3::point pos_;
	pup::gl1::rgb col_;
};

typedef std::shared_ptr<shape> shape_ptr;
typedef std::list<shape_ptr> shape_list;

class power :
	private boost::noncopyable
{
public:
	explicit power(blockfall& bfall, pup::timer& tmr, ::Uint32 charge_time);
	virtual ~power() throw();
	
	virtual void levelup() {}
	virtual void render() {}
	virtual void update() = 0;
	
	virtual bool activate();
	virtual void reset();
	
	virtual std::string get_info();
	
	virtual const char* get_name() const throw() = 0;
	
protected:
	blockfall& blockfall_;
	pup::interval charge_interval_;
	ng::block* blocks_;
	bool active_;
	::Uint32 animate_frames_;
	::Uint32 layer_area_;
};

typedef std::shared_ptr<power> power_ptr;
typedef std::list<power_ptr> power_list;

class kill_shape_power :
	public power
{
public:
	explicit kill_shape_power(blockfall& bfall, pup::timer& tmr);
	virtual ~kill_shape_power() throw();
	
	virtual void update();
	
	virtual const char* get_name() const throw() { return "KillShape"; }
};

class clear_color_power :
	public power
{
public:
	explicit clear_color_power(blockfall& bfall, pup::timer& tmr);
	virtual ~clear_color_power() throw();
	
	virtual void render();
	virtual void update();
	
	virtual bool activate();
	
	virtual const char* get_name() const throw() { return "ClearColor"; }
	
protected:
	pup::gl1::rgb clear_color_;
	::Uint32 y_;
	::Uint32 n_;
};

class vaporize_foundation_power :
	public power
{
public:
	explicit vaporize_foundation_power(blockfall& bfall, pup::timer& tmr);
	virtual ~vaporize_foundation_power() throw();
	
	virtual void render();
	virtual void update();
	
	virtual const char* get_name() const throw() { return "VaporizeFoundation"; }
};

class spike_trap_power :
	public power
{
public:
	explicit spike_trap_power(blockfall& bfall, pup::timer& tmr);
	virtual ~spike_trap_power() throw();
	
	virtual void levelup();
	virtual void update();
	
	virtual const char* get_name() const throw() { return "SpikeTrap"; }
	
protected:
	bool restore_gm_;
	bool gm_bottom_mesh_;
};

class gravity_beam_power :
	public power
{
public:
	explicit gravity_beam_power(blockfall& bfall, pup::timer& tmr);
	virtual ~gravity_beam_power() throw();
	
	virtual void render();
	virtual void update();
	
	virtual const char* get_name() const throw() { return "GravityBeam"; }
	
protected:
	virtual void drop_at(::Uint32 x, ::Uint32 y, ::Uint32 z);
	
	::Uint32 x_;
	::Uint32 z_;
};

class factory :
	private boost::noncopyable
{
public:
	typedef std::list<pup::gl1::rgb> rgb_list;

	explicit factory(
		const boost::filesystem::path& world_file,
		bool colorblind_rg_match = false,
		bool verbose = false
	);

	void setup();

	shape_ptr create();

	::Uint32 get_world_xz() const { return xz_; }
	::Uint32 get_world_y() const { return y_; }

	void set_random_colors(bool rc) { this->random_colors_ = rc; }

	const boost::filesystem::path& get_world_file() const { return world_file_; }

private:
	boost::property_tree::ptree pt_;
	boost::filesystem::path world_file_;
	boost::random::mt19937 rng_;
	boost::random::uniform_int_distribution<> unidist_;

	rgb_list colors_;
	shape_list shapes_;

	::Uint32 next_id_;

	::Uint32 xz_;
	::Uint32 y_;

	bool random_colors_;
};

} // ng

void pillarize(blockfall& bfall, ::Uint32 modv, ::Uint32 height);

struct highscore
{
	highscore() :
		id(0),
		crc(0),
		fabl(0),
		created("")
	{}

	::sqlite_int64 id;
	::sqlite_int64 crc;
	::sqlite_int64 fabl;
	std::string created;
	std::string world;
	std::string name;
	::sqlite_int64 points;
	::sqlite_int64 steps;
	::sqlite_int64 levels;
	::sqlite_int64 lines;
	::sqlite_int64 shapes;
	::sqlite_int64 powers;
	bool gm_bottom_mesh;
	bool gm_zero_g;
	bool gm_color_match;
	bool gm_one_shape_limit;
};

class blockfall :
	private boost::noncopyable
{
public:
	typedef pup::string_list string_list;
	typedef std::list<std::pair<::Sint64, std::string>> skill_list;
	typedef boost::function<void ()> gameover_function;
	typedef boost::function<void ()> sndfx_function;
	
	explicit blockfall(
		pup::timer& tmr,
		const gameover_function& gameover,
		const sndfx_function& sweepsnd,
		const sndfx_function& lvlupsnd,
		const boost::filesystem::path& shapes_file,
		const bool colorblind_rg_match,
		const bool gm_bottom_mesh = false,
		const bool gm_zero_g = false,
		const bool gm_color_match = false,
		const bool gm_one_shape_limit = false
	);
	virtual ~blockfall() throw();

	ng::block& get_world_block(::Uint32 x, ::Uint32 y, ::Uint32 z);
	ng::block* get_world_blocks() throw() { return world_blocks_; }
	
	ng::factory& get_factory() { return factory_; }
	ng::power_list& get_powers() { return powers_; }
	ng::shape_list& get_shapes() { return shapes_; }
	
	ng::power_ptr& get_kill_shape_power() { return kill_shape_power_; }
	ng::power_ptr& get_clear_color_power() { return clear_color_power_; }
	ng::power_ptr& get_vaporize_foundation_power() { return vaporize_foundation_power_; }
	ng::power_ptr& get_spike_trap_power() { return spike_trap_power_; }
	ng::power_ptr& get_gravity_beam_power() { return gravity_beam_power_; }
	
	::Uint32 get_lines_cleared() const { return lines_cleared_; }

	::Uint32 get_skill_level() const { return skill_level_; }
	::Sint64 get_skill_points() const { return skill_points_; }
	::Sint64 get_skill_ui_points() const { return skill_ui_points_; }
	::Uint32 get_xz() const { return xz_; }
	::Uint32 get_y() const { return y_; }
	
	pup::interval& get_msg_interval() { return msg_interval_; }
	pup::interval& get_step_interval() { return step_interval_; }
	pup::interval& get_spawn_interval() { return spawn_interval_; }

	void set_gm_bottom_mesh(bool v) { gm_bottom_mesh_ = v; }
	bool get_gm_bottom_mesh() { return gm_bottom_mesh_; }

	void set_gm_zero_g(bool v) { gm_zero_g_ = v; }
	bool get_gm_zero_g() { return gm_zero_g_; }

	void set_gm_color_match(bool v) { factory_.set_random_colors((gm_color_match_ = v)); }
	bool get_gm_color_match() { return gm_color_match_; }

	void set_gm_one_shape_limit(bool v) { gm_one_shape_limit_ = v; }
	bool get_gm_one_shape_limit() { return gm_one_shape_limit_; }

	virtual void create_world();
	virtual void destroy_world() throw();

	virtual void block_fall();

	virtual void rotate_shape_cw();
	virtual void rotate_shape_ccw();

	virtual void change_rotation_mode();
	ng::shape_rotation get_rotation_mode() const { return rtmode_; }

	std::string get_rotation_mode_desc() const
	{
		if (rtmode_ == ng::RT_XZ) { return "(X, Z)"; }
		if (rtmode_ == ng::RT_XY) { return "(X, Y)"; }
		if (rtmode_ == ng::RT_YZ) { return "(Y, Z)"; }
		return "(?, ?)";
	}

	virtual bool move_shape(double dX, double dY, double dZ);
	virtual bool move_shape(double dX, double dY, double dZ, ng::shape_ptr s_ptr);
	virtual bool collide_shape(double dX, double dY, double dZ, ng::shape_ptr s_ptr,
		ng::shape_ptr ignore_ptr = nullptr, bool world_only = false);

	virtual bool next_active();
	virtual bool prev_active();
	
	virtual bool has_active() { return !shapes_.empty(); }
	virtual ng::shape& get_active() { return *(*active_); }
	virtual ng::shape_list::iterator& get_active_it() { return active_; }
	
	virtual bool kill_active()
	{
		if (active_ != shapes_.end()) {
			active_ = shapes_.erase(active_);
			return true;
		}
		return false;
	}
	
	virtual void render();
	virtual void update();

	virtual bool update_move();
	virtual bool update_freeze(double dY = 1.0);
	virtual void update_sweep();

	virtual bool sweep_rows();
	virtual bool sweep_islands();
	virtual bool sweep_shapes();

	void clear();

	void simplify(pup::int_vector& dest) const;
	void build(const pup::int_vector& world);
	bool compare(const pup::int_vector& world);

	double scale(::Uint32 x) const { return static_cast<double>(x) / 4.0; }

	pup::string_list& get_flash_messages() { return flash_msgs_; }
	pup::string_list& get_skill_messages() { return skill_msgs_; }
	virtual ::Uint32 get_max_skill_messages() const { return 10; }

	inline void add_flash_message(const std::string& msg)
	{
		flash_msgs_.push_back(msg);
	}

	inline string_list::iterator get_flash_message_iterator()
	{
		if (!flash_msgs_.empty()) {
			if (flash_msg_frame_++ < flash_msg_frames_) {
				return flash_msgs_.begin();
			} else {
				flash_msg_frame_ = 0;
				flash_msgs_.pop_front();
			}
		}
		return flash_msgs_.end();
	}

	inline void add_skill_points(const ::Sint64 points) { skill_points_ += points; }
	inline void add_skill_message(const ::Sint64 points, const std::string& msg)
	{
		const std::string prefix(points >= 0? "+": "");
		waiting_skill_msgs_.push_front(skill_list::value_type(
			points,
			boost::str(boost::format("%1$09i: %2%%3% sp, %4%")
				% skill_steps_
				% prefix
				% points
				% msg
			)
		));
	}
	
	virtual void power_activated()
	{
		this->add_flash_message("<POWER>");
		powers_activated_++;
	}
	
	virtual pup::gl1::d3::point get_scaled_origo() const;
	virtual ::GLdouble get_scaled_side() const;
	
	static void checksum(highscore& hs);
	highscore get_highscore() const;

	virtual void render_blocks(
		const pup::gl1::d3::point& pt,
		const ng::block* blocks,
		const ::Uint32 x_l,
		const ::Uint32 y_l,
		const ::Uint32 z_l,
		const ::GLfloat alpha
	);
	
protected:
	int drop_block(int n, const ::Uint32 x, const ::Uint32 y, const ::Uint32 z, int dY = 1);

	void drop_walk(ng::block_addr_map& floaters, ng::block* base,
		const int x, const int y, const int z, const int origo_x, const int origo_z);
	void mainland_walk(ng::block_addr_set& mainland,
		const int x, const int y, const int z);
	
	virtual void render_shadow(ng::shape_ptr s_ptr);

	virtual ::Uint64 sweep_rows_reward(const ::Uint32 y);

	virtual void assign_block_coords(bool actual);
	
	virtual void grow_bottom_mesh();

	gameover_function gameover_;
	sndfx_function sweepsnd_;
	sndfx_function lvlupsnd_;
	
	pup::timer& timer_;
	pup::interval msg_interval_;
	pup::interval step_interval_;
	pup::interval spawn_interval_;
	pup::interval power_interval_;

	bool gm_bottom_mesh_;
	bool gm_zero_g_;
	bool gm_color_match_;
	bool gm_one_shape_limit_;

	::Uint32 xz_;
	::Uint32 y_;
	::Uint32 volume_;

	::Uint32 last_level_up_;
	::Uint32 lines_cleared_;
	::Uint32 shapes_spawned_;
	::Uint32 powers_activated_;
	
	::Uint32 skill_level_;
	::Uint64 skill_points_;
	::Uint64 skill_ui_points_;
	::Uint64 skill_steps_;
	::Uint32 skill_factor_;
	
	::Uint32 flash_msg_frame_;
	::Uint32 flash_msg_frames_;

	string_list flash_msgs_;
	string_list skill_msgs_;
	skill_list waiting_skill_msgs_;
	
	// World boundaries.
	pup::gl1::d3::cuboid bottom_cube_;
	pup::gl1::d3::line x_line_;
	pup::gl1::d3::line y_line_;
	pup::gl1::d3::line z_line_;

	pup::gl1::d3::pyramid x_arrow_;
	pup::gl1::d3::pyramid y_arrow_;
	pup::gl1::d3::pyramid z_arrow_;

	// Available space in the blockfall.
	ng::block* world_blocks_;
	
	ng::factory factory_;
	ng::power_list powers_;
	ng::shape_list shapes_;
	ng::shape_list::iterator active_;
	
	ng::shape_rotation rtmode_;
	
	ng::power_ptr kill_shape_power_;
	ng::power_ptr clear_color_power_;
	ng::power_ptr vaporize_foundation_power_;
	ng::power_ptr spike_trap_power_;
	ng::power_ptr gravity_beam_power_;
};

class game_controller :
	public pup::controller
{
public:
	enum camera_quadrant {
		CQ_0,
		CQ_1,
		CQ_2,
		CQ_3
	};

	explicit game_controller(pup::application& app);
	virtual ~game_controller() throw();
	
	void setup_blockfall(const boost::filesystem::path& world);
	blockfall* get_blockfall() throw() { return blockfall_.get(); }
	
	std::string get_default_world();
	boost::filesystem::path get_world_dir();
	
	virtual void init();

	virtual void think();
	virtual void render();
	
	virtual void gameover();
	virtual void sweepsnd();
	virtual void lvlupsnd();

	camera_quadrant get_camera_quadrant() const;
	
	virtual const char* get_name() const throw() { return "game"; }

	bool key_camera_cw(::SDL_Event& event);
	bool key_camera_ccw(::SDL_Event& event);
	bool key_camera_up(::SDL_Event& event);
	bool key_camera_down(::SDL_Event& event);
	bool key_camera_tiltup(::SDL_Event& event);
	bool key_camera_tiltdown(::SDL_Event& event);
	bool key_camera_zoomin(::SDL_Event& event);
	bool key_camera_zoomout(::SDL_Event& event);

	bool key_block_fall(::SDL_Event& event);
	bool key_block_y_plus(::SDL_Event& event);
	bool key_block_y_minus(::SDL_Event& event);
	bool key_block_z_plus(::SDL_Event& event);
	bool key_block_z_minus(::SDL_Event& event);
	bool key_block_x_plus(::SDL_Event& event);
	bool key_block_x_minus(::SDL_Event& event);
	bool key_block_rotate_cw(::SDL_Event& event);
	bool key_block_rotate_ccw(::SDL_Event& event);
	bool key_block_rotate_mode(::SDL_Event& event);
	bool key_block_next(::SDL_Event& event);
	bool key_block_prev(::SDL_Event& event);
	
	bool key_power_kill_shape(::SDL_Event& event);
	bool key_power_clear_color(::SDL_Event& event);
	bool key_power_vaporize_foundation(::SDL_Event& event);
	bool key_power_spike_trap(::SDL_Event& event);
	bool key_power_gravity_beam(::SDL_Event& event);
	
	bool key_menu(::SDL_Event& event);

private:
	typedef std::auto_ptr<blockfall> blockfall_ptr;
	
	virtual void render_hud();

	blockfall_ptr blockfall_;
	::GLdouble camera_alpha_;
	::GLdouble camera_beta_;
	::GLdouble camera_distance_;
};

namespace ui {

class ng_theme :
	public pup::gl1::ui::theme
{
public:
	explicit ng_theme(pup::gl1::ft::typewriter& typewriter);
};

class ng_form :
	public pup::gl1::ui::static_focus_form
{
public:
	explicit ng_form(pup::controller& ctrlr, pup::gl1::ui::theme& thm);

	virtual pup::gl1::ui::collection& get_menu() { return menu_; }

	virtual pup::gl1::ui::text& get_title() { return title_; }
	virtual pup::gl1::ui::text& get_subtitle() { return subtitle_; }

protected:
	pup::gl1::ui::expander l_space_;
	pup::gl1::ui::expander r_space_;
	pup::gl1::ui::expander menu_top_space_;
	pup::gl1::ui::text title_;
	pup::gl1::ui::text subtitle_;
	pup::gl1::ui::collection menu_;
};


class ng_w3col_form :
	public ng_form
{
public:
	explicit ng_w3col_form(pup::controller& ctrlr, pup::gl1::ui::theme& thm);
	
	pup::gl1::ui::collection& get_l_box() { return l_box_; }
	pup::gl1::ui::collection& get_m_box() { return m_box_; }
	pup::gl1::ui::collection& get_r_box() { return r_box_; }
	
protected:
	pup::gl1::ui::collection l_box_;
	pup::gl1::ui::expander box_l_space_;
	pup::gl1::ui::collection m_box_;
	pup::gl1::ui::expander box_r_space_;
	pup::gl1::ui::collection r_box_;
};

} // ui

class mainmenu_controller :
	public pup::form_controller<ui::ng_theme, ui::ng_form>
{
public:
	explicit mainmenu_controller(pup::application& app);

	virtual const char* get_name() const throw() { return "mainmenu"; }

	void game_btn_click(::SDL_Event& event);
	void options_btn_click(::SDL_Event& event);
	void quit_btn_click(::SDL_Event& event);

protected:
	pup::gl1::ui::button game_btn_;
	pup::gl1::ui::button options_btn_;
	pup::gl1::ui::button quit_btn_;
};

class gameon_controller :
	public pup::form_controller<ui::ng_theme, ui::ng_form>
{
public:
	explicit gameon_controller(pup::application& app);

	virtual void think();

	virtual const char* get_name() const throw() { return "gameon"; }

	void world_btn_click(::SDL_Event& event);
	void start_btn_click(::SDL_Event& event);
	void back_btn_click(::SDL_Event& event);

protected:
	void update_info_label();
	
	pup::controller_ptr game_controller_;
	pup::io::path_list world_paths_;
	pup::io::path_list::iterator world_;
	pup::gl1::ui::text info_;
	pup::gl1::ui::checkbox gm_bottom_mesh_ckbox_;
	pup::gl1::ui::checkbox gm_zero_g_ckbox_;
	pup::gl1::ui::checkbox gm_color_match_ckbox_;
	pup::gl1::ui::checkbox gm_one_shape_limit_ckbox_;
	pup::gl1::ui::button world_btn_;
	pup::gl1::ui::button start_btn_;
	pup::gl1::ui::button back_btn_;
};

class gameover_controller :
	public pup::form_controller<ui::ng_theme, ui::ng_form>
{
public:
	explicit gameover_controller(pup::application& app, highscore hs);

	virtual const char* get_name() const throw() { return "gameover"; }

	void next_btn_click(::SDL_Event& event);

protected:
	pup::gl1::ui::text msg_;
	pup::gl1::ui::input name_input_;
	pup::gl1::ui::button next_btn_;
	highscore highscore_;
};

class highscore_controller :
	public pup::form_controller<ui::ng_theme, ui::ng_form>
{
public:
	explicit highscore_controller(pup::application& app, highscore hs);

	virtual const char* get_name() const throw() { return "highscore"; }

	void back_btn_click(::SDL_Event& event);

protected:
	pup::gl1::ui::text entries_;
	pup::gl1::ui::button back_btn_;
	highscore highscore_;
};


class pause_controller :
	public pup::form_controller<ui::ng_theme, ui::ng_form>
{
public:
	explicit pause_controller(pup::application& app, const pup::controller_ptr& save);

	virtual const char* get_name() const throw() { return "gameover"; }

	void continue_btn_click(::SDL_Event& event);
	void back_btn_click(::SDL_Event& event);

protected:
	pup::controller_ptr saved_;
	pup::gl1::ui::button continue_btn_;
	pup::gl1::ui::button back_btn_;
};


namespace options {

class movement_input_controller :
	public pup::form_controller<ui::ng_theme, ui::ng_w3col_form>
{
public:
	explicit movement_input_controller(pup::application& app);

	virtual const char* get_name() const throw() { return "options::movement_input"; }

	void save_btn_click(::SDL_Event& event);
	void back_btn_click(::SDL_Event& event);

protected:
	pup::gl1::ui::keypress xplus_keypress_;
	pup::gl1::ui::keypress xminus_keypress_;
	pup::gl1::ui::keypress zplus_keypress_;
	pup::gl1::ui::keypress zminus_keypress_;
	
	pup::gl1::ui::keypress next_keypress_;
	pup::gl1::ui::keypress prev_keypress_;
	
	pup::gl1::ui::keypress fall_keypress_;
	pup::gl1::ui::keypress yplus_keypress_;
	pup::gl1::ui::keypress yminus_keypress_;
	
	pup::gl1::ui::button save_btn_;
	pup::gl1::ui::button back_btn_;
};

class rotation_input_controller :
	public pup::form_controller<ui::ng_theme, ui::ng_w3col_form>
{
public:
	explicit rotation_input_controller(pup::application& app);

	virtual const char* get_name() const throw() { return "options::rotation_input"; }

	void save_btn_click(::SDL_Event& event);
	void back_btn_click(::SDL_Event& event);

protected:
	pup::gl1::ui::keypress rotate_cw_keypress_;
	pup::gl1::ui::keypress rotate_ccw_keypress_;
	pup::gl1::ui::keypress rotate_mode_keypress_;

	pup::gl1::ui::button save_btn_;
	pup::gl1::ui::button back_btn_;
};


class camera_input_controller :
	public pup::form_controller<ui::ng_theme, ui::ng_w3col_form>
{
public:
	explicit camera_input_controller(pup::application& app);

	virtual const char* get_name() const throw() { return "options::camera_input"; }

	void save_btn_click(::SDL_Event& event);
	void back_btn_click(::SDL_Event& event);

protected:
	pup::gl1::ui::keypress cw_keypress_;
	pup::gl1::ui::keypress ccw_keypress_;
	pup::gl1::ui::keypress up_keypress_;
	pup::gl1::ui::keypress down_keypress_;
	pup::gl1::ui::keypress tiltup_keypress_;
	pup::gl1::ui::keypress tiltdown_keypress_;
	pup::gl1::ui::keypress zoomin_keypress_;
	pup::gl1::ui::keypress zoomout_keypress_;
	
	pup::gl1::ui::button save_btn_;
	pup::gl1::ui::button back_btn_;
};

class power_input_controller :
	public pup::form_controller<ui::ng_theme, ui::ng_w3col_form>
{
public:
	explicit power_input_controller(pup::application& app);

	virtual const char* get_name() const throw() { return "options::power_input"; }
	
	void save_btn_click(::SDL_Event& event);
	void back_btn_click(::SDL_Event& event);
	
protected:
	pup::gl1::ui::keypress kill_shape_keypress_;
	pup::gl1::ui::keypress clear_color_keypress_;
	pup::gl1::ui::keypress vaporize_foundation_keypress_;
	pup::gl1::ui::keypress spike_trap_keypress_;
	pup::gl1::ui::keypress gravity_beam_keypress_;
	
	pup::gl1::ui::button save_btn_;
	pup::gl1::ui::button back_btn_;
};

class special_input_controller :
	public pup::form_controller<ui::ng_theme, ui::ng_w3col_form>
{
public:
	explicit special_input_controller(pup::application& app);

	virtual const char* get_name() const throw() { return "options::special_input"; }
	
	void save_btn_click(::SDL_Event& event);
	void back_btn_click(::SDL_Event& event);
	
protected:
	pup::gl1::ui::keypress menu_keypress_;
	
	pup::gl1::ui::button save_btn_;
	pup::gl1::ui::button back_btn_;
};

class input_controller :
	public pup::form_controller<ui::ng_theme, ui::ng_form>
{
public:
	explicit input_controller(pup::application& app);

	virtual const char* get_name() const throw() { return "options::input"; }

	void movement_btn_click(::SDL_Event& event);
	void rotation_btn_click(::SDL_Event& event);
	void camera_btn_click(::SDL_Event& event);
	void power_btn_click(::SDL_Event& event);
	void special_btn_click(::SDL_Event& event);
	void back_btn_click(::SDL_Event& event);

protected:
	pup::gl1::ui::button movement_btn_;
	pup::gl1::ui::button rotation_btn_;
	pup::gl1::ui::button camera_btn_;
	pup::gl1::ui::button power_btn_;
	pup::gl1::ui::button special_btn_;
	pup::gl1::ui::button back_btn_;
};

class graphics_controller :
	public pup::form_controller<ui::ng_theme, ui::ng_form>
{
public:
	explicit graphics_controller(pup::application& app);

	virtual const char* get_name() const throw() { return "options::graphics"; }

	void save_btn_click(::SDL_Event& event);
	void back_btn_click(::SDL_Event& event);

protected:
	pup::gl1::ui::input screen_resolution_input_;
	pup::gl1::ui::input fov_input_;
	pup::gl1::ui::checkbox vsync_ckbox_;
	pup::gl1::ui::checkbox fullscreen_ckbox_;
	pup::gl1::ui::checkbox rgcolorblind_ckbox_;
	pup::gl1::ui::button save_btn_;
	pup::gl1::ui::button back_btn_;
};

class sound_controller :
	public pup::form_controller<ui::ng_theme, ui::ng_form>
{
public:
	explicit sound_controller(pup::application& app);

	virtual const char* get_name() const throw() { return "options::sound"; }

	void save_btn_click(::SDL_Event& event);
	void back_btn_click(::SDL_Event& event);

protected:
	pup::gl1::ui::input music_input_;
	pup::gl1::ui::input effect_input_;
	pup::gl1::ui::button save_btn_;
	pup::gl1::ui::button back_btn_;

};

} // options

class options_controller :
	public pup::form_controller<ui::ng_theme, ui::ng_form>
{
public:
	explicit options_controller(pup::application& app);

	virtual const char* get_name() const throw() { return "options"; }

	void input_btn_click(::SDL_Event& event);
	void graphics_btn_click(::SDL_Event& event);
	void sound_btn_click(::SDL_Event& event);
	void back_btn_click(::SDL_Event& event);

protected:
	pup::gl1::ui::button input_btn_;
	pup::gl1::ui::button graphics_btn_;
	pup::gl1::ui::button sound_btn_;
	pup::gl1::ui::button back_btn_;
};

class application :
	public pup::application
{
public:
	explicit application(::Uint32 sdl_flags, const boost::program_options::variables_map& opt_vm);
	virtual ~application() throw();

	virtual void before_loop();
	virtual void after_loop();

	virtual pup::controller_ptr get_start_controller();
	
	SQLite::Database& get_db() { return db_; }

protected:
	virtual void update_db();

	SQLite::Database db_;
};

namespace debug {

void print_world(const fabl::blockfall& blkfall);
void print_world(const fabl::blockfall& blkfall, std::ostream& out);

} // debug

} // fabl

#endif

