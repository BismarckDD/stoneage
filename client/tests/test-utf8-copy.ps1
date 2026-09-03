$ErrorActionPreference = 'Stop'
$source = Get-Content -LiteralPath (Join-Path $PSScriptRoot '../stoneage/proto/autil.cpp') -Raw -Encoding UTF8
$definitions = foreach ($name in @('is_double_unit', 'getUtf8CharNum', 'getUtf8SequenceLength', 'getUtf8PrefixBytes', 'copyUtf8CharByNum')) {
    $match = [regex]::Match($source, '(?ms)^(?:static )?(?:int |size_t |const char \*)' + $name + '\(.*?^\}')
    if (-not $match.Success) { throw "Cannot extract $name" }
    $match.Value
}
$fixture = Get-Content -LiteralPath (Join-Path $PSScriptRoot 'utf8_copy_test.cpp') -Raw -Encoding UTF8
$outputDir = Join-Path $PSScriptRoot '../../build/tests'
New-Item -ItemType Directory -Path $outputDir -Force | Out-Null
$exe = Join-Path $outputDir 'utf8_copy_test.exe'
($fixture + "`n" + ($definitions -join "`n")) | & g++ -std=c++11 -Wall -Wextra -Werror -x c++ - -o $exe
if ($LASTEXITCODE -ne 0) { throw 'UTF-8 copy test compilation failed' }
& $exe
if ($LASTEXITCODE -ne 0) { throw 'UTF-8 copy regression failed' }
