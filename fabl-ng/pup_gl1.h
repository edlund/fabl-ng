
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

#ifndef LIBPUP_PUP_GL1_H
#define LIBPUP_PUP_GL1_H

#include "pup_env.h"
#include "pup_core.h"

#include "pup_m.h"
#include "pup_app.h"

namespace pup {
namespace gl1 {

template <typename T>
struct basic_rgb
{
	inline basic_rgb() :
		r(0),
		g(0),
		b(0)
	{}

	template <typename U>
	explicit inline basic_rgb(const U& rv, const U& gv, const U&bv) :
		r(static_cast<T>(rv)),
		g(static_cast<T>(gv)),
		b(static_cast<T>(bv))
	{}

	template <typename U>
	inline void set(const U& rv, const U& gv, const U& bv)
	{
		r = static_cast<T>(rv);
		g = static_cast<T>(gv);
		b = static_cast<T>(bv);
	}

	template <typename U>
	inline void set(const basic_rgb<U>& rhs)
	{
		this->set(rhs.r, rhs.g, rhs.b);
	}

	template <typename U>
	inline bool operator==(const basic_rgb<U>& rhs)
	{
		return (
			r == static_cast<T>(rhs.r) &&
			g == static_cast<T>(rhs.g) &&
			b == static_cast<T>(rhs.b)
		);
	}

	template <typename U>
	inline bool operator!=(const basic_rgb<U>& rhs)
	{
		return !(*this == rhs);
	}

	T r;
	T g;
	T b;
};

typedef basic_rgb<::GLfloat> rgb;

namespace color {

extern const rgb black;
extern const rgb white;

extern const rgb r;
extern const rgb g;
extern const rgb b;
extern const rgb y;
extern const rgb c;
extern const rgb m;

const rgb& from_name(const std::string& name);

rgb from_hex(const std::string& hex);

} // color

enum wireframe {
	WF_NONE,
	WF_NORM,
	WF_REAL
};

struct drawable
{
	explicit drawable(
		const rgb& cv = rgb(PUP_C3f_WHITE)
	) :
		col(cv),
		lw(1.0),
		wf(WF_NONE)
	{}

	virtual ~drawable() throw()
	{}

	virtual void render()
	{
		this->render(1.0f);
	}

	virtual void render(const ::GLfloat alpha)
	{
		::glColor4f(col.r, col.g, col.b, alpha);

		if (wf != WF_NONE) {
			if (wf == WF_REAL) ::glDisable(GL_CULL_FACE);
			::glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		}

		::GLfloat slw;

		::glGetFloatv(GL_LINE_WIDTH, &slw);
		::glLineWidth(lw);
		::glPushMatrix();
			this->do_render();
		::glPopMatrix();
		::glLineWidth(slw);

		if (wf != WF_NONE) {
			::glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			if (wf == WF_REAL) ::glEnable(GL_CULL_FACE);
		}

	}

	virtual void do_render() = 0;

	rgb col; // color
	
	::GLfloat lw;
	
