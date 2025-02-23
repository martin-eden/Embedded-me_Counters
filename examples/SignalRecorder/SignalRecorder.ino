// Digital signal recorder

/*
  Author: Martin Eden
  Last mod.: 2025-02-23
*/

/*
  This is a signal recorder. It timestamps signal changes.
  (Or equivalently, it stores durations of signal levels.)

  What is important is storage capacity, time granularity and range.

  Storage capacity is 200 events.
  Time granularity is 250 kHz.
  Range is 50 000 granules.

  ( So event duration can be up to 0.2 s.)
*/

#include <me_Counters.h>

#include <me_BaseTypes.h>
#include <me_Uart.h>
#include <me_Console.h>
#include <me_Menu.h>

/*
  Signal duration in implementation-specific units of time

  We're using magic constant to represent that duration was over
  the limit we can store.
*/
typedef TUint_2 TDuration;

const TDuration MaxDuration = 50000;
const TDuration UnknownDuration = TUint_2_Max;

const TUint_2 MaxNumDurations = 200;
TDuration Durations[MaxNumDurations];

static TUint_2 NumDurations = 0;

void PrintDuration(
  TDuration * Duration
)
{
  if (*Duration == UnknownDuration)
    Console.Write("    ?");
  else
    Console.Print(*Duration);
}

void PrintDurations()
{
  for (TUint_2 Index = 0; Index < NumDurations; ++Index)
    PrintDuration(&Durations[Index]);

  Console.EndLine();
}

void ClearDurations()
{
  for (TUint_2 Index = 0; Index < NumDurations; ++Index)
    Durations[Index] = UnknownDuration;

  NumDurations = 0;
}

TBool AddDuration(
  TDuration * Duration
)
{
  if (NumDurations == MaxNumDurations)
    return false;

  ++NumDurations;

  Durations[NumDurations - 1] = *Duration;

  return true;
}

extern "C" void __vector_10() __attribute__((interrupt, used));

// Interrupt 10 is for counter 2 capture event
void __vector_10()
{
  me_Counters::TCounter2 CapturingCounter;

  TDuration Duration;

  if (CapturingCounter.Status->Done)
  {
    Duration = UnknownDuration;

    CapturingCounter.Status->Done = true; // Yep, cleared by writing one
  }
  else
  {
    TUint_2 EventMark = *CapturingCounter.EventMark;

    if (EventMark > MaxDuration)
      Duration = UnknownDuration;
    else
      Duration = EventMark;
  }

  AddDuration(&Duration);

  *CapturingCounter.Current = 0;

  CapturingCounter.Control->EventIsOnUpbeat =
    !CapturingCounter.Control->EventIsOnUpbeat;
}

void SetupCapturingCounter()
{
  using namespace me_Counters;

  TCounter2 CapturingCounter;

  CapturingCounter.SetAlgorithm(TAlgorithm_Counter2::Count_To2Pow16);
  CapturingCounter.Control->DriveSource =
    (TUint_1) TDriveSource_Counter2::Internal_SlowBy2Pow6;
  CapturingCounter.Control->EventIsOnUpbeat = false;
  CapturingCounter.Wiring->EnableEventInterrupt = true;
  *CapturingCounter.Current = 0;
}

void ClearDurations_Handler(
  TUint_2 Data [[gnu::unused]],
  TUint_2 Instance [[gnu::unused]]
)
{
  ClearDurations();
}

void PrintDurations_Handler(
  TUint_2 Data [[gnu::unused]],
  TUint_2 Instance [[gnu::unused]]
)
{
  PrintDurations();
}

void AddMenuItems(
  me_Menu::TMenu * Menu
)
{
  using
    me_Menu::Freetown::ToItem;

  TUint_2 UnusedAddr = 0;

  Menu->AddItem(ToItem("p", "Print captured durations", PrintDurations_Handler, UnusedAddr));
  Menu->AddItem(ToItem("c", "Clear data", ClearDurations_Handler, UnusedAddr));
}

void setup()
{

  me_Uart::Init(me_Uart::Speed_1M_Bps);

  SetupCapturingCounter();

  ClearDurations();

  Console.Print("Init done");

  {
    me_Menu::TMenu Menu;

    AddMenuItems(&Menu);

    Menu.AddBuiltinCommands();
    Menu.Print();

    Menu.Run();
  }

  Console.Print("Done");
}

void loop()
{
}

/*
  2025-01-09
  2025-01-28
  2025-02-23
*/
