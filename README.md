# Frame Window (starter)

Build: install Geode CLI + SDK, then `geode build` (Android: `geode build -p android64`, needs NDK).

Done: per-attempt click recorder, HUD with frame gap, JSON export to the mod save dir.
Next (scan): for each click, replay the level from a nearby checkpoint with that click shifted
by -N..+N frames at a fixed TPS, mark each offset as pass/fail; the pass range is the window.
Needs deterministic input injection (override GJBaseGameLayer::processCommands) and state
snapshots (CheckpointObject) to avoid replaying from the start.
