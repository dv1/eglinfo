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


#ifndef EGLINFO_BINARY_SHADER_FORMATS_HPP
#define EGLINFO_BINARY_SHADER_FORMATS_HPP

#include "opengl.hpp"


#ifndef GL_AMD_program_binary_Z400
#define GL_Z400_BINARY_AMD                                      0x8740
#define GL_AMD_program_binary_Z400 1
#endif


#ifndef GL_VIV_shader_binary
#define GL_SHADER_BINARY_VIV                                    0x8FC4
#define GL_VIV_shader_binary 1
#endif


#ifndef GL_ARM_mali_shader_binary
#define GL_MALI_SHADER_BINARY_ARM                               0x8F60
#define GL_ARM_mali_shader_binary 1
#endif


#ifndef GL_DMP_shader_binary
#define GL_SHADER_BINARY_DMP                                    0x9250
#define GL_DMP_shader_binary 1
#endif


#ifndef GL_FJ_shader_binary_GCCSO
#define GCCSO_SHADER_BINARY_FJ                                  0x9260
#define GL_FJ_shader_binary_GCCSO 1
#endif


#if !defined(GL_IMG_shader_binary) && !defined(GL_IMG_binary_shader)
#define GL_SGX_BINARY_IMG                                       0x8C0A
#define GL_IMG_shader_binary 1
#define GL_IMG_binary_shader 1
#endif


#endif
