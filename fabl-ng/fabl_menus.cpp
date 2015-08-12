
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

mainmenu_controller::mainmenu_controller(pup::application& app) :
	pup::form_controller<ui::ng_theme, ui::ng_form>(app),
	game_btn_(
		form_,
		"game_btn",
		boost::bind(&mainmenu_controller::game_btn_click, this, _1)
	),
	options_btn_(
		form_,
		"options_btn",
		boost::bind(&mainmenu_controller::options_btn_click, this, _1)
	),
	quit_btn_(
		form_,
		"quit_btn",
		boost::bind(&mainmenu_controller::quit_btn_click, this, _1)
	)
{
	form_.get_menu().push_back(game_btn_);
	game_btn_.set_label("New Game");

	form_.get_menu().push_back(options_btn_);
	options_btn_.set_label("Options");

	form_.get_menu().push_back(quit_btn_);
	quit_btn_.set_label("Quit");

	form_.finalize();

	app_.get_jukebox().start_mix();
}

void mainmenu_controller::game_btn_click(::SDL_Event& event)
{
	app_.queue_controller(pup::controller_ptr(new gameon_controller(app_)));
}

void mainmenu_controller::options_btn_click(::SDL_Event& event)
{
	app_.queue_controller(pup::controller_ptr(new options_controller(app_)));
}

void mainmenu_controller::quit_btn_click(::SDL_Event& event)
{
	app_.stop();
}

gameon_controller::gameon_controller(pup::application& app) :
	pup::form_controller<ui::ng_theme, ui::ng_form>(app),
	game_controller_(new game_controller(app)),
	world_(world_paths_.end()),
	info_(
		form_,
		"info",
		app.get_typewriter().find_ptr("UbuntuMono-R", 16),
		theme_.get_label(),
		pup::gl1::ft::ALIGN_CENTER
	),
	gm_bottom_mesh_ckbox_(form_, "gm_bottom_mesh_ckbox"),
	gm_zero_g_ckbox_(form_, "gm_zero_g_ckbox"),
	gm_color_match_ckbox_(form_, "gm_color_match_ckbox"),
	gm_one_shape_limit_ckbox_(form_, "gm_one_shape_limit_ckbox"),
	world_btn_(
		form_,
		"world_button",
		boost::bind(&gameon_controller::world_btn_click, this, _1)
	),
	start_btn_(
		form_,
		"start_btn",
		boost::bind(&gameon_controller::start_btn_click, this, _1)
	),
	back_btn_(
		form_,
		"back_btn",
		boost::bind(&gameon_controller::back_btn_click, this, _1)
	)
{
	form_.get_menu().push_back(info_);
	info_.set_after(30);
	
	form_.get_menu().push_back(gm_bottom_mesh_ckbox_);
	gm_bottom_mesh_ckbox_.set_label("BottomMesh Clear (Harder)");

	form_.get_menu().push_back(gm_zero_g_ckbox_);
	gm_zero_g_ckbox_.set_label("Zero-G Clear (Harder)");

	form_.get_menu().push_back(gm_color_match_ckbox_);
	gm_color_match_ckbox_.set_label("ColorMatch Clear (Harder)");

	form_.get_menu().push_back(gm_one_shape_limit_ckbox_);
	gm_one_shape_limit_ckbox_.set_label("One Shape Limit (Easier)");
	
	form_.get_menu().push_back(world_btn_);
	world_btn_.set_label("Change World");
	
	form_.get_menu().push_back(start_btn_);
	start_btn_.set_label("Start Game");

	form_.get_menu().push_back(back_btn_);
	back_btn_.set_label("Main Menu");

	form_.finalize();
	
	game_controller* gc = dynamic_cast<game_controller*>(
		game_controller_.get()
	);
	
	std::string default_world(gc->get_default_world());
	boost::filesystem::path world_dir(gc->get_world_dir());
	
	std::copy(
		boost::filesystem::directory_iterator(world_dir),
		boost::filesystem::directory_iterator(),
		std::back_inserter(world_paths_)
	);
	world_paths_.sort();
	
	using boost::filesystem::basename;
	
	for (auto it = world_paths_.begin(); it != world_paths_.end(); ++it) {
		if (basename(*it) == basename(default_world)) {
			world_ = it;
		}
	}
	this->update_info_label();
}

void gameon_controller::think()
{
	pup::form_controller<ui::ng_theme, ui::ng_form>::think();

	game_controller* gc = dynamic_cast<game_controller*>(game_controller_.get());
	blockfall* bf = gc->get_blockfall();
	
	if (bf) {
		bf->set_gm_bottom_mesh(gm_bottom_mesh_ckbox_.get_checked());
		bf->set_gm_zero_g(gm_zero_g_ckbox_.get_checked());
		bf->set_gm_color_match(gm_color_match_ckbox_.get_checked());
		bf->set_gm_one_shape_limit(gm_one_shape_limit_ckbox_.get_checked());
	} else {
		PUP_ERR(std::runtime_error, "game_controller is missing a blockfall"
			" - this should NEVER happen; congratulations");
	}
}

