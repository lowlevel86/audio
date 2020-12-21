#define MONO_TRACK 1
#define STEREO_TRACK 2

void releaseFloat32Wav(float *);
void readFloat32Wav(char *, float **, int *, int *, int *);
void writeF32MonoWav(char *, float *, int, int);
void writeF32StereoWav(char *, float *, float *, int, int);
