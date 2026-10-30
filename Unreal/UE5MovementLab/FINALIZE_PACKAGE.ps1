param([string]$PackageRoot,[string]$TestRoot)
$ErrorActionPreference='Stop'
if(!$PackageRoot){$PackageRoot=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '..\..\builds\UnrealAdventure-0.5.0'))}
if(!$TestRoot){$TestRoot=Join-Path $PSScriptRoot 'Saved\Packaged05'}
$PackageRoot=[IO.Path]::GetFullPath($PackageRoot);$TestRoot=[IO.Path]::GetFullPath($TestRoot)
$windows=Join-Path $PackageRoot 'Windows'
$game=Join-Path $windows 'CheweenieUE5Lab\Binaries\Win64\CheweenieUE5Lab.exe'
if(!(Test-Path -LiteralPath $game)){throw 'Packaged game missing'}
$payload=@(Get-ChildItem -LiteralPath $windows -Recurse -File | Where-Object {$_.FullName -notmatch '[\\/]Saved[\\/]'})
$newest=($payload | Sort-Object LastWriteTimeUtc -Descending | Select-Object -First 1).LastWriteTimeUtc
$checks=0
$required=@{'Gait\marty-gait.json'=14;'Outside\outside-smoke.json'=12;'Adventure\adventure-smoke.json'=25;'Adventure\adventure-resume.json'=6}
foreach($relative in @('Gait\marty-gait.json','Outside\outside-smoke.json','Adventure\adventure-smoke.json','Adventure\adventure-resume.json')){
    $path=Join-Path $TestRoot $relative
    if(!(Test-Path -LiteralPath $path) -or (Get-Item -LiteralPath $path).LastWriteTimeUtc -lt $newest){throw ('Missing/stale packaged test report: '+$relative)}
    $report=Get-Content -LiteralPath $path -Raw | ConvertFrom-Json
    if($report.failed.Count -or $report.passed.Count -lt $required[$relative]){throw ('Packaged test failed/incomplete: '+$relative)}
    $checks+=$report.passed.Count
}
if($checks -lt 57){throw 'Expected at least 57 packaged assertions'}
foreach($relative in @('Gait\MartyPortrait.png','Gait\MartyWalk.png','Outside\OutsideArrival.png','Outside\OutsidePark.png','Outside\OutsideSchool.png','Adventure\AdventureTitle.png')){
    $path=Join-Path $TestRoot $relative
    if(!(Test-Path -LiteralPath $path) -or (Get-Item -LiteralPath $path).LastWriteTimeUtc -lt $newest){throw ('Missing/stale rendered capture: '+$relative)}
}
Add-Type -AssemblyName System.Drawing
foreach($relative in @('Outside\OutsideArrival.png','Outside\OutsidePark.png','Outside\OutsideSchool.png')){
    $bitmap=[Drawing.Bitmap]::new((Join-Path $TestRoot $relative))
    try{
        $sum=0.0;$samples=0
        for($y=140;$y -lt ($bitmap.Height-100);$y+=32){for($x=80;$x -lt ($bitmap.Width-80);$x+=32){
            $c=$bitmap.GetPixel($x,$y);$sum+=.2126*$c.R+.7152*$c.G+.0722*$c.B;$samples++
        }}
        if(!$samples -or ($sum/$samples) -lt 8){throw ('Daylight capture is nearly black: '+$relative)}
    }finally{$bitmap.Dispose()}
}
$files=@($payload|ForEach-Object {[ordered]@{path=[IO.Path]::GetRelativePath($windows,$_.FullName).Replace('\','/');bytes=$_.Length;sha256=(Get-FileHash -LiteralPath $_.FullName -Algorithm SHA256).Hash.ToLowerInvariant()}})
$marker=[ordered]@{version='0.5.0';rendered=$true;checks=$checks;input='Synthetic keyboard/gamepad; physical hardware unverified';runtimeSavedExcluded=$true;files=$files}
$marker|ConvertTo-Json -Depth 5|Set-Content -LiteralPath (Join-Path $PackageRoot 'validated.json')
'Validated package enabled in UNREAL_PLAY.cmd: '+$checks+' assertions, '+$files.Count+' immutable payload files'
