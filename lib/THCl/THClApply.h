#ifndef THCL_APPLY_INC
#define THCL_APPLY_INC

#include "THClTensorCopy.h"
#include "THClReduceApplyUtils.h"
#include "templates/TemplatedKernel.h"
#include "util/easycl_stringhelper.h"
#include "EasyCL.h"
#include "CLKernel_structs.h"

#include <string>

std::string getApplyD_template();
std::string getApplyDv2_template();

//
// This file contains pointwise operation functions and kernels that
// work on both contiguous and non-contiguous tensor arguments of
// arbitrary (up to MAX_CLTORCH_DIMS) dimensioned arguments without
// copying or temporary storage.
//

// Threads per block for our apply kernel
#define THCL_APPLY_THREADS_PER_BLOCK 32 * 16

// Called when we are copying into an overlapping index `dst`, but
// we don't care which writer wins. Hacky but it works.
void THClTensor_copyIgnoringOverlaps(THClState* state,
                                       THClTensor* dst,
                                       THClTensor* src);

typedef struct TensorInfoCl {
  TensorInfoCl( TensorInfo<unsigned int> info ) {
    dims = info.dims;
    if( info.offset > ( 1l << 30 ) ) {
      throw std::runtime_error("size " + easycl::toString(info.offset) + " out of bounds");
    }
    offset = (int)info.offset;
    for( int i = 0; i < dims; i++ ) {
      sizes[i] = info.sizes[i];
      strides[i] = info.strides[i];
    }
  }
  TensorInfoCl( TensorInfo<unsigned long> info ) {
    dims = info.dims;
    if( info.offset > ( 1l << 30 ) ) {
      throw std::runtime_error("size " + easycl::toString(info.offset) + " out of bounds");
    }
    offset = (int)info.offset;
    for( int i = 0; i < dims; i++ ) {
      if( info.sizes[i] > ( 1l << 31 ) ) {
        throw std::runtime_error("size " + easycl::toString(info.sizes[i]) + " out of bounds");
      }
      sizes[i] = info.sizes[i];
      strides[i] = info.strides[i];
    }
  }
  unsigned int sizes[MAX_CLTORCH_DIMS];
  unsigned int strides[MAX_CLTORCH_DIMS];
  int offset;
  int dims;
} TensorInfoCl;

template< typename IndexType >
void kernelLaunch_pointwiseApply1( THClState *state, dim3 grid, dim3 block, int A, TensorInfo<IndexType> aInfo, IndexType totalElements, HasOperator1 const * op ) {
  TemplatedKernel kernelBuilder( state->cl );
  kernelBuilder.set("dim1", A);
  std::vector<int> dims;
  if( A >= 0 ) {
    dims.push_back(A);
  }
  std::string operation = op->operator1();
  int numTensors = 1;
  int numScalars = 0;
  HasScalars const*hasScalars = dynamic_cast<HasScalars const*>(op);
  if( hasScalars != 0 ) {
    numScalars = hasScalars->getNumScalars();
  }
  kernelBuilder.set("num_tensors", numTensors);
  kernelBuilder.set("num_scalars", numScalars);
  kernelBuilder.set("dims", dims);
  kernelBuilder.set("num_tensor_inputs", numTensors);
  kernelBuilder.set("MAX_CLTORCH_DIMS", MAX_CLTORCH_DIMS);
  kernelBuilder.set("operation", operation);
  std::string uniqueName = "applyDv2_1t" + easycl::toString(numScalars) + "s_" + easycl::toString(A) + "_" + op->operator1();
  CLKernel *kernel = kernelBuilder.buildKernel( uniqueName, "THClApplyDv2.cl", getApplyDv2_template(), "THClTensor_pointwiseApplyD" );
  // calculate workgroup sizes and stuff
  dim3 global_ws;
  for( int i = 0; i < 3; i++ ) {
      global_ws.vec[i] = grid.vec[i] * block.vec[i];
  }

  // set up tensorinfos
  TensorInfoCl aInfoCl(aInfo);

  if( false ) {
    std::cout << "numTensors " << numTensors << std::endl;
    std::cout << "operation " << operation << std::endl;
    std::cout << "totalElements " << totalElements << std::endl;
    std::cout << "a offset " << aInfoCl.offset << std::endl;
    std::cout << "adims " << aInfoCl.dims << std::endl;
    for( int i = 0; i < aInfoCl.dims; i++ ) {
      std::cout << "a dim" << i << " size=" << aInfoCl.sizes[i] << 
        " stride=" << aInfoCl.strides[i] << std::endl;
    }
    std::cout<< "block " << block << std::endl;
    std::cout<< "grid " << grid << std::endl;
    std::cout<< "global_ws " << global_ws << std::endl;
  }

  if( !aInfo.wrapper->isOnDevice() ) {
    aInfo.wrapper->createOnDevice();
  }

  kernel->in(1, &aInfoCl);
  kernel->inout( aInfo.wrapper );

  for( int i = 0; i < numScalars; i++ ) {
    kernel->in(hasScalars->getScalar(i));
  }

  if( totalElements > ( 1l << 30 )) {
    throw std::runtime_error("Error: out of bounds for totalelements=" + easycl::toString(totalElements));
  }
  kernel->in( (int)totalElements );
  kernel->run(3, global_ws.vec, block.vec);
  state->cl->finish();
}

