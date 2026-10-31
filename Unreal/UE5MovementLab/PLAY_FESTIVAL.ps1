
$ErrorActionPreference='Stop'
$package=if(Test-Path -LiteralPath (Join-Path $PSScriptRoot 'Windows')){$PSScriptRoot}else{[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '../../builds/UnrealBorkOrTreat-0.11.0'))}
$marker=Join-Path $package 'validated-festival.json'
if(!(Test-Path -LiteralPath $marker)){throw 'Validated Halloween package missing; see UNREAL_HALLOWEEN_SPECIAL.md.'}
$gate=Get-Content -LiteralPath $marker -Raw|ConvertFrom-Json
if($gate.version -ne 'Festival-0.11.0' -or $gate.checks -lt 260 -or !$gate.rendered -or @($gate.files).Count -lt 1){throw 'Halloween package validation incomplete'}
foreach($file in $gate.files){$target=Join-Path $package ('Windows/'+$file.path);if(!(Test-Path -LiteralPath $target) -or (Get-FileHash -LiteralPath $target -Algorithm SHA256).Hash -ne $file.sha256){throw 'Halloween package changed after validation; rebuild and rerun the packaged checks.'}}
$argsList=@('-dx12','-windowed','-ResX=1280','-ResY=720')
$local=Join-Path $package 'LocalGameData'
if(Test-Path -LiteralPath (Join-Path $local 'streets.json')){$argsList+=('-CheweenieNeighbourhood="'+(Join-Path $local 'streets.json')+'"')}
if(Test-Path -LiteralPath (Join-Path $local 'NeighbourDogs.json')){$argsList+=('-CheweenieDogs="'+(Join-Path $local 'NeighbourDogs.json')+'"')}
$argsList+='-CheweenieHalloweenSpecial'
Start-Process -FilePath (Join-Path $package 'Windows/CheweenieUE5Lab.exe') -ArgumentList $argsList
