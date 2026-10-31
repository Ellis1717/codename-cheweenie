# Codename .... Cheweenie — LAN Cozy Crew

LAN Cozy Crew is a separate two-player room mode using the real playable Comparison map and the retained Marty, Lakitu and Rio assets. One Windows PC hosts and plays; a second Windows PC joins its IPv4 address. No account service or paid dependency is required. Both PCs need the same packaged build. Each player owns one pet; a pet cannot be selected by both people at once.

This first networking milestone supports independent movement/cameras, walking/hurry/jump, pet selection, replicated pet voices and a shared carry/drop ball. Marty retains his procedural skinned gait. The cats use their existing static meshes and do not yet have articulated legs. Outdoor travel, solo quests, health/energy, digging, inventory, companion AI and shared save progression are not networked in this mode. The solo launcher, save and validated 0.9 package remain available independently.

## Play on two Windows PCs

1. Extract `builds/Codename-Cheweenie-LAN-0.1.0.zip` on the second PC, or copy the complete `builds/UnrealCoop-0.1.0` folder, including Windows, launch scripts and validated-coop.json. Unreal Editor is not required for the package. Install the included Unreal prerequisites on a PC that needs them (`Windows/Engine/Extras/Redist/en-us/UEPrereqSetup_x64.exe`). On the development PC, desktop shortcuts Host Cheweenie LAN and Join Cheweenie LAN target this independent package.
2. Connect both PCs to the same home Wi-Fi/Ethernet LAN. On the first PC run HOST_COOP.cmd. Keep the game running; its console lists available host IPv4 addresses and network adapters.
3. On the second PC run JOIN_COOP.cmd and enter the host's Wi-Fi/Ethernet IPv4 address. Use the host address, not the guest address. The default port is UDP 7777. If Windows asks, allow the game on the home/private network. This workflow does not change firewall rules automatically or require router port forwarding.
4. The host begins as Marty; the first guest begins as Lakitu. Press 3 to switch the guest to Rio, or use D-pad left/right to cycle available pets. Host and guest can each change to an unclaimed pet.
5. Close the game to leave. If the host closes, the session ends; launch Host/Join again to reconnect. No host migration or internet matchmaking is implemented. If joining fails, check that both copies match, the host is running, the address is correct, the network permits peer traffic, and UDP 7777 is allowed. Hotel/guest Wi-Fi often isolates devices.

| Action | Keyboard/mouse | Xbox / PlayStation |
|---|---|---|
| Move / look | WASD / mouse | Left stick / right stick |
| Hurry while held | Shift | A / Cross |
| Jump | Space | Y / Triangle |
| Pet voice | B | LB / L1 |
| Pick up / drop ball | E / Q | X-Square / LT-L2 |
| Select Marty / Lakitu / Rio | 1 / 2 / 3 | D-pad left/right cycles free pets |
| Camera / recenter | V / R | R3 / RB-R1 |
| Return to room spawn | H | D-pad Down |
| Controls overlay | Esc | Options / Menu |

The controls overlay blocks local walking without pausing friends. Cameras and input belong to each controller. Raw pet photos, personalized household references and private street data are not needed by or copied into this public room package.

## Architecture and optimization

CozyCoopMode and CozyCoopState own the room. CozyCoopPet subclasses the existing movement character, with a separate Enhanced Input context and no Adventure/DogLife save ticking. Shared deterministic room collision is prepared on every peer. Server-side selection checks prevent duplicate identities, and pickup checks validate distance/ownership. The server limits voice requests to one per 1.2 seconds and applies walking/hurry speeds itself. CharacterMovement provides predicted owner movement and replicated remote movement.

Network updates are capped at 20 Hz per pet (2 Hz minimum); skeletal hair/bone transforms are calculated locally rather than replicated. Remote Marty gait ticks at 20 Hz while the owner's gait uses normal frame updates. Cat gait and unused needs ticks are disabled. The ball has no idle tick, updates its held position locally on each peer, and disables replicated movement while held; carrier changes and the final dropped transform replicate. These are explicit work reductions, not a measured claim about whole-game FPS or WAN latency.

## Build and verify

Use the Unreal 5.8.4 toolchain recorded in the handoff. Build editor source with `Unreal/UE5MovementLab/BUILD.ps1`, then run TEST_COOP.ps1 (two real loopback processes). Add -Render for actual DX12 offscreen gameplay captures. The test injects keyboard and gamepad events; it is not a physical controller or separate-machine LAN test.

Package to a new directory, preserving existing releases:

```powershell
./Unreal/UE5MovementLab/PACKAGE.ps1 -OutputRoot 'D:/CheweenieValidation/GraphicsLabBuilds/UnrealCoop-0.1.0'
```

Repeat TEST_COOP.ps1 with -GameExe pointing at the new Windows/CheweenieUE5Lab.exe and fresh TestRoot/Headless and TestRoot/Rendered output folders (add -Render for the latter). Run TEST_COMPANIONS.ps1 with that binary and TestRoot/Solo, and the existing solo editor movement check, because collision setup is shared. Then run FINALIZE_COOP.ps1 -PackageRoot <package> -TestRoot <test root>. It verifies the packaged reports/captures/logs, writes the validated-coop.json payload hash manifest and adds portable launchers. Do not reuse the solo validated.json marker or replace UNREAL_PLAY.cmd with an unverified build.

Final verification passed: native Editor/Game builds, full cook/archive, 45 headless network + 45 rendered network + 34 solo companion play/reload checks (124 packaged assertions). The solo editor movement regression passed 16 before the final co-op-only smoothing attachment change; the shared solo source was unchanged afterward. Both actual packaged screenshots were inspected. The final runtime logs contained zero Error/Fatal categories or material fallbacks. All 43 payload hashes were independently verified inside the 490,708,596-byte transfer ZIP. Windows PowerShell 5.1 validated the package and rejected an invalid join address.

[Actual reports and repaired development findings](evidence/ue5-coop-build.json), [host view](screenshots/ue5-coop-host.png), [guest view](screenshots/ue5-coop-guest.png). Physical controllers, independent PCs, Wi-Fi/firewall behavior, subjective audio and whole-game performance remain unverified. The ZIP excludes runtime Saved folders and private references. Build products remain local and outside Git; scripts, source, documentation and sanitized evidence are versioned.
