#ifndef EGLINFO_EGL_CONFIG_HPP
#define EGLINFO_EGL_CONFIG_HPP

#include <EGL/egl.h>


namespace eglinfo
{


struct egl_config
{
	EGLint
		  m_index
		, m_id
		, m_level

		, m_colorbuf_type
		, m_colorbuf_size
		, m_colorbuf_rgba[4]

		, m_alpha_mask_size

		, m_luminance_size

		, m_depth_size

		, m_stencil_size

		, m_multisample_samples
		, m_multisample_buffers

		, m_visual_type
		, m_visual_id

		, m_surfaces

		, m_renderables
		, m_conformance
		, m_native_renderable

		, m_transparent_type
		, m_transparent_rgb[3]

		, m_caveat

		, m_pbuffer_maxwidth
		, m_pbuffer_maxheight
		, m_pbuffer_maxpixels

		, m_min_swap_interval
		, m_max_swap_interval

		, m_bind_to_rgb_texture
		, m_bind_to_rgba_texture

		;
};


} // namespace eglinfo end


#endif

