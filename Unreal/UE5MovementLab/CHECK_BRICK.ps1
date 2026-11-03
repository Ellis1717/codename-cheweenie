param([Parameter(Mandatory)][string]$ImagePath,[string]$ReportPath)
$ErrorActionPreference='Stop'
Add-Type -AssemblyName System.Drawing
if(!$ReportPath){$ReportPath=Join-Path (Split-Path -Parent $ImagePath) 'brick-render.json'}
$bitmap=[Drawing.Bitmap]::new([IO.Path]::GetFullPath($ImagePath))
try{
    if($bitmap.Width -ne 1920 -or $bitmap.Height -ne 1080){throw 'Expected a full-HD brick inspection capture.'}
    $r=0.;$g=0.;$b=0.;$samples=0
    # The scripted inspection frames the wall here; ground and sky are excluded.
    for($y=100;$y -lt 650;$y+=16){for($x=160;$x -lt 1640;$x+=16){$color=$bitmap.GetPixel($x,$y);$r+=$color.R;$g+=$color.G;$b+=$color.B;$samples++}}
    $r/=$samples;$g/=$samples;$b/=$samples
    if($r -lt 35 -or $r -lt $g*1.25 -or $g -lt $b*1.05){throw 'Brick capture lacks the clay palette; inspect for a missing-parent/default-grid material.'}
    @{width=$bitmap.Width;height=$bitmap.Height;meanRGB=@($r,$g,$b);samples=$samples;clayPaletteVisible=$true;scope='Actual pixels in the scripted wall inspection; not a realism score or performance benchmark'}|ConvertTo-Json|Set-Content -LiteralPath $ReportPath
}finally{$bitmap.Dispose()}
