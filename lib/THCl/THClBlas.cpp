#include <iostream>

#include "THClBlas.h"
#include "THClGeneral.h"

#include "util/stringhelper.h"
#include "EasyCL.h"
#include <clBLAS.h>

using namespace std;

void THClBlas_init(THClState *state, int devices, int device)
{
//  THClBlasState *blas_state = state->blasState;
//  blas_state->handles = (cublasHandle_t *)malloc(devices * sizeof(cublasHandle_t));
//  for (int i = 0; i < devices; i++) {
//    // Create handle on each device:
//    ClSetDevice(i);
//    cublasCreate(&blas_state->handles[i]);
//  }

//  // Set current handle:
//  blas_state->current_handle = &blas_state->handles[device];
//  blas_state->n_devices = devices;

//  // Restore device:
//  ClSetDevice(device);
    THError("Not implemented");
}

void THClBlas_shutdown(THClState *state)
{
//  THClBlasState *blas_state = state->blasState;
//  for (int i = 0; i < blas_state->n_devices; i++) {
//    cublasDestroy(blas_state->handles[i]);
//  }
//  free(blas_state->handles);
    THError("Not implemented");
}

void THClBlas_setHandle(THClState *state, int device)
{
//  THClBlasState *blas_state = state->blasState;
//  blas_state->current_handle = &blas_state->handles[device];
    THError("Not implemented");
}

//void THClBlas_setStream(THClState *state, int device, ClStream_t stream)
//{
////  THCublasCheck(cublasSetStream(state->blasState->handles[device], stream));
//    THError("Not implemented");
//}

void THClBlas_swap(THClState *state, long n, float *x, long incx, float *y, long incy)
{
//  if(n == 1)
//  {
//    incx = 1;
//    incy = 1;
//  }

//  if( (n <= INT_MAX) && (incx <= INT_MAX) && (incy <= INT_MAX) )
//  {
//    int i_n = (int)n;
//    int i_incx = (int)incx;
//    int i_incy = (int)incy;
//    THCublasCheck(cublasSswap(*state->blasState->current_handle, i_n, x, i_incx, y, i_incy));
//    return;
//  }
//  THError("Cublas_swap only supports n, incx and"
//          " incy upto signed integer limits: %d", INT_MAX);
    THError("Not implemented");
}

void THClBlas_scal(THClState *state, long n, float a, float *x, long incx)
{
 // if(n == 1)
//    incx = 1;

//  if( (n <= INT_MAX) && (incx <= INT_MAX) )
//  {
//    int i_n = (int)n;
//    int i_incx = (int)incx;
//    THCublasCheck(cublasSscal(*state->blasState->current_handle, i_n, &a, x, i_incx));
//    return;
//  }
//  THError("Cublas_scal only supports n and incx "
//          "upto signed integer limits: %d", INT_MAX);
    THError("Not implemented");
}

void THClBlas_copy(THClState *state, long n, float *x, long incx, float *y, long incy)
{
//  if(n == 1)
//  {
//    incx = 1;
//    incy = 1;
//  }

//  if( (n <= INT_MAX) && (incx <= INT_MAX) && (incy <= INT_MAX) )
//  {
//    int i_n = (int)n;
//    int i_incx = (int)incx;
//    int i_incy = (int)incy;
//    THCublasCheck(cublasScopy(*state->blasState->current_handle, i_n, x, i_incx, y, i_incy));
//    return;
//  }

//  THError("Cublas_copy only supports n, incx and incy "
//          "upto signed integer limits: %d", INT_MAX);
    THError("Not implemented");
}

void THClBlas_axpy(THClState *state, long n, float a, float *x, long incx, float *y, long incy)
{
 //   if(n == 1)
//  {
//    incx = 1;
//    incy = 1;
//  }

//  if( (n <= INT_MAX) && (incx <= INT_MAX) && (incy <= INT_MAX) )
//  {
//    int i_n = (int)n;
//    int i_incx = (int)incx;
//    int i_incy = (int)incy;
//    THCublasCheck(cublasSaxpy(*state->blasState->current_handle, i_n, &a, x, i_incx, y, i_incy));
//    return;
//  }

//  THError("Cublas_axpy only supports n, incx and incy "
//          "upto signed integer limits: %d", INT_MAX);
    THError("Not implemented");
}

