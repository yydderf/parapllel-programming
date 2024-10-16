#include "PPintrin.h"

// implementation of absSerial(), but it is vectorized using PP intrinsics
void absVector(float *values, float *output, int N)
{
  __pp_vec_float x;
  __pp_vec_float result;
  __pp_vec_float zero = _pp_vset_float(0.f);
  __pp_mask maskAll, maskIsNegative, maskIsNotNegative;

  //  Note: Take a careful look at this loop indexing.  This example
  //  code is not guaranteed to work when (N % VECTOR_WIDTH) != 0.
  //  Why is that the case?
  for (int i = 0; i < N; i += (N - i) % VECTOR_WIDTH ? N % VECTOR_WIDTH : VECTOR_WIDTH)
  {

    // All ones
    maskAll = _pp_init_ones();

    // All zeros
    maskIsNegative = _pp_init_ones(0);

    // Load vector of values from contiguous memory addresses
    _pp_vload_float(x, values + i, maskAll); // x = values[i];

    // Set mask according to predicate
    _pp_vlt_float(maskIsNegative, x, zero, maskAll); // if (x < 0) {

    // Execute instruction using mask ("if" clause)
    _pp_vsub_float(result, zero, x, maskIsNegative); //   output[i] = -x;

    // Inverse maskIsNegative to generate "else" mask
    maskIsNotNegative = _pp_mask_not(maskIsNegative); // } else {

    // Execute instruction ("else" clause)
    _pp_vload_float(result, values + i, maskIsNotNegative); //   output[i] = x; }

    // Write results back to memory
    _pp_vstore_float(output + i, result, maskAll);

    // i += (N - i) % VECTOR_WIDTH ? N % VECTOR_WIDTH : VECTOR_WIDTH;
  }
}

void clampedExpVector(float *values, int *exponents, float *output, int N)
{
  //
  // PP STUDENTS TODO: Implement your vectorized version of
  // clampedExpSerial() here.
  //
  // Your solution should work for any value of
  // N and VECTOR_WIDTH, not just when VECTOR_WIDTH divides N
  //
  __pp_vec_float x = _pp_vset_float(0);
  __pp_vec_int y = _pp_vset_int(0);
  __pp_vec_float result = _pp_vset_float(0);
  __pp_vec_float zero = _pp_vset_float(0.f);
  __pp_vec_int izero = _pp_vset_int(0);
  __pp_vec_int ione = _pp_vset_int(1);
  __pp_vec_float npn = _pp_vset_float(9.999999f);
  __pp_mask maskAll, maskIsZero, maskIsNotZero, maskGtNPN;

  for (int i = 0; i < N;) {
    int index = (N - i) % VECTOR_WIDTH ? N % VECTOR_WIDTH : VECTOR_WIDTH;
    maskAll = _pp_init_ones(index);
    maskIsZero = _pp_init_ones(0);

    _pp_vload_float(result, values + i, maskAll);
    _pp_vload_float(x, values + i, maskAll);
    _pp_vload_int(y, exponents + i, maskAll);

    _pp_veq_int(maskIsZero, y, izero, maskAll);
    _pp_vset_float(result, 1.f, maskIsZero);

    maskIsNotZero = _pp_mask_not(maskIsZero);
    maskGtNPN = _pp_mask_not(maskIsZero);

    _pp_vsub_int(y, y, ione, maskIsNotZero);
    _pp_vgt_int(maskIsNotZero, y, izero, maskIsNotZero);
    while (_pp_cntbits(maskIsNotZero)) {
      _pp_vmult_float(result, result, x, maskIsNotZero);
      _pp_vsub_int(y, y, ione, maskIsNotZero);
      _pp_vgt_int(maskIsNotZero, y, izero, maskIsNotZero);
    }

    _pp_vgt_float(maskGtNPN, result, npn, maskGtNPN);
    _pp_vset_float(result, 9.999999f, maskGtNPN);

    _pp_vstore_float(output + i, result, maskAll);
    i += index;
  }
}

// returns the sum of all elements in values
// You can assume N is a multiple of VECTOR_WIDTH
// You can assume VECTOR_WIDTH is a power of 2
float arraySumVector(float *values, int N)
{

  //
  // PP STUDENTS TODO: Implement your vectorized version of arraySumSerial here
  //

  for (int i = 0; i < N; i += VECTOR_WIDTH)
  {
  }

  return 0.0;
}