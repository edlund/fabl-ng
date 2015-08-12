
# FABL-ng - FAlling blocks next generation
# Copyright 2015 (C) Erik Edlund <erik.edlund@32767.se>

import platform

bits, binary = platform.architecture()

libpath_postfix = 'x86-64' if bits == '64bit' else 'x86-32'
libpaths = [
	'vendor/builds/boost-1.58/lib-linux-{}',
	'vendor/builds/sqlitecpp-1.1/lib-linux-{}'
]

env = Environment(
	CCFLAGS=[
		'-Wall',
		'-Wno-char-subscripts',
		'-Wno-unused-local-typedefs',
		'-Fno-omit-frame-pointer',
		'-g',
		'-O0',
		'-DBOOST_LOG_DYN_LINK'
	],
	CXXFLAGS=[
		'-std=c++14'
	],
	LIBS=[
		'm',
		'rt',
		'dl',
		'pthread',
		'boost_filesystem',
		'boost_log',
		'boost_log_setup',
		'boost_program_options',
		'boost_regex',
		'boost_system',
		'boost_thread',
		'SDL2_mixer',
		'sqlitecpp'
	],
	CPPPATH=[
		'vendor/builds/boost-1.58/include',
		'vendor/builds/sqlitecpp-1.1/include'
	],
	LIBPATH=[
		p.format(libpath_postfix) for p in libpaths
	]
)

binary = 'Debug/fabl-ng.bin'
sources = [
	'fabl-ng/main.cpp',
	'fabl-ng/pup.cpp',
	'fabl-ng/pup_app.cpp',
	'fabl-ng/pup_gl1.cpp',
	'fabl-ng/pup_io.cpp',
	'fabl-ng/pup_m.cpp',
	'fabl-ng/pup_snd.cpp',
	'fabl-ng/pup_test.cpp',
	'fabl-ng/fabl.cpp',
	'fabl-ng/fabl_menus.cpp',
	'fabl-ng/fabl_ng.cpp',
	'fabl-ng/fabl_options.cpp',
	'fabl-ng/fabl_ui.cpp',
	'fabl-ng/fabl_test.cpp'
]
packages = [
	'gl',
	'glu',
	'glew',
	'sdl2',
	'freetype2',
	'sqlite3'
]

for pkg in packages:
	env.ParseConfig("pkg-config {0} --cflags --libs".format(pkg))

env.Program(
	binary,
	sources
)