template< typename IndexType >
void kernelLaunch_pointwiseApply2( THClState *state, dim3 grid, dim3 block, int A, int B, TensorInfo<IndexType> aInfo, TensorInfo<IndexType> bInfo, IndexType totalElements, HasOperator2 const*op ) {
  TemplatedKernel kernelBuilder( state->cl );
  kernelBuilder.set("dim1", A);
  kernelBuilder.set("dim2", B);
  std::vector<int> dims;
  if( A >= 0 ) {
    dims.push_back(A);
  }
  if( B != A && B >= 0 ) {
    dims.push_back(B);
  }
  std::string operation = op->operator2();
  int numTensors = 2;
  int numScalars = 0;
  HasScalars const*hasScalars = dynamic_cast<HasScalars const*>(op);
  if( hasScalars != 0 ) {
    numScalars = hasScalars->getNumScalars();
  }
  kernelBuilder.set("num_tensors", numTensors);
  kernelBuilder.set("num_scalars", numScalars);
  kernelBuilder.set("dims", dims);
  kernelBuilder.set("MAX_CLTORCH_DIMS", MAX_CLTORCH_DIMS);
  kernelBuilder.set("operation", operation);
  std::string uniqueName = "applyDv2_" + easycl::toString(numTensors) + "t" + easycl::toString(numScalars) + "s_" + easycl::toString(A) + "_" + easycl::toString(B) + "_" + op->operator2();
  CLKernel *kernel = 0;
  try {
    kernel = kernelBuilder.buildKernel( uniqueName, "THClApplyDv2.cl", getApplyDv2_template(), "THClTensor_pointwiseApplyD" );
  } catch( std::runtime_error &e ) {
    std::cout << "Error building kernel in apply2 " << __FILE__ << ":" << easycl::toString( __LINE__ ) << ": " << e.what() << std::endl;
    throw e;
  }
  // calculate workgroup sizes and stuff
  dim3 global_ws;
  for( int i = 0; i < 3; i++ ) {
      global_ws.vec[i] = grid.vec[i] * block.vec[i];
  }

  // set up tensorinfos
  TensorInfoCl aInfoCl(aInfo);
  TensorInfoCl bInfoCl(bInfo);

  if( false ) {
    std::cout << "numTensors " << numTensors << std::endl;
    std::cout << "operation " << operation << std::endl;
    std::cout << "totalElements " << totalElements << std::endl;
    std::cout << "a offset " << aInfoCl.offset << 
      " b offset " << bInfoCl.offset << std::endl;
    std::cout << "adims " << aInfoCl.dims << " bdims " << bInfoCl.dims
      << std::endl;
    for( int i = 0; i < aInfoCl.dims; i++ ) {
      std::cout << "a dim" << i << " size=" << aInfoCl.sizes[i] << 
        " stride=" << aInfoCl.strides[i] << std::endl;
      std::cout << "b dim" << i << " size=" << bInfoCl.sizes[i] << 
        " stride=" << bInfoCl.strides[i] << std::endl;
    }
    std::cout<< "block " << block << std::endl;
    std::cout<< "grid " << grid << std::endl;
    std::cout<< "global_ws " << global_ws << std::endl;
  }

  if( !aInfo.wrapper->isOnDevice() ) {
    aInfo.wrapper->createOnDevice();
  }

  kernel->in(1, &aInfoCl);
  kernel->inout( aInfo.wrapper );

  kernel->in(1, &bInfoCl);
  kernel->inout( bInfo.wrapper );

  for( int i = 0; i < numScalars; i++ ) {
    kernel->in(hasScalars->getScalar(i));
  }

  if( totalElements > ( 1l << 30 )) {
    throw std::runtime_error("Error: out of bounds for totalelements=" + easycl::toString(totalElements));
  }
  kernel->in( (int)totalElements );
  kernel->run(3, global_ws.vec, block.vec);
  state->cl->finish();
}

