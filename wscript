#!/usr/bin/env python

from waflib.Build import BuildContext, CleanContext, InstallContext, UninstallContext

top = '.'
out = 'build'


executable_name = 'eglinfo'


def check_compiler_flag(conf, flag, lang):
	return conf.check(fragment = 'int main() { float f = 4.0; char c = f; return c - 4; }\n', execute = 0, define_ret = 0, msg = 'Checking for compiler switch %s' % flag, cxxflags = conf.env[lang + 'FLAGS'] + [flag], okmsg = 'yes', errmsg = 'no')  # the code inside fragment deliberately does an unsafe implicit cast float->char to trigger a compiler warning; sometimes, gcc does not tell about an unsupported parameter *unless* the code being compiled causes a warning


def check_compiler_flags_2(conf, cflags, ldflags, msg):
	return conf.check(fragment = 'int main() { float f = 4.0; char c = f; return c - 4; }\n', execute = 0, define_ret = 0, msg = msg, cxxflags = cflags, ldflags = ldflags, okmsg = 'yes', errmsg = 'no')  # the code inside fragment deliberately does an unsafe implicit cast float->char to trigger a compiler warning; sometimes, gcc does not tell about an unsupported parameter *unless* the code being compiled causes a warning


def add_compiler_flags(conf, env, flags, lang, compiler, uselib = ''):
	for flag in flags:
		if type(flag) == type(()):
			flag_candidate = flag[0]
			flag_alternative = flag[1]
		else:
			flag_candidate = flag
			flag_alternative = None

		if uselib:
			flags_pattern = lang + 'FLAGS_' + uselib
		else:
			flags_pattern = lang + 'FLAGS'

		if check_compiler_flag(conf, flag_candidate, compiler):
			env.prepend_value(flags_pattern, [flag_candidate])
		elif flag_alternative:
			if check_compiler_flag(conf, flag_alternative, compiler):
				env.prepend_value(flags_pattern, [flag_alternative])


valid_platforms = ['fb', 'x11']
valid_devices = ['generic', 'imx6']


def options(opt):
	opt.load('compiler_cxx')
	opt.add_option('--platform', action='store', default='', help='platform to build for (valid values: ' + ' '.join(valid_platforms) + ')')
	opt.add_option('--device', action='store', default='generic', help='device to build for (valid values: ' + ' '.join(valid_devices) + ')')


def check_vivante_egl(egl_macro):
	old_cxxflags = conf.env['CXXFLAGS']
	extra_cxxflags = ['-DLINUX']
	if egl_macro:
		extra_cxxflags += ['-D%s' % egl_macro]
	conf.env['CXXFLAGS'] += extra_cxxflags
	if conf.check_cxx(mandatory = 0, header_name = 'EGL/eglvivante.h', uselib_store = 'EGL'):
		conf.env['CXXFLAGS_EGL'] += extra_cxxflags
	conf.env['CXXFLAGS'] = old_cxxflags
	conf.define('WITH_VIVANTE_EGL', 1)


def configure(conf):
	try:
		valid_platforms.index(conf.options.platform)
	except ValueError:
		conf.fatal("Invalid or no platform selected")

	try:
		valid_devices.index(conf.options.device)
	except ValueError:
		conf.fatal("Invalid device selected")

	conf.load('compiler_cxx')

	# check and add compiler flags
	if conf.env['CXXFLAGS'] and conf.env['LINKFLAGS']:
		check_compiler_flags_2(conf, conf.env['CXXFLAGS'], conf.env['LINKFLAGS'], "Testing compiler flags %s and linker flags %s" % (' '.join(conf.env['CXXFLAGS']), ' '.join(conf.env['LINKFLAGS'])))
	elif conf.env['CXXFLAGS']:
		check_compiler_flags_2(conf, conf.env['CXXFLAGS'], '', "Testing compiler flags %s" % ' '.join(conf.env['CXXFLAGS']))
	elif conf.env['LINKFLAGS']:
		check_compiler_flags_2(conf, '', conf.env['LINKFLAGS'], "Testing linker flags %s" % ' '.join(conf.env['LINKFLAGS']))

	compiler_flags = ['-Wextra', '-Wall', '-std=c++98', '-pedantic']
