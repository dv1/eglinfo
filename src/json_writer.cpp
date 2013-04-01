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


}


void json_writer::json_print_base(int p_type, const char *p_data, uint32_t p_length)
{
	if (m_use_raw_print)
		json_print_raw(&m_printer, p_type, p_data, p_length);
	else
		json_print_pretty(&m_printer, p_type, p_data, p_length);
}


void json_writer::json_print_cstring(int const p_type, char const *p_string)
{
	json_print_base(p_type, p_string, std::strlen(p_string));
}


void json_writer::json_print(bool const p_value)
{
	json_print_base(p_value ? JSON_TRUE : JSON_FALSE, NULL, 0);
}


void json_writer::json_print(int const p_value)
{
	std::stringstream sstr;
	sstr << p_value;
	json_print_base(JSON_INT, sstr.str().c_str(), sstr.str().length());
}


void json_writer::json_print(char const *p_string)
{
	json_print_cstring(JSON_STRING, p_string);
}


void json_writer::json_print(std::string const &p_string)
{
	json_print_base(JSON_STRING, p_string.c_str(), p_string.length());
}


void json_writer::json_print(integers const &p_integers)
{
	json_print_base(JSON_ARRAY_BEGIN, NULL, 0);
	for (integers::const_iterator iter = p_integers.begin(); iter != p_integers.end(); ++iter)
		json_print(*iter);
	json_print_base(JSON_ARRAY_END, NULL, 0);
}


void json_writer::json_print_key(char const *p_key)
{
	json_print_cstring(JSON_KEY, p_key);
}


void json_writer::json_print_key(int const p_key)
{
	std::stringstream sstr;
	sstr << p_key;
	json_print_base(JSON_KEY, sstr.str().c_str(), sstr.str().length());
}


void json_writer::json_print_list(char const *p_list)
{
	std::stringstream sstr(p_list);
	json_print_base(JSON_ARRAY_BEGIN, NULL, 0);
	while (sstr)
	{
		std::string entry;
		sstr >> entry;
		if (!entry.empty())
			json_print(entry);
	}
	json_print_base(JSON_ARRAY_END, NULL, 0);
}


void json_writer::json_print_keylist(char const *p_key, char const *p_list)
{
	json_print_key(p_key);
	json_print_list(p_list);
}


void json_writer::json_print_keyarray(char const *p_key, EGLint const *p_array, int const p_size)
{
	json_print_key(p_key);
	json_print_base(JSON_ARRAY_BEGIN, NULL, 0);
	for (int i = 0; i < p_size; ++i)
		json_print(p_array[i]);
	json_print_base(JSON_ARRAY_END, NULL, 0);
}





json_writer::json_writer(std::ostream &p_out, bool const p_use_raw_print)
	: m_out(p_out)
	, m_ok(false)
	, m_use_raw_print(p_use_raw_print)
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
	json_print_base(JSON_OBJECT_BEGIN, NULL, 0);
}


void json_writer::end_write()
{
	json_print_base(JSON_OBJECT_END, NULL, 0);
	m_out << std::endl;
}


void json_writer::begin_api(EGLenum const, char const *p_api_name)
{
	json_print_key(p_api_name);
	json_print_base(JSON_OBJECT_BEGIN, NULL, 0);
}


void json_writer::end_api()
{
	json_print_base(JSON_OBJECT_END, NULL, 0);
}


void json_writer::next_api()
{
}


void json_writer::begin_egl_info()
{
	json_print_key("egl");
	json_print_base(JSON_OBJECT_BEGIN, NULL, 0);
}


void json_writer::end_egl_info()
{
	json_print_base(JSON_OBJECT_END, NULL, 0);
}


void json_writer::write_main_egl_info(
	  EGLint const p_major_version, EGLint const p_minor_version
	, char const *p_vendor
	, char const *p_version
	, char const *p_client_APIs
	, char const *p_extensions
)
{
	json_print_keyvalue("major_version", p_major_version);
	json_print_keyvalue("minor_version", p_minor_version);
	json_print_keyvalue("vendor", p_vendor);
	json_print_keyvalue("version", p_version);
	json_print_keylist("client_apis", p_client_APIs);
	json_print_keylist("extensions", p_extensions);
}


void json_writer::begin_write_egl_configs(EGLint const)
{
	json_print_key("configs");
	json_print_base(JSON_ARRAY_BEGIN, NULL, 0);
}


