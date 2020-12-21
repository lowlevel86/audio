#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#define MONO_TRACK 1
#define STEREO_TRACK 2


void releaseFloat32Wav(float *sndData)
{
   void **wavHeaderOffset;
   
   if (sndData == NULL)
   return;
   
   wavHeaderOffset = (void **)(sndData - sizeof(void *));
   free((void *)((void *)sndData - *wavHeaderOffset));
}


int readFloat32Wav(char *wavFileName, float **sndData, int *monoStereo, int *sampleCnt, int *sampleRate)
{
   int i, j;
   struct stat stat_p;
   FILE *wavFile;
   char *wavData;
   void **wavHeaderOffset;
   int fmtCode;
   int sndDataLoc;
   int trackSize;
   float *trackBuff;
   
   
   *sndData = NULL;
   *sampleCnt = 0;
   
   // does the file exist
   if (-1 == stat(wavFileName, &stat_p))
   {
      printf("File not found.\n");
      return 1;
   }
   
   // open file for read
   wavFile = fopen(wavFileName,"rb");
   if (!wavFile)
   {
      printf("Can not open file.\n");
      return 1;
   }
   
   wavData = (char *)malloc(stat_p.st_size);
   
   fread(wavData, sizeof(char), stat_p.st_size, wavFile);
   
   
   // search for the string "fmt "
   for (i=0; i < stat_p.st_size-4; i++)
   if (strncmp(&wavData[i], "fmt ", 4) == 0)
   break;
   
   if (i == stat_p.st_size-4)
   {
      printf("Can not read wave header.\n");
      return 1;
   }
   
   fmtCode = wavData[i+4 + 4];// 4 bytes after the string "fmt "
   *monoStereo = wavData[i+4 + 6];
   *sampleRate = *(int *)&wavData[i+4 + 8];
   
   // format code must equal 3 for 32-bit float
   if (fmtCode != 3)
   {
      printf("Wrong format.\n");
      return 1;
   }
   
   
   // get the sound data location
   for (i=0; i < stat_p.st_size-4; i++)
   if (strncmp(&wavData[i], "data", 4) == 0)
   break;
   
   if (i == stat_p.st_size-4)
   {
      printf("Can not read wave header.\n");
      return 1;
   }
   
   sndDataLoc = i+4 + 4;// 4 bytes after the word "data"
   *sndData = (float *)&wavData[sndDataLoc];
   
   
   // get sample count
   *sampleCnt = (stat_p.st_size - sndDataLoc) / sizeof(float);
   
   
   if (*monoStereo == STEREO_TRACK)
   {
      trackSize = *sampleCnt / 2;
      trackBuff = (float *)malloc(trackSize * sizeof(float));
      
      j = 0;
      for (i=1; i < *sampleCnt; i+=2)
      trackBuff[j++] = (*sndData)[i];
      
      j = 0;
      for (i=0; i < *sampleCnt; i+=2)
      (*sndData)[j++] = (*sndData)[i];
      
      for (i=0; i < trackSize; i++)
      (*sndData)[trackSize+i] = trackBuff[i];
      
      free(trackBuff);
   }
   
   
   // save wave header offset behind the sound data for freeing later
   wavHeaderOffset = (void **)(*sndData - sizeof(void *));
   *wavHeaderOffset = (void *)((void *)*sndData - (void *)wavData);
   
   fclose(wavFile);
   return 0;
}