#	compiler_flags += ['-O2', '-s']
	compiler_flags += ['-O0', '-g3', '-ggdb']

	add_compiler_flags(conf, conf.env, compiler_flags, 'CXX', 'CXX')

	extra_sources = []

	# platform and device specifics
	if conf.options.platform == "x11":
		conf.check_cxx(mandatory = 1, lib = 'X11', uselib_store = 'X11')
		conf.check_cxx(mandatory = 1, header_name = 'X11/Xlib.h', uselib_store = 'X11')
		extra_sources += ['src/platform_x11.cpp']
		if conf.options.device == "imx6":
			check_vivante_egl(None)
	elif conf.option.platform == "fb":
		if conf.options.device == "generic":
			extra_sources += ['src/platform_fb_generic.cpp']
		elif conf.options.device == "imx6":
			check_vivante_egl('EGL_API_FB')
			extra_sources += ['src/platform_fb_imx6.cpp']
	conf.env['PLATFORM_SOURCE'] = extra_sources

	# EGL
	conf.check_cxx(mandatory = 1, lib = 'EGL', uselib_store = 'EGL')
	conf.check_cxx(mandatory = 1, header_name = 'EGL/egl.h', uselib_store = 'EGL')

	# OpenVG
	with_openvg = \
	  conf.check_cxx(mandatory = 0, lib = 'OpenVG', uselib = 'EGL', uselib_store = 'OPENVG') and \
	  conf.check_cxx(mandatory = 0, header_name = 'VG/openvg.h', uselib = 'EGL', uselib_store = 'OPENVG')
	if with_openvg:
		conf.define('WITH_OPENVG', 1)
		conf.env['PLATFORM_SOURCE'] += ['src/process_openvg.cpp']

	# OpenGL, OpenGL ES 1, OpenGL ES 2
	with_gles1 = \
	  (conf.check_cxx(mandatory = 0, lib = 'GLESv1_CM', uselib = 'EGL', uselib_store = 'GLES1') or \
	   conf.check_cxx(mandatory = 0, lib = 'GLESv1_CL', uselib = 'EGL', uselib_store = 'GLES1')) and \
	  conf.check_cxx(mandatory = 0, header_name = 'GLES/gl.h', uselib = 'EGL', uselib_store = 'GLES1')
	with_gles2 = \
	  conf.check_cxx(mandatory = 0, lib = 'GLESv2', uselib = 'EGL', uselib_store = 'GLES2') and \
          conf.check_cxx(mandatory = 0, header_name = 'GLES2/gl2.h', uselib = 'EGL', uselib_store = 'GLES2')
	with_opengl = \
	  conf.check_cxx(mandatory = 0, lib = 'GL', uselib = 'EGL', uselib_store = 'OPENGL') and \
          conf.check_cxx(mandatory = 0, header_name = 'GL/gl.h', uselib = 'EGL', uselib_store = 'OPENGL')
	if with_gles1 or with_gles2 or with_opengl:
		conf.env['PLATFORM_SOURCE'] += ['src/process_glapi.cpp']
	if with_gles1:
		conf.define('WITH_GLES1', 1)
		conf.env['GLUSELIB'] = "GLES1"
	if with_gles2:
		conf.define('WITH_GLES1', 1)
		conf.define('WITH_GLES2', 1)
		conf.env['GLUSELIB'] = "GLES2"
	if with_opengl and not (with_gles1 or with_gles2):
		conf.define('WITH_OPENGL', 1)
		conf.env['GLUSELIB'] = "OPENGL"

	conf.write_config_header()


def build(bld):
	source = [ \
		"src/csv_writer.cpp", \
		"src/json_writer.cpp", \
		"src/log.cpp", \
		"src/main.cpp", \
		"src/process_egl.cpp", \
		"src/scopes.cpp", \
		"src/text_writer.cpp", \
	]
	bld(
		features = ['cxx', 'cxxprogram'],
		uselib = 'X11 EGL OPENVG %s' % bld.env['GLUSELIB'],
		includes = ['.', 'src'],
		target = executable_name,
		source = source + bld.env['PLATFORM_SOURCE']
	)

