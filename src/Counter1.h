// Counter 1 specific datatypes

/*
  Author: Martin Eden
  Last mod.: 2026-02-12
*/

#pragma once

#include <me_Counters.h>

#include <me_BaseTypes.h>

namespace me_Counters::Counter1
{
  // "Control"
  struct TBehavior
  {
    // Byte 1
    TUint_1 Algorithm_Bits12 : 2; // 1 2
    TUint_1 : 2;                  // 3 4
    TUint_1 PinActionOnMarkB : 2; // 5 6
    TUint_1 PinActionOnMarkA : 2; // 7 8
    // Byte 2
    TUint_1 DriveSource : 3;  // 1 2 3
    TBool Algorithm_Bit3 : 1; // 4
    TUint_1 : 2;              // 5 6
    TBool ImitateMarkB : 1;   // 7
    TBool ImitateMarkA : 1;   // 8
  };
}

/*
  2026-02-12
*/
