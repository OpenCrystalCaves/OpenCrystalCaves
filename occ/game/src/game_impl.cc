#include "game_impl.h"

#include <array>
#include <cstdint>
#include <sstream>

#include "constants.h"
#include "level_loader.h"
#include "logger.h"
#include "misc.h"

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

  // Hurt player when colliding enemy/hazards
  const auto prect = geometry::Rectangle(player_.position, player_.size);
  for (auto&& enemy : level_->enemies)
  {
    if (geometry::isColliding(prect, geometry::Rectangle(enemy->position, enemy->size)))
    {
      player_.hurt();
      break;
    }
  }
  // TODO: also get hurt from hazards
}

int GameImpl::get_bg_sprite(const int x, const int y) const
{
  return level_->get_bg(x, y);
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
      (player_.position.x() < platform.position.x() + SPRITE_W) && (player_.position.x() + player_.size.x() > platform.position.x());

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
      entrance.state = geometry::isColliding({player_.position, player_.size}, {entrance.position, SPRITE_W, SPRITE_H})
        ? EntranceState::OPEN
        : EntranceState::CLOSED;
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
      level_->exit->open = geometry::isColliding({player_.position, player_.size}, {level_->exit->position, SPRITE_W, 2 * SPRITE_H});
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
    if (player_input.jump && !player_.jumping && !player_.falling)
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

  player_.update(*level_);
}

void GameImpl::update_items()
{
  // Check if player hit an item
  // Player can cover at maximum 4 items
  // Check all 4 items, even though we might check the same item multiple times
  const std::array<geometry::Position, 4> positions = {
    geometry::Position((player_.position.x() + 0) / SPRITE_W, (player_.position.y() + 0) / SPRITE_H),
    geometry::Position((player_.position.x() + player_.size.x() - 1) / SPRITE_W, (player_.position.y() + 0) / SPRITE_H),
    geometry::Position((player_.position.x() + 0) / SPRITE_W, (player_.position.y() + player_.size.y() - 1) / SPRITE_H),
    geometry::Position((player_.position.x() + player_.size.x() - 1) / SPRITE_W, (player_.position.y() + player_.size.y() - 1) / SPRITE_H)};

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
        case ItemType::ITEM_TYPE_POWER:
          LOG_DEBUG("Player took item of type power", item.get_type());
          // Last 16 seconds
          player_.power_tick = 16 * FPS / FRAMES_PER_TICK;
          break;
        case ItemType::ITEM_TYPE_EGG:
          LOG_DEBUG("Player took egg", item.get_type());
          score_ += item.get_amount();
          break;
        case ItemType::ITEM_TYPE_KEY:
          LOG_DEBUG("Player took key", item.get_type());
          has_key_ = true;
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

  if (missile_.update(*level_))
  {
    particles_.emplace_back(new Explosion(missile_.position));
  }

  // Player wants to shoot new missile
  if (player_.shooting && !missile_.is_in_cooldown())
  {
    if (num_ammo_ > 0 || player_.godmode || player_.power_tick > 0)
    {
      missile_.init(player_);

      if (!player_.godmode && player_.power_tick == 0)
      {
        num_ammo_ -= 1;
      }
    }
    else
    {
      // Play "no ammo" sound when we have audio
    }
  }

  // Add missile to objects_ if alive
  if (missile_.alive)
  {
    objects_.emplace_back(missile_.position, missile_.get_sprite(), missile_.get_num_sprites(), false);
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
  for (size_t i = 0; i < level_->hazards.size(); i++)
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
