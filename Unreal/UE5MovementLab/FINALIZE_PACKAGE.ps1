param([string]$PackageRoot,[string]$TestRoot)
$ErrorActionPreference='Stop'
if(!$PackageRoot){$PackageRoot=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '..\..\builds\UnrealAdventure-0.9.0'))}
if(!$TestRoot){$TestRoot=Join-Path $PSScriptRoot 'Saved\Packaged09'}
$PackageRoot=[IO.Path]::GetFullPath($PackageRoot);$TestRoot=[IO.Path]::GetFullPath($TestRoot)
$windows=Join-Path $PackageRoot 'Windows'
$game=Join-Path $windows 'CheweenieUE5Lab\Binaries\Win64\CheweenieUE5Lab.exe'
if(!(Test-Path -LiteralPath $game)){throw 'Packaged game missing'}
$payload=@(Get-ChildItem -LiteralPath $windows -Recurse -File | Where-Object {$_.FullName -notmatch '[\\/]Saved[\\/]'})
$newest=($payload | Sort-Object LastWriteTimeUtc -Descending | Select-Object -First 1).LastWriteTimeUtc
$checks=0
$required=@{'Gait\marty-gait.json'=16;'Outside\outside-smoke.json'=12;'Adventure\adventure-smoke.json'=25;'Adventure\adventure-resume.json'=6;'Life\life-smoke.json'=23;'Life\life-resume.json'=5;'Audio\audio-smoke.json'=10;'Tricks\tricks-smoke.json'=29;'Tricks\tricks-resume.json'=5;'Navigation\navigation-smoke.json'=59;'Navigation\navigation-resume.json'=4;'Companions\companions-smoke.json'=30;'Companions\companions-resume.json'=4}
foreach($relative in $required.Keys){
    $path=Join-Path $TestRoot $relative
    if(!(Test-Path -LiteralPath $path) -or (Get-Item -LiteralPath $path).LastWriteTimeUtc -lt $newest){throw ('Missing/stale packaged test report: '+$relative)}
    $report=Get-Content -LiteralPath $path -Raw | ConvertFrom-Json
    if($report.failed.Count -or $report.passed.Count -lt $required[$relative]){throw ('Packaged test failed/incomplete: '+$relative)}
    $checks+=$report.passed.Count
}
if($checks -lt 228){throw 'Expected at least 228 packaged assertions'}
$performancePath=Join-Path $TestRoot 'Companions\route-performance.json'
if(!(Test-Path $performancePath) -or (Get-Item $performancePath).LastWriteTimeUtc -lt $newest){throw 'Missing/stale route-cache measurements'}
$performance=Get-Content $performancePath -Raw|ConvertFrom-Json
if($performance.samples -ne 64 -or $performance.graphBuilds -ne 1 -or ![double]::IsFinite($performance.medianRouteMs) -or $performance.medianRouteMs -lt 0 -or $performance.p95RouteMs -lt $performance.medianRouteMs){throw 'Invalid route-cache measurements'}
$coatPath=Join-Path $TestRoot 'Gait\coat-render.json'
if(!(Test-Path $coatPath) -or (Get-Item $coatPath).LastWriteTimeUtc -lt $newest){throw 'Missing/stale coat colour and resolution check'}
$coat=Get-Content $coatPath -Raw|ConvertFrom-Json
if(!$coat.tanCoatVisible -or $coat.width -ne 1920 -or $coat.height -ne 1080){throw 'Coat colour or full-HD portrait failed'}
$badShaders=@(Get-ChildItem $TestRoot -Filter '*.log' -Recurse|Where-Object {$_.Name -notmatch 'backup'}|Select-String -Pattern 'Failed to compile Material|Default Material will be used in game')
if($badShaders.Count){throw 'Material compilation fallback appeared in the final game logs'}
$audioPath=Join-Path $TestRoot 'Audio\audio-waveforms.json'
if(!(Test-Path $audioPath) -or (Get-Item $audioPath).LastWriteTimeUtc -lt $newest){throw 'Missing/stale audio waveform evidence'}
$audio=Get-Content $audioPath -Raw|ConvertFrom-Json
if($audio.Barks.rms -le .0002 -or $audio.Music.rms -le .0002 -or $audio.Muted.rms -ge .0002){throw 'Audio output or mute check failed'}
foreach($name in @('Barks','Music','Muted')){if($audio.$name.peak -ge .999 -or $audio.$name.seconds -le .5){throw 'Clipped or truncated audio capture'}}
foreach($relative in @('Gait\MartyPortrait.png','Gait\MartyWalk.png','Outside\OutsideArrival.png','Outside\OutsidePark.png','Outside\OutsideSchool.png','Adventure\AdventureTitle.png','Life\LifeDig.png','Life\LifeKitchen.png','Life\LifeJump.png','Life\LifeCouchTV.png','Tricks\MartyLying.png','Tricks\MartyRoll.png','Tricks\MartyShake.png','Tricks\MartyBackpack.png','Tricks\MartyBodySway.png','Navigation\NavigationIndoor.png','Navigation\NavigationSwitches.png','Navigation\NavigationOutside.png','Navigation\NavigationArea.png','Navigation\NavigationPetSwitches.png','Companions\CompanionCrew.png','Companions\CompanionMenu.png','Companions\CompanionSwitch.png')){
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
$marker=[ordered]@{version='0.9.0';rendered=$true;checks=$checks;input='Synthetic keyboard/gamepad; physical hardware unverified';runtimeSavedExcluded=$true;files=$files}
$marker|ConvertTo-Json -Depth 5|Set-Content -LiteralPath (Join-Path $PackageRoot 'validated.json')
'Validated package enabled in UNREAL_PLAY.cmd: '+$checks+' assertions, '+$files.Count+' immutable payload files'
