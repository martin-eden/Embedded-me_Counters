// Hardware counters interface

/*
  Author: Martin Eden
  Last mod.: 2026-02-12
*/

/*
  There is no time. There are no timers.

  Just counters. Ticking with speed proportional to main speed.
*/

/*
  In a nutshell counter is value incrementer/decrementer that
  runs alongside main code.

  When value matches some other value it can toggle pin.
  Pins are hardcoded (hardwired). Also it can raise interrupts.

  Each counter has two memory slots for values to match.
  In code they are called Mark A and Mark B.

  Despite cognitive load from reading datasheet there are only
  two modes:

    * Count from 0 to N
    * Count from 0 to N to 0
*/

#pragma once

#include <me_BaseTypes.h>
#include "Common.h"
#include "Counter1.h"
#include "Counter2.h"
#include "Counter3.h"

namespace me_Counters
{
  // Ticks source / slowdown
  enum struct TDriveSource_Counters12;
  // Mode of operation
  enum struct TAlgorithm_Counters13;
  // Pin action on match
  enum struct TPinAction;
  // Status
  struct TStatus;
  // Interrupts
  struct TInterrupts;

  /*
    Counter 1

    Basis counter. Can run from pin pulses (but not in sleep modes).
    Can drive two pins.

    * 8-bit endurance

    Input pins (for Arduino Uno)

      4 Pulse

    Output pins

      6 Mark A
      5 Mark B
  */
  namespace Counter1
  {
    typedef TDriveSource_Counters12 TDriveSource;
    typedef TAlgorithm_Counters13 TAlgorithm;
    struct TBehavior;

    struct TCounter
    {
      volatile TStatus * Status = (TStatus *) 53;
      TInterrupts * Interrupts = (TInterrupts *) 110;
      volatile TBehavior * Control = (TBehavior *) 68;
      volatile TUint_1 * Current = (TUint_1 *) 70;
      TUint_1 * MarkA = (TUint_1 *) 71;
      TUint_1 * MarkB = (TUint_1 *) 72;

      // Get/set mode of operation
      TAlgorithm GetAlgorithm();
      void SetAlgorithm(TAlgorithm);
    };

    // [Speed calcs] Hardware constant from slowdown factor
    TBool Prescale_HwFromSw(TUint_1 * HwValue, TUint_1 Prescale_PowOfTwo);
    // [Speed calcs] Slowdown factor from hardware constant
    TBool Prescale_SwFromHw(TUint_1 * Prescale_PowOfTwo, TUint_1 HwValue);
  }

  /*
    Counter 2

    * 16-bit endurance
    * Wide set of algorithms
    * Can store current value on "event" input pin change.
      Can even count to that value.

    Input pins

      5 Pulse
      8 Event

    Output pins

       9 Mark A
      10 Mark B
  */
  namespace Counter2
  {
    typedef TDriveSource_Counters12 TDriveSource;
    enum struct TAlgorithm;
    struct TBehavior;

    struct TCounter
    {
      volatile TStatus * Status = (TStatus *) 54;
      TInterrupts * Interrupts = (TInterrupts *) 111;
      volatile TBehavior * Control = (TBehavior *) 128;
      volatile TUint_2 * Current = (TUint_2 *) 132;
      volatile TUint_2 * EventMark = (TUint_2 *) 134;
      TUint_2 * MarkA = (TUint_2 *) 136;
      TUint_2 * MarkB = (TUint_2 *) 138;

      TAlgorithm GetAlgorithm();
      void SetAlgorithm(TAlgorithm);
    };

    TBool Prescale_HwFromSw(TUint_1 * HwValue, TUint_1 Prescale_PowOfTwo);
    TBool Prescale_SwFromHw(TUint_1 * Prescale_PowOfTwo, TUint_1 HwValue);
  }

  /*
    Counter 3

    * 8-bit endurance
    * Can live from pulses from external oscillator (use case is 32 kiHz
      oscillator). This allows him to stay awake in many sleep modes.
    * Wide set of main clock downscaling.

    Input pins

      XTAL1 Pulse

    Output pins

      11 Mark A
       3 Mark B
  */
  namespace Counter3
  {
    enum struct TSpeed;
    typedef TAlgorithm_Counters13 TAlgorithm;
    struct TBehavior;
    struct TAsyncState;

    struct TCounter
    {
      volatile TStatus * Status = (TStatus *) 55;
      TInterrupts * Interrupts = (TInterrupts *) 112;
      volatile TBehavior * Control = (TBehavior *) 176;
      volatile TUint_1 * Current = (TUint_1 *) 178;
      TUint_1 * MarkA = (TUint_1 *) 179;
      TUint_1 * MarkB = (TUint_1 *) 180;
      TAsyncState * AsyncState = (TAsyncState *) 182;

      TAlgorithm GetAlgorithm();
      void SetAlgorithm(TAlgorithm);
    };

    TBool Prescale_HwFromSw(TUint_1 * HwValue, TUint_1 Prescale_PowOfTwo);
    TBool Prescale_SwFromHw(TUint_1 * Prescale_PowOfTwo, TUint_1 HwValue);
  }

  // Control layout for all three counters
  struct TCountersControlStruc;

  // All counters start/stop
  struct TCountersControl
  {
    TCountersControlStruc * Control = (TCountersControlStruc *) 67;

    void Start();
    void Stop();
  };
}

/*
  2024 # #
  2025 # # # # # # # # #
  2026-02-12
*/
