// Hardware counters functions

/*
  Author: Martin Eden
  Last mod.: 2025-11-30
*/

#include <me_Counters.h>

#include <me_BaseTypes.h>
#include <me_HardwareClockScaling.h>

using namespace me_Counters;

// ( All counters control

void TCountersControl::Start()
{
  Control->StopCounters = false;
}

void TCountersControl::Stop()
{
  Control->StopCounters = true;
}

// )

// ( Counter 1

void TCounter1::SetAlgorithm(
  TAlgorithm_Counter1 Algorithm
)
{
  TUint_1 Bits12 = (TUint_1) Algorithm & 3;
  TUint_1 Bit3 = ((TUint_1) Algorithm >> 2) & 1;

  Control->Algorithm_Bits12 = Bits12;
  Control->Algorithm_Bit3 = Bit3;
}

TAlgorithm_Counter1 TCounter1::GetAlgorithm()
{
  TUint_1 Bits12 = Control->Algorithm_Bits12;
  TUint_1 Bit3 = Control->Algorithm_Bit3;

  TUint_1 AlgoNum = (Bit3 << 2) | Bits12;

  return (TAlgorithm_Counter1) AlgoNum;
}

// )

// ( Counter 2

// ( Algorithm

void TCounter2::SetAlgorithm(
  TAlgorithm_Counter2 Algorithm
)
{
  TUint_1 Bits12 = (TUint_1) Algorithm & 3;
  TUint_1 Bits34 = ((TUint_1) Algorithm >> 2) & 3;

  Control->Algorithm_Bits12 = Bits12;
  Control->Algorithm_Bits34 = Bits34;
}

TAlgorithm_Counter2 TCounter2::GetAlgorithm()
{
  TUint_1 Bits12 = Control->Algorithm_Bits12;
  TUint_1 Bits34 = Control->Algorithm_Bits34;

  TUint_1 AlgoNum = (Bits34 << 2) | Bits12;

  return (TAlgorithm_Counter2) AlgoNum;
}

// )

// )

// ( Counter 3

void TCounter3::SetAlgorithm(
  TAlgorithm_Counter3 Algorithm
)
{
  TUint_1 Bits12 = (TUint_1) Algorithm & 3;
  TUint_1 Bit3 = ((TUint_1) Algorithm >> 2) & 1;

  Control->Algorithm_Bits12 = Bits12;
  Control->Algorithm_Bit3 = Bit3;
}

TAlgorithm_Counter3 TCounter3::GetAlgorithm()
{
  TUint_1 Bits12 = Control->Algorithm_Bits12;
  TUint_1 Bit3 = Control->Algorithm_Bit3;

  TUint_1 AlgoNum = (Bit3 << 2) | Bits12;

  return (TAlgorithm_Counter3) AlgoNum;
}

// )

// ( Convert slowdown factor to hardware constant

TBool me_Counters::GetPrescaleConst_Counter1(
  TUint_1 * Result,
  TUint_1 Prescale_PowOfTwo
)
{
  switch (Prescale_PowOfTwo)
  {
    default:
      return false;
    case 0:
      *Result = (TUint_1) TDriveSource_Counter1::Internal_FullSpeed;
      break;
    case 3:
      *Result = (TUint_1) TDriveSource_Counter1::Internal_SlowBy2Pow3;
      break;
    case 6:
      *Result = (TUint_1) TDriveSource_Counter1::Internal_SlowBy2Pow6;
      break;
    case 8:
      *Result = (TUint_1) TDriveSource_Counter1::Internal_SlowBy2Pow8;
      break;
    case 10:
      *Result = (TUint_1) TDriveSource_Counter1::Internal_SlowBy2Pow10;
      break;
  }

  return true;
}

TBool me_Counters::GetPrescaleConst_Counter2(
  TUint_1 * Result,
  TUint_1 Prescale_PowOfTwo
)
{
  // Counters 1 and 2 have same prescale options
  return GetPrescaleConst_Counter1(Result, Prescale_PowOfTwo);
}

TBool me_Counters::GetPrescaleConst_Counter3(
  TUint_1 * Result,
  TUint_1 Prescale_PowOfTwo
)
{
  switch (Prescale_PowOfTwo)
  {
    default:
      return false;
    case 0:
      *Result = (TUint_1) TSpeed_Counter3::Full;
      break;
    case 3:
      *Result = (TUint_1) TSpeed_Counter3::SlowBy2Pow3;
      break;
    case 5:
      *Result = (TUint_1) TSpeed_Counter3::SlowBy2Pow5;
      break;
    case 6:
      *Result = (TUint_1) TSpeed_Counter3::SlowBy2Pow6;
      break;
    case 7:
      *Result = (TUint_1) TSpeed_Counter3::SlowBy2Pow7;
      break;
    case 8:
      *Result = (TUint_1) TSpeed_Counter3::SlowBy2Pow8;
      break;
    case 10:
      *Result = (TUint_1) TSpeed_Counter3::SlowBy2Pow10;
      break;
  }

  return true;
}

// )



/*
  2025-01-01
  2025-01-09
  2025-02-21
  2025-11-26
  2025-11-30
*/
