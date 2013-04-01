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


#include "openvg_stats.hpp"


namespace eglinfo
{


openvg_stats::openvg_stats()
	: m_max_scissor_rects(0)
	, m_max_dash_count(0)
	, m_max_kernel_size(0)
	, m_max_separable_kernel_size(0)
	, m_max_color_ramp_stops(0)
	, m_max_image_width(0)
	, m_max_image_height(0)
	, m_max_image_pixels(0)
	, m_max_image_bytes(0)
	, m_max_gaussian_std_deviation(0)
{
}


} // namespace eglinfo end


