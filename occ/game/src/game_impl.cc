#include "game_impl.h"

#include <array>
#include <cstdint>
#include <sstream>

#include "level_loader.h"
#include "logger.h"
#include "misc.h"

static constexpr auto gravity = 8u;
static constexpr auto jump_velocity = misc::make_array<int>(0, -8, -8, -8, -4, -4, -2, -2, -2, -2, 2, 2, 2, 2, 4, 4);
static constexpr auto jump_velocity_fall_index = 10u;

std::unique_ptr<Game> Game::create()
{
  return std::make_unique<GameImpl>();
}

bool GameImpl::init(const ExeData& exe_data, const LevelId level)
{
  level_ = LevelLoader::load(exe_data, level);
  if (!level_)
  {
    return false;
  }

  player_ = Player();
  player_.position = level_->player_spawn;
  entering_level = level;

  score_ = 0u;
  num_ammo_ = 5u;
  num_lives_ = 3u;
  has_key_ = false;

  missile_.alive = false;

  return true;
}

void GameImpl::update(unsigned game_tick, const PlayerInput& player_input)
{
  (void)game_tick;  // Not needed atm

  // Clear objects_
  objects_.clear();

  // Update the level (e.g. moving platforms and other objects)
  // TODO: don't update enemies off screen
  update_level();
  update_actors();
  update_player(player_input);
  update_items();
  update_missile();
  update_enemies();
  update_hazards();
}

int GameImpl::get_bg_sprite(const int x, const int y) const
{
  return level_->get_bg(x, y);
}

const Tile& GameImpl::get_tile(int tile_x, int tile_y) const
{
  return level_->get_tile(tile_x, tile_y);
}

const Item& GameImpl::get_item(int tile_x, int tile_y) const
{
  return level_->get_item(tile_x, tile_y);
}

std::wstring GameImpl::get_debug_info() const
{
  std::wostringstream oss;

  oss << L"player position: (" << player_.position.x() << L", " << player_.position.y() << L")\n";
  oss << L"player velocity: (" << player_.velocity.x() << L", " << player_.velocity.y() << L")\n";
  oss << L"player collide: " << (player_.collide_x ? L"x " : L"_ ") << (player_.collide_y ? L"y" : L"_") << L"\n";
  oss << L"player walking: " << (player_.walking ? L"true" : L"false") << L"\n";
  oss << L"player jumping: " << (player_.jumping ? L"true" : L"false") << L"\n";
  oss << L"player falling: " << (player_.falling ? L"true" : L"false") << L"\n";
  oss << L"player shooting: " << (player_.shooting ? L"true" : L"false") << "\n";
  oss << L"missile alive: " << (missile_.alive ? L"true" : L"false") << L"\n";
  oss << L"missile position: (" << missile_.position.x() << L", " << missile_.position.y() << L")\n";

  return oss.str();
}

void GameImpl::update_level()
{
  // Update all MovingPlatforms
  for (auto& platform : level_->moving_platforms)
  {
    // Update platform
    const auto player_on_platform = (player_.position.y() + player_.size.y() == platform.position.y()) &&
      (player_.position.x() < platform.position.x() + 16) && (player_.position.x() + player_.size.x() > platform.position.x());

    platform.update(*level_);

    // Move player if standing on platform
    if (player_on_platform)
    {
      // Only move player if not colliding with any static objects
      const auto new_player_pos = player_.position + platform.get_velocity(*level_);
      if (!level_->collides_solid(new_player_pos, player_.size))
      {
        player_.position = new_player_pos;
      }
    }
  }

  // Add moving platforms to objects_
  for (auto& platform : level_->moving_platforms)
  {
    objects_.emplace_back(platform.position, platform.sprite_id, platform.num_sprites, platform.is_reverse());
  }

  // Add entrances
  for (auto& entrance : level_->entrances)
  {
    if (entrance.state != EntranceState::COMPLETE)
    {
      entrance.state =
        geometry::isColliding({player_.position, player_.size}, {entrance.position, 16, 16}) ? EntranceState::OPEN : EntranceState::CLOSED;
      if (entrance.state == EntranceState::OPEN)
      {
        entering_level = static_cast<LevelId>(entrance.level);
      }
    }
    entrance.update();
    objects_.emplace_back(entrance.position, entrance.get_sprite(), 1, false);
  }

  // Add exit
  if (level_->exit)
  {
    if (!level_->exit->open)
    {
      level_->exit->open = geometry::isColliding({player_.position, player_.size}, {level_->exit->position, 16, 32});
      if (level_->exit->open)
      {
        // TODO: play sound
        entering_level = static_cast<LevelId>(LevelId::MAIN_LEVEL);
      }
    }
    level_->exit->update();
    for (const auto& sprite_pos : level_->exit->get_sprites())
    {
      objects_.emplace_back(sprite_pos.first, static_cast<int>(sprite_pos.second), 1, false);
    }
  }
}