template< typename IndexType >
void kernelLaunch_pointwiseApply3( THClState *state, dim3 grid, dim3 block, int A, int B, int C, TensorInfo<IndexType> aInfo, TensorInfo<IndexType> bInfo, TensorInfo<IndexType> cInfo, IndexType totalElements, HasOperator3 const*op ) {
  TemplatedKernel kernelBuilder( state->cl );
  kernelBuilder.set("dim1", A);
  kernelBuilder.set("dim2", B);
  kernelBuilder.set("dim3", C);
  std::vector<int> dims;
  if( A >= 0 ) {
    dims.push_back(A);
  }
  if( B != A && B >= 0 ) {
    dims.push_back(B);
  }
  if( C != A && C != B && C >= 0 ) {
    dims.push_back(C);
  }
  std::string operation = op->operator3();
  int numTensors = 3;
  int numScalars = 0;
  HasScalars const*hasScalars = dynamic_cast<HasScalars const*>(op);
  if( hasScalars != 0 ) {
    numScalars = hasScalars->getNumScalars();
  }
  kernelBuilder.set("num_tensors", numTensors);
  kernelBuilder.set("num_scalars", numScalars);
  kernelBuilder.set("dims", dims);
  kernelBuilder.set("MAX_CLTORCH_DIMS", MAX_CLTORCH_DIMS);
  kernelBuilder.set("operation", operation);
  std::string uniqueName = "applyDv2_3t" + easycl::toString(numScalars) + "s_" + easycl::toString(A) + "_" + easycl::toString(B) + "_" + easycl::toString(C) + "_" + op->operator3();
  CLKernel *kernel = kernelBuilder.buildKernel( uniqueName, "THClApplyDv2.cl", getApplyDv2_template(), "THClTensor_pointwiseApplyD" );
  // calculate workgroup sizes and stuff
  dim3 global_ws;
  for( int i = 0; i < 3; i++ ) {
      global_ws.vec[i] = grid.vec[i] * block.vec[i];
  }

  // set up tensorinfos
  TensorInfoCl aInfoCl(aInfo);
  TensorInfoCl bInfoCl(bInfo);
  TensorInfoCl cInfoCl(cInfo);

  if( false ) {
    std::cout << "numTensors " << numTensors << std::endl;
    std::cout << "operation " << operation << std::endl;
    std::cout << "totalElements " << totalElements << std::endl;
    std::cout << "a offset " << aInfoCl.offset << 
      " b offset " << bInfoCl.offset << std::endl;
    std::cout << "adims " << aInfoCl.dims << " bdims " << bInfoCl.dims
      << std::endl;
    for( int i = 0; i < aInfoCl.dims; i++ ) {
      std::cout << "a dim" << i << " size=" << aInfoCl.sizes[i] << 
        " stride=" << aInfoCl.strides[i] << std::endl;
      std::cout << "b dim" << i << " size=" << bInfoCl.sizes[i] << 
        " stride=" << bInfoCl.strides[i] << std::endl;
    }
    std::cout<< "block " << block << std::endl;
    std::cout<< "grid " << grid << std::endl;
    std::cout<< "global_ws " << global_ws << std::endl;
  }

  if( !aInfo.wrapper->isOnDevice() ) {
    aInfo.wrapper->createOnDevice();
  }
  kernel->in(1, &aInfoCl);
  kernel->inout( aInfo.wrapper );

  kernel->in(1, &bInfoCl);
  kernel->inout( bInfo.wrapper );

  kernel->in(1, &cInfoCl);
  kernel->inout( cInfo.wrapper );

  for( int i = 0; i < numScalars; i++ ) {
    kernel->in(hasScalars->getScalar(i));
  }

  if( totalElements > ( 1l << 30 )) {
    throw std::runtime_error("Error: out of bounds for totalelements=" + easycl::toString(totalElements));
  }
  kernel->in( (int)totalElements );
  kernel->run(3, global_ws.vec, block.vec);
  state->cl->finish();
}

