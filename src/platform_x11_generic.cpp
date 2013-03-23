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


#include <iostream>
#include "platform.hpp"


namespace eglinfo
{


native_window::native_window(native_display const &p_native_display, EGLint const p_visual_id)
	: m_native_display(p_native_display)
	, m_egl_native_window(0)
{
	EGLNativeDisplayType egl_native_display = p_native_display.get_egl_native_display();

	int screen_num = DefaultScreen(egl_native_display);
	Window root_window = RootWindow(egl_native_display, screen_num);

	XVisualInfo visual_info_template, *visual_info;
	int num_visuals;
	visual_info_template.visualid = p_visual_id;
	visual_info = XGetVisualInfo(egl_native_display, VisualIDMask, &visual_info_template, &num_visuals);

	if (!visual_info)
	{
		std::cerr << "Could not get X11 visual\n";
		return;
	}

	XSetWindowAttributes attr;
	unsigned long mask;
	attr.background_pixel = 0;
	attr.border_pixel = 0;
	attr.colormap = XCreateColormap(egl_native_display, root_window, visual_info->visual, AllocNone);
	attr.event_mask = StructureNotifyMask | ExposureMask | KeyPressMask;
	mask = CWBackPixel | CWBorderPixel | CWColormap | CWEventMask;
	m_egl_native_window = XCreateWindow(egl_native_display, root_window, 0, 0, 400, 300,
			    0, visual_info->depth, InputOutput,
			    visual_info->visual, mask, &attr);

	{
		XSizeHints sizehints;
		sizehints.x = 0;
		sizehints.y = 0;
		sizehints.width  = 400;
		sizehints.height = 300;
		sizehints.flags = USSize | USPosition;
		XSetNormalHints(egl_native_display, m_egl_native_window, &sizehints);
		XSetStandardProperties(egl_native_display, m_egl_native_window, "eglinfo_testwindow", "eglinfo_testwindow", None, (char **)NULL, 0, &sizehints);
	}

	XFree(visual_info);
}


native_window::~native_window()
{
	if (m_egl_native_window)
		XDestroyWindow(m_native_display.get_egl_native_display(), m_egl_native_window);
}


native_display::native_display(char const *p_name)
{
	if (p_name == NULL)
	{
		std::cerr << "Using default X11 display\n";
		m_egl_native_display = XOpenDisplay(0);
	}
	else
	{
		std::cerr << "Using X11 display " << p_name << "\n";
		m_egl_native_display = XOpenDisplay(p_name);
	}
	if (m_egl_native_display == NULL)
		std::cerr << "Could not open display\n";
	else
		std::cerr << "\n";
}


native_display::~native_display()
{
	if (m_egl_native_display != NULL)
		XCloseDisplay(m_egl_native_display);
}


bool native_display::is_ok() const
{
	return m_egl_native_display != NULL;
}


} // namespace eglinfo end