void GameImpl::update_player(const PlayerInput& player_input)
{
  /**
   * Updating the player is done in these steps:
   * 1. Update player information based on input
   * 2. Update player velocity based on player information
   * 3. Update player position based on player velocity
   * 4. Update player information based on collision
   */

  if (entering_level != level_->level_id)
  {
    return;
  }

  /**
   * 1. Update player information based on input
   */

  // Check cheat codes
  if (player_input.noclip_pressed)
  {
    player_.noclip = !player_.noclip;
    player_.falling = true;
    player_.velocity = Vector<int>(0, 0);
    LOG_DEBUG("Player noclip %s", player_.noclip ? "ON" : "OFF");
  }
  if (player_input.ammo_pressed)
  {
    num_ammo_ = 40;
    LOG_DEBUG("Extra ammo");
  }
  if (player_input.godmode_pressed)
  {
    player_.godmode = !player_.godmode;
    LOG_DEBUG("God mode %s", player_.godmode ? "ON" : "OFF");
    if (!player_.godmode && player_.reverse_gravity)
    {
      player_.reverse_gravity = false;
      LOG_DEBUG("Reverse gravity OFF");
    }
  }
  if (player_input.reverse_gravity_pressed && player_.godmode)
  {
    player_.reverse_gravity = !player_.reverse_gravity;
    LOG_DEBUG("Reverse gravity %s", player_.reverse_gravity ? "ON" : "OFF");
  }

  // Check left / right
  if ((player_input.left && player_input.right) || (!player_input.left && !player_input.right))
  {
    player_.walking = false;
  }
  else if (player_.walking &&
           ((player_input.right && player_.direction == Player::Direction::right) ||
            (player_input.left && player_.direction == Player::Direction::left)))
  {
    // Player is still walking in the same direction, just update walk tick
    player_.walk_tick += 1u;
  }
  else if (player_input.left || player_input.right)
  {
    // Player started to walk
    player_.walking = true;
    player_.walk_tick = 0u;

    // Set direction
    player_.direction = player_input.right ? Player::Direction::right : Player::Direction::left;
  }

  // Check up / down (noclip)
  if (player_.noclip)
  {
    if (player_input.up)
    {
      player_.velocity = Vector<int>(player_.velocity.x(), -4);
    }
    else if (player_input.down)
    {
      player_.velocity = Vector<int>(player_.velocity.x(), 4);
    }
    else
    {
      player_.velocity = Vector<int>(player_.velocity.x(), 0);
    }
  }

  // Check jump
  if (!player_.noclip)
  {
    if (player_input.jump && !player_.jumping && !player_.falling &&
        !level_->collides_solid(player_.position + geometry::Position(0, player_.reverse_gravity ? 1 : -1), player_.size))
    {
      // Player wants to jump
      player_.jumping = true;
      player_.jump_tick = 0u;
    }
    else if (player_.jumping)
    {
      // Player still jumping
      player_.jump_tick += 1u;
    }
  }

  // Check shooting/interacting
  bool interacted = false;
  if (player_input.shoot)
  {
    const auto rect = geometry::Rectangle(player_.position, player_.size);
    for (auto&& a : level_->actors)
    {
      if (geometry::isColliding(rect, geometry::Rectangle(a->position, a->size)) && a->interact(*level_))
      {
        interacted = true;
        break;
      }
    }
  }
  if (!interacted)
  {
    player_.shooting = player_input.shoot;
  }

  /**
   * 2. Update player velocity based on player information
   */

  // Set y velocity
  if (!player_.noclip)
  {
    if (player_.jumping)
    {
      player_.velocity = Vector<int>(player_.velocity.x(), jump_velocity[player_.jump_tick]);
    }
    else
    {
      player_.velocity = Vector<int>(player_.velocity.x(), gravity);
    }
    if (player_.reverse_gravity)
    {
      player_.velocity = Vector<int>(player_.velocity.x(), -player_.velocity.y());
    }
  }

  // Set x velocity
  if (player_.walking)
  {
    // First step is 2 pixels / tick, then 4 pixels / tick
    const auto velocity = player_.walk_tick == 0u ? 2 : 4;
    if (player_.direction == Player::Direction::right)
    {
      player_.velocity = Vector<int>(velocity, player_.velocity.y());
    }
    else  // player_.direction == Player::Direction::left
    {
      player_.velocity = Vector<int>(-velocity, player_.velocity.y());
    }
  }
  else
  {
    player_.velocity = Vector<int>(0, player_.velocity.y());
  }

  /**
   * 3. Update player position based on player velocity
   */

  player_.collide_x = false;
  player_.collide_y = false;
  const auto destination = player_.position + player_.velocity;

  // Move on x axis
  const auto step_x = destination.x() > player_.position.x() ? 1 : -1;
  while (player_.position.x() != destination.x())
  {
    const auto new_player_pos = player_.position + geometry::Position(step_x, 0);

    if (!player_.noclip &&
        (level_->collides_solid(new_player_pos, player_.size) ||
         // collide with world edges
         new_player_pos.x() < 0 || new_player_pos.x() >= level_->width * 16 - player_.size.x()))
    {
      player_.collide_x = true;
      break;
    }

    player_.position = new_player_pos;
  }

  // Move on y axis
  const auto step_y = destination.y() > player_.position.y() ? 1 : -1;
  while (player_.position.y() != destination.y())
  {
    const auto new_player_pos = player_.position + geometry::Position(0, step_y);

    // If player is falling down (step_y == 1) we need to check for collision with platforms
    if (!player_.noclip && (level_->collides_solid(new_player_pos, player_.size) || (step_y == 1 && player_on_platform(new_player_pos))))
    {
      player_.collide_y = true;
      break;
    }

    player_.position = new_player_pos;
  }

  /**
   * 4. Update player information based on collision
   */

  // Check if player hit something while walking
  if (player_.walking && player_.collide_x)
  {
    player_.walking = false;
  }

  // Check if player still jumping
  if (player_.jumping)
  {
    // Check if player hit something while jumping
    if (player_.collide_y)
    {
      if (player_.reverse_gravity ? player_.velocity.y() > 0 : player_.velocity.y() < 0)
      {
        // Player hit something while jumping up
        // Skip to "falling down" velocity
        player_.jump_tick = jump_velocity_fall_index;
      }
      else  // player_.velocity.y() > 0
      {
        // Player landed
        player_.jumping = false;
      }
    }
    else if (player_.jump_tick == jump_velocity.size() - 1u)
    {
      // Player jump ended
      player_.jumping = false;
    }
    else if (player_.jump_tick != 0 &&
             level_->collides_solid(player_.position + geometry::Position(0, player_.reverse_gravity ? -1 : 1), player_.size))
    {
      // Player did not actually collide with the ground, but standing directly above it
      // and this isn't the first tick in the jump, so we can consider the jump to have
      // ended here
      player_.jumping = false;
    }
  }

  // Check if player is falling
  if (!player_.noclip)
  {
    player_.falling = !player_.jumping && player_.velocity.y() > 0 && !player_.collide_y &&
      !level_->collides_solid(player_.position + geometry::Position(0, 1), player_.size);
  }
}

