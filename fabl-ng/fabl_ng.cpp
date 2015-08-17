
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
namespace ng {

shape::shape(
	const ::Uint32 id,
	const ::Uint32 side,
	const ::Uint32 highdice,
	const ::Uint32 lifts,
	const bool rotatable,
	const std::string& name,
	const bool shallow
) :
	id_(id),
	name_(name),
	rotatable_(rotatable),
	shallow_(shallow),
	side_(side),
	highdice_(highdice),
	lifts_(lifts),
	blocks_(shallow? nullptr: new block[side * side * side])
{
	// Warning: the order matters! See rotations.
	rotations_.push_back(boost::bind(&shape::rotate_xz_cw, this));
	rotations_.push_back(boost::bind(&shape::rotate_xz_ccw, this));
	rotations_.push_back(boost::bind(&shape::rotate_xy_cw, this));
	rotations_.push_back(boost::bind(&shape::rotate_xy_ccw, this));
	rotations_.push_back(boost::bind(&shape::rotate_yz_cw, this));
	rotations_.push_back(boost::bind(&shape::rotate_yz_ccw, this));
}

shape::~shape() throw()
{
	if (!shallow_)
		delete[] blocks_;
}

block& shape::get_block(::Uint32 x, ::Uint32 y, ::Uint32 z)
{
	return blocks_[pup::m::index_from_3d(x, y, z, side_, side_, side_)];
}

void shape::set_id(::Uint32 id)
{
	id_ = id;
	for (::Uint32 i = 0; i < this->get_volume(); ++i) {
		blocks_[i].set_unique(id_);
	}
}

void shape::set_color(const pup::gl1::rgb& col)
{
	for (::Uint32 i = 0; i < this->get_volume(); ++i) {
		if (blocks_[i].is_solid())
			blocks_[i].set_color(col);
	}
	col_ = col;
}

shape* shape::clone(const bool shallow) const
{
	shape* copy = new shape(
		id_,
		side_,
		highdice_,
		lifts_,
		rotatable_,
		name_,
		shallow
	);

	copy->pos_ = pos_;
	copy->col_ = col_;

	if (shallow) {
		copy->blocks_ = blocks_;
	} else {
		std::copy(blocks_, blocks_ + this->get_volume(), copy->blocks_);
	}
	
	return copy;
}

void shape::freeze_to(block* blocks, ::Uint32 x_l, ::Uint32 y_l, ::Uint32 z_l)
{
	PUP_3_LOOP(::Uint32,
		x = 0, x < side_, ++x,
		y = 0, y < side_, ++y,
		z = 0, z < side_, ++z,
		{
			block& blk = this->get_block(x, y, z);
			if (blk.is_solid()) {
				blocks[pup::m::index_from_3d(
					static_cast<::Uint32>(pos_.x()) + x,
					static_cast<::Uint32>(pos_.y()) + y,
					static_cast<::Uint32>(pos_.z()) + z,
					x_l,
					y_l,
					z_l
				)] = blk;
			}
		}
	);
}

power::power(blockfall& bfall, pup::timer& tmr, ::Uint32 charge_time) :
	blockfall_(bfall),
	charge_interval_(tmr, charge_time),
	blocks_(nullptr),
	active_(false),
	animate_frames_(0),
	layer_area_(blockfall_.get_xz() * blockfall_.get_xz())
{
}

power::~power() throw()
{
	delete[] blocks_;
}

bool power::activate()
{
	if (!active_ && charge_interval_.test_expired()) {
		blockfall_.power_activated();
		active_ = true;
		return true;
	}
	return false;
}

void power::reset()
{
	if (!active_) {
		charge_interval_.renew();
	}
}

std::string power::get_info()
{
	double charging = charge_interval_.remaining_sec();
	int min = static_cast<int>(charging / 60);
	int sec = static_cast<int>(charging) % 60;
	return charging?
		boost::str(boost::format("[%1$02i:%2$02i] %3%") % min % sec % this->get_name()):
		(active_?
			boost::str(boost::format("[POWER] %1%") % this->get_name()):
			boost::str(boost::format("[READY] %1%") % this->get_name())
		);
}

kill_shape_power::kill_shape_power(blockfall& bfall, pup::timer& tmr) :
	power(bfall, tmr, 1000 * 90)
{
}

kill_shape_power::~kill_shape_power() throw()
{
}
	
void kill_shape_power::update()
{
	if (active_) {
		active_ = false;
		::Uint32 points = 100;
		charge_interval_.renew();
		blockfall_.kill_active();
		blockfall_.add_skill_points(points);
		blockfall_.add_skill_message(points, "blockocide bonus");
	}
}

clear_color_power::clear_color_power(blockfall& bfall, pup::timer& tmr) :
	power(bfall, tmr, 1000 * 120),
	y_(0)
{
	blocks_ = new block[layer_area_];
	for (::Uint32 i = 0; i < layer_area_; ++i) {
		blocks_[i].set_state(ST_FILLED);
	}
}

clear_color_power::~clear_color_power() throw()
{
}

void clear_color_power::render()
{
	if (active_) {
		blockfall_.render_blocks(
			pup::gl1::d3::point(0, y_, 0),
			blocks_,
			blockfall_.get_xz(),
			1,
			blockfall_.get_xz(),
			0.25f
		);
	}
}

void clear_color_power::update()
{
	if (active_) {
		if (y_ < blockfall_.get_y()) {
			PUP_2_LOOP(::Uint32,
				x = 0, x < blockfall_.get_xz(), ++x,
				z = 0, z < blockfall_.get_xz(), ++z,
				{
					block& blk = blockfall_.get_world_block(
						x,
						y_,
						z
					);
					if (blk.is_solid() && clear_color_ == blk.get_color()) {
						blk.clear();
					}
				}
			);
			y_++;
		} else {
			charge_interval_.renew();
			clear_color_.set(0, 0, 0);
			y_ = 0;
			active_ = false;
		}
	}
}

bool clear_color_power::activate()
{
	if (blockfall_.has_active()) {
		power::activate();
		if (active_ && clear_color_ == pup::gl1::color::black) {
			clear_color_ = blockfall_.get_active().get_color();
			for (::Uint32 i = 0; i < layer_area_; ++i) {
				blocks_[i].set_color(clear_color_);
			}
			::Uint32 points = 100;
			blockfall_.add_skill_points(points);
			blockfall_.add_skill_message(points, "color scan bonus");
			return true;
		}
	}
	return false;
}

vaporize_foundation_power::vaporize_foundation_power(blockfall& bfall, pup::timer& tmr) :
	power(bfall, tmr, 1000 * 180)
{
	blocks_ = new block[layer_area_];
	for (::Uint32 i = 0; i < layer_area_; ++i) {
		blocks_[i].set_state(ST_FILLED);
		blocks_[i].set_color(pup::gl1::color::white);
	}
}

vaporize_foundation_power::~vaporize_foundation_power() throw()
{
}

void vaporize_foundation_power::render()
{
	if (animate_frames_ > 0) {
		blockfall_.render_blocks(
			pup::gl1::d3::point(0, blockfall_.get_y() - 1, 0),
			blocks_,
			blockfall_.get_xz(),
			1,
			blockfall_.get_xz(),
			static_cast<::GLfloat>(animate_frames_) / 10.0f
		);
		animate_frames_--;
	}
}

void vaporize_foundation_power::update()
{
	if (active_) {
		PUP_2_LOOP(::Uint32,
			x = 0, x < blockfall_.get_xz(), ++x,
			z = 0, z < blockfall_.get_xz(), ++z,
			{
				blockfall_.get_world_block(x, blockfall_.get_y() - 1, z).clear();
			}
		);
		
		if (blockfall_.get_gm_zero_g()) {
			blockfall_.set_gm_zero_g(false);
			blockfall_.sweep_islands();
			blockfall_.set_gm_zero_g(true);
		}

		::Uint32 points = 100;
		blockfall_.add_skill_points(points);
		blockfall_.add_skill_message(points, "vaporization bonus");
		
		charge_interval_.renew();
		active_ = false;
		animate_frames_ = 9;
	}
}

spike_trap_power::spike_trap_power(blockfall& bfall, pup::timer& tmr) :
	power(bfall, tmr, 1000 * 240),
	restore_gm_(false),
	gm_bottom_mesh_(false)
{
}

spike_trap_power::~spike_trap_power() throw()
{
}

void spike_trap_power::levelup()
{
	if (restore_gm_) {
		blockfall_.set_gm_bottom_mesh(gm_bottom_mesh_);
		restore_gm_ = false;
	}
}

void spike_trap_power::update()
{
	if (active_) {
		gm_bottom_mesh_ = blockfall_.get_gm_bottom_mesh();
		pillarize(blockfall_, 1, 3);
		blockfall_.set_gm_bottom_mesh(false);
		charge_interval_.renew();
		
		::Uint32 points = 10;
		blockfall_.add_skill_points(points);
		blockfall_.add_skill_message(points, "spike bonus");
		
		active_ = false;
		restore_gm_ = true;
	}
}

gravity_beam_power::gravity_beam_power(blockfall& bfall, pup::timer& tmr) :
	power(bfall, tmr, 1000 * 480),
	x_(0),
	z_(0)
{
	blocks_ = new block[1 * bfall.get_y() * 1];
	for (::Uint32 i = 0; i < bfall.get_y(); ++i) {
		blocks_[i].set_state(ST_FILLED);
		blocks_[i].set_color(pup::gl1::color::white);
	}
}

gravity_beam_power::~gravity_beam_power() throw()
{
}

void gravity_beam_power::render()
{
	if (animate_frames_ > 0) {
		blockfall_.render_blocks(
			pup::gl1::d3::point(x_ - 1, 0, z_),
			blocks_,
			1,
			blockfall_.get_y(),
			1,
			0.25f
		);
	}
}

void gravity_beam_power::update()
{
	while (active_) {
		if (x_ < blockfall_.get_xz()) {
			for (::Sint32 y = blockfall_.get_y() - 2; y >= 0; --y) {
				this->drop_at(x_, y, z_);
			}
			x_++;
			animate_frames_ = 1;
			break;
		} else {
			if (z_ < blockfall_.get_xz() - 1) {
				z_++;
			} else {
				::Uint32 points = 100;
				blockfall_.add_skill_points(points);
				blockfall_.add_skill_message(points, "v(t)=-gt+v0 bonus");
				charge_interval_.renew();
				active_ = false;
				animate_frames_ = 0;
				z_ = 0;
			}
			x_ = 0;
		}
	}
}

void gravity_beam_power::drop_at(::Uint32 x, ::Uint32 y, ::Uint32 z)
{
	while (
		y + 1 < blockfall_.get_y() &&
		blockfall_.get_world_block(x, y + 0, z).is_solid() == true &&
		blockfall_.get_world_block(x, y + 1, z).is_solid() == false
	) {
		std::swap(
			blockfall_.get_world_block(x, y + 0, z),
			blockfall_.get_world_block(x, y + 1, z)
		);
		y++;
	}
}

factory::factory(
	const boost::filesystem::path& world_file,
	bool colorblind_rg_match,
	bool verbose
) :
	world_file_(world_file),
	random_colors_(false)
{
	if (!boost::filesystem::exists(world_file)) {
		PUP_ERR(std::runtime_error, boost::str(boost::format(
			"shapes file %1% does not exist") % world_file.string()));
	}
	boost::property_tree::read_json(world_file.string(), pt_);

	xz_ = pt_.get<::Uint32>("world_xz");
	y_ = pt_.get<::Uint32>("world_y");

	if (verbose) {
		BOOST_LOG_TRIVIAL(info) << boost::format(
			"fabl::factory()\n"
			"\tworld_file: %1%\n"
			"\tworld_description: %2%\n"
			"\tworld_xz: %3%\n"
			"\tworld_y: %4%\n"
		)
			% world_file.string()
			% pt_.get<std::string>("description")
			% xz_
			% y_;
	}
	
	if (colorblind_rg_match) {
		colors_.push_back(pup::gl1::color::y);
		colors_.push_back(pup::gl1::color::c);
		colors_.push_back(pup::gl1::color::m);
	} else {
		colors_.push_back(pup::gl1::color::r);
		colors_.push_back(pup::gl1::color::g);
		colors_.push_back(pup::gl1::color::b);
	}
}

void factory::setup()
{
	using boost::property_tree::ptree;

	BOOST_FOREACH(ptree::value_type& v_shape, pt_.get_child("shapes")) {
		shape_ptr s_ptr(new shape(
			0,
			v_shape.second.get<::Uint32>("cubed"),
			v_shape.second.get<::Uint32>("highdice"),
			v_shape.second.get<::Uint32>("lifts"),
			v_shape.second.get<bool>("rotatable"),
			v_shape.second.get<std::string>("name")
		));

		pup::gl1::rgb color;
		pup::int_vector blocks;

		color = pup::gl1::color::from_hex(v_shape.second.get<std::string>("color"));

		BOOST_FOREACH(ptree::value_type& v_block, v_shape.second.get_child("blocks")) {
			blocks.push_back(v_block.second.get_value<int>());
		}

		if (blocks.size() != s_ptr->get_volume())
			PUP_ERR(std::runtime_error, "shape.cubed^3 != number of blocks");

		for (pup::int_vector::size_type i = 0; i < blocks.size(); ++i) {
			if (blocks[i] != 0) {
				s_ptr->get_blocks()[i].set_state(ST_FILLED);
			}
		}
		s_ptr->set_color(color);

		shapes_.push_back(s_ptr);
	}
}

shape_ptr factory::create()
{
	using boost::random::uniform_int_distribution;

	shape_ptr s_ptr;
	::Uint16 s_retries = 9;

	do {
		s_ptr = *pup::random_element(
			shapes_.begin(),
			shapes_.end()
		);
		unidist_.param(uniform_int_distribution<>::param_type(
			0,
			s_ptr->get_highdice()
		));

		if (!s_ptr->get_highdice() || unidist_(rng_) == 0)
			break;
	} while (--s_retries != 0);

	s_ptr = shape_ptr(s_ptr->clone());
	s_ptr->set_id(++next_id_);

	for (::Uint32 i = 0; i < 2; ++i)
		s_ptr->random_rotation();

	unidist_.param(uniform_int_distribution<>::param_type(
		0,
		xz_ - s_ptr->get_side()
	));

	s_ptr->get_position().x(unidist_(rng_));
	s_ptr->get_position().z(unidist_(rng_));

	if (random_colors_) {
		s_ptr->set_color(*pup::random_element(
			colors_.begin(),
			colors_.end()
		));
	}

	return s_ptr;
}

} // ng
} // fabl
