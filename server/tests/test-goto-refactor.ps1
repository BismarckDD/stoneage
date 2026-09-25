param([string]$Compiler = 'gcc')
$ErrorActionPreference = 'Stop'
$definitions = foreach ($entry in @(
    @{ Path = '../common/workspace.c'; Names = @('strncpysafe', 'strncpysafe2') },
    @{ Path = '../common/utils/util_string.c'; Names = @('findFieldDelimiter', 'getDelimitedField') },
    @{ Path = '../gmsv/char/skill.c'; Names = @('SKILL_readField', 'SKILL_makeSkillFromStringToArg') },
    @{ Path = '../gmsv/item/item.c'; Names = @('ITEM_readField', 'ITEM_makeExistItemsFromStringToArg') },
    @{ Path = '../gmsv/char/char_item.c'; Names = @('CHAR_finishMoneyDrop', 'CHAR_DropMoney') }
)) {
    $source = Get-Content -LiteralPath (Join-Path $PSScriptRoot $entry.Path) -Raw -Encoding UTF8
    foreach ($name in $entry.Names) {
        $match = [regex]::Match($source, '(?ms)^(?:static )?(?:const char \*|char \*|BOOL |void )' + $name + '\([^;{]*\)\s*\{.*?^\}')
        if (-not $match.Success) { throw "Cannot extract $name" }
        $match.Value
    }
}
$fixture = Get-Content -LiteralPath (Join-Path $PSScriptRoot 'goto_refactor_test.c') -Raw
$outputDir = Join-Path $PSScriptRoot '../../build/tests'
New-Item -ItemType Directory -Path $outputDir -Force | Out-Null
foreach ($variant in @('normal', 'simplified')) {
    $executable = Join-Path $outputDir "goto_refactor_$variant.exe"
    $flags = @('-std=c11', '-O2', '-Wall', '-Wextra', '-Werror')
    if ($variant -eq 'simplified') { $flags += '-D_SIMPLIFY_ITEMSTRING' }
    ($fixture + "`n" + ($definitions -join "`n")) |
        & $Compiler @flags -x c - -o $executable
    if ($LASTEXITCODE -ne 0) { throw "Compilation failed: $variant" }
    $output = Join-Path $outputDir "goto_refactor_$variant.log"
    $process = Start-Process -FilePath $executable -WindowStyle Hidden -PassThru -RedirectStandardOutput $output
    if (-not $process.WaitForExit(10000)) {
        $process.Kill()
        throw "Test timed out (possible record-advancement regression): $variant"
    }
    if ($process.ExitCode -ne 0) { throw "Regression failed: $variant" }
    Get-Content -LiteralPath $output
}
