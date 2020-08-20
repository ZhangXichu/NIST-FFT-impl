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

#include "../include/mkl_dfti.h" /* the dynamic libraries need to be preloaded by setting the environment variable LD_PRELOAD */
#include "statistics.h"

typedef double _Complex mkl_cpx64; /* use double precision */

void DiscreteFourierTransformMKL(int n){
    int i;
    mkl_cpx64 *in;
    mkl_cpx64 *out;
    MKL_LONG status;
    double *m;
    double p_value;

    if (((in = (mkl_cpx64 *)calloc(n, sizeof(mkl_cpx64))) == NULL) ||
		((out = (mkl_cpx64 *)calloc(n, sizeof(mkl_cpx64))) == NULL) ||
		((m = (double*)calloc(n, sizeof(double))) == NULL)) {
		printf("\t\tUnable to allocate working arrays for the DFT.\n");
		if (in != NULL)
			free(in);
		if (out != NULL)
			free(out);
		if (m != NULL)
			free(m);
		return;
	}

    for (i = 0; i < n; i++){
        in[i] = 2 * ((double)(get_nth_block4(array, i) & 1)) - 1; // what's the difference of this to the original implementation of X[]?
    }

    DFTI_DESCRIPTOR_HANDLE desc_handle = NULL;

    status = DftiCreateDescriptor(&desc_handle, DFTI_DOUBLE, DFTI_COMPLEX, 1, n);
    status = DftiSetValue(desc_handle, DFTI_PLACEMENT, DFTI_NOT_INPLACE);
    status = DftiCommitDescriptor(desc_handle);
    status = DftiComputeForward(desc_handle, in, out);
    status = DftiFreeDescriptor(&desc_handle);

    /* check the transformed result */
    printf("\n");
    for (i = 0; i < n; i++){
        printf("%0.2f %0.2fi", creal(out[i]), cimag(out[i]));
    }
    printf("\n");

    for (i = 0; i < n; i++){
        m[i] = sqrt(pow(creal(out[i]), 2) + pow(cimag(out[i]), 2));
    }
    /* statistics */
    p_value = get_pvalue(n, m);
    // printf("MKL: p_value: %lf \n",p_value); /* use p-value to verify the result */

    free(in);
    free(out);
}