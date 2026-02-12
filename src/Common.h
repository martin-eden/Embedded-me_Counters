// Common type definitions for counters

/*
  Author: Martin Eden
  Last mod.: 2026-02-12
*/

#pragma once

#include <me_Counters.h>

#include <me_BaseTypes.h>

namespace me_Counters
{
  // Ticks handling for counters 1 and 2
  enum struct TDriveSource_Counters12
  {
    None = 0,
    Internal_FullSpeed = 1,
    Internal_SlowBy2Pow3 = 2,
    Internal_SlowBy2Pow6 = 3,
    Internal_SlowBy2Pow8 = 4,
    Internal_SlowBy2Pow10 = 5,
    External_Downbeat = 6,
    External_Upbeat = 7,
  };

  // Mode of operation for counters 1 and 3
  enum struct TAlgorithm_Counters13
  {
    Count_To2Pow8 = 0,
    Count_ToMarkA = 2,
    Pwm_To2Pow8 = 1,
    Pwm_ToMarkA = 5,
    FastPwm_To2Pow8 = 3,
    FastPwm_ToMarkA = 7,
  };

  /*
    Pin action on match

    For some modes meaning of "Set" and "Clear" is reversed.
    As it's context-dependent we can't have proper names at this level.
  */
  enum struct TPinAction
  {
    None = 0,
    Toggle = 1,
    Set = 2,
    Clear = 3,
  };

  // Status
  struct TStatus
  {
    TBool Done : 1;         // 1
    TBool GotMarkA : 1;     // 2
    TBool GotMarkB : 1;     // 3
    TUint_1 : 2;            // 4 5
    TBool GotEventMark : 1; // 6 -- only for counter 2
    TUint_1 : 2;            // 7 8
  };

  // Interrupts
  struct TInterrupts
  {
    TBool OnDone : 1;  // 1
    TBool OnMarkA : 1; // 2
    TBool OnMarkB : 1; // 3
    TUint_1 : 2;       // 4 5
    TBool OnEvent : 1; // 6 -- only for counter 2
    TUint_1 : 2;       // 7 8
  };

  // Control layout for all three counters
  struct TCountersControlStruc
  {
    TBool ResetCounters12Prescaler : 1; // 1
    TBool ResetCounter3Prescaler : 1;   // 2
    TUint_1 : 5;                        // 3 4 5 6 7
    TBool StopCounters : 1;             // 8
  };
}

/*
  2026-02-12
*/
