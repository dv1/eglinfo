#ifndef EGLINFO_PROCESS_EGL_HPP
#define EGLINFO_PROCESS_EGL_HPP

#include <vector>
#include "egl_scope.hpp"
#include "platform.hpp"
#include "scoped_resources.hpp"


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

		, m_caveats

		, m_pbuffer_maxwidth
		, m_pbuffer_maxheight
		, m_pbuffer_maxpixels

		, m_min_swap_interval
		, m_max_swap_interval

		, m_bind_to_rgb_texture
		, m_bind_to_rgba_texture

		;
};


template < typename EGLWriter >
bool process_egl_info(EGLWriter &p_egl_writer, egl_scope const &p_egl_scope)
{
	write_main_egl_info(
		  p_egl_writer
		, p_egl_scope.get_major_version(), p_egl_scope.get_minor_version()
		, eglQueryString(p_egl_scope.get_display(), EGL_VENDOR)
		, eglQueryString(p_egl_scope.get_display(), EGL_VERSION)
		, eglQueryString(p_egl_scope.get_display(), EGL_CLIENT_APIS)
		, eglQueryString(p_egl_scope.get_display(), EGL_EXTENSIONS)
	);

	EGLint num_configs;
	if (!eglGetConfigs(p_egl_scope.get_display(), 0, 0, &num_configs) && (num_configs > 0))
	{
		PRINT_EGL_ERROR("Could not retrieve the number of EGL configurations");
		return false;
	}

	typedef std::vector < EGLConfig > egl_config_ids;

	egl_config_ids config_ids(num_configs);
	EGLint final_num_configs;
	if (!eglGetConfigs(p_egl_scope.get_display(), &config_ids[0], num_configs, &final_num_configs))
	{
		PRINT_EGL_ERROR("Could not retrieve the EGL configurations");
		return false;
	}
	num_configs = final_num_configs;

	if (num_configs > 0)
	{
		EGLDisplay d = p_egl_scope.get_display();

		begin_write_egl_configs(p_egl_writer);

		for (EGLint i = 0; i < num_configs; ++i)
		{
			egl_config config;
			config.m_index = i;

			eglGetConfigAttrib(d, config_ids[i], EGL_CONFIG_ID,                &config.m_id);
			eglGetConfigAttrib(d, config_ids[i], EGL_LEVEL,                    &config.m_framebuf_level);
			
			eglGetConfigAttrib(d, config_ids[i], EGL_COLOR_BUFFER_TYPE,        &config.m_colorbuf_type);
			eglGetConfigAttrib(d, config_ids[i], EGL_BUFFER_SIZE,              &config.m_colorbuf_size);
			eglGetConfigAttrib(d, config_ids[i], EGL_RED_SIZE,                 &config.m_colorbuf_rgba[0]);
			eglGetConfigAttrib(d, config_ids[i], EGL_GREEN_SIZE,               &config.m_colorbuf_rgba[1]);
			eglGetConfigAttrib(d, config_ids[i], EGL_BLUE_SIZE,                &config.m_colorbuf_rgba[2]);
			eglGetConfigAttrib(d, config_ids[i], EGL_ALPHA_SIZE,               &config.m_colorbuf_rgba[3]);

			eglGetConfigAttrib(d, config_ids[i], EGL_ALPHA_MASK_SIZE,          &config.m_alpha_mask_size);

			eglGetConfigAttrib(d, config_ids[i], EGL_LUMINANCE_SIZE,           &config.m_luminance_size);

			eglGetConfigAttrib(d, config_ids[i], EGL_DEPTH_SIZE,               &config.m_depth_size);

			eglGetConfigAttrib(d, config_ids[i], EGL_STENCIL_SIZE,             &config.m_stencil_size);

			eglGetConfigAttrib(d, config_ids[i], EGL_SAMPLES,                  &config.m_multisample_samples);
			eglGetConfigAttrib(d, config_ids[i], EGL_SAMPLE_BUFFERS,           &config.m_multisample_buffers);

			eglGetConfigAttrib(d, config_ids[i], EGL_NATIVE_VISUAL_TYPE,       &config.m_visual_type);
			eglGetConfigAttrib(d, config_ids[i], EGL_NATIVE_VISUAL_ID,         &config.m_visual_id);

			eglGetConfigAttrib(d, config_ids[i], EGL_SURFACE_TYPE,             &config.m_surfaces);

			eglGetConfigAttrib(d, config_ids[i], EGL_RENDERABLE_TYPE,          &config.m_renderables);
			eglGetConfigAttrib(d, config_ids[i], EGL_CONFORMANT,               &config.m_conformance);
			eglGetConfigAttrib(d, config_ids[i], EGL_NATIVE_RENDERABLE,        &config.m_native_renderable);

			eglGetConfigAttrib(d, config_ids[i], EGL_TRANSPARENT_TYPE,         &config.m_transparent_type);
			eglGetConfigAttrib(d, config_ids[i], EGL_TRANSPARENT_RED_VALUE,    &config.m_transparent_rgb[0]);
			eglGetConfigAttrib(d, config_ids[i], EGL_TRANSPARENT_GREEN_VALUE,  &config.m_transparent_rgb[1]);
			eglGetConfigAttrib(d, config_ids[i], EGL_TRANSPARENT_BLUE_VALUE,   &config.m_transparent_rgb[2]);

			eglGetConfigAttrib(d, config_ids[i], EGL_CONFIG_CAVEATS,           &config.m_caveats);

			eglGetConfigAttrib(d, config_ids[i], EGL_PBUFFER_MAXWIDTH,         &config.m_pbuffer_maxwidth);
			eglGetConfigAttrib(d, config_ids[i], EGL_PBUFFER_MAXHEIGHT,        &config.m_pbuffer_maxheight);
			eglGetConfigAttrib(d, config_ids[i], EGL_PBUFFER_MAXPIXELS,        &config.m_pbuffer_maxpixels);

			eglGetConfigAttrib(d, config_ids[i], EGL_MIN_SWAP_INTERVAL,        &config.m_min_swap_interval);
			eglGetConfigAttrib(d, config_ids[i], EGL_MAX_SWAP_INTERVAL,        &config.m_max_swap_interval);
			
			eglGetConfigAttrib(d, config_ids[i], EGL_BIND_TO_TEXTURE_RGB,      &config.m_bind_to_rgb_texture);
			eglGetConfigAttrib(d, config_ids[i], EGL_BIND_TO_TEXTURE_RGBA,     &config.m_bind_to_rgba_texture);

			write_egl_config(p_egl_writer, config);
		}

		end_write_egl_configs(p_egl_writer);
	}
	else
		write_no_egl_configs(p_egl_writer);

	return true;
}


