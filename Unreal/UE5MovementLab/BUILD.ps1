param([string]$EngineRoot='D:\Epic Games\UE_5.8')
$ErrorActionPreference='Stop'
$build=Join-Path $EngineRoot 'Engine\Build\BatchFiles\Build.bat'
if(-not(Test-Path -LiteralPath $build)){throw 'Unreal 5.8 build tools missing; supply -EngineRoot.'}
$vswhere=Join-Path ${env:ProgramFiles(x86)} 'Microsoft Visual Studio\Installer\vswhere.exe'
if(-not(Test-Path -LiteralPath $vswhere)){throw 'Visual Studio Installer is missing. Install current Visual Studio C++ Build Tools (v14.44 or newer).'}
$installations=& $vswhere -all -products '*' -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -format json | ConvertFrom-Json
$supported=@($installations | ForEach-Object {
    $tools=Join-Path $_.installationPath 'VC\Tools\MSVC'
    if(Test-Path -LiteralPath $tools){Get-ChildItem -LiteralPath $tools -Directory | Where-Object { [version]$_.Name -ge [version]'14.44' }}
})
if($supported.Count -eq 0){throw 'Install current Visual Studio 2022 C++ Build Tools v14.44 or newer. v14.42 is banned; v14.38 passes detection but fails compiling UE5.8 headers. See README.md.'}
# Materialize an independent copy from the retained preview. Never overwrite a hand-edited copy.
$content=Join-Path $PSScriptRoot 'Content'
if(-not(Test-Path -LiteralPath $content)) {
    Copy-Item -LiteralPath (Join-Path $PSScriptRoot '..\UE5VisualLab\Content') -Destination $content -Recurse
}
$project=Join-Path $PSScriptRoot 'CheweenieUE5Lab.uproject'
& $build CheweenieUE5LabEditor Win64 Development "-Project=$project" -WaitMutex -MaxParallelActions=1 -NoXGE -NoUBA
if($LASTEXITCODE -ne 0){throw "Native compilation failed ($LASTEXITCODE). Inspect UnrealBuildTool output for the failing source line."}
if(-not(Test-Path -LiteralPath (Join-Path $PSScriptRoot 'Binaries\Win64\UnrealEditor-CheweenieUE5Lab.dll'))){throw 'Native module DLL missing after build.'}
