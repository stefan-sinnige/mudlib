# MUD Library

## Preface

<img src="./doc/mud.png" width="200px"/>

> Omne tulit punctum qui miscuit utile dulci,
> lectorem delectando pariterque monendo.
> He who blends usefulness and pleasantness wins every
> vote, at once delighting and instructing the reader.
> Horace - Ars Poetica (343-344)

When Horace postulates in his "Ars Poetica" (19 B.C.) that one of the
qualifications of a poet is to mix the usefulness with sweetness in order to
both delight and instruct the reader, little would he have known that this
would not only apply to the written art of poetry, but to the art of
programming as well. Taken from verse 343 of this poem is the Latin phrase
_miscere utile dulci_ that captures this principle. Abbreviated to MUD,
is the project name that is being uses as the basis for naming various
projects by the same Author.

## Overview

The MUD library is a private project that aims at self-development of software
engineering. It is a general purpose application framework library that
conceptualises various aspects and technologies of application development. A
number of such frameworks already exists and have been quite successful. So why
is there a need for yet another one? Well, there probably is not really a need
for it, but I will refer to its premise. The intention is not only to
be useful and delightful for anyone who may find it suitable for their own
purposes, but equally so, it will be useful and delightful for the author
who created it. This includes the ever evolving C++ language constructs as
well as the need to abstract and combine many principles of application
development into a consistent single framework.

Let's take, as an example, the technology behind programming a TCP server and
client application. Back in 1983, the Berkeley Sockets API (application
programming interface) was introduced to define the functions to support
creating of sockets, establishing and accepting connections and sending and
receiving data from one endpoint to another. This API was adopted into the
POSIX (Portable Operating System Interface) standard and is supported on
almost any platform. For example, it is incorporated into UNIX systems and
their derivations, like Oracle Solaris, Linux and Apple Darwin, and also in
Microsoft Windows. Even to this day, applications written in the 80s would
still be able to build and run on modern operating systems.

Throughout the decades, advances has been made in all aspects of computer
science. The adoption of object-oriented programming is one of those advances
and led to the introduction of C++ and other programming languages. This has
created some a dilemma that the procedural API for handling sockets of POSIX
has to be mapped within an object-oriented framework. Some library providers
have adopted these concepts successfully and these have been widely adopted,
even to the point that they have been incorporated into \Cpp standards (like
some aspects of the Boost library).

However, with all these different libraries, there is no single abstraction of
such concepts which is a 'correct' one. An ACE (Adaptive Communication
Environment) socket differs in a Boost socket by their viewpoint on how a
socket can be represented. Although there is inevitably a great number of
overlap of functionality that each has to provide, the differences between them
and the design concepts they implemented, needs to be assessed to their
usefulness in the specific problem domain and which can be applied more
effectively.

Another aspect of application framework libraries is that there are many
libraries that focus on a certain key feature, but there are only a few that
cater an entire spectrum. There are a predominantly a few popular libraries
for handling SSL, XML or SOAP communication, and most of them are integrated
in a certain way in an application framework. However, that makes a consistent
implementation of such a framework harder. Of course, one would gain an awful
lot from the experience of such mature libraries, but they may not be a
prefect fit for the concept that the application framework would like
to convey. Such libraries are often integrated with an adapter pattern to make
them fit within the confinements of the application framework, even though some
concessions may need to be made in the process.

In this aspect, MUD library serves the purpose foremost, to present yet another
view of the current technologies and how they can be applied in application
development. It is advocating a certain viewpoint on how different
technologies can operate together, with the same underlying concepts and
methodologies, while providing a consistent application framework on all
levels.  Therefore, it is aiming to implement many of the technologies on a
_per need_ basis. It is not trying to be complete in every aspect it
supports, but only implements those aspects that are vital for a certain
application. This library is therefore ever evolving!

## Installation

As the MUD library is intended to design and write everything from scratch,
the list of dependencies are minimal and include

* autoconf
* autoconf-archive
* automake
* libtool
* flex
* bison
* standard C/C++ library

The full build, test and installation steps would usually be
```
autoreconf -fiv
./configure
make
make check
make install
```
Depending on the compiler suite being used, the configuration step may need
be adjusted by supplying custom overrides. For example `CXXFLAGS=-std=c++20` may
need to be supplied if the compiler does not use C++-20 extensions by default.

## Documentation

The documentation is still an ongoing concern. All the source code is written
to be self-explanatory, but does rely on the developer to have some background
information on the subject matter. Wherever possible, the `doc` folder contains
relevant information and references to the original sources of truth that have
been used to develop the source code. This is certainly more relevant for anyone
who wishes to learn from the source code or wishes to enhance on it.

The code is written with doxygen style comments, but has not been generated any
documentation from this (yet). A set of samples can be examined for its intended
uses as well as the test cases. Both provide real-life and compilable code
excerptions to test and show-case its correctness and application.

## License

Unless explictely stated otherwise, this project and all its source code is
provided under the MIT license. See the LICENSE file for more information.