template < typename GLAPIWriter >
bool process_glapi_info(
	  GLAPIWriter &p_glapi_writer
	, egl_scope const &p_egl_scope
	, char const *p_api_name
	, EGLenum const p_api
	, EGLint const p_renderable_type
	, EGLint const p_client_version = -1

)
{
	EGLint attribs[] =
	{
		EGL_RENDERABLE_TYPE, p_renderable_type,
		EGL_RED_SIZE, 1,
		EGL_GREEN_SIZE, 1,
		EGL_BLUE_SIZE, 1,
		EGL_NONE
	};
	EGLint ctx_attribs[] =
	{
		EGL_CONTEXT_CLIENT_VERSION, p_client_version,
		EGL_NONE
	};

	if (p_client_version == -1)
		ctx_attribs[0] = EGL_NONE;

	EGLint num_configs;
	EGLConfig config;
	if (!eglChooseConfig(p_egl_scope.get_display(), attribs, &config, 1, &num_configs) || (num_configs < 1))
	{
		PRINT_EGL_ERROR("Could not find config for " << p_api_name << " (perhaps this API is unsupported?)");
		return false;
	}

	EGLint vid;
	if (!eglGetConfigAttrib(p_egl_scope.get_display(), config, EGL_NATIVE_VISUAL_ID, &vid))
	{
		PRINT_EGL_ERROR("Could not get native visual ID from chosen config");
		return false;
	}


	native_window window(p_egl_scope.get_egl_native_display(), vid);

	eglBindAPI(p_api);

	scoped_context context(p_egl_scope.get_display(), eglCreateContext(p_egl_scope.get_display(), config, EGL_NO_CONTEXT, ctx_attribs));
	scoped_surface surface(p_egl_scope.get_display(), eglCreateWindowSurface(p_egl_scope.get_display(), config, window.get_egl_native_window(), NULL));

	if (!eglMakeCurrent(p_egl_scope.get_display(), surface.get_surface(), surface.get_surface(), context.get_context()))
	{
		PRINT_EGL_ERROR("eglMakeCurrent() failed");
		return false;
	}

	write_glapi_info(
		  p_glapi_writer
		, p_api
		, reinterpret_cast < char const * > (glGetString(GL_VERSION))
		, reinterpret_cast < char const * > (glGetString(GL_RENDERER))
		, reinterpret_cast < char const * > (glGetString(GL_EXTENSIONS))
	);

	eglMakeCurrent(p_egl_scope.get_display(), EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);

	return true;
}


} // namespace eglinfo end


#endif