	wireframe wf;
};

namespace d2 {

typedef m::spoint_2d point;
typedef m::spoint_2d velocity;

typedef m::ssize_2d size;

struct rectangle :
	public drawable
{
	explicit rectangle(
		const point& pv = point(),
		const size& sv = size(1, 1),
		const rgb& cv = rgb(PUP_C3f_WHITE)
	) :
		drawable(cv),
		pos(pv),
		dim(sv)
	{}

	virtual void do_render();

	point pos; // position
	size dim; // size
};

// Push a projection matrix that will make object world
// coordinates identical to window coordinates.
inline void push_screen_coordinate_matrix() throw()
{
	::glPushAttrib(GL_TRANSFORM_BIT);

	::GLint viewport[4];
	::glGetIntegerv(GL_VIEWPORT, viewport);

	::glMatrixMode(GL_PROJECTION);
	::glPushMatrix();
	::glLoadIdentity();

	::gluOrtho2D(
		viewport[0],
		viewport[2],
		viewport[1],
		viewport[3]
	);

	::glMatrixMode(GL_MODELVIEW);
	::glLoadIdentity();
	::glClear(GL_DEPTH_BUFFER_BIT);

	::glDepthMask(GL_FALSE);
	::glDisable(GL_DEPTH_TEST);

	::glPopAttrib();
}

// Alias for %push_screen_coordinate_matrix().
inline void push_matrix() throw()
{
	push_screen_coordinate_matrix();
}

// Pop a screen coordinate projection matrix.
inline void pop_screen_coordinate_matrix() throw()
{
	::glPushAttrib(GL_TRANSFORM_BIT);
	::glMatrixMode(GL_PROJECTION);
	::glPopMatrix();
	::glMatrixMode(GL_MODELVIEW);
	::glLoadIdentity();
	::glDepthMask(GL_TRUE);
	::glEnable(GL_DEPTH_TEST);
	::glPopAttrib();
}

// Alias for %pop_screen_coordinate_matrix().
inline void pop_matrix() throw()
{
	pop_screen_coordinate_matrix();
}

struct scoped_screen_coordinate_matrix :
	private boost::noncopyable
{
	scoped_screen_coordinate_matrix() throw() { push_screen_coordinate_matrix(); }
	~scoped_screen_coordinate_matrix() throw() { pop_screen_coordinate_matrix(); }
};

typedef scoped_screen_coordinate_matrix scoped_matrix;

} // d2

namespace d3 {

typedef m::dpoint_3d point;
typedef m::dpoint_3d rotation;
typedef m::dpoint_3d velocity;

typedef m::dsize_3d size;

struct cuboid :
	public drawable
{
	explicit cuboid(
		const point& pv = point(),
		const size& sv = size(PUP_SZ3f_1),
		const rgb& cv = rgb(PUP_C3f_WHITE)
	) :
		drawable(cv),
		pos(pv),
		dim(sv)
	{}

	virtual void do_render();

	point pos; // position
	size dim; // size
};

struct line :
	public drawable
{
	explicit line(
		const point& p0 = point(),
		const point& p1 = point(),
		const rgb& cv = rgb(PUP_C3f_WHITE)
	) :
		drawable(cv),
		pos0(p0),
		pos1(p1)
	{}

	virtual void do_render();

	point pos0;
	point pos1;
};

// FIXME: This doesn't make too much sense.
struct pyramid :
	public drawable
{
	explicit pyramid(
		const ::GLdouble av = 0.0f,
		const ::GLdouble szav = 1.0f,
		const ::GLdouble szov = 1.0f,
		const point& pv = point(),
		const point& dv = point(+0.0, +1.0, +0.0),
		const point& nv = point(+0.0, +1.0, +0.0),
		const rgb& cv = rgb(PUP_C3f_WHITE)
	) :
		drawable(cv),
		a(av),
		sza(szav),
		szo(szov),
		pos(pv),
		dir(dv)
	{}

	virtual void do_render();

	::GLdouble a; // angle in degrees
	::GLdouble sza; // size, adjecant side
	::GLdouble szo; // size, opposite side
	point pos; // position
	point dir; // direction
};

// Utility class for working with %gluLookAt().
struct view
{
	explicit view(
		const point& ev = point(),
		const point& cv = point(),
		const point& uv = point()
	) throw() :
		clmask(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT),
		e(ev),
		c(cv),
		u(uv)
	{}

	inline void clear() const throw()
	{
		::glClear(clmask);
		::glLoadIdentity();
	}

	inline void look() const throw()
	{
		::gluLookAt(
			PUP_3(e.x(), e.y(), e.z()),
			PUP_3(c.x(), c.y(), c.z()),
			PUP_3(u.x(), u.y(), u.z())
		);
	}

	::GLbitfield clmask;