void json_writer::write_egl_config(egl_config const &p_egl_config)
{
	json_print_base(JSON_OBJECT_BEGIN, NULL, 0);

	json_print_keyvalue("index", p_egl_config.m_index);
	json_print_keyvalue("id", p_egl_config.m_id);
	json_print_keyvalue("level", p_egl_config.m_level);

	json_print_keyvalue("colorbuf_type", p_egl_config.m_colorbuf_type);
	json_print_keyvalue("colorbuf_size", p_egl_config.m_colorbuf_size);
	json_print_keyarray("colorbuf_rgba", p_egl_config.m_colorbuf_rgba, 4);

	json_print_keyvalue("alpha_mask_size", p_egl_config.m_alpha_mask_size);

	json_print_keyvalue("luminance_size", p_egl_config.m_luminance_size);

	json_print_keyvalue("depth_size", p_egl_config.m_depth_size);

	json_print_keyvalue("stencil_size", p_egl_config.m_stencil_size);

	json_print_keyvalue("multisample_samples", p_egl_config.m_multisample_samples);
	json_print_keyvalue("multisample_buffers", p_egl_config.m_multisample_buffers);

	json_print_keyvalue("visual_type", p_egl_config.m_visual_type);
	json_print_keyvalue("visual_id", p_egl_config.m_visual_id);

	json_print_keyvalue("surfaces", p_egl_config.m_surfaces);

	json_print_keyvalue("renderables", p_egl_config.m_renderables);
	json_print_keyvalue("conformance", p_egl_config.m_conformance);
	json_print_keyvalue("native_renderable", p_egl_config.m_native_renderable);

	json_print_keyvalue("transparent_type", p_egl_config.m_transparent_type);
	json_print_keyarray("transparent_rgb", p_egl_config.m_transparent_rgb, 3);

	json_print_keyvalue("caveat", p_egl_config.m_caveat);

	json_print_keyvalue("pbuffer_maxwidth", p_egl_config.m_pbuffer_maxwidth);
	json_print_keyvalue("pbuffer_maxheight", p_egl_config.m_pbuffer_maxheight);
	json_print_keyvalue("pbuffer_maxpixels", p_egl_config.m_pbuffer_maxpixels);

	json_print_keyvalue("min_swap_interval", p_egl_config.m_min_swap_interval);
	json_print_keyvalue("max_swap_interval", p_egl_config.m_max_swap_interval);

	json_print_keyvalue("bind_to_rgb_texture", p_egl_config.m_bind_to_rgb_texture);
	json_print_keyvalue("bind_to_rgba_texture", p_egl_config.m_bind_to_rgba_texture);

	json_print_base(JSON_OBJECT_END, NULL, 0);
}


void json_writer::end_write_egl_configs()
{
	json_print_base(JSON_ARRAY_END, NULL, 0);
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
	json_print_keyvalue("version", p_version);
	json_print_keyvalue("renderer", p_renderer);
	json_print_keylist("extensions", p_extensions);
}


void json_writer::write_glapi_stats(
	  EGLenum const
	, char const *
	, glapi_stats const &p_stats
)
{
	json_print_key("main_stats");
	json_print_base(JSON_OBJECT_BEGIN, NULL, 0);

	json_print_keyvalue("max_texture_size", p_stats.m_main_stats.m_max_texture_size);
	json_print_keyvalue("max_cubemap_texture_size", p_stats.m_main_stats.m_max_cubemap_texture_size);
	json_print_keyvalue("max_texture_image_units", p_stats.m_main_stats.m_max_texture_image_units);
	json_print_keyvalue("max_renderbuffer_size", p_stats.m_main_stats.m_max_renderbuffer_size);
	json_print_keyvalue("max_combined_texture_image_units", p_stats.m_main_stats.m_max_combined_texture_image_units);
	json_print_keyvalue("num_compressed_texture_formats", p_stats.m_main_stats.m_num_compressed_texture_formats);
	json_print_keyarray("aliased_line_width_range", p_stats.m_main_stats.m_aliased_line_width_range, 2);
	json_print_keyarray("aliased_point_size_range", p_stats.m_main_stats.m_aliased_point_size_range, 2);
	json_print_keyvalue("implementation_color_read_format", p_stats.m_main_stats.m_implementation_color_read_format);
	json_print_keyvalue("implementation_color_read_type", p_stats.m_main_stats.m_implementation_color_read_type);
	json_print_keyarray("max_viewport_dims", p_stats.m_main_stats.m_max_viewport_dims, 2);
	json_print_keyvalue("subpixel_bits", p_stats.m_main_stats.m_subpixel_bits);

	json_print_key("compressed_texture_formats");
	json_print_base(JSON_OBJECT_BEGIN, NULL, 0);
	for (integers::const_iterator iter = p_stats.m_main_stats.m_compressed_texture_formats.begin(); iter != p_stats.m_main_stats.m_compressed_texture_formats.end(); ++iter)
	{
		char const *str = get_compressed_texture_format_string(*iter);
		json_print_key(*iter);
		if (str != 0)
			json_print(str);
		else
			json_print_base(JSON_NULL, NULL, 0);
	}
	json_print_base(JSON_OBJECT_END, NULL, 0);

	json_print_base(JSON_OBJECT_END, NULL, 0);

	if (p_stats.m_shader_capable)
	{
		json_print_key("shader_stats");
		json_print_base(JSON_OBJECT_BEGIN, NULL, 0);

		json_print_keyvalue("max_vertex_attribs", p_stats.m_shader_stats.m_max_vertex_attribs);
		json_print_keyvalue("max_vertex_texture_image_units", p_stats.m_shader_stats.m_max_vertex_texture_image_units);
		json_print_keyvalue("num_program_binary_formats", p_stats.m_shader_stats.m_num_program_binary_formats);
		json_print_keyvalue("num_shader_binary_formats", p_stats.m_shader_stats.m_num_shader_binary_formats);
		json_print_keyvalue("max_varying_vectors", p_stats.m_shader_stats.m_max_varying_vectors);
		json_print_keyvalue("max_vertex_uniform_vectors", p_stats.m_shader_stats.m_max_vertex_uniform_vectors);
		json_print_keyvalue("max_fragment_uniform_vectors", p_stats.m_shader_stats.m_max_fragment_uniform_vectors);
		json_print_keyvalue("shader_compiler", p_stats.m_shader_stats.m_shader_compiler);

		json_print_key("program_binary_formats");
		json_print_base(JSON_OBJECT_BEGIN, NULL, 0);
		for (integers::const_iterator iter = p_stats.m_shader_stats.m_program_binary_formats.begin(); iter != p_stats.m_shader_stats.m_program_binary_formats.end(); ++iter)
		{
			char const *str = get_program_binary_format_string(*iter);
			json_print_key(*iter);
			if (str != 0)
				json_print(str);
			else
				json_print_base(JSON_NULL, NULL, 0);
		}
		json_print_base(JSON_OBJECT_END, NULL, 0);

		json_print_key("shader_binary_formats");
		json_print_base(JSON_OBJECT_BEGIN, NULL, 0);
		for (integers::const_iterator iter = p_stats.m_shader_stats.m_shader_binary_formats.begin(); iter != p_stats.m_shader_stats.m_shader_binary_formats.end(); ++iter)
		{
			char const *str = get_shader_binary_format_string(*iter);
			json_print_key(*iter);
			if (str != 0)
				json_print(str);
			else
				json_print_base(JSON_NULL, NULL, 0);
		}
		json_print_base(JSON_OBJECT_END, NULL, 0);

		json_print_base(JSON_OBJECT_END, NULL, 0);
	}
}


