#!/usr/bin/env bash

set -e

# work from either APM directory or above
[ -d ArduPlane ] || cd APM

export DOCS_OUTPUT_BASE=./docs

(
$DOCS_OUTPUT_BASE/build-libs.sh
$DOCS_OUTPUT_BASE/build-arduplane.sh
) > $DOCS_OUTPUT_BASE/build_docs.log 2>&1
