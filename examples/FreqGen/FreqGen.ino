// 38 kHz generator

/*
  Author: Martin Eden
  Last mod.: 2025-02-21
*/

/*
  This snippet generates 38000 Hz frequency on pin 11

  38 kHz is used for infrared emitter. Counter 3 is used for this job.

  You can adapt it to other frequencies but keep in mind that half-wave
  duration value in ticks must be fit in byte. So for lower frequencies
  you need to slowdown counter.
*/

#include <me_BaseTypes.h>
#include <me_Uart.h>
#include <me_Console.h>
#include <me_Counters.h>

void ReferenceFreqGen()
{
  const TUint_1 OutputPin = 11;
  const TUint_4 OutputFreq = 38000;

  tone(OutputPin, OutputFreq);
}

void FreqGen()
{
  /*
    tone() is using interrupt handler to toggle any pin

    We're not using interrupts here. ATmega 328/P has three hardware
    counters. Each counter has two hardware pins that it can
    set/clear/toggle on event. (Some secondary pins can't be toggled
    in some modes tho.)

    Timer Primary Secondary
      1       6        5
      2       9       10
      3      11        3
  */

  TUint_4 DesiredFreq_Hz = 38000;
  TUint_1 HalfWaveDuration_Ut = F_CPU / (DesiredFreq_Hz * 2);

  const TUint_1 OutputPin = 11;

  using namespace me_Counters;

  TCounter3 Counter;

  Counter.SetAlgorithm(TAlgorithm_Counter3::Count_ToMarkA);
  Counter.Control->Speed = (TUint_1) TSpeed_Counter3::Full;
  Counter.Control->PinActionOnMarkA = (TUint_1) TPinAction::Toggle;
  *Counter.MarkA = HalfWaveDuration_Ut - 1;
  *Counter.Current = 0;

  pinMode(OutputPin, OUTPUT);

  Counter.Control->ImitateMarkA = true;

  Console.Write("We're emitting frequency");
  Console.Print(DesiredFreq_Hz);
  Console.Write("Hz at pin");
  Console.Print(OutputPin);
  Console.Write(".");
  Console.EndLine();
}

void setup()
{
  me_Uart::Init(me_Uart::Speed_115k_Bps);

  Console.Print("Hello there.");
  FreqGen();
  // ReferenceFreqGen();
  Console.Print("That's all folks!");
}

void loop()
{
}

/*
  2025-02-21
*/
