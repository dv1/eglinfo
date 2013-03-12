#include <algorithm>
#include <iomanip>
#include <cstring>
#include "text_writer.hpp"


namespace eglinfo
{


namespace
{


char const * yesno(bool const b)
{
	return b ? "yes" : "no";
}


void print_list(char const *p_prefix, char const *p_list)
{
	if (p_list == NULL)
		return;

	char const *list_end = p_list + std::strlen(p_list);
	char const *iter = p_list;

	while (iter != list_end)
	{
		char const *new_iter = std::find(iter, list_end, ' ');

		if (new_iter != iter)
		{
			if (p_prefix != NULL)
				std::cout << p_prefix;
			for (char const *c = iter; c != new_iter; ++c)
				std::cout << *c;
			std::cout << "\n";
		}

		if (new_iter != list_end)
			++new_iter;

		iter = new_iter;
	}
}


char const * get_colorbuf_type_str(EGLint const p_colorbuf_type)
{
	switch (p_colorbuf_type)
	{
		case EGL_RGB_BUFFER: return "rgb"; break;
		case EGL_LUMINANCE_BUFFER: return "lum"; break;
		default: return "???";
	}
}


char const * get_transparent_type_str(EGLint const p_transparent_type)
{
	switch (p_transparent_type)
	{
		case EGL_TRANSPARENT_RGB: return "rgb"; break;
		case EGL_NONE: return "none"; break;
		default: return "???";
	}
}


void print_surfaces(std::ostream &p_out, EGLint const p_surfaces, std::string::size_type const p_padding)
{
	std::string str;
	if (p_surfaces & EGL_WINDOW_BIT)
		str += "win,";
	if (p_surfaces & EGL_PBUFFER_BIT)
		str += "pb,";
	if (p_surfaces & EGL_PIXMAP_BIT)
		str += "pix,";

	if (!str.empty())
		str.resize(str.length() - 1);

	if (str.length() < p_padding)
	{
		std::string::size_type oldlen = str.length();
		str.resize(p_padding);
		std::fill(str.begin() + oldlen, str.end(), ' ');
	}

	p_out << str;
}


void print_renderables(std::ostream &p_out, EGLint const p_renderables, std::string::size_type const p_padding)
{
	std::string str;
	if (p_renderables & EGL_OPENGL_BIT)
		str += "gl,";
	if (p_renderables & EGL_OPENGL_ES_BIT)
		str += "es,";
	if (p_renderables & EGL_OPENGL_ES2_BIT)
		str += "es2,";
	if (p_renderables & EGL_OPENVG_BIT)
		str += "vg,";

	if (!str.empty())
		str.resize(str.length() - 1);

	if (str.length() < p_padding)
	{
		std::string::size_type oldlen = str.length();
		str.resize(p_padding);
		std::fill(str.begin() + oldlen, str.end(), ' ');
	}

	p_out << str;
}


} // unnamed namespace end



text_writer::text_writer(std::ostream &p_out)
	: m_out(p_out)
{
}


void text_writer::begin_write()
{
}


void text_writer::end_write()
{
}


void text_writer::next_api()
{
	m_out << std::endl;
}


void text_writer::write_main_egl_info(
	  EGLint const p_major_version, EGLint const p_minor_version
	, char const *p_vendor
	, char const *p_version
	, char const *p_client_APIs
	, char const *p_extensions
)
{
	m_out
		<< "EGL information:\n"
		<< "    API version:    " << p_major_version << "." << p_minor_version << "\n"
		<< "    vendor string:  " << p_vendor << "\n"
		<< "    version string: " << p_version << "\n"
#ifdef EGL_VERSION_1_2
		<< "    client APIs:    " << p_client_APIs << "\n"
#endif
		<< "    extensions:\n"
		;

	print_list("      ", p_extensions);
}


void text_writer::begin_write_egl_configs(EGLint const p_num_configs)
{
	m_out << "   number of configurations: " << p_num_configs << "\n";
	m_out << "\n";

	m_out << "      #      ID   LEVEL   COLORBUFFER             DEPTH   STENCIL   MULTISAMPLE       VISUAL          SURFACES     RENDERABLES           TRANSPARENT\n";
	m_out << "                          type size  r  g  b  a   size    size      samples buffers   type   id                    apis         native   type  r  g  b\n";
}


void text_writer::write_egl_config(egl_config const &p_egl_config)
{
	m_out << std::setfill(' ') << std::dec;

	m_out << std::setw(7) << p_egl_config.m_index;

	m_out << std::setw(8) << p_egl_config.m_id;

	m_out << "   ";

	m_out << std::setw(5) << p_egl_config.m_level;

	m_out << "   ";

	m_out << std::setw(4) << get_colorbuf_type_str(p_egl_config.m_colorbuf_type);
	m_out << " ";
	m_out << std::setw(4) << p_egl_config.m_colorbuf_size;
	m_out << " ";
	m_out << std::setw(2) << p_egl_config.m_colorbuf_rgba[0];
	m_out << " ";
	m_out << std::setw(2) << p_egl_config.m_colorbuf_rgba[1];
	m_out << " ";
	m_out << std::setw(2) << p_egl_config.m_colorbuf_rgba[2];
	m_out << " ";
	m_out << std::setw(2) << p_egl_config.m_colorbuf_rgba[3];

	m_out << "   ";

	m_out << std::setw(5) << p_egl_config.m_depth_size;

	m_out << "   ";

	m_out << std::setw(7) << p_egl_config.m_stencil_size;

	m_out << "   ";

	m_out << std::setw(7) << p_egl_config.m_multisample_samples;
	m_out << " ";
	m_out << std::setw(7) << p_egl_config.m_multisample_buffers;

	m_out << "   ";

	m_out << "0x" << std::hex << std::setw(4) << std::setfill('0') << p_egl_config.m_visual_type;
	m_out << " ";
	m_out << "0x" << std::hex << std::setw(4) << std::setfill('0') << p_egl_config.m_visual_id;
	m_out << std::dec << std::setfill(' ');

	m_out << "   ";

	print_surfaces(m_out, p_egl_config.m_surfaces, 10);

	m_out << "   ";

	print_renderables(m_out, p_egl_config.m_renderables, 12);
	m_out << " ";
	m_out << std::setw(6) << yesno(p_egl_config.m_native_renderable);

	m_out << "   ";

	m_out << std::setw(4) << get_transparent_type_str(p_egl_config.m_transparent_type);
	m_out << " ";
	m_out << std::setw(2) << p_egl_config.m_transparent_rgb[0];
	m_out << " ";
	m_out << std::setw(2) << p_egl_config.m_transparent_rgb[1];
	m_out << " ";
	m_out << std::setw(2) << p_egl_config.m_transparent_rgb[2];

	m_out << "\n";
}


void text_writer::end_write_egl_configs()
{
}


void text_writer::write_no_egl_configs()
{
	m_out << "      No configurations found\n";
}


void text_writer::write_main_glapi_info(
	  EGLenum const
	, char const *p_api_name
	, char const *p_version
	, char const *p_renderer
	, char const *p_extensions
)
{
	m_out
		<< p_api_name << " information:\n"
		<< "    version string:  " << p_version << "\n"
		<< "    renderer string: " << p_renderer << "\n"
		<< "    extensions:\n"
		;

	print_list("      ", p_extensions);

}


void text_writer::write_main_vg_info(
	  char const *p_vendor
	, char const *p_version
	, char const *p_renderer
	, char const *p_extensions
)
{
	m_out << "OpenVG information:\n";

	std::cout
		<< "    vendor string:   " << p_vendor << "\n"
		<< "    version string:  " << p_version << "\n"
		<< "    renderer string: " << p_renderer << "\n"
		;

	std::cout
		<< "    extensions:\n"
		;
	print_list("      ", p_extensions);
}


void text_writer::begin_write_vg_image_format_acceleration()
{
	std::cout
		<< "    image format acceleration:\n"
		<< "      image format   accelerated\n"
		;
}


void text_writer::write_vg_image_format_acceleration(
	  int const
	, char const *p_name
	, bool const p_accelerated
)
{
	std::cout << std::setw(18) << p_name;
	std::cout << "   ";
	std::cout << std::setw(11) << yesno(p_accelerated);
	std::cout << "\n";
}


void text_writer::end_write_vg_image_format_acceleration()
{
}


void text_writer::write_vg_path_datatype_acceleration(
	  bool const p_signed8
	, bool const p_signed16
	, bool const p_signed32
	, bool const p_float
)
{
	std::cout
		<< "    path datatype acceleration:\n"
		<< "       s8  s16  s32    f\n"
		<< std::setw(9) << yesno(p_signed8)
		<< " "
		<< std::setw(4) << yesno(p_signed16)
		<< " "
		<< std::setw(4) << yesno(p_signed32)
		<< " "
		<< std::setw(4) << yesno(p_float)
		<< "\n"
		;
}


} // namespace eglinfo end

