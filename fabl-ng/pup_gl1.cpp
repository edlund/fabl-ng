
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
namespace gl1 {

namespace color {

const rgb black(PUP_C3f_BLACK);
const rgb white(PUP_C3f_WHITE);

const rgb r(PUP_C3f_R);
const rgb g(PUP_C3f_G);
const rgb b(PUP_C3f_B);
const rgb y(PUP_C3f_Y);
const rgb c(PUP_C3f_C);
const rgb m(PUP_C3f_M);

const rgb& from_name(const std::string& name)
{
#define NAME_CK(Static, Candidate, Color) \
	if (Color == Candidate || #Static == Candidate) return Static;
	
	if (name == "black") return black;
	if (name == "white") return white;

	NAME_CK(r, name, "red")
	NAME_CK(g, name, "green")
	NAME_CK(b, name, "blue")
	NAME_CK(y, name, "yellow")
	NAME_CK(c, name, "cyan")
	NAME_CK(m, name, "magenta")

	PUP_ERR(std::logic_error, boost::str(boost::format("unknown color %1%") % name));

	// Never reached, gets rid of warnings.
	return black;

#undef NAME_CK
}

rgb from_hex(const std::string& hex)
{
	std::string num(!hex.empty() && hex[0] == '#'? hex.substr(1): hex);
	int rgb16 = stoi(num, nullptr, 16);
	rgb c(
		static_cast<float>((rgb16 >> 16) & 0xff) / 255.0f,
		static_cast<float>((rgb16 >> 8) & 0xff) / 255.0f,
		static_cast<float>(rgb16 & 0xff) / 255.0f
	);

	return c;
}

} // color

namespace d2 {

void rectangle::do_render()
{
	::glRecti(
		pos.x(),
		pos.y(),
		pos.x() + dim.w(),
		pos.y() + dim.h()
	);
}

} // d2

namespace d3 {

void cuboid::do_render()
{
	::glBegin(GL_QUADS);

	// Left
	::glNormal3d(+1.0f, +0.0f, +0.0f);
	::glVertex3d(+(dim.w() / 2) + pos.x(), +(dim.h() / 2) + pos.y(), +(dim.d() / 2) + pos.z());
	::glVertex3d(+(dim.w() / 2) + pos.x(), -(dim.h() / 2) + pos.y(), +(dim.d() / 2) + pos.z());
	::glVertex3d(+(dim.w() / 2) + pos.x(), -(dim.h() / 2) + pos.y(), -(dim.d() / 2) + pos.z());
	::glVertex3d(+(dim.w() / 2) + pos.x(), +(dim.h() / 2) + pos.y(), -(dim.d() / 2) + pos.z());

	// Right
	::glNormal3d(-1.0f, +0.0f, +0.0f);
	::glVertex3d(-(dim.w() / 2) + pos.x(), +(dim.h() / 2) + pos.y(), -(dim.d() / 2) + pos.z());
	::glVertex3d(-(dim.w() / 2) + pos.x(), -(dim.h() / 2) + pos.y(), -(dim.d() / 2) + pos.z());
	::glVertex3d(-(dim.w() / 2) + pos.x(), -(dim.h() / 2) + pos.y(), +(dim.d() / 2) + pos.z());
	::glVertex3d(-(dim.w() / 2) + pos.x(), +(dim.h() / 2) + pos.y(), +(dim.d() / 2) + pos.z());

	// Front
	::glNormal3d(+0.0f, +0.0f, -1.0f);
	::glVertex3d(+(dim.w() / 2) + pos.x(), +(dim.h() / 2) + pos.y(), -(dim.d() / 2) + pos.z());
	::glVertex3d(+(dim.w() / 2) + pos.x(), -(dim.h() / 2) + pos.y(), -(dim.d() / 2) + pos.z());
	::glVertex3d(-(dim.w() / 2) + pos.x(), -(dim.h() / 2) + pos.y(), -(dim.d() / 2) + pos.z());
	::glVertex3d(-(dim.w() / 2) + pos.x(), +(dim.h() / 2) + pos.y(), -(dim.d() / 2) + pos.z());

	// Back
	::glNormal3d(+0.0f, +0.0f, +1.0f);
	::glVertex3d(-(dim.w() / 2) + pos.x(), +(dim.h() / 2) + pos.y(), +(dim.d() / 2) + pos.z());
	::glVertex3d(-(dim.w() / 2) + pos.x(), -(dim.h() / 2) + pos.y(), +(dim.d() / 2) + pos.z());
	::glVertex3d(+(dim.w() / 2) + pos.x(), -(dim.h() / 2) + pos.y(), +(dim.d() / 2) + pos.z());
	::glVertex3d(+(dim.w() / 2) + pos.x(), +(dim.h() / 2) + pos.y(), +(dim.d() / 2) + pos.z());

	// Up
	::glNormal3d(+0.0f, +1.0f, +0.0f);
	::glVertex3d(-(dim.w() / 2) + pos.x(), +(dim.h() / 2) + pos.y(), -(dim.d() / 2) + pos.z());
	::glVertex3d(-(dim.w() / 2) + pos.x(), +(dim.h() / 2) + pos.y(), +(dim.d() / 2) + pos.z());
	::glVertex3d(+(dim.w() / 2) + pos.x(), +(dim.h() / 2) + pos.y(), +(dim.d() / 2) + pos.z());
	::glVertex3d(+(dim.w() / 2) + pos.x(), +(dim.h() / 2) + pos.y(), -(dim.d() / 2) + pos.z());

	// Down
	::glNormal3d(+0.0f, -1.0f, +0.0f);
	::glVertex3d(-(dim.w() / 2) + pos.x(), -(dim.h() / 2) + pos.y(), +(dim.d() / 2) + pos.z());
	::glVertex3d(-(dim.w() / 2) + pos.x(), -(dim.h() / 2) + pos.y(), -(dim.d() / 2) + pos.z());
	::glVertex3d(+(dim.w() / 2) + pos.x(), -(dim.h() / 2) + pos.y(), -(dim.d() / 2) + pos.z());
	::glVertex3d(+(dim.w() / 2) + pos.x(), -(dim.h() / 2) + pos.y(), +(dim.d() / 2) + pos.z());

	::glEnd();
}

void line::do_render()
{
	::glBegin(GL_LINES);

	::glNormal3d(+0.0f, +1.0f, +0.0f);
	::glVertex3d(pos0.x(), pos0.y(), pos0.z());
	::glVertex3d(pos1.x(), pos1.y(), pos1.z());

	::glEnd();
}

void pyramid::do_render()
{
	::glRotated(+a, +dir.x(), +dir.y(), +dir.z());
	::glBegin(GL_TRIANGLES);
	
	// Front
	::glNormal3d(+0.0f, +1.0f, +1.0f);
	::glVertex3d(+pos.x(), +pos.y(), +pos.z()); // Top
	::glVertex3d(pos.x() - szo, pos.y() - sza, pos.z() + szo);
	::glVertex3d(pos.x() + szo, pos.y() - sza, pos.z() + szo);

	// Right
	::glNormal3d(+1.0f, +1.0f, +0.0f);
	::glVertex3d(pos.x(), pos.y(), pos.z()); // Top
	::glVertex3d(pos.x() + szo, pos.y() - sza, pos.z() + szo);
	::glVertex3d(pos.x() + szo, pos.y() - sza, pos.z() - szo);

	// Back
	::glNormal3d(+0.0f, +1.0f, -1.0f);
	::glVertex3d(pos.x(), pos.y(), pos.z()); // Top
	::glVertex3d(pos.x() + szo, pos.y() - sza, pos.z() - szo);
	::glVertex3d(pos.x() - szo, pos.y() - sza, pos.z() - szo);

	// Left
	::glNormal3d(-1.0f, +1.0f, +0.0f);
	::glVertex3d(pos.x(), pos.y(), pos.z()); // Top
	::glVertex3d(pos.x() - szo, pos.y() - sza, pos.z() - szo);
	::glVertex3d(pos.x() - szo, pos.y() - sza, pos.z() + szo);

	// Bottom 1
	::glNormal3d(+0.0f, -1.0f, +0.0f);
	::glVertex3d(pos.x() - szo, pos.y() - sza, pos.z() + szo);
	::glVertex3d(pos.x() - szo, pos.y() - sza, pos.z() - szo);
	::glVertex3d(pos.x() + szo, pos.y() - sza, pos.z() + szo);
	// Bottom 2
	::glNormal3d(+0.0f, -1.0f, +0.0f);
	::glVertex3d(pos.x() + szo, pos.y() - sza, pos.z() - szo);
	::glVertex3d(pos.x() + szo, pos.y() - sza, pos.z() + szo);
	::glVertex3d(pos.x() - szo, pos.y() - sza, pos.z() - szo);

	::glEnd();
}

} // d3

namespace ft {

face::face(
	typewriter& tw,
	const boost::filesystem::path& path,
	int size,
	const rgb& c,
	::GLfloat d
) :
	first_char_(32),
	last_char_(127),
	color_(c),
	textures_(new ::GLuint[128]),
	lists_(::glGenLists(128)),
	size_(static_cast<::GLfloat>(size)),
	divisor_(d),
	dims_(new d2::size[128])
{
	::FT_Face f;
	
	if (::FT_New_Face(tw.get_lib(), path.string().c_str(), 0, &f) != 0)
		PUP_ERR(std::runtime_error, "failed to load font face");

	::FT_Set_Char_Size(f, size << 6, size << 6, 96, 96);
	::glGenTextures(128, textures_);

	for (unsigned char ch = first_char_; ch <= last_char_; ++ch)
		this->load_char(f, ch);
	
	::FT_Done_Face(f);
}

face::~face() throw()
{
	glDeleteLists(lists_, 128);
	glDeleteTextures(128, textures_);

	delete[] dims_;
	delete[] textures_;
}

void face::print_2d(int x, int y, const std::string& text, const rgb& col)
{
	//::GLuint font = lists_;
	::GLfloat size = this->line_height();

	string_vector lines;

	boost::split(lines, text, boost::is_any_of("\n"));

	::glPushAttrib(GL_LIST_BIT
		| GL_CURRENT_BIT
		| GL_ENABLE_BIT
		| GL_TRANSFORM_BIT
	);

	::glMatrixMode(GL_MODELVIEW);
	::glDisable(GL_LIGHTING);
	::glEnable(GL_TEXTURE_2D);
	::glDisable(GL_DEPTH_TEST);
	::glEnable(GL_BLEND);
	::glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	::glListBase(lists_);

	::GLfloat modelview_matrix[16];
	::glGetFloatv(GL_MODELVIEW_MATRIX, modelview_matrix);

	::glColor3f(col.r, col.g, col.b);

	for (auto it = lines.begin(); it != lines.end(); ++it) {
		int i = it - lines.begin();

		::glPushMatrix();

		::glLoadIdentity();
		::glTranslatef(
			static_cast<::GLfloat>(x),
			static_cast<::GLfloat>(y - size * i),
			0.0f
		);
		::glMultMatrixf(modelview_matrix);

		::glCallLists(it->length(), GL_UNSIGNED_BYTE, it->c_str());
		
		::glPopMatrix();
	}

	::glPopAttrib();
}

void face::print_2d(int x, int y, const std::string& text)
{
	this->print_2d(x, y, text, color_);
}

::GLuint face::text_width(const std::string& text)
{
	::GLuint width = 0;
	::GLuint length = text.length();
	
	for (::GLuint i = 0; i < length; ++i)
		width += dims_[text[i]].w();
	return width;
}

::GLfloat face::text_height(const std::string& text)
{
	return this->get_sizef() + this->line_height()
		* std::count(text.begin(), text.end(), '\n');
}

::GLfloat face::line_height()
{
	return this->get_sizef() / divisor_;
}

void face::load_char(::FT_Face f, unsigned char ch)
{
	if (::FT_Load_Glyph(f, ::FT_Get_Char_Index(f, ch), FT_LOAD_DEFAULT) != 0)
		PUP_ERR(std::runtime_error, boost::str(boost::format("failed to load character '%1%'") % ch));

	::FT_Glyph glyph;
	
	if (::FT_Get_Glyph(f->glyph, &glyph) != 0)
		PUP_ERR(std::runtime_error, "failed to get glyph");
	
	::FT_Glyph_To_Bitmap(&glyph, FT_RENDER_MODE_NORMAL, 0, 1);

	::FT_BitmapGlyph bitmap_glyph = reinterpret_cast<FT_BitmapGlyph>(glyph);
	::FT_Bitmap& bitmap = bitmap_glyph->bitmap;

	auto next_ilog2 = [](int i) -> int
	{
		int log2 = 1;
		while (log2 < i)
			log2 <<= 1;
		return log2;
	};

#ifdef PUP_WIN
typedef int load_char_size_type;
#else
typedef std::size_t load_char_size_type;
#endif

	load_char_size_type width = next_ilog2(bitmap.width);
	load_char_size_type height = next_ilog2(bitmap.rows);

	::GLubyte* expanded_data = new ::GLubyte[2 * width * height];

	for (load_char_size_type j = 0; j < height; ++j) {
		for (load_char_size_type i = 0; i < width; ++i) {
			expanded_data[2 * (i + j * width)]
				= expanded_data[2 * (i + j * width) + 1]
				= (i >= bitmap.width || j >= bitmap.rows)?
					0 : bitmap.buffer[i + bitmap.width * j];
		}
	}

	::glBindTexture(GL_TEXTURE_2D, textures_[ch]);
	::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	::glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height,
		0, GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE, expanded_data);