	point e; // eye
	point c; // center
	point u; // up
};

} // d3

inline void place_light(const ::GLenum num, ::GLfloat* diffuse,
	::GLfloat* ambient, ::GLfloat* specular, ::GLfloat* position)
{
	if (diffuse)
		::glLightfv(num, GL_DIFFUSE, diffuse);
	if (ambient)
		::glLightfv(num, GL_AMBIENT, ambient);
	if (specular)
		::glLightfv(num, GL_SPECULAR, specular);

	::glLightfv(num, GL_POSITION, position);
	::glEnable(num);
}

struct scoped_disable_lighting :
	private boost::noncopyable
{
	scoped_disable_lighting() throw() { ::glDisable(GL_LIGHTING); }
	~scoped_disable_lighting() throw() { ::glEnable(GL_LIGHTING); }
};

namespace ft {

enum text_align {
	ALIGN_LEFT,
	ALIGN_RIGHT,
	ALIGN_CENTER
};

class face;
class typewriter;

// FIXME: UTF-8 support not implemented, can only handle ASCII.
class face :
	private boost::noncopyable
{
public:
	explicit face(
		typewriter& tw,
		const boost::filesystem::path& path,
		int size,
		const rgb& c,
		::GLfloat d
	);
	virtual ~face() throw();

	virtual void print_2d(int x, int y, const std::string& text, const rgb& col);
	virtual void print_2d(int x, int y, const std::string& text);
	
	virtual ::GLuint text_width(const std::string& text);
	virtual ::GLfloat text_height(const std::string& text);
	virtual ::GLfloat line_height();

	virtual void set_color(const rgb& c) throw() { color_ = c; }
	virtual void set_divisor(::GLfloat d) { divisor_ = d; }

	virtual ::GLfloat get_sizef() { return size_; }
	virtual ::GLint get_sizei() { return static_cast<::GLint>(this->get_sizef()); }

protected:
	virtual void load_char(::FT_Face f, unsigned char ch);

	const unsigned char first_char_;
	const unsigned char last_char_;

	rgb color_;

	::GLuint* textures_;
	::GLuint lists_;

	::GLfloat size_;
	::GLfloat divisor_;

	d2::size* dims_;
};

typedef std::shared_ptr<face> face_ptr;
typedef std::map<std::string, face_ptr> face_map;

class typewriter :
	private boost::noncopyable
{
public:
	typewriter();
	virtual ~typewriter() throw();

	void load(const boost::filesystem::path& path, int size,
		const rgb& c = rgb(PUP_C3f_WHITE), ::GLfloat d = 1.0f);

	face_ptr& find_ptr(std::string name, int size);
	face& find(std::string name, int size);
	
	::FT_Library get_lib() throw() { return lib_; }

protected:
	::FT_Library lib_;
	face_map faces_;
};

void print_2d(const std::string& text, const rgb& col, face_ptr& face,
	int origo_x, int origo_y, int w, int h, text_align align);

} // ft

namespace ui {

class form;
class element;
class expander;
class collection;
class widget;

class element :
	private boost::noncopyable,
	public reactor
{
public:
	explicit element(form& owner, const std::string& name);
	virtual ~element() throw();

	virtual bool react(::SDL_Event& event) = 0;
	virtual void think() = 0;
	virtual void render() = 0;

	virtual void finalize() = 0;

	virtual int get_width() = 0;
	virtual int get_height() = 0;

	virtual form& get_owner() throw() { return owner_; }
	virtual const form& get_owner() const throw() { return owner_; }

	virtual element* get_parent() throw() { return parent_; }
	virtual const element* get_parent() const throw() { return parent_; }
	virtual void set_parent(element* parent)
	{
		parent_ = parent;
	}
	
	virtual void set_name(const std::string& name) { name_ = name; }
	const std::string& get_name() const { return name_; }

	virtual void set_relative_pos(const d2::point& p) { relative_pos_ = p; }
	virtual const d2::point& get_relative_pos() const
	{
		return relative_pos_;
	}

	virtual int get_after() const throw() { return after_; }
	virtual void set_after(int a) throw() { after_ = a; }

	virtual d2::point screen_coordinate();
	virtual d2::point as_screen_coordinate(const d2::point& pos);

	virtual int get_padding() const throw() { return padding_; }
	virtual void set_padding(int p) throw()
	{
		padding_ = p;
	}

private:
	form& owner_;
	element* parent_;
protected:
	std::string name_;
	d2::point relative_pos_;
	int after_;
	int padding_;
};

typedef std::shared_ptr<element> element_ptr;

bool test_hover(element& el, const ::SDL_Event& event);
bool test_clicked(element& el, const ::SDL_Event& event, const ::Uint8 button);

class expander :
	public element
{
public:
	explicit expander(form& owner, const std::string& name) :
		element(owner, name),
		border_(0)
	{
		content_box_.dim.wh(0, 0);
		border_box_.dim.wh(0, 0);
	}

	virtual ~expander() throw()
	{}

	virtual bool react(::SDL_Event& event) { return false; }
	virtual void think() {}
	virtual void render();

	virtual void finalize();

	virtual void set_parent(element* parent)
	{
		expander* e = dynamic_cast<expander*>(parent);
		if (!border_box_.dim.w() && e) { this->set_width(e->get_inner_size().w()); }
		if (!border_box_.dim.h() && e) { this->set_height(e->get_inner_size().h()); }
		element::set_parent(parent);
	}
	
#define SET_X_REL2PARENT(Short, Long)                                          \
	virtual void set_##Long##_rel2parent(unsigned int percentage)              \
	{                                                                          \
		expander* parent = dynamic_cast<expander*>(this->get_parent());        \
		if (!parent)                                                           \
			PUP_ERR(std::logic_error, "expander is an orphan");                \
		this->set_##Long(static_cast<int>(                                     \
			static_cast<float>(percentage) *                                   \
			static_cast<float>(parent->get_inner_size().Short()) /           \
			100.0f + 0.5f                                                      \
		));                                                                    \
	}

