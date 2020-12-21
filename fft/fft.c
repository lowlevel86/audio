#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "fft.h"

// This is a KISS implementation of
// the Cooley-Tukey recursive FFT algorithm.
// This is modified c++ code taken from a wikipedia page:
// https://en.wikipedia.org/wiki/Cooley%E2%80%93Tukey_FFT_algorithm#C++_Example_Code

struct complx {
   double real;
   double imag;
};

static double complxAbs(struct complx n)
{
   return sqrt(n.real * n.real + n.imag * n.imag);
}

static struct complx complxExp(struct complx n)
{
   struct complx output;
   
   output.real = exp(n.real) * cos(n.imag);
   output.imag = exp(n.real) * sin(n.imag);
   
   return output;
}

static struct complx complxMult(struct complx a, struct complx b)
{
   struct complx output;
   
   output.real = a.real * b.real - a.imag * b.imag;
   output.imag = a.real * b.imag + a.imag * b.real;
   
   return output;
}

static struct complx complxAdd(struct complx a, struct complx b)
{
   struct complx output;
   
   output.real = a.real + b.real;
   output.imag = a.imag + b.imag;
   
   return output;
} 

static struct complx complxSub(struct complx a, struct complx b)
{
   struct complx output;
   
   output.real = a.real - b.real;
   output.imag = a.imag - b.imag;
   
   return output;
} 

// separate even/odd elements to lower/upper halves of array respectively.
// Due to Butterfly combinations, this turns out to be the simplest way 
// to get the job done without clobbering the wrong elements.
static void separate(struct complx *a, int n)
{
   int i;
   struct complx *b;
   
   b = (struct complx *)malloc(n/2 * sizeof(struct complx));
   
   for(i=0; i < n/2; i++) // copy all odd elements to heap storage
   {
      b[i].real = a[i*2+1].real;
      b[i].imag = a[i*2+1].imag;
   }
   
   for(i=0; i < n/2; i++) // copy all even elements to lower-half of a[]
   {
      a[i].real = a[i*2].real;
      a[i].imag = a[i*2].imag;
   }
   
   for(i=0; i < n/2; i++) // copy all odd (from heap) to upper-half of a[]
   {
      a[i+n/2].real = b[i].real;
      a[i+n/2].imag = b[i].imag;
   }
   
   free(b);
}

// N input samples in X[] are FFT'd and results left in X[].
// Because of Nyquist theorem, N samples means 
// only first N/2 FFT results in X[] are the answer.
// (upper half of X[] is a reflection with no new information).
static void fft(struct complx *X, int N)
{
   int i;
   
   if (N < 2)
   return;

   separate(X, N); // all evens to lower half, all odds to upper half
   
   fft(X, N/2); // recurse even items
   
   fft(&X[0]+N/2, N/2); // recurse odd items
   
   // combine results of two half recursions
   for (i=0; i < N/2; i++)
   {
      struct complx e; // even
      struct complx o; // odd
      struct complx w; // twiddle-factor
      
      e.real = X[i].real;
      e.imag = X[i].imag;
      
      o.real = X[i+N/2].real;
      o.imag = X[i+N/2].imag;
      
      w.real = 0;
      w.imag = -2 * M_PI * i / N;
      w = complxExp(w);
      
      X[i] = complxAdd(e, complxMult(w, o));
      X[i+N/2] = complxSub(e, complxMult(w, o));
   }
}


void fftGetData(struct fftDataHND *hnd, float *inputData, int sampleCnt, int sampleRate)
{
   int i, j;
   double freqRes;
   struct complx *complxData;
   
   // data count needs to be a power of 2
   hnd->cnt = 2 << (int)(log(sampleCnt - 0.5) / log(2.0));
   
   // freq step in FFT result
   freqRes = (double)sampleRate / hnd->cnt;
   
   // malloc complxData array
   complxData = (struct complx *)malloc(hnd->cnt * sizeof(struct complx));
   
   // copy the input samples to the complxData array
   for (i=0; i < hnd->cnt; i++)
   {
      if (i >= sampleCnt)
      complxData[i].real = 0.0;
      else
      complxData[i].real = inputData[i];
      
      complxData[i].imag = 0.0;
   }
   
   // compute fft for this data
   fft(complxData, hnd->cnt);

   // allocate memory for output data
   hnd->data = (double *)malloc(hnd->cnt * sizeof(double));
   
   j=0;
   for (i=0; i < hnd->cnt; i+=2)
   {
      hnd->data[i] = j * freqRes;
      hnd->data[i+1] = complxAbs(complxData[j]);
      j++;
   }
   
   free(complxData);
}

void fftFreeData(struct fftDataHND *hnd)
{
   if (hnd->cnt)
   free(hnd->data);
   
   hnd->cnt = 0;
}
