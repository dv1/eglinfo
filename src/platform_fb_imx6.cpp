#include <sstream>
#include "platform.hpp"


namespace eglinfo
{


native_window::native_window(native_display const &p_native_display, EGLint const p_visual_id)
	: m_native_display(p_native_display)
{
	m_egl_native_window = fbCreateWindow(p_native_display.get_egl_native_display(), 0, 0, 0, 0);
}


native_window::~native_window()
{
}


native_display::native_display(char const *p_name)
{
	int display_index = 0;
	if (p_name != 0)
	{
		std::stringstream sstr(p_name);
		sstr >> display_index;
	}

	m_egl_native_display = fbGetDisplayByIndex(display_index);
}


native_display::~native_display()
{
}


bool native_display::is_ok() const
{
	return true;
}


} // namespace eglinfo end

