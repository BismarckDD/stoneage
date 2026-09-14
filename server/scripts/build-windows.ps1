[CmdletBinding()]
param(
    # -b / -B is accepted as a shorthand for the build configuration so that
    # "-b debug" selects the Debug configuration. Without the alias PowerShell
    # would bind the "-b" prefix to -BuildDirectory and build into a
    # directory literally named "debug".
    [Alias('b')]
    [ValidateSet('Debug', 'Release', 'RelWithDebInfo')]
    [string]$Configuration = 'Debug',

    [string]$BuildDirectory,

    [switch]$Reconfigure
)

$ErrorActionPreference = 'Stop'
$serverRoot = Split-Path -Parent $PSScriptRoot
# ValidateSet only checks the value, it does not normalise the casing. Keep the
# canonical spelling so -b debug and -Configuration debug behave identically.
$Configuration = @('Debug', 'Release', 'RelWithDebInfo') |
    Where-Object { $_ -ieq $Configuration } |
    Select-Object -First 1
# Debug (the default configuration) builds into server\build\windows and emits
# gmsv.exe / saac.exe into server\build\windows\bin so the executable sits next
# to the runtime data directory. Pass -BuildDirectory to build elsewhere; the
# binaries then land in <BuildDirectory>\bin.
if ([string]::IsNullOrWhiteSpace($BuildDirectory)) {
    $BuildDirectory = Join-Path $serverRoot 'build\windows'
}
if (![System.IO.Path]::IsPathRooted($BuildDirectory)) {
    # Resolve relative to the caller's location so every path handed to CMake
    # is absolute, and so the cache comparison below is unambiguous.
    $BuildDirectory = Join-Path (Get-Location -PSProvider FileSystem).Path $BuildDirectory
}
$BuildDirectory = [System.IO.Path]::GetFullPath($BuildDirectory)
# Guard against the classic mix-up of a configuration name for a directory
# name, which would silently create a stray build tree next to the sources.
if ((Split-Path -Leaf $BuildDirectory) -in @('Debug', 'Release', 'RelWithDebInfo')) {
    throw ("-BuildDirectory was given '$BuildDirectory', which looks like a " +
        "configuration name. Use -Configuration (or -b) for " +
        "Debug/Release/RelWithDebInfo, and -BuildDirectory only for the " +
        "directory that holds the CMake build tree.")
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
# All configurations, including Debug, emit their executables into
# <BuildDirectory>\bin -- that is server\build\windows\bin for the default
# build directory. The path is passed to CMake explicitly below.
$binDirectory = Join-Path $BuildDirectory 'bin'
$cachedConfiguration = $null
$cachedBuildTesting = $null
$cachedOutputDirectory = $null
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
    $outputCacheEntry = Select-String -LiteralPath $cachePath `
        -Pattern '^CMAKE_RUNTIME_OUTPUT_DIRECTORY:[^=]*=(.*)$' | Select-Object -First 1
    if ($outputCacheEntry) {
        $cachedOutputDirectory = $outputCacheEntry.Matches[0].Groups[1].Value.Trim()
    }
}

# A stale cache that points the runtime output somewhere else would silently
# drop the Debug executables outside server\build\windows\bin, so treat a
# mismatching output directory the same way as a mismatching configuration.
$outputDirectoryMatches = $false
if (![string]::IsNullOrWhiteSpace($cachedOutputDirectory)) {
    $resolvedOutputDirectory = $cachedOutputDirectory
    if (![System.IO.Path]::IsPathRooted($resolvedOutputDirectory)) {
        # CMake resolves a relative runtime output directory against the
        # binary directory of the target, i.e. the build directory.
        $resolvedOutputDirectory = Join-Path $BuildDirectory $resolvedOutputDirectory
    }
    try {
        $outputDirectoryMatches =
            [System.IO.Path]::GetFullPath($resolvedOutputDirectory).TrimEnd([char[]]@('\', '/')) `
                -ieq $binDirectory.TrimEnd([char[]]@('\', '/'))
    } catch {
        $outputDirectoryMatches = $false
    }
}

if ($Reconfigure -or !(Test-Path -LiteralPath $cachePath -PathType Leaf) -or
    $cachedConfiguration -ne $Configuration -or $cachedBuildTesting -ne 'OFF' -or
    !$outputDirectoryMatches) {
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
    Write-Host ("Using existing CMake configuration: $BuildDirectory " +
        "($Configuration, output $binDirectory)")
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

$expectedArtifacts = @('saac.exe', 'gmsv.exe')
$missingArtifacts = $expectedArtifacts | Where-Object {
    !(Test-Path -LiteralPath (Join-Path $binDirectory $_) -PathType Leaf)
}
if ($missingArtifacts) {
    throw ("The $Configuration build did not produce " +
        "$($missingArtifacts -join ', ') in $binDirectory.")
}

Write-Host "Build complete: $binDirectory ($Configuration)"
Get-ChildItem -LiteralPath $binDirectory -Filter '*.exe' | Select-Object FullName
