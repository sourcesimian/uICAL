uICAL Development Environment   <!-- omit in toc -->
=============================

***How to setup an make code changes to the uICAL project***

- [Project Layout](#project-layout)
- [Development Environment](#development-environment)
- [Actions](#actions)


# Project Layout

* `devenv/` - a docker container environment which provides required development tooling
* `examples/` - sample projects which demonstrate uICAL
* `micropython-module/` - Bindings for Micro Python (_still a work in progress_)
* `python-module/` - Bindings for Python (_working but limited API_)
* `src/`  - C++ source for uICAL library
* `test/` - Tests in C++ and Python


# Development Environment

First build the container devenv with `./devenv/make-devenv.sh`, which acts as shell and environment where all the required tooling for development is installed. This means that you do not need to make any changes to your machine to participate to uICAL development.

# Actions

Run `./de make test-cpp`.

Run `./de make test-python`.

Run `./de make clean`.

Run `./de make coverage`.

Run `./de make memory`.
