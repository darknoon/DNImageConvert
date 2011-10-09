//
//  DNImageConvert.h
//  ImageConvert
//
//  Created by Andrew Pouliot on 10/8/11.
//  Copyright (c) 2011 Darknoon. All rights reserved.
//

#ifndef ImageConvert_DNImageConvert_h
#define ImageConvert_DNImageConvert_h

#include <stdbool.h>
#include <sys/types.h>

//Destination buffer must be at least srcBufferSize / 2
//srcBufferSize must be a multiple of 4, naturally
extern bool DNConvert_ARGB8888toRGB565(void *src, size_t srcBufferSize, void *dst);

#endif