	SET_X_REL2PARENT(w, width)
	SET_X_REL2PARENT(h, height)

#undef SET_X_REL2PARENT
	
	virtual void set_content_box_col(const rgb& c) throw() { content_box_.col.set(c); }
	virtual void set_border_box_col(const rgb& c) throw() { border_box_.col.set(c); }

	virtual void set_width(int w) throw()
	{
		border_box_.dim.w(w);
		content_box_.dim.w(border_box_.dim.w() - 2 * border_);
		inner_size_.w(content_box_.dim.w() - 2 * padding_);
	}

	virtual void set_height(int h) throw()
	{
		border_box_.dim.h(h);
		content_box_.dim.h(border_box_.dim.h() - 2 * border_);
		inner_size_.h(content_box_.dim.h() - 2 * padding_);
	}

	virtual void set_padding(int p) throw()
	{
		element::set_padding(p);
		this->resize();
	}

	virtual int get_border() const throw() { return border_; }
	virtual void set_border(int b) throw()
	{
		border_ = b;
		this->resize();
	}

	virtual const d2::size& get_content_size() const throw() { return content_box_.dim; }
	virtual const d2::size& get_border_size() const throw() { return border_box_.dim; }
	virtual const d2::size& get_inner_size() const throw() { return inner_size_; }

	virtual d2::point border_screen_coordinate();
	virtual d2::point content_screen_coordinate();

	virtual int get_width() { return this->get_border_size().w(); }
	virtual int get_height() { return this->get_border_size().h(); }

	virtual int get_inner_width() { return this->get_inner_size().w(); }
	virtual int get_inner_height() { return this->get_inner_size().h(); }

protected:
	virtual void resize();

	d2::rectangle content_box_;
	d2::rectangle border_box_;
	d2::size inner_size_;
	int border_;
};

class collection :
	public expander
{
public:
	typedef std::list<element*> element_list;
	typedef element_list::iterator iterator;
	typedef element_list::const_iterator const_iterator;
	typedef element_list::reverse_iterator reverse_iterator;
	typedef element_list::const_reverse_iterator const_reverse_iterator;

	explicit collection(form& owner, const std::string& name);
	virtual ~collection() throw();

	virtual bool react(::SDL_Event& event);
	virtual void think();
	virtual void render();

	virtual void finalize();
	
	virtual void push_front(element& el);
	virtual void push_back(element& el);
	virtual bool remove(element& el);

	iterator begin() { return elements_.begin(); }
	const_iterator begin() const { return elements_.begin(); }
	iterator end() { return elements_.end(); }
	const_iterator end() const { return elements_.end(); }

	reverse_iterator rbegin() { return elements_.rbegin(); }
	const_reverse_iterator rbegin() const { return elements_.rbegin(); }
	reverse_iterator rend() { return elements_.rend(); }
	const_reverse_iterator rend() const { return elements_.rend(); }

protected:
	element_list elements_;
};

class validator
{
public:
	virtual ~validator() throw()
	{}
	
