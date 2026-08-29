[CmdletBinding()]
param([ValidateSet('x64','x86')][string]$Platform='x64')
$ErrorActionPreference='Stop'
$here=Split-Path -Parent $MyInvocation.MyCommand.Path
$vswhere=Join-Path ${env:ProgramFiles(x86)} 'Microsoft Visual Studio\Installer\vswhere.exe'
$install=& $vswhere -latest -products '*' -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath
if(-not $install){throw '未找到 Visual Studio C++ 桌面开发工具'}
$dev=Join-Path $install 'Common7\Tools\VsDevCmd.bat'
$out=Join-Path $here 'bin'
New-Item -ItemType Directory -Force $out | Out-Null
$arch=if($Platform -eq 'x64'){'amd64'}else{'x86'}
$source=Join-Path $here 'StoneAgeAssetViewer.cpp'
$exe=Join-Path $out 'StoneAgeAssetViewer.exe'
$cmd='"{0}" -arch={1} && cl /nologo /utf-8 /std:c++17 /EHsc /O2 "{2}" /Fe:"{3}" /link user32.lib gdi32.lib comctl32.lib shell32.lib ole32.lib advapi32.lib' -f $dev,$arch,$source,$exe
cmd /c $cmd
if($LASTEXITCODE -ne 0){throw "编译失败，退出码 $LASTEXITCODE"}
Write-Host "构建成功: $exe" -ForegroundColor Green
