/*

eglinfo EGL information utility

Copyright (c) 2014 Carlos Rafael Giani

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

#include <iostream>
#include <stddef.h>
#include <string.h>
#include <errno.h>
#include <wayland-egl.h>


namespace eglinfo
{


struct native_window::internals
{
	struct wl_registry *m_registry;
	struct wl_compositor *m_compositor;
	struct wl_shell *m_shell;
	struct wl_surface *m_surface;
	struct wl_shell_surface *m_shell_surface;

	internals()
		: m_registry(NULL)
		, m_compositor(NULL)
		, m_shell(NULL)
		, m_surface(NULL)
		, m_shell_surface(NULL)
	{
	}

	~internals()
	{
		if (m_shell_surface != NULL)
			wl_shell_surface_destroy(m_shell_surface);

		if (m_surface != NULL)
			wl_surface_destroy(m_surface);

		if (m_shell != NULL)
			wl_shell_destroy(m_shell);

		if (m_compositor != NULL)
			wl_compositor_destroy(m_compositor);
	}
};


namespace
{


void registry_handle_global(void *data, struct wl_registry *registry, uint32_t id, char const *interface, uint32_t)
{
	native_window::internals *internals = reinterpret_cast < native_window::internals* > (data);

	std::string iface(interface);

	if (iface ==  "wl_compositor")
		internals->m_compositor = reinterpret_cast < struct wl_compositor* > (wl_registry_bind(registry, id, &wl_compositor_interface, 1));
	else if (iface == "wl_shell")
		internals->m_shell = reinterpret_cast < struct wl_shell* > (wl_registry_bind(registry, id, &wl_shell_interface, 1));
}

void registry_handle_global_remove(void *, struct wl_registry *, uint32_t)
{
}

static const struct wl_registry_listener registry_listener =
{
	registry_handle_global,
	registry_handle_global_remove
};


}


native_window::native_window(native_display const &p_native_display, EGLint const p_visual_id)
	: m_native_display(p_native_display)
	, m_egl_native_window(0)
{
	m_internals = new internals;

	struct wl_display *display = reinterpret_cast < struct wl_display* > (p_native_display.get_egl_native_display());

	m_internals->m_registry = wl_display_get_registry(display);
	wl_registry_add_listener(m_internals->m_registry, &registry_listener, m_internals);
	if (wl_display_dispatch(display) == -1)
	{
		std::cerr << "wl_display_dispatch failed: " << strerror(errno) << std::endl;
		return;
	}

	if (m_internals->m_compositor == NULL)
	{
		std::cerr << "compositor pointer is NULL" << std::endl;
		return;
	}

	if (m_internals->m_shell == NULL)
	{
		std::cerr << "shell pointer is NULL" << std::endl;
		return;
	}

	if ((m_internals->m_surface = wl_compositor_create_surface(m_internals->m_compositor)) == NULL)
	{
		std::cerr << "creating Wayland surface failed" << std::endl;
		return;
	}

	if ((m_internals->m_shell_surface = wl_shell_get_shell_surface(m_internals->m_shell, m_internals->m_surface)) == NULL)
	{
		std::cerr << "creating Wayland shell surface failed" << std::endl;
		return;
	}

	m_egl_native_window = EGLNativeWindowType(wl_egl_window_create(m_internals->m_surface, 400, 300));
}


native_window::~native_window()
{
	if (m_egl_native_window != NULL)
	{
		wl_egl_window_destroy(reinterpret_cast < struct wl_egl_window* > (m_egl_native_window));
	}

	delete m_internals;
}


native_display::native_display(char const *p_name)
{
	if (p_name == NULL)
		std::cerr << "Using default Wayland display\n";
	else
		std::cerr << "Using Wayland display " << p_name << "\n";

	m_egl_native_display = EGLNativeDisplayType(wl_display_connect(p_name));
	if (m_egl_native_display == NULL)
	{
		std::cerr << "wl_display_connect failed: " << strerror(errno) << std::endl;
		return;
	}
}


native_display::~native_display()
{
	struct wl_display *display = reinterpret_cast < struct wl_display* > (m_egl_native_display);

	if (display != NULL)
	{
		wl_display_flush(display);
		wl_display_disconnect(display);
	}
}


bool native_display::is_ok() const
{
	return m_egl_native_display != NULL;
}


} // namespace eglinfo end
