#ifndef EGLINFO_PROCESS_OPENVG_HPP
#define EGLINFO_PROCESS_OPENVG_HPP

#include "writer.hpp"
#include "scopes.hpp"


namespace eglinfo
{


void print_vg_info(writer &p_vg_writer, scoped_egl_init const &p_egl);


} // namespace eglinfo end


#endif

