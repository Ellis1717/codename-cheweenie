param([ValidateSet('Solo','Host','Join')][string]$Mode='Solo',[string]$Address)
$ErrorActionPreference='Stop'
$package=if(Test-Path -LiteralPath (Join-Path $PSScriptRoot 'Windows')){$PSScriptRoot}else{[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '../../builds/UnrealWorld-0.10.0'))}
$marker=Join-Path $package 'validated-world.json'
if(!(Test-Path -LiteralPath $marker)){throw 'Validated World 0.10 package missing. The retained solo and LAN launchers remain available.'}
$gate=Get-Content -LiteralPath $marker -Raw|ConvertFrom-Json
if($gate.version -ne 'World-0.10.0' -or $gate.checks -lt 197 -or !$gate.rendered -or @($gate.files).Count -lt 1){throw 'World package validation incomplete'}
foreach($file in $gate.files){$target=Join-Path $package ('Windows/'+$file.path);if(!(Test-Path -LiteralPath $target) -or (Get-FileHash -LiteralPath $target -Algorithm SHA256).Hash -ne $file.sha256){throw 'World package changed after validation; rerun packaged checks and FINALIZE_WORLD.ps1'}}
if($Mode -ne 'Solo'){& (Join-Path $package 'PLAY_COOP.ps1') -Mode $Mode -Address $Address;return}
$argsList=@('-dx12','-windowed','-ResX=1920','-ResY=1080')
$local=Join-Path $package 'LocalGameData'
if(Test-Path -LiteralPath (Join-Path $local 'streets.json')){$argsList+=('-CheweenieNeighbourhood="'+(Join-Path $local 'streets.json')+'"')}
if(Test-Path -LiteralPath (Join-Path $local 'NeighbourDogs.json')){$argsList+=('-CheweenieDogs="'+(Join-Path $local 'NeighbourDogs.json')+'"')}
Start-Process -FilePath (Join-Path $package 'Windows/CheweenieUE5Lab.exe') -ArgumentList $argsList
