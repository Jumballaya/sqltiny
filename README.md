# SQLTiny

Small SQL storage, this project is for learning purposes only

## Build

run `make` to build

## Tests

The end-2-end testing is done with Ruby's Rspec. The Unit tests are done with C.

You can find the tests in `/tests`

## Install

run `sudo make install` to install

## Resources

[Architecture of SQLite](https://www.sqlite.org/arch.html)

## Ideas
The idea is to build a working (but simple) relational database that can have SQL queries ran against it. Another goal of mine is to learn how to build and grow a C codebase.

#### Organization --
- **Development** code goes into the `src` folder. The `src` folder is for the actual source code and the libs/headers are described below.
- **Test** code goes into the `tests` folder. There is a `tests/includes` folder that has some testing tools for the unit tests. The `tests/unit` houses the unit test `.c` files starting with the word `Test`. These test files all compile into their own binary and are ran when you run `make test`. The `tests/e2e` folder has Ruby rspec end-to-end tests. These run the binaries that are built to the `bin` folder and make assertions based on the behavior of those binaries.
- **Library** files like project headers aswell as any 3rd party headers that might need to be added go into the `include` folder.
- **Build** code goes into the `build` folder. This is where any temporary build files are housed and is deleted when you run `make clean`.
- **Binary** output files go into the `bin` folder. This is where any final binaries build to.
- **Documentation** goes in the `docs` folder. This includes any build notes, dev notes, etc. This is also where the API and usage documentation will live.

#### What to test --
With Ruby rspec, the behavior of the built binary files will be tested by running the binary with given input and asserting against the output.

For unit tests, I will write simple C tests and helper macros. The goal is for these tests to be as light as possible (the idea is the compiler + rspec should catch most bugs) and to really only test 2 things:

  - Utility functions (math functions, abstractions, formatters, etc.)
  - Exposed C API (final high-level API to interact with SQLTiny)
