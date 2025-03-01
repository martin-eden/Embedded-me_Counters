// Run-time wallclock using hardware counter

/*
  Author: Martin Eden
  Last mod.: 2025-03-01
*/

/*
  Idea is to tune hardware counter as timer ticking every millisecond.
  Every millisecond interrupt occurs.

  In interrupt handler we're updating "run time" which is number of
  seconds and number of milliseconds.

  Time wraps around at 1000 seconds. Of course it can be extended
  to support longer run times but we had no practical need for this.

  So time granularity is 1 ms. It can be extended to 1/250 ms (4 us)
  but again, no practical need for that.

  As a demo, we realized delay_ms() function as loop waiting for
  rich clock time. It has error margin up to 1 ms. For precise
  milli- and micro-seconds delays one should use tuned busy loops.
*/

#include <me_Counters.h>

#include <me_BaseTypes.h>
#include <me_Uart.h>
#include <me_Console.h>

me_Counters::TCounter3 Rtc;

const TUint_2 MaxSeconds = 1000;

const TUint_1 DebugPin = 13;

struct TTimestamp
{
  TUint_2 S;
  TUint_2 Ms;
};

volatile TTimestamp RunTime = { 0, 0 };

TTimestamp GetTime()
{
  TTimestamp Result;

  TUint_1 PrevSreg = SREG;
  cli();
  // Fucking great language: you can have "volatile" record but can't copy it
  Result.S = RunTime.S;
  Result.Ms = RunTime.Ms;
  SREG = PrevSreg;

  return Result;
}

void SetTime(
  TTimestamp Ts
)
{
  TUint_1 PrevSreg = SREG;
  cli();
  RunTime.S = Ts.S;
  RunTime.Ms = Ts.Ms;
  SREG = PrevSreg;
}

void PrintTimestamp(
  TTimestamp Ts
)
{
  Console.Write("Timestamp (");
  Console.Print(Ts.S);
  Console.Print(Ts.Ms);
  Console.Write(")");
  Console.EndLine();
}

TBool ToTimestamp(
  TTimestamp * Result,
  TUint_2 S,
  TUint_2 Ms
)
{
  if ((S >= MaxSeconds) || (Ms >= 1000))
    return false;

  Result->S = S;
  Result->Ms = Ms;

  return true;
}

TBool MillisToTimestamp(
  TTimestamp * Result,
  TUint_2 NumMills
)
{
  return ToTimestamp(Result, NumMills / 1000, NumMills % 1000);
}

TSint_1 CompareTimestamps(
  TTimestamp A,
  TTimestamp B
)
{
  if (A.S < B.S)
    return -1;

  if (A.S > B.S)
    return 1;

  if (A.Ms < B.Ms)
    return -1;

  if (A.Ms > B.Ms)
    return 1;

  return 0;
}

TBool BalanceTimestamp(
  TTimestamp * Ts
)
{
  TBool IsWrapped = false;

  while (Ts->Ms >= 1000)
  {
    Ts->Ms -= 1000;
    ++Ts->S;
  }

  if (Ts->S >= MaxSeconds)
    IsWrapped = true;

  while (Ts->S >= MaxSeconds)
    Ts->S -= MaxSeconds;

  return IsWrapped;
}

TBool AddTimestamp(
  TTimestamp * Dest,
  TTimestamp Ts
)
{
  Dest->S += Ts.S;
  Dest->Ms += Ts.Ms;

  TBool IsWrapped = BalanceTimestamp(Dest);

  if (IsWrapped)
    return false;

  return true;
}

void Delay(
  TTimestamp DeltaTs
)
{
  TTimestamp EndTs = GetTime();
  TBool IsWrapped = !AddTimestamp(&EndTs, DeltaTs);

  if (IsWrapped)
    while (CompareTimestamps(GetTime(), EndTs) >= 0);

  while (CompareTimestamps(GetTime(), EndTs) < 0);
}

TBool Delay_ms(
  TUint_2 NumMilliseconds
)
{
  TTimestamp DelayTs;

  if (!MillisToTimestamp(&DelayTs, NumMilliseconds))
    return false;

  Delay(DelayTs);

  return true;
}

extern "C" void __vector_7() __attribute__((signal, used));

// Interrupt 7 is for counter 3 mark A event. Expected to trigger every ms
void __vector_7()
{
  TTimestamp Ts = GetTime();

  ++Ts.Ms;
  BalanceTimestamp(&Ts);

  SetTime(Ts);
}

void setup()
{
  me_Uart::Init(me_Uart::Speed_115k_Bps);

  // Set counter mark A to 1 ms
  {
    using namespace me_Counters;

    Rtc.SetAlgorithm(TAlgorithm_Counter3::Count_ToMarkA);
    Rtc.Control->Speed = (TUint_1) TSpeed_Counter3::SlowBy2Pow6;
    *Rtc.Current = 0;
    *Rtc.MarkA = 249;
    Rtc.Wiring->EnableMarkAInterrupt = true;
  }

  pinMode(DebugPin, OUTPUT);

  Console.Print("Init done.");
}

void loop()
{
  Delay_ms(1760);

  digitalWrite(DebugPin, !digitalRead(DebugPin));

  PrintTimestamp(GetTime());
}
