$ErrorActionPreference = 'Stop'
$sourcePath = Join-Path $PSScriptRoot '../common/gmsv_server_recv.c'
$source = Get-Content -LiteralPath $sourcePath -Raw -Encoding UTF8
$definitions = foreach ($name in @('isValidUtf8CharacterName', 'checkStringErr')) {
    $pattern = '(?ms)^(?:static )?BOOL ' + $name + '\([^\r\n]*\) \{.*?^\}'
    $match = [regex]::Match($source, $pattern)
    if (-not $match.Success) { throw "Cannot extract $name from production source" }
    $match.Value
}
$fixture = Get-Content -LiteralPath (Join-Path $PSScriptRoot 'check_string_err_test.c') -Raw
$outputDir = Join-Path $PSScriptRoot '../../build/tests'
New-Item -ItemType Directory -Path $outputDir -Force | Out-Null
$executable = Join-Path $outputDir 'check_string_err_test.exe'
# Compile the production function bodies, not a second implementation.
($fixture + "`n" + ($definitions -join "`n")) |
    & gcc -std=c11 -Wall -Wextra -Werror -x c - -o $executable
if ($LASTEXITCODE -ne 0) { throw 'Validator test compilation failed' }
& $executable
if ($LASTEXITCODE -ne 0) { throw 'Validator tests failed' }
