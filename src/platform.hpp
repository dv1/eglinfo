#ifndef EGLINFO_PLATFORM_HPP
#define EGLINFO_PLATFORM_HPP

#include <EGL/egl.h>


namespace eglinfo
{


class native_window
{
public:
	native_window(EGLNativeDisplayType const &p_egl_native_display, EGLint const p_visual_id);
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
	EGLNativeDisplayType m_egl_native_display;
	EGLNativeWindowType m_egl_native_window;

	struct internals;
	internals *m_internals;
};


class native_display
{
public:
	explicit native_display(char const *p_name);
	~native_display();

	EGLNativeDisplayType get_egl_native_display() const
	{
		return m_egl_native_display;
	}

	bool is_ok() const;

private:
	EGLNativeDisplayType m_egl_native_display;

	struct internals;
	internals *m_internals;
};


} // namespace eglinfo end


#endif

