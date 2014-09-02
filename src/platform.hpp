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


#ifndef EGLINFO_PLATFORM_HPP
#define EGLINFO_PLATFORM_HPP

#include <EGL/egl.h>


namespace eglinfo
{


class native_display;


class native_window
{
public:
	struct internals;


	native_window(native_display const &p_native_display, EGLint const p_visual_id);
	~native_window();


	EGLNativeDisplayType get_egl_native_display() const
	{
		return m_egl_native_display;
	}


	EGLNativeWindowType get_egl_native_window() const
	{
		return m_egl_native_window;
	}


private:
	native_display const &m_native_display;

	EGLNativeDisplayType m_egl_native_display;
	EGLNativeWindowType m_egl_native_window;

	internals *m_internals;
};


class native_display
{
public:
	struct internals;

	explicit native_display(char const *p_name);
	~native_display();

	EGLNativeDisplayType get_egl_native_display() const
	{
		return m_egl_native_display;
	}

	bool is_ok() const;

	internals* get_internals() const
	{
		return m_internals;
	}

private:
	EGLNativeDisplayType m_egl_native_display;

	internals *m_internals;
};


} // namespace eglinfo end


#endif

