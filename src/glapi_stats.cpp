#include "glapi_stats.hpp"


namespace eglinfo
{


main_glapi_stats::main_glapi_stats()
	: m_max_texture_size(0)
	, m_max_cubemap_texture_size(0)
	, m_max_texture_image_units(0)
	, m_max_renderbuffer_size(0)
	, m_max_combined_texture_image_units(0)
	, m_num_compressed_texture_formats(0)
{
}


shader_glapi_stats::shader_glapi_stats()
	: m_max_vertex_attribs(0)
	, m_max_vertex_texture_image_units(0)
	, m_num_shader_binary_formats(0)
	, m_max_varying_vectors(0)
	, m_max_vertex_uniform_vectors(0)
	, m_max_fragment_uniform_vectors(0)
{
}


glapi_stats::glapi_stats()
	: m_shader_capable(false)
{
}


} // namespace eglinfo end

