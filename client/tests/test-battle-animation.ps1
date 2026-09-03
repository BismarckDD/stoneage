$ErrorActionPreference = 'Stop'
$source = Get-Content -LiteralPath (Join-Path $PSScriptRoot '../stoneage/system/pattern.cpp') -Raw -Encoding UTF8
$definitions = foreach ($name in @('finishMissingAnimation', 'pattern')) {
    $match = [regex]::Match($source, '(?ms)^(?:static )?int ' + $name + '\([^\r\n]*\) \{.*?^\}')
    if (-not $match.Success) { throw "Cannot extract $name from production source" }
    $match.Value
}
$fixture = Get-Content -LiteralPath (Join-Path $PSScriptRoot 'battle_animation_test.cpp') -Raw
$outputDir = Join-Path $PSScriptRoot '../../build/tests'
New-Item -ItemType Directory -Path $outputDir -Force | Out-Null
$executable = Join-Path $outputDir 'battle_animation_test.exe'
($fixture + "`n" + ($definitions -join "`n")) |
    & g++ -std=c++11 -Wall -Wextra -Werror -x c++ - -o $executable
if ($LASTEXITCODE -ne 0) { throw 'Battle animation test compilation failed' }
& $executable
if ($LASTEXITCODE -ne 0) { throw 'Battle animation tests failed' }
