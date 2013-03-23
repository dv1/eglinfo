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


#include "platform.hpp"


namespace eglinfo
{


native_window::native_window(native_display const &p_native_display, EGLint const p_visual_id)
	: m_native_display(p_native_display)
	, m_egl_native_window(0)
{
}


native_window::~native_window()
{
}


native_display::native_display(char const *)
	: m_egl_native_display(EGL_DEFAULT_DISPLAY)
{
}


native_display::~native_display()
{
}


bool native_display::is_ok() const
{
	return true;
}


} // namespace eglinfo end

