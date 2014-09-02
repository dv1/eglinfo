#!/usr/bin/env python


####
## PREREQUISITES

from waflib.Build import BuildContext, CleanContext, InstallContext, UninstallContext, Logs

top = '.'
out = 'build'


executable_name = 'eglinfo'




####
## OPTIONS


valid_platforms = ['fb', 'x11']
valid_devices = ['generic', 'imx6', 'beagleboard', 'raspberrypi']
needs_sysroot = ['raspberrypi']
needs_prefix = ['raspberrypi']

def options(opt):
	opt.load('compiler_c')
	opt.load('compiler_cxx')
	egl_opts = opt.add_option_group("EGL specific options")
	egl_opts.add_option('--platform', action='store', default='', help='platform to build for (valid values: ' + ' '.join(valid_platforms) + ')')
	egl_opts.add_option('--device', action='store', default='generic', help='device to build for (valid values: ' + ' '.join(valid_devices) + ')')
	egl_opts.add_option('--sysroot', action='store', default='', help='sysroot path')




####
## CONFIGURATION


# generic (device agnostic) checks


# Tests if a given compiler flag is supported by the compiler
def check_compiler_flag(conf, flag, lang):
	# the code inside fragment deliberately does an unsafe implicit cast float->char to trigger a compiler warning; sometimes, gcc does not tell about an unsupported parameter *unless* the code being compiled causes a warning
	if lang == "CC":
		return conf.check_cc(fragment = 'int main() { float f = 4.0; char c = f; return c - 4; }\n', execute = 0, define_ret = 0, msg = 'Checking for compiler switch %s' % flag, cflags = conf.env[lang + 'FLAGS'] + [flag], okmsg = 'yes', errmsg = 'no')
	elif lang == "CXX":
		return conf.check_cxx(fragment = 'int main() { float f = 4.0; char c = f; return c - 4; }\n', execute = 0, define_ret = 0, msg = 'Checking for compiler switch %s' % flag, cflags = conf.env[lang + 'FLAGS'] + [flag], okmsg = 'yes', errmsg = 'no')
	else:
		conf.fatal('check_compiler_flag called with invalid language "%s"' % lang)

# Check each given compiler flag, and add it to the CFLAGS of the given uselib if the check is successful
# By default, uselib is "", meaning that flags are added to the global CFLAGS value
# Flags can also have an alternative; if the flag check fails, the alternative is checked in its place
# Example call:
#    add_compiler_flags(conf, env, ['-Wall', '-march=core2', ('-std=c++11', 'std=c++0x')], 'CXX', 'FOO')
def add_compiler_flags(conf, env, flags, var_lang, comp_lang, uselib = ''):
	for flag in flags:
		if type(flag) == type(()):
			flag_candidate = flag[0]
			flag_alternative = flag[1]
		else:
			flag_candidate = flag
			flag_alternative = None

		if uselib:
			flags_pattern = var_lang + 'FLAGS_' + uselib
		else:
			flags_pattern = var_lang + 'FLAGS'

		if check_compiler_flag(conf, flag_candidate, comp_lang):
			env.prepend_value(flags_pattern, [flag_candidate])
		elif flag_alternative:
			if check_compiler_flag(conf, flag_alternative, comp_lang):
				env.prepend_value(flags_pattern, [flag_alternative])

# Try to find one library of a given list
# The first library found is stored in the given uselib_store
def check_lib_list(conf, uselib, uselib_store, lib_list):
	for lib in lib_list:
		if conf.check_cxx(mandatory = 0, lib = lib, uselib = uselib, uselib_store = uselib_store):
			return True
	return False

