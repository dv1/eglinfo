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


void json_print_cstring(json_printer &p_printer, int const p_type, char const *p_string)
{
	json_print_pretty(&p_printer, p_type, p_string, std::strlen(p_string));
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


void json_print(json_printer &p_printer, char const *p_string)
{
	json_print_cstring(p_printer, JSON_STRING, p_string);
}


void json_print(json_printer &p_printer, std::string const &p_string)
{
	json_print_pretty(&p_printer, JSON_STRING, p_string.c_str(), p_string.length());
}


void json_print(json_printer &p_printer, integers const &p_integers)
{
	json_print_pretty(&p_printer, JSON_ARRAY_BEGIN, NULL, 0);
	for (integers::const_iterator iter = p_integers.begin(); iter != p_integers.end(); ++iter)
		json_print(p_printer, *iter);
	json_print_pretty(&p_printer, JSON_ARRAY_END, NULL, 0);
}




void json_print_key(json_printer &p_printer, char const *p_key)
{
	json_print_cstring(p_printer, JSON_KEY, p_key);
}


void json_print_key(json_printer &p_printer, int const p_key)
{
	std::stringstream sstr;
	sstr << p_key;
	json_print_pretty(&p_printer, JSON_KEY, sstr.str().c_str(), sstr.str().length());
}


void json_print_list(json_printer &p_printer, char const *p_list)
{
	std::stringstream sstr(p_list);
	json_print_pretty(&p_printer, JSON_ARRAY_BEGIN, NULL, 0);
	while (sstr)
	{
		std::string entry;
		sstr >> entry;
		if (!entry.empty())
			json_print(p_printer, entry);
	}
	json_print_pretty(&p_printer, JSON_ARRAY_END, NULL, 0);
}


template < typename T >
void json_print_keyvalue(json_printer &p_printer, char const *p_key, T const &p_value)
{
	json_print_key(p_printer, p_key);
	json_print(p_printer, p_value);
}


void json_print_keylist(json_printer &p_printer, char const *p_key, char const *p_list)
{
	json_print_key(p_printer, p_key);
	json_print_list(p_printer, p_list);
}


void json_print_keyarray(json_printer &p_printer, char const *p_key, EGLint const *p_array, int const p_size)
{
	json_print_key(p_printer, p_key);
	json_print_pretty(&p_printer, JSON_ARRAY_BEGIN, NULL, 0);
	for (int i = 0; i < p_size; ++i)
		json_print(p_printer, p_array[i]);
	json_print_pretty(&p_printer, JSON_ARRAY_END, NULL, 0);
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
	json_print_key(m_printer, p_api_name);
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
	json_print_key(m_printer, "egl");
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
	json_print_keyvalue(m_printer, "major_version", p_major_version);
	json_print_keyvalue(m_printer, "minor_version", p_minor_version);
	json_print_keyvalue(m_printer, "vendor", p_vendor);
	json_print_keyvalue(m_printer, "version", p_version);
	json_print_keylist(m_printer, "client_apis", p_client_APIs);
	json_print_keylist(m_printer, "extensions", p_extensions);
}


void json_writer::begin_write_egl_configs(EGLint const)
{
	json_print_key(m_printer, "configs");
	json_print_pretty(&m_printer, JSON_ARRAY_BEGIN, NULL, 0);
}


void json_writer::write_egl_config(egl_config const &p_egl_config)
{
	json_print_pretty(&m_printer, JSON_OBJECT_BEGIN, NULL, 0);

	json_print_keyvalue(m_printer, "index", p_egl_config.m_index);
	json_print_keyvalue(m_printer, "id", p_egl_config.m_id);
	json_print_keyvalue(m_printer, "level", p_egl_config.m_level);

	json_print_keyvalue(m_printer, "colorbuf_type", p_egl_config.m_colorbuf_type);
	json_print_keyvalue(m_printer, "colorbuf_size", p_egl_config.m_colorbuf_size);
	json_print_keyarray(m_printer, "colorbuf_rgba", p_egl_config.m_colorbuf_rgba, 4);

	json_print_keyvalue(m_printer, "alpha_mask_size", p_egl_config.m_alpha_mask_size);

	json_print_keyvalue(m_printer, "luminance_size", p_egl_config.m_luminance_size);

	json_print_keyvalue(m_printer, "depth_size", p_egl_config.m_depth_size);

	json_print_keyvalue(m_printer, "stencil_size", p_egl_config.m_stencil_size);

	json_print_keyvalue(m_printer, "multisample_samples", p_egl_config.m_multisample_samples);
	json_print_keyvalue(m_printer, "multisample_buffers", p_egl_config.m_multisample_buffers);

	json_print_keyvalue(m_printer, "visual_type", p_egl_config.m_visual_type);
	json_print_keyvalue(m_printer, "visual_id", p_egl_config.m_visual_id);

	json_print_keyvalue(m_printer, "surfaces", p_egl_config.m_surfaces);

	json_print_keyvalue(m_printer, "renderables", p_egl_config.m_renderables);
	json_print_keyvalue(m_printer, "conformance", p_egl_config.m_conformance);
	json_print_keyvalue(m_printer, "native_renderable", p_egl_config.m_native_renderable);

	json_print_keyvalue(m_printer, "transparent_type", p_egl_config.m_transparent_type);
	json_print_keyarray(m_printer, "transparent_rgb", p_egl_config.m_transparent_rgb, 3);

	json_print_keyvalue(m_printer, "caveat", p_egl_config.m_caveat);

	json_print_keyvalue(m_printer, "pbuffer_maxwidth", p_egl_config.m_pbuffer_maxwidth);
	json_print_keyvalue(m_printer, "pbuffer_maxheight", p_egl_config.m_pbuffer_maxheight);
	json_print_keyvalue(m_printer, "pbuffer_maxpixels", p_egl_config.m_pbuffer_maxpixels);

	json_print_keyvalue(m_printer, "min_swap_interval", p_egl_config.m_min_swap_interval);
	json_print_keyvalue(m_printer, "max_swap_interval", p_egl_config.m_max_swap_interval);

	json_print_keyvalue(m_printer, "bind_to_rgb_texture", p_egl_config.m_bind_to_rgb_texture);
	json_print_keyvalue(m_printer, "bind_to_rgba_texture", p_egl_config.m_bind_to_rgba_texture);

	json_print_pretty(&m_printer, JSON_OBJECT_END, NULL, 0);
}


void json_writer::end_write_egl_configs()
{
	json_print_pretty(&m_printer, JSON_ARRAY_END, NULL, 0);
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
	json_print_keyvalue(m_printer, "version", p_version);
	json_print_keyvalue(m_printer, "renderer", p_renderer);
	json_print_keylist(m_printer, "extensions", p_extensions);
}


void json_writer::write_glapi_stats(
	  EGLenum const
	, char const *
	, glapi_stats const &p_stats
)
{
	json_print_key(m_printer, "main_stats");
	json_print_pretty(&m_printer, JSON_OBJECT_BEGIN, NULL, 0);

	json_print_keyvalue(m_printer, "max_texture_size", p_stats.m_main_stats.m_max_texture_size);
	json_print_keyvalue(m_printer, "max_cubemap_texture_size", p_stats.m_main_stats.m_max_cubemap_texture_size);
	json_print_keyvalue(m_printer, "max_texture_image_units", p_stats.m_main_stats.m_max_texture_image_units);
	json_print_keyvalue(m_printer, "max_renderbuffer_size", p_stats.m_main_stats.m_max_renderbuffer_size);
	json_print_keyvalue(m_printer, "max_combined_texture_image_units", p_stats.m_main_stats.m_max_combined_texture_image_units);
	json_print_keyvalue(m_printer, "num_compressed_texture_formats", p_stats.m_main_stats.m_num_compressed_texture_formats);
	json_print_keyarray(m_printer, "aliased_line_width_range", p_stats.m_main_stats.m_aliased_line_width_range, 2);
	json_print_keyarray(m_printer, "aliased_point_size_range", p_stats.m_main_stats.m_aliased_point_size_range, 2);
	json_print_keyvalue(m_printer, "implementation_color_read_format", p_stats.m_main_stats.m_implementation_color_read_format);
	json_print_keyvalue(m_printer, "implementation_color_read_type", p_stats.m_main_stats.m_implementation_color_read_type);
	json_print_keyarray(m_printer, "max_viewport_dims", p_stats.m_main_stats.m_max_viewport_dims, 2);
	json_print_keyvalue(m_printer, "subpixel_bits", p_stats.m_main_stats.m_subpixel_bits);

	json_print_key(m_printer, "compressed_texture_formats");
	json_print_pretty(&m_printer, JSON_OBJECT_BEGIN, NULL, 0);
	for (integers::const_iterator iter = p_stats.m_main_stats.m_compressed_texture_formats.begin(); iter != p_stats.m_main_stats.m_compressed_texture_formats.end(); ++iter)
	{
		char const *str = get_compressed_texture_format_string(*iter);
		json_print_key(m_printer, *iter);
		if (str != 0)
			json_print(m_printer, str);
		else
			json_print_pretty(&m_printer, JSON_NULL, NULL, 0);
	}
	json_print_pretty(&m_printer, JSON_OBJECT_END, NULL, 0);

	json_print_pretty(&m_printer, JSON_OBJECT_END, NULL, 0);

	if (p_stats.m_shader_capable)
	{
		json_print_key(m_printer, "shader_stats");
		json_print_pretty(&m_printer, JSON_OBJECT_BEGIN, NULL, 0);

		json_print_keyvalue(m_printer, "max_vertex_attribs", p_stats.m_shader_stats.m_max_vertex_attribs);
		json_print_keyvalue(m_printer, "max_vertex_texture_image_units", p_stats.m_shader_stats.m_max_vertex_texture_image_units);
		json_print_keyvalue(m_printer, "num_shader_binary_formats", p_stats.m_shader_stats.m_num_shader_binary_formats);
		json_print_keyvalue(m_printer, "max_varying_vectors", p_stats.m_shader_stats.m_max_varying_vectors);
		json_print_keyvalue(m_printer, "max_vertex_uniform_vectors", p_stats.m_shader_stats.m_max_vertex_uniform_vectors);
		json_print_keyvalue(m_printer, "max_fragment_uniform_vectors", p_stats.m_shader_stats.m_max_fragment_uniform_vectors);
		json_print_keyvalue(m_printer, "shader_compiler", p_stats.m_shader_stats.m_shader_compiler);

		json_print_key(m_printer, "shader_binary_formats");
		json_print_pretty(&m_printer, JSON_OBJECT_BEGIN, NULL, 0);
		for (integers::const_iterator iter = p_stats.m_shader_stats.m_shader_binary_formats.begin(); iter != p_stats.m_shader_stats.m_shader_binary_formats.end(); ++iter)
		{
			char const *str = get_shader_binary_format_string(*iter);
			json_print_key(m_printer, *iter);
			if (str != 0)
				json_print(m_printer, str);
			else
				json_print_pretty(&m_printer, JSON_NULL, NULL, 0);
		}
		json_print_pretty(&m_printer, JSON_OBJECT_END, NULL, 0);

		json_print_pretty(&m_printer, JSON_OBJECT_END, NULL, 0);
	}
}


void json_writer::write_main_vg_info(
	  char const *p_vendor
	, char const *p_version
	, char const *p_renderer
	, char const *p_extensions
)
{
	json_print_keyvalue(m_printer, "vendor", p_vendor);
	json_print_keyvalue(m_printer, "version", p_version);
	json_print_keyvalue(m_printer, "renderer", p_renderer);
	json_print_keylist(m_printer, "extensions", p_extensions);
}


void json_writer::begin_write_vg_image_format_acceleration()
{
	json_print_key(m_printer, "image_format_acceleration");
	json_print_pretty(&m_printer, JSON_OBJECT_BEGIN, NULL, 0);
}


void json_writer::write_vg_image_format_acceleration(
	  int const p_format
	, char const *p_name
	, bool const p_accelerated
)
{
	json_print_key(m_printer, p_format);
	json_print_pretty(&m_printer, JSON_OBJECT_BEGIN, NULL, 0);
	json_print_keyvalue(m_printer, "name", p_name);
	json_print_keyvalue(m_printer, "accelerated", p_accelerated);
	json_print_pretty(&m_printer, JSON_OBJECT_END, NULL, 0);
}


void json_writer::end_write_vg_image_format_acceleration()
{
	json_print_pretty(&m_printer, JSON_OBJECT_END, NULL, 0);
}


void json_writer::write_vg_path_datatype_acceleration(
	  bool const p_signed8
	, bool const p_signed16
	, bool const p_signed32
	, bool const p_float
)
{
	json_print_key(m_printer, "path_datatype_acceleration");
	json_print_pretty(&m_printer, JSON_OBJECT_BEGIN, NULL, 0);
	json_print_keyvalue(m_printer, "s8", p_signed8);
	json_print_keyvalue(m_printer, "s16", p_signed16);
	json_print_keyvalue(m_printer, "s32", p_signed32);
	json_print_keyvalue(m_printer, "f", p_float);
	json_print_pretty(&m_printer, JSON_OBJECT_END, NULL, 0);
}


} // namespace eglinfo end

