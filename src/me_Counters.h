// Hardware counters interface

/*
  Author: Martin Eden
  Last mod.: 2025-02-24
*/

/*
  There is no time. There are no timers.

  Just counters. Ticking with speed proportional to main speed.

  ( Yes, main speed is typically fixed to some millions pulses per
    second. It's even called "clock" an pulses are called "ticks".
  ) Still, by design it's just counters.
*/

/*
  There are three counters on ATmega328/P.

  Counter concept

    Input (Alive, N, MarkA, MarkB)
    Output (MarkAEvent, MarkBEvent, ReachedEndEvent)
    Logic (
      while Alive
        for i = 1, N
          Sleep()
          if (i == MarkA)
            MarkAEvent()
          if (i == MarkB)
            MarkBEvent()
        ReachedEndEvent()
    )

  This code runs parallel with main code. Events are just setting bits
  in memory. Hardware magic of that bits allows to call interrupt
  handler in main code.

  ( There is fancier symmetric loop:

    for 1, N (...);
    for N, 1 (...);

  ) that is used for "correct" PWM modes.
*/

#include <me_BaseTypes.h>

namespace me_Counters
{
  // Ticks source / slowdowns

  enum struct TDriveSource_Counters12
  {
    None = 0,
    Internal_Fullspeed = 1,
    Internal_SlowBy2Pow3 = 2,
    Internal_SlowBy2Pow6 = 3,
    Internal_SlowBy2Pow8 = 4,
    Internal_SlowBy2Pow10 = 5,
    External_Downbeat = 6,
    External_Upbeat = 7,
  };

  enum struct TSpeed_Counter3
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

  typedef TDriveSource_Counters12 TDriveSource_Counter1;
  typedef TDriveSource_Counters12 TDriveSource_Counter2;

  // Modes of operation

  enum struct TAlgorithm_Counters13
  {
    Count_To2Pow8 = 0,
    Count_ToMarkA = 2,
    Pwm_To2Pow8 = 1,
    Pwm_ToMarkA = 5,
    FastPwm_To2Pow8 = 3,
    FastPwm_ToMarkA = 7,
  };

  enum struct TAlgorithm_Counter2
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

  typedef TAlgorithm_Counters13 TAlgorithm_Counter1;
  typedef TAlgorithm_Counters13 TAlgorithm_Counter3;

  enum struct TPinAction
  {
    Nothing = 0,
    Toggle = 1,
    Clear = 2,
    Set = 3,
  };

  // Status

  struct TStatus // 053 | 054 | 055
  {
    TBool Done : 1;         // 1
    TBool GotMarkA : 1;     // 2
    TBool GotMarkB : 1;     // 3
    TUint_1 : 2;            // 4 5
    TBool GotEventMark : 1; // 6 -- exclusive for counter 2
    TUint_1 : 2;            // 7 8
  };

  struct TAsyncState // 182
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

  // Mode of operation

  struct TBehavior_Counter1 // 068
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

  struct TBehavior_Counter2 // 128
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

  /*
    Layout is the same as for counter 1 but I want field name
    "Speed" instead of "DriveSource".
  */
  struct TBehavior_Counter3 // 176
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

  // Interrupts

  struct TInterrupts // 110 | 111 | 112
  {
    TBool EnableOverflowInterrupt : 1; // 1
    TBool EnableMarkAInterrupt : 1;    // 2
    TBool EnableMarkBInterrupt : 1;    // 3
    TUint_1 : 2;                       // 4 5
    TBool EnableEventInterrupt : 1;    // 6 -- only for counter 2 at 111
    TUint_1 : 2;                       // 7 8
  };

  /*
    Counter 1

    Basis counter. Can run from pin pulses (but not in sleep modes).
    Can drive two pins.

    * 8-bit endurance

    Input pins (for Arduino Uno)

      4 Pulse

    Output pins

      5 MarkB
      6 MarkA
  */
  struct TCounter1
  {
    volatile TStatus * Status = (TStatus *) 53;
    TInterrupts * Wiring = (TInterrupts *) 110;
    TBehavior_Counter1 * Control = (TBehavior_Counter1 *) 68;
    volatile TUint_1 * Current = (TUint_1 *) 70;
    TUint_1 * MarkA = (TUint_1 *) 71;
    TUint_1 * MarkB = (TUint_1 *) 72;

    void SetAlgorithm(TAlgorithm_Counter1);
    TAlgorithm_Counter1 GetAlgorithm();
  };

  /*
    Counter 2

    * 16-bit endurance
    * Can store current value on "event" input pin change.
      Can even count to that value.

    Input pins

      5 Pulse
      8 Event

    Output pins

       9 MarkA
      10 MarkB
  */
  struct TCounter2
  {
    volatile TStatus * Status = (TStatus *) 54;
    TInterrupts * Wiring = (TInterrupts *) 111;
    TBehavior_Counter2 * Control = (TBehavior_Counter2 *) 128;
    volatile TUint_2 * Current = (TUint_2 *) 132;
    TUint_2 * EventMark = (TUint_2 *) 134;
    TUint_2 * MarkA = (TUint_2 *) 136;
    TUint_2 * MarkB = (TUint_2 *) 138;

    void SetAlgorithm(TAlgorithm_Counter2);
    TAlgorithm_Counter2 GetAlgorithm();
  };

  /*
    Counter 3

    * 8-bit endurance
    * Can live from pulses from external oscillator (use case is 32 kiHz
      oscillator). This allows him to stay awake in many sleep modes.

    Input pins

      XTAL1 Pulse

    Output pins

       3 MarkB
      11 MarkA
  */
  struct TCounter3
  {
    volatile TStatus * Status = (TStatus *) 55;
    TInterrupts * Wiring = (TInterrupts *) 112;
    TBehavior_Counter3 * Control = (TBehavior_Counter3 *) 176;
    volatile TUint_1 * Current = (TUint_1 *) 178;
    TUint_1 * MarkA = (TUint_1 *) 179;
    TUint_1 * MarkB = (TUint_1 *) 180;
    TAsyncState * AsyncState = (TAsyncState *) 182;

    void SetAlgorithm(TAlgorithm_Counter3);
    TAlgorithm_Counter3 GetAlgorithm();
  };

  struct TCountersControlStruc // 067
  {
    TBool ResetCounters12Prescaler : 1; // 1
    TBool ResetCounter3Prescaler : 1;   // 2
    TUint_1 : 5;                        // 3 4 5 6 7
    TBool StopCounters : 1;             // 8
  };

  struct TCountersControl
  {
    TCountersControlStruc * Control = (TCountersControlStruc *) 67;

    void Start();
    void Stop();
  };
}

/*
  2024-12-29
  2024-12-30
  2025-01-01
  2025-02-21
  2025-02-23
  2025-02-24
*/
