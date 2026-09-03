$ErrorActionPreference = 'Stop'
$source = Get-Content -LiteralPath (Join-Path $PSScriptRoot '../stoneage/oft/oft.cpp') -Raw -Encoding UTF8
$parser = [regex]::Match($source, '(?ms)^static char get_command\(void\)\s*\{.*?^\}')
$gate = [regex]::Match($source, '(?ms)^        if \(command_no != ATT_MALFUNCTION\)[^\r\n]*\r?\n        \{\r?\n            if \(ATR_VCT_NO\(a1\) != 0\).*?^        \}')
if (-not $parser.Success -or -not $gate.Success) { throw 'Cannot extract battle command handling from production source' }
$fixture = Get-Content -LiteralPath (Join-Path $PSScriptRoot 'battle_command_test.cpp') -Raw
$outputDir = Join-Path $PSScriptRoot '../../build/tests'
New-Item -ItemType Directory -Path $outputDir -Force | Out-Null
$executable = Join-Path $outputDir 'battle_command_test.exe'
# Exercise the actual parser and actor-readiness gate without the graphics runtime.
$code = $fixture + "`n" + $parser.Value + "`nvoid dispatch() { do {`n" + $gate.Value + "`n++executed; } while (0); }"
$code | & g++ -std=c++11 -Wall -Wextra -x c++ - -o $executable
if ($LASTEXITCODE -ne 0) { throw 'Battle command test compilation failed' }
& $executable
if ($LASTEXITCODE -ne 0) { throw 'Battle command tests failed' }
