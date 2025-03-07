// Digital signal recorder/replayer

/*
  Author: Martin Eden
  Last mod.: 2025-03-07
*/

/*
  This is a signal recorder. It timestamps signal changes.
  (Or equivalently, it stores durations of signal levels.)
  Parameters are tuned for recording signals from IR receiver.

  Signal durations are printed in microseconds.

  We're using feature unique to counter 2. It can copy current
  counter's value to separate variable when signal on event pin
  changes. Event pin is 8.

  For signal recorded important things are storage capacity,
  time granularity and duration span.

  Storage capacity is 150 events.
  Time granularity is 2 MHz (0.5 us).
  Duration span ~ 11 days.
*/

/*
  Wiring

    8 Input
    11 Output
*/

#include <me_Counters.h>
#include <me_Timestamp.h>
#include <me_RunTime.h>

#include <me_BaseTypes.h>
#include <me_Uart.h>
#include <me_Console.h>
#include <me_Menu.h>

const TUint_1
  InputPin = 8,
  OutputPin = 3;

typedef me_Timestamp::TTimestamp TDuration;

const TDuration UnknownDuration = { 0, 0, 0, 0 };

const TUint_2 MaxNumDurations = 150;
TDuration Durations[MaxNumDurations];

TDuration LastEvent = UnknownDuration;

TUint_2 NumDurations = 0;

void PrintDuration(
  TDuration Duration
)
{
  Console.Write("Duration");
  Console.Write("(");

  TBool IsStarted;

  IsStarted = false;

  if (Duration.KiloS != 0)
    IsStarted = true;

  if (IsStarted)
    Console.Print(Duration.KiloS);

  if (Duration.S != 0)
    IsStarted = true;

  if (IsStarted)
    Console.Print(Duration.S);

  if (Duration.MilliS != 0)
    IsStarted = true;

  if (IsStarted)
    Console.Print(Duration.MilliS);

  if (Duration.MicroS != 0)
    IsStarted = true;

  if (IsStarted)
    Console.Print(Duration.MicroS);

  Console.Write(")");
  Console.EndLine();
}

void PrintDurations()
{
  Console.Print("(");

  for (TUint_2 Index = 0; Index < NumDurations; ++Index)
    PrintDuration(Durations[Index]);

  Console.Print(")");

  Console.EndLine();
}

