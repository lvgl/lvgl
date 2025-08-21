//
//  main.cpp
//  GIF decode timing demo
//
//  Created by Laurence Bank on 8/2/20.
//  Copyright © 2020 Laurence Bank. All rights reserved.
//
#include "../src/AnimatedGIF.h"
#include "../src/gif.inl"

#include "../test_images/badgers.h"

GIFIMAGE gif;

void GIFDraw(GIFDRAW *pDraw)
{
} /* GIFDraw() */

int main(int argc, const char * argv[]) {
char szTemp[256];
int rc, iFrame;

    printf("Animated GIF Linux Demo\n");
    printf("Run with no parameters to test in-memory decoding\n");
    printf("Or pass a filename on the command line\n\n");
    GIF_begin(&gif, BIG_ENDIAN_PIXELS);
    printf("Starting GIF decoder...\n");
    if (argc == 2) // use filename
        rc = GIF_openFile(&gif, argv[1], GIFDraw);
    else
        rc = GIF_openRAM(&gif, (uint8_t *)badgers, sizeof(badgers), GIFDraw);
    if (rc)
    {
        printf("Successfully opened GIF\n");
        printf("Image size: %d x %d\n", GIF_getCanvasWidth(&gif), GIF_getCanvasHeight(&gif));
        iFrame = 0;
        while (GIF_playFrame(&gif, NULL, NULL))
        {
            iFrame++;
            printf("Successfully decoded frame %d\n", iFrame);
        }
        if (GIF_getComment(&gif, szTemp))
            printf("GIF Comment: \"%s\"\n", szTemp);
        GIF_close(&gif);
    }
    return 0;
}