def check_gccso_shader_binary_def(conf):
	fragment_template = """
		#include <EGL/egl.h>
		#include <GLES2/gl2.h>
		#include <GLES2/gl2ext.h>
		#include <iostream>

		int main()
		{
			std::cout << (%s);
			return 0;
		}
	"""
	checks = [ \
		( 'GL_GCCSO_SHADER_BINARY_FJ', 'WITH_FIXED_GCCSO_SHADER_BINARY_DEF', 'fixed' ), \
		( 'GCCSO_SHADER_BINARY_FJ', 'WITH_OLD_GCCSO_SHADER_BINARY_DEF', 'old' ), \
	]
	passed_check = None
	for check in checks:
		if conf.check_cxx(fragment = (fragment_template % check[0]), uselib = 'EGL GLES2', execute = 0, mandatory = 0, define_ret = 0, msg = 'Checking for %s gccso shader binary definition' % check[2], okmsg = 'yes', errmsg = 'no'):
			passed_check = check
			break

	if passed_check:
		conf.define(passed_check[1], 1)
		return True

	return False

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
	retval = \
	  check_lib_list(conf = conf, uselib = uselib, uselib_store = 'GLES1', lib_list = lib_list) and \
	  conf.check_cxx(mandatory = 0, header_name = 'GLES/gl.h', uselib = uselib, uselib_store = 'GLES1') and \
	  conf.check_cxx(mandatory = 0, header_name = ['GLES2/gl2.h', 'GLES2/gl2ext.h'], uselib = uselib, define_name = 'WITH_GLEXT_H', uselib_store = 'GLES2')
	return retval

def check_gles2(conf, uselib = 'EGL', lib_list = ['GLESv2']):
	retval = \
	  check_lib_list(conf = conf, uselib = uselib, uselib_store = 'GLES2', lib_list = lib_list) and \
	  conf.check_cxx(mandatory = 0, header_name = 'GLES2/gl2.h', uselib = uselib, uselib_store = 'GLES2') and \
	  conf.check_cxx(mandatory = 0, header_name = ['GLES2/gl2.h', 'GLES2/gl2ext.h'], uselib = uselib, define_name = 'WITH_GL2EXT_H', uselib_store = 'GLES2')
	if retval:
		check_gccso_shader_binary_def(conf)
	return retval

def check_opengl(conf, uselib = 'EGL'):
	return \
	  conf.check_cxx(mandatory = 0, lib = 'GL', uselib = uselib, uselib_store = 'OPENGL') and \
          conf.check_cxx(mandatory = 0, header_name = 'GL/gl.h', uselib = uselib, uselib_store = 'OPENGL')



# device specific configuration


def configure_raspberrypi_device(conf, platform):
	conf.env['PLATFORM_USELIBS'] = ['GLES2', 'EGL']
	if platform == "x11":
		check_x11(conf)
		conf.env['PLATFORM_SOURCE'] = ['src/platform_x11_generic.cpp']
		conf.env['PLATFORM_USELIBS'] += ["X11"]
	elif platform == "fb":
		conf.env['PLATFORM_SOURCE'] = ['src/platform_fb_raspberrypi.cpp']
	conf.check_cxx(mandatory = 1, lib = ['GLESv2', 'EGL', 'bcm_host'], uselib_store = 'EGL')
	import os
	sysroot = conf.options.sysroot + conf.options.prefix
	vcos_pthread_path = os.path.join(sysroot, 'include/interface/vcos/pthreads')
	vcms_host_path = os.path.join(sysroot, 'include/interface/vmcs_host/linux')
	if not conf.check_cxx(mandatory = 0, header_name = ['vcos_platform_types.h', 'EGL/egl.h', 'bcm_host.h'], includes = [vcos_pthread_path, vcms_host_path], uselib_store = 'EGL'):
		conf.fatal('Check if --prefix and --sysroot are set correctly.')
	conf.env['WITH_APIS'] = []
	if check_gles2(conf):
		conf.env['WITH_APIS'] += ['GLES1', 'GLES2']
	if check_openvg(conf):
		conf.env['WITH_APIS'] += ['OPENVG']

