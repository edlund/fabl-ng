
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

#include "fabl.h"

namespace fabl {

namespace options {

movement_input_controller::movement_input_controller(pup::application& app) :
	pup::form_controller<ui::ng_theme, ui::ng_w3col_form>(app),
	xplus_keypress_(form_, "xplus_keypress"),
	xminus_keypress_(form_, "xminus_keypress"),
	zplus_keypress_(form_, "zplus_keypress"),
	zminus_keypress_(form_, "zminus_keypress"),
	next_keypress_(form_, "next_keypress"),
	prev_keypress_(form_, "prev_keypress"),
	fall_keypress_(form_, "fall_keypress"),
	yplus_keypress_(form_, "yplus_keypress"),
	yminus_keypress_(form_, "yminus_keypress"),
	save_btn_(
		form_,
		"save_btn",
		boost::bind(&movement_input_controller::save_btn_click, this, _1)
	),
	back_btn_(
		form_,
		"back_btn",
		boost::bind(&movement_input_controller::back_btn_click, this, _1)
	)
{
	boost::property_tree::ptree& pt = app.get_ptree();
	
	form_.get_l_box().push_back(xplus_keypress_);
	xplus_keypress_.set_label("X Plus");
	xplus_keypress_.set_value(pt.get<std::string>("game_control.block_x_plus"));
	
	form_.get_m_box().push_back(xminus_keypress_);
	xminus_keypress_.set_label("X Minus");
	xminus_keypress_.set_value(pt.get<std::string>("game_control.block_x_minus"));
	
	form_.get_r_box().push_back(zplus_keypress_);
	zplus_keypress_.set_label("Z Plus");
	zplus_keypress_.set_value(pt.get<std::string>("game_control.block_z_plus"));
	
	form_.get_l_box().push_back(zminus_keypress_);
	zminus_keypress_.set_label("Z Minus");
	zminus_keypress_.set_value(pt.get<std::string>("game_control.block_z_minus"));
	
	form_.get_m_box().push_back(next_keypress_);
	next_keypress_.set_label("Next Block");
	next_keypress_.set_value(pt.get<std::string>("game_control.block_next"));

	form_.get_r_box().push_back(prev_keypress_);
	prev_keypress_.set_label("Prev Block");
	prev_keypress_.set_value(pt.get<std::string>("game_control.block_prev"));
	
	form_.get_l_box().push_back(fall_keypress_);
	fall_keypress_.set_label("Drop block");
	fall_keypress_.set_value(pt.get<std::string>("game_control.block_fall"));
	
	form_.get_m_box().push_back(yplus_keypress_);
	yplus_keypress_.set_label("Y Plus");
	yplus_keypress_.set_value(pt.get<std::string>("game_control.block_y_plus"));

	form_.get_r_box().push_back(yminus_keypress_);
	yminus_keypress_.set_label("Y Minus");
	yminus_keypress_.set_value(pt.get<std::string>("game_control.block_y_minus"));
	
	form_.get_m_box().push_back(save_btn_);
	save_btn_.set_label("Save");
	
	form_.get_m_box().push_back(back_btn_);
	back_btn_.set_label("Cancel");
	
	form_.finalize();
}

void movement_input_controller::save_btn_click(::SDL_Event& event)
{
	if (form_.validate()) {
		boost::property_tree::ptree& pt = this->get_app().get_ptree();
		
		boost::filesystem::path config(this->get_app().get_config_path());
		
		pt.put("game_control.block_x_plus", xplus_keypress_.string_value());
		pt.put("game_control.block_x_minus", xminus_keypress_.string_value());
		pt.put("game_control.block_z_plus", zplus_keypress_.string_value());
		pt.put("game_control.block_z_minus", zminus_keypress_.string_value());
		
		pt.put("game_control.block_next", next_keypress_.string_value());
		pt.put("game_control.block_prev", prev_keypress_.string_value());
		
		pt.put("game_control.block_fall", fall_keypress_.string_value());
		pt.put("game_control.block_y_plus", yplus_keypress_.string_value());
		pt.put("game_control.block_y_minus", yminus_keypress_.string_value());
		
		boost::property_tree::ini_parser::write_ini(config.string(), pt);
		
		this->get_app().configure();
		this->back_btn_click(event);
	}
}

void movement_input_controller::back_btn_click(::SDL_Event& event)
{
	app_.queue_controller(
		pup::controller_ptr(new input_controller(app_))
	);
}

rotation_input_controller::rotation_input_controller(pup::application& app) :
	pup::form_controller<ui::ng_theme, ui::ng_w3col_form>(app),
	rotate_cw_keypress_(form_, "rotate_cw_keypress"),
	rotate_ccw_keypress_(form_, "rotate_ccw_keypress"),
	rotate_mode_keypress_(form_, "rotate_mode_keypress"),
	save_btn_(
		form_,
		"save_btn",
		boost::bind(&rotation_input_controller::save_btn_click, this, _1)
	),
	back_btn_(
		form_,
		"back_btn",
		boost::bind(&rotation_input_controller::back_btn_click, this, _1)
	)
{
	boost::property_tree::ptree& pt = app.get_ptree();
	
	form_.get_l_box().push_back(rotate_cw_keypress_);
	rotate_cw_keypress_.set_label("Rotate CW");
	rotate_cw_keypress_.set_value(pt.get<std::string>("game_control.block_rotate_cw"));
	
	form_.get_m_box().push_back(rotate_ccw_keypress_);
	rotate_ccw_keypress_.set_label("Rotate CCW");
	rotate_ccw_keypress_.set_value(pt.get<std::string>("game_control.block_rotate_ccw"));
	
	form_.get_r_box().push_back(rotate_mode_keypress_);
	rotate_mode_keypress_.set_label("Rotate mode");
	rotate_mode_keypress_.set_value(pt.get<std::string>("game_control.block_rotate_mode"));
	
	form_.get_m_box().push_back(save_btn_);
	save_btn_.set_label("Save");
	
	form_.get_m_box().push_back(back_btn_);
	back_btn_.set_label("Cancel");
	
	form_.finalize();
}

void rotation_input_controller::save_btn_click(::SDL_Event& event)
{
	if (form_.validate()) {
		boost::property_tree::ptree& pt = this->get_app().get_ptree();
		
		boost::filesystem::path config(this->get_app().get_config_path());
		
		pt.put("game_control.block_rotate_cw", rotate_cw_keypress_.string_value());
		pt.put("game_control.block_rotate_ccw", rotate_ccw_keypress_.string_value());
		pt.put("game_control.block_rotate_mode", rotate_mode_keypress_.string_value());
		
		boost::property_tree::ini_parser::write_ini(config.string(), pt);
		
		this->get_app().configure();
		this->back_btn_click(event);
	}
}

void rotation_input_controller::back_btn_click(::SDL_Event& event)
{
	app_.queue_controller(
		pup::controller_ptr(new input_controller(app_))
	);
}

camera_input_controller::camera_input_controller(pup::application& app) :
	pup::form_controller<ui::ng_theme, ui::ng_w3col_form>(app),
	cw_keypress_(form_, "cw_keypress"),
	ccw_keypress_(form_, "ccw_keypress"),
	up_keypress_(form_, "up_keypress"),
	down_keypress_(form_, "down_keypress"),
	tiltup_keypress_(form_, "tiltup_keypress"),
	tiltdown_keypress_(form_, "tiltdown_keypress"),
	zoomin_keypress_(form_, "zoomin_keypress"),
	zoomout_keypress_(form_, "zoomout_keypress"),
	save_btn_(
		form_,
		"save_btn",
		boost::bind(&camera_input_controller::save_btn_click, this, _1)
	),
	back_btn_(
		form_,
		"back_btn",
		boost::bind(&camera_input_controller::back_btn_click, this, _1)
	)
{
	boost::property_tree::ptree& pt = app.get_ptree();
	
	form_.get_l_box().push_back(cw_keypress_);
	cw_keypress_.set_label("Rotate CW");
	cw_keypress_.set_value(pt.get<std::string>("game_control.camera_cw"));
	
	form_.get_m_box().push_back(ccw_keypress_);
	ccw_keypress_.set_label("Rotate CCW");
	ccw_keypress_.set_value(pt.get<std::string>("game_control.camera_ccw"));
	
	form_.get_r_box().push_back(up_keypress_);
	up_keypress_.set_label("Up");
	up_keypress_.set_value(pt.get<std::string>("game_control.camera_up"));
	
	form_.get_l_box().push_back(down_keypress_);
	down_keypress_.set_label("Down");
	down_keypress_.set_value(pt.get<std::string>("game_control.camera_down"));
	
	form_.get_m_box().push_back(tiltup_keypress_);
	tiltup_keypress_.set_label("Tilt up");
	tiltup_keypress_.set_value(pt.get<std::string>("game_control.camera_tiltup"));
	
	form_.get_r_box().push_back(tiltdown_keypress_);
	tiltdown_keypress_.set_label("Tilt down");
	tiltdown_keypress_.set_value(pt.get<std::string>("game_control.camera_tiltdown"));
	
	form_.get_l_box().push_back(zoomin_keypress_);
	zoomin_keypress_.set_label("Zoom in");
	zoomin_keypress_.set_value(pt.get<std::string>("game_control.camera_zoomin"));
	
	form_.get_m_box().push_back(zoomout_keypress_);
	zoomout_keypress_.set_label("Zoom out");
	zoomout_keypress_.set_value(pt.get<std::string>("game_control.camera_zoomout"));
	
	form_.get_m_box().push_back(save_btn_);
	save_btn_.set_label("Save");
	
	form_.get_m_box().push_back(back_btn_);
	back_btn_.set_label("Cancel");
	
	form_.finalize();
}

void camera_input_controller::save_btn_click(::SDL_Event& event)
{
	if (form_.validate()) {
		boost::property_tree::ptree& pt = this->get_app().get_ptree();
		
		boost::filesystem::path config(this->get_app().get_config_path());
		
		pt.put("game_control.camera_cw", cw_keypress_.string_value());
		pt.put("game_control.camera_ccw", ccw_keypress_.string_value());
		pt.put("game_control.camera_up", up_keypress_.string_value());
		pt.put("game_control.camera_down", down_keypress_.string_value());
		pt.put("game_control.camera_tiltup", tiltup_keypress_.string_value());
		pt.put("game_control.camera_tiltdown", tiltdown_keypress_.string_value());
		pt.put("game_control.camera_zoomin", zoomin_keypress_.string_value());
		pt.put("game_control.camera_zoomout", zoomout_keypress_.string_value());
		
		boost::property_tree::ini_parser::write_ini(config.string(), pt);
		
		this->get_app().configure();
		this->back_btn_click(event);
	}
}

void camera_input_controller::back_btn_click(::SDL_Event& event)
{
	app_.queue_controller(
		pup::controller_ptr(new input_controller(app_))
	);
}

power_input_controller::power_input_controller(pup::application& app) :
	pup::form_controller<ui::ng_theme, ui::ng_w3col_form>(app),
	kill_shape_keypress_(form_, "kill_shape_keypress"),
	clear_color_keypress_(form_, "clear_color_keypress"),
	vaporize_foundation_keypress_(form_, "vaporize_foundation_keypress"),
	spike_trap_keypress_(form_, "spike_trap_keypress"),
	gravity_beam_keypress_(form_, "gravity_beam_keypress"),
	save_btn_(
		form_,
		"save_btn",
		boost::bind(&power_input_controller::save_btn_click, this, _1)
	),
	back_btn_(
		form_,
		"back_btn",
		boost::bind(&power_input_controller::back_btn_click, this, _1)
	)
{
	boost::property_tree::ptree& pt = app.get_ptree();
	
	form_.get_l_box().push_back(kill_shape_keypress_);
	kill_shape_keypress_.set_label("KillShape");
	kill_shape_keypress_.set_value(pt.get<std::string>("game_control.power_kill_shape"));
	
	form_.get_m_box().push_back(clear_color_keypress_);
	clear_color_keypress_.set_label("ClearColor");
	clear_color_keypress_.set_value(pt.get<std::string>("game_control.power_clear_color"));
	
	form_.get_r_box().push_back(vaporize_foundation_keypress_);
	vaporize_foundation_keypress_.set_label("VaporizeFndtn");
	vaporize_foundation_keypress_.set_value(pt.get<std::string>("game_control.power_vaporize_foundation"));
	
	form_.get_l_box().push_back(spike_trap_keypress_);
	spike_trap_keypress_.set_label("SpikeTrap");
	spike_trap_keypress_.set_value(pt.get<std::string>("game_control.power_spike_trap"));
	
	form_.get_m_box().push_back(gravity_beam_keypress_);
	gravity_beam_keypress_.set_label("GravityBeam");
	gravity_beam_keypress_.set_value(pt.get<std::string>("game_control.power_gravity_beam"));
	
	form_.get_m_box().push_back(save_btn_);
	save_btn_.set_label("Save");
	
	form_.get_m_box().push_back(back_btn_);
	back_btn_.set_label("Cancel");
	
	form_.finalize();
}

void power_input_controller::save_btn_click(::SDL_Event& event)
{
	if (form_.validate()) {
		boost::property_tree::ptree& pt = this->get_app().get_ptree();
		
		boost::filesystem::path config(this->get_app().get_config_path());
		
		pt.put("game_control.power_kill_shape", kill_shape_keypress_.string_value());
		pt.put("game_control.power_clear_color", clear_color_keypress_.string_value());
		pt.put("game_control.power_vaporize_foundation", vaporize_foundation_keypress_.string_value());
		pt.put("game_control.power_spike_trap", spike_trap_keypress_.string_value());
		pt.put("game_control.power_gravity_beam", gravity_beam_keypress_.string_value());
		
		boost::property_tree::ini_parser::write_ini(config.string(), pt);
		
		this->get_app().configure();
		this->back_btn_click(event);
	}
}

void power_input_controller::back_btn_click(::SDL_Event& event)
{
	app_.queue_controller(
		pup::controller_ptr(new input_controller(app_))
	);
}

special_input_controller::special_input_controller(pup::application& app) :
	pup::form_controller<ui::ng_theme, ui::ng_w3col_form>(app),
	menu_keypress_(form_, "menu_keypress"),
	save_btn_(
		form_,
		"save_btn",
		boost::bind(&special_input_controller::save_btn_click, this, _1)
	),
	back_btn_(
		form_,
		"back_btn",
		boost::bind(&special_input_controller::back_btn_click, this, _1)
	)
{
	boost::property_tree::ptree& pt = app.get_ptree();
	
	form_.get_m_box().push_back(menu_keypress_);
	menu_keypress_.set_label("Menu/Pause");
	menu_keypress_.set_value(pt.get<std::string>("game_control.menu"));
	
	form_.get_m_box().push_back(save_btn_);
	save_btn_.set_label("Save");
	
	form_.get_m_box().push_back(back_btn_);
	back_btn_.set_label("Cancel");
	
	form_.finalize();
}

void special_input_controller::save_btn_click(::SDL_Event& event)
{
	if (form_.validate()) {
		boost::property_tree::ptree& pt = this->get_app().get_ptree();
		
		boost::filesystem::path config(this->get_app().get_config_path());
		
		pt.put("game_control.menu", menu_keypress_.string_value());
		
		boost::property_tree::ini_parser::write_ini(config.string(), pt);
		
		this->get_app().configure();
		this->back_btn_click(event);
	}
}

void special_input_controller::back_btn_click(::SDL_Event& event)
{
	app_.queue_controller(
		pup::controller_ptr(new input_controller(app_))
	);
}

input_controller::input_controller(pup::application& app) :
	pup::form_controller<ui::ng_theme, ui::ng_form>(app),
	movement_btn_(
		form_,
		"movement_btn",
		boost::bind(&input_controller::movement_btn_click, this, _1)
	),
	rotation_btn_(
		form_,
		"rotation_btn",
		boost::bind(&input_controller::rotation_btn_click, this, _1)
	),
	camera_btn_(
		form_,
		"camera_btn",
		boost::bind(&input_controller::camera_btn_click, this, _1)
	),
	power_btn_(
		form_,
		"power_btn",
		boost::bind(&input_controller::power_btn_click, this, _1)
	),
	special_btn_(
		form_,
		"special_btn",
		boost::bind(&input_controller::special_btn_click, this, _1)
	),
	back_btn_(
		form_,
		"back_btn",
		boost::bind(&input_controller::back_btn_click, this, _1)
	)
{
	form_.get_menu().push_back(movement_btn_);
	movement_btn_.set_label("Movement");
	
	form_.get_menu().push_back(rotation_btn_);
	rotation_btn_.set_label("Rotation");

	form_.get_menu().push_back(camera_btn_);
	camera_btn_.set_label("Camera");
	
	form_.get_menu().push_back(power_btn_);
	power_btn_.set_label("Power");
	
	form_.get_menu().push_back(special_btn_);
	special_btn_.set_label("Special");
	
	form_.get_menu().push_back(back_btn_);
	back_btn_.set_label("Back");
	
	form_.finalize();
}

void input_controller::movement_btn_click(::SDL_Event& event)
{
	app_.queue_controller(
		pup::controller_ptr(new movement_input_controller(app_))
	);
}

void input_controller::rotation_btn_click(::SDL_Event& event)
{
	app_.queue_controller(
		pup::controller_ptr(new rotation_input_controller(app_))
	);
}

void input_controller::camera_btn_click(::SDL_Event& event)
{
	app_.queue_controller(
		pup::controller_ptr(new camera_input_controller(app_))
	);
}

void input_controller::power_btn_click(::SDL_Event& event)
{
	app_.queue_controller(
		pup::controller_ptr(new power_input_controller(app_))
	);
}

void input_controller::special_btn_click(::SDL_Event& event)
{
	app_.queue_controller(
		pup::controller_ptr(new special_input_controller(app_))
	);
}

void input_controller::back_btn_click(::SDL_Event& event)
{
	app_.queue_controller(
		pup::controller_ptr(new options_controller(app_))
	);
}

graphics_controller::graphics_controller(pup::application& app) :
	pup::form_controller<ui::ng_theme, ui::ng_form>(app),
	screen_resolution_input_(form_, "screen_resolution_input"),
	fov_input_(form_, "fov_input"),
	vsync_ckbox_(form_, "vsync_ckbox"),
	fullscreen_ckbox_(form_, "fullscreen_ckbox"),
	rgcolorblind_ckbox_(form_, "rgcolorblind_ckbox"),
	save_btn_(
		form_,
		"save_btn",
		boost::bind(&graphics_controller::save_btn_click, this, _1)
	),
	back_btn_(
		form_,
		"back_btn",
		boost::bind(&graphics_controller::back_btn_click, this, _1)
	)
{
	boost::property_tree::ptree& pt = app.get_ptree();

	std::string fov(pt.get<std::string>("graphics.fov"));
	std::string screen_resolution(boost::str(
		boost::format("%1%x%2%")
			% pt.get<int>("graphics.window_width")
			% pt.get<int>("graphics.window_height")
	));

	form_.get_menu().push_back(screen_resolution_input_);
	screen_resolution_input_.set_label("Resolution (WxH pixels)");
	screen_resolution_input_.set_value(screen_resolution);
	screen_resolution_input_.add_validator(
		"WxH", pup::gl1::ui::regex_validator("^\\d+x\\d+$")
	);

	form_.get_menu().push_back(fov_input_);
	fov_input_.set_label("Field Of View (X.Y degrees)");
	fov_input_.set_value(fov);
	fov_input_.add_validator(
		"FOV", pup::gl1::ui::regex_validator("^\\d+\\.\\d+$")
	);
	
	form_.get_menu().push_back(vsync_ckbox_);
	vsync_ckbox_.set_label("VSync");
	vsync_ckbox_.set_checked(pt.get<bool>("graphics.vsync"));
	
	form_.get_menu().push_back(fullscreen_ckbox_);
	fullscreen_ckbox_.set_label("Fullscreen");
	fullscreen_ckbox_.set_checked(pt.get<bool>("graphics.window_fullscreen"));
	
	form_.get_menu().push_back(rgcolorblind_ckbox_);
	rgcolorblind_ckbox_.set_label("R/G colorblind matching");
	rgcolorblind_ckbox_.set_checked(pt.get<bool>("general.rgcolorblind_match"));

	form_.get_menu().push_back(save_btn_);
	save_btn_.set_label("Save");

	form_.get_menu().push_back(back_btn_);
	back_btn_.set_label("Cancel");

	form_.finalize();
}

void graphics_controller::save_btn_click(::SDL_Event& event)
{
	if (form_.validate()) {
		boost::property_tree::ptree& pt = this->get_app().get_ptree();
		
		boost::filesystem::path config(this->get_app().get_config_path());
		std::string screen_resolution(screen_resolution_input_.string_value());
		pup::string_vector screen_resolution_parts;
		boost::split(
			screen_resolution_parts,
			screen_resolution,
			boost::is_any_of("x")
		);
		
		pt.put("graphics.window_width", screen_resolution_parts[pup::WD]);
		pt.put("graphics.window_height", screen_resolution_parts[pup::HT]);
		pt.put("graphics.fov", fov_input_.string_value());
		pt.put("graphics.vsync", vsync_ckbox_.string_value());
		pt.put("graphics.window_fullscreen", fullscreen_ckbox_.string_value());
		pt.put("general.rgcolorblind_match", rgcolorblind_ckbox_.string_value());
		
		boost::property_tree::ini_parser::write_ini(config.string(), pt);
		
		this->get_app().configure();
		this->back_btn_click(event);
	}
}

void graphics_controller::back_btn_click(::SDL_Event& event)
{
	app_.queue_controller(
		pup::controller_ptr(new options_controller(app_))
	);
}

sound_controller::sound_controller(pup::application& app) :
	pup::form_controller<ui::ng_theme, ui::ng_form>(app),
	music_input_(form_, "music_input"),
	effect_input_(form_, "effect_input"),
	save_btn_(
		form_,
		"save_btn",
		boost::bind(&sound_controller::save_btn_click, this, _1)
	),
	back_btn_(
		form_,
		"back_btn",
		boost::bind(&sound_controller::back_btn_click, this, _1)
	)
{
	boost::property_tree::ptree& pt = app.get_ptree();

	std::string music_vol(pt.get<std::string>("sound.music_volume"));
	std::string effect_vol(pt.get<std::string>("sound.effect_volume"));
	
	pup::gl1::ui::regex_validator snd_validator("^\\d+$");

	form_.get_menu().push_back(music_input_);
	music_input_.set_label("Music volume (0 <= v <= 128)");
	music_input_.set_value(music_vol);
	music_input_.add_validator("volume", snd_validator);

	form_.get_menu().push_back(effect_input_);
	effect_input_.set_label("Effect volume (0 <= v <= 128)");
	effect_input_.set_value(effect_vol);
	effect_input_.add_validator("volume", snd_validator);

	form_.get_menu().push_back(save_btn_);
	save_btn_.set_label("Save");

	form_.get_menu().push_back(back_btn_);
	back_btn_.set_label("Cancel");

	form_.finalize();
}

void sound_controller::save_btn_click(::SDL_Event& event)
{
	if (form_.validate()) {
		boost::property_tree::ptree& pt = this->get_app().get_ptree();
		
		boost::filesystem::path config(this->get_app().get_config_path());
		
		pt.put("sound.music_volume", music_input_.string_value());
		pt.put("sound.effect_volume", effect_input_.string_value());
		
		boost::property_tree::ini_parser::write_ini(config.string(), pt);
		
		this->get_app().configure();
		this->back_btn_click(event);
	}
}

void sound_controller::back_btn_click(::SDL_Event& event)
{
	app_.queue_controller(
		pup::controller_ptr(new options_controller(app_))
	);
}

} // options

options_controller::options_controller(pup::application& app) :
	pup::form_controller<ui::ng_theme, ui::ng_form>(app),
	input_btn_(
		form_,
		"input_btn",
		boost::bind(&options_controller::input_btn_click, this, _1)
	),
	graphics_btn_(
		form_,
		"graphics_btn",
		boost::bind(&options_controller::graphics_btn_click, this, _1)
	),
	sound_btn_(
		form_,
		"sound_btn",
		boost::bind(&options_controller::sound_btn_click, this, _1)
	),
	back_btn_(
		form_,
		"back_btn",
		boost::bind(&options_controller::back_btn_click, this, _1)
	)
{
	form_.get_menu().push_back(input_btn_);
	input_btn_.set_label("Input");

	form_.get_menu().push_back(graphics_btn_);
	graphics_btn_.set_label("Graphics");

	form_.get_menu().push_back(sound_btn_);
	sound_btn_.set_label("Sound");

	form_.get_menu().push_back(back_btn_);
	back_btn_.set_label("Main Menu");

	form_.finalize();
}

void options_controller::input_btn_click(::SDL_Event& event)
{
	app_.queue_controller(
		pup::controller_ptr(new options::input_controller(app_))
	);
}

void options_controller::graphics_btn_click(::SDL_Event& event)
{
	app_.queue_controller(
		pup::controller_ptr(new options::graphics_controller(app_))
	);
}

void options_controller::sound_btn_click(::SDL_Event& event)
{
	app_.queue_controller(
		pup::controller_ptr(new options::sound_controller(app_))
	);
}

void options_controller::back_btn_click(::SDL_Event& event)
{
	app_.queue_controller(
		pup::controller_ptr(new mainmenu_controller(app_))
	);
}

} // fabl
