#pragma once

#include "player_input.h"
#include <event.h>

PlayerInput input_to_player_input(const Input& input);

static constexpr geometry::Size SCREEN_SIZE = geometry::Size(320, 200);

// The size of the game camera
static constexpr geometry::Size CAMERA_SIZE = geometry::Size(320, 192);

// The size of the game camera after stretching, which is done in the original Crystal Caves
static constexpr geometry::Size CAMERA_SIZE_STRETCHED = geometry::Size(CAMERA_SIZE.x(), CAMERA_SIZE.y() * 6 / 5);

// The size of the game camera after scaling - this should be based on the (dynamic) window size in the future
static constexpr geometry::Size CAMERA_SIZE_SCALED = CAMERA_SIZE_STRETCHED * 3.0f;

// The size of the game window
static constexpr geometry::Size WINDOW_SIZE = CAMERA_SIZE_SCALED;