void json_writer::write_main_vg_info(
	  char const *p_vendor
	, char const *p_version
	, char const *p_renderer
	, char const *p_extensions
)
{
	json_print_keyvalue("vendor", p_vendor);
	json_print_keyvalue("version", p_version);
	json_print_keyvalue("renderer", p_renderer);
	json_print_keylist("extensions", p_extensions);
}


void json_writer::write_vg_stats(openvg_stats const &p_stats)
{
	json_print_key("main_stats");
	json_print_base(JSON_OBJECT_BEGIN, NULL, 0);

	json_print_keyvalue("max_scissor_rects", p_stats.m_max_scissor_rects);
	json_print_keyvalue("max_dash_count", p_stats.m_max_dash_count);
	json_print_keyvalue("max_kernel_size", p_stats.m_max_kernel_size);
	json_print_keyvalue("max_separable_kernel_size", p_stats.m_max_separable_kernel_size);
	json_print_keyvalue("max_color_ramp_stops", p_stats.m_max_color_ramp_stops);
	json_print_keyvalue("max_image_width", p_stats.m_max_image_width);
	json_print_keyvalue("max_image_height", p_stats.m_max_image_height);
	json_print_keyvalue("max_image_pixels", p_stats.m_max_image_pixels);
	json_print_keyvalue("max_image_bytes", p_stats.m_max_image_bytes);
	json_print_keyvalue("max_gaussian_std_deviation", p_stats.m_max_gaussian_std_deviation);

	json_print_base(JSON_OBJECT_END, NULL, 0);
}


void json_writer::begin_write_vg_image_format_acceleration()
{
	json_print_key("image_format_acceleration");
	json_print_base(JSON_OBJECT_BEGIN, NULL, 0);
}


void json_writer::write_vg_image_format_acceleration(
	  int const p_format
	, char const *p_name
	, bool const p_accelerated
)
{
	json_print_key(p_format);
	json_print_base(JSON_OBJECT_BEGIN, NULL, 0);
	json_print_keyvalue("name", p_name);
	json_print_keyvalue("accelerated", p_accelerated);
	json_print_base(JSON_OBJECT_END, NULL, 0);
}


void json_writer::end_write_vg_image_format_acceleration()
{
	json_print_base(JSON_OBJECT_END, NULL, 0);
}


void json_writer::write_vg_path_datatype_acceleration(
	  bool const p_signed8
	, bool const p_signed16
	, bool const p_signed32
	, bool const p_float
)
{
	json_print_key("path_datatype_acceleration");
	json_print_base(JSON_OBJECT_BEGIN, NULL, 0);
	json_print_keyvalue("s8", p_signed8);
	json_print_keyvalue("s16", p_signed16);
	json_print_keyvalue("s32", p_signed32);
	json_print_keyvalue("f", p_float);
	json_print_base(JSON_OBJECT_END, NULL, 0);
}


} // namespace eglinfo end

