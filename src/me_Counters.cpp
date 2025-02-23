// Hardware counters functions

/*
  Author: Martin Eden
  Last mod.: 2025-02-21
*/

#include <me_Counters.h>

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

/*
  2025-01-01
  2025-01-09
  2025-02-21
*/
