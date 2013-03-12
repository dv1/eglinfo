#ifndef EGLINFO_GLAPI_STATS_HPP
#define EGLINFO_GLAPI_STATS_HPP

#include <EGL/egl.h>
#include <GLES2/gl2.h>


namespace eglinfo
{


struct main_glapi_stats
{
	GLint
		  m_max_texture_size
		, m_max_cubemap_texture_size
		, m_max_texture_image_units
		, m_max_renderbuffer_size
		, m_max_combined_texture_image_units
		, m_num_compressed_texture_formats
		;

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

	shader_glapi_stats();
};


struct glapi_stats
{
	main_glapi_stats m_main_stats;

	bool m_shader_capable;
	shader_glapi_stats m_shader_stats;

	glapi_stats();
};


} // namespace eglinfo end


#endif
