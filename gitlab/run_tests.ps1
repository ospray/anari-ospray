## Copyright 2020-2021 Intel Corporation
## SPDX-License-Identifier: Apache-2.0

echo "Running tests"

$env:Path += ";.\build\Release"
$env:ANARI_LIBRARY = "ospray"
.\build\Release\anariRenderTests.exe

exit $LASTEXITCODE
