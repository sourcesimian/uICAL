#!/bin/bash

python3 -m venv virtualenv
source virtualenv/bin/activate

pip3 install -r devenv/docker/python3-requirements.txt

pip3 install --editable .

