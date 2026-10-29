$ErrorActionPreference = 'Stop'
$assets = Get-ChildItem Assets -Recurse | Where-Object { $_.Extension -ne '.meta' -and $_.FullName -notmatch '\\Generated(\\|$)' }
foreach ($asset in $assets) {
    $meta = $asset.FullName + '.meta'
    if (!(Test-Path -LiteralPath $meta)) {
        $text = "fileFormatVersion: 2`nguid: $([guid]::NewGuid().ToString('N'))`n"
        if ($asset.PSIsContainer) { $text += "folderAsset: yes`nDefaultImporter:`n  externalObjects: {}`n  userData:`n  assetBundleName:`n  assetBundleVariant:`n" }
        [IO.File]::WriteAllText($meta, $text)
    }
}
$files = @('.gitignore','AGENTS.md','README.md','PLAY.cmd') + @(Get-ChildItem Assets,Packages,ProjectSettings,docs,Validation -Recurse -File | Where-Object { $_.FullName -notmatch '\\(bin|obj|Generated|UnityAudioProject|PrivateNeighbourhood)\\' -and $_.Name -notin @('Generated.meta','PrivateNeighbourhood.meta') } | ForEach-Object { $_.FullName })
$items = foreach ($f in $files) {
    $i = Get-Item -LiteralPath $f
    $binary = $i.Extension -in @('.png','.jpg','.jpeg','.wav','.fbx')
    $content = if ($binary) { [Convert]::ToBase64String([IO.File]::ReadAllBytes($i.FullName)) } else { [IO.File]::ReadAllText($i.FullName) }
    @{path=[IO.Path]::GetRelativePath((Get-Location).Path,$i.FullName).Replace('\','/'); content=$content; encoding=$(if ($binary) {'base64'} else {'utf-8'})}
}
ConvertTo-Json -InputObject @($items) -Depth 3 -Compress