void gameon_controller::world_btn_click(::SDL_Event& event)
{
	if (++world_ == world_paths_.end()) {
		world_ = world_paths_.begin();
	}
	game_controller* gc = dynamic_cast<game_controller*>(game_controller_.get());
	gc->setup_blockfall(*world_);
	
	this->update_info_label();
	app_.get_soundboard().play("click");
}

void gameon_controller::start_btn_click(::SDL_Event& event)
{
	app_.get_music().fadeout(3000);
	app_.queue_controller(game_controller_);
}

void gameon_controller::back_btn_click(::SDL_Event& event)
{
	app_.queue_controller(
		pup::controller_ptr(new mainmenu_controller(app_))
	);
}

void gameon_controller::update_info_label()
{
	game_controller* gc = dynamic_cast<game_controller*>(game_controller_.get());
	blockfall* bf = gc->get_blockfall();
	
	const unsigned int xz = bf? bf->get_xz(): 0;
	const unsigned int y = bf? bf->get_y(): 0;
	
	std::string label(boost::str(boost::format("World '%1%' (%2%;%3%;%2%)")
		% boost::filesystem::basename(*world_)
		% xz
		% y
	));
	
	info_.set_label(label);
}

gameover_controller::gameover_controller(pup::application& app, highscore hs) :
	pup::form_controller<ui::ng_theme, ui::ng_form>(app),
	msg_(
		form_,
		"msg",
		app.get_typewriter().find_ptr("UbuntuMono-R", 32),
		pup::gl1::color::from_hex("#ffffff"),
		pup::gl1::ft::ALIGN_CENTER
	),
	name_input_(form_, "name_input"),
	next_btn_(
		form_,
		"next_btn",
		boost::bind(&gameover_controller::next_btn_click, this, _1)
	),
	highscore_(hs)
{
	form_.get_title().set_label(std::to_string(hs.points));
	form_.get_subtitle().set_label("SkillPoints");

	form_.get_menu().push_back(msg_);
	msg_.set_label("< Game Over >");
	msg_.set_after(40);

	form_.get_menu().push_back(name_input_);
	name_input_.set_label("Name ([-a-z0-9], max 8 chars)");
	name_input_.add_validator(
		"alnum", pup::gl1::ui::regex_validator("^[-a-z0-9]{1,8}$")
	);

	form_.get_menu().push_back(next_btn_);
	next_btn_.set_label("Save");

	form_.finalize();
	form_.set_focused(&name_input_);
}

void gameover_controller::next_btn_click(::SDL_Event& event)
{
	if (form_.validate()) {
		highscore_.name = name_input_.string_value();
		app_.queue_controller(
			pup::controller_ptr(new highscore_controller(app_, highscore_))
		);
	}
}

highscore_controller::highscore_controller(pup::application& app, highscore hs) :
	pup::form_controller<ui::ng_theme, ui::ng_form>(app),
	entries_(
		form_,
		"entries",
		app.get_typewriter().find_ptr("UbuntuMono-R", 12),
		pup::gl1::color::from_hex("#ffffff"),
		pup::gl1::ft::ALIGN_LEFT
	),
	back_btn_(
		form_,
		"next_btn",
		boost::bind(&highscore_controller::back_btn_click, this, _1)
	),
	highscore_(hs)
{
	form_.get_title().set_label("HighScore");
	form_.get_subtitle().set_label("BlockMasters");

	form_.get_menu().push_back(entries_);
	
	form_.get_menu().push_back(back_btn_);
	back_btn_.set_label("Main Manu");

	SQLite::Database& db = dynamic_cast<application&>(app).get_db();

	SQLite::Statement insert(db, ""
		"INSERT INTO `highscore` VALUES(                             "
		"    NULL, ?, ?, date(), ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?  "
		")                                                           "
	);

	insert.bind(1, highscore_.crc);
	insert.bind(2, highscore_.fabl);
	insert.bind(3, highscore_.world);
	insert.bind(4, highscore_.name);
	insert.bind(5, highscore_.points);
	insert.bind(6, highscore_.steps);
	insert.bind(7, highscore_.levels);
	insert.bind(8, highscore_.lines);
	insert.bind(9, highscore_.shapes);
	insert.bind(10, highscore_.powers);
	insert.bind(11, highscore_.gm_bottom_mesh);
	insert.bind(12, highscore_.gm_zero_g);
	insert.bind(13, highscore_.gm_color_match);
	insert.bind(14, highscore_.gm_one_shape_limit);
	insert.exec();

	highscore_.id = db.getLastInsertRowid();

	pup::string_list rows;
	pup::string_list queries {
		"SELECT * FROM `highscore` WHERE( "
		"    `crc` = ? AND `points` > ?   "
		") ORDER BY `points` DESC LIMIT ? ",

		"SELECT * FROM `highscore` WHERE( "
		"    `crc` = ? AND `points` <= ?  "
		") ORDER BY `points` DESC LIMIT ? "
	};

	int first = 4;
	int second = 5;
	int limits[2] = {
		first,
		second
	};

	for (auto query = queries.begin(); query != queries.end(); ++query) {
		SQLite::Statement select(db, *query);

		std::size_t n = std::distance(queries.begin(), query);

		select.bind(1, highscore_.crc);
		select.bind(2, highscore_.points);
		select.bind(3, limits[n]);

		while (select.executeStep()) {
			limits[n]--;
			rows.push_back(boost::str(boost::format("[%1%] %2$ 9s: %3% sp")
				% select.getColumn("created")
				% select.getColumn("name")
				% select.getColumn("points")
			));
		}

		if (n == 0 && limits[n]) {
			limits[1] += limits[0];
		}
	}
	while (static_cast<int>(rows.size()) < first + second) {
		rows.push_back("[1970-01-01]         -: -");
	}

	entries_.set_label(boost::algorithm::join(rows, "\n"));
	entries_.set_after(64);

	form_.finalize();
}

