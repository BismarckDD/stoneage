[CmdletBinding()]
param(
    [ValidateSet('saac', 'gmsv', 'all')]
    [string]$Server = 'all',

    [Parameter(Mandatory)]
    [string]$DataDirectory,

    [string]$BuildDirectory
)

$ErrorActionPreference = 'Stop'
$serverRoot = Split-Path -Parent $PSScriptRoot
if ([string]::IsNullOrWhiteSpace($BuildDirectory)) {
    $BuildDirectory = Join-Path $serverRoot 'build\windows'
}
$binDirectory = Join-Path $BuildDirectory 'bin'
$dataRoot = (Resolve-Path -LiteralPath $DataDirectory).Path
$msysRoot = if ($env:MSYS2_ROOT) { $env:MSYS2_ROOT } else { 'C:\msys64' }
$ucrtBin = Join-Path $msysRoot 'ucrt64\bin'

$saacPath = Join-Path $binDirectory 'saac.exe'
$gmsvPath = Join-Path $binDirectory 'gmsv.exe'
if (($Server -eq 'saac' -or $Server -eq 'all') -and
    !(Test-Path -LiteralPath $saacPath -PathType Leaf)) {
    throw "Cannot find $saacPath. Run build-windows.ps1 first."
}
if (($Server -eq 'gmsv' -or $Server -eq 'all') -and
    !(Test-Path -LiteralPath $gmsvPath -PathType Leaf)) {
    throw "Cannot find $gmsvPath. Run build-windows.ps1 first."
}

if (($Server -eq 'saac' -or $Server -eq 'all') -and
    !(Test-Path -LiteralPath (Join-Path $dataRoot 'acserv.cf') -PathType Leaf)) {
    throw "The data directory does not contain acserv.cf: $dataRoot"
}
if (($Server -eq 'gmsv' -or $Server -eq 'all') -and
    !(Test-Path -LiteralPath (Join-Path $dataRoot 'setup.cf') -PathType Leaf)) {
    throw "The data directory does not contain setup.cf: $dataRoot"
}

$env:PATH = "$ucrtBin;$env:PATH"
Push-Location $dataRoot
try {
    if ($Server -eq 'saac') {
        & $saacPath
        exit $LASTEXITCODE
    }
    if ($Server -eq 'gmsv') {
        & $gmsvPath -f setup.cf
        exit $LASTEXITCODE
    }

    $saacProcess = Start-Process -FilePath $saacPath -WorkingDirectory $dataRoot `
        -PassThru -WindowStyle Hidden
    try {
        Start-Sleep -Seconds 2
        if ($saacProcess.HasExited) {
            throw "SAAC failed to start; exit code $($saacProcess.ExitCode)."
        }
        & $gmsvPath -f setup.cf
        exit $LASTEXITCODE
    }
    finally {
        if (!$saacProcess.HasExited) {
            $saacProcess.CloseMainWindow() | Out-Null
            if (!$saacProcess.WaitForExit(5000)) {
                Stop-Process -Id $saacProcess.Id
            }
        }
    }
}
finally {
    Pop-Location
}