void ClearDurations()
{
  for (TUint_2 Index = 0; Index < NumDurations; ++Index)
    Durations[Index] = UnknownDuration;

  NumDurations = 0;

  LastEvent = UnknownDuration;
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

extern "C" void __vector_10() __attribute__((interrupt, used)); // (1)

/*
  [1]: "interrupt" attribute means that interrupts are _enabled_ at
    start of handler. That's actually desired and important:
    we're using GetTime() which gets timestamp that updated by
    another interrupt routine.

    When using "signal" attribute that don't enables interrupts
    we got conditions that our "vector 10" is executed before
    "vector 11" used for updating timestamp.

    In that case time update is pending, we're using old time
    (but with right amount of microseconds) and we're getting
    negative difference from last time.
*/

// Interrupt 10 is for counter 2 capture event
void __vector_10()
{
  // Set counter to capture opposite side of signal edge
  {
    me_Counters::TCounter2 CapturingCounter;

    CapturingCounter.Control->EventIsOnUpbeat =
      !CapturingCounter.Control->EventIsOnUpbeat;
  }

  // Add duration since last event to array
  {
    using
      me_RunTime::GetTime,
      me_Timestamp::Subtract,
      me_Timestamp::Add;

    TDuration CurTime, Duration;

    CurTime = GetTime();

    Duration = CurTime;

    // (1)
    if (!Subtract(&Duration, LastEvent))
    {
      const TDuration MilliS = { 0, 0, 1, 0 };
      Add(&Duration, MilliS);
    }

    AddDuration(&Duration);

    LastEvent = CurTime;
  }

  /*
    [1]: Still horrible workaround for negative time difference case

      GetTime() lives on timer 2. Our event lives on timer 2.
      "Event" interrupt from timer 2 has higher priority than
      "Mark" interrupt of timer 2. Microseconds part from GetTime()
      is always correct.

      Imagine the time period is year. We're updating year number
      after detecting "New year" event. Also we have "received parcel"
      event with higher priority.

      So it's like January 3rd, we coming to get parcel. Last parcel was
      December 30th year 2024. We asking for GetTime() to get full date
      for time delta calculation. And returned date is 2024-01-03.
      Because year number was not updated yet.

      That's what happening for microseconds and milliseconds.

      Workaround is bad because it must know how time is updated and
      it does same job for it's own variable. But we see no other
      practical solution.
  */
}

void StartRecording()
{
  me_Counters::TCounter2 CaptiveCounter;

  CaptiveCounter.Control->EventIsOnUpbeat = false;
  CaptiveCounter.Interrupts->OnEvent = true;
}

void StopRecording()
{
  me_Counters::TCounter2 CaptiveCounter;

  CaptiveCounter.Interrupts->OnEvent = false;
  CaptiveCounter.Status->GotEventMark = true; // yep, cleared by one
}

void SetupRecorder()
{
  StartRecording();
  StopRecording();
}

void SetupFreqGen()
{
  pinMode(OutputPin, OUTPUT);

  TUint_4 DesiredFreq_Hz = 38000;
  TUint_1 WaveDuration_Ut = (TUint_4) F_CPU / DesiredFreq_Hz / 8;

  using namespace me_Counters;

  TCounter3 Counter;

  StopFreqGen();

  Counter.SetAlgorithm(TAlgorithm_Counter3::FastPwm_ToMarkA);

  *Counter.MarkA = WaveDuration_Ut - 1;
  Counter.Control->PinActionOnMarkA = (TUint_1) TPinAction::None;

  *Counter.MarkB = *Counter.MarkA / 2;
  Counter.Control->PinActionOnMarkB = (TUint_1) TPinAction::Set;
}

void StartFreqGen()
{
  using namespace me_Counters;

  TCounter3 Counter;

  *Counter.Current = 0;
  Counter.Control->Speed = (TUint_1) TSpeed_Counter3::SlowBy2Pow3;
}

void StopFreqGen()
{
  const TUint_1 CounterStoppingMargin = 10;

  using namespace me_Counters;

  TCounter3 Counter;

  if (Counter.Control->Speed != (TUint_1) TSpeed_Counter3::None)
    while (*Counter.Current > CounterStoppingMargin);

  Counter.Control->Speed = (TUint_1) TSpeed_Counter3::None;
  *Counter.Current = 0;
}

void ReplayDurations()
{
  Console.Print("(");

  // First duration contains time from system start. So we'll ignore it
  TUint_2 Index = 1;

  while (true)
  {
    {
      if (Index >= NumDurations)
        break;

      TDuration Duration = Durations[Index];

      {
        const TDuration EmitOverhead = { 0, 0, 0, 47 };
        const TDuration Zero = { 0, 0, 0, 0 };

        if (me_Timestamp::Compare(Duration, EmitOverhead) <= 0)
          Duration = Zero;
        else
          me_Timestamp::Subtract(&Duration, EmitOverhead);
      }

      // PrintDuration(Duration);

      StartFreqGen();

      me_RunTime::Delay(Duration);

      StopFreqGen();

      ++Index;
    }
    {
      if (Index >= NumDurations)
        break;

      TDuration Duration = Durations[Index];

      // PrintDuration(Duration);

      {
        const TDuration DelayOverhead = { 0, 0, 0, 57 };
        const TDuration Zero = { 0, 0, 0, 0 };

        if (me_Timestamp::Compare(Duration, DelayOverhead) <= 0)
          Duration = Zero;
        else
          me_Timestamp::Subtract(&Duration, DelayOverhead);
      }

      me_RunTime::Delay(Duration);

      ++Index;
    }
  }

  Console.Print(")");
}

void TestEmitter()
{
  TDuration Duration;
  ClearDurations();

  Duration = { 0, 0, 2, 0 };
  AddDuration(&Duration);

  Duration = { 0, 0, 1, 0 };
  AddDuration(&Duration);

  Duration = { 0, 0, 0, 500 };
  AddDuration(&Duration);

  Duration = { 0, 0, 1, 0 };
  AddDuration(&Duration);

  ReplayDurations();
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

void StartRecording_Handler(
  TUint_2 Data [[gnu::unused]],
  TUint_2 Instance [[gnu::unused]]
)
{
  StartRecording();
}

void StopRecording_Handler(
  TUint_2 Data [[gnu::unused]],
  TUint_2 Instance [[gnu::unused]]
)
{
  StopRecording();
}

void Replay_Handler(
  TUint_2 Data [[gnu::unused]],
  TUint_2 Instance [[gnu::unused]]
)
{
  ReplayDurations();
}

void TestEmitter_Handler(
  TUint_2 Data [[gnu::unused]],
  TUint_2 Instance [[gnu::unused]]
)
{
  TestEmitter();
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

  Menu->AddItem(ToItem("b", "Begin recording", StartRecording_Handler, UnusedAddr));
  Menu->AddItem(ToItem("e", "End recording", StopRecording_Handler, UnusedAddr));

  Menu->AddItem(ToItem("r", "Replay captured signal", Replay_Handler, UnusedAddr));
  Menu->AddItem(ToItem("t", "Test emitter", TestEmitter_Handler, UnusedAddr));
}

void setup()
{
  me_Uart::Init(me_Uart::Speed_115k_Bps);

  me_RunTime::Setup();

  SetupRecorder();

  SetupFreqGen();

  ClearDurations();

  Console.Print("Init done.");

  {
    me_Menu::TMenu Menu;

    AddMenuItems(&Menu);

    Menu.AddBuiltinCommands();
    Menu.Print();

    Menu.Run();
  }

  Console.Print("Done.");
}

void loop()
{
}

/*
  2025-01 # #
  2025-02 #
  2025-03 # # #
  2025-03-07
*/