void highscore_controller::back_btn_click(::SDL_Event& event)
{
	app_.queue_controller(
		pup::controller_ptr(new mainmenu_controller(app_))
	);
}

pause_controller::pause_controller(pup::application& app, const pup::controller_ptr& save) :
	pup::form_controller<ui::ng_theme, ui::ng_form>(app),
	saved_(save),
	continue_btn_(
		form_,
		"continue_btn",
		boost::bind(&pause_controller::continue_btn_click, this, _1)
	),
	back_btn_(
		form_,
		"back_btn",
		boost::bind(&pause_controller::back_btn_click, this, _1)
	)
{
	pup::string_vector hints {
#ifdef PUP_WIN
		"You are running the\nwindows build.",
		"Best Windows version?\nEasy; 3.11.",
#else
		"You are running the\nunix build.",
		"Gaming on linux?\nThis IS the future.",
#endif
		"This game might be good\nfor your brain.",
		"Match high on the Y-axis\nfor more SkillPoints.",
		"<INSERT WITTY PAUSE TEXT>",
		"Match width and depth\nsimultaneously for a bonus.",
		"Playing this game might\nmake you a communist.",
		"The Random Number Generator\nis not your friend.",
		"Orange is both a color\nand a fruit!",
		"Don't copy that floppy!",
		"This game is written in C++.",
		"Did you read the README?",
		"You can change the FOV\nbut why would you?",
		"The Zero-G gamemode was\ncreated for masochists.",
		"This message was\nrandomly selected.",
		"The sum of all natural\nnumbers is -1/12.",
		"C++ templates are\nTuring-complete.",
		"\"Things are seldom\nwhat they seem.\"",
		"You can lift some shapes.",
		"Thank you for playing!",
		"Your taste in games\nis impeccable!",
		"This game was inspired by\na game beginning with a \"T\".",
		"You will loose power\ncharge when pausing. Sorry.",
		"\"Only the shallow know\nthemselves.\" - Oscar Wilde",
		"The mass of a body is a\nmeasure of its energy content.",
		"Do not store cats in boxes with\nradioactive emitters and poison.",
		"Do NOT trust cubes talking to you!",
		"Use powers to escape\ntricky situations.",
		"Powers can be used to\ngain massive amounts of SP.",
		"Doctors Without Borders\nwill greatly appreciate donations.",
		"SP is an abbreviation\nof \"skill points\".",
		"Apologies for the look and\nUI of the game - The Developer",
		"Bananas are naturally\nslightly radioactive.",
		"Apple Seeds are known\nto contain amygdalin.",
		"Amygdalin degrades into\nHCN when metabolized.",
		"HCN, or Hydrogen cyanide, is\na colorless, poisonous liquid.",
		"Doctors Without Borders received\nthe Nobel Peace Prize in 1999.",
		"\"Even the darkest night will end\nAnd the sun will rise.\"",
		"\"Every moment brings a treasure,\nOf its own especial pleasure.\"",
		"Cubes are superior to spheres\nin every way imaginable.",
		"e^(i*pi)+1 = 0",
		"(cos(x)+i*sin(x))^(n) =\n= cos(n*x)+i*sin(n*x)",
		"\"There was a barber and his wife,\nAnd she was beautiful.\"",
		"Leonhard Euler was a pioneering\nSwiss mathematician and physicist.",
		"\"This is our cry. This is our\nprayer. Peace in the world.\"",
		"\"Would have, could have, should\nhave - it don't matter.\""
	};
	
	std::string label("---\n\n");
	
	label.append(*pup::random_element(
		hints.begin(),
		hints.end()
	));
	
	form_.get_title().set_label("Game Paused");
	form_.get_subtitle().set_label(label);
	
	form_.get_menu().push_back(continue_btn_);
	continue_btn_.set_label("Continue Game");
	
	form_.get_menu().push_back(back_btn_);
	back_btn_.set_label("Main Menu");

	form_.finalize();
}

void pause_controller::continue_btn_click(::SDL_Event& event)
{
	app_.queue_controller(saved_);
}

void pause_controller::back_btn_click(::SDL_Event& event)
{
	app_.get_music().fadeout(3000);
	app_.queue_controller(
		pup::controller_ptr(new mainmenu_controller(app_))
	);
}

} // fabl