inline dim3 getApplyBlock() {
  return dim3(THCL_APPLY_THREADS_PER_BLOCK);
}

inline bool getApplyGrid(THClState* state, long totalElements, dim3& grid) {
//  int curDevice = -1;
//  cudaGetDevice(&curDevice);

//  if (curDevice == -1) {
//    return false;
//  }

//  // Assume a reasonable number of SMs if no state is available
//  int numSM =
//    state ? state->deviceProperties[curDevice].multiProcessorCount : 15;

  // dont think we can get number of SMs in OpenCL? (at least, not in opencl 1.1?)
  // just hardcode to 16 for now...
  // FIXME
  int numSM = 16;

  // 16 warps per block * 4 per SM gives 64 warps per SM at maximum,
  // which seems to be a good sweetspot for latency hiding
  grid = dim3(mymin(DIVUP(totalElements, (long long) THCL_APPLY_THREADS_PER_BLOCK),
                  4LL * numSM));
  return true;
}

template <typename Op>
bool THClTensor_pointwiseApply1(THClState* state,
                                  THClTensor* a,
                                  const Op& op,
                                  TensorArgType aType = ReadWrite) {
  long totalElements = THClTensor_nElement(state, a);

  if (THClTensor_nDimension(state, a) > MAX_CLTORCH_DIMS) {
    return false;
  }

  if (THClTensor_nDimension(state, a) == 0) {
    // Zero-dim tensor; do nothing
    return true;
  }

  const dim3 block = getApplyBlock();

  dim3 grid;
  if (!getApplyGrid(state, totalElements, grid)) {
    return false;
  }

  // If tensor args have overlapping indices and are read/write, then
  // we must expand the tensor to a contiguous form first, since
  // otherwise there are conflicting writes. Upon copying back to the
  // non-contiguous form, there will be conflicting writes, but at
  // least with copy, one of the updaters will win atomically. This is
  // a sketchy property of the old system as well (writing into all
  // indices of a tensor with overlapping indices should probably be
  // an error, since it is unclear which one should win), but we will
  // preserve this last-writer-wins (in arbitrary copy order) behavior.
  THClTensor* oldA = NULL;

  if (aType == ReadWrite && THCL_overlappingIndices(state, a)) {
    // Must perform in contiguous space
    oldA = a;
    a = THClTensor_newContiguous(state, a);
  }

  // It is possible that the tensor dimensions are able to be collapsed,
  // and thus we can reduce the actual code complexity of the copy by
  // exploiting this knowledge statically, since the div/mod is the
  // most expensive part of the operation, more so than memory accesses.
  // For instance, when copying a non-contiguous to a contiguous tensor
  // (or vice versa), the contiguous tensor can be collapsed to one
  // dimension, and the loop to translate the linear index to the array
  // index can be similarly collapsed. That is what this unrolling is for.
#define HANDLE_CASE(TYPE, A)                                   \
   kernelLaunch_pointwiseApply1<TYPE>(state, grid, block, A, aInfo, (TYPE) totalElements, &op ); \
  /*THClTensor_pointwiseApply1<Op, TYPE, A>                    \
    <<<grid, block, 0, THClState_getCurrentStream(state)>>>(    \
      aInfo, (TYPE) totalElements, op);*/

#define HANDLE_A_CASE(TYPE, A)                      \
  {                                                 \
    if (aInfo.isContiguous()) {                     \
      HANDLE_CASE(TYPE, -2);                        \
    } else {                                        \
      switch (A) {                                  \
        case 1:                                     \
          HANDLE_CASE(TYPE, 1);                     \
          break;                                    \
        case 2:                                     \
          HANDLE_CASE(TYPE, 2);                     \
          break;                                    \
        case 3:                                     \
          HANDLE_CASE(TYPE, 3);                     \
          break;                                    \
        default:                                    \
          HANDLE_CASE(TYPE, -1);                    \
          break;                                    \
      }                                             \
    }                                               \
  }

  // Can we use 32-bit integer math in the kernel (the linear ID for the copy
  // and the resulting non-linear offset is all computable using 32-bit math?)
  // We also use unsigned index math in the kernel, as signed div/mod has
  // additional overhead.
  if (THCL_canUse32BitIndexMath(state, a)) {
    TensorInfo<unsigned int> aInfo(state, a);

    HANDLE_A_CASE(unsigned int, aInfo.dims);
  } else {
    TensorInfo<unsigned long> aInfo(state, a);

    // For large tensors, we only compile the completely contiguous
    // version and the completely generic version, to reduce
    // compilation time.
    if (aInfo.isContiguous()) {
      /*THClTensor_pointwiseApply1<Op, unsigned long, -2>
        <<<grid, block, 0, THClState_getCurrentStream(state)>>>(
          aInfo, (unsigned long) totalElements, op);*/
      THError("Not implemented");
    } else {
      /*THClTensor_pointwiseApply1<Op, unsigned long, -1>
        <<<grid, block, 0, THClState_getCurrentStream(state)>>>(
          aInfo, (unsigned long) totalElements, op);*/
      THError("Not implemented");
    }
  }
#undef HANDLE_CASE
#undef HANDLE_A_CASE

  if (oldA) {
    // Ignore overlaps when copying back; if we use THClTensor_copy
    // instead, it will recursively try and invoke ourselves to make
    // oldA contiguous.
    THClTensor_copyIgnoringOverlaps(state, oldA, a);
    THClTensor_free(state, a);
    a = oldA;
  }

  return true;
}

