#!/bin/bash

set -euo pipefail

cd $(dirname "$(dirname "$0")")

# Ignore build/include rule because cpplint wants to include "src/xxx.h" files
# instead of "./xxx.h"
cpplint --filter -build/include src/*

eslint .
