#include "scopes.hpp"
#include "log.hpp"


namespace eglinfo
{


egl_scope::egl_scope(native_display const &p_native_display)
	: m_native_display(p_native_display)
{
	m_display = eglGetDisplay(p_native_display.get_egl_native_display());
	if (m_display == EGL_NO_DISPLAY)
	{
		PRINT_EGL_ERROR("Could not obtain EGL display");
		return;
	}

	if (!eglInitialize(m_display, &m_major_version, &m_minor_version))
	{
		PRINT_EGL_ERROR("Could not initialize EGL");
		m_display = EGL_NO_DISPLAY;
		return;
	}
}

egl_scope::~egl_scope()
{
	if (m_display == EGL_NO_DISPLAY)
		return;

	eglMakeCurrent(m_display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
	eglTerminate(m_display);
}

egl_scope::operator bool() const
{
	return m_display != EGL_NO_DISPLAY;
}

native_display const & egl_scope::get_native_display() const
{
	return m_native_display;
}

EGLDisplay const & egl_scope::get_display() const
{
	return m_display;
}

EGLint egl_scope::get_major_version() const
{
	return m_major_version;
}

EGLint egl_scope::get_minor_version() const
{
	return m_minor_version;
}


scoped_context::scoped_context(EGLDisplay const &p_display, EGLContext const &p_context)
	: m_display(p_display)
	, m_context(p_context)
{
	if (m_context == EGL_NO_CONTEXT)
	{
		PRINT_EGL_ERROR("No valid context");
		return;
	}
}

scoped_context::~scoped_context()
{
	if (m_context != EGL_NO_CONTEXT)
		eglDestroyContext(m_display, m_context);
}

EGLContext const & scoped_context::get_context() const
{
	return m_context;
}


scoped_surface::scoped_surface(EGLDisplay const &p_display, EGLSurface const &p_surface)
	: m_display(p_display)
	, m_surface(p_surface)
{
	if (m_surface == EGL_NO_SURFACE)
	{
		PRINT_EGL_ERROR("No valid surface");
		return;
	}
}

scoped_surface::~scoped_surface()
{
	if (m_surface != EGL_NO_SURFACE)
		eglDestroySurface(m_display, m_surface);
}

EGLSurface const & scoped_surface::get_surface() const
{
	return m_surface;
}


} // namespace eglinfo end

