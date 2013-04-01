/*

eglinfo EGL information utility

Copyright (c) 2013 Carlos Rafael Giani

This software is provided 'as-is', without any express or implied
warranty. In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

   1. The origin of this software must not be misrepresented; you must not
   claim that you wrote the original software. If you use this software
   in a product, an acknowledgment in the product documentation would be
   appreciated but is not required.

   2. Altered source versions must be plainly marked as such, and must not be
   misrepresented as being the original software.

   3. This notice may not be removed or altered from any source
   distribution.

*/


#include <vector>

#include <EGL/egl.h>
#include <VG/openvg.h>

#include "process_openvg.hpp"

#include "log.hpp"
#include "platform.hpp"


namespace eglinfo
{


namespace
{


char const * get_path_datatype_acceleration(VGPathDatatype const p_datatype)
{
	VGHardwareQueryResult acceleration = vgHardwareQuery(VG_PATH_DATATYPE_QUERY, p_datatype);
	return (acceleration == VG_HARDWARE_ACCELERATED) ? "yes" : "no";
}

struct image_format_accel_entry
{
	VGImageFormat m_format;
	char const *m_name;
};

image_format_accel_entry const image_format_accel_table[] =
{
	/* RGB{A,X} channel ordering */
	{ VG_sRGBX_8888 , "sRGBX_8888" },
	{ VG_sRGBA_8888 , "sRGBA_8888" },
	{ VG_sRGBA_8888_PRE , "sRGBA_8888_PRE" },
	{ VG_sRGB_565 , "sRGB_565" },
	{ VG_sRGBA_5551 , "sRGBA_5551" },
	{ VG_sRGBA_4444 , "sRGBA_4444" },
	{ VG_sL_8 , "sL_8" },
	{ VG_lRGBX_8888 , "lRGBX_8888" },
	{ VG_lRGBA_8888 , "lRGBA_8888" },
	{ VG_lRGBA_8888_PRE , "lRGBA_8888_PRE" },
	{ VG_lL_8 , "lL_8" },
	{ VG_A_8 , "A_8" },
	{ VG_BW_1 , "BW_1" },

	/* {A,X}RGB channel ordering */
	{ VG_sXRGB_8888 , "sXRGB_8888" },
	{ VG_sARGB_8888 , "sARGB_8888" },
	{ VG_sARGB_8888_PRE , "sARGB_8888_PRE" },
	{ VG_sARGB_1555 , "sARGB_1555" },
	{ VG_sARGB_4444 , "sARGB_4444" },
	{ VG_lXRGB_8888 , "lXRGB_8888" },
	{ VG_lARGB_8888 , "lARGB_8888" },
	{ VG_lARGB_8888_PRE , "lARGB_8888_PRE" },

	/* BGR{A,X} channel ordering */
	{ VG_sBGRX_8888 , "sBGRX_8888" },
	{ VG_sBGRA_8888 , "sBGRA_8888" },
	{ VG_sBGRA_8888_PRE , "sBGRA_8888_PRE" },
	{ VG_sBGR_565 , "sBGR_565" },
	{ VG_sBGRA_5551 , "sBGRA_5551" },
	{ VG_sBGRA_4444 , "sBGRA_4444" },
	{ VG_lBGRX_8888 , "lBGRX_8888" },
	{ VG_lBGRA_8888 , "lBGRA_8888" },
	{ VG_lBGRA_8888_PRE , "lBGRA_8888_PRE" },

	/* {A,X}BGR channel ordering */
	{ VG_sXBGR_8888 , "sXBGR_8888" },
	{ VG_sABGR_8888 , "sABGR_8888" },
	{ VG_sABGR_8888_PRE , "sABGR_8888_PRE" },
	{ VG_sABGR_1555 , "sABGR_1555" },
	{ VG_sABGR_4444 , "sABGR_4444" },
	{ VG_lXBGR_8888 , "lXBGR_8888" },
	{ VG_lABGR_8888 , "lABGR_8888" },
	{ VG_lABGR_8888_PRE , "lABGR_8888_PRE" },

	{ VGImageFormat(0), NULL }
};


} // unnamed namespace end



void process_vg_info(writer &p_vg_writer, egl_scope const &p_egl_scope)
{
	EGLint attribs[] =
	{
		EGL_RENDERABLE_TYPE, EGL_OPENVG_BIT,
		EGL_RED_SIZE, 1,
		EGL_GREEN_SIZE, 1,
		EGL_BLUE_SIZE, 1,
		EGL_NONE
	};

	EGLint num_configs;
	EGLConfig config;
	if (!eglChooseConfig(p_egl_scope.get_display(), attribs, &config, 1, &num_configs) || (num_configs < 1))
	{
		PRINT_EGL_ERROR("Could not find config for OpenVG (perhaps this API is unsupported?)");
		return;
	}

	EGLint vid;
	if (!eglGetConfigAttrib(p_egl_scope.get_display(), config, EGL_NATIVE_VISUAL_ID, &vid))
	{
		PRINT_EGL_ERROR("Could not get native visual ID from chosen config");
		return;
	}


	native_window window(p_egl_scope.get_native_display(), vid);

	eglBindAPI(EGL_OPENVG_API);

	scoped_context context(p_egl_scope.get_display(), eglCreateContext(p_egl_scope.get_display(), config, EGL_NO_CONTEXT, NULL));
	scoped_surface surface(p_egl_scope.get_display(), eglCreateWindowSurface(p_egl_scope.get_display(), config, window.get_egl_native_window(), NULL));

	if (!eglMakeCurrent(p_egl_scope.get_display(), surface.get_surface(), surface.get_surface(), context.get_context()))
	{
		PRINT_EGL_ERROR("eglMakeCurrent() failed");
		return;
	}

	p_vg_writer.write_main_vg_info(
		  reinterpret_cast < char const * > (vgGetString(VG_VENDOR))
		, reinterpret_cast < char const * > (vgGetString(VG_VERSION))
		, reinterpret_cast < char const * > (vgGetString(VG_RENDERER))
		, reinterpret_cast < char const * > (vgGetString(VG_EXTENSIONS))
	);

	openvg_stats stats;
	stats.m_max_scissor_rects          = vgGeti(VG_MAX_SCISSOR_RECTS);
	stats.m_max_dash_count             = vgGeti(VG_MAX_DASH_COUNT);
	stats.m_max_kernel_size            = vgGeti(VG_MAX_KERNEL_SIZE);
	stats.m_max_separable_kernel_size  = vgGeti(VG_MAX_SEPARABLE_KERNEL_SIZE);
	stats.m_max_color_ramp_stops       = vgGeti(VG_MAX_COLOR_RAMP_STOPS);
	stats.m_max_image_width            = vgGeti(VG_MAX_IMAGE_WIDTH);
	stats.m_max_image_height           = vgGeti(VG_MAX_IMAGE_HEIGHT);
	stats.m_max_image_pixels           = vgGeti(VG_MAX_IMAGE_PIXELS);
	stats.m_max_image_bytes            = vgGeti(VG_MAX_IMAGE_BYTES);
	stats.m_max_gaussian_std_deviation = vgGeti(VG_MAX_GAUSSIAN_STD_DEVIATION);

	p_vg_writer.write_vg_stats(stats);

	p_vg_writer.begin_write_vg_image_format_acceleration();
	for (image_format_accel_entry const *accel_entry = image_format_accel_table; accel_entry->m_name != NULL; ++accel_entry)
	{
		VGHardwareQueryResult acceleration = vgHardwareQuery(VG_IMAGE_FORMAT_QUERY, accel_entry->m_format);
		p_vg_writer.write_vg_image_format_acceleration(accel_entry->m_format, accel_entry->m_name, (acceleration == VG_HARDWARE_ACCELERATED));
	}
	p_vg_writer.end_write_vg_image_format_acceleration();

	p_vg_writer.write_vg_path_datatype_acceleration(
		  get_path_datatype_acceleration(VG_PATH_DATATYPE_S_8)
		, get_path_datatype_acceleration(VG_PATH_DATATYPE_S_16)
		, get_path_datatype_acceleration(VG_PATH_DATATYPE_S_32)
		, get_path_datatype_acceleration(VG_PATH_DATATYPE_F)
	);

	eglMakeCurrent(p_egl_scope.get_display(), EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
}


} // namespace eglinfo end