void GameImpl::update_items()
{
  // Check if player hit an item
  // Player can cover at maximum 4 items
  // Check all 4 items, even though we might check the same item multiple times
  const std::array<geometry::Position, 4> positions = {
    geometry::Position((player_.position.x() + 0) / 16, (player_.position.y() + 0) / 16),
    geometry::Position((player_.position.x() + player_.size.x() - 1) / 16, (player_.position.y() + 0) / 16),
    geometry::Position((player_.position.x() + 0) / 16, (player_.position.y() + player_.size.y() - 1) / 16),
    geometry::Position((player_.position.x() + player_.size.x() - 1) / 16, (player_.position.y() + player_.size.y() - 1) / 16)};

  for (const auto& position : positions)
  {
    const auto& item = get_item(position.x(), position.y());
    if (item.valid())
    {
      switch (item.get_type())
      {
        case ItemType::ITEM_TYPE_CRYSTAL:
          LOG_DEBUG("Player took item of type crystal (%d)", item.get_type());
          score_ += CRYSTAL_SCORE;
          break;
        case ItemType::ITEM_TYPE_AMMO:
          LOG_DEBUG("Player took item of type ammo (%d), amount: %d", item.get_type(), item.get_amount());
          num_ammo_ += item.get_amount();

          // 99 is max ammo
          num_ammo_ = num_ammo_ > MAX_AMMO ? MAX_AMMO : num_ammo_;
          break;
        case ItemType::ITEM_TYPE_SCORE:
          LOG_DEBUG("Player took item of type score (%d), amount: %d", item.get_type(), item.get_amount());
          score_ += item.get_amount();
          break;
        default:
          LOG_ERROR("unknown item type");
          break;
      }

      level_->remove_item(position.x(), position.y());
    }
  }
}

