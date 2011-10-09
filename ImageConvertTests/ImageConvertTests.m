//
//  ImageConvertTests.m
//  ImageConvertTests
//
//  Created by Andrew Pouliot on 10/8/11.
//  Copyright (c) 2011 Darknoon. All rights reserved.
//

#import "ImageConvertTests.h"

#import "DNImageConvert.h"

@implementation ImageConvertTests

- (void)setUp
{
    [super setUp];
    
    // Set-up code here.
}

- (void)tearDown
{
    // Tear-down code here.
    
    [super tearDown];
}

- (void)testExample
{
	unsigned char src[4] = {123, 45, 6, 75};
	
	unsigned int pixelCount = 1003;
	unsigned char *srcBuffer = malloc(pixelCount * 4);
	
	for (int i=0; i<pixelCount; i++) {
		memcpy(&srcBuffer[4 * i], src, 4);
	}
	
	unsigned short *dstBuffer = malloc(pixelCount * 2);
	
	BOOL ok = DNConvert_ARGB8888toRGB565(srcBuffer, pixelCount * 4, dstBuffer);
	STAssertTrue(ok, @"Conversion should succeed");
	
	//down <12/31><11/63><0/31> = 
	unsigned short expected = ((src[0] >> 3) << 11) | ((src[1] >> 2) << 5) | ((src[2] >> 3) << 0);
	for (int i=0; i<pixelCount; i++) {
		STAssertEquals(dstBuffer[i], expected, @"not equal!");
	}
	
	free(srcBuffer);
	free(dstBuffer);
	
}

@end
