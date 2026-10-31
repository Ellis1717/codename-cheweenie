# Pet likeness brief — Codename .... Cheweenie

This extracts high-level appearance details from the owner's retained pictures for the next character-art pass. It is an authoring reference, not a newly generated model, a scan or an animation test. [Structured profiles](../Unreal/UE5MovementLab/SourceArt/PetReferences/appearance_profiles.json) retain the observations, confidence, source IDs and unknowns.

## Evidence and reference priority

The initial retained evidence is four photo files (Marty two, Lakitu one, Rio one). All 19 additional attached files were inspected: **23 distinct photo files**, including repeated views at different resolutions/encodings. This is not 23 independent views. The archive cat pictures are byte-identical to the retained cat references, and Marty's first archive picture duplicates his retained reference. Those copies do not supply extra views.

| ID | Reference | Useful for | Main limitation |
|---|---|---|---|
| M01 | Marty held, frontal three-quarter | Muzzle, eye/ear shapes, coat, chest, collar and paws | Held pose, perspective and partly obscured body |
| M02 | Marty standing on a seat, elevated view | Face, ear variation, body/leg impression and raised tail | Foreshortening and tail blur |
| L01 | Lakitu held, near frontal | Asymmetric mask, nose, whiskers, chest and front paws | Body stretched by the held pose; rear/tail unseen |
| R01 | Rio resting, elevated view | Black face, white chin/bib, dark coat and ears | Tucked limbs, shadowed face and hidden tail |
| C01 | Marty illustrated concept sheet | Finish, provisional turnarounds and pose intentions | Not measured anatomy or gameplay footage |
| B01 | Retained start-page cover | Selected rendering finish, warmth and cozy presentation | Stylized lighting and partly covered anatomy |

Real photographs anchor likeness. The cover remains the requested visual-quality target; concept art guides presentation and provisional forms. Where they differ, the working assumption is recognizable real pets with the cover's finish. The concept's rounder/warmer Marty should not silently replace the photographs' longer muzzle or cream chest.

Raw family photos stay in ignored PrivateReferences. Additional originals stay in ignored .codex-attachments, with 19 hash-verified retained private copies and versioned source manifests. A local source-ID/hash manifest is retained alongside them under ArtAudit. Public notes contain pet-only observations; no photo backgrounds, people, contact details, tag inscriptions, location metadata or background artwork are extracted or republished. Labels or mission text inside concept art are reference content, not development instructions.


### Additional attached views

The owner confirmed all five supplied dog views as Marty on 2026-10-31: M01/M02 and A05–A09, including the repeated held/standing views. The owner also confirmed the five Lakitu views: L01 and A15–A19, including the repeated held view. Rio and mixed-cat A10 identities remain provisional visual matches.

| ID | Candidate pet | Visible evidence / limitation |
|---|---|---|
| A01 | Rio | Reclining side, white socks/front toe patches and dark tail |
| A02 | Rio | Dim head profile, white chin |
| A03 | Rio | Reclining full side, paws and curled dark tail |
| A04 | Rio | Stretched resting, leg markings |
| A05 | Marty | Elevated face in costume, obscured torso |
| A06 | Marty | Same standing view as M02 |
| A07 | Marty | Same held view as M01 |
| A08 | Marty | Seated garment view, ear folds and tan tail |
| A09 | Marty | Toy-carrying turn, motion blur |
| A10 | Rio and Lakitu | Both resting; foreground dark-faced cat |
| A11 | Rio | Side-resting yawn, white paws/underside |
| A12 | Rio | Same loaf view as R01 |
| A13 | Rio | Side resting, greenish-looking eyes and pink visible toe pads |
| A14 | Rio | Costumed standing side, dark tail/white paws |
| A15 | Lakitu | Curled elevated, broad irregular dorsal black patches on white |
| A16 | Lakitu | Front crouch, asymmetric mask/pink nose |
| A17 | Lakitu | Curled side, white underside/dark tail |
| A18 | Lakitu | Same held view as L01 |
| A19 | Lakitu | Front three-quarter crouch, mask/dorsal patch edges |

## Marty

**Identity to preserve:** warm tan/fawn short coat, a projecting tapered muzzle, soft folded ears, pale cream chest/underside, patterned black-and-white collar and a blue bone-shaped tag.

The muzzle narrows toward a shaped, dark rounded-triangular nose. Dark lip edges and individual toes/claws are visible. The eyes look dark and reflective, with eyelid rims; their exact iris colour is not resolved. The ears fold outward/down with rounded tips and darker folded/inner areas. Their spread differs between the two photos, so a single rigid ear pose would miss an important feature.

