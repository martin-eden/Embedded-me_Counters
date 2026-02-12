// Specific code for counter 3

/*
  Author: Martin Eden
  Last mod.: 2026-02-12
*/

#include <me_Counters.h>

#include <me_BaseTypes.h>

// Set mode of operation
void me_Counters::Counter3::TCounter::SetAlgorithm(
  me_Counters::Counter3::TAlgorithm Algorithm
)
{
  TUint_1 Bits12 = (TUint_1) Algorithm & 3;
  TUint_1 Bit3 = ((TUint_1) Algorithm >> 2) & 1;

  Control->Algorithm_Bits12 = Bits12;
  Control->Algorithm_Bit3 = Bit3;
}

// Get mode of operation
me_Counters::Counter3::TAlgorithm
  me_Counters::Counter3::TCounter::GetAlgorithm()
{
  TUint_1 Bits12 = Control->Algorithm_Bits12;
  TUint_1 Bit3 = Control->Algorithm_Bit3;

  TUint_1 AlgoNum = (Bit3 << 2) | Bits12;

  return (me_Counters::Counter3::TAlgorithm) AlgoNum;
}

// Get hardware constant for requested slowdown
TBool me_Counters::Counter3::Prescale_HwFromSw(
  TUint_1 * HwValue,
  TUint_1 Prescale_PowOfTwo
)
{
  switch (Prescale_PowOfTwo)
  {
    default: return false;
    case 0: *HwValue = (TUint_1) Counter3::TSpeed::Full; break;
    case 3: *HwValue = (TUint_1) Counter3::TSpeed::SlowBy2Pow3; break;
    case 5: *HwValue = (TUint_1) Counter3::TSpeed::SlowBy2Pow5; break;
    case 6: *HwValue = (TUint_1) Counter3::TSpeed::SlowBy2Pow6; break;
    case 7: *HwValue = (TUint_1) Counter3::TSpeed::SlowBy2Pow7; break;
    case 8: *HwValue = (TUint_1) Counter3::TSpeed::SlowBy2Pow8; break;
    case 10: *HwValue = (TUint_1) Counter3::TSpeed::SlowBy2Pow10; break;
  }

  return true;
}

// Get slowdown from hardware constant
TBool me_Counters::Counter3::Prescale_SwFromHw(
  TUint_1 * Prescale_PowOfTwo,
  TUint_1 HwValue
)
{
  switch (Counter3::TSpeed(HwValue))
  {
    default: return false;
    case Counter3::TSpeed::Full: *Prescale_PowOfTwo = 0; break;
    case Counter3::TSpeed::SlowBy2Pow3: *Prescale_PowOfTwo = 3; break;
    case Counter3::TSpeed::SlowBy2Pow5: *Prescale_PowOfTwo = 5; break;
    case Counter3::TSpeed::SlowBy2Pow6: *Prescale_PowOfTwo = 6; break;
    case Counter3::TSpeed::SlowBy2Pow7: *Prescale_PowOfTwo = 7; break;
    case Counter3::TSpeed::SlowBy2Pow8: *Prescale_PowOfTwo = 8; break;
    case Counter3::TSpeed::SlowBy2Pow10: *Prescale_PowOfTwo = 10; break;
  }

  return true;
}

/*
  2026-02-12
*/
