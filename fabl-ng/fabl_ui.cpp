
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
namespace ui {

ng_theme::ng_theme(pup::gl1::ft::typewriter& typewriter) :
	pup::gl1::ui::theme(
		typewriter,
		pup::gl1::color::from_hex("#F0EEEE"), // label
		pup::gl1::color::from_hex("#FFFFFF"), // value
		pup::gl1::color::from_hex("#FFFFFF"), // button
		pup::gl1::color::from_hex("#004800"), // norm_box
		pup::gl1::color::from_hex("#770010"), // err_box
		pup::gl1::color::from_hex("#4D4D4D"), // bg_box
		pup::gl1::color::from_hex("#003200"), // shadow_box
		pup::gl1::color::from_hex("#065F06"), // antishadow_box
		pup::gl1::rgb(+0.10f, +0.10f, +0.10f), // focus_delta
		pup::gl1::rgb(+0.05f, +0.05f, +0.05f) // hover_delta
	)
{
	this->set_label_face(typewriter.find_ptr("UbuntuMono-R", 10));
	this->set_value_face(typewriter.find_ptr("UbuntuMono-R", 14));
	this->set_button_face(typewriter.find_ptr("UbuntuMono-R", 14));
}

ng_form::ng_form(pup::controller& ctrlr, pup::gl1::ui::theme& thm) :
	pup::gl1::ui::static_focus_form(ctrlr, thm),
	l_space_(*this, "l_space"),
	r_space_(*this, "r_space"),
	menu_top_space_(*this, "menu_top_space"),
	title_(
		*this,
		"title",
		ctrlr.get_app().get_typewriter().find_ptr("UbuntuMono-R", 32),
		pup::gl1::color::from_hex("#ffffff"),
		pup::gl1::ft::ALIGN_CENTER
	),
	subtitle_(
		*this,
		"subtitle",
		ctrlr.get_app().get_typewriter().find_ptr("UbuntuMono-R", 14),
		pup::gl1::color::from_hex("#ffffff"),
		pup::gl1::ft::ALIGN_CENTER
	),
	menu_(*this, "menu")
{
	pup::gl1::rgb bg(this->get_theme().get("bg_box"));

	this->get_collection().push_back(l_space_);
	l_space_.set_width_rel2parent(30);
	l_space_.set_content_box_col(bg);

	this->get_collection().push_back(menu_);
	menu_.set_width_rel2parent(40);
	menu_.set_padding(10);
	menu_.set_content_box_col(bg);

	this->get_collection().push_back(r_space_);
	r_space_.set_width_rel2parent(30);
	r_space_.set_content_box_col(bg);

	menu_.push_back(menu_top_space_);
	menu_top_space_.set_height_rel2parent(10);
	menu_top_space_.set_after(10);
	menu_top_space_.set_content_box_col(bg);

	menu_.push_back(title_);
	title_.set_label("FallingBlocks");
	title_.set_after(20);

	menu_.push_back(subtitle_);
	subtitle_.set_label("NextGeneration");
	subtitle_.set_after(80);
}

ng_w3col_form::ng_w3col_form(pup::controller& ctrlr, pup::gl1::ui::theme& thm) :
	ng_form(ctrlr, thm),
	l_box_(*this, "l_box"),
	box_l_space_(*this, "box_l_space"),
	m_box_(*this, "m_box"),
	box_r_space_(*this, "box_r_space"),
	r_box_(*this, "r_box")
{
	pup::gl1::rgb bg(this->get_theme().get("bg_box"));
	
	this->get_menu().push_back(l_box_);
	l_box_.set_width_rel2parent(32);
	l_box_.set_content_box_col(bg);
	
	this->get_menu().push_back(box_l_space_);
	box_l_space_.set_width_rel2parent(2);
	box_l_space_.set_content_box_col(bg);
	
	this->get_menu().push_back(m_box_);
	m_box_.set_width_rel2parent(32);
	m_box_.set_content_box_col(bg);
	
	this->get_menu().push_back(box_r_space_);
	box_r_space_.set_width_rel2parent(2);
	box_r_space_.set_content_box_col(bg);
	
	this->get_menu().push_back(r_box_);
	r_box_.set_width_rel2parent(32);
	r_box_.set_content_box_col(bg);
}

} // ui
} // fabl
