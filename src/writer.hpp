#ifndef EGLINFO_WRITER_HPP
#define EGLINFO_WRITER_HPP

#include <EGL/egl.h>
#include "egl_config.hpp"
#include "glapi_stats.hpp"


namespace eglinfo
{


class writer
{
public:
	virtual void begin_write() = 0;
	virtual void end_write() = 0;
	
	virtual void next_api() = 0;

	virtual void write_main_egl_info(
		  EGLint const p_major_version, EGLint const p_minor_version
		, char const *p_vendor
		, char const *p_version
		, char const *p_client_APIs
		, char const *p_extensions
	) = 0;

	virtual void begin_write_egl_configs(EGLint const p_num_configs) = 0;
	virtual void write_egl_config(egl_config const &p_egl_config) = 0;
	virtual void end_write_egl_configs() = 0;
	virtual void write_no_egl_configs() = 0;

	virtual void write_main_glapi_info(
		  EGLenum const p_api
		, char const *p_api_name
		, char const *p_version
		, char const *p_renderer
		, char const *p_extensions
	) = 0;

	virtual void write_glapi_stats(
		  EGLenum const p_api
		, char const *p_api_name
		, glapi_stats const &p_stats
	) = 0;

	virtual void write_main_vg_info(
		  char const *p_vendor
		, char const *p_version
		, char const *p_renderer
		, char const *p_extensions
	) = 0;

	virtual void begin_write_vg_image_format_acceleration() = 0;
	virtual void write_vg_image_format_acceleration(
		  int const p_format
		, char const *p_name
		, bool const p_accelerated
	) = 0;
	virtual void end_write_vg_image_format_acceleration() = 0;
	
	virtual void write_vg_path_datatype_acceleration(
		  bool const p_signed8
		, bool const p_signed16
		, bool const p_signed32
		, bool const p_float
	) = 0;

	virtual ~writer()
	{
	}
};


} // namespace eglinfo end


#endif

