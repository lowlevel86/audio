// Note: Ported from code written in Basic from somewhere else.
//       The source code description was "Quick & Dirty Simple Slow ReSampling technique".

#include <stdio.h>
#include <math.h>

double getResampSamp(double x, float *inputSamps, int sampCnt, double LP_freq, int sampRate, int winLgth)
{
   int i, j;
   int winStart, winEnd;
   double gain, hann_y, sinc_denom, sinc_y, y;
   
   gain = 2.0 * LP_freq / sampRate; // gain correction factor
   y = 0.0;
   
   winStart = -winLgth / 2; // winLgth (number of samples in Hann window)
   winEnd = winLgth / 2;
   
   for (i=winStart; i < winEnd; i++)
   {
      j = (int)x + i; // input sample index
      
      // calculate von Hann window
      hann_y = 0.5 * (1.0 - cos(2 * M_PI * (j - x) / winLgth + M_PI)); // "+ M_PI" moves window 1/2 length
      
      // sinc function
      sinc_denom = 2.0 * M_PI * (j - x) * LP_freq / sampRate;
      
      if (sinc_denom != 0)
      sinc_y = sin(sinc_denom) / sinc_denom;
      else
      sinc_y = 1.0;
      
      // calculate output sample
      if ((j >= 0) && (j < sampCnt))
      y += gain * hann_y * sinc_y * inputSamps[j];
   }
   
   return y;
}

int resample(float *samps, int sampCnt, int sampRate, float *output_samps, int output_sampCnt)
{
   #define WINDOW_LENGTH 16 // filter quality increases with a larger window length
   #define LP_FREQ_COEFFICIENT 0.45 // LP_freq should be less than half the smallest sampling rate
   int i;
   double x, x_step;
   double LP_freq;
   
   if ((samps == NULL) || (output_samps == NULL))
   {
      printf("One or both buffers are NULL in resample().\n");
      return 1;
   }
   
   if ((!sampCnt) || (!output_sampCnt) || (!sampRate))
   {
      printf("Invalid parameter/s in resample().\n");
      return 1;
   }
   
   // initialize output sample location and step ratio
   x = 0;
   x_step = (double)sampCnt / output_sampCnt;
   
   // find low pass filter cutoff frequency
   if (sampCnt < output_sampCnt)
   LP_freq = sampRate * LP_FREQ_COEFFICIENT;
   else
   LP_freq = sampRate * ((double)output_sampCnt / sampCnt) * LP_FREQ_COEFFICIENT;
   
   for (i=0; i < output_sampCnt; i++)
   {
      output_samps[i] = getResampSamp(x, samps, sampCnt, LP_freq, sampRate, WINDOW_LENGTH);
      x += x_step;
   }
   
   return 0;
}
