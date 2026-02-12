// Counter 3 specific datatypes

/*
  Author: Martin Eden
  Last mod.: 2026-02-12
*/

#pragma once

#include <me_Counters.h>

#include <me_BaseTypes.h>

namespace me_Counters::Counter3
{
  // Slowdown
  enum struct TSpeed
  {
    None = 0,
    Full = 1,
    SlowBy2Pow3 = 2,
    SlowBy2Pow5 = 3,
    SlowBy2Pow6 = 4,
    SlowBy2Pow7 = 5,
    SlowBy2Pow8 = 6,
    SlowBy2Pow10 = 7,
  };

  /*
    Mode of operation

    Layout is the same as for counter 1 but I wanted field name
    "Speed" instead of "DriveSource".
  */
  struct TBehavior
  {
    // Byte 1
    TUint_1 Algorithm_Bits12 : 2; // 1 2
    TUint_1 : 2;                  // 3 4
    TUint_1 PinActionOnMarkB : 2; // 5 6
    TUint_1 PinActionOnMarkA : 2; // 7 8
    // Byte 2
    TUint_1 Speed : 3;        // 1 2 3
    TBool Algorithm_Bit3 : 1; // 4
    TUint_1 : 2;              // 5 6
    TBool ImitateMarkB : 1;   // 7
    TBool ImitateMarkA : 1;   // 8
  };

  // Async mode state
  struct TAsyncState
  {
    TBool IsUpdatingBehaviorByte2 : 1; // 1
    TBool IsUpdatingBehaviorByte1 : 1; // 2
    TBool IsUpdatingMarkB : 1;         // 3
    TBool IsUpdatingMarkA : 1;         // 4
    TBool IsUpdatingCurrent : 1;       // 5
    TBool RunFromExternalClock : 1;    // 6
    TBool EnableExternalClock : 1;     // 7
    TUint_1 : 1;                       // 8
  };
}

/*
  2026-02-12
*/
