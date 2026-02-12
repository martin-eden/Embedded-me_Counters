// Counter 2 specific datatypes

/*
  Author: Martin Eden
  Last mod.: 2026-02-12
*/

#pragma once

#include <me_Counters.h>

#include <me_BaseTypes.h>

namespace me_Counters::Counter2
{
  // Mode of operation
  enum struct TAlgorithm
  {
    Count_To2Pow16 = 0,
    Count_ToMarkA = 4,
    Count_ToEventMark = 12,
    SymPwm_ToMarkA = 9,
    SymPwm_ToEventMark = 8,
    Pwm_To2Pow8 = 1,
    Pwm_To2Pow9 = 2,
    Pwm_To2Pow10 = 3,
    Pwm_ToMarkA = 11,
    Pwm_ToEventMark = 10,
    FastPwm_To2Pow8 = 5,
    FastPwm_To2Pow9 = 6,
    FastPwm_To2Pow10 = 7,
    FastPwm_ToMarkA = 15,
    FastPwm_ToEventMark = 14,
  };

  // "Control"
  struct TBehavior
  {
    // Byte 1
    TUint_1 Algorithm_Bits12 : 2; // 1 2
    TUint_1 : 2;                  // 3 4
    TUint_1 PinActionOnMarkB : 2; // 5 6
    TUint_1 PinActionOnMarkA : 2; // 7 8
    // Byte 2
    TUint_1 DriveSource : 3;        // 1 2 3
    TUint_1 Algorithm_Bits34 : 2;   // 4 5
    TUint_1 : 1;                    // 6
    TBool EventIsOnUpbeat : 1;      // 7
    TBool EnableInputDebouncer : 1; // 8
    // Byte 3
    TUint_1 : 6;            // 1 2 3 4 5 6
    TBool ImitateMarkB : 1; // 7
    TBool ImitateMarkA : 1; // 8
  };
}

/*
  2026-02-12
*/
