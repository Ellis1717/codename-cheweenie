param([Parameter(Mandatory)][string]$PackageRoot,[Parameter(Mandatory)][string]$TestRoot)
$ErrorActionPreference='Stop'
$PackageRoot=[IO.Path]::GetFullPath($PackageRoot);$TestRoot=[IO.Path]::GetFullPath($TestRoot)
# Reuse the complete two-peer/solo-companion gate; this also installs portable LAN launchers.
& (Join-Path $PSScriptRoot 'FINALIZE_COOP.ps1') -PackageRoot $PackageRoot -TestRoot $TestRoot
$gate=Get-Content -LiteralPath (Join-Path $PackageRoot 'validated-coop.json') -Raw|ConvertFrom-Json
$payload=@(Get-ChildItem -LiteralPath (Join-Path $PackageRoot 'Windows') -Recurse -File|Where-Object {$_.FullName -notmatch '[\\/]Saved[\\/]'})
$newest=($payload|Sort-Object LastWriteTimeUtc -Descending|Select-Object -First 1).LastWriteTimeUtc
$checks=$gate.checks
foreach($entry in @(@('World/world-smoke.json',14),@('Adventure/adventure-smoke.json',25),@('Adventure/adventure-resume.json',6),@('Life/life-smoke.json',23),@('Life/life-resume.json',5))){
    $file=Join-Path $TestRoot $entry[0]
    if(!(Test-Path -LiteralPath $file) -or (Get-Item -LiteralPath $file).LastWriteTimeUtc -lt $newest){throw ('Missing/stale world regression: '+$entry[0])}
    $result=Get-Content -LiteralPath $file -Raw|ConvertFrom-Json
    if($result.failed.Count -or $result.passed.Count -lt $entry[1]){throw ('Failed/incomplete world regression: '+$entry[0])};$checks+=$result.passed.Count
}
Add-Type -AssemblyName System.Drawing
foreach($name in @('WorldRoom.png','WorldDogView.png','WorldHome.png','WorldPark.png')){
    $file=Join-Path $TestRoot ('World/'+$name)
    if(!(Test-Path -LiteralPath $file) -or (Get-Item -LiteralPath $file).LastWriteTimeUtc -lt $newest){throw 'World capture missing/stale'}
    $bitmap=[Drawing.Bitmap]::new($file)
    try{if($bitmap.Width -ne 1920 -or $bitmap.Height -ne 1080){throw 'Expected full-HD actual world captures'};$sum=0.0;$samples=0
        for($y=320;$y -lt 900;$y+=40){for($x=100;$x -lt 1560;$x+=40){$c=$bitmap.GetPixel($x,$y);$sum+=.2126*$c.R+.7152*$c.G+.0722*$c.B;$samples++}}
        if(($sum/$samples) -lt 20){throw ('World capture is too dark: '+$name)}
    }finally{$bitmap.Dispose()}
}
$bad=@(Get-ChildItem -LiteralPath $TestRoot -Filter '*.log' -Recurse|Where-Object {$_.Name -notmatch 'backup'}|Select-String -Pattern 'Log\w+: (Error|Fatal):|Failed to compile Material|Default Material will be used in game')
if($bad.Count){throw 'Final world runtime logs contain Error/Fatal or shader fallbacks'}
[ordered]@{version='World-0.10.0';checks=$checks;rendered=$true;physicalControllersVerified=$false;separateMachinesVerified=$false;files=$gate.files}|ConvertTo-Json -Depth 7|Set-Content -LiteralPath (Join-Path $PackageRoot 'validated-world.json')
Copy-Item -LiteralPath (Join-Path $PSScriptRoot 'PLAY_WORLD.ps1') -Destination (Join-Path $PackageRoot 'PLAY_WORLD.ps1')
'@echo off','powershell -NoProfile -ExecutionPolicy Bypass -File "%~dp0PLAY_WORLD.ps1"','if errorlevel 1 pause'|Set-Content -LiteralPath (Join-Path $PackageRoot 'PLAY_WORLD.cmd')
'World package enabled: '+$checks+' packaged assertions, four full-HD readability-checked captures, retained solo/LAN paths.'
