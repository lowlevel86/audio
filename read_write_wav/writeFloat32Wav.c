#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>


int writeF32MonoWav(char *wavFileName, float *sndData, int sampleCnt, int sampleRate)
{
   int chunkSize;
   int byteRate;
   int subChunkSize;
   FILE *wavFile;
   
   //  'R',  'I',  'F',  'F', [     chunkSize      ],  'W',  'A',  'V',  'E',  'f',  'm',  't', 0x20,
   // 0x12, 0x00, 0x00, 0x00, 0x03, 0x00,  [ chCnt ],  [    sampleRate     ], [      byteRate      ],
   // [ bAlign ], 0x20, 0x00, 0x00, 0x00,  'f',  'a',  'c',  't', 0x04, 0x00, 0x00, 0x00, [ sample->
   // Frames   ],  'd',  'a',  't',  'a',  [    subChunkSize   ]
   char waveFormatChunk[58]={ 'R',  'I',  'F',  'F',    0,    0,    0,    0,  'W',  'A',  'V',  'E',  'f',  'm',  't', 0x20,
                             0x12, 0x00, 0x00, 0x00, 0x03, 0x00,    1,    0,    0,    0,    0,    0,    0,    0,    0,    0,
                                4,    0, 0x20, 0x00, 0x00, 0x00,  'f',  'a',  'c',  't', 0x04, 0x00, 0x00, 0x00,    0,    0,
                                0,    0,  'd',  'a',  't',  'a',    0,    0,    0,    0};
   
   
   // file size except for "RIFF\0\0\0\0"
   chunkSize = sampleCnt * sizeof(float) + sizeof(waveFormatChunk) - 8;
   memcpy(&waveFormatChunk[4], (char *)(&chunkSize), 4);
   
   // samples per second
   memcpy(&waveFormatChunk[24], (char *)(&sampleRate), 4);
   
   // bytes per second
   byteRate = sampleRate * sizeof(float);
   memcpy(&waveFormatChunk[28], (char *)(&byteRate), 4);
   
   // frame count
   memcpy(&waveFormatChunk[46], (char *)(&sampleCnt), 4);
   
   // size of data
   subChunkSize = sampleCnt * sizeof(float);
   memcpy(&waveFormatChunk[54], (char *)(&subChunkSize), 4);
   

   // open file for write
   wavFile = fopen(wavFileName,"wb");
   if (!wavFile)
   {
      printf("Can not open file.\n");
      return 1;
   }
   
   fwrite(waveFormatChunk, sizeof(char), sizeof(waveFormatChunk), wavFile);
   fwrite(sndData, sizeof(float), sampleCnt, wavFile);
   
   fclose(wavFile);
   
   
   return 0;
}


int writeF32StereoWav(char *wavFileName, float *sndDataLft, float *sndDataRght, int sampleCnt, int sampleRate)
{
   #define WRITE_BUFF_SIZE 4096
   int i, j;
   float writeBuff[WRITE_BUFF_SIZE];
   int chunkSize;
   int byteRate;
   int subChunkSize;
   FILE *wavFile;
   
   //  'R',  'I',  'F',  'F', [     chunkSize      ],  'W',  'A',  'V',  'E',  'f',  'm',  't', 0x20,
   // 0x12, 0x00, 0x00, 0x00, 0x03, 0x00,  [ chCnt ],  [    sampleRate     ], [      byteRate      ],
   // [ bAlign ], 0x20, 0x00, 0x00, 0x00,  'f',  'a',  'c',  't', 0x04, 0x00, 0x00, 0x00, [ sample->
   // Frames   ],  'd',  'a',  't',  'a',  [    subChunkSize   ]
   char waveFormatChunk[58]={ 'R',  'I',  'F',  'F',    0,    0,    0,    0,  'W',  'A',  'V',  'E',  'f',  'm',  't', 0x20,
                             0x12, 0x00, 0x00, 0x00, 0x03, 0x00,    2,    0,    0,    0,    0,    0,    0,    0,    0,    0,
                                8,    0, 0x20, 0x00, 0x00, 0x00,  'f',  'a',  'c',  't', 0x04, 0x00, 0x00, 0x00,    0,    0,
                                0,    0,  'd',  'a',  't',  'a',    0,    0,    0,    0};
   
   
   // file size except for "RIFF\0\0\0\0"
   chunkSize = sampleCnt * sizeof(float) * 2 + sizeof(waveFormatChunk) - 8;
   memcpy(&waveFormatChunk[4], (char *)(&chunkSize), 4);
   
   // samples per second
   memcpy(&waveFormatChunk[24], (char *)(&sampleRate), 4);
   
   // bytes per second
   byteRate = sampleRate * sizeof(float) * 2;
   memcpy(&waveFormatChunk[28], (char *)(&byteRate), 4);
   
   // frame count
   memcpy(&waveFormatChunk[46], (char *)(&sampleCnt), 4);
   
   // size of data
   subChunkSize = sampleCnt * sizeof(float) * 2;
   memcpy(&waveFormatChunk[54], (char *)(&subChunkSize), 4);
   

   // open file for write
   wavFile = fopen(wavFileName,"wb");
   if (!wavFile)
   {
      printf("Can not open file.\n");
      return 1;
   }
   
   fwrite(waveFormatChunk, sizeof(char), sizeof(waveFormatChunk), wavFile);
   
   j = 0;
   while (j < sampleCnt)
   {
      for (i=0; i < WRITE_BUFF_SIZE; i+=2)
      {
         if (j == sampleCnt)
         break;
         
         writeBuff[i] = sndDataLft[j];
         writeBuff[i+1] = sndDataRght[j];
         j++;
      }
      
      fwrite(writeBuff, sizeof(float), i, wavFile);
   }
   
   fclose(wavFile);
   
   
   return 0;
}
