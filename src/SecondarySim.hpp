//THIS IS THE ONE PIECE OF CODE WHERE I AM GOING TO TOTALLY WING IT AND IT IS GOING TO TURN OUT VERY UGLY
//BUT HEY IF IT WORKS IT WORKS, AND SOMEONE WITH BETTER KNOWLEDGE OF DATA STRUCTURES CAN SURELY DO BETTER

#ifndef RBST_SECONDARYSIM_HPP
#define RBST_SECONDARYSIM_HPP

#include <iostream>
//std
#include <array>
#include <map>
#include <vector>
//raylib
#include <raylib.h>
//-----
#include "Math.hpp"

const float POSITION_FORGIVENESS = 0.5f;

struct BasicFlux
{
	bool associated; Vec2 pos;
};
struct SidedFlux
{
	bool associated; Vec2 pos; std::uint8_t owner;
};
struct HitscanFlux
{
	bool associated; Vec2 pos; Vec2 dir; std::uint8_t owner;
};

struct SecSimFlux
{
	std::vector<SidedFlux> projs;
	std::vector<BasicFlux> combos;
	std::vector<BasicFlux> grazes;
	std::vector<BasicFlux> alerts;
	std::vector<HitscanFlux> hitscans;
};

using SecSimFluxHistory = std::map<long, SecSimFlux>;

struct BasicParticle
{
	long frame; int lifetime; Vec2 pos;
};
struct SidedParticle
{
	long frame; int lifetime; Vec2 pos; std::uint8_t owner;
};
struct HitscanParticle
{
	long frame; int lifetime; Vec2 pos; Vec2 dir; std::uint8_t owner;
};
struct EffectParticle
{
	long frame; int lifetime; Vec2 pos; std::array<Vector4,10> subParts;
};

std::array<Vector4, 10> createSubParts()
{
	std::array<Vector4, 10> subParts;
	for (int i = 0; i < 10; i++)
	{
		float angle = GetRandomValue(0, 359);
		float height = GetRandomValue(5, 15) / 20.0f;
		float size = GetRandomValue(8, 12) / 10.0f;
		subParts[i] = Vector4{
			cos(angle),sin(angle),height,size
		};
	}
	return subParts;
}

struct SecSimParticles
{
	std::vector<SidedParticle> projs;
	std::vector<BasicParticle> combos;
	std::vector<EffectParticle> grazes;
	std::vector<EffectParticle> alerts;
	std::vector<HitscanParticle> hitscans;
};

void currentFrameSecSim(SecSimFlux* flux, SecSimParticles* particles, long currFrame);

