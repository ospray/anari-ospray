## Copyright 2020 Intel Corporation
## SPDX-License-Identifier: Apache-2.0

Param(
  [string] $G = 'Ninja'
)

md build
cd build

cmake --version

$exitCode = 0

cmake -DCMAKE_INSTALL_PREFIX=install -L `
-G $G `
$args `
../superbuild
if ($LastExitCode) { $exitCode++ }

cmake --build . --config Release
if ($LastExitCode) { $exitCode++ }

exit $LASTEXITCODE
