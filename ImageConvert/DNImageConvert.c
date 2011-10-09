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
	
	for(int i = 0; i < pixelCount; ++i, ++inPixel32) {
		const unsigned int r = ((*inPixel32 >> 0 ) & 0xFF);
		const unsigned int g = ((*inPixel32 >> 8 ) & 0xFF);
		const unsigned int b = ((*inPixel32 >> 16) & 0xFF);
		*outPixel16++ = ((r >> 3) << 11) | ((g >> 2) << 5) | ((b >> 3) << 0);
	}
	
	return true;
}