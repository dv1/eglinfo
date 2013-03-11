#include <vector>

#include <EGL/egl.h>
#include <GLES2/gl2.h>

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



void print_vg_info(writer &p_vg_writer, scoped_egl_init const &p_egl)
{
	std::cout << "OpenVG information:\n";

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
	if (!eglChooseConfig(p_egl.get_display(), attribs, &config, 1, &num_configs) || (num_configs < 1))
	{
		PRINT_EGL_ERROR("Could not find config for OpenVG (perhaps this API is unsupported?)");
		return;
	}

	EGLint vid;
	if (!eglGetConfigAttrib(p_egl.get_display(), config, EGL_NATIVE_VISUAL_ID, &vid))
	{
		PRINT_EGL_ERROR("Could not get native visual ID from chosen config");
		return;
	}


	native_window window(p_egl.get_egl_native_display(), vid);

	eglBindAPI(EGL_OPENVG_API);

	scoped_context context(p_egl.get_display(), eglCreateContext(p_egl.get_display(), config, EGL_NO_CONTEXT, NULL));
	scoped_surface surface(p_egl.get_display(), eglCreateWindowSurface(p_egl.get_display(), config, window.get_egl_native_window(), NULL));

	if (!eglMakeCurrent(p_egl.get_display(), surface.get_surface(), surface.get_surface(), context.get_context()))
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

	eglMakeCurrent(p_egl.get_display(), EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
}


} // namespace eglinfo end

