#include "player.h"

#include "constants.h"
#include "level.h"
#include "misc.h"

static constexpr auto GRAVITY = 8u;
constexpr geometry::Size Player::size;
static constexpr auto jump_velocity = misc::make_array<int>(-8, -8, -8, -4, -4, -2, -2, -2, -2, 2, 2, 2, 2, 4, 4);
static constexpr auto jump_velocity_fall_index = 9u;

void Player::update(const Level& level)
{
  /**
   * Updating the player is done in these steps:
   * 2. Update player velocity based on player information
   * 3. Update player position based on player velocity
   * 4. Update player information based on collision
   */

  /**
   * 2. Update player velocity based on player information
   */
  if (power_tick > 0)
  {
    power_tick--;
  }

  if (hurt_tick > 0)
  {
    hurt_tick--;
  }

  // Set y velocity
  if (!noclip)
  {
    if (jumping)
    {
      velocity = Vector<int>(velocity.x(), jump_velocity[jump_tick]);
    }
    else
    {
      velocity = Vector<int>(velocity.x(), GRAVITY);
    }
    if (reverse_gravity)
    {
      velocity = Vector<int>(velocity.x(), -velocity.y());
    }
  }

  // Set x velocity
  if (walking)
  {
    // First step is 2 pixels / tick, then 4 pixels / tick
    const auto walk_velocity = walk_tick == 0u ? 2 : 4;
    if (direction == Player::Direction::right)
    {
      velocity = Vector<int>(walk_velocity, velocity.y());
    }
    else  // direction == Player::Direction::left
    {
      velocity = Vector<int>(-walk_velocity, velocity.y());
    }
  }
  else
  {
    velocity = Vector<int>(0, velocity.y());
  }

  /**
   * 3. Update player position based on player velocity
   */

  collide_x = false;
  collide_y = false;
  const auto destination = position + velocity;

  // Move on x axis
  const auto step_x = destination.x() > position.x() ? 1 : -1;
  while (position.x() != destination.x())
  {
    const auto new_player_pos = position + geometry::Position(step_x, 0);

    if (!noclip &&
        (level.collides_solid(new_player_pos, size) ||
         // collide with world edges
         new_player_pos.x() < 0 || new_player_pos.x() >= level.width * SPRITE_W - size.x()))
    {
      collide_x = true;
      break;
    }

    position = new_player_pos;
  }

  // Move on y axis
  const auto step_y = destination.y() > position.y() ? 1 : -1;
  while (position.y() != destination.y())
  {
    const auto new_player_pos = position + geometry::Position(0, step_y);

    // If player is falling down (step_y == 1) we need to check for collision with platforms
    if (!noclip && (level.collides_solid(new_player_pos, size) || (step_y == 1 && level.player_on_platform(new_player_pos, size))))
    {
      collide_y = true;
      break;
    }

    position = new_player_pos;
  }

  /**
   * 4. Update player information based on collision
   */

  // Check if player hit something while walking
  if (walking && collide_x)
  {
    walking = false;
  }

  // Check if player still jumping
  if (jumping)
  {
    // Check if player hit something while jumping
    if (collide_y)
    {
      if (reverse_gravity ? velocity.y() > 0 : velocity.y() < 0)
      {
        // Player hit something while jumping up
        const auto position_below = position - geometry::Position(0, step_y);
        if (level.collides_solid(position_below, size) || level.player_on_platform(position_below, size))
        {
          // If player already on platform, stop jumping immediately
          jumping = false;
        }
        else
        {
          // Skip to "falling down" velocity
          jump_tick = jump_velocity_fall_index;
        }
      }
      else  // velocity.y() > 0
      {
        // Player landed
        jumping = false;
      }
    }
    else if (jump_tick == jump_velocity.size() - 1u)
    {
      // Player jump ended
      jumping = false;
    }
    else if (jump_tick != 0 && level.collides_solid(position + geometry::Position(0, reverse_gravity ? -1 : 1), size))
    {
      // Player did not actually collide with the ground, but standing directly above it
      // and this isn't the first tick in the jump, so we can consider the jump to have
      // ended here
      jumping = false;
    }
  }

  // Check if player is falling
  if (!noclip)
  {
    falling = !jumping && velocity.y() > 0 && !collide_y && !level.collides_solid(position + geometry::Position(0, 1), size);
  }
}

void Player::hurt()
{
  // Hurt me- you can't get hurt again
  if (hurt_tick > 0)
  {
    return;
  }

  hurt_tick = 24;

  if (health_ > 0)
  {
    health_--;
  }
  // TODO: die on 0 health
  // TODO: sound
}

bool Player::is_flashing() const
{
  // Flash on/off for 4 ticks
  return hurt_tick > 0 && (((hurt_tick - 1) / 4) & 1);
}
