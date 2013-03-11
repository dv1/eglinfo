#ifndef EGLINFO_PROCESS_EGL_HPP
#define EGLINFO_PROCESS_EGL_HPP

#include "writer.hpp"
#include "scopes.hpp"


namespace eglinfo
{


bool process_egl_info(writer &p_egl_writer, egl_scope const &p_egl_scope);


} // namespace eglinfo end


#endif

