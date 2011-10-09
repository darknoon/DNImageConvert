//
//  DNImageConvert.c
//  ImageConvert
//
//  Created by Andrew Pouliot on 10/8/11.
//  Copyright (c) 2011 Darknoon. All rights reserved.
//

#include "DNImageConvert.h"

#define USE_NEON_IMPL 0

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
	const int pixelsPerLoop = 8;
	for(i = 0; i < pixelCount; i += pixelsPerLoop, inPixel32 += pixelsPerLoop, outPixel16 += pixelsPerLoop) {
		//Read all r,g,b pixels into 3 registers
		uint8x8x4_t rgba  = vld4_u8(inPixel32);
		//Right-shift r,g,b as appropriate
		uint8x8_t r = vshr_n_u8(rgba.val[0], 3);
		uint8x8_t g = vshr_n_u8(rgba.val[1], 2);
		uint8x8_t b = vshr_n_u8(rgba.val[2], 3);
		
		//Compose first 4 into 4 32-bit ints
		//compose red by widening shift left by 11
#if 0 //WTF doesn't this work??
		uint16x8_t r5_g6_b5 = vshll_n_u8(r, 11);
		r5_g6_b5 += vshll_n_u8(g, 5);
		r5_g6_b5 += vshll_n_u8(b, 0);
#else
		uint16x8_t r5_g6_b5 = vmovl_u8(b);
		//Widen r
		uint16x8_t r16 = vmovl_u8(r);
		//Left shift into position within 16-bit int
		r16 = vshlq_n_u16(r16, 11);
		r5_g6_b5 |= r16;

		//Widen g
		uint16x8_t g16 = vmovl_u8(g);
		//Left shift into position within 16-bit int
		g16 = vshlq_n_u16(g16, 5);

		r5_g6_b5 |= g16;
#endif
		
		//Now write back to memory
		vst1q_u16(outPixel16, r5_g6_b5);		
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