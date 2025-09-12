// Digital signal recorder/replayer

/*
  Author: Martin Eden
  Last mod.: 2025-09-12
*/

/*
  This is a signal recorder. It timestamps signal changes.
  (Or equivalently, it stores durations of signal levels.)
  Parameters are tuned for recording signals from IR receiver.

  Signal durations are printed in microseconds.

  We're using feature unique to counter 2. It can copy current
  counter's value to separate variable when signal on event pin
  changes. Event pin is 8.

  Storage capacity is 120 events.
  Time granularity is 0.5 us (2 MHz).
  Duration span ~ 11 days.
*/

/*
  Wiring

    8 Input
    3 Output
*/

#include <me_Counters.h>

#include <me_BaseTypes.h>

#include <me_Timestamp.h>
#include <me_RunTime.h>
#include <me_Delays.h>

#include <me_Menu.h>
#include <me_Console.h>
#include <me_DebugPrints.h>

const TUint_1
  InputPin = 8,
  OutputPin = 3;

typedef me_Timestamp::TTimestamp TDuration;

const TDuration UnknownDuration = { 0, 0, 0, 0 };

const TUint_2 MaxNumDurations = 120;
TDuration Durations[MaxNumDurations];

TDuration LastEvent = UnknownDuration;

TUint_2 NumDurations = 0;

void PrintDurations()
{
  Console.Print("(");

  for (TUint_2 Index = 0; Index < NumDurations; ++Index)
    me_DebugPrints::PrintDuration(Durations[Index]);

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
  TDuration Duration
)
{
  if (NumDurations == MaxNumDurations)
    return false;

  ++NumDurations;

  Durations[NumDurations - 1] = Duration;

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

// Interrupt 10 is for counter 2 capture event, pin 8
void __vector_10()
{
  // Trigger next capture at opposite side of signal edge
  {
    me_Counters::TCounter2 CapturingCounter;

    CapturingCounter.Control->EventIsOnUpbeat =
      !CapturingCounter.Control->EventIsOnUpbeat;
  }

  // Add current time to array, we'll process it at printing
  AddDuration(me_RunTime::GetTime());
}

void StartRecording()
{
  me_Counters::TCounter2 CaptiveCounter;

  me_RunTime::Start();

  CaptiveCounter.Control->EventIsOnUpbeat = false;
  CaptiveCounter.Interrupts->OnEvent = true;
}

void StopRecording()
{
  me_Counters::TCounter2 CaptiveCounter;

  CaptiveCounter.Interrupts->OnEvent = false;
  CaptiveCounter.Status->GotEventMark = true; // yep, cleared by one

  me_RunTime::Stop();
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

  if (WaveDuration_Ut < 2)
    return;

  Counter.SetAlgorithm(TAlgorithm_Counter3::FastPwm_ToMarkA);

  *Counter.MarkA = WaveDuration_Ut - 1;
  Counter.Control->PinActionOnMarkA = (TUint_1) TPinAction::None;

  *Counter.MarkB = (WaveDuration_Ut - 1) / 2;
  Counter.Control->PinActionOnMarkB = (TUint_1) TPinAction::Set;
}

void StartFreqGen()
{
  using namespace me_Counters;

  TCounter3 Counter;

  Counter.Control->Speed = (TUint_1) TSpeed_Counter3::SlowBy2Pow3;
  *Counter.Current = 0;
}

void StopFreqGen()
{
  const TUint_1 CounterStoppingMargin = 10;

  using namespace me_Counters;

  TCounter3 Counter;

  // If we're not stopped finish wave cycle
  if (Counter.Control->Speed != (TUint_1) TSpeed_Counter3::None)
    while (*Counter.Current > CounterStoppingMargin);

  Counter.Control->Speed = (TUint_1) TSpeed_Counter3::None;
  *Counter.Current = 0;
}

void ReplayDurations()
{
  const TDuration Zero = { 0, 0, 0, 0 };
  const TDuration EmitOverhead = { 0, 0, 0, 176 };
  const TDuration DelayOverhead = { 0, 0, 0, 232 };
  TUint_2 Index;
  TDuration Duration;

  // First duration contains time from system start. So we'll ignore it
  Index = 1;

  while (true)
  {
    if (Index >= NumDurations)
      break;

    Duration = Durations[Index];

    // Emit wave
    {
      if (!me_Timestamp::Subtract(&Duration, EmitOverhead))
        Duration = Zero;

      StartFreqGen();
      me_Delays::Delay_Duration(Duration);
      StopFreqGen();

      ++Index;
    }

    if (Index >= NumDurations)
      break;

    Duration = Durations[Index];

    // Sleep
    {
      if (!me_Timestamp::Subtract(&Duration, DelayOverhead))
        Duration = Zero;

      me_Delays::Delay_Duration(Duration);

      ++Index;
    }
  }
}

void TestEmitter()
{
  ClearDurations();

  AddDuration({ 0, 0, 2, 0 });
  AddDuration({ 0, 0, 1, 0 });
  AddDuration({ 0, 0, 1, 0 });
  AddDuration({ 0, 0, 2, 0 });

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

  TUint_2 Unused = 0;

  Menu->AddItem(ToItem("b", "Begin recording", StartRecording_Handler, Unused));
  Menu->AddItem(ToItem("e", "End recording", StopRecording_Handler, Unused));

  Menu->AddItem(ToItem("p", "Print captured data", PrintDurations_Handler, Unused));
  Menu->AddItem(ToItem("c", "Clear data", ClearDurations_Handler, Unused));

  Menu->AddItem(ToItem("r", "Replay captured data", Replay_Handler, Unused));

  Menu->AddItem(ToItem("t", "Test emitter (overwrite recording and replay)", TestEmitter_Handler, Unused));
}

void setup()
{
  Console.Init();

  SetupRecorder();

  SetupFreqGen();

  ClearDurations();

  Console.Print("IR signal player/recorder.");

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
  // Add duration since last event to array
  {
    TDuration CurTime, Duration;

    CurTime = me_RunTime::GetTime();

    Duration = CurTime;

    // (1)
    if (!me_Timestamp::Subtract(&Duration, LastEvent))
    {
      const TDuration MilliS = { 0, 0, 1, 0 };
      me_Timestamp::Add(&Duration, MilliS);
    }

    AddDuration(&Duration);

    LastEvent = CurTime;
  }
  */

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

/*
  2025-01 # #
  2025-02 #
  2025-03 # # # # #
  2025-09-12
*/
