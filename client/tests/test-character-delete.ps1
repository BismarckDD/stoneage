$ErrorActionPreference = 'Stop'
$source = Get-Content -LiteralPath (Join-Path $PSScriptRoot '../stoneage/system/netproc.cpp') -Raw -Encoding UTF8
$handler = [regex]::Match($source, '(?ms)^void lssproto_CD_recv\(.*?^\}')
if (-not $handler.Success) { throw 'Cannot extract CD handler' }
$fixture = Get-Content -LiteralPath (Join-Path $PSScriptRoot 'character_delete_test.cpp') -Raw
$outputDir = Join-Path $PSScriptRoot '../../build/tests'
New-Item -ItemType Directory -Path $outputDir -Force | Out-Null
$exe = Join-Path $outputDir 'character_delete_test.exe'
($fixture + "`n" + $handler.Value) | & g++ -std=c++11 -Wall -Wextra -Wno-unused-parameter -x c++ - -o $exe
if ($LASTEXITCODE -ne 0) { throw 'CD handler test compilation failed' }
& $exe
if ($LASTEXITCODE -ne 0) { throw 'CD handler regression failed' }
