param([string]$EngineRoot='D:\Epic Games\UE_5.8')
$ErrorActionPreference='Stop'
$build=Join-Path $EngineRoot 'Engine\Build\BatchFiles\Build.bat'
if(-not(Test-Path -LiteralPath $build)){throw 'Unreal 5.8 build tools missing; supply -EngineRoot.'}
# Materialize an independent copy from the retained preview. Never overwrite a hand-edited copy.
$content=Join-Path $PSScriptRoot 'Content'
if(-not(Test-Path -LiteralPath $content)) {
    Copy-Item -LiteralPath (Join-Path $PSScriptRoot '..\UE5VisualLab\Content') -Destination $content -Recurse
}
$project=Join-Path $PSScriptRoot 'CheweenieUE5Lab.uproject'
& $build CheweenieUE5LabEditor Win64 Development "-Project=$project" -WaitMutex -MaxParallelActions=2
if($LASTEXITCODE -ne 0){throw "Native compilation failed ($LASTEXITCODE). Check the compiler requirement in README.md."}
if(-not(Test-Path -LiteralPath (Join-Path $PSScriptRoot 'Binaries\Win64\UnrealEditor-CheweenieUE5Lab.dll'))){throw 'Native module DLL missing after build.'}
