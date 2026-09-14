[CmdletBinding()]
param(
    [ValidateSet('Debug', 'Release', 'RelWithDebInfo')]
    [string]$Configuration = 'Debug',

    [string]$BuildDirectory,

    [switch]$Reconfigure
)

$ErrorActionPreference = 'Stop'
$serverRoot = Split-Path -Parent $PSScriptRoot
if ([string]::IsNullOrWhiteSpace($BuildDirectory)) {
    $BuildDirectory = Join-Path $serverRoot 'build\windows'
}

function Find-Tool {
    param(
        [Parameter(Mandatory)] [string]$Name,
        [Parameter(Mandatory)] [string[]]$Candidates
    )

    $command = Get-Command $Name -ErrorAction SilentlyContinue
    if ($null -ne $command) {
        return $command.Source
    }
    foreach ($candidate in $Candidates) {
        if (Test-Path -LiteralPath $candidate -PathType Leaf) {
            return $candidate
        }
    }
    throw "Cannot find $Name. Install the MSYS2 UCRT64 toolchain; see server/README-Windows.md."
}

$msysRoot = if ($env:MSYS2_ROOT) { $env:MSYS2_ROOT } else { 'C:\msys64' }
$ucrtBin = Join-Path $msysRoot 'ucrt64\bin'
$cmake = Find-Tool -Name 'cmake.exe' -Candidates @(
    (Join-Path $ucrtBin 'cmake.exe')
)
$ninja = Find-Tool -Name 'ninja.exe' -Candidates @(
    (Join-Path $ucrtBin 'ninja.exe')
)
$gcc = Find-Tool -Name 'gcc.exe' -Candidates @(
    (Join-Path $ucrtBin 'gcc.exe')
)

$env:PATH = "$ucrtBin;$env:PATH"
$cachePath = Join-Path $BuildDirectory 'CMakeCache.txt'
$binDirectory = Join-Path $BuildDirectory 'bin'
$cachedConfiguration = $null
$cachedBuildTesting = $null
if (Test-Path -LiteralPath $cachePath -PathType Leaf) {
    $cacheEntry = Select-String -LiteralPath $cachePath `
        -Pattern '^CMAKE_BUILD_TYPE:STRING=(.*)$' | Select-Object -First 1
    if ($cacheEntry) {
        $cachedConfiguration = $cacheEntry.Matches[0].Groups[1].Value
    }
    $testingCacheEntry = Select-String -LiteralPath $cachePath `
        -Pattern '^BUILD_TESTING:BOOL=(.*)$' | Select-Object -First 1
    if ($testingCacheEntry) {
        $cachedBuildTesting = $testingCacheEntry.Matches[0].Groups[1].Value
    }
}

if ($Reconfigure -or !(Test-Path -LiteralPath $cachePath -PathType Leaf) -or
    $cachedConfiguration -ne $Configuration -or $cachedBuildTesting -ne 'OFF') {
    Write-Host "Configuring the Windows server build: $BuildDirectory"
    & $cmake -S $serverRoot -B $BuildDirectory -G Ninja `
        "-DCMAKE_MAKE_PROGRAM=$ninja" `
        "-DCMAKE_C_COMPILER=$gcc" `
        "-DCMAKE_BUILD_TYPE=$Configuration" `
        "-DCMAKE_RUNTIME_OUTPUT_DIRECTORY=$binDirectory" `
        "-DBUILD_TESTING=OFF"
    if ($LASTEXITCODE -ne 0) {
        throw "CMake configuration failed with exit code $LASTEXITCODE."
    }
} else {
    Write-Host "Using existing CMake configuration: $BuildDirectory ($Configuration)"
}

& $cmake --build $BuildDirectory --parallel --target saac gmsv
if ($LASTEXITCODE -ne 0) {
    throw "Server build failed with exit code $LASTEXITCODE."
}

# Remove stale executable artifacts left by older builds. Runtime DLLs are kept.
if (Test-Path -LiteralPath $binDirectory -PathType Container) {
    Get-ChildItem -LiteralPath $binDirectory -Filter '*.exe' -File |
        Where-Object { $_.Name -notin @('saac.exe', 'gmsv.exe') } |
        Remove-Item -Force
}

Write-Host "Build complete: $binDirectory"
Get-ChildItem -LiteralPath $binDirectory -Filter '*.exe' | Select-Object FullName
