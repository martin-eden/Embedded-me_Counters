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

static volatile TUint_4 NumSecondsPassed = 0;
static volatile TUint_2 MilliSecondsPart = 0;
static me_Counters::TCounter2 Rtc;

const TUint_2 MaxSeconds = 10;

struct TTimestamp
{
  TUint_2 S;
  TUint_2 Ms;
  TUint_2 Us;
};

const TTimestamp ZeroTimestamp = { 0, 0, 0 };

void PrintTimestamp(
  TTimestamp Ts
)
{
  Console.Write("Timestamp (");
  Console.Print(Ts.S);
  Console.Print(Ts.Ms);
  Console.Print(Ts.Us);
  Console.Write(")");
  Console.EndLine();
}

TTimestamp ToTimestamp(
  TUint_2 S,
  TUint_2 Ms,
  TUint_2 Us
)
{
  TTimestamp Result;

  Result.S = S;
  Result.Ms = Ms;
  Result.Us = Us;

  return Result;
}

TTimestamp MicrosToTimestamp(
  TUint_2 NumMicros
)
{
  TTimestamp Result;

  TUint_2 Remainder = NumMicros;

  Result.Us = Remainder % 1000;

  Remainder /= 1000;

  Result.Ms = Remainder % 1000;

  Remainder /= 1000;

  if (Remainder > MaxSeconds)
    Remainder = MaxSeconds;

  Result.S = Remainder;

  return Result;
}

TTimestamp MillisToTimestamp(
  TUint_2 NumMills
)
{
  TTimestamp Result;

  TUint_2 Remainder = NumMills;

  Result.Us = 0;

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
  Dest->Us += Ts.Us;
  Dest->Ms += Ts.Ms;
  Dest->S += Ts.S;

  if (Dest->Us >= 1000)
  {
    Dest->Us -= 1000;
    ++Dest->Ms;
  }

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

TBool TimestampIsBefore(
  TTimestamp CurrentTs,
  TTimestamp BorderTs
)
{
  if (CurrentTs.S < BorderTs.S)
    return true;

  if (CurrentTs.S > BorderTs.S)
    return false;

  if (CurrentTs.Ms < BorderTs.Ms)
    return true;

  if (CurrentTs.Ms > BorderTs.Ms)
    return false;

  if (CurrentTs.Us < BorderTs.Us)
    return true;

  return false;
}

TBool TimestampIsAfter(
  TTimestamp CurrentTs,
  TTimestamp BorderTs
)
{
  return !TimestampIsBefore(CurrentTs, BorderTs);
}

TUint_2 GetMicrosPart()
{
  return (TUint_4) (*Rtc.Current) * 1000 / 249;
}

TTimestamp GetTimestamp()
{
  return ToTimestamp(NumSecondsPassed, MilliSecondsPart, GetMicrosPart());
}

void WaitTill(
  TTimestamp Ts
)
{
  while (TimestampIsBefore(GetTimestamp(), Ts));
}

void WaitAfter(
  TTimestamp Ts
)
{
  while (TimestampIsAfter(GetTimestamp(), Ts));
  WaitTill(Ts);
}

void Delay_us(
  TUint_2 NumMicroseconds
)
{
  TTimestamp DeltaTs = MicrosToTimestamp(NumMicroseconds);

  TTimestamp EndTs = GetTimestamp();

  if (!AddTimestamp(&EndTs, DeltaTs))
    WaitAfter(EndTs);
  else
    WaitTill(EndTs);
}

void Delay_ms(
  TUint_2 NumMilliseconds
)
{
  TTimestamp DeltaTs = MillisToTimestamp(NumMilliseconds);

  TTimestamp EndTs = GetTimestamp();

  if (!AddTimestamp(&EndTs, DeltaTs))
    WaitAfter(EndTs);
  else
    WaitTill(EndTs);
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

  Console.Print("Init done.");
}

void loop()
{
  Delay_ms(1373);
  // Delay_us(653);
  // delay(1000);

  PrintTimestamp(GetTimestamp());
  // Console.Print("Tic");
}
