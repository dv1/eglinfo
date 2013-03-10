#include "platform.hpp"
#include "process_egl.hpp"
#include "write_text.hpp"
#include "write_json.hpp"
#include "write_csv.hpp"


template < typename Writer >
void write_info(Writer &p_writer, eglinfo::egl_scope const &p_egl_scope)
{
	begin_write(p_writer);

	process_egl_info(p_writer, p_egl_scope);

	next_api(p_writer);
	process_glapi_info(p_writer, p_egl_scope, "OpenGL",      EGL_OPENGL_API,    EGL_OPENGL_BIT       );
	next_api(p_writer);
	process_glapi_info(p_writer, p_egl_scope, "OpenGL ES 1", EGL_OPENGL_ES_API, EGL_OPENGL_ES_BIT,  1);
	next_api(p_writer);
	process_glapi_info(p_writer, p_egl_scope, "OpenGL ES 2", EGL_OPENGL_ES_API, EGL_OPENGL_ES2_BIT, 2);

	end_write(p_writer);
}


int main(int argc, char **argv)
{
	using namespace eglinfo;

	native_display d((argc > 1) ? argv[1] : reinterpret_cast <char*> (0));
	if (!d.is_ok())
		return -1;

	egl_scope egl(d.get_egl_native_display());

	return 0;
}

