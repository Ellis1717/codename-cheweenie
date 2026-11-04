param([Parameter(Mandatory)][string]$PackageRoot,[Parameter(Mandatory)][string]$TestRoot,[ValidateSet('Unified-0.13.0','Unified-0.13.1','Unified-0.14.0','Unified-0.14.1')][string]$ReleaseVersion='Unified-0.13.0')
$ErrorActionPreference='Stop'
$PackageRoot=[IO.Path]::GetFullPath($PackageRoot);$TestRoot=[IO.Path]::GetFullPath($TestRoot)
$windows=Join-Path $PackageRoot 'Windows'
if(!(Test-Path -LiteralPath (Join-Path $windows 'CheweenieUE5Lab.exe'))){throw 'Packaged executable missing'}
$payload=@(Get-ChildItem -LiteralPath $windows -File -Recurse|Where-Object {$_.FullName -notmatch '[\\/]Saved[\\/]|[\\/]LocalGameData[\\/]' -and $_.Extension -ne '.log'})
$newest=($payload|Sort-Object LastWriteTimeUtc -Descending|Select-Object -First 1).LastWriteTimeUtc
$checks=0
foreach($entry in @(@('Home/home-progression.json',34),@('Home/home-resume.json',5),@('Modifier/trick-modifier.json',32),@('Tricks/tricks-smoke.json',29),@('Tricks/tricks-resume.json',5),@('Audio/audio-smoke.json',10),@('Hub/hub-smoke.json',29),@('HubLan/hub-host.json',8),@('HubLan/hub-guest.json',9),@('FurDetail/fur-detail.json',15),@('FurDetail/fur-detail-medium.json',4),@('FurDetail/fur-detail-low.json',4),@('Gait/marty-gait.json',30),@('World/world-smoke.json',14),@('Navigation/navigation-smoke.json',60),@('Navigation/navigation-resume.json',4),@('Life/life-smoke.json',23),@('Life/life-resume.json',5),@('Festival/festival-smoke.json',40),@('Festival/festival-resume.json',6),@('Costumes/costumes-smoke.json',29),@('Costumes/costumes-resume.json',2),@('Coop/coop-host.json',32),@('Coop/coop-guest.json',29),@('Companions/companions-smoke.json',30),@('Companions/companions-resume.json',4),@('Adventure/adventure-smoke.json',25),@('Adventure/adventure-resume.json',6),@('Ratchet/ratchet-unlock.json',6),@('Ratchet/ratchet-resume.json',2),@('Ratchet/ratchet-memory.json',12),@('Ratchet/ratchet-return.json',3),@('UnlockedCoop/coop-host.json',31),@('UnlockedCoop/coop-guest.json',28))){
    $file=Join-Path $TestRoot $entry[0]
    if(!(Test-Path -LiteralPath $file) -or (Get-Item -LiteralPath $file).LastWriteTimeUtc -lt $newest){throw ('Missing/stale acceptance report: '+$entry[0])}
    $result=Get-Content -LiteralPath $file -Raw|ConvertFrom-Json
    if($result.failed.Count -or $result.passed.Count -lt $entry[1]){throw ('Failed/incomplete acceptance: '+$entry[0])};$checks+=$result.passed.Count
}
Add-Type -AssemblyName System.Drawing
$hasOverlays=Test-Path -LiteralPath (Join-Path $PSScriptRoot 'TEST_OVERLAYS.ps1')
if($ReleaseVersion -in @('Unified-0.13.1','Unified-0.14.0','Unified-0.14.1')){
    $brickProbePath=Join-Path $TestRoot 'World/brick-render.json'
    if(!(Test-Path -LiteralPath $brickProbePath) -or (Get-Item -LiteralPath $brickProbePath).LastWriteTimeUtc -lt $newest){throw 'Fresh brick pixel verification missing.'}
    $brickProbe=Get-Content -LiteralPath $brickProbePath -Raw|ConvertFrom-Json
    if(!$brickProbe.clayPaletteVisible -or $brickProbe.width -ne 1920 -or $brickProbe.height -ne 1080){throw 'Brick pixel verification failed.'}
    $releaseReports=,@('World/world-smoke.json',16)
    if($hasOverlays){$releaseReports+=@(@('Overlays/main/overlay-smoke.json',79),@('Overlays/resume/overlay-smoke.json',3))}
    foreach($entry in $releaseReports){
        $path=Join-Path $TestRoot $entry[0]
        if(!(Test-Path -LiteralPath $path) -or (Get-Item -LiteralPath $path).LastWriteTimeUtc -lt $newest){throw ('Missing/stale release report: '+$entry[0])}
        $report=Get-Content -LiteralPath $path -Raw|ConvertFrom-Json
        if($report.failed.Count -or $report.passed.Count -lt $entry[1]){throw ('Failed release report: '+$entry[0])}
        if($entry[0] -notlike 'World/*'){$checks+=$report.passed.Count}
    }
    $releaseCaptures=@('World/WorldBrickDogHeight.png','World/WorldHome.png')
    if($hasOverlays){$releaseCaptures+=@('Overlays/main/OverlaysDefault.png','Overlays/main/OverlaysHidden.png','Overlays/main/OverlaySettings.png')}
    foreach($name in $releaseCaptures){
        $path=Join-Path $TestRoot $name
        if(!(Test-Path -LiteralPath $path) -or (Get-Item -LiteralPath $path).LastWriteTimeUtc -lt $newest){throw ('Missing/stale release capture: '+$name)}
        $bitmap=[Drawing.Bitmap]::new($path)
        try{$expectedWidth=if($name -like 'World/*'){1920}else{1280};$expectedHeight=if($name -like 'World/*'){1080}else{720};if($bitmap.Width -ne $expectedWidth -or $bitmap.Height -ne $expectedHeight){throw ('Invalid release capture dimensions: '+$name)}}finally{$bitmap.Dispose()}
    }
}
foreach($name in @('Home/HomeCatReaction.png','Home/HomeBedRest.png','Home/HomeArmorWorkshop.png','Home/HomeGym.png','Modifier/TrickModifierGuide.png','Modifier/TrickModifierLie.png','Modifier/TrickModifierRoll.png','Modifier/TrickModifierShake.png','Tricks/MartyLying.png','Tricks/MartyRoll.png','Tricks/MartyShake.png','Tricks/MartyBackpack.png','Tricks/MartyBodySway.png','Hub/ModeHub.png','Hub/ModeHubHost.png','Hub/ModeHubArchives.png','FurDetail/FurDetailMenu.png','Gait/MartyFurGameplay.png','Gait/MartyFurFar.png','Gait/MartyLikenessOverview.png','Gait/MartyEarRootMeal.png','Gait/MartyTailRootSway.png','Gait/MartyTailRootSmooth.png','Gait/MartyEarRootIdle.png','Festival/FestivalTitle.png','Festival/FestivalPorch.png','Festival/FestivalFinale.png','Costumes/HalloweenMartySide.png','Costumes/HalloweenMartyTop.png','Costumes/HalloweenMartyLying.png','Costumes/HalloweenMartyRolling.png')){
    $file=Join-Path $TestRoot $name
    if(!(Test-Path -LiteralPath $file) -or (Get-Item -LiteralPath $file).LastWriteTimeUtc -lt $newest){throw ('Missing/stale capture: '+$name)}
    $bitmap=[Drawing.Bitmap]::new($file)
    try{if($bitmap.Width -ne 1280 -or $bitmap.Height -ne 720){throw ('Expected 1280x720 capture: '+$name)}}finally{$bitmap.Dispose()}
}
$wavePath=Join-Path $TestRoot 'Audio/audio-waveforms.json'
if(!(Test-Path -LiteralPath $wavePath) -or (Get-Item -LiteralPath $wavePath).LastWriteTimeUtc-lt $newest){throw 'Audio waveform verification missing or stale'}
$wave=Get-Content -LiteralPath $wavePath -Raw|ConvertFrom-Json
foreach($name in @('Barks','Music','Muted')){ $clip=$wave.$name;if(!$clip -or $clip.seconds-le .5 -or $clip.peak-ge .999 -or ($name-ne 'Muted' -and $clip.rms-le .0002) -or ($name-eq 'Muted' -and $clip.rms-ge .0002)){throw ('Audio waveform check failed: '+$name)} }
if($ReleaseVersion -in @('Unified-0.14.0','Unified-0.14.1')){
    foreach($entry in @(@('Movement/movement-smoke.json',16),@('Life/life-smoke.json',45),@('Life/life-resume.json',7),@('Audio/audio-smoke.json',19),@('Costumes/costumes-smoke.json',44),@('Jackets/jackets-smoke.json',21),@('Jackets/jackets-resume.json',3),@('Upstairs/upstairs-smoke.json',18),@('ControllerGuide/controller-guide-smoke.json',14),@('OverlayStyles/main/overlay-styles.json',11),@('OverlayStyles/resume/overlay-styles.json',4))){
        $path=Join-Path $TestRoot $entry[0]
        if(!(Test-Path -LiteralPath $path) -or (Get-Item -LiteralPath $path).LastWriteTimeUtc -lt $newest){throw ('Missing/stale combined-game report: '+$entry[0])}
        $report=Get-Content -LiteralPath $path -Raw|ConvertFrom-Json
        if($report.failed.Count -or $report.passed.Count -lt $entry[1]){throw ('Failed combined-game report: '+$entry[0])}
        if($entry[0] -like 'Movement/*' -or $entry[0] -like 'Jackets/*' -or $entry[0] -like 'Upstairs/*' -or $entry[0] -like 'ControllerGuide/*' -or $entry[0] -like 'OverlayStyles/*'){$checks+=$report.passed.Count}
    }
    $squeak=$wave.RainbowSqueak
    if(!$squeak -or $squeak.seconds -le .1 -or $squeak.rms -le .0002 -or $squeak.peak -ge .999){throw 'Rainbow Guy bite squeak waveform verification failed'}
    foreach($name in @('Jackets/JacketPink.png','Life/DigDeep.png','Life/RainbowGuy.png','Upstairs/UpstairsGameplay.png','ControllerGuide/ControllerGuideXbox.png','ControllerGuide/ControllerGuidePlayStation.png','ControllerGuide/ControllerGuideKeyboard.png','OverlayStyles/main/Style3Menu.png')){
        $path=Join-Path $TestRoot $name
        if(!(Test-Path -LiteralPath $path) -or (Get-Item -LiteralPath $path).LastWriteTimeUtc -lt $newest){throw ('Missing/stale combined-game capture: '+$name)}
    }
}
if($ReleaseVersion -eq 'Unified-0.14.1'){
    $path=Join-Path $TestRoot 'ControllerButtons/controller-buttons.json'
    if(!(Test-Path -LiteralPath $path) -or (Get-Item -LiteralPath $path).LastWriteTimeUtc -lt $newest){throw 'Fresh controller-button acceptance missing.'}
    $report=Get-Content -LiteralPath $path -Raw|ConvertFrom-Json
    if($report.failed.Count -or $report.passed.Count -lt 50){throw 'Controller-button acceptance failed.'}
    $capture=Join-Path $TestRoot 'ControllerButtons/ControllerButtonsGameplay.png'
    if(!(Test-Path -LiteralPath $capture) -or (Get-Item -LiteralPath $capture).LastWriteTimeUtc -lt $newest){throw 'Fresh rendered controller-button capture missing.'}
    $checks+=$report.passed.Count
}
$bad=@(Get-ChildItem -LiteralPath $TestRoot -Filter '*.log' -Recurse|Select-String -Pattern 'Log\w+: (Error|Fatal):|Failed to compile Material|Default Material will be used|\[VSM\].*overflow')
if($bad.Count){throw 'Final runtime logs contain errors, material fallbacks or VSM overflows'}
$files=@($payload|ForEach-Object {@{path=[IO.Path]::GetRelativePath($windows,$_.FullName).Replace('\','/');sha256=(Get-FileHash -LiteralPath $_.FullName -Algorithm SHA256).Hash}})
if($ReleaseVersion -in @('Unified-0.13.1','Unified-0.14.0','Unified-0.14.1')){Copy-Item -LiteralPath (Join-Path $PSScriptRoot 'PORTABLE_README.txt') -Destination (Join-Path $PackageRoot 'README.txt')}
@{version=$ReleaseVersion;checks=$checks;rendered=$true;overlaysVerified=($hasOverlays -and $ReleaseVersion -in @('Unified-0.13.1','Unified-0.14.0','Unified-0.14.1'));combinedGameVerified=($ReleaseVersion -in @('Unified-0.14.0','Unified-0.14.1'));controllerButtonsVerified=($ReleaseVersion -eq 'Unified-0.14.1');physicalControllersVerified=$false;separateMachinesVerified=$false;files=$files}|ConvertTo-Json -Depth 6|Set-Content -LiteralPath (Join-Path $PackageRoot 'validated-unified.json')
Copy-Item -LiteralPath (Join-Path $PSScriptRoot 'PLAY_GAME.ps1') -Destination (Join-Path $PackageRoot 'PLAY_GAME.ps1')
'@echo off','powershell -NoProfile -ExecutionPolicy Bypass -File "%~dp0PLAY_GAME.ps1" %*','if errorlevel 1 pause'|Set-Content -LiteralPath (Join-Path $PackageRoot 'PLAY_GAME.cmd')
'Unified adventure enabled: '+$checks+' packaged checks, '+$files.Count+' payload hashes and fresh seasonal captures. Visual inspection and physical controllers are separate checks.'
