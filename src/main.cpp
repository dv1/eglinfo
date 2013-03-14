#include <config.h>

#include "platform.hpp"

#include "process_egl.hpp"
#if defined(WITH_GLES1) || defined(WITH_GLES2) || defined(WITH_OPENGL)
#include "process_glapi.hpp"
#endif
#if defined(WITH_OPENVG)
#include "process_openvg.hpp"
#endif

#include "writer.hpp"
#include "text_writer.hpp"
#include "json_writer.hpp"
#include "csv_writer.hpp"


void write_info(eglinfo::writer &p_writer, eglinfo::egl_scope const &p_egl_scope)
{
	p_writer.begin_write();

	p_writer.begin_egl_info();
	eglinfo::process_egl_info(p_writer, p_egl_scope);
	p_writer.end_egl_info();

#if defined(WITH_OPENGL)
	p_writer.next_api();
	p_writer.begin_api(EGL_OPENGL_API, "opengl");
	eglinfo::process_glapi_info(p_writer, p_egl_scope, "OpenGL",      EGL_OPENGL_API,    true,  EGL_OPENGL_BIT       );
	p_writer.end_api();
#endif
#if defined(WITH_GLES1)
	p_writer.next_api();
	p_writer.begin_api(EGL_OPENGL_ES_API, "opengles1");
	eglinfo::process_glapi_info(p_writer, p_egl_scope, "OpenGL ES 1", EGL_OPENGL_ES_API, false, EGL_OPENGL_ES_BIT,  1);
	p_writer.end_api();
#endif
#if defined(WITH_GLES2)
	p_writer.next_api();
	p_writer.begin_api(EGL_OPENGL_ES_API, "opengles2");
	eglinfo::process_glapi_info(p_writer, p_egl_scope, "OpenGL ES 2", EGL_OPENGL_ES_API, true,  EGL_OPENGL_ES2_BIT, 2);
	p_writer.end_api();
#endif
#if defined(WITH_OPENVG)
	p_writer.next_api();
	p_writer.begin_api(EGL_OPENVG_API, "openvg");
	eglinfo::process_vg_info(p_writer, p_egl_scope);
	p_writer.end_api();
#endif

	p_writer.end_write();
}


int main(int argc, char **argv)
{
	using namespace eglinfo;

	native_display d((argc > 1) ? argv[1] : reinterpret_cast <char*> (0));
	if (!d.is_ok())
		return -1;

	egl_scope egl(d.get_egl_native_display());

	eglinfo::json_writer writer(std::cout);
	write_info(writer, egl);

	return 0;
}

