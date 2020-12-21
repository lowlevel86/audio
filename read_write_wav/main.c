#include <stdio.h>
#include "readFloat32Wav.h"
#include "writeFloat32Wav.h"

//sox can convert audio to 32-bit floating-point:
//sox a.wav -e floating-point -b 32 -c 1 b.wav

int main()
{
   float *sndData;
   int monoStereo;
   int sampCnt;
   int sampRate;
   
   readFloat32Wav("sine440_540_640.wav", &sndData, &monoStereo, &sampCnt, &sampRate);
   
   writeF32MonoWav("output.wav", sndData, sampCnt, sampRate);
   
   releaseFloat32Wav(sndData);
   return 0;
}
