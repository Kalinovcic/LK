# LK

LK single-file public domain libraries for C & C++

library           | description
------------------|--------------
**lk_platform.h** | Platform layer with hot code reloading
**lk_nocrt.c**    | Minimal boilerplate required to compile without the CRT on MSVC

tool              | description
------------------|--------------
**lk_build.cpp**  | Easy-to-use single-file incremental build system for C & C++. Not thoroughly tested, I wouldn't recommend using it yet.

### Licence
This software is in the public domain. Anyone can use it, modify it,
roll'n'smoke hardcopies of the source code, sell it to the terrorists, etc.
You have no legal obligation to do anything else, although I would appreciate attribution.

For more license information, see UNLICENCE.

### Thanks
- to Casey Muratori for implementing live code editing on Handmade Hero and inspiring me to create lk_platform.h
- to Sean Barrett for his brilliant library design philosophy
- to Microsoft for being incompetent and forcing me to create lk_build.cpp
