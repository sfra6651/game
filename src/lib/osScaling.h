#pragma once

// Returns the OS display scaling factor for the primary connected monitor.
// Use this to compensate for the mismatch between raylib's GetMonitorWidth()
// (which returns native resolution) and the actual X11/OS coordinate space.
//
// Usage:
//   float scale = getOsScaleFactor();
//   int w = GetMonitorWidth(monitor) * scale;
//   int h = GetMonitorHeight(monitor) * scale;
//   SetWindowSize(w, h);

float getOsScaleFactor();
