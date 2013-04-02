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


#ifndef EGLINFO_WRITER_HPP
#define EGLINFO_WRITER_HPP

#include <EGL/egl.h>
#include <config.h>
#include "egl_config.hpp"

#if defined(WITH_OPENGL) || defined(WITH_GLES1) || defined(WITH_GLES2)
#include "glapi_stats.hpp"
#endif

#if defined(WITH_OPENVG)
#include "openvg_stats.hpp"
#endif


namespace eglinfo
{


/**
 * Abstract class for writing EGL & GLES & OpenVG information.
 * It uses a state-based model similar to SAX. API processing code takes in a reference to a writer,
 * and calls these functions. How the information is conveyed to the user is up to the writer.
 */
class writer
{
public:
	// These begin and end the overall writing. Writers can start and end the output or document here.
	virtual void begin_write() = 0;
	virtual void end_write() = 0;

	// Begin and end an information block about the specific API.
	virtual void begin_api(EGLenum const p_api, char const *p_api_name) = 0;
	virtual void end_api() = 0;
	// This is used to signify a space between APIs. Writers can use this to put in spacings between API
	// information blocks.
	virtual void next_api() = 0;

	// Begin and end information about EGL itself.
	virtual void begin_egl_info() = 0;
	virtual void end_egl_info() = 0;
	// Write main EGL information.
	virtual void write_main_egl_info(
		  EGLint const p_major_version, EGLint const p_minor_version
		, char const *p_vendor         // vendor string
		, char const *p_version        // version string
		, char const *p_client_APIs    // whitespace-separated list of supported APIs
		, char const *p_extensions     // whitespace-separated list of extensions
	) = 0;
	// Begin and end the list of EGL configurations.
	virtual void begin_write_egl_configs(EGLint const p_num_configs) = 0;
	virtual void end_write_egl_configs() = 0;
	// Write an EGL configuration.
	virtual void write_egl_config(egl_config const &p_egl_config) = 0;
	// Used if there are no EGL configurations (instead of an empty EGL configs block).
	virtual void write_no_egl_configs() = 0;

#if defined(WITH_OPENGL) || defined(WITH_GLES1) || defined(WITH_GLES2)
	// Common function for writing main information about a GL API (OpenGL ES 1.x, OpenGL ES 2.x, and Desktop OpenGL).
	virtual void write_main_glapi_info(
		  EGLenum const p_api        // the EGL API name
		, char const *p_api_name     // human-readable name of the API
		, char const *p_version      // version string
		, char const *p_renderer     // renderer string
		, char const *p_extensions   // whitespace-separated list of extensions
	) = 0;

	// Common function for writing stats about a GL API (OpenGL ES 1.x, OpenGL ES 2.x, and Desktop OpenGL).
	// Stats are information like: maximum number of varyings, maximum anisotropy, list of supported shader binary languages etc.
	virtual void write_glapi_stats(
		  EGLenum const p_api         // the EGL API name
		, char const *p_api_name      // human-readable name of the API
		, glapi_stats const &p_stats  // main and shader specific stats about the API
	) = 0;
#endif

#if defined(WITH_OPENVG)
	// Write main information about OpenVG.
	virtual void write_main_vg_info(
		  char const *p_vendor         // vendor string
		, char const *p_version        // version string
		, char const *p_renderer       // renderer string
		, char const *p_extensions     // whitespace-separated list of extensions
	) = 0;

	// Write stats about OpenVG.
	virtual void write_vg_stats(openvg_stats const &p_stats) = 0;

	// Begin and end the list of image formats and whether or not they are hardware-accelerated.
	virtual void begin_write_vg_image_format_acceleration() = 0;
	virtual void end_write_vg_image_format_acceleration() = 0;
	// Write information about a specific format.
	virtual void write_vg_image_format_acceleration(
		  int const p_format          // the OpenVG format name
		, char const *p_name          // human-readable version of the name
		, bool const p_accelerated    // true = format is hardware-accelerated  false = otherwise
	) = 0;

	// Write information about path datatype acceleration
	// true = datatype is hardware-accelerated  false = otherwise
	virtual void write_vg_path_datatype_acceleration(
		  bool const p_signed8
		, bool const p_signed16
		, bool const p_signed32
		, bool const p_float
	) = 0;
#endif

	virtual ~writer()
	{
	}
};


} // namespace eglinfo end


#endif

