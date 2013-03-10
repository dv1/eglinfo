#ifndef EGLINFO_SCOPED_RESOURCES_HPP
#define EGLINFO_SCOPED_RESOURCES_HPP

#include <EGL/egl.h>


namespace eglinfo
{


class egl_scope
{
public:
	explicit egl_scope(EGLNativeDisplayType const &p_egl_native_display);
	~egl_scope();

	operator bool() const;

	EGLNativeDisplayType const & get_egl_native_display() const;
	EGLDisplay const & get_display() const;

	EGLint get_major_version() const;
	EGLint get_minor_version() const;

private:
	EGLNativeDisplayType m_egl_native_display;
	EGLDisplay m_display;
	EGLint m_major_version, m_minor_version;
};


class scoped_context
{
public:
	explicit scoped_context(EGLDisplay const &p_display, EGLContext const &p_context);
	~scoped_context();

	EGLContext const & get_context() const;

private:
	EGLDisplay m_display;
	EGLContext m_context;
};


class scoped_surface
{
public:
	explicit scoped_surface(EGLDisplay const &p_display, EGLSurface const &p_surface);
	~scoped_surface();

	EGLSurface const & get_surface() const;

private:
	EGLDisplay m_display;
	EGLSurface m_surface;
};


} // namespace eglinfo end


#endif