void GameImpl::update_missile()
{
  // Update particles (explosions etc.)
  for (auto it = particles_.begin(); it != particles_.end();)
  {
    auto p = it->get();
    p->update();

    if (!p->is_alive())
    {
      it = particles_.erase(it);
    }
    else
    {
      objects_.emplace_back(p->position, p->get_sprite(), 1, false);
      it++;
    }
  }

  // Move the missile if it's alive
  if (missile_.alive)
  {
    auto speed = missile_.frame < missile_.speed.size() ? missile_.speed[missile_.frame] : missile_.speed.back();
    while (speed-- > 0)
    {
      missile_.position += geometry::Position((missile_.right ? 1 : -1), 0);

      // Adjust position due to collision size being smaller than sprite size
      if (level_->collides_solid(missile_.position + geometry::Position(0, 3), missile_.size))
      {
        missile_.alive = false;
        missile_.set_cooldown();
        particles_.emplace_back(new Explosion(missile_.position));
        break;
      }

      // Adjust position due to collision size being smaller than sprite size
      auto enemy = collides_enemy(missile_.position + geometry::Position(0, 3), missile_.size);
      if (enemy)
      {
        missile_.alive = false;

        enemy->on_hit();

        break;
      }
    }
  }
  missile_.update();

  // Player wants to shoot new missile
  if (player_.shooting && !missile_.is_in_cooldown())
  {
    if (num_ammo_ > 0)
    {
      missile_.alive = true;
      missile_.frame = 0;
      if (player_.direction == Player::Direction::right)
      {
        missile_.right = true;
        missile_.position = player_.position + geometry::Position(player_.size.x() - 2, 0);
      }
      else
      {
        missile_.right = false;
        missile_.position = player_.position - geometry::Position(player_.size.x() - 2, 0);
      }

      num_ammo_ -= 1;
    }
    else
    {
      // Play "no ammo" sound when we have audio
    }
  }

  // Add missile to objects_ if alive
  if (missile_.alive)
  {
    objects_.emplace_back(missile_.position, (missile_.right ? 296 : 302), 6, false);
  }
}

