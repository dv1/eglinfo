#include <config.h>
#include <vector>

#include <EGL/egl.h>
#include <GLES2/gl2.h>

#include "glapi_stats.hpp"
#include "process_glapi.hpp"

#include "log.hpp"
#include "platform.hpp"


#if defined (WITH_OPENGL) || defined(WITH_GLES2)
#define PROCESS_SHADER_STATS 1
#endif


namespace eglinfo
{


namespace
{


GLint get_gl_integer(GLenum const p_name)
{
	GLint i;
	glGetIntegerv(p_name, &i);
	return i;
}


}


bool process_glapi_info(
	  writer &p_glapi_writer
	, egl_scope const &p_egl_scope
	, char const *p_api_name
	, EGLenum const p_api
	, bool const p_shader_capable
	, EGLint const p_renderable_type
	, EGLint const p_client_version
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

	/*
	max texture size  glGetIntegerv(GL_MAX_TEXTURE_SIZE)
	max texture cube map  glGetIntegerv(GL_MAX_CUBE_MAP_TEXTURE_SIZE)
	max texture image units  glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS)
	read color format
	read color type
	max render buffer size  glGetIntegerv(GL_MAX_RENDERBUFFER_SIZE)
	max combined texture image units  glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS)
	(num) compressed texture formats  glGetIntegerv(GL_NUM_COMPRESSED_TEXTURE_FORMATS) & GL_COMPRESSED_TEXTURE_FORMATS
	max texture anisotropy  

	shader specific:
	max vertex attribs  glGetIntegerv(GL_MAX_VERTEX_ATTRIBS)
	max vertex texture image units  glGetIntegerv(GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS)
	max program binary formats  glGetIntegerv(GL_NUM_PROGRAM_BINARY_FORMATS)
	max shader binary formats  glGetIntegerv(GL_NUM_SHADER_BINARY_FORMATS)
	max varying vectors  glGetIntegerv(GL_MAX_VARYING_VECTORS)
	max vertex uniform vectors  glGetIntegerv(GL_MAX_VERTEX_UNIFORM_VECTORS)
	max fragment uniform vectors  glGetIntegerv(GL_MAX_FRAGMENT_UNIFORM_VECTORS)
	*/

	p_glapi_writer.write_main_glapi_info(
		  p_api
		, p_api_name
		, reinterpret_cast < char const * > (glGetString(GL_VERSION))
		, reinterpret_cast < char const * > (glGetString(GL_RENDERER))
		, reinterpret_cast < char const * > (glGetString(GL_EXTENSIONS))
	);

	glapi_stats stats;

	stats.m_main_stats.m_max_texture_size = get_gl_integer(GL_MAX_TEXTURE_SIZE);
	stats.m_main_stats.m_max_cubemap_texture_size = get_gl_integer(GL_MAX_CUBE_MAP_TEXTURE_SIZE);
	stats.m_main_stats.m_max_texture_image_units = get_gl_integer(GL_MAX_TEXTURE_IMAGE_UNITS);
	stats.m_main_stats.m_max_renderbuffer_size = get_gl_integer(GL_MAX_RENDERBUFFER_SIZE);
	stats.m_main_stats.m_max_combined_texture_image_units = get_gl_integer(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS);
	stats.m_main_stats.m_num_compressed_texture_formats = get_gl_integer(GL_NUM_COMPRESSED_TEXTURE_FORMATS);

#ifdef PROCESS_SHADER_STATS
	if (p_shader_capable)
	{
		stats.m_shader_capable = true;

		stats.m_shader_stats.m_max_vertex_attribs = get_gl_integer(GL_MAX_VERTEX_ATTRIBS);
		stats.m_shader_stats.m_max_vertex_texture_image_units = get_gl_integer(GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS);
		stats.m_shader_stats.m_num_shader_binary_formats = get_gl_integer(GL_NUM_SHADER_BINARY_FORMATS);
		stats.m_shader_stats.m_max_varying_vectors = get_gl_integer(GL_MAX_VARYING_VECTORS);
		stats.m_shader_stats.m_max_vertex_uniform_vectors = get_gl_integer(GL_MAX_VERTEX_UNIFORM_VECTORS);
		stats.m_shader_stats.m_max_fragment_uniform_vectors = get_gl_integer(GL_MAX_FRAGMENT_UNIFORM_VECTORS);
	}
#endif

	p_glapi_writer.write_glapi_stats(p_api, p_api_name, stats);

	eglMakeCurrent(p_egl_scope.get_display(), EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);

	return true;
}


} // namespace eglinfo end

