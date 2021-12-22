## Copyright 2020-2021 Intel Corporation
## SPDX-License-Identifier: Apache-2.0

md build
cd build

cmake --version

cmake -L `
-G $args[0] `
-T $args[1] `
../superbuild

cmake --build . --config Release --target ALL_BUILD

exit $LASTEXITCODE
