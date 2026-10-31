param([Parameter(Mandatory)][string]$PackageRoot,[Parameter(Mandatory)][string]$TestRoot)
$ErrorActionPreference='Stop'
$PackageRoot=[IO.Path]::GetFullPath($PackageRoot);$TestRoot=[IO.Path]::GetFullPath($TestRoot)
$windows=Join-Path $PackageRoot 'Windows'
if(!(Test-Path -LiteralPath (Join-Path $windows 'CheweenieUE5Lab.exe'))){throw 'Packaged executable missing'}
$payload=@(Get-ChildItem -LiteralPath $windows -File -Recurse|Where-Object {$_.FullName -notmatch '[\\/]Saved[\\/]|[\\/]LocalGameData[\\/]' -and $_.Extension -ne '.log'})
$newest=($payload|Sort-Object LastWriteTimeUtc -Descending|Select-Object -First 1).LastWriteTimeUtc
$checks=0
foreach($entry in @(@('World/world-smoke.json',14),@('Navigation/navigation-smoke.json',59),@('Navigation/navigation-resume.json',4),@('Life/life-smoke.json',23),@('Life/life-resume.json',5),@('Festival/festival-smoke.json',40),@('Festival/festival-resume.json',6),@('Costumes/costumes-smoke.json',27),@('Costumes/costumes-resume.json',2),@('Coop/coop-host.json',31),@('Coop/coop-guest.json',28),@('Companions/companions-smoke.json',30),@('Companions/companions-resume.json',4),@('Adventure/adventure-smoke.json',25),@('Adventure/adventure-resume.json',6),@('Ratchet/ratchet-unlock.json',6),@('Ratchet/ratchet-resume.json',2),@('Ratchet/ratchet-memory.json',12),@('Ratchet/ratchet-return.json',3),@('UnlockedCoop/coop-host.json',30),@('UnlockedCoop/coop-guest.json',27))){
    $file=Join-Path $TestRoot $entry[0]
    if(!(Test-Path -LiteralPath $file) -or (Get-Item -LiteralPath $file).LastWriteTimeUtc -lt $newest){throw ('Missing/stale acceptance report: '+$entry[0])}
    $result=Get-Content -LiteralPath $file -Raw|ConvertFrom-Json
    if($result.failed.Count -or $result.passed.Count -lt $entry[1]){throw ('Failed/incomplete acceptance: '+$entry[0])};$checks+=$result.passed.Count
}
Add-Type -AssemblyName System.Drawing
foreach($name in @('Festival/FestivalTitle.png','Festival/FestivalPorch.png','Festival/FestivalFinale.png','Costumes/HalloweenMartySide.png','Costumes/HalloweenMartyTop.png','Costumes/HalloweenMartyLying.png','Costumes/HalloweenMartyRolling.png')){
    $file=Join-Path $TestRoot $name
    if(!(Test-Path -LiteralPath $file) -or (Get-Item -LiteralPath $file).LastWriteTimeUtc -lt $newest){throw ('Missing/stale capture: '+$name)}
    $bitmap=[Drawing.Bitmap]::new($file)
    try{if($bitmap.Width -ne 1280 -or $bitmap.Height -ne 720){throw ('Expected 1280x720 capture: '+$name)}}finally{$bitmap.Dispose()}
}
$bad=@(Get-ChildItem -LiteralPath $TestRoot -Filter '*.log' -Recurse|Select-String -Pattern 'Log\w+: (Error|Fatal):|Failed to compile Material|Default Material will be used')
if($bad.Count){throw 'Final runtime logs contain errors or material fallbacks'}
$files=@($payload|ForEach-Object {@{path=[IO.Path]::GetRelativePath($windows,$_.FullName).Replace('\','/');sha256=(Get-FileHash -LiteralPath $_.FullName -Algorithm SHA256).Hash}})
@{version='Festival-0.11.1';checks=$checks;rendered=$true;physicalControllersVerified=$false;separateMachinesVerified=$false;files=$files}|ConvertTo-Json -Depth 6|Set-Content -LiteralPath (Join-Path $PackageRoot 'validated-festival.json')
Copy-Item -LiteralPath (Join-Path $PSScriptRoot 'PLAY_FESTIVAL.ps1') -Destination (Join-Path $PackageRoot 'PLAY_FESTIVAL.ps1')
'@echo off','powershell -NoProfile -ExecutionPolicy Bypass -File "%~dp0PLAY_FESTIVAL.ps1" %*','if errorlevel 1 pause'|Set-Content -LiteralPath (Join-Path $PackageRoot 'PLAY_FESTIVAL.cmd')
'Halloween enabled: '+$checks+' packaged checks, '+$files.Count+' payload hashes and fresh seasonal captures. Visual inspection and physical controllers are separate checks.'
