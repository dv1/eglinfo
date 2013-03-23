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


#include "glapi_stats.hpp"
#include "compressed_texformats.hpp"


namespace eglinfo
{


main_glapi_stats::main_glapi_stats()
	: m_max_texture_size(0)
	, m_max_cubemap_texture_size(0)
	, m_max_texture_image_units(0)
	, m_max_renderbuffer_size(0)
	, m_max_combined_texture_image_units(0)
	, m_num_compressed_texture_formats(0)
{
}


shader_glapi_stats::shader_glapi_stats()
	: m_max_vertex_attribs(0)
	, m_max_vertex_texture_image_units(0)
	, m_num_shader_binary_formats(0)
	, m_max_varying_vectors(0)
	, m_max_vertex_uniform_vectors(0)
	, m_max_fragment_uniform_vectors(0)
{
}


glapi_stats::glapi_stats()
	: m_shader_capable(false)
{
}


char const * get_compressed_texture_format_string(GLint const p_format)
{
	switch (p_format)
	{
		case GL_COMPRESSED_RGBA_ASTC_4x4_KHR: return "COMPRESSED_RGBA_ASTC_4x4_KHR";
		case GL_COMPRESSED_RGBA_ASTC_5x4_KHR: return "COMPRESSED_RGBA_ASTC_5x4_KHR";
		case GL_COMPRESSED_RGBA_ASTC_5x5_KHR: return "COMPRESSED_RGBA_ASTC_5x5_KHR";
		case GL_COMPRESSED_RGBA_ASTC_6x5_KHR: return "COMPRESSED_RGBA_ASTC_6x5_KHR";
		case GL_COMPRESSED_RGBA_ASTC_6x6_KHR: return "COMPRESSED_RGBA_ASTC_6x6_KHR";
		case GL_COMPRESSED_RGBA_ASTC_8x5_KHR: return "COMPRESSED_RGBA_ASTC_8x5_KHR";
		case GL_COMPRESSED_RGBA_ASTC_8x6_KHR: return "COMPRESSED_RGBA_ASTC_8x6_KHR";
		case GL_COMPRESSED_RGBA_ASTC_8x8_KHR: return "COMPRESSED_RGBA_ASTC_8x8_KHR";
		case GL_COMPRESSED_RGBA_ASTC_10x5_KHR: return "COMPRESSED_RGBA_ASTC_10x5_KHR";
		case GL_COMPRESSED_RGBA_ASTC_10x6_KHR: return "COMPRESSED_RGBA_ASTC_10x6_KHR";
		case GL_COMPRESSED_RGBA_ASTC_10x8_KHR: return "COMPRESSED_RGBA_ASTC_10x8_KHR";
		case GL_COMPRESSED_RGBA_ASTC_10x10_KHR: return "COMPRESSED_RGBA_ASTC_10x10_KHR";
		case GL_COMPRESSED_RGBA_ASTC_12x10_KHR: return "COMPRESSED_RGBA_ASTC_12x10_KHR";
		case GL_COMPRESSED_RGBA_ASTC_12x12_KHR: return "COMPRESSED_RGBA_ASTC_12x12_KHR";
		case GL_COMPRESSED_SRGB8_ALPHA8_ASTC_4x4_KHR: return "COMPRESSED_SRGB8_ALPHA8_ASTC_4x4_KHR";
		case GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x4_KHR: return "COMPRESSED_SRGB8_ALPHA8_ASTC_5x4_KHR";
		case GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x5_KHR: return "COMPRESSED_SRGB8_ALPHA8_ASTC_5x5_KHR";
		case GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x5_KHR: return "COMPRESSED_SRGB8_ALPHA8_ASTC_6x5_KHR";
		case GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x6_KHR: return "COMPRESSED_SRGB8_ALPHA8_ASTC_6x6_KHR";
		case GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x5_KHR: return "COMPRESSED_SRGB8_ALPHA8_ASTC_8x5_KHR";
		case GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x6_KHR: return "COMPRESSED_SRGB8_ALPHA8_ASTC_8x6_KHR";
		case GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x8_KHR: return "COMPRESSED_SRGB8_ALPHA8_ASTC_8x8_KHR";
		case GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x5_KHR: return "COMPRESSED_SRGB8_ALPHA8_ASTC_10x5_KHR";
		case GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x6_KHR: return "COMPRESSED_SRGB8_ALPHA8_ASTC_10x6_KHR";
		case GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x8_KHR: return "COMPRESSED_SRGB8_ALPHA8_ASTC_10x8_KHR";
		case GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x10_KHR: return "COMPRESSED_SRGB8_ALPHA8_ASTC_10x10_KHR";
		case GL_COMPRESSED_SRGB8_ALPHA8_ASTC_12x10_KHR: return "COMPRESSED_SRGB8_ALPHA8_ASTC_12x10_KHR";
		case GL_COMPRESSED_SRGB8_ALPHA8_ASTC_12x12_KHR: return "COMPRESSED_SRGB8_ALPHA8_ASTC_12x12_KHR";

		case GL_ETC1_RGB8_OES: return "ETC1_RGB8";

		case GL_PALETTE4_RGB8_OES: return "PALETTE4_RGB8_OES";
		case GL_PALETTE4_RGBA8_OES: return "PALETTE4_RGBA8_OES";
		case GL_PALETTE4_R5_G6_B5_OES: return "PALETTE4_R5_G6_B5_OES";
		case GL_PALETTE4_RGBA4_OES: return "PALETTE4_RGBA4_OES";
		case GL_PALETTE4_RGB5_A1_OES: return "PALETTE4_RGB5_A1_OES";
		case GL_PALETTE8_RGB8_OES: return "PALETTE8_RGB8_OES";
		case GL_PALETTE8_RGBA8_OES: return "PALETTE8_RGBA8_OES";
		case GL_PALETTE8_R5_G6_B5_OES: return "PALETTE8_R5_G6_B5_OES";
		case GL_PALETTE8_RGBA4_OES: return "PALETTE8_RGBA4_OES";
		case GL_PALETTE8_RGB5_A1_OES: return "PALETTE8_RGB5_A1_OES";

		case GL_3DC_X_AMD: return "3DC_X_AMD";
		case GL_3DC_XY_AMD: return "3DC_XY_AMD";

		case GL_ATC_RGB_AMD: return "ATC_RGB";
		case GL_ATC_RGBA_EXPLICIT_ALPHA_AMD: return "ATC_RGBA_EXPLICIT_ALPHA_AMD";
		case GL_ATC_RGBA_INTERPOLATED_ALPHA_AMD: return "ATC_RGBA_INTERPOLATED_ALPHA_AMD";

		case GL_COMPRESSED_RGB_PVRTC_4BPPV1_IMG: return "COMPRESSED_RGB_PVRTC_4BPPV1_IMG";
		case GL_COMPRESSED_RGB_PVRTC_2BPPV1_IMG: return "COMPRESSED_RGB_PVRTC_2BPPV1_IMG";
		case GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG: return "COMPRESSED_RGBA_PVRTC_4BPPV1_IMG";
		case GL_COMPRESSED_RGBA_PVRTC_2BPPV1_IMG: return "COMPRESSED_RGBA_PVRTC_2BPPV1_IMG";

		case GL_COMPRESSED_RGBA_PVRTC_4BPPV2_IMG: return "COMPRESSED_RGBA_PVRTC_4BPPV2_IMG";
		case GL_COMPRESSED_RGBA_PVRTC_2BPPV2_IMG: return "COMPRESSED_RGBA_PVRTC_2BPPV2_IMG";
		
		case GL_COMPRESSED_RGB_S3TC_DXT1_EXT: return "COMPRESSED_RGB_S3TC_DXT1_EXT";
		case GL_COMPRESSED_RGBA_S3TC_DXT1_EXT: return "COMPRESSED_RGBA_S3TC_DXT1_EXT";
		
		case GL_COMPRESSED_RGBA_S3TC_DXT3_ANGLE: return "COMPRESSED_RGBA_S3TC_DXT3_ANGLE";
		case GL_COMPRESSED_RGBA_S3TC_DXT5_ANGLE: return "COMPRESSED_RGBA_S3TC_DXT5_ANGLE";
		
		case GL_COMPRESSED_SRGB_S3TC_DXT1_NV: return "COMPRESSED_SRGB_S3TC_DXT1_NV";
		case GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT1_NV: return "COMPRESSED_SRGB_ALPHA_S3TC_DXT1_NV";
		case GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT3_NV: return "COMPRESSED_SRGB_ALPHA_S3TC_DXT3_NV";
		case GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT5_NV: return "COMPRESSED_SRGB_ALPHA_S3TC_DXT5_NV";

		case GL_RGB_S3TC: return "RGB_S3TC";
		case GL_RGB4_S3TC: return "RGB4_S3TC";
		case GL_RGBA_S3TC: return "RGBA_S3TC";
		case GL_RGBA4_S3TC: return "RGBA4_S3TC";
		case GL_RGBA_DXT5_S3TC: return "RGBA_DXT5_S3TC";
		case GL_RGBA4_DXT5_S3TC: return "RGBA4_DXT5_S3TC";

		default: return 0;
	}
}


char const * get_shader_binary_format_string(GLint const p_format)
{
	switch (p_format)
	{
#ifdef GL_AMD_program_binary_Z400
		case GL_Z400_BINARY_AMD: return "Z400_BINARY_AMD";
#endif
#ifdef GL_VIV_shader_binary
		case GL_SHADER_BINARY_VIV: return "SHADER_BINARY_VIV";
#endif
#ifdef GL_ARM_mali_shader_binary
		case GL_MALI_SHADER_BINARY_ARM: return "MALI_SHADER_BINARY";
#endif
#ifdef GL_DMP_shader_binary
		case GL_SHADER_BINARY_DMP: return "SHADER_BINARY_DMP";
#endif
#ifdef GL_FJ_shader_binary_GCCSO
		case GCCSO_SHADER_BINARY_FJ: return "FJ_shader_binary_GCCSO";
#endif
#ifdef GL_IMG_shader_binary
		case GL_SGX_BINARY_IMG: return "SGX_BINARY_IMG";
#endif
		default: return 0;
	}
}


char const * get_color_format_string(GLint const p_format)
{
	switch (p_format)
	{
		case GL_ALPHA: return "alpha";
		case GL_RGB: return "RGB";
		case GL_RGBA: return "RGBA";
		default: return 0;
	}
}


char const * get_color_type_string(GLint const p_type)
{
	switch (p_type)
	{
		case GL_UNSIGNED_BYTE: return "unsigned byte";
		case GL_UNSIGNED_SHORT_5_6_5: return "unsigned int 5:6:5";
		case GL_UNSIGNED_SHORT_4_4_4_4: return "unsigned int 4:4:4:4";
		case GL_UNSIGNED_SHORT_5_5_5_1: return "unsigned int 5:5:5:1";
		default: return 0;
	}
}


} // namespace eglinfo end

