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

