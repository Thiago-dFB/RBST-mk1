#ifndef RBST_GAMESTATE_HPP
#define RBST_GAMESTATE_HPP

//ETL
#include <etl/vector.h>
//-----
#include "Math.hpp"
#include "Config.hpp"
#include "Input.hpp"
#include "Player.hpp"
#include "SecondarySim.hpp"

const size_t MAX_PROJECTILES = 16;

enum RoundPhase
{
	Countdown,
	Play,
	End
};

struct Projectile
{
	Vec2 pos = v2::zero();
	Vec2 vel = v2::zero();
	playerid owner = 0;
	int16 lifetime;
};

struct GameState
{
	long frame = 0;
	int16 roundCountdown = 0;
	RoundPhase phase = Countdown;

	Player p1;
	int16 health1 = 0;
	int16 rounds1 = 0;
	bool p1DmgThisFrame = false;

	Player p2;
	int16 health2 = 0;
	int16 rounds2 = 0;
	bool p2DmgThisFrame = false;

	etl::vector<Projectile, MAX_PROJECTILES> projs;
};

GameState initialState(const Config* cfg)
{
	GameState state;
	state.frame = 0;
	state.roundCountdown = cfg->roundCountdown;
	state.phase = RoundPhase::Countdown;

	respawnPlayer(&(state.p1), cfg, 1);
	state.health1 = cfg->playerHealth;
	state.rounds1 = 0;
	state.p1DmgThisFrame = false;

	respawnPlayer(&(state.p2), cfg, 2);
	state.health2 = cfg->playerHealth;
	state.rounds2 = 0;
	state.p2DmgThisFrame = false;

	return state;
}

void regDamage(GameState* state, playerid damaged)
{
	if (damaged == 1)
	{
		(state->health1)--;
		state->p1DmgThisFrame = true;
	}
	else
	{
		(state->health2)--;
		state->p2DmgThisFrame = true;
	}
}

void altShot(GameState* state, SecSimFlux* flux, const Config* cfg, Vec2 origin, Vec2 direction, playerid owner);

void damagePlayer(Player* player, SecSimFlux* flux, GameState* state, const Config* cfg, Vec2 origin, int8 force)
{
	if (player->pushdown.top() == PState::Charging && player->chargeCount >= cfg->chargeDuration)
	{
		player->pushdown.pop();
		altShot(state, flux, cfg, player->pos, player->dir, player->id);
	}
	//CANCEL CURRENT ACTION
	if (player->pushdown.top() == PState::Dashing || player->pushdown.top() == PState::Charging)
	{
		player->pushdown.pop();
	}
	//APPLY HITSTOP AND FORCE
	player->pushdown.push(PState::Hitstop);
	switch (force)
	{
	case 1:
		player->hitstopCount = cfg->weakHitstop;
		player->vel = v2::add(player->vel, v2::normalizeMult(v2::sub(player->pos, origin), cfg->weakForce));
		break;
	case 2:
		player->hitstopCount = cfg->midHitstop;
		player->vel = v2::add(player->vel, v2::normalizeMult(v2::sub(player->pos, origin), cfg->midForce));
		break;
	case 3:
		player->hitstopCount = cfg->strongHitstop;
		player->vel = v2::add(player->vel, v2::normalizeMult(v2::sub(player->pos, origin), cfg->strongForce));
		break;
	}
}

