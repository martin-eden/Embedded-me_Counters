// Specific code for counter 2

/*
  Author: Martin Eden
  Last mod.: 2026-02-12
*/

#include <me_Counters.h>

#include <me_BaseTypes.h>

// Set mode of operation
void me_Counters::Counter2::TCounter::SetAlgorithm(
  me_Counters::Counter2::TAlgorithm Algorithm
)
{
  TUint_1 Bits12 = (TUint_1) Algorithm & 3;
  TUint_1 Bits34 = ((TUint_1) Algorithm >> 2) & 3;

  Control->Algorithm_Bits12 = Bits12;
  Control->Algorithm_Bits34 = Bits34;
}

// Get mode of operation
me_Counters::Counter2::TAlgorithm
  me_Counters::Counter2::TCounter::GetAlgorithm()
{
  TUint_1 Bits12 = Control->Algorithm_Bits12;
  TUint_1 Bits34 = Control->Algorithm_Bits34;

  TUint_1 AlgoNum = (Bits34 << 2) | Bits12;

  return (me_Counters::Counter2::TAlgorithm) AlgoNum;
}

// Get hardware constant for requested slowdown
TBool me_Counters::Counter2::Prescale_HwFromSw(
  TUint_1 * HwValue,
  TUint_1 Prescale_PowOfTwo
)
{
  // Counters 1 and 2 have same prescale options
  return
    me_Counters::Counter1::Prescale_HwFromSw(HwValue, Prescale_PowOfTwo);
}

// Get slowdown from hardware constant
TBool me_Counters::Counter2::Prescale_SwFromHw(
  TUint_1 * Prescale_PowOfTwo,
  TUint_1 HwValue
)
{
  return
    me_Counters::Counter1::Prescale_SwFromHw(Prescale_PowOfTwo, HwValue);
}

// )
/*
  2026-02-12
*/
