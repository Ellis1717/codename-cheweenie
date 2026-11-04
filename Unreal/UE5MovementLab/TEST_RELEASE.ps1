param([Parameter(Mandatory)][string]$GameExe,[Parameter(Mandatory)][string]$OutputRoot,[string]$LegacySave,[string]$MapFile,[int]$Port=19140)
$ErrorActionPreference='Stop'
$GameExe=[IO.Path]::GetFullPath($GameExe);$OutputRoot=[IO.Path]::GetFullPath($OutputRoot)
if(!(Test-Path -LiteralPath $GameExe)){throw 'Release executable missing'}
New-Item -ItemType Directory -Path $OutputRoot -Force|Out-Null
# Suites are sequential: they intentionally exercise persisted preferences and unlocks.
foreach($suite in @(@('NAVIGATION','Navigation'),@('COMPANIONS','Companions'),@('ADVENTURE','Adventure'))){
    & (Join-Path $PSScriptRoot ('TEST_'+$suite[0]+'.ps1')) -GameExe $GameExe -OutputRoot (Join-Path $OutputRoot $suite[1])
}
& (Join-Path $PSScriptRoot 'TEST_LIFE.ps1') -GameExe $GameExe -Render -OutputRoot (Join-Path $OutputRoot 'Life')
& (Join-Path $PSScriptRoot 'TEST_JACKETS.ps1') -GameExe $GameExe -Render -OutputRoot (Join-Path $OutputRoot 'Jackets')
& (Join-Path $PSScriptRoot 'TEST_UPSTAIRS.ps1') -GameExe $GameExe -Render -OutputRoot (Join-Path $OutputRoot 'Upstairs')
& (Join-Path $PSScriptRoot 'TEST_CONTROLLER_GUIDE.ps1') -GameExe $GameExe -Render -OutputRoot (Join-Path $OutputRoot 'ControllerGuide')
& (Join-Path $PSScriptRoot 'TEST_OVERLAY_STYLES.ps1') -GameExe $GameExe -Render -OutputRoot (Join-Path $OutputRoot 'OverlayStyles')
& (Join-Path $PSScriptRoot 'TEST_WORLD.ps1') -GameExe $GameExe -Render -ReadableBrick -OutputRoot (Join-Path $OutputRoot 'World')
$homeChecks=@{GameExe=$GameExe;Render=$true;OutputRoot=(Join-Path $OutputRoot 'Home')}
if($LegacySave){$homeChecks.LegacySave=$LegacySave}
& (Join-Path $PSScriptRoot 'TEST_HOME.ps1') @homeChecks
foreach($suite in @(@('MODE_HUB','Hub'),@('TRICK_MODIFIER','Modifier'),@('TRICKS','Tricks'),@('GAIT','Gait'),@('FUR_DETAIL','FurDetail'),@('COSTUMES','Costumes'),@('FESTIVAL','Festival'))){
    & (Join-Path $PSScriptRoot ('TEST_'+$suite[0]+'.ps1')) -GameExe $GameExe -Render -OutputRoot (Join-Path $OutputRoot $suite[1])
}
if(Test-Path -LiteralPath (Join-Path $PSScriptRoot 'TEST_OVERLAYS.ps1')){& (Join-Path $PSScriptRoot 'TEST_OVERLAYS.ps1') -GameExe $GameExe -Render -OutputRoot (Join-Path $OutputRoot 'Overlays')}
& (Join-Path $PSScriptRoot 'TEST_AUDIO.ps1') -GameExe $GameExe -OutputRoot (Join-Path $OutputRoot 'Audio')
& (Join-Path $PSScriptRoot 'TEST_MODE_HUB.ps1') -GameExe $GameExe -Lan -Port $Port -OutputRoot (Join-Path $OutputRoot 'HubLan')
& (Join-Path $PSScriptRoot 'TEST_COOP.ps1') -GameExe $GameExe -Port ($Port+1) -OutputRoot (Join-Path $OutputRoot 'Coop')
& (Join-Path $PSScriptRoot 'TEST_RATCHET.ps1') -GameExe $GameExe -OutputRoot (Join-Path $OutputRoot 'RatchetSeed')
& (Join-Path $PSScriptRoot 'TEST_COOP.ps1') -GameExe $GameExe -Ratchet -Port ($Port+2) -OutputRoot (Join-Path $OutputRoot 'UnlockedCoop')
& (Join-Path $PSScriptRoot 'TEST_RATCHET.ps1') -GameExe $GameExe -Render -OutputRoot (Join-Path $OutputRoot 'Ratchet')
& (Join-Path $PSScriptRoot 'CHECK_COAT.ps1') -OutputRoot (Join-Path $OutputRoot 'Gait') -Width 1280 -Height 720
if($MapFile){& (Join-Path $PSScriptRoot 'TEST_OUTSIDE.ps1') -GameExe $GameExe -MapFile $MapFile -OutputRoot (Join-Path $OutputRoot 'PrivateMap')}
& (Join-Path $PSScriptRoot 'FINALIZE_UNIFIED.ps1') -PackageRoot (Split-Path -Parent (Split-Path -Parent $GameExe)) -TestRoot $OutputRoot -ReleaseVersion 'Unified-0.14.0'
