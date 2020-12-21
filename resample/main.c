#include <stdio.h>
#include <stdlib.h>
#include "readWriteFloat32Wav.h"
#include "resample.h"


int main()
{
   #define resample_ratio 1.45 // 0.75
   float *samps;
   int monoStereo;
   int sampCnt;
   int sampRate;
   float *output;
   int output_sampCnt;
   
   readFloat32Wav("sine440_540_640.wav", &samps, &monoStereo, &sampCnt, &sampRate);
   
   output_sampCnt = sampCnt * resample_ratio;
   output = (float *)malloc(output_sampCnt * sizeof(float));
   
   resample(samps, sampCnt, sampRate, output, output_sampCnt);
   
   writeF32MonoWav("output.wav", output, output_sampCnt, sampRate);
   
   free(output);
   releaseFloat32Wav(samps);
   return 0;
}
