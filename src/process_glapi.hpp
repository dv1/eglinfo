#ifndef EGLINFO_PROCESS_GLAPI_HPP
#define EGLINFO_PROCESS_GLAPI_HPP

#include <EGL/egl.h>
#include "scopes.hpp"
#include "writer.hpp"


namespace eglinfo
{


bool process_glapi_info(
	  writer &p_glapi_writer
	, egl_scope const &p_egl_scope
	, char const *p_api_name
	, EGLenum const p_api
	, EGLint const p_renderable_type
	, EGLint const p_client_version = -1
);


} // namespace eglinfo end


#endif

