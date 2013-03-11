#include <vector>

#include <EGL/egl.h>
#include <GLES2/gl2.h>

#include "process_egl.hpp"
#include "log.hpp"
#include "platform.hpp"
#include "egl_config.hpp"


namespace eglinfo
{


bool process_egl_info(writer &p_egl_writer, egl_scope const &p_egl_scope)
{
	p_egl_writer.write_main_egl_info(
		  p_egl_scope.get_major_version(), p_egl_scope.get_minor_version()
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

		p_egl_writer.begin_write_egl_configs(num_configs);

		for (EGLint i = 0; i < num_configs; ++i)
		{
			egl_config config;
			config.m_index = i;

			eglGetConfigAttrib(d, config_ids[i], EGL_CONFIG_ID,                &config.m_id);
			eglGetConfigAttrib(d, config_ids[i], EGL_LEVEL,                    &config.m_level);
			
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

			eglGetConfigAttrib(d, config_ids[i], EGL_CONFIG_CAVEAT,            &config.m_caveat);

			eglGetConfigAttrib(d, config_ids[i], EGL_MAX_PBUFFER_WIDTH,        &config.m_pbuffer_maxwidth);
			eglGetConfigAttrib(d, config_ids[i], EGL_MAX_PBUFFER_HEIGHT,       &config.m_pbuffer_maxheight);
			eglGetConfigAttrib(d, config_ids[i], EGL_MAX_PBUFFER_PIXELS,       &config.m_pbuffer_maxpixels);

			eglGetConfigAttrib(d, config_ids[i], EGL_MIN_SWAP_INTERVAL,        &config.m_min_swap_interval);
			eglGetConfigAttrib(d, config_ids[i], EGL_MAX_SWAP_INTERVAL,        &config.m_max_swap_interval);
			
			eglGetConfigAttrib(d, config_ids[i], EGL_BIND_TO_TEXTURE_RGB,      &config.m_bind_to_rgb_texture);
			eglGetConfigAttrib(d, config_ids[i], EGL_BIND_TO_TEXTURE_RGBA,     &config.m_bind_to_rgba_texture);

			p_egl_writer.write_egl_config(config);
		}

		p_egl_writer.end_write_egl_configs();
	}
	else
		p_egl_writer.write_no_egl_configs();

	return true;
}


} // namespace eglinfo end

