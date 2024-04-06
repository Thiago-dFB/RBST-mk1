#ifndef RBST_PLAYER_HPP
#define RBST_PLAYER_HPP

//ETL
#include <etl/stack.h>
//-----
#include "Math.hpp"
#include "Config.hpp"
#include "Input.hpp"

using playerid = std::uint8_t;

enum PState
{
	Standby,
	Default,
	Charging,
	Dashing,
	Hitstop
};

struct Player
{
	playerid id = 0;
	etl::stack<PState, 4> pushdown;
	//xform
	Vec2 pos = v2::zero();
	Vec2 vel = v2::zero();
	Vec2 dir = v2::zero();
	//shots
	int16 ammo = 0;
	int16 chargeCount = 0;
	//dash
	int16 stamina = 0;
	Vec2 perfectPos = v2::zero();
	Vec2 dashVel = v2::zero();
	int16 dashCount = 0;
	//other countdowns
	int16 hitstopCount = 0;
	bool stunned = false;
};

void respawnPlayer(Player* player, const Config* cfg, playerid id)
{
	player->id = id;
	switch (id)
	{
	case 1:
		player->pos = v2::scalarMult(v2::left(), cfg->spawnRadius);
		player->dir = v2::right();
		break;
	case 2:
		player->pos = v2::scalarMult(v2::right(), cfg->spawnRadius);
		player->dir = v2::left();
		break;
	}
	player->vel = v2::zero();
	player->ammo = cfg->ammoMax;
	player->chargeCount = 0;
	player->stamina = cfg->staminaMax;
	player->perfectPos = v2::zero();
	player->dashVel = v2::zero();
	player->dashCount = 0;
	player->hitstopCount = 0;
	player->stunned = false;

	while (!player->pushdown.empty())
	{
		player->pushdown.pop();
	}
	player->pushdown.push(PState::Default);
}

void movePlayer(Player* player, const Config* cfg, PlayerInput input)
{
	num_det speed = v2::length(player->vel);
	Vec2 impulse = v2::scalarMult(player->dir, cfg->playerWalkAccel);
	num_det quarter_pi = speed.pi() / 4;
	switch (player->pushdown.top())
	{
	case PState::Standby:
		input.mov = MoveInput::Neutral;
		input.mouse = num_det{ 0 };
		//no break, fall through to default
	case PState::Default:
		//MOUSE MOVEMENT
		player->dir = v2::normalize(v2::rotate(player->dir, input.mouse));

		if (player->stunned) input.mov = MoveInput::Neutral;

		//NORMAL WALKING MOVEMENT - ACCELERATION
		
		switch (input.mov)
		{
		case MoveInput::Neutral:
			if (speed < cfg->playerWalkFric)
			{
				player->vel = v2::zero();
				impulse = v2::zero();
				//player naturally breaks out of stun when fully stopped
				player->stunned = false;
			}
			else
			{
				//every opposite vector gets normalized to friction
				impulse = v2::scalarMult(player->vel, num_det{ -1 });
			}
			break;
		case MoveInput::ForLeft:
			impulse = v2::rotate(impulse, -quarter_pi);
			break;
		case MoveInput::Left:
			impulse = v2::rotate(impulse, -speed.half_pi());
			break;
		case MoveInput::BackLeft:
			impulse = v2::rotate(impulse, speed.pi() + quarter_pi);
			break;
		case MoveInput::Back:
			impulse = v2::scalarMult(impulse, num_det{ -1 });
			break;
		case MoveInput::BackRight:
			impulse = v2::rotate(impulse, speed.pi() - quarter_pi);
			break;
		case MoveInput::Right:
			impulse = v2::rotate(impulse, speed.half_pi());
			break;
		case MoveInput::ForRight:
			impulse = v2::rotate(impulse, quarter_pi);
			break;
		}
		//if player is backpedaling, turn directly opposite force into friction
		if (v2::dot(player->vel, impulse) < num_det{ 0 })
		{
			impulse = v2::rejection(impulse, player->vel);
			impulse = v2::add(impulse, v2::normalizeMult(player->vel, -cfg->playerWalkFric));
		}
		player->vel = v2::add(player->vel, impulse);
		if (v2::length(player->vel) > cfg->playerWalkSpeed)
		{
			player->vel = v2::normalizeMult(player->vel, cfg->playerWalkSpeed);
		}

		//NORMAL WALKING MOVEMENT - DISPLACEMENT
		player->pos = v2::add(player->pos, player->vel);
		break;
	case PState::Dashing:
		player->dir = v2::rotate(player->dir, input.mouse);

		if (player->dashCount < cfg->dashPhase)
		{
			num_det alpha = num_det{ player->dashCount } / num_det{ cfg->dashPhase };
			Vec2 displace = v2::lerp(player->vel, player->dashVel, alpha);
			player->pos = v2::add(player->pos, displace);
		}
		else
		{
			player->pos = v2::add(player->pos, player->dashVel);
			player->vel = player->dashVel;
		}
		break;
	}

	//CORRECT TO WITHIN ARENA
	if (v2::length(player->pos) > cfg->arenaRadius)
	{
		player->pos = v2::normalizeMult(player->pos, cfg->arenaRadius);
		player->vel = v2::rejection(player->vel, player->pos);
	}
}

#endif