template <typename Op>
bool THClTensor_pointwiseApply2(THClState* state,
                                  THClTensor* a,
                                  THClTensor* b,
                                  const Op& op,
                                  TensorArgType aType = ReadWrite,
                                  TensorArgType bType = ReadOnly) {
  long totalElements = THClTensor_nElement(state, a);

  if (totalElements != THClTensor_nElement(state, b)) {
    return false;
  }

  if (THClTensor_nDimension(state, a) > MAX_CLTORCH_DIMS ||
      THClTensor_nDimension(state, b) > MAX_CLTORCH_DIMS) {
    return false;
  }

  if (THClTensor_nDimension(state, a) == 0) {
    // Zero-dim tensor; do nothing
    return true;
  }

  const dim3 block = getApplyBlock();

  dim3 grid;
  if (!getApplyGrid(state, totalElements, grid)) {
    return false;
  }

  // If tensor args have overlapping indices and are read/write, then
  // we must expand the tensor to a contiguous form first, since
  // otherwise there are conflicting writes. Upon copying back to the
  // non-contiguous form, there will be conflicting writes, but at
  // least with copy, one of the updaters will win atomically. This is
  // a sketchy property of the old system as well (writing into all
  // indices of a tensor with overlapping indices should probably be
  // an error, since it is unclear which one should win), but we will
  // preserve this last-writer-wins (in arbitrary copy order) behavior.
  THClTensor* oldA = NULL;
  THClTensor* oldB = NULL;

  if (aType == ReadWrite && THCL_overlappingIndices(state, a)) {
    // Must perform in contiguous space
    oldA = a;
    a = THClTensor_newContiguous(state, a);
  }
  if (bType == ReadWrite && THCL_overlappingIndices(state, b)) {
    // Must perform in contiguous space
    oldB = b;
    b = THClTensor_newContiguous(state, b);
  }

  // It is possible that the tensor dimensions are able to be collapsed,
  // and thus we can reduce the actual code complexity of the copy by
  // exploiting this knowledge statically, since the div/mod is the
  // most expensive part of the operation, more so than memory accesses.
  // For instance, when copying a non-contiguous to a contiguous tensor
  // (or vice versa), the contiguous tensor can be collapsed to one
  // dimension, and the loop to translate the linear index to the array
  // index can be similarly collapsed. That is what this unrolling is for.
#define HANDLE_CASE(TYPE, A, B)                                \
   kernelLaunch_pointwiseApply2< TYPE>(state, grid, block, A, B, aInfo, bInfo, (TYPE) totalElements, &op ); \
  /* THClTensor_pointwiseApply2<Op, TYPE, A, B>                 \
    <<<grid, block, 0, THClState_getCurrentStream(state)>>>(    \
      aInfo, bInfo, (TYPE) totalElements, op); */

#define HANDLE_B_CASE(TYPE, A, B)                   \
  {                                                 \
    if (bInfo.isContiguous()) {                     \
      HANDLE_CASE(TYPE, A, -2);                     \
    } else {                                        \
      switch (B) {                                  \
        case 1:                                     \
          HANDLE_CASE(TYPE, A, 1);                  \
          break;                                    \
        case 2:                                     \
          HANDLE_CASE(TYPE, A, 2);                  \
          break;                                    \
        case 3:                                     \
          HANDLE_CASE(TYPE, A, 3);                  \
          break;                                    \
        default:                                    \
          HANDLE_CASE(TYPE, A, -1);                 \
          break;                                    \
      }                                             \
    }                                               \
  }

#define HANDLE_A_CASE(TYPE, A, B)                   \
  {                                                 \
    if (aInfo.isContiguous()) {                     \
      HANDLE_B_CASE(TYPE, -2, B);                   \
    } else {                                        \
      switch (A) {                                  \
        case 1:                                     \
          HANDLE_B_CASE(TYPE, 1, B);                \
          break;                                    \
        case 2:                                     \
          HANDLE_B_CASE(TYPE, 2, B);                \
          break;                                    \
        case 3:                                     \
          HANDLE_B_CASE(TYPE, 3, B);                \
          break;                                    \
        default:                                    \
          HANDLE_B_CASE(TYPE, -1, B);               \
          break;                                    \
      }                                             \
    }                                               \
  }

  if (THCL_canUse32BitIndexMath(state, a) &&
      THCL_canUse32BitIndexMath(state, b)) {
    TensorInfo<unsigned int> aInfo(state, a);
    TensorInfo<unsigned int> bInfo(state, b);

    HANDLE_A_CASE(unsigned int, aInfo.dims, bInfo.dims);
  } else {
    TensorInfo<unsigned long> aInfo(state, a);
    TensorInfo<unsigned long> bInfo(state, b);

    // For large tensors, we only compile the completely contiguous
    // version and the completely generic version, to reduce
    // compilation time.
    if (aInfo.isContiguous() && bInfo.isContiguous()) {
      THError("Not implemented");
//      THClTensor_pointwiseApply2<Op, unsigned long, -2, -2>
//        <<<grid, block, 0, THClState_getCurrentStream(state)>>>(
//          aInfo, bInfo, (unsigned long) totalElements, op);
    } else {
      THError("Not implemented");
//      THClTensor_pointwiseApply2<Op, unsigned long, -1, -1>
//        <<<grid, block, 0, THClState_getCurrentStream(state)>>>(
//          aInfo, bInfo, (unsigned long) totalElements, op);
    }
  }
#undef HANDLE_CASE
#undef HANDLE_B_CASE
#undef HANDLE_A_CASE

  if (oldA) {
    // Ignore overlaps when copying back; if we use THClTensor_copy
    // instead, it will recursively try and invoke ourselves to make
    // oldA contiguous.
    THClTensor_copyIgnoringOverlaps(state, oldA, a);
    THClTensor_free(state, a);
    a = oldA;
  }

  if (oldB) {
    // Ignore overlaps when copying back; if we use THClTensor_copy
    // instead, it will recursively try and invoke ourselves to make
    // oldB contiguous.
    THClTensor_copyIgnoringOverlaps(state, oldB, b);
    THClTensor_free(state, b);
    b = oldB;
  }

  return true;
}