float THClBlas_dot(THClState *state, long n, float *x, long incx, float *y, long incy)
{
//  if(n == 1)
//  {
//    incx = 1;
//    incy = 1;
//  }

//  if( (n <= INT_MAX) && (incx <= INT_MAX) && (incy <= INT_MAX) )
//  {
//    int i_n = (int)n;
//    int i_incx = (int)incx;
//    int i_incy = (int)incy;
//    float result;
//    THCublasCheck(cublasSdot(*state->blasState->current_handle, i_n, x, i_incx, y, i_incy, &result));
//    ClDeviceSynchronize();
//    return result;
//  }
//  THError("Cublas_dot only supports n, incx and incy "
//          "upto signed integer limits: %d", INT_MAX);
    THError("Not implemented");
  return -1;
}

/* Level 2 */
void THClBlas_gemv(THClState *state, char trans, long m, long n, float alpha, float *a, long lda, float *x, long incx, float beta, float *y, long incy)
{
//  if(n == 1)
//    lda = m;

//  cublasOperation_t op;
//  if (trans == 't') op = CUBLAS_OP_T;
//  else if (trans == 'n') op = CUBLAS_OP_N;
//  else if (trans == 'c') op = CUBLAS_OP_C;

//  if( (m <= INT_MAX) && (n <= INT_MAX) &&
//      (lda > 0) && (lda <= INT_MAX) &&
//      (incx > 0) && (incx <= INT_MAX) &&
//      (incy > 0) && (incy <= INT_MAX) )
//  {
//    int i_m = (int)m;
//    int i_n = (int)n;
//    int i_lda = (int)lda;
//    int i_incx = (int)incx;
//    int i_incy = (int)incy;

//    THCublasCheck(cublasSgemv(*state->blasState->current_handle, op, i_m, i_n, &alpha, a, i_lda, x, i_incx, &beta, y, i_incy));
//    return;
//  }
//  THError("Cublas_gemv only supports m, n, lda, incx, incy"
//          "in the range 0 < [val] <= %d", INT_MAX);
    THError("Not implemented");
}

void THClBlas_ger(THClState *state, long m, long n, float alpha, float *x, long incx, float *y, long incy, float *a, long lda)
{
//  if(n == 1)
//    lda = m;

//  if( (m <= INT_MAX) && (n <= INT_MAX) && (lda <= INT_MAX)  && (incx <= INT_MAX) && (incy <= INT_MAX) )
//    {
//      int i_m = (int)m;
//      int i_n = (int)n;
//      int i_lda = (int)lda;
//      int i_incx = (int)incx;
//      int i_incy = (int)incy;

//      THCublasCheck(cublasSger(*state->blasState->current_handle, i_m, i_n, &alpha, x, i_incx, y, i_incy, a, i_lda));
//      return;
//    }
//  THError("Cublas_ger only supports m, n, lda, incx, incy"
//          "with the bound [val] <= %d", INT_MAX);
    THError("Not implemented");
}

clblasTranspose convertTransToClblasOperation(char trans) {
  if (trans == 't') return clblasTrans;
  else if (trans == 'n') return clblasNoTrans ;
  else if (trans == 'c') return clblasConjTrans;
  else {
    THError("trans must be one of: t, n, c");
    return clblasTrans;
  }
  THError("Not implemented");
}

void adjustLd(char transa, char transb, long m, long n, long k, long *lda, long *ldb, long *ldc)
{
  int transa_ = ((transa == 't') || (transa == 'T'));
  int transb_ = ((transb == 't') || (transb == 'T'));

  if(n == 1)
    *ldc = m;

  if(transa_)
  {
    if(m == 1)
      *lda = k;
  }
  else
  {
    if(k == 1)
      *lda = m;
  }

  if(transb_)
  {
    if(k == 1)
      *ldb = n;
  }
  else
  {
    if(n == 1)
      *ldb = k;
  }
}