def configure_beagleboard_device(conf, platform):
	# OpenGL ES 1 does not seem to work properly for this device
	conf.env['PLATFORM_USELIBS'] = ['GLES2', 'OPENVG', 'EGL']
	if platform == "x11":
		check_x11(conf)
		conf.env['PLATFORM_SOURCE'] = ['src/platform_x11_generic.cpp']
		conf.env['PLATFORM_USELIBS'] += ["X11"]
	elif platform == "fb":
		conf.env['PLATFORM_SOURCE'] = ['src/platform_fb_generic.cpp']
	conf.check_cxx(mandatory = 1, lib = ['EGL', 'IMGegl', 'srv_um'], uselib_store = 'EGL')
	conf.check_cxx(mandatory = 1, header_name = 'EGL/egl.h', uselib_store = 'EGL')
	conf.env['WITH_APIS'] = []
	if check_gles2(conf):
		conf.env['WITH_APIS'] += ['GLES2']
	if check_openvg(conf):
		conf.env['WITH_APIS'] += ['OPENVG']

def check_vivante_egl(conf, egl_extra_defines):
	conf.check_cxx(mandatory = 1, lib = ['EGL', 'GAL'], uselib_store = 'EGL')
	old_cxxflags = list(conf.env['CXXFLAGS'])
	extra_defines = ['LINUX']
	if egl_extra_defines:
		extra_defines += egl_extra_defines
	extra_cxxflags = map(lambda x: conf.env['DEFINES_ST'] % x, extra_defines)
	conf.env['CXXFLAGS'] += extra_cxxflags
	conf.check_cxx(mandatory = 1, header_name = 'EGL/eglvivante.h', uselib_store = 'EGL')
	conf.check_cxx(mandatory = 1, header_name = 'EGL/egl.h', uselib_store = 'EGL')
	conf.env['CXXFLAGS_EGL'] += extra_cxxflags
	conf.env['CXXFLAGS'] = old_cxxflags
	conf.define('WITH_VIVANTE_EGL', 1)
def configure_imx6_device(conf, platform):
	conf.env['PLATFORM_USELIBS'] = ['GLES2', 'OPENVG', 'EGL']
	conf.env['WITH_APIS'] = ['GLES2', 'OPENVG']
	if platform == "x11":
		check_x11(conf)
		check_vivante_egl(conf, None)
		conf.env['PLATFORM_SOURCE'] = ['src/platform_x11_generic.cpp']
		conf.env['PLATFORM_USELIBS'] += ["X11"]
		conf.env['WITH_APIS'] += ['OPENGL']
	elif platform == "fb":
		check_vivante_egl(conf, ['EGL_API_FB'])
		conf.env['PLATFORM_SOURCE'] = ['src/platform_fb_imx6.cpp']
	check_gles2(conf)
	check_openvg(conf)

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

	with_gles1 = check_gles1(conf, lib_list = ['GLESv1_CM', 'GLESV1_CL', 'GLES_CM', 'GLES_CL'])
	with_gles2 = check_gles2(conf, lib_list = ['GLESv2', 'GLESv2x', 'GLESv2SC'])
	with_opengl = check_opengl(conf)

	gluselib = None

	if with_gles2:
		conf.env['WITH_APIS'] += ['GLES1', 'GLES2']
		gluselib = 'GLES2'
	elif with_gles1:
		conf.env['WITH_APIS'] += ['GLES1']
		gluselib = 'GLES1'

	if with_opengl:
		conf.env['WITH_APIS'] += ['OPENGL']
		if not gluselib:
			gluselib = 'OPENGL'

	if gluselib:
		conf.env['PLATFORM_USELIBS'] += [gluselib]

	conf.env['PLATFORM_USELIBS'] += ['EGL']



# main configuration function


