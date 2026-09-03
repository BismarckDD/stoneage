$ErrorActionPreference = 'Stop'
$source = Get-Content -LiteralPath (Join-Path $PSScriptRoot '../gmsv/char/chatmagic.c') -Raw -Encoding UTF8
$strings = Get-Content -LiteralPath (Join-Path $PSScriptRoot '../common/utils/util_string.c') -Raw
$definitions = foreach ($name in @('CHAR_parseMetamoImage', 'CHAR_CHAT_DEBUG_metamo', 'easyGetTokenFromString')) {
    $text = if ($name -eq 'easyGetTokenFromString') { $strings } else { $source }
    $match = [regex]::Match($text, '(?ms)^(?:static )?(?:int|void) ' + $name + '\(.*?^\}')
    if (-not $match.Success) { throw "Cannot extract $name" }
    $match.Value
}
$fixture = Get-Content -LiteralPath (Join-Path $PSScriptRoot 'metamo_test.c') -Raw
$outputDir = Join-Path $PSScriptRoot '../../build/tests'
New-Item -ItemType Directory -Path $outputDir -Force | Out-Null
$exe = Join-Path $outputDir 'metamo_test.exe'
($fixture + "`n" + ($definitions -join "`n")) | & gcc -std=c11 -Wall -Wextra -Werror -x c - -o $exe
if ($LASTEXITCODE -ne 0) { throw 'Metamo test compilation failed' }
& $exe
if ($LASTEXITCODE -ne 0) { throw 'Metamo regression failed' }
