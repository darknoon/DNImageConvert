//
//  DNImageConvert.c
//  ImageConvert
//
//  Created by Andrew Pouliot on 10/8/11.
//  Copyright (c) 2011 Darknoon. All rights reserved.
//

#include "DNImageConvert.h"

#define USE_NEON_IMPL 1

#if defined(__ARM_NEON__)
#include <arm_neon.h>
#endif

bool DNConvert_ARGB8888toRGB565(void *src, size_t srcBufferSize, void *dst)
{
	if (!src || !dst || srcBufferSize == 0 || srcBufferSize%4 != 0) return false;
	
	//Convert "RRRRRRRRRGGGGGGGGBBBBBBBBAAAAAAAA" to "RRRRRGGGGGGBBBBB"
	unsigned int*			inPixel32 = (unsigned int*)src;
	unsigned short*			outPixel16 = (unsigned short*)dst;
	
	const unsigned int pixelCount = srcBufferSize / 4;
	
#if defined(__ARM_NEON__) && USE_NEON_IMPL
	int i;
	const int pixelsPerLoop = 4;
	for(i = 0; i < pixelCount; i += 4, inPixel32 += pixelsPerLoop, outPixel16 += pixelsPerLoop) {
#if 1
		//First implementation without special reads...
		
		//Do 256 bits at a time on NEON aka four pixels
		uint32x4_t p = vld1q_u32(inPixel32);
		
		//Create a constant vector of 0xFF
		uint32x4_t lastByte = vdupq_n_u32(0xFF);
		
		//Select r
		uint32x4_t r = vandq_u32(p, lastByte);
		
		//Select g
		uint32x4_t g = vshrq_n_u32(p, 8);
		g = vandq_u32(g, lastByte);
		
		//Select b
		uint32x4_t b = vshrq_n_u32(p, 16);
		b = vandq_u32(b, lastByte);
		
		//Accumulate r, g
		//r = r >> 3 ie convert to 5-bit int
		r = vshrq_n_u32(r, 3);
		//r = (r >> 3) << 11 ie setup place in output int
		r = vshlq_n_u32(r, 11);
		
		//g = g >> 2 ie convert to 6-bit int
		g = vshrq_n_u32(g, 2);
		//g = (r >> 2) << 5 ie setup place in output int
		g = vshlq_n_u32(g, 5);
		
		// rg = r | g
		uint32x4_t rg = vorrq_u32(r, g);
		
		//Accumulate rg, b
		
		//b = b >> 3 ie convert to smaller int
		b = vshrq_n_u32(b, 3);
		
		uint32x4_t rgb = vorrq_u32(rg, b);
		
		//Select the low 16 bits of each 32-bit int
		uint16x4_t rgb16 = vmovn_u32(rgb);
		
		//Now write back to memory
		vst1_u16(outPixel16, rgb16);
#else 
		
#endif
		
	}
	//Do the end on normal flt hardware
	for(; i < pixelCount; ++i, ++inPixel32) {
		const unsigned int r = ((*inPixel32 >> 0 ) & 0xFF);
		const unsigned int g = ((*inPixel32 >> 8 ) & 0xFF);
		const unsigned int b = ((*inPixel32 >> 16) & 0xFF);
		*outPixel16++ = ((r >> 3) << 11) | ((g >> 2) << 5) | ((b >> 3) << 0);	
	}
#else
	for(int i = 0; i < pixelCount; ++i, ++inPixel32) {
		const unsigned int r = ((*inPixel32 >> 0 ) & 0xFF);
		const unsigned int g = ((*inPixel32 >> 8 ) & 0xFF);
		const unsigned int b = ((*inPixel32 >> 16) & 0xFF);
		*outPixel16++ = ((r >> 3) << 11) | ((g >> 2) << 5) | ((b >> 3) << 0);
	}
#endif
	
	return true;
}