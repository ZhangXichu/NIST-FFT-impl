#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <complex.h>
#include "../include/externs.h"
#include "../include/utilities.h"
#include "../include/cephes.h"
#include "../include/erf.h"
#include "../include/tools.h"
#include "../include/stat_fncs.h"

#include "../include/ipp/ipps.h"
#include "statistics.h"

void DiscreteFourierTransformIPP(int n){

    int i;
    int sizeSpec, sizeInitBuf, sizeWorkBuf;
    IppStatus status;
    Ipp64fc *in;
    Ipp64fc *out;
    Ipp8u *initBuf, *workBuf;
    IppsDFTSpec_C_64fc *spec = 0; /* initialized by the init function */

    double* m;
    double p_value;

    in = ippsMalloc_64fc(n);
    out = ippsMalloc_64fc(n); 

    status = ippsDFTGetSize_C_64fc(n, IPP_FFT_DIV_INV_BY_N, ippAlgHintAccurate, &sizeSpec, &sizeInitBuf, &sizeWorkBuf);

    /* check size */
    // printf("sizeSpec: %d\n", sizeSpec);
    // printf("sizeInitBuf: %d\n", sizeInitBuf);
    // printf("sizeWorkBuf: %d\n", sizeWorkBuf);

    spec = (IppsDFTSpec_C_64fc*)ippsMalloc_8u(sizeSpec);
    initBuf = ippsMalloc_8u(sizeInitBuf);
    workBuf = ippsMalloc_8u(sizeWorkBuf);

    m = (double *)calloc(n/2, sizeof(double));

    for (i = 0; i < n; i++){
        in[i].re = 2 * ((double)(get_nth_block4(array, i) & 1)) - 1; // what's the difference of this to the original implementation of X[]?
        in[i].im = 0.0;
    }

    /* initialization */
    status = ippsDFTInit_C_64fc(n, IPP_FFT_DIV_INV_BY_N, ippAlgHintAccurate, spec, initBuf); /* temp work buffer not used */

    /* transformation */
    status = ippsDFTFwd_CToC_64fc(in, out, spec, workBuf);

    /* check the transformed result */
    // printf("\n");
    // for (i = 0; i < n; i++){
    //     printf("%0.2f %0.2fi ", out[i].re, out[i].im);
    // }
    // printf("\n");

    for (i = 0; i < n/2; i++){
        m[i] = sqrt(pow(out[i].re,2)+pow(out[i].im,2));
    }
    p_value = get_pvalue(n, m);
    // printf("intel IPP: p-value: %lf \n ",p_value);

    if (initBuf) ippsFree(initBuf);
    if (workBuf) ippsFree(workBuf);
    if (spec) ippsFree(spec);

    ippsFree(in);
    ippsFree(out);

    free(m);
}