	delete[] expanded_data;

	::glNewList(lists_ + ch, GL_COMPILE);
	::glBindTexture(GL_TEXTURE_2D, textures_[ch]);
	
	::glPushMatrix();
		::glTranslatef(static_cast<::GLfloat>(bitmap_glyph->left), 0, 0);
		::glTranslatef(0, static_cast<::GLfloat>(
			static_cast<int>(bitmap_glyph->top) - static_cast<int>(bitmap.rows)
		), 0);

		::GLfloat x = static_cast<::GLfloat>(bitmap.width) / static_cast<::GLfloat>(width);
		::GLfloat y = static_cast<::GLfloat>(bitmap.rows) / static_cast<::GLfloat>(height);

		::glBegin(GL_QUADS);
			::glTexCoord2d(0, 0);
			::glVertex2f(0, static_cast<::GLfloat>(bitmap.rows));

			::glTexCoord2d(0, y);
			::glVertex2f(0, 0);

			::glTexCoord2d(x, y);
			::glVertex2f(static_cast<::GLfloat>(bitmap.width), 0);

			::glTexCoord2d(x, 0);
			::glVertex2f(static_cast<::GLfloat>(bitmap.width), static_cast<::GLfloat>(bitmap.rows));
		::glEnd();
	::glPopMatrix();

