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

#include <me_BaseTypes.h>

namespace me_Counters
{
  // Ticks source / slowdowns

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
    TBool OnDone : 1;  // 1
    TBool OnMarkA : 1; // 2
    TBool OnMarkB : 1; // 3
    TUint_1 : 2;       // 4 5
    TBool OnEvent : 1; // 6 -- only for counter 2 at 111
    TUint_1 : 2;       // 7 8
  };

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
  struct TCounter1
  {
    volatile TStatus * Status = (TStatus *) 53;
    TInterrupts * Interrupts = (TInterrupts *) 110;
    volatile TBehavior_Counter1 * Control = (TBehavior_Counter1 *) 68;
    volatile TUint_1 * Current = (TUint_1 *) 70;
    TUint_1 * MarkA = (TUint_1 *) 71;
    TUint_1 * MarkB = (TUint_1 *) 72;

    void SetAlgorithm(TAlgorithm_Counter1);
    TAlgorithm_Counter1 GetAlgorithm();
  };

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
  struct TCounter2
  {
    volatile TStatus * Status = (TStatus *) 54;
    TInterrupts * Interrupts = (TInterrupts *) 111;
    volatile TBehavior_Counter2 * Control = (TBehavior_Counter2 *) 128;
    volatile TUint_2 * Current = (TUint_2 *) 132;
    volatile TUint_2 * EventMark = (TUint_2 *) 134;
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
    * Wide set of main clock downscaling.

    Input pins

      XTAL1 Pulse

    Output pins

      11 Mark A
       3 Mark B
  */
  struct TCounter3
  {
    volatile TStatus * Status = (TStatus *) 55;
    TInterrupts * Interrupts = (TInterrupts *) 112;
    volatile TBehavior_Counter3 * Control = (TBehavior_Counter3 *) 176;
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

  // Convert slowdown factor to hardware constant
  TBool Prescale_HwFromSw_Counter1(TUint_1 * HwValue, TUint_1 Prescale_PowOfTwo);
  TBool Prescale_HwFromSw_Counter2(TUint_1 * HwValue, TUint_1 Prescale_PowOfTwo);
  TBool Prescale_HwFromSw_Counter3(TUint_1 * HwValue, TUint_1 Prescale_PowOfTwo);

  // Reverse conversion: slowdown factor from hardware constant
  TBool Prescale_SwFromHw_Counter1(TUint_1 * Prescale_PowOfTwo, TUint_1 HwValue);
  TBool Prescale_SwFromHw_Counter2(TUint_1 * Prescale_PowOfTwo, TUint_1 HwValue);
  TBool Prescale_SwFromHw_Counter3(TUint_1 * Prescale_PowOfTwo, TUint_1 HwValue);
}

/*
  2024 # #
  2025 # # # # # # # # #
*/
