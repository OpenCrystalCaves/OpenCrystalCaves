#include "utils.h"

PlayerInput input_to_player_input(const Input& input)
{
  PlayerInput pi;
  pi.left = input.left.down;
  pi.right = input.right.down;
  pi.up = input.up.down;
  pi.down = input.down.down;
  pi.jump = input.z.down;
  pi.shoot = input.x.pressed();
  pi.left_pressed = input.left.pressed();
  pi.right_pressed = input.right.pressed();
  pi.up_pressed = input.up.pressed();
  pi.down_pressed = input.down.pressed();
  pi.jump_pressed = input.z.pressed();
  pi.shoot_pressed = input.x.pressed();
  pi.noclip_pressed = input.noclip.pressed();
  pi.ammo_pressed = input.ammo.pressed();
  pi.godmode_pressed = input.godmode.pressed();
  pi.reverse_gravity_pressed = input.reverse_gravity.pressed();
  pi.remaster_pressed = input.space.pressed();
  return pi;
}