def configure(conf):
	if not conf.options.platform in valid_platforms:
		conf.fatal('Invalid or no platform selected; run "./waf configure --help" for details about the --platform option')

	if not conf.options.device in valid_devices:
		conf.fatal('Invalid device selected; run "./waf configure --help" for details about the --device option')

	if (conf.options.device in needs_sysroot) and not (conf.options.sysroot):
		conf.fatal('Device %s needs --sysroot to be set' % conf.options.device)

	if (conf.options.device in needs_prefix) and not (conf.options.prefix):
		conf.fatal('Device %s needs --prefix to be set' % conf.options.device)


	conf.load('compiler_c')
	conf.load('compiler_cxx')

	# device specifics
	device_conf_func_name = "configure_%s_device" % conf.options.device
	try:
		device_conf_func = globals()[device_conf_func_name]
		device_conf_func(conf, conf.options.platform)
	except KeyError:
		conf.fatal("Failed to call function '%s'" % device_conf_func_name)

	for api in conf.env['WITH_APIS']:
		conf.env['WITH_' + api] = True
		conf.define('WITH_' + api, 1)

	if conf.env['WITH_GLES1'] or conf.env['WITH_GLES2'] or conf.env['WITH_OPENGL']:
		conf.env['PLATFORM_SOURCE'] += ['src/glapi_stats.cpp', 'src/process_glapi.cpp']
	if conf.env['WITH_OPENVG']:
		conf.env['PLATFORM_SOURCE'] += ['src/openvg_stats.cpp', 'src/process_openvg.cpp']

	conf.write_config_header()

	add_compiler_flags(conf, conf.env, ['-Wextra', '-Wall', '-Wno-unused-parameter', '-std=c99', '-pedantic'], 'C', 'CC')
	add_compiler_flags(conf, conf.env, ['-Wextra', '-Wall', '-std=c++98', '-pedantic'], 'CXX', 'CXX')

	variants_flags = [
		{'name':'debug', 'flags':['-O0', '-g3', '-ggdb']},
		{'name':'release', 'flags':['-O2']},
	]

	original_env = conf.env
	for variant_flags in variants_flags:
		conf.setenv(variant_flags['name'], env = original_env.derive())
		add_compiler_flags(conf, conf.env, variant_flags['flags'], 'C', 'CC')
		add_compiler_flags(conf, conf.env, variant_flags['flags'], 'CXX', 'CXX')

	def log_api(name, b):
		conf.start_msg(name)
		if b:
			conf.end_msg("yes", "GREEN")
		else:
			conf.end_msg("no", "YELLOW")

	Logs.pprint("NORMAL", "")
	conf.msg("Building for platform", conf.options.platform)
	conf.msg("Building for device"  , conf.options.device)
	Logs.pprint("NORMAL", "")
	Logs.pprint("NORMAL", "API support:")
	log_api("Desktop OpenGL", conf.env['WITH_OPENGL'])
	log_api("OpenGL ES 1.x", conf.env['WITH_GLES1'])
	log_api("OpenGL ES 2.x", conf.env['WITH_GLES2'])
	log_api("OpenVG", conf.env['WITH_OPENVG'])




####
## BUILDING


def build(bld):
	source = [ \
		"src/json_writer.cpp", \
		"src/log.cpp", \
		"src/main.cpp", \
		"src/process_egl.cpp", \
		"src/scopes.cpp", \
		"src/text_writer.cpp", \
		"src/json-sax/json.c", \
	]
	bld(
		features = ['cxx', 'cxxprogram'],
		uselib = bld.env['PLATFORM_USELIBS'],
		includes = ['.', '..', 'src'],
		target = executable_name,
		source = source + bld.env['PLATFORM_SOURCE']
	)




####
## INITIALIZATION


def init(ctx):
	from waflib.Build import BuildContext, CleanContext, InstallContext, UninstallContext

	for x in 'debug release'.split():
		for y in (BuildContext, CleanContext, InstallContext, UninstallContext):
			name = y.__name__.replace('Context','').lower()
			class tmp(y):
				cmd = name + '_' + x
				variant = x

	def buildall(ctx):
		import waflib.Options
		for x in ['debug', 'release']:
			waflib.Options.commands.insert(0, x)

	for y in (BuildContext, CleanContext, InstallContext, UninstallContext):
		class tmp(y):
			variant = 'release'

