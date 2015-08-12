
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

namespace pup {

const char* application_name() throw()
{
	return APPLICATION_NAME;
}

const char* application_version() throw()
{
	return APPLICATION_VERSION;
}

} // pup

namespace fabl {

void pillarize(blockfall& bfall, ::Uint32 modv, ::Uint32 height)
{
	::Uint32 n = 0;
	const ::Uint32 lim_xz = bfall.get_xz();
	const ::Uint32 lim_y = bfall.get_y();
	PUP_2_LOOP(::Uint32,
		x = 0, x < lim_xz, ++x,
		z = 0, z < lim_xz, ++z,
		{
			if (n++ % 2 == modv) {
				for (::Uint32 y = lim_y - 1; y > lim_y - height; --y) {
					ng::block& blk = bfall.get_world_block(x, y, z);
					blk.set_state(ng::ST_FILLED);
					blk.set_color(pup::gl1::color::white);
				}
			}
		}
	);
}

blockfall::blockfall(
	pup::timer& tmr,
	const gameover_function& gameover,
	const sndfx_function& sweepsnd,
	const sndfx_function& lvlupsnd,
	const boost::filesystem::path& world_file,
	const bool colorblind_rg_match,
	const bool gm_bottom_mesh,
	const bool gm_zero_g,
	const bool gm_color_match,
	const bool gm_one_shape_limit
) :
	gameover_(gameover),
	sweepsnd_(sweepsnd),
	lvlupsnd_(lvlupsnd),
	timer_(tmr),
	msg_interval_(tmr, 250),
	step_interval_(tmr, 2000),
	spawn_interval_(tmr, 8000),
	power_interval_(tmr, 100),
	gm_bottom_mesh_(gm_bottom_mesh),
	gm_zero_g_(gm_zero_g),
	gm_color_match_(gm_color_match),
	gm_one_shape_limit_(gm_one_shape_limit),
	xz_(0),
	y_(0),
	volume_(0),
	last_level_up_(0),
	lines_cleared_(0),
	shapes_spawned_(0),
	skill_level_(1),
	skill_points_(0),
	skill_ui_points_(0),
	skill_steps_(0),
	skill_factor_(1),
	flash_msg_frame_(0),
	flash_msg_frames_(60),
	world_blocks_(nullptr),
	factory_(world_file, colorblind_rg_match),
	shapes_(),
	active_(shapes_.end()),
	rtmode_(ng::RT_XZ),
	kill_shape_power_(nullptr),
	clear_color_power_(nullptr),
	vaporize_foundation_power_(nullptr),
	spike_trap_power_(nullptr),
	gravity_beam_power_(nullptr)
{
	factory_.setup();
}

blockfall::~blockfall() throw()
{
	this->destroy_world();
}

ng::block& blockfall::get_world_block(::Uint32 x, ::Uint32 y, ::Uint32 z)
{
	return world_blocks_[pup::m::index_from_3d(x, y, z, xz_, y_, xz_)];
}

void blockfall::create_world()
{
	this->destroy_world();

	xz_ = factory_.get_world_xz();
	y_ = factory_.get_world_y();
	volume_ = xz_ * y_ * xz_;

	bottom_cube_.dim.w(this->scale(xz_));
	bottom_cube_.dim.h(0.01);
	bottom_cube_.dim.d(this->scale(xz_));
	bottom_cube_.col.set(0.4, 0.4, 0.4);

	::GLdouble y_line_x = +this->scale(xz_) / 2.0;
	::GLdouble y_line_z = -this->scale(xz_) / 2.0;

	x_line_.pos0.xyz(+y_line_x, 0.01, y_line_z);
	x_line_.pos1.xyz(-y_line_x, 0.01, y_line_z);
	x_line_.col.set(pup::gl1::color::r);
	x_line_.lw = 4.0;

	y_line_.pos0.xyz(y_line_x, 0.01, y_line_z);
	y_line_.pos1.xyz(y_line_x, this->scale(y_) + 0.01, y_line_z);
	y_line_.col.set(pup::gl1::color::g);
	y_line_.lw = 4.0;

	z_line_.pos0.xyz(y_line_x, 0.01, +y_line_z);
	z_line_.pos1.xyz(y_line_x, 0.01, -y_line_z);
	z_line_.col.set(pup::gl1::color::b);
	z_line_.lw = 4.0;

	x_arrow_.dir.xyz(0.0, 0.0, 1.0);
	x_arrow_.a = 90.0f;
	x_arrow_.sza = 0.2;
	x_arrow_.szo = 0.05;
	x_arrow_.pos.x(+0.01);
	x_arrow_.pos.y(+y_line_x + 0.2);
	x_arrow_.pos.z(+y_line_z);
	x_arrow_.col.set(pup::gl1::color::r);

	y_arrow_.dir.xyz(0.0, 1.0, 0.0);
	y_arrow_.a = 0.0f;
	y_arrow_.sza = 0.2;
	y_arrow_.szo = 0.05;
	y_arrow_.pos.x(+y_line_x);
	y_arrow_.pos.y(+this->scale(y_) + 0.01 + 0.2);
	y_arrow_.pos.z(+y_line_z);
	y_arrow_.col.set(pup::gl1::color::g);

	z_arrow_.dir.xyz(1.0, 0.0, 0.0);
	z_arrow_.a = 90.0f;
	z_arrow_.sza = 0.2;
	z_arrow_.szo = 0.05;
	z_arrow_.pos.x(+y_line_x);
	z_arrow_.pos.y(+y_line_x + 0.2);
	z_arrow_.pos.z(+0.01);
	z_arrow_.col.set(pup::gl1::color::b);

	world_blocks_ = new ng::block[volume_];
	
	kill_shape_power_.reset(
		new ng::kill_shape_power(*this, timer_)
	);
	clear_color_power_.reset(
		new ng::clear_color_power(*this, timer_)
	);
	vaporize_foundation_power_.reset(
		new ng::vaporize_foundation_power(*this, timer_)
	);
	spike_trap_power_.reset(
		new ng::spike_trap_power(*this, timer_)
	);
	gravity_beam_power_.reset(
		new ng::gravity_beam_power(*this, timer_)
	);
	
	powers_.push_back(kill_shape_power_);
	powers_.push_back(clear_color_power_);
	powers_.push_back(vaporize_foundation_power_);
	powers_.push_back(spike_trap_power_);
	powers_.push_back(gravity_beam_power_);
}

void blockfall::destroy_world() throw()
{
	powers_.clear();
	
	kill_shape_power_.reset();
	clear_color_power_.reset();
	vaporize_foundation_power_.reset();
	spike_trap_power_.reset();
	gravity_beam_power_.reset();
	
	xz_ = 0;
	y_ = 0;

	delete[] world_blocks_;
}

void blockfall::block_fall()
{
	while (this->move_shape(+0.0, +1.0, +0.0))
		pup::do_nothing();
}

void blockfall::rotate_shape_cw()
{
	if (rtmode_ == ng::RT_XZ) { (*active_)->rotate_xz_cw(); }
	if (rtmode_ == ng::RT_XY) { (*active_)->rotate_xy_cw(); }
	if (rtmode_ == ng::RT_YZ) { (*active_)->rotate_yz_cw(); }

	if (this->collide_shape(0.0, 0.0, 0.0, *active_))
		this->rotate_shape_ccw();
}

void blockfall::rotate_shape_ccw()
{
	if (rtmode_ == ng::RT_XZ) { (*active_)->rotate_xz_ccw(); }
	if (rtmode_ == ng::RT_XY) { (*active_)->rotate_xy_ccw(); }
	if (rtmode_ == ng::RT_YZ) { (*active_)->rotate_yz_ccw(); }

	if (this->collide_shape(0.0, 0.0, 0.0, *active_))
		this->rotate_shape_cw();
}

void blockfall::change_rotation_mode()
{
	if (
		rtmode_ != ng::RT_XZ &&
		rtmode_ != ng::RT_XY &&
		rtmode_ != ng::RT_YZ
	) {
		PUP_ERR(std::runtime_error, "unknown rotation mode");
	}
	else if (rtmode_ == ng::RT_XZ) { rtmode_ = ng::RT_XY; }
	else if (rtmode_ == ng::RT_XY) { rtmode_ = ng::RT_YZ; }
	else if (rtmode_ == ng::RT_YZ) { rtmode_ = ng::RT_XZ; }
}

bool blockfall::move_shape(double dX, double dY, double dZ)
{
	if (active_ != shapes_.end()) {
		return this->move_shape(dX, dY, dZ, *active_);
	}
	return false;
}

bool blockfall::move_shape(double dX, double dY, double dZ, ng::shape_ptr s_ptr)
{
	if (!this->collide_shape(dX, dY, dZ, s_ptr)) {
		s_ptr->get_position().x_d(dX);
		s_ptr->get_position().y_d(dY);
		s_ptr->get_position().z_d(dZ);
		return true;
	}
	return false;
}

bool blockfall::collide_shape(double dX, double dY, double dZ, ng::shape_ptr s_ptr,
	ng::shape_ptr ignore_ptr, bool world_only)
{
	pup::gl1::d3::point pos(s_ptr->get_position());

	pos.x_d(dX);
	pos.y_d(dY);
	pos.z_d(dZ);

	int side = s_ptr->get_side();

	PUP_3_LOOP(int,
		x = 0, x < side, ++x,
		y = 0, y < side, ++y,
		z = 0, z < side, ++z,
		{
			if (!s_ptr->get_block(x, y, z).is_solid()) {
				continue;
			}

			int world_x = static_cast<int>(pos.x()) + x;
			int world_y = static_cast<int>(pos.y()) + y;
			int world_z = static_cast<int>(pos.z()) + z;

			if (
				(world_x < 0 || world_x >= static_cast<int>(xz_)) ||
				(world_z < 0 || world_z >= static_cast<int>(xz_)) ||
				(world_y < 0 || world_y >= static_cast<int>(y_))
			) {
				return true;
			}
			if (this->get_world_block(world_x, world_y, world_z).is_solid()) {
				return true;
			}
		}
	);

	if (world_only) {
		return false;
	}

	return false;
}

bool blockfall::next_active()
{
	ng::shape_list::iterator tmp(active_);
	if (tmp != shapes_.end() && ++tmp != shapes_.end()) {
		++active_;
		return true;
	}
	return false;
}

bool blockfall::prev_active()
{
	if (active_ != shapes_.begin()) {
		--active_;
		return true;
	}
	return false;
}

void blockfall::render()
{
	bottom_cube_.render();
	
	x_line_.render();
	y_line_.render();
	z_line_.render();

	x_arrow_.render();
	y_arrow_.render();
	z_arrow_.render();

	this->render_blocks(
		pup::gl1::d3::point(0.0, 0.0, 0.0),
		world_blocks_,
		xz_,
		y_,
		xz_,
		1.0f
	);

	for (auto it = shapes_.begin(); it != shapes_.end(); ++it) {
		this->render_blocks(
			(*it)->get_position(),
			(*it)->get_blocks(),
			(*it)->get_side(),
			(*it)->get_side(),
			(*it)->get_side(),
			active_ == it? 1.0f: 0.5f
		);
		if (active_ == it)
			this->render_shadow(*it);
	}
	
	for (auto it = powers_.begin(); it != powers_.end(); ++it) {
		(*it)->render();
	}
}

pup::gl1::d3::point blockfall::get_scaled_origo() const
{
	return pup::gl1::d3::point(
		+this->scale(xz_) / 2.0,
		+this->scale(y_),
		+this->scale(xz_) / 2.0
	);
}

::GLdouble blockfall::get_scaled_side() const
{
	return this->scale(xz_) / static_cast<::GLdouble>(xz_);
}

void blockfall::checksum(highscore& hs)
{
	std::string buf(boost::str(boost::format("w(%1%)_gm1(%2%)_gm2(%3%)_gm3(%4%)_gm4(%5%)")
		% hs.world
		% hs.gm_bottom_mesh
		% hs.gm_zero_g
		% hs.gm_color_match
		% hs.gm_one_shape_limit
	));

	boost::crc_32_type crc_32;
	crc_32.process_bytes(buf.data(), buf.length());
	hs.crc = crc_32.checksum();
}

highscore blockfall::get_highscore() const
{
	highscore hs;

	hs.fabl = FABL_VERSION;
	hs.world = boost::filesystem::basename(factory_.get_world_file());
	hs.points = skill_points_;
	hs.steps = skill_steps_;
	hs.levels = skill_level_;
	hs.lines = lines_cleared_;
	hs.shapes = shapes_spawned_;
	hs.powers = powers_activated_;
	hs.gm_bottom_mesh = gm_bottom_mesh_;
	hs.gm_zero_g = gm_zero_g_;
	hs.gm_color_match = gm_color_match_;
	hs.gm_one_shape_limit = gm_one_shape_limit_;

	blockfall::checksum(hs);

	return hs;
}

void blockfall::render_blocks(const pup::gl1::d3::point& pt, const ng::block* blocks,
	const ::Uint32 x_l, const ::Uint32 y_l, const ::Uint32 z_l,
	const ::GLfloat alpha)
{
	::GLdouble side = this->get_scaled_side();

	pup::gl1::d3::cuboid drawable_cube;
	pup::gl1::d3::point origo(this->get_scaled_origo());
	
	drawable_cube.dim = pup::gl1::d3::size(side, side, side);

	PUP_3_LOOP(::Uint32,
		x = 0, x < x_l, ++x,
		y = 0, y < y_l, ++y,
		z = 0, z < z_l, ++z,
		{
			::Uint32 i = pup::m::index_from_3d(
				x, y, z,
				x_l, y_l, z_l
			);
			if (!blocks[i].need_render()) {
				continue;
			}
			drawable_cube.col.set(blocks[i].get_color());
			drawable_cube.pos.xyz(
				origo.x() - side * (pt.x() + x) - side / 2,
				origo.y() - side * (pt.y() + y) - side / 2,
				origo.z() - side * (pt.z() + z) - side / 2
			);
			drawable_cube.render(alpha);
		}
	);
}

void blockfall::render_shadow(ng::shape_ptr s_ptr)
{
	double dY = 0.0;
	ng::shape_ptr ghost_ptr(s_ptr->clone(true));
	
	while (!this->collide_shape(0.0, dY + 1.0, 0.0, ghost_ptr, s_ptr)) {
		dY += 1.0;
	}

	ghost_ptr->get_position().y_d(+dY);

	this->render_blocks(
		ghost_ptr->get_position(),
		ghost_ptr->get_blocks(),
		ghost_ptr->get_side(),
		ghost_ptr->get_side(),
		ghost_ptr->get_side(),
		0.5f
	);
}

void blockfall::update()
{
	bool flush_message = false;
	
	if (step_interval_.expired()) {
		this->update_freeze();
		this->update_sweep();
		this->update_move();
		skill_steps_++;
		flush_message = true;
		
		if (lines_cleared_ / 10 != last_level_up_) {
			const ::Uint32 step_delta = step_interval_.delta() - 100;
			const ::Uint64 level_bonus = skill_level_++ * 1000;
			last_level_up_ = lines_cleared_ / 10;
			skill_points_ += level_bonus;
			if (step_delta > 200) {
				step_interval_.set_delta(step_delta);
				spawn_interval_.set_delta(step_delta * 3);
			}
			lvlupsnd_();
			this->add_flash_message("LVL UP!");
			this->add_skill_message(level_bonus, boost::str(boost::format(
				"%1% level bonus"
			) % level_bonus));

			if (gm_bottom_mesh_)
				this->grow_bottom_mesh();
			
			for (auto it = powers_.begin(); it != powers_.end(); ++it) {
				(*it)->levelup();
			}
		}
		step_interval_.renew();
	}
	
	if (power_interval_.expired()) {
		for (auto it = powers_.begin(); it != powers_.end(); ++it) {
			(*it)->update();
		}
	}

	bool missing_shape = shapes_.empty();
	
	if ((!gm_one_shape_limit_ && spawn_interval_.expired()) || missing_shape) {
		ng::shape_ptr s_ptr(factory_.create());
		shapes_.push_back(s_ptr);
		if (missing_shape)
			active_ = shapes_.begin();
		if (this->collide_shape(0.0, 0.0, 0.0, s_ptr)) {
			gameover_();
		}
		shapes_spawned_++;
		spawn_interval_.renew();
	}
	
	if (msg_interval_.expired() || flush_message) {
		if (!waiting_skill_msgs_.empty()) {
			auto rbegin = waiting_skill_msgs_.rbegin();
			skill_ui_points_ += rbegin->first;
			skill_msgs_.push_front(rbegin->second);
			waiting_skill_msgs_.pop_back();
			sweepsnd_();
		}
		if (skill_msgs_.size() > this->get_max_skill_messages()) {
			skill_msgs_.pop_back();
		}
		msg_interval_.renew();
	}
}

bool blockfall::update_move()
{
	for (auto it = shapes_.begin(); it != shapes_.end(); ++it) {
		this->move_shape(+0.0, +1.0, +0.0, *it);
	}
	return true;
}

bool blockfall::update_freeze(double dY)
{
	bool frozen = false;

	for (auto it = shapes_.begin(); it != shapes_.end(); /* in-loop */) {
		if (this->collide_shape(+0.0, +dY, +0.0, *it, nullptr, true)) {
			(*it)->freeze_to(world_blocks_, xz_, y_, xz_);
			if (active_ == it) {
				active_ = it = shapes_.erase(it);
			} else {
				it = shapes_.erase(it);
			}
			frozen = true;
		} else
			++it;
	}
	if (active_ == shapes_.end()) {
		active_ = shapes_.begin();
	}
	return frozen;
}

void blockfall::update_sweep()
{
	bool rows;
	bool islands;
	bool frozen;

	::Uint32 passes = 0;

	do {
		rows = this->sweep_rows();
		islands = this->sweep_islands();
		frozen = this->sweep_shapes();
		passes++;
	} while (rows || islands || frozen);

	if (passes > 2) {
		::Uint32 factor = skill_factor_ * passes;
		::Uint32 bonus = 100 * factor;
		skill_points_ += bonus;
		this->add_skill_message(bonus, boost::str(boost::format(
			"%1%x clear bonus"
		) % factor));
	}

	skill_factor_ = 1;
}

int blockfall::drop_block(int n, const ::Uint32 x, const ::Uint32 y, const ::Uint32 z, int dY)
{
	int i = y;
	while (i + dY >= 0 && i + dY < static_cast<int>(y_) && n != 0) {
		ng::block& curr = this->get_world_block(x, i + 0, z);
		ng::block& next = this->get_world_block(x, i + dY, z);
		if (!curr.is_solid() || next.is_solid())
			break;
		std::swap(curr, next);
		i += dY;
		n -= 1;
	}
	return i - static_cast<int>(y);
}

void blockfall::drop_walk(ng::block_addr_map& floaters, ng::block* base,
	const int x, const int y, const int z, const int orig_x, const int orig_z)
{
	if (
		x >= 0 && x < static_cast<int>(xz_) &&
		z >= 0 && z < static_cast<int>(xz_) &&
		y >= 0 && y < static_cast<int>(y_) &&
		!gm_zero_g_
	) {
		ng::block* step = &this->get_world_block(x, y, z);

		if (step->is_solid() && base == nullptr) {
			ng::block_addr_set addrs;
			ng::block_addr_map::value_type addrs_val(step, addrs);
			if (floaters.insert(addrs_val).second) {
				this->drop_walk(floaters, step, x - 1, y, z, orig_x, orig_z);
				this->drop_walk(floaters, step, x + 1, y, z, orig_x, orig_z);
				this->drop_walk(floaters, step, x, y - 1, z, orig_x, orig_z);
				this->drop_walk(floaters, step, x, y, z - 1, orig_x, orig_z);
				this->drop_walk(floaters, step, x, y, z + 1, orig_x, orig_z);
			}
		} else if (
			step->is_solid() &&
			base != step &&
			base->get_unique() == step->get_unique()
		) {
			if (floaters.find(base)->second.insert(step).second) {
				this->drop_walk(floaters, base, x - 1, y, z, orig_x, orig_z);
				this->drop_walk(floaters, base, x + 1, y, z, orig_x, orig_z);
				this->drop_walk(floaters, base, x, y - 1, z, orig_x, orig_z);
				this->drop_walk(floaters, base, x, y + 1, z, orig_x, orig_z);
				this->drop_walk(floaters, base, x, y, z - 1, orig_x, orig_z);
				this->drop_walk(floaters, base, x, y, z + 1, orig_x, orig_z);
			}
		} else if (
			step->is_solid() &&
			x == orig_x &&
			z == orig_z
		) {
			floaters.insert(ng::block_addr_map::value_type(step, ng::block_addr_set()));
			this->drop_walk(floaters, step, x, y - 1, z, orig_x, orig_z);
		} else {
			pup::do_nothing();
		}
	}
}

::Uint64 blockfall::sweep_rows_reward(const ::Uint32 y)
{
	const ::Uint32 dY = this->y_ - y + 1;
	const ::Uint32 xz_reward = this->xz_ * this->skill_level_;
	const ::Uint32 reward_factor = dY * xz_reward * skill_factor_++;
	const ::Uint32 base_reward = 100;
	const ::Uint64 points = base_reward * reward_factor;
	this->add_skill_message(points, boost::str(boost::format(
		"%1%x normal clear"
	) % reward_factor));
	return points;
}

bool blockfall::sweep_rows()
{
	// Clear rows in the {%x, %z} planes, starting from the top and
	// moving down. Going from the top and down is important since
	// the other way around might cheat the gamer of matched rows
	// since blocks would start falling down into the space of the
	// cleared row.

	this->assign_block_coords(true);

	bool swept_x = false;
	bool swept_z = false;
	
	ng::block_addr_map floaters;
	
#define COLOR_LOOP(A, B) \
	for (::Uint32 A = 0; A < xz_ && gm_color_match_; ++A) { \
		if (A##_rows[A] != xz_) \
			continue; \
		pup::gl1::rgb prev(0, 0, 0); \
		for (::Uint32 B = 0; B < xz_; ++B) { \
			ng::block& blk = this->get_world_block(x, y, z); \
			A##_colors[A] += (B == 0 || prev == blk.get_color()); \
			prev = blk.get_color(); \
		} \
	}
#define SWEEP_LOOP(A, B) \
	for (::Uint32 A = 0; A < xz_; ++A) { \
		if (A##_rows[A] != xz_ || (gm_color_match_ && A##_colors[A] != xz_)) \
			continue; \
		swept_##A = true; \
		for (::Uint32 B = 0; B < xz_; ++B) { \
			this->get_world_block(x, y, z).clear(); \
			this->drop_walk(floaters, nullptr, x, y - 1, z, x, z); \
		} \
		skill_points_ += this->sweep_rows_reward(y); \
		lines_cleared_ += 1; \
	}

	for (::Uint32 y = 0; y < y_ && !(swept_x || swept_z); ++y) {
		typedef std::vector<::Uint32> u32_vector;

		u32_vector x_rows(xz_, 0);
		u32_vector z_rows(xz_, 0);

		for (::Uint32 x = 0; x < xz_; ++x) {
			for (::Uint32 z = 0; z < xz_; ++z) {
				if (this->get_world_block(x, y, z).is_solid()) {
					x_rows[x]++;
					z_rows[z]++;
				}
			}
		}
		
		u32_vector x_colors(xz_, 0);
		u32_vector z_colors(xz_, 0);

		COLOR_LOOP(z, x)
		COLOR_LOOP(x, z)

		SWEEP_LOOP(z, x)
		SWEEP_LOOP(x, z)
	}
	
#undef SWEEP_LOOP
#undef COLOR_LOOP
	
	// Remember: each block_addr_set is sorted smallest address
	// to largest, but lower blocks in the blockfall must be droped
	// before the ones above them! This is easily fixed by walking
	// the block_addr_set backwards. To avoid missing blocks that
	// should drop due to sort order, the block_addr_set is walked
	// over and over again until no more blocks drop.

	int root_drops;
	int related_drops;
	int sp_drop_base = 10;
	int sp_drop_factor = 0;

	do {
		root_drops = 0;
		related_drops = 0;
		for (auto iit = floaters.begin(); iit != floaters.end(); ++iit) {
			int n = this->drop_block(-1, iit->first->x, iit->first->y, iit->first->z);
			root_drops += n;
			sp_drop_base += 10 * n;
			for (auto jit = iit->second.rbegin(); jit != iit->second.rend() && n > 0; ++jit) {
				int m = this->drop_block(n, (*jit)->x, (*jit)->y, (*jit)->z);
				related_drops += m;
				sp_drop_factor += 1;
			}
		}
	} while (root_drops != 0 || related_drops != 0);

	if (sp_drop_factor) {
		const ::Uint64 drop_bonus = sp_drop_base * sp_drop_factor;
		skill_points_ += drop_bonus;
		this->add_skill_message(drop_bonus, "drop bonus");
	}

	if (swept_x && swept_z) {
		const ::Uint64 xz_bonus = skill_points_ / 8;
		skill_points_ += xz_bonus;
		this->add_skill_message(xz_bonus, "xz clear bonus");
	}

	this->assign_block_coords(false);

	return swept_x || swept_z;
}

void blockfall::mainland_walk(ng::block_addr_set& mainland,
	const int x, const int y, const int z)
{
	if (
		x >= 0 && x < static_cast<int>(xz_) &&
		z >= 0 && z < static_cast<int>(xz_) &&
		y > 0 && y < static_cast<int>(y_)
	) {
		ng::block& step = this->get_world_block(x, y, z);
		if (step.is_solid() && mainland.insert(&step).second) {
			// x
			this->mainland_walk(mainland, x + 1, y, z);
			this->mainland_walk(mainland, x - 1, y, z);
			// y
			this->mainland_walk(mainland, x, y + 1, z);
			this->mainland_walk(mainland, x, y - 1, z);
			// z
			this->mainland_walk(mainland, x, y, z + 1);
			this->mainland_walk(mainland, x, y, z - 1);
		}
	}
}

bool blockfall::sweep_islands()
{
	// %sweep_rows() can create "islands" of floating blocks which
	// are not directly connected to any block which is resting
	// on the bottom of the blockfall. One way to deal with this
	// is to find all blocks that are resting and drop every other
	// block until they collide. A somewhat slow, but simple solution.

	if (gm_zero_g_) {
		return false;
	}

	::Uint32 floor_y = y_ - 1;
	ng::block_addr_set mainland;

	bool dropped = false;

	PUP_2_LOOP(::Uint32,
		x = 0, x < xz_, ++x,
		z = 0, z < xz_, ++z,
		{
			this->mainland_walk(mainland, x, floor_y, z);
		}
	);

	// Now every block that is not part of the mainland can be dropped.

	PUP_3_LOOP(::Uint32,
		y = y_ - 1, y > 0, --y,
		x = 0, x < xz_, ++x,
		z = 0, z < xz_, ++z,
		{
			ng::block& step = this->get_world_block(x, y, z);
			if (step.is_solid() && mainland.find(&step) == mainland.end()) {
				if (this->drop_block(-1, x, y, z) > 0)
					dropped = true;
			}
		}
	);

	if (dropped) {
		::Uint64 sp = skill_level_ * skill_factor_ * 100;
		skill_points_ += sp;
		this->add_skill_message(sp, "shape shatter");
	}

	return dropped;
}

bool blockfall::sweep_shapes()
{
	bool frozen = this->update_freeze(0.0);

	return frozen;
}

void blockfall::assign_block_coords(bool actual)
{
	PUP_3_LOOP(::Uint32,
		x = 0, x < xz_, ++x,
		y = 0, y < y_, ++y,
		z = 0, z < xz_, ++z,
		{
			ng::block& b = this->get_world_block(x, y, z);
			b.x = actual? x: 0;
			b.y = actual? y: 0;
			b.z = actual? z: 0;
		}
	);
}

void blockfall::grow_bottom_mesh()
{
	for (::Uint32 n = 0; n < 2; ++n) {
		PUP_3_LOOP(::Uint32,
			x = 0, x < xz_, ++x,
			z = 0, z < xz_, ++z,
			y = 0, y < y_ - 1, ++y,
			{
				std::swap(
					this->get_world_block(x, y + 0, z),
					this->get_world_block(x, y + 1, z)
				);
			}
		);
	}

	pillarize(*this, 0, 3);
}

void blockfall::clear()
{
	PUP_3_LOOP(::Uint32,
		y = 0, y < y_, ++y,
		x = 0, x < xz_, ++x,
		z = 0, z < xz_, ++z,
		{
			this->get_world_block(x, y, z).clear();
		}
	);
}

void blockfall::simplify(pup::int_vector& dest) const
{
	dest.reserve(volume_);
	for (::Uint32 i = 0; i < volume_; ++i) {
		dest.push_back(world_blocks_[i].is_solid()? world_blocks_[i].get_unique(): 0);
	}
}

void blockfall::build(const pup::int_vector& world)
{
	if (volume_ != world.size()) {
		PUP_ERR(std::logic_error, "blockfall volume != world size");
	}
	for (auto it = world.begin(); it != world.end(); ++it) {
		if (*it) {
			ng::block& blk = world_blocks_[std::distance(world.begin(), it)];
			blk.set_state(ng::ST_FILLED);
			blk.set_unique(*it);
		}
	}
}

bool blockfall::compare(const pup::int_vector& world)
{
	if (volume_ != world.size()) {
		PUP_ERR(std::logic_error, "blockfall volume != world size");
	}
	for (auto it = world.begin(); it != world.end(); ++it) {
		ng::block& blk = world_blocks_[std::distance(world.begin(), it)];
		if (
			(!*it && blk.is_solid()) ||
			(*it && (!blk.is_solid() ||
				blk.get_unique() != static_cast<::Uint32>(*it)))
		) {
			return false;
		}
	}
	return true;
}

game_controller::game_controller(pup::application& app) :
	pup::controller(app),
	blockfall_(nullptr),
	camera_alpha_(0.0),
	camera_beta_(-0.75),
	camera_distance_(10.0)
{
	PUP_KEY_DISPATCH(game_controller, camera_cw, "game_control");
	PUP_KEY_DISPATCH(game_controller, camera_ccw, "game_control");
	PUP_KEY_DISPATCH(game_controller, camera_up, "game_control");
	PUP_KEY_DISPATCH(game_controller, camera_down, "game_control");
	PUP_KEY_DISPATCH(game_controller, camera_tiltup, "game_control");
	PUP_KEY_DISPATCH(game_controller, camera_tiltdown, "game_control");
	PUP_KEY_DISPATCH(game_controller, camera_zoomin, "game_control");
	PUP_KEY_DISPATCH(game_controller, camera_zoomout, "game_control");

	PUP_KEY_DISPATCH(game_controller, block_fall, "game_control");
	PUP_KEY_DISPATCH(game_controller, block_y_plus, "game_control");
	PUP_KEY_DISPATCH(game_controller, block_y_minus, "game_control");
	PUP_KEY_DISPATCH(game_controller, block_z_plus, "game_control");
	PUP_KEY_DISPATCH(game_controller, block_z_minus, "game_control");
	PUP_KEY_DISPATCH(game_controller, block_x_plus, "game_control");
	PUP_KEY_DISPATCH(game_controller, block_x_minus, "game_control");
	PUP_KEY_DISPATCH(game_controller, block_rotate_cw, "game_control");
	PUP_KEY_DISPATCH(game_controller, block_rotate_ccw, "game_control");
	PUP_KEY_DISPATCH(game_controller, block_rotate_mode, "game_control");
	PUP_KEY_DISPATCH(game_controller, block_next, "game_control");
	PUP_KEY_DISPATCH(game_controller, block_prev, "game_control");
	
	PUP_KEY_DISPATCH(game_controller, power_kill_shape, "game_control");
	PUP_KEY_DISPATCH(game_controller, power_clear_color, "game_control");
	PUP_KEY_DISPATCH(game_controller, power_vaporize_foundation, "game_control");
	PUP_KEY_DISPATCH(game_controller, power_spike_trap, "game_control");
	PUP_KEY_DISPATCH(game_controller, power_gravity_beam, "game_control");
	
	PUP_KEY_DISPATCH(game_controller, menu, "game_control");

	view_.e.x(0.0);
	view_.e.y(2.0);
	view_.e.z(camera_distance_);

	view_.c.x(0.0);
	view_.c.y(1.0);
	view_.c.z(0.0);

	view_.u.x(0.0);
	view_.u.y(1.0);
	view_.u.z(0.0);

	pup::m::orbit3d(
		view_.e,
		view_.c,
		camera_alpha_,
		camera_beta_,
		camera_distance_
	);
	
	this->setup_blockfall(this->get_world_dir() / this->get_default_world());
	
	pup::gl1::rgb rgb(pup::gl1::color::from_hex("4d4d4d"));
	::glClearColor(rgb.r, rgb.b, rgb.g, 1.0f);
}

game_controller::~game_controller() throw()
{
}

void game_controller::setup_blockfall(const boost::filesystem::path& world)
{
#define PASSOVER_GM(BlockfallPtr, GameMode) \
	(BlockfallPtr.get()? BlockfallPtr->get_##GameMode(): false)
	
	blockfall_.reset(new blockfall(
		app_.get_timer(),
		boost::bind(&game_controller::gameover, this),
		boost::bind(&game_controller::sweepsnd, this),
		boost::bind(&game_controller::lvlupsnd, this),
		world,
		app_.get_ptree().get<bool>("general.rgcolorblind_match"),
		PASSOVER_GM(blockfall_, gm_bottom_mesh),
		PASSOVER_GM(blockfall_, gm_zero_g),
		PASSOVER_GM(blockfall_, gm_color_match),
		PASSOVER_GM(blockfall_, gm_one_shape_limit)
	));
	
	blockfall_->create_world();
	
#undef PASSOVER_GM
}

std::string game_controller::get_default_world()
{
	return app_.get_ptree().get<std::string>("general.default_world");
}

boost::filesystem::path game_controller::get_world_dir()
{
	return app_.get_ptree().get<std::string>("general.world_dir");
}

void game_controller::init()
{
	pup::controller::init();
	::SDL_ShowCursor(SDL_DISABLE);
	
	ng::power_list& powers = blockfall_->get_powers();
	for (auto it = powers.begin(); it != powers.end(); ++it) {
		(*it)->reset();
	}
}

void game_controller::think()
{
	blockfall_->update();
}

void game_controller::render()
{
	view_.clear();
	view_.look();
	
	blockfall_->render();
	this->render_hud();
}

void game_controller::render_hud()
{
	pup::gl1::d2::scoped_screen_coordinate_matrix ssc_matrix;

	int w_width;
	int w_height;

	app_.get_window_size(&w_width, &w_height);

	pup::gl1::ft::face_ptr& info_face = app_.get_typewriter().find_ptr("UbuntuMono-R", 14);
	
	info_face->set_color(pup::gl1::color::white);
	
	double step_sec = blockfall_->get_step_interval().remaining_sec();
	double spawn_sec = blockfall_->get_gm_one_shape_limit()?
		-1.0: blockfall_->get_spawn_interval().remaining_sec();
	
	info_face->print_2d(20, w_height - 20 - 16, boost::str(
		boost::format(
			"$ Info\n"
			"Level: %1%\n"
			"LinesCleared: %2%\n"
			"SkillPoints: %3%\n"
			"NextStep: %4$.1f sec\n"
			"NextSpawn: %5$.1f sec\n"
			"ShapeName: %6%\n"
			"ShapeLifts: %7%\n"
			"RotationMode: %8%\n"
		)
			% blockfall_->get_skill_level()
			% blockfall_->get_lines_cleared()
			% blockfall_->get_skill_ui_points()
			% step_sec
			% spawn_sec
			% blockfall_->get_active().get_name()
			% blockfall_->get_active().get_lifts()
			% blockfall_->get_rotation_mode_desc()
	));
	
	std::string pow("$ Powers\n");
	ng::power_list& powers = blockfall_->get_powers();
	for (auto it = powers.begin(); it != powers.end(); ++it) {
		pow.append((*it)->get_info() + "\n");
	}
	info_face->print_2d(20, w_height - 80 - 16 * 8, pow);
	
	std::string msg("$ Msgs\n");
	pup::string_list& msgs = blockfall_->get_skill_messages();
	for (auto it = msgs.begin(); it != msgs.end(); ++it) {
		msg.append(*it + "\n");
	}
	info_face->print_2d(20, 20 + 16 * (blockfall_->get_max_skill_messages()), msg);

	auto flash_msg = blockfall_->get_flash_message_iterator();
	if (flash_msg != blockfall_->get_flash_messages().end()) {
		pup::gl1::ft::face_ptr& flash_face = app_.get_typewriter().find_ptr("UbuntuMono-R", 32);
		pup::gl1::ft::print_2d(*flash_msg, pup::gl1::color::white, flash_face,
			0, w_height - 64, w_width, w_height - 64, pup::gl1::ft::ALIGN_CENTER);
	}
}

void game_controller::gameover()
{
	::SDL_ShowCursor(SDL_ENABLE);
	app_.get_jukebox().stop();
	app_.get_music().fadeout(1000);
	app_.queue_controller(pup::controller_ptr(
		new gameover_controller(app_, blockfall_->get_highscore())
	));
}

void game_controller::sweepsnd()
{
	app_.get_soundboard().play("sweep");
}

void game_controller::lvlupsnd()
{
	app_.get_soundboard().play("lvlup");
}

game_controller::camera_quadrant game_controller::get_camera_quadrant() const
{
	auto angle_fix = [](double v) -> double
	{
		v = std::fmod(v + M_PI / 4.0, 2.0 * M_PI);
		return v < 0.0? 2.0 * M_PI + v: v;
	};

	double a = angle_fix(camera_alpha_);

	if (a >= 0.0 && a < M_PI / 2.0)
		return CQ_0;
	else if (a >= M_PI / 2.0 && a < M_PI)
		return CQ_1;
	else if (a >= M_PI && a < 3.0 * M_PI / 2)
		return CQ_2;
	else // (a >= 3.0 * M_PI / 2 && a < 2.0 * M_PI)
		return CQ_3;
}

bool game_controller::key_camera_cw(::SDL_Event& event)
{
	camera_alpha_ += 0.1;
	pup::m::orbit3d(
		view_.e,
		view_.c,
		camera_alpha_,
		camera_beta_,
		camera_distance_
	);
	return false;
}

bool game_controller::key_camera_ccw(::SDL_Event& event)
{
	camera_alpha_ -= 0.1;
	pup::m::orbit3d(
		view_.e,
		view_.c,
		camera_alpha_,
		camera_beta_,
		camera_distance_
	);
	return false;
}

bool game_controller::key_camera_up(::SDL_Event& event)
{
	view_.c.y_d(+0.1);
	pup::m::orbit3d(
		view_.e,
		view_.c,
		camera_alpha_,
		camera_beta_,
		camera_distance_
	);
	return false;
}

bool game_controller::key_camera_down(::SDL_Event& event)
{
	view_.c.y_d(-0.1);
	pup::m::orbit3d(
		view_.e,
		view_.c,
		camera_alpha_,
		camera_beta_,
		camera_distance_
	);
	return false;
}

bool game_controller::key_camera_tiltup(::SDL_Event& event)
{
	if (camera_beta_ > -M_PI / 2 + 0.3) {
		camera_beta_ -= 0.1;
		pup::m::orbit3d(
			view_.e,
			view_.c,
			camera_alpha_,
			camera_beta_,
			camera_distance_
		);
	}
	return false;
}

bool game_controller::key_camera_tiltdown(::SDL_Event& event)
{
	if (camera_beta_ < +M_PI / 2 - 0.3) {
		camera_beta_ += 0.1;
		pup::m::orbit3d(
			view_.e,
			view_.c,
			camera_alpha_,
			camera_beta_,
			camera_distance_
		);
	}
	return false;
}

bool game_controller::key_camera_zoomin(::SDL_Event& event)
{
	if (camera_distance_ >= 2.0) {
		camera_distance_ -= 0.2;
		pup::m::orbit3d(
			view_.e,
			view_.c,
			camera_alpha_,
			camera_beta_,
			camera_distance_
		);
	}
	return false;
}

bool game_controller::key_camera_zoomout(::SDL_Event& event)
{
	if (camera_distance_ <= 16.0) {
		camera_distance_ += 0.2;
		pup::m::orbit3d(
			view_.e,
			view_.c,
			camera_alpha_,
			camera_beta_,
			camera_distance_
		);
	}
	return false;
}

bool game_controller::key_block_fall(::SDL_Event& event)
{
	app_.get_soundboard().play("drop");
	blockfall_->block_fall();
	return true;
}

bool game_controller::key_block_y_plus(::SDL_Event& event)
{
	if (blockfall_->move_shape(+0.0, +1.0, +0.0))
		app_.get_soundboard().play("move");
	return true;
}

bool game_controller::key_block_y_minus(::SDL_Event& event)
{
	if (blockfall_->get_active().get_lifts() != 0) {
		blockfall_->move_shape(+0.0, -1.0, +0.0);
		blockfall_->get_active().get_lifts()--;
		app_.get_soundboard().play("move");
	}
	return true;
}

bool game_controller::key_block_z_plus(::SDL_Event& event)
{
	bool moved = false;
	switch (this->get_camera_quadrant()) {
	case CQ_0: moved = blockfall_->move_shape(+0.0, +0.0, +1.0); break;
	case CQ_1: moved = blockfall_->move_shape(+1.0, +0.0, +0.0); break;
	case CQ_2: moved = blockfall_->move_shape(+0.0, +0.0, -1.0); break;
	case CQ_3: moved = blockfall_->move_shape(-1.0, +0.0, +0.0); break;
	}
	if (moved)
		app_.get_soundboard().play("move");
	return true;
}

bool game_controller::key_block_z_minus(::SDL_Event& event)
{
	bool moved = false;
	switch (this->get_camera_quadrant()) {
	case CQ_0: moved = blockfall_->move_shape(+0.0, +0.0, -1.0); break;
	case CQ_1: moved = blockfall_->move_shape(-1.0, +0.0, +0.0); break;
	case CQ_2: moved = blockfall_->move_shape(+0.0, +0.0, +1.0); break;
	case CQ_3: moved = blockfall_->move_shape(+1.0, +0.0, +0.0); break;
	}
	if (moved)
		app_.get_soundboard().play("move");
	return true;
}

bool game_controller::key_block_x_plus(::SDL_Event& event)
{
	bool moved = false;
	switch (this->get_camera_quadrant()) {
	case CQ_0: moved = blockfall_->move_shape(+1.0, +0.0, +0.0); break;
	case CQ_1: moved = blockfall_->move_shape(+0.0, +0.0, -1.0); break;
	case CQ_2: moved = blockfall_->move_shape(-1.0, +0.0, +0.0); break;
	case CQ_3: moved = blockfall_->move_shape(+0.0, +0.0, +1.0); break;
	}
	if (moved)
		app_.get_soundboard().play("move");
	return true;
}

bool game_controller::key_block_x_minus(::SDL_Event& event)
{
	bool moved = false;
	switch (this->get_camera_quadrant()) {
	case CQ_0: moved = blockfall_->move_shape(-1.0, +0.0, +0.0); break;
	case CQ_1: moved = blockfall_->move_shape(+0.0, +0.0, +1.0); break;
	case CQ_2: moved = blockfall_->move_shape(+1.0, +0.0, +0.0); break;
	case CQ_3: moved = blockfall_->move_shape(+0.0, +0.0, -1.0); break;
	}
	if (moved)
		app_.get_soundboard().play("move");
	return true;
}

bool game_controller::key_block_rotate_cw(::SDL_Event& event)
{
	app_.get_soundboard().play("rotate");
	blockfall_->rotate_shape_cw();
	return true;
}

bool game_controller::key_block_rotate_ccw(::SDL_Event& event)
{
	app_.get_soundboard().play("rotate");
	blockfall_->rotate_shape_ccw();
	return true;
}

bool game_controller::key_block_rotate_mode(::SDL_Event& event)
{
	blockfall_->change_rotation_mode();
	return true;
}

bool game_controller::key_block_next(::SDL_Event& event)
{
	if (blockfall_->next_active())
		app_.get_soundboard().play("switch");
	return true;
}

bool game_controller::key_block_prev(::SDL_Event& event)
{
	if (blockfall_->prev_active())
		app_.get_soundboard().play("switch");
	return true;
}

bool game_controller::key_power_kill_shape(::SDL_Event& event)
{
	if (blockfall_->get_kill_shape_power()->activate()) {
		return true;
	}
	return false;
}

bool game_controller::key_power_clear_color(::SDL_Event& event)
{
	if (blockfall_->get_clear_color_power()->activate()) {
		return true;
	}
	return false;
}

bool game_controller::key_power_vaporize_foundation(::SDL_Event& event)
{
	if (blockfall_->get_vaporize_foundation_power()->activate()) {
		return true;
	}
	return false;
}

bool game_controller::key_power_spike_trap(::SDL_Event& event)
{
	if (blockfall_->get_spike_trap_power()->activate()) {
		return true;
	}
	return false;
}

bool game_controller::key_power_gravity_beam(::SDL_Event& event)
{
	if (blockfall_->get_gravity_beam_power()->activate()) {
		return true;
	}
	return false;
}

bool game_controller::key_menu(::SDL_Event& event)
{
	::SDL_ShowCursor(SDL_ENABLE);
	app_.queue_controller(
		pup::controller_ptr(new pause_controller(app_, app_.get_current_controller()))
	);
	return true;
}

application::application(::Uint32 sdl_flags, const boost::program_options::variables_map& opt_vm) :
	pup::application(sdl_flags, opt_vm),
	db_("../resources/highscore.db3", SQLITE_OPEN_READWRITE)
{
	typewriter_.load("../resources/font/ubuntu/UbuntuMono-R.ttf", 10, pup::gl1::color::g, 1.00f);
	typewriter_.load("../resources/font/ubuntu/UbuntuMono-R.ttf", 12, pup::gl1::color::g, 1.00f);
	typewriter_.load("../resources/font/ubuntu/UbuntuMono-R.ttf", 14, pup::gl1::color::g, 0.86f);
	typewriter_.load("../resources/font/ubuntu/UbuntuMono-R.ttf", 16, pup::gl1::color::g, 1.00f);
	typewriter_.load("../resources/font/ubuntu/UbuntuMono-R.ttf", 32, pup::gl1::color::g, 1.00f);

	::GLfloat light_diffuse1[] = { +1.0f, +1.0f, +1.0f, +1.0f };
	::GLfloat light_ambient1[] = { +0.0f, +0.0f, +0.0f, +1.0f };
	::GLfloat light_specular1[] = { +1.0f, +1.0f, +1.0f, +1.0f };
	
	::GLfloat light_position1[] = { +0.0f, +2.0f, +2.0f, +1.0f };

	pup::gl1::place_light(
		GL_LIGHT0,
		light_diffuse1,
		light_ambient1,
		light_specular1,
		light_position1
	);
	
	this->update_db();

	std::printf(
		"\n"
		"FABL-ng - FAlling BLocks Next Generation\n"
		"Copyright 2015 (C) Erik Edlund <erik.edlund@32767.se>\n\n"
		
		"This program is distributed in the hope that it will be useful,\n"
		"but WITHOUT ANY WARRANTY; without even the implied warranty of\n"
		"MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the\n"
		"GNU General Public License version 3 for more details.\n\n"
	);
	
}

application::~application() throw()
{
}

void application::before_loop()
{
	pup::application::before_loop();
	
	this->get_jukebox().load_dir(this->get_ptree().get<std::string>("general.music_dir"));
	
	boost::filesystem::path sound_dir(this->get_ptree().get<std::string>("general.sound_dir"));
	
	pup::snd::soundboard& sounds = this->get_soundboard();
	
	sounds.load_file(sound_dir / "click.wav", "click");
	sounds.load_file(sound_dir / "drop.wav", "drop");
	sounds.load_file(sound_dir / "lvlup.wav", "lvlup");
	sounds.load_file(sound_dir / "move.wav", "move");
	sounds.load_file(sound_dir / "rotate.wav", "rotate");
	sounds.load_file(sound_dir / "sweep.wav", "sweep");
	sounds.load_file(sound_dir / "switch.wav", "switch");
}

void application::after_loop()
{
	pup::application::after_loop();
}

pup::controller_ptr application::get_start_controller()
{
	return pup::controller_ptr(new mainmenu_controller(*this));
}

void application::update_db()
{
	SQLite::Statement create_table(db_, ""
		"CREATE TABLE IF NOT EXISTS `highscore`(           "
		"    `id` INTEGER PRIMARY KEY,                     "
		"    `crc` UNSIGNED INT,                           "
		"    `fabl` UNSIGNED INT,                          "
		"    `created` DATE,                               "
		"    `world` TEXT,                                 "
		"    `name` VARCHAR(8),                            "
		"    `points` UNSIGNED BIG INT,                    "
		"    `steps` UNSIGNED BIG INT,                     "
		"    `levels` UNSIGNED INT,                        "
		"    `lines` UNSIGNED INT,                         "
		"    `shapes` UNSIGNED INT,                        "
		"    `powers` UNSIGNED INT,                        "
		"    `gm_bottom_mesh` BOOLEAN DEFAULT FALSE,       "
		"    `gm_zero_g` BOOLEAN DEFAULT FALSE,            "
		"    `gm_color_match` BOOLEAN DEFAULT FALSE,       "
		"    `gm_one_shape_limit` BOOLEAN DEFAULT FALSE    "
		")                                                 "
	);

	create_table.exec();

	SQLite::Statement select(db_, "SELECT * FROM `highscore` WHERE `fabl` <> ?");

	select.bind(1, FABL_VERSION);

	std::printf("\nChecking highscore database...\n");

	while (select.executeStep()) {
		int id = select.getColumn("id");

		std::printf("- updating highscore id=%d\n", id);

		highscore hs;

#ifdef PUP_WIN_MSC
#pragma warning(push)
#pragma warning(disable : 4800)
#endif
		
		hs.world = static_cast<const char*>(select.getColumn("world"));
		hs.gm_bottom_mesh = static_cast<int>(select.getColumn("gm_bottom_mesh"));
		hs.gm_zero_g = static_cast<int>(select.getColumn("gm_zero_g"));
		hs.gm_color_match = static_cast<int>(select.getColumn("gm_color_match"));
		hs.gm_one_shape_limit = static_cast<int>(select.getColumn("gm_one_shape_limit"));

#ifdef PUP_WIN_MSC
#pragma warning(pop)
#endif
		
		blockfall::checksum(hs);

		SQLite::Statement update(db_, ""
			"UPDATE `highscore`        "
			"SET `crc` = ?, `fabl` = ? "
			"WHERE `id` = ? LIMIT 1    "
		);

		update.bind(1, hs.crc);
		update.bind(2, FABL_VERSION);
		update.bind(3, id);

		update.exec();
	}

	std::printf("\n... done!\n\n");
}

namespace debug {

void print_world(const fabl::blockfall& blkfall)
{
	print_world(blkfall, std::cout);
}

void print_world(const fabl::blockfall& blkfall, std::ostream& out)
{
	pup::int_vector world;
	blkfall.simplify(world);

	const int xz_l = blkfall.get_xz();
	const int y_l = blkfall.get_y();

	for (int y = 0; y < y_l; ++y) {
		for (int z = 0; z < xz_l; ++z) {
			for (int x = 0; x < xz_l; ++x) {
				int v = world[pup::m::index_from_3d(x, y, z, xz_l, y_l, xz_l)];
				out << " " << std::setw(9) << std::setfill('0') << v;
			}
			out << std::endl;
		}
		out << std::endl;
	}
}

} // debug

} // fabl