//Call this after the last frame of a rollback, but not after simulating the current frame
void rollbackSecSim(SecSimFluxHistory* fluxHist, SecSimParticles* particles, long rollbackFrame)
{
	//Compare particles to flux
	//PROJECTILE COLLISIONS
	//can have slight variations in frame and position if collided with player
	for (int it = 0; it < particles->projs.size();)
	{
		//particle is within flux
		if (particles->projs.at(it).frame > rollbackFrame)
		{
			bool exists = false;
			std::vector<SidedFlux>::iterator fluxIt;
			//check frames from history
			for (auto histIt = fluxHist->begin(); histIt != fluxHist->end(); histIt++)
			{
				//only the ones within flux
				if (histIt->first <= rollbackFrame) continue;
				fluxIt = histIt->second.projs.begin();
				for (; fluxIt != histIt->second.projs.end(); fluxIt++)
				{
					//don't compare with an already associated flux
					if (fluxIt->associated) continue;
					exists = (fpm::abs(particles->projs.at(it).pos.x - fluxIt->pos.x) + fpm::abs(particles->projs.at(it).pos.y - fluxIt->pos.y)) < num_det{ POSITION_FORGIVENESS };
					if (exists)
					{
						fluxIt->associated = true;
						particles->projs.at(it).pos = fluxIt->pos;
						particles->projs.at(it).frame = histIt->first;
						break;
					}
				}
				//break out of wider loop
				if (exists) break;
			}
			if (!exists)
			{
				particles->projs.erase(particles->projs.begin() + it);
				continue;
			}
		}

		it++;
	}
	//COMBOS
	//if it did not happen in that frame at that position, it did not happen
	for (int it = 0; it < particles->combos.size();)
	{
		//particle is within flux
		if (particles->combos.at(it).frame > rollbackFrame)
		{
			bool exists = false;
			auto fluxIt = fluxHist->at(particles->combos.at(it).frame).combos.begin();
			for (; fluxIt != fluxHist->at(particles->combos.at(it).frame).combos.end(); fluxIt++)
			{
				//don't compare with an already associated flux
				if (fluxIt->associated) continue;
				exists = v2::equal(particles->combos.at(it).pos, fluxIt->pos);
				if (exists)
				{
					fluxIt->associated = true;
					break;
				}
			}
			if (!exists)
			{
				particles->combos.erase(particles->combos.begin() + it);
				continue;
			}
		}

		it++;
	}
	//GRAZES
	//if it did not happen in that frame and NEAR that position, it did not happen
	for (int it = 0; it < particles->grazes.size();)
	{
		//particle is within flux
		if (particles->grazes.at(it).frame > rollbackFrame)
		{
			long exactFrame = particles->grazes.at(it).frame;
			bool exists = false;
			auto fluxIt = fluxHist->at(exactFrame).grazes.begin();
			for (; fluxIt != fluxHist->at(exactFrame).grazes.end(); fluxIt++)
			{
				//don't compare with an already associated flux
				if (fluxIt->associated) continue;
				exists = (fpm::abs(particles->grazes.at(it).pos.x - fluxIt->pos.x) + fpm::abs(particles->grazes.at(it).pos.y - fluxIt->pos.y)) < num_det{ POSITION_FORGIVENESS };
				if (exists)
				{
					fluxIt->associated = true;
					particles->grazes.at(it).pos = fluxIt->pos;
					break;
				}
			}
			if (!exists)
			{
				particles->grazes.erase(particles->grazes.begin() + it);
				continue;
			}
		}

		it++;
	}
	//ALERTS
	//can have slight variations in frame and position depending on when player inputted
	for (int it = 0; it < particles->alerts.size();)
	{
		//particle is within flux
		if (particles->alerts.at(it).frame > rollbackFrame)
		{
			bool exists = false;
			std::vector<BasicFlux>::iterator fluxIt;
			//check frames from history
			for (auto histIt = fluxHist->begin(); histIt != fluxHist->end(); histIt++)
			{
				//only the ones within flux
				if (histIt->first <= rollbackFrame) continue;
				fluxIt = histIt->second.alerts.begin();
				for (; fluxIt != histIt->second.alerts.end(); fluxIt++)
				{
					//don't compare with an already associated flux
					if (fluxIt->associated) continue;
					exists = (fpm::abs(particles->alerts.at(it).pos.x - fluxIt->pos.x) + fpm::abs(particles->alerts.at(it).pos.y - fluxIt->pos.y)) < num_det{ POSITION_FORGIVENESS };
					if (exists)
					{
						fluxIt->associated = true;
						particles->alerts.at(it).pos = fluxIt->pos;
						particles->alerts.at(it).frame = histIt->first;
						break;
					}
				}
				//break out of wider loop
				if (exists) break;
			}
			if (!exists)
			{
				particles->alerts.erase(particles->alerts.begin() + it);
				continue;
			}
		}

		it++;
	}
	//HITSCANS
	//if it did not happen in that frame at that position, it did not happen
	for (int it = 0; it < particles->hitscans.size();)
	{
		//particle is within flux
		if (particles->hitscans.at(it).frame > rollbackFrame)
		{
			bool exists = false;
			auto fluxIt = fluxHist->at(particles->hitscans.at(it).frame).hitscans.begin();
			for (; fluxIt != fluxHist->at(particles->hitscans.at(it).frame).hitscans.end(); fluxIt++)
			{
				//don't compare with an already associated flux
				if (fluxIt->associated) continue;
				exists = v2::equal(particles->hitscans.at(it).pos, fluxIt->pos);
				if (exists)
				{
					fluxIt->associated = true;
					break;
				}
			}
			if (!exists)
			{
				particles->hitscans.erase(particles->hitscans.begin() + it);
				continue;
			}
		}

		it++;
	}

	//Add particles of non-associated flux
	for (auto it = fluxHist->begin(); it != fluxHist->end(); it++)
	{
		if (it->first <= rollbackFrame) continue;
		currentFrameSecSim(&it->second, particles, it->first);
	}
}

