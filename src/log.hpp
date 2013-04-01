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
			std::cerr << MSG << ": " << get_egl_error_string(egl_error_num__) << "\n"; \
		} \
		else \
		{ \
			std::cerr << MSG << "\n"; \
		} \
	}


} // namespace eglinfo end


#endif