	virtual bool operator()(const std::string& value) = 0;
};

class notempty_validator :
	public validator
{
public:
	virtual ~notempty_validator() throw()
	{}

	virtual bool operator()(const std::string& value)
	{
		return !value.empty();
	}
};

class regex_validator :
	public validator
{
public:
	template <class RegEx>
	explicit regex_validator(const RegEx& re) :
		regex_(re)
	{}
	virtual ~regex_validator() throw()
	{}
	
	virtual bool operator()(const std::string& value)
	{
		return boost::regex_match(value, regex_);
	}
	
private:
	boost::regex regex_;
};

class widget :
	public element
{
public:
	typedef boost::function<bool (const std::string&)> validator_function;
	typedef std::map<std::string, validator_function> validator_map;
	
	explicit widget(form& owner, const std::string& name);
	virtual ~widget() throw();

	virtual bool react(::SDL_Event& event);
	virtual void think() = 0;
	
	virtual void finalize();
	
	virtual int get_width();
	
	virtual void set_label(const std::string& label) { label_ = label; }
	
	virtual std::string string_value() = 0;
	
	virtual bool validate();
	virtual void add_validator(const std::string& name, const validator_function& v_fn);
	virtual void remove_validator(const std::string& name);

protected:
	bool hover_;
	std::string label_;
	d2::point label_pos_;
	validator_map validators_;
};

class text :
	public widget
{
public:
	explicit text(
		form& owner,
		const std::string& name,
		const ft::face_ptr& face,
		const rgb& col,
		const ft::text_align& align
	);
	virtual ~text() throw();

	virtual bool react(::SDL_Event& event);
	virtual void think();
	virtual void render();

	virtual void finalize();

	virtual int get_height();
	
	virtual std::string string_value()
	{
		return boost::str(boost::format("text:%1%:%2%")
			% name_
			% label_
		);
	}

protected:
	ft::face_ptr face_;
	rgb col_;
	ft::text_align align_;
};

class input :
	public widget
{
public:
	explicit input(form& owner, const std::string& name);
	virtual ~input() throw();

	virtual bool react(::SDL_Event& event);
	virtual void think();
	virtual void render();

	virtual void finalize();

	virtual int get_height();

	virtual void set_value(const std::string& v) { value_ = v; }
	virtual const std::string& get_value() const { return value_; }
	virtual std::string string_value() { return this->get_value(); }

protected:
	char append_;
	std::string value_;
	d2::point value_pos_;
	d2::rectangle value_box_;
	d2::rectangle shadow_box_;
};

class keypress :
	public input
{
public:
	explicit keypress(form& owner, const std::string& name);
	virtual ~keypress() throw();

	virtual bool react(::SDL_Event& event);
};

class button :
	public widget
{
public:
	typedef boost::function<void (::SDL_Event& event)> click_function;

	explicit button(form& owner, const std::string& name, const click_function& action);
	virtual ~button() throw();

	virtual bool react(::SDL_Event& event);
	virtual void think();
	virtual void render();

	virtual void finalize();

	virtual int get_height();
	
	virtual std::string string_value()
	{
		return boost::str(boost::format("button:%1%:%2%")
			% name_
			% label_
		);
	}

protected:
	click_function action_;
	d2::rectangle label_box_;
	d2::rectangle shadow_box_;
};

class checkbox :
	public widget
{
public:
	explicit checkbox(form& owner, const std::string& name);
	virtual ~checkbox() throw();

	virtual bool react(::SDL_Event& event);
	virtual void think();
	virtual void render();

	virtual void finalize();

	virtual int get_height();

	virtual void set_checked(bool c) { checked_ = c; }
	virtual bool get_checked() { return checked_; }
	virtual std::string string_value() { return std::to_string(checked_); }

protected:
	bool checked_;
};

class theme :
	private boost::noncopyable
{
public:
	typedef boost::function<const rgb& ()> color_function;
	typedef std::map<std::string, color_function> color_map;

	explicit theme(
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
	);
	virtual ~theme() throw();

	rgb get(const std::string& name, const std::string& delta = "zero") const;

	inline const rgb& get_label() const { return label_; }
	inline const rgb& get_value() const { return value_; }
	inline const rgb& get_button() const { return value_; }
	inline const rgb& get_norm_box() const { return norm_box_; }
	inline const rgb& get_err_box() const { return err_box_; }
	inline const rgb& get_bg_box() const { return bg_box_; }
	inline const rgb& get_shadow_box() const { return shadow_box_; }
	inline const rgb& get_antishadow_box() const { return antishadow_box_; }
	
	inline const rgb& get_zero_delta() const { return zero_delta_; }
	inline const rgb& get_focus_delta() const { return focus_delta_; }
	inline const rgb& get_hover_delta() const { return hover_delta_; }

	virtual void set_label_face(const ft::face_ptr& face) { label_face_ = face; }
	virtual ft::face_ptr& get_label_face()
	{
		if (!label_face_)
			PUP_ERR(std::runtime_error, "label face not set");
		return label_face_;
	}

	virtual void set_value_face(const ft::face_ptr& face) { value_face_ = face; }
	virtual ft::face_ptr& get_value_face()
	{
		if (!value_face_)
			PUP_ERR(std::runtime_error, "value face not set");
		return value_face_;
	}

	virtual void set_button_face(const ft::face_ptr& face) { button_face_ = face; }
	virtual ft::face_ptr& get_button_face()
	{
		if (!button_face_)
			PUP_ERR(std::runtime_error, "button face not set");
		return button_face_;
	}

	virtual ft::typewriter& get_typewriter() { return typewriter_; }

protected:
	ft::typewriter& typewriter_;
	ft::face_ptr label_face_;
	ft::face_ptr value_face_;
	ft::face_ptr button_face_;

	color_map colors_;
	color_map deltas_;
	rgb label_;
	rgb value_;
	rgb button_;
	rgb norm_box_;
	rgb err_box_;
	rgb bg_box_;
	rgb shadow_box_;
	rgb antishadow_box_;

	rgb zero_delta_;
	rgb focus_delta_;
	rgb hover_delta_;
};

class form :
	private boost::noncopyable,
	public reactor
{
public:
	explicit form(controller& ctrlr, theme& thm);
	virtual ~form() throw() {}

	virtual bool react(::SDL_Event& event);
	virtual void think();
	virtual void render();

	virtual void finalize();
	
	virtual bool validate();

	virtual void set_focused(element* el) = 0;
	virtual bool is_focused(element* el) const = 0;

	virtual collection& get_collection() throw() { return elements_; }
	virtual controller& get_controller() throw() { return ctrlr_; }

	virtual const d2::point& get_form_pos() const { return form_pos_; }

	virtual const d2::size& get_form_size() const { return form_size_; }
	virtual const d2::size& get_window_size() const { return window_size_; }

	theme& get_theme() { return theme_; }

protected:
	bool validate_collection(const collection* elements);
	
	controller& ctrlr_;
	theme& theme_;
	collection elements_;
	d2::point form_pos_;
	d2::size form_size_;
	d2::size window_size_;
};

// Simple form with global, thread-unsafe element focus
// management. Should work well for most normal main and
// option menus.
class static_focus_form :
	public form
{
public:
	explicit static_focus_form(controller& ctrlr, theme& thm) :
		form(ctrlr, thm)
	{}

	virtual ~static_focus_form() throw() {}

	virtual void set_focused(element* el) { focused = el; }
	virtual bool is_focused(element* el) const { return focused == el; }

protected:
	static element* focused;
};

} // ui

} // gl1
} // pup

#endif