// this is a c++ template
template <typename Op>
bool THClTensor_pointwiseApply3(THClState* state,
                                  THClTensor* a,
                                  THClTensor* b,
                                  THClTensor* c,
                                  const Op& op,
                                  TensorArgType aType = ReadWrite,
                                  TensorArgType bType = ReadOnly,
                                  TensorArgType cType = ReadOnly) {
  long totalElements = THClTensor_nElement(state, a);

  if (totalElements != THClTensor_nElement(state, b) ||
      totalElements != THClTensor_nElement(state, c)) {
    return false;
  }

  if (THClTensor_nDimension(state, a) > MAX_CLTORCH_DIMS ||
      THClTensor_nDimension(state, b) > MAX_CLTORCH_DIMS ||
      THClTensor_nDimension(state, c) > MAX_CLTORCH_DIMS) {
    return false;
  }

  if (THClTensor_nDimension(state, a) == 0) {
    // Zero-dim tensor; do nothing
    return true;
  }

  const dim3 block = getApplyBlock();

  dim3 grid;
  if (!getApplyGrid(state, totalElements, grid)) {
    return false;
  }

  // If tensor args have overlapping indices and are read/write, then
  // we must expand the tensor to a contiguous form first, since
  // otherwise there are conflicting writes. Upon copying back to the
  // non-contiguous form, there will be conflicting writes, but at
  // least with copy, one of the updaters will win atomically. This is
  // a sketchy property of the old system as well (writing into all
  // indices of a tensor with overlapping indices should probably be
  // an error, since it is unclear which one should win), but we will
  // preserve this last-writer-wins (in arbitrary copy order) behavior.
  THClTensor* oldA = NULL;
  THClTensor* oldB = NULL;
  THClTensor* oldC = NULL;

  if (aType == ReadWrite && THCL_overlappingIndices(state, a)) {
    // Must perform in contiguous space
    oldA = a;
    a = THClTensor_newContiguous(state, a);
  }

  if (bType == ReadWrite && THCL_overlappingIndices(state, b)) {
    // Must perform in contiguous space
    oldB = b;
    b = THClTensor_newContiguous(state, b);
  }

  if (cType == ReadWrite && THCL_overlappingIndices(state, c)) {
    // Must perform in contiguous space
    oldC = c;
    c = THClTensor_newContiguous(state, c);
  }

#define HANDLE_CASE(TYPE, A, B, C)                                      \
    /* kernel launch ... */ \
   kernelLaunch_pointwiseApply3<TYPE>(state, grid, block, A, B, C, aInfo, bInfo, cInfo, (TYPE) totalElements, &op ); \
  /* THClTensor_pointwiseApply3<Op, TYPE, A, B, C> */                      \
    /* <<<grid, block, 0, THClState_getCurrentStream(state)>>>(             \
      aInfo, bInfo, cInfo, (TYPE) totalElements, op); */

#define HANDLE_C_CASE(TYPE, A, B, C)             \
  {                                              \
    if (cInfo.isContiguous()) {                  \
      HANDLE_CASE(TYPE, A, B, -2);               \
    } else {                                     \
      switch (C) {                               \
        case 1:                                  \
          HANDLE_CASE(TYPE, A, B, 1);            \
          break;                                 \
        case 2:                                  \
          HANDLE_CASE(TYPE, A, B, 2);            \
          break;                                 \
        case 3:                                  \
          HANDLE_CASE(TYPE, A, B, 3);            \
          break;                                 \
        default:                                 \
          HANDLE_CASE(TYPE, A, B, -1);           \
          break;                                 \
      }                                          \
    }                                            \
  }

#define HANDLE_B_CASE(TYPE, A, B, C)                 \
  {                                                  \
    if (bInfo.isContiguous()) {                      \
      HANDLE_C_CASE(TYPE, A, -2, C);                 \
    } else {                                         \
      switch (B) {                                   \
        case 1:                                      \
          HANDLE_C_CASE(TYPE, A, 1, C);              \
          break;                                     \
        case 2:                                      \
          HANDLE_C_CASE(TYPE, A, 2, C);              \
          break;                                     \
        case 3:                                      \
          HANDLE_C_CASE(TYPE, A, 3, C);              \
          break;                                     \
        default:                                     \
          HANDLE_C_CASE(TYPE, A, -1, C);             \
          break;                                     \
      }                                              \
    }                                                \
  }

#define HANDLE_A_CASE(TYPE, A, B, C)                 \
  {                                                  \
    if (aInfo.isContiguous()) {                      \
      HANDLE_B_CASE(TYPE, -2, B, C);                 \
    } else {                                         \
      switch (A) {                                   \
        case 1:                                      \
          HANDLE_B_CASE(TYPE, 1, B, C);              \
          break;                                     \
        case 2:                                      \
          HANDLE_B_CASE(TYPE, 2, B, C);              \
          break;                                     \
        case 3:                                      \
          HANDLE_B_CASE(TYPE, 3, B, C);              \
          break;                                     \
        default:                                     \
          HANDLE_B_CASE(TYPE, -1, B, C);             \
          break;                                     \
      }                                              \
    }                                                \
  }

  if (THCL_canUse32BitIndexMath(state, a) &&
      THCL_canUse32BitIndexMath(state, b) &&
      THCL_canUse32BitIndexMath(state, c)) {
    TensorInfo<unsigned int> aInfo(state, a);
    TensorInfo<unsigned int> bInfo(state, b);
    TensorInfo<unsigned int> cInfo(state, c);

    HANDLE_A_CASE(unsigned int, aInfo.dims, bInfo.dims, cInfo.dims);
  } else {
    TensorInfo<unsigned long> aInfo(state, a);
    TensorInfo<unsigned long> bInfo(state, b);
    TensorInfo<unsigned long> cInfo(state, c);

    // For large tensors, we only compile the completely contiguous
    // version and the completely generic version, to reduce
    // compilation time.
    if (aInfo.isContiguous() && bInfo.isContiguous() && cInfo.isContiguous()) {
      THError("Not implemented");
//      THClTensor_pointwiseApply3<Op, unsigned long, -2, -2, -2>
//        <<<grid, block, 0, THClState_getCurrentStream(state)>>>(
//          aInfo, bInfo, cInfo, (unsigned long) totalElements, op);
    } else {
      THError("Not implemented");
//      THClTensor_pointwiseApply3<Op, unsigned long, -1, -1, -1>
//        <<<grid, block, 0, THClState_getCurrentStream(state)>>>(
//          aInfo, bInfo, cInfo, (unsigned long) totalElements, op);
    }
  }
#undef HANDLE_CASE
#undef HANDLE_C_CASE
#undef HANDLE_B_CASE
#undef HANDLE_A_CASE

  if (oldA) {
    // Ignore overlaps when copying back; if we use THClTensor_copy
    // instead, it will recursively try and invoke ourselves to make
    // oldA contiguous.
    THClTensor_copyIgnoringOverlaps(state, oldA, a);
    THClTensor_free(state, a);
    a = oldA;
  }

  if (oldB) {
    // Ignore overlaps when copying back; if we use THClTensor_copy
    // instead, it will recursively try and invoke ourselves to make
    // oldB contiguous.
    THClTensor_copyIgnoringOverlaps(state, oldB, b);
    THClTensor_free(state, b);
    b = oldB;
  }

  if (oldC) {
    // Ignore overlaps when copying back; if we use THClTensor_copy
    // instead, it will recursively try and invoke ourselves to make
    // oldC contiguous.
    THClTensor_copyIgnoringOverlaps(state, oldC, c);
    THClTensor_free(state, c);
    c = oldC;
  }

  return true;
}

#undef THCL_APPLY_THREADS_PER_BLOCK

#endif // THCL_APPLY_INC

