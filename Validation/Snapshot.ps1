$ErrorActionPreference = 'Stop'
$assets = Get-ChildItem Assets -Recurse | Where-Object { $_.Extension -ne '.meta' }
foreach ($asset in $assets) {
    $meta = $asset.FullName + '.meta'
    if (!(Test-Path -LiteralPath $meta)) {
        $text = "fileFormatVersion: 2`nguid: $([guid]::NewGuid().ToString('N'))`n"
        if ($asset.PSIsContainer) { $text += "folderAsset: yes`nDefaultImporter:`n  externalObjects: {}`n  userData: `n  assetBundleName: `n  assetBundleVariant: `n" }
        [IO.File]::WriteAllText($meta, $text)
    }
}
$files = @('.gitignore','AGENTS.md','README.md') + @(Get-ChildItem Assets,Packages,ProjectSettings,docs,Validation -Recurse -File | Where-Object { $_.FullName -notmatch '\\(bin|obj)\\' } | ForEach-Object { $_.FullName })
$items = foreach ($f in $files) {
    $i = Get-Item -LiteralPath $f
    @{path=[IO.Path]::GetRelativePath((Get-Location).Path,$i.FullName).Replace('\','/'); content=[IO.File]::ReadAllText($i.FullName)}
}
ConvertTo-Json -InputObject @($items) -Depth 3 -Compress
