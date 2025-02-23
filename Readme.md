# What

(2024-12/2025-02 ...)

Interface to ATmega328/P counters (aka timers).

All code I've seen for working with counters are unreadable with
names like TCCR1B. This implementation is uses none of them.

Counters are just hardware circuits running along with main code
and using hardcoded memory locations for input/output.
We're providing interface to them with more readable names.

We want to thank C++ for bitfields.

## Code

* [Interface][Interface]
* [Implementation][Implementation]
* [Examples][Examples]

## Requirements

  * arduino-cli
  * bash

## Install/remove

This is low-level library which depends only of "me_BaseTypes".
However examples depend on my other libraries. To save us time
I would advise to clone [GetLibs][GetLibs] repo and run it's code
to get all my stuff.

## See also

* [My other embedded C++ libraries][Embedded]
* [My other repositories][Repos]

[Interface]: src/me_Counters.h
[Implementation]: src/me_Counters.cpp
[Examples]: examples/

[GetLibs]: https://github.com/martin-eden/Embedded-Framework-GetLibs

[Embedded]: https://github.com/martin-eden/Embedded_Crafts/tree/master/Parts
[Repos]: https://github.com/martin-eden/contents
