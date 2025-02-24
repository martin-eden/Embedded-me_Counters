// [me_Counters] test/demo

/*
  That's a test sketch where I'm experimenting.
*/

/*
  Author: Martin Eden
  Last mod.: 2025-02-24
*/

#include <me_Counters.h>

#include <me_BaseTypes.h>
#include <me_Uart.h>
#include <me_Console.h>

me_Counters::TCounter3 Rtc;

const TUint_2 MaxSeconds = 10;

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
  TUint_2 Millis = NumMills % 1000;
  TUint_2 Seconds = NumMills / 1000;

  if (Seconds >= MaxSeconds)
    return false;

  Result->Ms = Millis;
  Result->S = Seconds;

  return true;
}

TBool AddTimestamp(
  TTimestamp * Dest,
  TTimestamp Ts
)
{
  Dest->Ms += Ts.Ms;
  Dest->S += Ts.S;

  if (Dest->Ms >= 1000)
  {
    Dest->Ms -= 1000;
    ++Dest->S;
  }

  if (Dest->S >= MaxSeconds)
  {
    Dest->S -= MaxSeconds;
    return false;
  }

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

void Delay(
  TTimestamp DeltaTs
)
{
  TTimestamp EndTs = GetTime();
  AddTimestamp(&EndTs, DeltaTs);

  while (!TimestampIsLess(GetTime(), EndTs));
  while (TimestampIsLess(GetTime(), EndTs));

  if (TimestampIsLess(GetTime(), EndTs))
  {
    Console.Indent();

    Console.Print("WTF?!");

    Console.Write("Wait ");
    PrintTimestamp(EndTs);
    Console.Write("Real ");
    PrintTimestamp(GetTime());

    Console.Unindent();
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

extern "C" void __vector_7() __attribute__((signal, used));

// Interrupt 7 is for counter 3 mark A event
void __vector_7()
{
  ++RunTime.Ms;

  if (RunTime.Ms == 1000)
  {
    RunTime.Ms = 0;
    ++RunTime.S;

    if (RunTime.S == MaxSeconds)
      RunTime.S = 0;
  }
}

void TestAddTs()
{
  TTimestamp TimeA = { 8, 787 };
  TTimestamp Delta = { 1, 213 };
  PrintTimestamp(TimeA);
  PrintTimestamp(Delta);
  AddTimestamp(&TimeA, Delta);
  PrintTimestamp(TimeA);
}

void setup()
{
  me_Uart::Init(me_Uart::Speed_115k_Bps);

  // Set counter mark A to 1 ms
  {
    using namespace me_Counters;

    Rtc.SetAlgorithm(TAlgorithm_Counter3::Count_ToMarkA);
    Rtc.Control->Speed = (TUint_1) TSpeed_Counter3::SlowBy2Pow6;
    Rtc.Control->PinActionOnMarkA = (TUint_1) TPinAction::Toggle;
    *Rtc.Current = 0;
    *Rtc.MarkA = 249;
    Rtc.Wiring->EnableMarkAInterrupt = true;
  }

  const TUint_1 OutputPin = 11;
  pinMode(OutputPin, OUTPUT);

  // TestAddTs();

  Console.Print("Init done.");
}

void loop()
{
  Delay_ms(1100);
  // delay(1100);

  PrintTimestamp(GetTime());
  // Console.Print("Tic");
}
