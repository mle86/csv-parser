#!/bin/bash
. $(dirname "$0")/init.sh

# This script tests if csv-parser will compile.

assertCmd -v "make -C $HERE/../"

success
