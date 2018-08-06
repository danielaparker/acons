#!/bin/bash
set -e

DIR=$(cd $(dirname ${BASH_SOURCE[0]}) && pwd)

mkdir ${DIR}/acons_tests
cd ${DIR}/../..
${DIR}/acons_tests
