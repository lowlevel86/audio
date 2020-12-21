struct fftDataHND {
   double *data; // x,y data array
   int cnt;
};

void fftGetData(struct fftDataHND *, float *, int, int);
void fftFreeData(struct fftDataHND *);