void altShot(GameState* state, SecSimFlux* flux, const Config* cfg, Vec2 origin, Vec2 direction, playerid owner)
{
	Player* opposition;
	if (owner == 1)	opposition = &(state->p2); else opposition = &(state->p1);
	flux->hitscans.push_back({ false, origin, direction, owner });
	//PARRY
	if (opposition->pushdown.top() == PState::Dashing &&
		opposition->dashCount < cfg->dashPerfect)
	{
		Vec2 dotDist = v2::closest(origin, direction, opposition->perfectPos);
		if (v2::rayWithinRadius(dotDist.x, dotDist.y, cfg->playerRadius))
		{
			//right back at ya
			origin = v2::add(v2::projection(v2::sub(opposition->perfectPos, origin), direction), origin);
			direction = v2::scalarMult(direction, num_det{ -1 });
			owner = opposition->id;
			opposition->pushdown.push(PState::Hitstop);
			opposition->hitstopCount = cfg->midHitstop;
			if (owner == 1)	opposition = &(state->p2); else opposition = &(state->p1);
			//add another hitscan juuuust a bit to the side
			flux->hitscans.push_back({ false, v2::add(origin, v2::scalarMult(v2::rotate(direction, origin.x.half_pi()), num_det{0.01f})), direction, owner});
		}
	}
	//DIRECT HIT OR GRAZE
	Vec2 dotDist = v2::closest(origin, direction, opposition->pos);
	if (!opposition->stunned && (dotDist.x > num_det{ 0 }))
	{
		if (dotDist.y < cfg->playerRadius)
		{
			damagePlayer(opposition, flux, state, cfg, origin, 2);
			if (owner == 1)	regDamage(state, 2); else regDamage(state, 1);
		}
		else if (dotDist.y < cfg->grazeRadius)
		{
			opposition->ammo = cfg->ammoMax;
			opposition->stamina = cfg->staminaMax;
			flux->grazes.push_back({ false, opposition->pos });
		}
	}

	//COMBO
	auto it = state->projs.begin();
	while (it != state->projs.end())
	{
		Vec2 dotDist = v2::closest(origin, direction, it->pos);
		if (v2::rayWithinRadius(dotDist.x, dotDist.y, cfg->projRadius))
		{
			if (!state->p1.stunned && v2::length(v2::sub(it->pos, state->p1.pos)) < (cfg->comboRadius + cfg->playerRadius))
			{
				damagePlayer(&(state->p1), flux, state, cfg, it->pos, 3);
				regDamage(state, 1);
			}
			if (!state->p2.stunned && v2::length(v2::sub(it->pos, state->p2.pos)) < (cfg->comboRadius + cfg->playerRadius))
			{
				damagePlayer(&(state->p2), flux, state, cfg, it->pos, 3);
				regDamage(state, 2);
			}
			flux->combos.push_back({ false,it->pos });
			state->projs.erase(it);
		}
		else
		{
			++it;
		}
	}
}

Vec2 pickDashDir(Vec2 front, MoveInput mov)
{
	const num_det num;
	const num_det quarter_pi = num.pi() / 4;
	switch (mov)
	{
	case MoveInput::ForLeft:
		return v2::rotate(front, -quarter_pi);
	case MoveInput::Left:
		return v2::rotate(front, -num.half_pi());
	case MoveInput::BackLeft:
		return v2::rotate(front, num.pi() + quarter_pi);
	case MoveInput::Back:
		return v2::scalarMult(front, num_det{ -1 });
	case MoveInput::BackRight:
		return v2::rotate(front, num.pi() - quarter_pi);
	case MoveInput::Right:
		return v2::rotate(front, num.half_pi());
	case MoveInput::ForRight:
		return v2::rotate(front, quarter_pi);
	default:
		return front;
	}
}

