#ifndef EGLINFO_OPENGL_HPP
#define EGLINFO_OPENGL_HPP


#include <config.h>


#include <EGL/egl.h>


// Prefer ES2 header over ES1
#if defined(WITH_GLES2)

#include <GLES2/gl2.h>
#if defined(WITH_GL2EXT_H)
#include <GLES2/gl2ext.h>
#endif

#elif defined(WITH_GLES1)

#include <GLES/gl.h>
#if defined(WITH_GLEXT_H)
#include <GLES/glext.h>
#endif

#elif defined(WITH_OPENGL)
#include <GL/gl.h>
#endif


#endif

