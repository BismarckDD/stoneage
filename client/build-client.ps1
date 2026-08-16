[CmdletBinding()]
param(
    [ValidateSet('VER25_Release', 'VER25_Debug')]
    [string]$Configuration = 'VER25_Release',

    [ValidateSet('Win32', 'x64')]
    [string]$Platform = 'Win32',

    [string]$PlatformToolset = 'v145',

    [switch]$Rebuild
)

$ErrorActionPreference = 'Stop'
$scriptDirectory = Split-Path -Parent $MyInvocation.MyCommand.Path
$solutionPath = Join-Path $scriptDirectory 'stoneage.sln'

if (-not (Test-Path -LiteralPath $solutionPath)) {
    throw "Solution not found: $solutionPath"
}

$msbuildPath = $null
$vswherePath = Join-Path ${env:ProgramFiles(x86)} 'Microsoft Visual Studio\Installer\vswhere.exe'

if (Test-Path -LiteralPath $vswherePath) {
    $installationPath = & $vswherePath `
        -latest `
        -products '*' `
        -requires Microsoft.Component.MSBuild `
        -property installationPath

    if ($installationPath) {
        $candidate = Join-Path $installationPath 'MSBuild\Current\Bin\MSBuild.exe'
        if (Test-Path -LiteralPath $candidate) {
            $msbuildPath = $candidate
        }
    }
}

if (-not $msbuildPath) {
    $candidatePaths = @(
        'C:\Program Files\Microsoft Visual Studio\18\Community\MSBuild\Current\Bin\MSBuild.exe',
        'C:\Program Files\Microsoft Visual Studio\2022\Community\MSBuild\Current\Bin\MSBuild.exe',
        'C:\Program Files\Microsoft Visual Studio\2022\Professional\MSBuild\Current\Bin\MSBuild.exe',
        'C:\Program Files\Microsoft Visual Studio\2022\Enterprise\MSBuild\Current\Bin\MSBuild.exe'
    )

    $msbuildPath = $candidatePaths |
        Where-Object { Test-Path -LiteralPath $_ } |
        Select-Object -First 1
}

if (-not $msbuildPath) {
    throw 'MSBuild was not found. Install Visual Studio with Desktop development with C++.'
}

$target = if ($Rebuild) { 'Rebuild' } else { 'Build' }
$arguments = @(
    $solutionPath,
    '/m',
    "/t:$target",
    "/p:Configuration=$Configuration",
    "/p:Platform=$Platform",
    "/p:PlatformToolset=$PlatformToolset"
)

Write-Host "MSBuild: $msbuildPath"
Write-Host "Configuration: $Configuration|$Platform ($target)"

& $msbuildPath @arguments
if ($LASTEXITCODE -ne 0) {
    throw "Client build failed. MSBuild exit code: $LASTEXITCODE"
}

$configurationDirectory = $Configuration.ToUpperInvariant()
$outputPath = Join-Path (Split-Path -Parent $scriptDirectory) "build\$configurationDirectory\stoneage.exe"

if (Test-Path -LiteralPath $outputPath) {
    Write-Host "Build succeeded: $outputPath" -ForegroundColor Green
} else {
    Write-Host 'Build succeeded, but stoneage.exe was not found in the default output directory.' -ForegroundColor Yellow
}
