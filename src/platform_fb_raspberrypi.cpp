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
#include <bcm_host.h>


namespace eglinfo
{


struct native_display::internals
{
	DISPMANX_DISPLAY_HANDLE_T m_dispman_display;

	internals()	
		: m_dispman_display(0)
	{
	}
};


native_display::native_display(char const *)
	: m_egl_native_display(EGL_DEFAULT_DISPLAY)
{
	m_internals = new internals;
	m_internals->m_dispman_display = vc_dispmanx_display_open(0/* LCD */);
}


native_display::~native_display()
{
	vc_dispmanx_display_close(m_internals->m_dispman_display);
	delete m_internals;
}


bool native_display::is_ok() const
{
	return true;
}




native_window::native_window(native_display const &p_native_display, EGLint const)
	: m_native_display(p_native_display)
	, m_egl_native_display(p_native_display.get_egl_native_display())
	, m_egl_native_window(0)
{
	uint32_t width, height;
	graphics_get_display_size(0/* LCD */, &width, &height);

	VC_RECT_T dst_rect;
	dst_rect.x = 0;
	dst_rect.y = 0;
	dst_rect.width = width;
	dst_rect.height = height;

	VC_RECT_T src_rect;
	src_rect.x = 0;
	src_rect.y = 0;
	src_rect.width = width << 16;
	src_rect.height = height << 16;

	DISPMANX_UPDATE_HANDLE_T dispman_update = vc_dispmanx_update_start(0);
	DISPMANX_ELEMENT_HANDLE_T dispman_element = vc_dispmanx_element_add(
		dispman_update,
		m_native_display.get_internals()->m_dispman_display,
		0,
		&dst_rect,
		0,
		&src_rect,
		DISPMANX_PROTECTION_NONE,
		0,
		0,
		DISPMANX_NO_ROTATE
	);

	vc_dispmanx_update_submit_sync(dispman_update);

	EGL_DISPMANX_WINDOW_T *dispman_window = new EGL_DISPMANX_WINDOW_T;
	dispman_window->element = dispman_element;
	dispman_window->width = width;
	dispman_window->height = height;

	m_egl_native_window = reinterpret_cast < EGLNativeWindowType > (dispman_window);
}


native_window::~native_window()
{
	EGL_DISPMANX_WINDOW_T *dispman_window = reinterpret_cast < EGL_DISPMANX_WINDOW_T* > (m_egl_native_window);
	DISPMANX_UPDATE_HANDLE_T dispman_update = vc_dispmanx_update_start(0);
	vc_dispmanx_element_remove(dispman_update, dispman_window->element);
	vc_dispmanx_update_submit_sync(dispman_update);
}


} // namespace eglinfo end