//can't do this while rolling back because more than one rollback can happen within a frame
//also this function is necessary for demo playback
void increaseParticleLifetime(SecSimParticles* particles)
{
	//in case you're thinking iterating like this is weird C++ forced my hand (aka things go wrong and I don't know why)
	for (int it = 0; it < particles->projs.size();)
	{
		particles->projs.at(it).lifetime++;
		if (particles->projs.at(it).lifetime > 60)
			particles->projs.erase(particles->projs.begin() + it);
		else
			it++;
	}
	for (int it = 0; it < particles->combos.size();)
	{
		particles->combos.at(it).lifetime++;
		if (particles->combos.at(it).lifetime > 60)
			particles->combos.erase(particles->combos.begin() + it);
		else
			it++;
	}
	for (int it = 0; it < particles->grazes.size();)
	{
		particles->grazes.at(it).lifetime++;
		if (particles->grazes.at(it).lifetime > 60)
			particles->grazes.erase(particles->grazes.begin() + it);
		else
			it++;
	}
	for (int it = 0; it < particles->alerts.size();)
	{
		particles->alerts.at(it).lifetime++;
		if (particles->alerts.at(it).lifetime > 60)
			particles->alerts.erase(particles->alerts.begin() + it);
		else
			it++;
	}
	for (int it = 0; it < particles->hitscans.size();)
	{
		particles->hitscans.at(it).lifetime++;
		if (particles->hitscans.at(it).lifetime > 60)
			particles->hitscans.erase(particles->hitscans.begin() + it);
		else
			it++;
	}
}

void currentFrameSecSim(SecSimFlux* flux, SecSimParticles* particles, long currFrame)
{
	for (auto fluxIt = flux->projs.begin(); fluxIt != flux->projs.end(); fluxIt++)
	{
		if (!fluxIt->associated)
		{
			SidedParticle part = { currFrame, 0, fluxIt->pos, fluxIt->owner };
			particles->projs.push_back(part);
			fluxIt->associated = true;
		}
	}
	for (auto fluxIt = flux->combos.begin(); fluxIt != flux->combos.end(); fluxIt++)
	{
		if (!fluxIt->associated)
		{
			BasicParticle part = { currFrame, 0, fluxIt->pos };
			particles->combos.push_back(part);
			fluxIt->associated = true;
		}
	}
	for (auto fluxIt = flux->grazes.begin(); fluxIt != flux->grazes.end(); fluxIt++)
	{
		if (!fluxIt->associated)
		{
			EffectParticle part = { currFrame, 0, fluxIt->pos, createSubParts() };
			particles->grazes.push_back(part);
			fluxIt->associated = true;
		}
	}
	for (auto fluxIt = flux->alerts.begin(); fluxIt != flux->alerts.end(); fluxIt++)
	{
		if (!fluxIt->associated)
		{
			EffectParticle part = { currFrame, 0, fluxIt->pos, createSubParts() };
			particles->alerts.push_back(part);
			fluxIt->associated = true;
		}
	}
	for (auto fluxIt = flux->hitscans.begin(); fluxIt != flux->hitscans.end(); fluxIt++)
	{
		if (!fluxIt->associated)
		{
			HitscanParticle part = { currFrame, 0, fluxIt->pos, fluxIt->dir, fluxIt->owner };
			particles->hitscans.push_back(part);
			fluxIt->associated = true;
		}
	}
}

#endif