/* Level 3 */
void THClBlas_gemm(THClState *state, char transa, char transb, long m, long n, long k, float alpha, CLWrapper *aWrapper, long offseta, long lda, CLWrapper *bWrapper, long offsetb, long ldb, float beta, CLWrapper *cWrapper, long offsetc, long ldc)
{
  adjustLd(transa, transb, m, n, k, &lda, &ldb, &ldc);
  clblasTranspose opa = convertTransToClblasOperation(transa);
  clblasTranspose opb = convertTransToClblasOperation(transb);

  cout << "INT_MAX " << INT_MAX << endl;
  cout << "m=" << m << endl;
  cout << "m <= INT_MAX " << (m <= INT_MAX ) << endl;
  if( (m <= INT_MAX) && (n <= INT_MAX) && (k <= INT_MAX) && (lda <= INT_MAX)  && (ldb <= INT_MAX) && (ldc <= INT_MAX) )
  {
    int i_m = (int)m;
    int i_n = (int)n;
    int i_k = (int)k;
    int i_lda = (int)lda;
    int i_ldb = (int)ldb;
    int i_ldc = (int)ldc;

    cl_int err;

    err = clblasSetup();
    if (err != CL_SUCCESS) {
        THError("clblasSetup() failed with %d", err);
    }

//    CLFloatWrapper *Awrap = cl->wrap( M * K, A );
//    CLFloatWrapper *Bwrap = cl->wrap( K * N, B );
//    CLFloatWrapper *Cwrap = cl->wrap( M * N, C );
//    Awrap->copyToDevice();
//    Bwrap->copyToDevice();
//    Cwrap->copyToDevice();

//    size_t lda = K;        /* i.e. lda = K */
//    size_t ldb = N;        /* i.e. ldb = N */
//    size_t ldc = N;        /* i.e. ldc = N */

    if( !aWrapper->isOnDevice() ) {
      aWrapper->createOnDevice();
    }
    if( !bWrapper->isOnDevice() ) {
      bWrapper->createOnDevice();
    }
    if( !cWrapper->isOnDevice() ) {
      cWrapper->createOnDevice();
    }

    // DEBUG
    aWrapper->copyToHost();
    cout << "A:" << endl;
    for( int i = 0; i < 6; i++ ) {
      cout << "A[i]=" << ((float *)aWrapper->getHostArray())[i] << endl;
    }
    bWrapper->copyToHost();
    cout << "B:" << endl;
    for( int i = 0; i < 6; i++ ) {
      cout << "B[i]=" << ((float *)bWrapper->getHostArray())[i] << endl;
    }

    cl_event event = NULL;
      cout << "m=" << i_m << " n=" << i_n << " k=" << i_k
           << " lda=" << i_lda << " ldb=" << i_ldb << " ldc=" << i_ldc << endl;
    err = clblasSgemm(clblasColumnMajor, opa, opb, i_m, i_n, i_k,
                         alpha, aWrapper->getBuffer(), offseta, i_lda,
                         bWrapper->getBuffer(), offsetb, i_ldb, beta,
                         cWrapper->getBuffer(), offsetc, i_ldc,
                         1, state->cl->queue, 0, NULL, &event);
    if (err != CL_SUCCESS) {
        THError("clblasSgemm() failed with %d", err);
    }
    else {
        /* Wait for calculations to be finished. */
        err = clWaitForEvents(1, &event);
    }

    /* Finalize work with clblas. */
    clblasTeardown();

    // DEBUG
//    cWrapper->copyToHost();
//    cout << "C:" << endl;
//    for( int i = 0; i < 4; i++ ) {
//      cout << "C[i]=" << ((float *)cWrapper->getHostArray())[i] << endl;
//    }


//    THCublasCheck(cublasSgemm(*state->blasState->current_handle, opa, opb, i_m, i_n, i_k, &alpha, a, i_lda, b, i_ldb, &beta, c, i_ldc));
    return;
  }
  THError("Clblas_gemm only supports m, n, k, lda, ldb, ldc"
          "with the bound [val] <= %d", INT_MAX);
}

void THClBlas_gemmBatched(THClState *state, char transa, char transb, long m, long n, long k,
                            float alpha, const float *a[], long lda, const float *b[], long ldb,
                            float beta, float *c[], long ldc, long batchCount)
{
//  if( (m >= INT_MAX) || (n >= INT_MAX) || (k >= INT_MAX) || (lda >= INT_MAX)  || (ldb >= INT_MAX) || (ldc >= INT_MAX) || (batchCount >= INT_MAX) )
//  {
//    THError("Cublas_gemm only supports m, n, k, lda, ldb, ldc, batchCount"
//            "with the bound [val] <= %d", INT_MAX);
//  }

//  adjustLd(transa, transb, m, n, k, &lda, &ldb, &ldc);
//  cublasOperation_t opa = convertTransToCublasOperation(transa);
//  cublasOperation_t opb = convertTransToCublasOperation(transb);

//  THCublasCheck(cublasSgemmBatched(*state->blasState->current_handle,
//                                   opa, opb, (int)m, (int)n, (int)k,
//                                   &alpha, a, (int)lda, b, (int)ldb, &beta, c, (int)ldc,
//                                   (int)batchCount));
    THError("Not implemented");
}

