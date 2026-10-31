param([string]$Blender='C:\Program Files\Blender Foundation\Blender 5.0\blender.exe')
$ErrorActionPreference='Stop'
if(!(Test-Path -LiteralPath $Blender)){throw 'Blender 5 is required to regenerate original scenery; supply -Blender.'}
& $Blender --background --python-exit-code 1 --python (Join-Path $PSScriptRoot 'Scripts/build_world_art.py')
if($LASTEXITCODE){throw 'Original world asset generation failed'}
$manifest=Get-Content -LiteralPath (Join-Path $PSScriptRoot 'SourceArt/WorldRealism/manifest.json') -Raw|ConvertFrom-Json
if(@($manifest.meshes.PSObject.Properties).Count -lt 12){throw 'World mesh manifest incomplete'}
'Original scenery exported. Run IMPORT_WORLD.ps1, then BUILD.ps1 and TEST_WORLD.ps1.'