void GameImpl::update_enemies()
{
  for (auto it = level_->enemies.begin(); it != level_->enemies.end();)
  {
    auto e = it->get();
    // TODO: When enemy getting hit and not dying the enemy sprite should turn white for
    //       some time. All colors except black in the sprite should become white.
    //       This is applicable for when the player gets hit as well
    //       Modify the sprite on the fly / some kind of filter, or pre-create white sprites
    //       for all player and enemy sprite when loading sprites?
    e->update({player_.position, player_.size}, *level_);

    // Check if enemy died
    if (!e->is_alive())
    {
      e->on_death(*level_);

      // TODO: When an enemy dies there should be another type of explosion
      //       or bones spawning. The explosion/bones should move during animation
      //       in the same direction as the missile coming from
      // Create explosion where enemy is
      // explosion_.alive = true;
      // explosion_.frame = 0;
      // explosion_.position = e->position;

      // Give score
      score_ += e->get_points();
      // Don't even bother showing score particle unless it is high enough (>= 1000?)
      if (e->get_points() >= 1000)
      {
        particles_.emplace_back(new ScoreParticle(e->position, e->get_points()));
      }

      // Remove enemy
      it = level_->enemies.erase(it);
    }
    else
    {
      for (const auto& sprite_pos : e->get_sprites(*level_))
      {
        objects_.emplace_back(sprite_pos.first, static_cast<int>(sprite_pos.second), 1, false);
      }
      it++;
    }
  }
}

void GameImpl::update_hazards()
{
  for (int i = 0; i < level_->hazards.size(); i++)
  {
    auto h = level_->hazards[i].get();
    h->update({player_.position, player_.size}, *level_);

    // Check if hazard died
    if (!h->is_alive())
    {
      level_->hazards.erase(level_->hazards.begin() + i);
    }
    else
    {
      for (const auto& sprite_pos : h->get_sprites(*level_))
      {
        objects_.emplace_back(sprite_pos.first, static_cast<int>(sprite_pos.second), 1, false);
      }
    }
  }
}

void GameImpl::update_actors()
{
  for (auto&& a : level_->actors)
  {
    a->update({player_.position, player_.size}, *level_);
    for (const auto& sprite_pos : a->get_sprites(*level_))
    {
      objects_.emplace_back(sprite_pos.first, static_cast<int>(sprite_pos.second), 1, false);
    }
  }
}

/**
 * Checks if given position and size collides with any enemy.
 *
 * Returns the first colliding enemy, or null if none found.
 */
Enemy* GameImpl::collides_enemy(const geometry::Position& position, const geometry::Size& size)
{
  const auto rect = geometry::Rectangle(position, size);
  for (auto&& enemy : level_->enemies)
  {
    if (geometry::isColliding(rect, geometry::Rectangle(enemy->position, enemy->size)))
    {
      return enemy.get();
    }
  }
  return nullptr;
}

bool GameImpl::player_on_platform(const geometry::Position& player_position)
{
  // Need to check both static platforms (e.g. foreground items with SOLID_TOP)
  // and moving platforms

  // Standing on a static platform requires the player to stand on the edge of a tile
  if ((player_position.y() + player_.size.y() - 1) % 16 == 0)
  {
    // Player can be on either 1 or 2 tiles, check both (or same...)
    if (get_tile(player_position.x() / 16, (player_position.y() + player_.size.y() - 1) / 16).is_solid_top() ||
        get_tile((player_position.x() + player_.size.x()) / 16, (player_position.y() + player_.size.y() - 1) / 16).is_solid_top())
    {
      return true;
    }
  }

  // Check moving platforms
  for (const auto& platform : level_->moving_platforms)
  {
    // Player only collides if standing exactly on top of the platform, just like with static platforms
    if ((player_position.y() + player_.size.y() - 1 == platform.position.y()) && (player_position.x() < platform.position.x() + 16) &&
        (player_position.x() + player_.size.x() > platform.position.x()))
    {
      return true;
    }
  }

  return false;
}
