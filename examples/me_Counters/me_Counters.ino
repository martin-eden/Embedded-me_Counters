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

static volatile TUint_2 NumSecondsPassed = 0;
static volatile TUint_2 MilliSecondsPart = 0;

static me_Counters::TCounter2 Rtc;

const TUint_2 MaxSeconds = 10;

struct TTimestamp
{
  TUint_2 S;
  TUint_2 Ms;
};

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

TTimestamp MillisToTimestamp(
  TUint_2 NumMills
)
{
  TTimestamp Result;

  TUint_2 Remainder = NumMills;

  Result.Ms = Remainder % 1000;

  Remainder /= 1000;

  if (Remainder > MaxSeconds)
    Remainder = MaxSeconds;

  Result.S = Remainder;

  return Result;
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

void TestAddTs()
{
  TTimestamp TimeA = { 7, 625 };
  TTimestamp Delta = { 1, 373 };
  PrintTimestamp(TimeA);
  PrintTimestamp(Delta);
  AddTimestamp(&TimeA, Delta);
  PrintTimestamp(TimeA);
}

TTimestamp GetTime()
{
  return ToTimestamp(NumSecondsPassed, MilliSecondsPart);
}

TBool TimeIsBefore(
  TTimestamp Border
)
{
  TTimestamp Time = GetTime();

  if (Time.S < Border.S)
    return true;

  if (Time.S > Border.S)
    return false;

  if (Time.Ms < Border.Ms)
    return true;

  if (Time.Ms > Border.Ms)
    return false;

  return false;
}

TBool TimeIsAfter(
  TTimestamp BorderTs
)
{
  return !TimeIsBefore(BorderTs);
}

void WaitTill(
  TTimestamp Ts
)
{
  while (TimeIsBefore(Ts));
}

void WaitAfter(
  TTimestamp Ts
)
{
  while (TimeIsAfter(Ts));
  WaitTill(Ts);
}

void Delay(
  TTimestamp DeltaTs
)
{
  TTimestamp EndTs = GetTime();

  if (!AddTimestamp(&EndTs, DeltaTs))
    WaitAfter(EndTs);
  else
    WaitTill(EndTs);
}

void Delay_ms(
  TUint_2 NumMilliseconds
)
{
  Delay(MillisToTimestamp(NumMilliseconds));
}

extern "C" void __vector_11() __attribute__((interrupt, used));

// Interrupt 11 is for counter 2 mark A event
void __vector_11()
{
  ++MilliSecondsPart;

  if (MilliSecondsPart == 1000)
  {
    MilliSecondsPart = 0;
    ++NumSecondsPassed;

    if (NumSecondsPassed == MaxSeconds)
      NumSecondsPassed = 0;
  }
}

void setup()
{
  me_Uart::Init(me_Uart::Speed_115k_Bps);

  {
    using namespace me_Counters;

    Rtc.SetAlgorithm(TAlgorithm_Counter2::Count_ToMarkA);
    Rtc.Control->DriveSource = (TUint_1) TDriveSource_Counter2::Internal_SlowBy2Pow6;
    Rtc.Control->PinActionOnMarkA = (TUint_1) TPinAction::Toggle;
    *Rtc.Current = 0;
    *Rtc.MarkA = 249;
    Rtc.Wiring->EnableMarkAInterrupt = true;
  }

  const TUint_1 OutputPin = 9;
  pinMode(OutputPin, OUTPUT);

  TestAddTs();

  Console.Print("Init done.");
}

void loop()
{
  Delay_ms(1373);
  // Delay_us(653);
  // delay(1000);

  PrintTimestamp(GetTime());
  // Console.Print("Tic");
}
