#include "platform.hpp"
#include <bcm_host.h>


namespace eglinfo
{


native_window::native_window(EGLNativeDisplayType const &p_egl_native_display, EGLint const p_visual_id)
	: m_egl_native_display(p_egl_native_display)
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
		m_egl_native_display,
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


} // namespace eglinfo end

