#include "platform.hpp"


namespace eglinfo
{


native_window::native_window(EGLNativeDisplayType const &p_egl_native_display, EGLint const p_visual_id)
	: m_egl_native_display(p_egl_native_display)
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

