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


#include <config.h>
#include <unistd.h>
#include <memory>

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


void print_usage(char const *arg0)
{
	std::cerr
		<< "usage: " << arg0 << " [options]\n"
		<< "\t-h        : this help\n"
		<< "\t-w WRITER : use specified writer for output\n"
		<< "\t            valid values are: json text\n"
		<< "\t-d NAME   : output information for the native display with specified name\n"
		<< "\t-r        : if using the JSON writer, do a raw print instead of a pretty one\n"
		;
}


int main(int argc, char **argv)
{
	std::string native_display_name;
	typedef std::auto_ptr < eglinfo::writer > writer_ptr;
	writer_ptr writer;

	int opt;
	std::string writer_type = "text";
	bool json_raw_print = false;
	while ((opt = getopt(argc, argv, "w:d:rh")) != -1)
	{
		switch (opt)
		{
			case 'w':
			{
				if (optarg == 0)
				{
					print_usage(argv[0]);
					return -1;
				}
				writer_type = optarg;
				break;
			}
			case 'd':
			{
				if (optarg == 0)
				{
					print_usage(argv[0]);
					return -1;
				}
				native_display_name = optarg;
				break;
			}
			case 'r':
				json_raw_print = true;
				break;
			case 'h':
				print_usage(argv[0]);
				return 0;
			default:
				print_usage(argv[0]);
				return -1;
		}
	}

	if (writer_type == "json")
		writer = writer_ptr(new eglinfo::json_writer(std::cout, json_raw_print));
	else if (writer_type == "text")
		writer = writer_ptr(new eglinfo::text_writer(std::cout));
	else
	{
		std::cerr << "unknown writer '" << writer_type << "'\n";
		std::cerr << "\n";
		print_usage(argv[0]);
		return -1;
	}

	eglinfo::native_display d(native_display_name.empty() ? reinterpret_cast < char const* > (0) : native_display_name.c_str());
	if (!d.is_ok())
	{
		std::cerr << "could not initialize with native display " << native_display_name << "\n";
		return -1;
	}

	eglinfo::egl_scope egl(d);

	write_info(*writer, egl);

	return 0;
}

