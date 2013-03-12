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


def check_lib_list(conf, uselib, uselib_store, lib_list):
	for lib in lib_list:
		if conf.check_cxx(mandatory = 0, lib = lib, uselib = uselib, uselib_store = uselib_store):
			return True
	return False


valid_platforms = ['fb', 'x11']
valid_devices = ['generic', 'imx6']


def options(opt):
	opt.load('compiler_cxx')
	opt.add_option('--platform', action='store', default='', help='platform to build for (valid values: ' + ' '.join(valid_platforms) + ')')
	opt.add_option('--device', action='store', default='generic', help='device to build for (valid values: ' + ' '.join(valid_devices) + ')')


# generic (device agnostic) checks

def check_x11(conf, uselib = ''):
	conf.check_cxx(mandatory = 1, lib = 'X11', uselib = uselib, uselib_store = 'X11')
	conf.check_cxx(mandatory = 1, header_name = 'X11/Xlib.h', uselib = uselib, uselib_store = 'X11')


def check_egl(conf, uselib = ''):
	conf.check_cxx(mandatory = 1, lib = 'EGL', uselib = uselib, uselib_store = 'EGL')
	conf.check_cxx(mandatory = 1, header_name = 'EGL/egl.h', uselib = uselib, uselib_store = 'EGL')


def check_openvg(conf, uselib = 'EGL'):
	return \
	  conf.check_cxx(mandatory = 0, lib = 'OpenVG', uselib = uselib, uselib_store = 'OPENVG') and \
	  conf.check_cxx(mandatory = 0, header_name = 'VG/openvg.h', uselib = uselib, uselib_store = 'OPENVG')


def check_gles1(conf, uselib = 'EGL', lib_list = ['GLESv1_CM', 'GLESV1_CL']):
	return \
	  check_lib_list(conf = conf, uselib = uselib, uselib_store = 'GLES1', lib_list = lib_list) and \
	  conf.check_cxx(mandatory = 0, header_name = 'GLES/gl.h', uselib = uselib, uselib_store = 'GLES1')


def check_gles2(conf, uselib = 'EGL', lib_list = ['GLESv2']):
	return \
	  check_lib_list(conf = conf, uselib = uselib, uselib_store = 'GLES2', lib_list = lib_list) and \
          conf.check_cxx(mandatory = 0, header_name = 'GLES2/gl2.h', uselib = uselib, uselib_store = 'GLES2')


def check_opengl(conf, uselib = 'EGL'):
	return \
	  conf.check_cxx(mandatory = 0, lib = 'GL', uselib = uselib, uselib_store = 'OPENGL') and \
          conf.check_cxx(mandatory = 0, header_name = 'GL/gl.h', uselib = uselib, uselib_store = 'OPENGL')


# device specific configuration


def check_vivante_egl(conf, egl_macro):
	conf.check_cxx(mandatory = 1, lib = ['EGL', 'GAL'], uselib_store = 'EGL')
	old_cxxflags = list(conf.env['CXXFLAGS'])
	extra_cxxflags = ['-DLINUX']
	if egl_macro:
		extra_cxxflags += ['-D%s' % egl_macro]
	conf.env['CXXFLAGS'] += extra_cxxflags
	conf.check_cxx(mandatory = 1, header_name = 'EGL/eglvivante.h', uselib_store = 'EGL')
	conf.check_cxx(mandatory = 1, header_name = 'EGL/egl.h', uselib_store = 'EGL')
	conf.env['CXXFLAGS_EGL'] += extra_cxxflags
	conf.env['CXXFLAGS'] = old_cxxflags
	conf.define('WITH_VIVANTE_EGL', 1)


def configure_imx6_device(conf, platform):
	conf.env['PLATFORM_USELIBS'] = ['GLES2', 'OPENVG', 'EGL']
	if platform == "x11":
		check_x11(conf)
		check_vivante_egl(conf, None)
		conf.env['PLATFORM_SOURCE'] = ['src/platform_x11_generic.cpp']
		conf.env['PLATFORM_USELIBS'] += ["X11"]
	elif platform == "fb":
		check_vivante_egl(conf, 'EGL_API_FB')
		conf.env['PLATFORM_SOURCE'] = ['src/platform_fb_imx6.cpp']
	check_gles2(conf)
	check_openvg(conf)
	conf.env['WITH_APIS'] = ['GLES1', 'GLES2', 'OPENVG', 'OPENGL']


def configure_generic_device(conf, platform):
	conf.env['PLATFORM_USELIBS'] = []
	if platform == "x11":
		check_x11(conf)
		conf.env['PLATFORM_SOURCE'] = ['src/platform_x11_generic.cpp']
		conf.env['PLATFORM_USELIBS'] += ["X11"]
	elif platform == "fb":
		conf.env['PLATFORM_SOURCE'] = ['src/platform_fb_generic.cpp']

	check_egl(conf, '')

	conf.env['WITH_APIS'] = []

	if check_openvg(conf):
		conf.define('WITH_OPENVG', 1)
		conf.env['WITH_APIS'] += ['OPENVG']
		conf.env['PLATFORM_USELIBS'] += ['OPENVG']

	with_gles1 = check_gles1(conf)
	with_gles2 = check_gles2(conf)
	with_opengl = check_opengl(conf)

	gluselib = None

	if with_gles2:
		conf.env['WITH_APIS'] += ['GLES1', 'GLES2']
		gluselib = 'GLES2'
	elif with_gles1:
		conf.env['WITH_APIS'] += ['GLES1']
		gluselib = 'GLES1'
	elif with_opengl:
		conf.env['WITH_APIS'] += ['OPENGL']
		gluselib = 'OPENGL'

	if gluselib:
		conf.env['PLATFORM_USELIBS'] += [gluselib]

	conf.env['PLATFORM_USELIBS'] += ['EGL']



# main functions

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

	# device specifics
	if conf.options.device == "imx6":
		configure_imx6_device(conf, conf.options.platform)
	elif conf.options.device == "generic":
		configure_generic_device(conf, conf.options.platform)

	for api in conf.env['WITH_APIS']:
		conf.env['WITH_' + api] = True
		conf.define('WITH_' + api, 1)

	if conf.env['WITH_GLES1'] or conf.env['WITH_GLES2'] or conf.env['WITH_OPENGL']:
		conf.env['PLATFORM_SOURCE'] += ['src/process_glapi.cpp']
	if conf.env['WITH_OPENVG']:
		conf.env['PLATFORM_SOURCE'] += ['src/process_openvg.cpp']

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
		uselib = bld.env['PLATFORM_USELIBS'],
		includes = ['.', 'src'],
		target = executable_name,
		source = source + bld.env['PLATFORM_SOURCE']
	)

