#include <cstring>
#include <sstream>
#include "json_writer.hpp"


namespace eglinfo
{


namespace
{


int json_print_callback(void *p_data, char const *p_string, uint32_t const p_strlength)
{
	std::ostream *p_out = reinterpret_cast < std::ostream* > (p_data);
	p_out->write(p_string, p_strlength);
	return 0;
}


void json_print(json_printer &p_printer, bool const p_value)
{
	json_print_pretty(&p_printer, p_value ? JSON_TRUE : JSON_FALSE, NULL, 0);
}


void json_print(json_printer &p_printer, int const p_value)
{
	std::stringstream sstr;
	sstr << p_value;
	json_print_pretty(&p_printer, JSON_INT, sstr.str().c_str(), sstr.str().length());
}


void json_print(json_printer &p_printer, int const p_type, const char *p_string)
{
	json_print_pretty(&p_printer, p_type, p_string, std::strlen(p_string));
}


}


json_writer::json_writer(std::ostream &p_out)
	: m_out(p_out)
	, m_ok(false)
{
	if (json_print_init(&m_printer, json_print_callback, &m_out))
	{
		std::cerr << "Could not initialize JSON printer\n";
		return;
	}

	m_ok = true;
}


json_writer::~json_writer()
{
	if (m_ok)
		json_print_free(&m_printer);
}


void json_writer::begin_write()
{
	json_print_pretty(&m_printer, JSON_OBJECT_BEGIN, NULL, 0);
}


void json_writer::end_write()
{
	json_print_pretty(&m_printer, JSON_OBJECT_END, NULL, 0);
}


void json_writer::begin_api(EGLenum const, char const *p_api_name)
{
	json_print(m_printer, JSON_KEY, p_api_name);
	json_print_pretty(&m_printer, JSON_OBJECT_BEGIN, NULL, 0);
}


void json_writer::end_api()
{
	json_print_pretty(&m_printer, JSON_OBJECT_END, NULL, 0);
}


void json_writer::next_api()
{
}


void json_writer::begin_egl_info()
{
	json_print(m_printer, JSON_KEY, "egl");
	json_print_pretty(&m_printer, JSON_OBJECT_BEGIN, NULL, 0);
}


void json_writer::end_egl_info()
{
	json_print_pretty(&m_printer, JSON_OBJECT_END, NULL, 0);
}


void json_writer::write_main_egl_info(
	  EGLint const p_major_version, EGLint const p_minor_version
	, char const *p_vendor
	, char const *p_version
	, char const *p_client_APIs
	, char const *p_extensions
)
{
	json_print(m_printer, JSON_KEY, "major_version");
	json_print(m_printer, p_major_version);
	json_print(m_printer, JSON_KEY, "minor_version");
	json_print(m_printer, p_minor_version);
	json_print(m_printer, JSON_KEY, "vendor");
	json_print(m_printer, JSON_STRING, p_vendor);
	json_print(m_printer, JSON_KEY, "version");
	json_print(m_printer, JSON_STRING, p_version);
}


void json_writer::begin_write_egl_configs(EGLint const p_num_configs)
{
	json_print(m_printer, JSON_KEY, "configs");
	json_print_pretty(&m_printer, JSON_OBJECT_BEGIN, NULL, 0);
}


void json_writer::write_egl_config(egl_config const &p_egl_config)
{
}


void json_writer::end_write_egl_configs()
{
	json_print_pretty(&m_printer, JSON_OBJECT_END, NULL, 0);
}


void json_writer::write_no_egl_configs()
{
	begin_write_egl_configs(0);
	end_write_egl_configs();
}


void json_writer::write_main_glapi_info(
	  EGLenum const
	, char const *
	, char const *p_version
	, char const *p_renderer
	, char const *p_extensions
)
{
	json_print(m_printer, JSON_KEY, "version");
	json_print(m_printer, JSON_STRING, p_version);
	json_print(m_printer, JSON_KEY, "renderer");
	json_print(m_printer, JSON_STRING, p_renderer);
}


void json_writer::write_glapi_stats(
	  EGLenum const p_api
	, char const *p_api_name
	, glapi_stats const &p_stats
)
{
}


void json_writer::write_main_vg_info(
	  char const *p_vendor
	, char const *p_version
	, char const *p_renderer
	, char const *p_extensions
)
{
	json_print(m_printer, JSON_KEY, "vendor");
	json_print(m_printer, JSON_STRING, p_vendor);
	json_print(m_printer, JSON_KEY, "version");
	json_print(m_printer, JSON_STRING, p_version);
	json_print(m_printer, JSON_KEY, "renderer");
	json_print(m_printer, JSON_STRING, p_renderer);
}


void json_writer::begin_write_vg_image_format_acceleration()
{
}


void json_writer::write_vg_image_format_acceleration(
	  int const p_format
	, char const *p_name
	, bool const p_accelerated
)
{
}


void json_writer::end_write_vg_image_format_acceleration()
{
}


void json_writer::write_vg_path_datatype_acceleration(
	  bool const p_signed8
	, bool const p_signed16
	, bool const p_signed32
	, bool const p_float
)
{
}


} // namespace eglinfo end