	::glTranslatef(static_cast<::GLfloat>(f->glyph->advance.x >> 6), 0, 0);

	dims_[ch].wh(
		f->glyph->advance.x >> 6,
		f->glyph->metrics.height >> 6
	);

	::glEndList();
}

typewriter::typewriter()
{
	if (::FT_Init_FreeType(&lib_) != 0)
		PUP_ERR(std::runtime_error, "failed to init ft library");
}

typewriter::~typewriter() throw()
{
	faces_.clear();

	::FT_Done_FreeType(lib_);
}

void typewriter::load(const boost::filesystem::path& path,
	int size, const rgb& c, ::GLfloat d)
{
	faces_.insert(face_map::value_type(boost::filesystem::basename(path)
		.append(std::to_string(size)), face_ptr(new face(*this, path, size, c, d))));
}

face_ptr& typewriter::find_ptr(std::string name, int size)
{
	std::string id(name);
	auto it = faces_.find(id.append(std::to_string(size)));
	if (it == faces_.end()) {
		PUP_ERR(std::runtime_error, boost::str(boost::format(
			"face \"%1%\" with size %2% not found") % name % size));
	}
	return it->second;
}

face& typewriter::find(std::string name, int size)
{
	return *this->find_ptr(name, size);
}

void print_2d(const std::string& text, const rgb& col, face_ptr& face,
	int origo_x, int origo_y, int w, int h, text_align align)
{
	int x;
	int y = origo_y;
	
	string_vector lines;
	boost::split(lines, text, boost::is_any_of("\n"));

	for (auto line = lines.begin(); line != lines.end(); ++line) {
		switch (align) {
		case ft::ALIGN_LEFT:
			x = origo_x;
			break;
		case ft::ALIGN_RIGHT:
			x = origo_x + w - face->text_width(*line);
			break;
		case ft::ALIGN_CENTER:
			x = origo_x + w / 2 - face->text_width(*line) / 2;
			break;
		}

		face->print_2d(x, y, *line, col);

		y -= static_cast<int>(face->line_height());
	}
}

} // ft

namespace ui {

element::element(form& owner, const std::string& name) :
	owner_(owner),
	parent_(nullptr),
	name_(name),
	relative_pos_(0, 0),
	after_(0),
	padding_(0)
{
}

element::~element() throw()
{
}

d2::point element::screen_coordinate()
{
	return this->as_screen_coordinate(this->get_relative_pos());
}

d2::point element::as_screen_coordinate(const d2::point& pos)
{
	const d2::point& form(this->get_owner().get_form_pos());
	const d2::size& winsz(this->get_owner().get_window_size());
	return d2::point(
		form.x() + pos.x(),
		winsz.h() - form.y() - pos.y()
	);
}

bool test_hover(element& el, const ::SDL_Event& event)
{
	const int x = event.button.x;
	const int y = event.button.y;

	const d2::point& relative_pos(el.get_relative_pos());
	const d2::size border_size(
		el.get_width(),
		el.get_height()
	);

	if (
		(event.type == SDL_MOUSEMOTION || event.type == SDL_MOUSEBUTTONDOWN) &&
		(x >= relative_pos.x() && x <= relative_pos.x() + border_size.w()) &&
		(y >= relative_pos.y() && y <= relative_pos.y() + border_size.h())
	) {
		return true;
	}
	return false;
}

bool test_clicked(element& el, const::SDL_Event& event, const ::Uint8 button)
{
	if (
		event.type == SDL_MOUSEBUTTONDOWN &&
		event.button.button == button &&
		test_hover(el, event)
	) {
		return true;
	}
	return false;
}

void expander::render()
{
	border_box_.render();
	content_box_.render();
}

void expander::finalize()
{
	border_box_.pos = this->border_screen_coordinate();
	content_box_.pos = this->content_screen_coordinate();
}

void expander::resize()
{
	this->set_width(border_box_.dim.w());
	this->set_height(border_box_.dim.h());
}

d2::point expander::border_screen_coordinate()
{
	d2::point p(this->screen_coordinate());
	p.y_d(-this->get_border_size().h());
	return p;
}

d2::point expander::content_screen_coordinate()
{
	d2::point p(this->screen_coordinate());
	p.x_d(+(border_));
	p.y_d(-(this->get_content_size().h() + border_));
	return p;
}

collection::collection(form& owner, const std::string& name) :
	expander(owner, name)
{
}

collection::~collection() throw()
{
}

bool collection::react(::SDL_Event& event)
{
	for (auto it = elements_.begin(); it != elements_.end(); ++it) {
		if ((*it)->react(event))
			return true;
	}
	return false;
}

void collection::think()
{
	for (auto it = elements_.begin(); it != elements_.end(); ++it)
		(*it)->think();
}

void collection::render()
{
	expander::render();

	for (auto it = elements_.begin(); it != elements_.end(); ++it) {
		(*it)->render();
	}
}

void collection::finalize()
{
	expander::finalize();

	d2::point r(0, 0);
	d2::point b(this->get_relative_pos());

	int h = 0;
	int x_base = b.x() + border_ + padding_;
	int row_width = 0;

	r.x(x_base);
	r.y(b.y() + border_ + padding_);
	
	for (auto it = elements_.begin(); it != elements_.end(); ++it) {
		(*it)->set_relative_pos(r);
		(*it)->finalize();
		
		r.x_d((*it)->get_width());
		h = std::max(h, (*it)->get_height() + (*it)->get_after());
		row_width += (*it)->get_width();

		if (row_width >= this->get_inner_size().w()) {
			r.x(x_base);
			r.y_d(h);
			
			h = 0;
			row_width = 0;
		}
	}
}

void collection::push_front(element& el)
{
	el.set_parent(this);
	elements_.push_front(&el);
}

void collection::push_back(element& el)
{
	el.set_parent(this);
	elements_.push_back(&el);
}

bool collection::remove(element& el)
{
	auto it = std::find(elements_.begin(), elements_.end(), &el);
	if (it != elements_.end()) {
		(*it)->set_parent(nullptr);
		elements_.remove(&el);
		return true;
	}
	return false;
}

widget::widget(form& owner, const std::string& name) :
	element(owner, name),
	hover_(false)
{
	after_ = 10;
}

widget::~widget() throw()
{
}

bool widget::react(::SDL_Event& event)
{
	if (event.type == SDL_MOUSEMOTION || event.type == SDL_MOUSEBUTTONDOWN)
		hover_ = test_hover(*this, event);
	return false;
}

void widget::finalize()
{
	int mouse_x;
	int mouse_y;
	
	::SDL_GetMouseState(&mouse_x, &mouse_y);
	::SDL_Event dummy_event;
	std::memset(&dummy_event, 0, sizeof(::SDL_Event));
	
	dummy_event.type = SDL_MOUSEMOTION;
	dummy_event.button.x = mouse_x;
	dummy_event.button.y = mouse_y;
	
	hover_ = test_hover(*this, dummy_event);
}

int widget::get_width()
{
	collection* c = dynamic_cast<collection*>(this->get_parent());
	return c? c->get_inner_width(): 0;
}

bool widget::validate()
{
	for (auto it = validators_.begin(); it != validators_.end(); ++it) {
		if (!it->second(this->string_value()))
			return false;
	}
	return true;
}

void widget::add_validator(const std::string& name, const validator_function& v_fn)
{
	validators_.insert(validator_map::value_type(name, v_fn));
}

void widget::remove_validator(const std::string& name)
{
	auto it = validators_.find(name);
	if (it != validators_.end())
		validators_.erase(it);
}

text::text(
	form& owner,
	const std::string& name,
	const ft::face_ptr& face,
	const rgb& col,
	const ft::text_align& align
) :
	widget(owner, name),
	face_(face),
	col_(col),
	align_(align)
{
	after_ = 10;
}

text::~text() throw()
{
}

bool text::react(::SDL_Event& event)
{
	return false;
}

void text::think()
{
}

void text::render()
{
	ft::print_2d(
		label_,
		col_,
		face_,
		label_pos_.x() + padding_,
		label_pos_.y() + padding_,
		this->get_width() - padding_,
		this->get_height() - padding_,
		align_
	);
}

void text::finalize()
{
	d2::point b(this->get_relative_pos());
	b.y_d(+(face_->get_sizei() + padding_));
	label_pos_ = this->as_screen_coordinate(b);
	
	widget::finalize();
}

int text::get_height()
{
	return static_cast<int>(face_->text_height(label_)) + 2 * padding_;
}

input::input(form& owner, const std::string& name) :
	widget(owner, name)
{
	padding_ = 4;
}

input::~input() throw()
{
}

bool input::react(::SDL_Event& event)
{
	widget::react(event);

	if (test_clicked(*this, event, SDL_BUTTON_LEFT)) {
		this->get_owner().set_focused(this);
		return true;
	} else if (event.type == SDL_KEYDOWN && this->get_owner().is_focused(this)) {
		
		// FIXME! This is stupid, but it saves time for the time being,
		// obviously SDL_TEXTINPUT should be used.

		switch (event.key.keysym.sym) {
			
#define KEY_HACK_SPEC(Key, Char) \
	case SDLK_##Key: value_.append(Char); return true; break
#define KEY_HACK(Key) \
	KEY_HACK_SPEC(Key, #Key)
			
			KEY_HACK(a);
			KEY_HACK(b);
			KEY_HACK(c);
			KEY_HACK(d);
			KEY_HACK(e);
			KEY_HACK(f);
			KEY_HACK(g);
			KEY_HACK(h);
			KEY_HACK(i);
			KEY_HACK(j);
			KEY_HACK(k);
			KEY_HACK(l);
			KEY_HACK(m);
			KEY_HACK(n);
			KEY_HACK(o);
			KEY_HACK(p);
			KEY_HACK(q);
			KEY_HACK(r);
			KEY_HACK(s);
			KEY_HACK(t);
			KEY_HACK(u);
			KEY_HACK(v);
			KEY_HACK(w);
			KEY_HACK(x);
			KEY_HACK(y);
			KEY_HACK(z);

			KEY_HACK(0);
			KEY_HACK(1);
			KEY_HACK(2);
			KEY_HACK(3);
			KEY_HACK(4);
			KEY_HACK(5);
			KEY_HACK(6);
			KEY_HACK(7);
			KEY_HACK(8);
			KEY_HACK(9);

			KEY_HACK_SPEC(SPACE, " ");
			KEY_HACK_SPEC(PERIOD, ".");
			KEY_HACK_SPEC(EXCLAIM, "!");
			KEY_HACK_SPEC(QUOTE, "'");
			KEY_HACK_SPEC(QUOTEDBL, "\"");
			KEY_HACK_SPEC(PERCENT, "%");
			KEY_HACK_SPEC(DOLLAR, "$");
			KEY_HACK_SPEC(AMPERSAND, "&");
			KEY_HACK_SPEC(LEFTPAREN, "(");
			KEY_HACK_SPEC(RIGHTPAREN, ")");
			KEY_HACK_SPEC(ASTERISK, "*");
			KEY_HACK_SPEC(PLUS, "+");
			KEY_HACK_SPEC(MINUS, "-");
			KEY_HACK_SPEC(COMMA, ",");
			KEY_HACK_SPEC(SLASH, "/");
			KEY_HACK_SPEC(COLON, ":");
			KEY_HACK_SPEC(SEMICOLON, ";");
			KEY_HACK_SPEC(LESS, "<");
			KEY_HACK_SPEC(EQUALS, "=");
			KEY_HACK_SPEC(GREATER, ">");
			KEY_HACK_SPEC(QUESTION, "?");
			KEY_HACK_SPEC(AT, "@");
			
			KEY_HACK_SPEC(KP_DIVIDE, "/");
			KEY_HACK_SPEC(KP_MULTIPLY, "*");
			KEY_HACK_SPEC(KP_MINUS, "-");
			KEY_HACK_SPEC(KP_PLUS, "+");
			KEY_HACK_SPEC(KP_0, "0");
			KEY_HACK_SPEC(KP_1, "1");
			KEY_HACK_SPEC(KP_2, "2");
			KEY_HACK_SPEC(KP_3, "3");
			KEY_HACK_SPEC(KP_4, "4");
			KEY_HACK_SPEC(KP_5, "5");
			KEY_HACK_SPEC(KP_6, "6");
			KEY_HACK_SPEC(KP_7, "7");
			KEY_HACK_SPEC(KP_8, "8");
			KEY_HACK_SPEC(KP_9, "9");
			KEY_HACK_SPEC(KP_PERIOD, ".");
			KEY_HACK_SPEC(KP_EQUALS, "=");
			
			case SDLK_BACKSPACE:
				if (value_.size())
					value_.pop_back();
				return true;
			break;

#undef KEY_HACK
#undef KEY_HACK_SPEC
			
		}
	}
	return false;
}

void input::think()
{
	bool focused = this->get_owner().is_focused(this);
	bool valid = this->validate();
	
	std::string delta(focused? "focus": "zero");
	
	shadow_box_.col = this->get_owner().get_theme().get("antishadow_box", "zero");
	value_box_.col = valid?
		this->get_owner().get_theme().get("norm_box", delta):
		this->get_owner().get_theme().get("err_box", delta);
	
	append_ = focused && static_cast<int>(
		this->get_owner().get_controller().get_app().get_timer().total_sec()
	) % 2 == 0? '_': ' ';
}

void input::render()
{
	shadow_box_.render();
	value_box_.render();

	this->get_owner().get_theme().get_label_face()->print_2d(
		label_pos_.x(),
		label_pos_.y(),
		label_,
		this->get_owner().get_theme().get("value")
	);

	std::string value_fix(value_ + append_);
	
	this->get_owner().get_theme().get_value_face()->print_2d(
		value_pos_.x(),
		value_pos_.y(),
		value_fix,
		this->get_owner().get_theme().get("value")
	);
}

void input::finalize()
{
	d2::point b(this->get_relative_pos());

	int label_size = this->get_owner().get_theme().get_label_face()->get_sizei();

	d2::point label_relative_pos(
		b.x() + padding_,
		b.y() + label_size + padding_
	);
	label_pos_ = this->as_screen_coordinate(label_relative_pos);

	d2::point value_relative_pos(
		b.x() + padding_,
		b.y() + this->get_height() - 2 * padding_
	);
	value_pos_ = this->as_screen_coordinate(value_relative_pos);
	
	value_box_.dim.wh(this->get_width(), this->get_height());
	shadow_box_.dim.wh(this->get_width(), this->get_height());

	d2::point value_box_relative_pos(
		b.x(),
		b.y() + this->get_height()
	);
	value_box_.pos = this->as_screen_coordinate(value_box_relative_pos);

	d2::point shadow_box_relative_pos(
		b.x() - 1,
		b.y() + this->get_height() - 1
	);
	shadow_box_.pos = this->as_screen_coordinate(shadow_box_relative_pos);
	
	widget::finalize();
}

int input::get_height()
{
	int label_size = this->get_owner().get_theme().get_label_face()->get_sizei();
	int value_size = this->get_owner().get_theme().get_value_face()->get_sizei();
	return 5 * padding_ + label_size + value_size;
}

keypress::keypress(form& owner, const std::string& name) :
	input(owner, name)
{
}

keypress::~keypress() throw()
{
}

bool keypress::react(::SDL_Event& event)
{
	widget::react(event);

	if (test_clicked(*this, event, SDL_BUTTON_LEFT)) {
		this->get_owner().set_focused(this);
		return true;
	} else if (event.type == SDL_KEYDOWN && this->get_owner().is_focused(this)) {
		for (auto it = global::keycodes.begin(); it != global::keycodes.end(); ++it) {
			if (it->second == event.key.keysym.sym) {
				value_ = it->first;
				return true;
			}
		}
	}
	return false;
}

button::button(form& owner, const std::string& name, const click_function& action) :
	widget(owner, name),
	action_(action)
{
	padding_ = 8;
}

button::~button() throw()
{
}

bool button::react(::SDL_Event& event)
{
	widget::react(event);

	if (test_clicked(*this, event, SDL_BUTTON_LEFT)) {
		this->get_owner().set_focused(this);
		action_(event);
		return true;
	}
	return false;
}

void button::think()
{
	std::string delta(hover_? "hover": "zero");
	label_box_.col = this->get_owner().get_theme().get("norm_box", delta);
	shadow_box_.col = this->get_owner().get_theme().get("shadow_box", delta);
}

void button::render()
{
	shadow_box_.render();
	label_box_.render();
	
	ft::print_2d(
		label_,
		this->get_owner().get_theme().get("button"),
		this->get_owner().get_theme().get_button_face(),
		label_pos_.x(),
		label_pos_.y(),
		this->get_width() - 2 * padding_,
		this->get_height() - 2 * padding_,
		ft::ALIGN_CENTER
	);
}

void button::finalize()
{
	d2::point b(this->get_relative_pos());

	d2::point label_relative_pos(
		b.x() + padding_,
		b.y() + this->get_height() - padding_
	);
	label_pos_ = this->as_screen_coordinate(label_relative_pos);

	label_box_.dim.wh(this->get_width(), this->get_height());
	shadow_box_.dim.wh(this->get_width(), this->get_height());

	d2::point label_box_relative_pos(
		b.x(),
		b.y() + this->get_height()
	);
	label_box_.pos = this->as_screen_coordinate(label_box_relative_pos);

	d2::point shadow_box_relative_pos(
		b.x() + 2,
		b.y() + this->get_height() + 2
	);
	shadow_box_.pos = this->as_screen_coordinate(shadow_box_relative_pos);
	
	widget::finalize();
}

int button::get_height()
{
	return this->get_owner().get_theme().get_button_face()->get_sizei()
		+ 2 * padding_;
}

checkbox::checkbox(form& owner, const std::string& name) :
	widget(owner, name),
	checked_(false)
{
	after_ = 20;
}

checkbox::~checkbox() throw()
{
}

bool checkbox::react(::SDL_Event& event)
{
	widget::react(event);

	if (test_clicked(*this, event, SDL_BUTTON_LEFT)) {
		this->get_owner().set_focused(this);
		checked_ = !checked_;
		return true;
	}
	return false;
}

void checkbox::think()
{
}

void checkbox::render()
{
	std::string box((checked_? "[X] ": "[ ] ") + label_);
	this->get_owner().get_theme().get_value_face()->print_2d(
		label_pos_.x() + padding_,
		label_pos_.y() + padding_,
		box,
		this->get_owner().get_theme().get("label")
	);
}

void checkbox::finalize()
{
	d2::point b(this->get_relative_pos());
	b.y_d(+(this->get_owner().get_theme().get_value_face()->get_sizei() + padding_));
	label_pos_ = this->as_screen_coordinate(b);
	
	widget::finalize();
}

int checkbox::get_height()
{
	return this->get_owner().get_theme().get_value_face()->get_sizei() + 2 * padding_;
}

theme::theme(
	ft::typewriter& typewriter,

	const rgb& label,
	const rgb& value,
	const rgb& button,
	const rgb& norm_box,
	const rgb& err_box,
	const rgb& bg_box,
	const rgb& shadow_box,
	const rgb& antishadow_box,

	const rgb& focus_delta,
	const rgb& hover_delta
) :
	typewriter_(typewriter),

	label_(label),
	value_(value),
	button_(button),
	norm_box_(norm_box),
	err_box_(err_box),
	bg_box_(bg_box),
	shadow_box_(shadow_box),
	antishadow_box_(antishadow_box),

	zero_delta_(0.0f, 0.0f, 0.0f),
	focus_delta_(focus_delta),
	hover_delta_(hover_delta)
{
#define COLOR_MAP_INSERT_SPEC(Map, Name, Function) \
	Map.insert(color_map::value_type(#Name, boost::bind(&theme::get_##Function, this)))
#define COLOR_MAP_INSERT(Map, Name) \
	COLOR_MAP_INSERT_SPEC(Map, Name, Name)

	COLOR_MAP_INSERT(colors_, label);
	COLOR_MAP_INSERT(colors_, value);
	COLOR_MAP_INSERT(colors_, button);
	COLOR_MAP_INSERT(colors_, norm_box);
	COLOR_MAP_INSERT(colors_, err_box);
	COLOR_MAP_INSERT(colors_, bg_box);
	COLOR_MAP_INSERT(colors_, shadow_box);
	COLOR_MAP_INSERT(colors_, antishadow_box);

	COLOR_MAP_INSERT_SPEC(deltas_, zero, zero_delta);
	COLOR_MAP_INSERT_SPEC(deltas_, focus, focus_delta);
	COLOR_MAP_INSERT_SPEC(deltas_, hover, hover_delta);

#undef COLOR_MAP_INSERT
#undef COLOR_MAP_INSERT_SPEC
}

theme::~theme() throw()
{
}

rgb theme::get(const std::string& name, const std::string& delta) const
{
	auto cit = colors_.find(name);
	if (cit == colors_.end()) {
		PUP_ERR(std::runtime_error, "unknown color requested");
	}

	auto dit = deltas_.find(delta);
	if (dit == deltas_.end()) {
		PUP_ERR(std::runtime_error, "unknown delta requested");
	}
	
	const rgb& c((cit->second)());
	const rgb& d((dit->second)());
	
	return rgb(
		c.r + d.r,
		c.g + d.g,
		c.b + d.b
	);
}

form::form(controller& ctrlr, theme& thm) :
	ctrlr_(ctrlr),
	theme_(thm),
	elements_(*this, "root")
{
	int w;
	int h;

	ctrlr_.get_app().get_window_size(&w, &h);

	form_pos_.xy(0, 0);
	form_size_.wh(w, h);
	window_size_.wh(w, h);
	elements_.set_width(w);
	elements_.set_height(h);
}

bool form::react(::SDL_Event& event)
{
	return elements_.react(event);
}

void form::think()
{
	elements_.think();
}

void form::render()
{
	scoped_disable_lighting disable_lighting;
	d2::scoped_screen_coordinate_matrix ssc_matrix;
	elements_.render();
}

void form::finalize()
{
	this->get_collection().finalize();
}

bool form::validate()
{
	return this->validate_collection(&elements_);
}

bool form::validate_collection(const collection* elements)
{
	for (auto it = elements->begin(); it != elements->end(); ++it) {
		collection* c = dynamic_cast<collection*>(*it);
		widget* w = dynamic_cast<widget*>(*it);
		if (c && !this->validate_collection(c))
			return false;
		if (w && !w->validate())
			return false;
	}
	return true;
}

element* static_focus_form::focused = nullptr;

} // ui

} // gl1
} // pup
