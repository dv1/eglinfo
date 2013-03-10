#ifndef EGLINFO_LOG_HPP
#define EGLINFO_LOG_HPP

#include <EGL/egl.h>
#include <iostream>


namespace eglinfo
{


char const *get_egl_error_string(EGLint p_error);


#define PRINT_EGL_ERROR(MSG) \
	{ \
		EGLint egl_error_num__ = eglGetError(); \
		if (egl_error_num__ != EGL_SUCCESS) \
		{ \
			std::cerr << MSG << get_egl_error_string(egl_error_num__) << "\n"; \
		} \
		else \
		{ \
			std::cerr << MSG << "\n"; \
		} \
	}


} // namespace eglinfo end


#endif

