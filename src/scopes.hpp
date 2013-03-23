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


#ifndef EGLINFO_SCOPED_RESOURCES_HPP
#define EGLINFO_SCOPED_RESOURCES_HPP

#include <EGL/egl.h>
#include "platform.hpp"


namespace eglinfo
{


class egl_scope
{
public:
	explicit egl_scope(native_display const &p_native_display);
	~egl_scope();

	operator bool() const;

	native_display const & get_native_display() const;
	EGLDisplay const & get_display() const;

	EGLint get_major_version() const;
	EGLint get_minor_version() const;

private:
	native_display const &m_native_display;
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