The standing view suggests an elongated body and compact limbs, but it is not a calibrated side view. The lower belly has finer coat coverage. The tail is raised and blurred in M02; owner-confirmed A05/A08/A09 add narrow tan tail views in different curves. Its neutral profile and measured length remain open. Costume outlines are excluded from anatomy. The collar's black/white motif is clear, while its precise checker/houndstooth-like pattern needs closer evidence. The tag's blue bone silhouette can be retained without copying contact text.

**First modelling fixes:** establish muzzle/skull proportions, shaped eyelids, natural eye size, ear folds and recognizable paws before increasing hair density. Compare ear-tip height against the jaw in matching poses and preserve the spread/fold changes visible between these confirmed views. Keep the fur short and directional. Avoid a spherical snout, oversized toy-like eyes or a plush long-haired coat. Do not turn a photographed open-mouth expression into a permanent grin or a diagnosed bite shape.

## Lakitu

The owner confirmed all five supplied views as Lakitu on 2026-10-31: held (L01/A18), curled dorsal (A15), outdoor front (A16), curled side (A17) and indoor crouch (A19). These confirm identity without adding independent views to the existing reference set.

**Identity to preserve:** the asymmetric black-and-white face, central white blaze, pink nose, white muzzle/chest/front legs, dark crown/ears and long white whiskers.

In the displayed L01 image, the **viewer-right eye sits inside the large black patch** extending onto the cheek. The **viewer-left eye has a largely white surround** beneath the dark crown. The white blaze broadens down the nose bridge into the muzzle. Upright triangular ears have fine light edge hairs. The irises look warm yellow/olive-brown under the photographed lighting; that is a provisional colour observation.

White forelegs, front paws, chest and visible belly contrast with dark side/back areas. Owner-confirmed A15–A17/A19 add broad irregular black back/side patches on a mostly white body, and a dark tail curled beside the body. Exact boundaries, rear paws, tail tip and length remain incomplete. A dark collar and metallic hanging tags are visible; exact accessory design is not locked.

**First modelling fixes:** preserve the face asymmetry with separate marking control on both sides. Keep the pink nose and white muzzle separate from the dark eye patch. Avoid a symmetric tuxedo face or dog-like muzzle. The held pose is a marking reference, not a standing proportion template.

Left/right here is **viewer-relative**. Anatomical sidedness and possible photo mirroring remain unconfirmed; do not silently mirror the markings during UV work.

## Rio

**Identity to preserve:** a predominantly black face/crown/upper muzzle, dark nose, distinct white lower chin/throat/bib and long light whiskers.

The upper body visible in R01 is black with fine fur catching the light. The broad white bib curves beneath the face; its exact upper boundary is partly hidden by the head angle. Upright triangular ears are visible. The eyes are shadowed, so their iris hue and normal opening are unresolved.

The rounded silhouette belongs to a resting/loaf pose with tucked limbs. It does not establish a round standing body, short legs, body size relative to Lakitu or a permanent expression. Candidate A01–A04/A11/A13/A14 reveal white hind socks, white front toes/paws, lighter underside regions and a long dark tapering tail. A13 shows muted greenish-looking irises and some pink toe pads. Exact hue, sock boundaries, full underside/pad pattern and collar details remain open.

**First modelling fixes:** retain the black-face/white-chin distinction from Lakitu. Do not add Lakitu's forehead blaze, white eye surround or pink nose. Reveal black fur through directional detail, highlights and lighting, rather than making the whole coat grey. Use candidate photo evidence for white paws/socks with provisional boundaries; do not infer accessories from the cover alone.

## Motion and the next art gate

These are still photographs. They cannot establish gait timing, joint limits, ear spring/damping, tail wag frequency or how a jump lands. Separate species-appropriate skeletons, skin weights and authored movement remain necessary. Animation targets in the structured profiles are inferred production requirements, not extracted motion data.

The next quality gate is a neutral-lit, in-engine front/side/three-quarter comparison for each pet. Verify anatomy and the identity markings first, then coat/eyes/nose, then actual walk/turn/stop and ear/tail footage. Preserve the working 0.9 build and original assets throughout. Unseen anatomy and markings must remain labelled as inferred until reviewed; owner corrections can update this brief without affecting the current game.

## Verification

Reviewed all four initial pet photos, both presentation references and all 19 attached files. Parsed the structured JSON and checked every observation source ID. Verified SHA-256 equality for 19 retained private copies. Runtime source, content, configuration and the Unity fallback have no changes. Engine compilation, gameplay and controller tests were not rerun for this documentation/authoring-data change; no new gameplay results are claimed.
