#ifndef EGLINFO_GLAPI_STATS_HPP
#define EGLINFO_GLAPI_STATS_HPP

#include <vector>
#include <EGL/egl.h>
#include "opengl.hpp"


namespace eglinfo
{


typedef std::vector < GLint > integers;


struct main_glapi_stats
{
	GLint
		  m_max_texture_size
		, m_max_cubemap_texture_size
		, m_max_texture_image_units
		, m_max_renderbuffer_size
		, m_max_combined_texture_image_units
		, m_num_compressed_texture_formats
		, m_aliased_line_width_range[2]
		, m_aliased_point_size_range[2]
		, m_implementation_color_read_format
		, m_implementation_color_read_type
		, m_max_viewport_dims[2]
		, m_subpixel_bits
		;

	integers m_compressed_texture_formats;

	main_glapi_stats();
};


struct shader_glapi_stats
{
	GLint
		  m_max_vertex_attribs
		, m_max_vertex_texture_image_units
		, m_num_shader_binary_formats
		, m_max_varying_vectors
		, m_max_vertex_uniform_vectors
		, m_max_fragment_uniform_vectors
		;

	bool m_shader_compiler;

	integers m_shader_binary_formats;

	shader_glapi_stats();
};


struct glapi_stats
{
	main_glapi_stats m_main_stats;

	bool m_shader_capable;
	shader_glapi_stats m_shader_stats;

	glapi_stats();
};


char const * get_compressed_texture_format_string(GLint const p_format);
char const * get_shader_binary_format_string(GLint const p_format);
char const * get_color_format_string(GLint const p_format);
char const * get_color_type_string(GLint const p_type);


} // namespace eglinfo end


#endif
