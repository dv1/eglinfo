#include <iostream>
#include "platform.hpp"


namespace eglinfo
{


native_window::native_window(EGLNativeDisplayType const &p_egl_native_display, EGLint const p_visual_id)
	: m_egl_native_display(p_egl_native_display)
	, m_egl_native_window(0)
{
	int screen_num = DefaultScreen(p_egl_native_display);
	Window root_window = RootWindow(p_egl_native_display, screen_num);

	XVisualInfo visual_info_template, *visual_info;
	int num_visuals;
	visual_info_template.visualid = p_visual_id;
	visual_info = XGetVisualInfo(p_egl_native_display, VisualIDMask, &visual_info_template, &num_visuals);

	if (!visual_info)
	{
		std::cerr << "Could not get X11 visual\n";
		return;
	}

	XSetWindowAttributes attr;
	unsigned long mask;
	attr.background_pixel = 0;
	attr.border_pixel = 0;
	attr.colormap = XCreateColormap(p_egl_native_display, root_window, visual_info->visual, AllocNone);
	attr.event_mask = StructureNotifyMask | ExposureMask | KeyPressMask;
	mask = CWBackPixel | CWBorderPixel | CWColormap | CWEventMask;
	m_egl_native_window = XCreateWindow(p_egl_native_display, root_window, 0, 0, 400, 300,
			    0, visual_info->depth, InputOutput,
			    visual_info->visual, mask, &attr);

	{
		XSizeHints sizehints;
		sizehints.x = 0;
		sizehints.y = 0;
		sizehints.width  = 400;
		sizehints.height = 300;
		sizehints.flags = USSize | USPosition;
		XSetNormalHints(p_egl_native_display, m_egl_native_window, &sizehints);
		XSetStandardProperties(p_egl_native_display, m_egl_native_window, "eglinfo_testwindow", "eglinfo_testwindow", None, (char **)NULL, 0, &sizehints);
	}

	XFree(visual_info);
}


native_window::~native_window()
{
	if (m_egl_native_window)
		XDestroyWindow(m_egl_native_display, m_egl_native_window);
}


native_display::native_display(char const *p_name)
{
	if (p_name == NULL)
	{
		std::cout << "Using default X11 display\n";
		m_egl_native_display = XOpenDisplay(0);
	}
	else
	{
		std::cout << "Using X11 display " << p_name << "\n";
		m_egl_native_display = XOpenDisplay(p_name);
	}
	if (m_egl_native_display == NULL)
		std::cerr << "Could not open display\n";
	else
		std::cout << "\n";
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

