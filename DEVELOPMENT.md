uICAL Development Environment   <!-- omit in toc -->
=============================

***How to setup an make code changes to the uICAL project***

- [Project Layout](#project-layout)
- [Development Environment](#development-environment)
- [Actions](##actions)
- [Debugging in vscode](#Debugging-in-vscode)


# Project Layout

* `devenv/` - a docker container environment which provides required development tooling
* `examples/` - sample projects which demonstrate uICAL
* `micropython-module/` - Bindings for Micro Python (_still a work in progress_)
* `python-module/` - Bindings for Python (_working but limited API_)
* `src/`  - C++ source for uICAL library
* `test/` - Tests in C++ and Python


# Development Environment

First build the container devenv with `./devenv/make-devenv.sh`, which acts as shell and environment where all the required tooling for development is installed. This means that you do not need to make any changes to your machine to participate to uICAL development.

## Actions

Run `./de make test-cpp`.

Run `./de make test-python`.

Run `./de make clean`.

Run `./de make coverage`.

Run `./de make memory`.


# Debugging in vscode

Launch configurations for debugging are included in `.vscode/launch.json`.

1. Run `./devenv/make-local-devenv.sh` to build a local virtual python environment with the uICAL module.
    - You can activate this environment manually by running `source ./virtualenv/bin/activate`, but it needs to be selected in vscode to use the launch configs.

2. Select `./virtualenv/bin/python` as the python interpreter in vscode.
    - Vscode should prompt you when it first detects the environment.
    - Alternatively, select it manually through the vscode command palette (`Shift+Ctrl+P`) and type `>Python: Select Interpreter`.

3. Ensure that all the tests pass:

    - **Run:**. `./de make clean test-all`

4. Launch through vscode
    - Open the **Run and Debug** tab on the left, select the configuration in the top, and click the green button.

