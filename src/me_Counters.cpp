// Hardware counters functions

/*
  Author: Martin Eden
  Last mod.: 2026-02-12
*/

#include <me_Counters.h>

#include <me_BaseTypes.h>

using namespace me_Counters;

// ( All counters control

void TCountersControl::Start()
{
  Control->StopCounters = false;
}

void TCountersControl::Stop()
{
  Control->StopCounters = true;
}

// )

/*
  2025 # # # # # #
  2026-02-12
*/
