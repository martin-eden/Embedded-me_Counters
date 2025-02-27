// [me_Counters] test/demo

/*
  That's a test sketch where I'm experimenting.
*/

/*
  Author: Martin Eden
  Last mod.: 2025-02-27
*/

#include <me_Counters.h>

#include <me_BaseTypes.h>
#include <me_Uart.h>
#include <me_Console.h>

me_Counters::TCounter3 Rtc;

const TUint_2 MaxSeconds = 10;

const TUint_1 DebugPin = 13;

struct TTimestamp
{
  TUint_2 S;
  TUint_2 Ms;
};

TTimestamp RunTime = { 0, 0 };

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

TTimestamp ToTimestamp(
  TUint_2 S,
  TUint_2 Ms
)
{
  TTimestamp Result;

  Result.S = S;
  Result.Ms = Ms;

  return Result;
}

TBool MillisToTimestamp(
  TTimestamp * Result,
  TUint_2 NumMills
)
{
  Result->Ms = NumMills % 1000;
  Result->S = NumMills / 1000;

  if (Result->S >= MaxSeconds)
    return false;

  return true;
}

TTimestamp GetTime()
{
  TTimestamp Result;

  TUint_1 PrevSreg = SREG;
  cli();
  Result = RunTime;
  SREG = PrevSreg;

  return Result;
}

TBool TimestampIsLess(
  TTimestamp Ts,
  TTimestamp Border
)
{
  if (Ts.S < Border.S)
    return true;

  if (Ts.S > Border.S)
    return false;

  if (Ts.Ms < Border.Ms)
    return true;

  if (Ts.Ms > Border.Ms)
    return false;

  return false;
}

TBool BalanceTimestamp(
  TTimestamp * Ts
)
{
  TBool Result = false;

  while (Ts->Ms >= 1000)
  {
    Ts->Ms -= 1000;
    ++Ts->S;
  }

  if (Ts->S >= MaxSeconds)
    Result = true;

  while (Ts->S >= MaxSeconds)
    Ts->S -= MaxSeconds;

  return Result;
}

// TBool AddTimestamp(TTimestamp *, TTimestamp) __attribute__ ((optimize("O0")));

TBool AddTimestamp(
  TTimestamp * Dest,
  TTimestamp Ts
)
{
  Dest->Ms += Ts.Ms;
  Dest->S += Ts.S;

  TBool IsWrapped = BalanceTimestamp(Dest);

  if (IsWrapped)
    return false;

  return true;
}

TBool TimestampIsNonZero(
  TTimestamp Ts
)
{
  return (Ts.S != 0) || (Ts.Ms != 0);
}

void Delay(
  TTimestamp DeltaTs
)
{
  const TUint_4 MaxNumIters = (TUint_4) 256 * 64 * 1000 * 2;
  TUint_4 NumIters;

  TTimestamp EndTs = GetTime();
  TBool IsWrapped = AddTimestamp(&EndTs, DeltaTs);

  if (IsWrapped)
  {
    NumIters = 0;
    while (TimestampIsNonZero(GetTime()))
    {
      ++NumIters;
      if (NumIters > MaxNumIters)
      {
        Console.Print("Still suck.");
        PrintTimestamp(EndTs);
        PrintTimestamp(GetTime());
        while (true);
      }
    }
  }

  NumIters = 0;
  while (TimestampIsLess(GetTime(), EndTs))
  {
    ++NumIters;
    if (NumIters > MaxNumIters)
    {
      Console.Print("Suck.");
      PrintTimestamp(EndTs);
      PrintTimestamp(GetTime());
      while (true);
    }
  }
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

// Set time for next tick
void AdvanceTime()
{
  ++RunTime.Ms;

  if (RunTime.Ms == 1000)
    digitalWrite(DebugPin, !digitalRead(DebugPin));

  BalanceTimestamp(&RunTime);
}

extern "C" void __vector_7() __attribute__((signal, used));

// Interrupt 7 is for counter 3 mark A event. Expected to trigger every ms
void __vector_7()
{
  AdvanceTime();
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
  Delay_ms(1100);
  // delay(1100);

  // PrintTimestamp(GetTime());
  // Console.Print("Tic");
}
