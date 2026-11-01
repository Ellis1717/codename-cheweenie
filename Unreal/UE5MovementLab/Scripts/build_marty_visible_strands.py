"""Denser raised short hairs on the retained photo-guided Marty anatomy.

Blender 5 --background --factory-startup --python this_file
Earlier models, generators and materials remain intact.
"""
import pathlib, json, hashlib
script = pathlib.Path(__file__).resolve()
source = (script.parent/'build_marty_likeness.py').read_text()
assert 'MartyPhotoReference' in source
source = source.replace('MartyPhotoReference', 'MartyVisibleStrands')
anchor = "exec(compile(soft,str(script),'exec'),globals())"
assert source.count(anchor) == 1
edits = """
# The inherited generator still creates a closed skin and outward single ribbons.
# More coverage, readable widths and modest lift keep the short coat visible.
for old,new in [
    ('32000','64000'),
    ('rng.uniform(.10,.28)','rng.uniform(.20,.48)'),
    ('width=rng.uniform(.006,.012)','width=rng.uniform(.010,.018)'),
    ('direction=(n*.20+flow*.98).normalized()',
     'direction=(n*.46+flow*.888).normalized()'),
    ('rng.uniform(.96,1.04)','rng.uniform(.90,1.12)'),
]:
    assert old in soft,old
    soft=soft.replace(old,new)
"""
source = source.replace(anchor, edits+'\n'+anchor)
exec(compile(source, str(script), 'exec'), globals())
manifest_path = script.parent.parent/'SourceArt'/'MartyVisibleStrands'/'manifest.json'
manifest = json.loads(manifest_path.read_text())
manifest.update(fur_hairs=64000, technique='64000 raised outward-facing single tapered ribbons',
                body_strand_length_cm=[.20,.48], facial_ear_length_multiplier=.55,
                strand_half_width_cm=[.010,.018], normal_lift_fraction=.46,
                strand_colour_variation=[.90,1.12],
                source_script_sha256=hashlib.sha256(script.read_bytes()).hexdigest(),
                performance='Additional vertex work; runtime performance requires measurement')
manifest_path.write_text(json.dumps(manifest, indent=2))
