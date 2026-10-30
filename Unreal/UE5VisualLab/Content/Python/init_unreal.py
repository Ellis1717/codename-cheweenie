"""Optional native render capture; normal editor/game launch does nothing here."""
import os
import time
import unreal

if os.environ.get("CHEWEENIE_UE5_CAPTURE") == "1":
    _started = time.monotonic()
    _capture_requested = False
    _handle = None

    def _capture_tick(delta_seconds):
        global _capture_requested
        elapsed = time.monotonic() - _started
        if not _capture_requested and elapsed > 60:
            world = unreal.get_editor_subsystem(unreal.UnrealEditorSubsystem).get_editor_world()
            level = unreal.get_editor_subsystem(unreal.LevelEditorSubsystem)
            level.set_level_viewport_camera_info(unreal.Vector(-360, -360, 150), unreal.Rotator(pitch=-9, yaw=42), unreal.Name("None"))
            level.editor_set_game_view(True, unreal.Name("None"))
            unreal.SystemLibrary.execute_console_command(world, "r.HighResScreenshotDelay 64")
            unreal.SystemLibrary.execute_console_command(world, "HighResShot 1280x720 filename=UE5Preview.png")
            unreal.log("CHEWEENIE_UE5_CAPTURE_REQUESTED")
            _capture_requested = True
        if _capture_requested and elapsed > 90:
            unreal.unregister_slate_post_tick_callback(_handle)
            unreal.SystemLibrary.quit_editor()

    _handle = unreal.register_slate_post_tick_callback(_capture_tick)
