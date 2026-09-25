param([string]$Compiler = 'gcc')
$ErrorActionPreference = 'Stop'
$definitions = foreach ($entry in @(
    @{ Path = '../common/workspace.c'; Names = @('strncpysafe', 'strncpysafe2') },
    @{ Path = '../common/utils/util_string.c'; Names = @('findFieldDelimiter', 'getDelimitedField', 'nextDelimitedField') }
)) {
    $source = Get-Content -LiteralPath (Join-Path $PSScriptRoot $entry.Path) -Raw -Encoding UTF8
    foreach ($name in $entry.Names) {
        $match = [regex]::Match($source, '(?ms)^(?:static )?(?:const char \*|char \*|BOOL )' + $name + '\([^;{]*\) \{.*?^\}')
        if (-not $match.Success) { throw "Cannot extract $name" }
        $match.Value
    }
}
$fixture = Get-Content -LiteralPath (Join-Path $PSScriptRoot 'delimited_field_test.c') -Raw
$outputDir = Join-Path $PSScriptRoot '../../build/tests'
New-Item -ItemType Directory -Path $outputDir -Force | Out-Null
$executable = Join-Path $outputDir 'delimited_field_test.exe'
# Compile the production parser and copy helpers, including strict warnings.
($fixture + "`n" + ($definitions -join "`n")) |
    & $Compiler -std=c11 -O2 -Wall -Wextra -Werror -x c - -o $executable
if ($LASTEXITCODE -ne 0) { throw 'Delimited field test compilation failed' }
& $executable
if ($LASTEXITCODE -ne 0) { throw 'Delimited field tests failed' }