GameState simulate(GameState state, SecSimFlux* flux, const Config* cfg, InputData input)
{
	state.frame++;
	state.roundCountdown--;
	switch (state.phase)
	{
	case RoundPhase::Countdown:
		if (state.roundCountdown <= 0)
		{
			state.roundCountdown = cfg->roundTime;
			state.phase = RoundPhase::Play;
		}
		break;
	case RoundPhase::End:
		if (state.roundCountdown <= 0)
		{
			if (state.rounds1 < cfg->roundsToWin && state.rounds2 < cfg->roundsToWin)
			{
				state.roundCountdown = cfg->roundCountdown;
				state.phase = RoundPhase::Countdown;
				respawnPlayer(&(state.p1), cfg, state.p1.id);
				state.health1 = cfg->playerHealth;
				respawnPlayer(&(state.p2), cfg, state.p2.id);
				state.health2 = cfg->playerHealth;
				state.projs.clear();
			}
			break;
		}
		// will simulate at half speed
		else if (state.roundCountdown % 2 == 0)
		{
			break;
		}
		input.p1Input = PlayerInput{};
		input.p2Input = PlayerInput{};
	case RoundPhase::Play:
		state.p1DmgThisFrame = false;
		state.p2DmgThisFrame = false;

		//PLAYER 1
		movePlayer(&(state.p1), cfg, input.p1Input);
		state.p1.ammo = std::min(++state.p1.ammo, cfg->ammoMax);
		state.p1.stamina = std::min(++state.p1.stamina, cfg->staminaMax);
		switch (state.p1.pushdown.top())
		{
		case PState::Dashing:
			state.p1.dashCount++;
			if (state.p1.dashCount >= cfg->dashDuration)
			{
				state.p1.pushdown.pop();
			}
			break;
		case PState::Charging:
			state.p1.chargeCount++;
			break;
		case PState::Hitstop:
			state.p1.hitstopCount--;
			if (state.p1.hitstopCount <= 0)
			{
				state.p1.pushdown.pop();
			}
			break;
		}
		//PLAYER 2
		movePlayer(&(state.p2), cfg, input.p2Input);
		state.p2.ammo = std::min(++state.p2.ammo, cfg->ammoMax);
		state.p2.stamina = std::min(++state.p2.stamina, cfg->staminaMax);
		switch (state.p2.pushdown.top())
		{
		case PState::Dashing:
			state.p2.dashCount++;
			if (state.p2.dashCount >= cfg->dashDuration)
			{
				state.p2.pushdown.pop();
			}
			break;
		case PState::Charging:
			state.p2.chargeCount++;
			break;
		case PState::Hitstop:
			state.p2.hitstopCount--;
			if (state.p2.hitstopCount <= 0)
			{
				state.p2.pushdown.pop();
			}
			break;
		}
		//PROJECTILES - MOVE AND CHECK FOR COLLISION OR PARRY
		auto it = state.projs.begin();
		while (it != state.projs.end())
		{
			bool erased = false;
			it->pos = v2::add(it->pos, it->vel);
			it->lifetime++;
			if (v2::length(it->pos) > cfg->arenaRadius)
			{
				flux->projs.push_back({ false,it->pos,it->owner });
				state.projs.erase(it);
				erased = true;
			}
			else
			{
				switch (it->owner)
				{
				case 1:
					if (state.p2.pushdown.top() == PState::Dashing &&
						state.p2.dashCount < cfg->dashPerfect &&
						v2::length(v2::sub(it->pos, state.p2.perfectPos)) < (cfg->playerRadius + cfg->projRadius))
					{
						//ayo a parry just happened, send that projectile back
						it->owner = 2;
						num_det newSpeed = v2::length(it->vel) * cfg->projCounterMultiply;
						it->vel = v2::normalizeMult(v2::sub(state.p1.pos, it->pos), newSpeed);
						state.p2.pushdown.push(PState::Hitstop);
						state.p2.hitstopCount = cfg->weakHitstop;
					}
					else if (!state.p2.stunned && //not stunned
						(state.p2.pushdown.top() != PState::Dashing || state.p2.dashCount < it->lifetime) && //not dashing, or dashing but dash is "younger"
						v2::length(v2::sub(it->pos, state.p2.pos)) < (cfg->playerRadius + cfg->projRadius)) //collision happened
					{
						damagePlayer(&(state.p2), flux, &state, cfg, it->pos, 1);
						regDamage(&state, 2);
						flux->projs.push_back({ false,it->pos,it->owner });
						state.projs.erase(it);
						erased = true;
					}
					break;
				case 2:
					if (state.p1.pushdown.top() == PState::Dashing &&
						state.p1.dashCount < cfg->dashPerfect &&
						v2::length(v2::sub(it->pos, state.p1.perfectPos)) < (cfg->playerRadius + cfg->projRadius))
					{
						//ayo a parry just happened, send that projectile back
						it->owner = 1;
						num_det newSpeed = v2::length(it->vel) * cfg->projCounterMultiply;
						it->vel = v2::normalizeMult(v2::sub(state.p2.pos, it->pos), newSpeed);
						state.p1.pushdown.push(PState::Hitstop);
						state.p1.hitstopCount = cfg->weakHitstop;
					}
					else if (
						!state.p1.stunned && //not stunned
						(state.p1.pushdown.top() != PState::Dashing || state.p1.dashCount < it->lifetime) && //not dashing, or dashing but dash is "younger"
						v2::length(v2::sub(it->pos, state.p1.pos)) < (cfg->playerRadius + cfg->projRadius)) //collision happened
					{
						damagePlayer(&(state.p1), flux, &state, cfg, it->pos, 1);
						regDamage(&state, 1);
						flux->projs.push_back({ false,it->pos,it->owner });
						state.projs.erase(it);
						erased = true;
					}
					break;
				}
			}
			if (!erased) ++it;
		}
		//DASHING
		bool directColl = v2::length(v2::sub(state.p1.pos, state.p2.pos)) < (cfg->playerRadius + cfg->playerRadius);
		//BOTH DASHING IN THIS FRAME
		if (state.p1.pushdown.top() == PState::Dashing && state.p2.pushdown.top() == PState::Dashing)
		{
			//DIRECT HIT, MOST RECENT DASH LOSES
			if (directColl)
			{
				int dashDiff = state.p1.dashCount - state.p2.dashCount;
				if (dashDiff > 0)
				{
					damagePlayer(&(state.p2), flux, &state, cfg, state.p1.pos, 2);
					regDamage(&state, 2);
					state.p1.pushdown.push(PState::Hitstop);
					state.p1.hitstopCount = cfg->midHitstop;
				}
				else if (dashDiff < 0)
				{
					damagePlayer(&(state.p1), flux, &state, cfg, state.p2.pos, 2);
					regDamage(&state, 1);
					state.p2.pushdown.push(PState::Hitstop);
					state.p2.hitstopCount = cfg->midHitstop;
				}
				else
				{
					damagePlayer(&(state.p1), flux, &state, cfg, state.p2.pos, 2);
					regDamage(&state, 1);
					damagePlayer(&(state.p2), flux, &state, cfg, state.p1.pos, 2);
					regDamage(&state, 2);
				}
			}
			//P2 PERFECT EVADES
			else if (state.p2.dashCount < cfg->dashPerfect && (v2::length(v2::sub(state.p1.pos, state.p2.perfectPos))) < (cfg->playerRadius + cfg->playerRadius))
			{
				damagePlayer(&(state.p1), flux, &state, cfg, state.p2.perfectPos, 2);
				regDamage(&state, 1);
				state.p2.pushdown.push(PState::Hitstop);
				state.p2.hitstopCount = cfg->midHitstop;
			}
			//P1 PERFECT EVADES
			else if (state.p1.dashCount < cfg->dashPerfect && (v2::length(v2::sub(state.p2.pos, state.p1.perfectPos))) < (cfg->playerRadius + cfg->playerRadius))
			{
				damagePlayer(&(state.p2), flux, &state, cfg, state.p1.perfectPos, 2);
				regDamage(&state, 2);
				state.p1.pushdown.push(PState::Hitstop);
				state.p1.hitstopCount = cfg->midHitstop;
			}
		}
		//P1 HITS P2
		else if (directColl && !state.p2.stunned && state.p1.pushdown.top() == PState::Dashing)
		{
			damagePlayer(&(state.p2), flux, &state, cfg, state.p1.pos, 2);
			regDamage(&state, 2);
			state.p1.pushdown.push(PState::Hitstop);
			state.p1.hitstopCount = cfg->midHitstop;
		}
		//P2 HITS P1
		else if (directColl && !state.p1.stunned && state.p2.pushdown.top() == PState::Dashing)
		{
			damagePlayer(&(state.p1), flux, &state, cfg, state.p2.pos, 2);
			regDamage(&state, 1);
			state.p2.pushdown.push(PState::Hitstop);
			state.p2.hitstopCount = cfg->midHitstop;
		}

		//ALT SHOT
		if (state.p1.pushdown.top() == PState::Charging && state.p1.chargeCount >= cfg->chargeDuration)
		{
			state.p1.pushdown.pop();
			altShot(&state, flux, cfg, state.p1.pos, state.p1.dir, 1);
		}
		if (state.p2.pushdown.top() == PState::Charging && state.p2.chargeCount >= cfg->chargeDuration)
		{
			state.p2.pushdown.pop();
			altShot(&state, flux, cfg, state.p2.pos, state.p2.dir, 2);
		}

		state.p1.stunned = state.p1.stunned || state.p1DmgThisFrame;
		state.p2.stunned = state.p2.stunned || state.p2DmgThisFrame;

		//PLAYER 1 ATTACKS
		if (state.p1.stunned)
		{
			//alert: break out of stun at the cost all your stamina
			if (state.p1.pushdown.top() != PState::Hitstop && //can't alert out of hitstop
				input.p1Input.atk == AttackInput::Dash &&
				!state.p1DmgThisFrame && //can't alert out of the same frame you were damaged
				state.p1.stamina >= cfg->dashCost) //need to have enough stamina for a dash
			{
				state.p1.dashCount = 0;
				state.p1.dashVel = v2::scalarMult(pickDashDir(state.p1.dir, input.p1Input.mov), cfg->playerDashSpeed);
				state.p1.perfectPos = state.p1.pos;
				state.p1.pushdown.push(PState::Dashing);
				state.p1.stamina = 0;
				state.p1.stunned = false;
				flux->alerts.push_back({false, state.p1.pos});
			}
			//cancel your next move nevertheless
			input.p1Input.atk = AttackInput::None;
		}
		if (state.p1.pushdown.top() == PState::Default)
		{
			switch (input.p1Input.atk)
			{
			case AttackInput::Dash:
				if (state.p1.stamina < cfg->dashCost) break;
				state.p1.dashCount = 0;
				state.p1.dashVel = v2::scalarMult(pickDashDir(state.p1.dir, input.p1Input.mov), cfg->playerDashSpeed);
				state.p1.perfectPos = state.p1.pos;
				state.p1.pushdown.push(PState::Dashing);
				state.p1.stamina = state.p1.stamina - cfg->dashCost;
				break;
			case AttackInput::Shot:
				if (state.p1.ammo < cfg->shotCost) break;
				state.projs.push_back({ state.p1.pos, v2::scalarMult(state.p1.dir, cfg->projSpeed), 1, 0 });
				state.p1.ammo = state.p1.ammo - cfg->shotCost;
				break;
			case AttackInput::AltShot:
				if (state.p1.ammo < cfg->altShotCost) break;
				state.p1.chargeCount = 0;
				state.p1.pushdown.push(PState::Charging);
				state.p1.ammo = state.p1.ammo - cfg->altShotCost;
				break;
			}
		}
		//WEAVE A DASH INTO ANOTHER
		else if (state.p1.pushdown.top() == PState::Dashing &&
			input.p1Input.atk == AttackInput::Dash &&
			state.p1.stamina >= cfg->dashCost)
		{
			state.p1.dashCount = 0;
			state.p1.dashVel = v2::scalarMult(pickDashDir(state.p1.dir, input.p1Input.mov), cfg->playerDashSpeed);
			state.p1.perfectPos = state.p1.pos;
			state.p1.stamina = state.p1.stamina - cfg->dashCost;
		}

		//PLAYER 2 ATTACKS
		if (state.p2.stunned)
		{
			//alert: break out of stun at the cost all your stamina
			if (state.p2.pushdown.top() != PState::Hitstop && //can't alert out of hitstop
				input.p2Input.atk == AttackInput::Dash &&
				!state.p2DmgThisFrame && //can't alert out of the same frame you were damaged
				state.p2.stamina >= cfg->dashCost) //need to have enough stamina for a dash
			{
				state.p2.dashCount = 0;
				state.p2.dashVel = v2::scalarMult(pickDashDir(state.p2.dir, input.p2Input.mov), cfg->playerDashSpeed);
				state.p2.perfectPos = state.p2.pos;
				state.p2.pushdown.push(PState::Dashing);
				state.p2.stamina = 0;
				state.p2.stunned = false;
				flux->alerts.push_back({ false, state.p2.pos });
			}
			//cancel your next move nevertheless
			input.p2Input.atk = AttackInput::None;
		}
		if (state.p2.pushdown.top() == PState::Default)
		{
			switch (input.p2Input.atk)
			{
			case AttackInput::Dash:
				if (state.p2.stamina < cfg->dashCost) break;
				state.p2.dashCount = 0;
				state.p2.dashVel = v2::scalarMult(pickDashDir(state.p2.dir, input.p2Input.mov), cfg->playerDashSpeed);
				state.p2.perfectPos = state.p2.pos;
				state.p2.pushdown.push(PState::Dashing);
				state.p2.stamina = state.p2.stamina - cfg->dashCost;
				break;
			case AttackInput::Shot:
				if (state.p2.ammo < cfg->shotCost) break;
				state.projs.push_back({ state.p2.pos, v2::scalarMult(state.p2.dir, cfg->projSpeed), 2, 0 });
				state.p2.ammo = state.p2.ammo - cfg->shotCost;
				break;
			case AttackInput::AltShot:
				if (state.p2.ammo < cfg->altShotCost) break;
				state.p2.chargeCount = 0;
				state.p2.pushdown.push(PState::Charging);
				state.p2.ammo = state.p2.ammo - cfg->altShotCost;
				break;
			}
		}
		//WEAVE A DASH INTO ANOTHER
		else if (state.p2.pushdown.top() == PState::Dashing &&
			input.p2Input.atk == AttackInput::Dash &&
			state.p2.stamina >= cfg->dashCost)
		{
			state.p2.dashCount = 0;
			state.p2.dashVel = v2::scalarMult(pickDashDir(state.p2.dir, input.p2Input.mov), cfg->playerDashSpeed);
			state.p2.perfectPos = state.p2.pos;
			state.p2.stamina = state.p2.stamina - cfg->dashCost;
		}

		//ROUND END
		if (state.phase == RoundPhase::Play && (state.roundCountdown <= 0 || state.health1 <= 0 || state.health2 <= 0))
		{
			if (state.health1 > state.health2)
			{
				state.rounds1++;
			}
			else if (state.health2 > state.health1)
			{
				state.rounds2++;
			}
			state.roundCountdown = cfg->roundEndTime;
			state.phase = RoundPhase::End;
		}

		break;
	}
	return state;
}

bool endCondition(const GameState* state, const Config* cfg)
{
	bool phaseIsEnd = state->phase == End;
	bool countdownEnded = state->roundCountdown <= 0;
	bool aPlayerWon = state->rounds1 >= cfg->roundsToWin || state->rounds2 >= cfg->roundsToWin;
	return phaseIsEnd && countdownEnded && aPlayerWon;
}

#endif