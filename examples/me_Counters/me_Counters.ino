// [me_Counters] test/demo

/*
  That's a test sketch where I'm experimenting.
*/

/*
  Author: Martin Eden
  Last mod.: 2025-02-23
*/

#include <me_Counters.h>

#include <me_BaseTypes.h>
#include <me_Uart.h>
#include <me_Console.h>

// extern "C" void __vector_10() __attribute__((interrupt, used));

struct TLineState
{
  TBool IsLow;
  TBool IsSensibleMark;
  TUint_2 StartMark;
};

TBool GetLineChange(
  TLineState * Signal
)
{
  me_Counters::TCounter2 CapturingCounter;

  TBool IsUnknowSignalLength = false;

  if (CapturingCounter.Status->GotEventMark)
  {
    Signal->IsLow = !CapturingCounter.Control->EventIsOnUpbeat;

    if (CapturingCounter.Status->Done)
    {
      IsUnknowSignalLength = true;
      CapturingCounter.Status->Done = true; // Yep, cleared by writing one
    }

    if (IsUnknowSignalLength)
    {
      Signal->IsSensibleMark = false;
      Signal->StartMark = 0;
    }
    else
    {
      Signal->IsSensibleMark = true;
      Signal->StartMark = *CapturingCounter.EventMark;
    }

    (*CapturingCounter.Current) = 0;

    CapturingCounter.Control->EventIsOnUpbeat =
      !CapturingCounter.Control->EventIsOnUpbeat;

    CapturingCounter.Status->GotEventMark = true; // Also cleared by writing one

    return true;
  }

  return false;
}

TUint_2 GetUsFromTicks(
  TUint_2 NumTicks
)
{
  const TUint_2 Prescale = 64;
  return (TUint_4) (1000000 / (F_CPU / Prescale)) * NumTicks;
}

void setup()
{
  me_Uart::Init(me_Uart::Speed_1M_Bps);

  {
    using
      me_Counters::TCounter2,
      me_Counters::TAlgorithm_Counter2,
      me_Counters::TDriveSource_Counter2;

    TCounter2 CapturingCounter;

    CapturingCounter.SetAlgorithm(TAlgorithm_Counter2::Count_To2Pow16);
    CapturingCounter.Control->DriveSource = (TUint_1) TDriveSource_Counter2::Internal_SlowBy2Pow6;
    (*CapturingCounter.Current) = 0;
    CapturingCounter.Control->EventIsOnUpbeat = false;
  }

  Console.Print("Init done");
}

void loop()
{
  TLineState LineState;
  TBool IsTic;

  if (GetLineChange(&LineState))
  {
    IsTic = LineState.IsLow;

    if (IsTic)
      Console.Write("Tic");
    else
      Console.Write("Tac");

    if (LineState.IsSensibleMark)
      Console.Print(LineState.StartMark);
    else
      Console.Write("     ? ");

    if (IsTic)
      Console.Write(" ");
    else
      Console.EndLine();
  }
}

/*
  2024-12-29
  2024-12-30
  2025-01-01
  2025-01-05
*/
