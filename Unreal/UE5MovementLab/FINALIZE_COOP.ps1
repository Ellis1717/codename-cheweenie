param([Parameter(Mandatory)][string]$PackageRoot,[Parameter(Mandatory)][string]$TestRoot)
$ErrorActionPreference='Stop'
$PackageRoot=[IO.Path]::GetFullPath($PackageRoot);$TestRoot=[IO.Path]::GetFullPath($TestRoot)
$windows=Join-Path $PackageRoot 'Windows'
if(!(Test-Path -LiteralPath (Join-Path $windows 'CheweenieUE5Lab.exe'))){throw 'Missing packaged game'}
$payload=@(Get-ChildItem -LiteralPath $windows -Recurse -File|Where-Object {$_.FullName -notmatch '[\\/]Saved[\\/]'})
$newest=($payload|Sort-Object LastWriteTimeUtc -Descending|Select-Object -First 1).LastWriteTimeUtc
$checks=0
foreach($phase in @('Headless','Rendered')){foreach($role in @('host','guest')){
    $report=Join-Path $TestRoot ($phase+'/coop-'+$role+'.json')
    if(!(Test-Path -LiteralPath $report) -or (Get-Item -LiteralPath $report).LastWriteTimeUtc -lt $newest){throw ('Missing/stale packaged co-op report: '+$phase+'/'+$role)}
    $r=Get-Content -LiteralPath $report -Raw|ConvertFrom-Json
    $minimum=if($role -eq 'host'){24}else{21}
    if($r.failed.Count -or $r.passed.Count -lt $minimum){throw 'Failed/incomplete co-op report'};$checks+=$r.passed.Count
}}
foreach($relative in @('Solo/companions-smoke.json','Solo/companions-resume.json')){
    $report=Join-Path $TestRoot $relative
    if(!(Test-Path -LiteralPath $report) -or (Get-Item -LiteralPath $report).LastWriteTimeUtc -lt $newest){throw 'Missing/stale solo regression'}
    $r=Get-Content -LiteralPath $report -Raw|ConvertFrom-Json
    if($r.failed.Count -or $r.passed.Count -lt $(if($relative -like '*resume*'){4}else{30})){throw 'Solo regression failed'};$checks+=$r.passed.Count
}
foreach($name in @('CoopHost.png','CoopGuest.png')){
    $capture=Join-Path $TestRoot ('Rendered/'+$name)
    if(!(Test-Path -LiteralPath $capture) -or (Get-Item -LiteralPath $capture).LastWriteTimeUtc -lt $newest){throw 'Missing/stale actual gameplay capture'}
}
$bad=@(Get-ChildItem -LiteralPath $TestRoot -Filter '*.log' -Recurse|Where-Object {$_.Name -notmatch 'backup'}|Select-String -Pattern 'Log\w+: (Error|Fatal):|Failed to compile Material|Default Material will be used in game')
if($bad.Count){throw 'Error/fatal or material fallback in final packaged test logs'}
$files=@($payload|ForEach-Object {[ordered]@{path=[IO.Path]::GetRelativePath($windows,$_.FullName).Replace('\','/');sha256=(Get-FileHash -LiteralPath $_.FullName -Algorithm SHA256).Hash.ToLowerInvariant()}})
[ordered]@{version='LAN-0.1.0';checks=$checks;loopbackProcesses=2;rendered=$true;physicalControllersVerified=$false;separateMachinesVerified=$false;files=$files}|ConvertTo-Json -Depth 6|Set-Content -LiteralPath (Join-Path $PackageRoot 'validated-coop.json')
Copy-Item -LiteralPath (Join-Path $PSScriptRoot 'PLAY_COOP.ps1') -Destination (Join-Path $PackageRoot 'PLAY_COOP.ps1')
foreach($mode in @('HOST','JOIN')){
    '@echo off',('powershell -NoProfile -ExecutionPolicy Bypass -File "%~dp0PLAY_COOP.ps1" -Mode '+$(if($mode -eq 'HOST'){'Host'}else{'Join'})),'if errorlevel 1 pause'|Set-Content -LiteralPath (Join-Path $PackageRoot ($mode+'_COOP.cmd'))
}
('Enabled LAN package: '+$checks+' assertions; '+$files.Count+' payload hashes. Separate PCs and physical controllers remain unverified.')
