#ifndef RBST_CONFIG_HPP
#define RBST_CONFIG_HPP

//fpm
#include <fpm/ios.hpp>
//TOML++
#include <toml++/toml.h>
//-----
#include "Math.hpp"

struct Config
{
	//GameRules
	int16 playerHealth = 0;
	int16 roundsToWin = 0;
	int16 roundCountdown = 0;
	int16 roundTime = 0;
	int16 roundEndTime = 0;
	//PlayerResources
	int16 ammoMax = 0;
	int16 shotCost = 0;
	int16 altShotCost = 0;
	int16 staminaMax = 0;
	int16 dashCost = 0;
	//PlayerAction
	int16 dashDuration = 0;
	int16 dashPhase = 0;
	int16 dashPerfect = 0;
	int16 chargeDuration = 0;
	//PlayerSpeed
	num_det playerWalkSpeed{ 0 };
	num_det playerWalkAccel{ 0 };
	num_det playerWalkFric{ 0 };
	num_det playerDashSpeed{ 0 };
	//ProjSpeed
	num_det projSpeed{ 0 };
	num_det projCounterMultiply{ 0 };
	//Radius
	num_det playerRadius{ 0 };
	num_det grazeRadius{ 0 };
	num_det projRadius{ 0 };
	num_det comboRadius{ 0 };
	num_det arenaRadius{ 0 };
	num_det spawnRadius{ 0 };
	//HitStrength
	num_det weakForce{ 0 };
	int16 weakHitstop = 0;
	num_det midForce{ 0 };
	int16 midHitstop = 0;
	num_det strongForce{ 0 };
	int16 strongHitstop = 0;
};

Config readTOMLForCfg()
{
	Config cfg;
	auto file = toml::parse_file("RBST_config.toml");
	std::istringstream iss;
	num_det extract;

	cfg.playerHealth = file["GameRules"]["playerHealth"].value_or(5);
	cfg.roundsToWin = file["GameRules"]["roundsToWin"].value_or(2);
	cfg.roundCountdown = file["GameRules"]["roundCountdown"].value_or(3) * 60;
	cfg.roundTime = file["GameRules"]["roundTime"].value_or(100) * 60;
	cfg.roundEndTime = file["GameRules"]["roundEndTime"].value_or(3) * 60;

	cfg.ammoMax = file["PlayerResources"]["ammoMax"].value_or(120);
	cfg.shotCost = file["PlayerResources"]["shotCost"].value_or(60);
	cfg.altShotCost = file["PlayerResources"]["railCost"].value_or(120);
	cfg.staminaMax = file["PlayerResources"]["staminaMax"].value_or(150);
	cfg.dashCost = file["PlayerResources"]["dashCost"].value_or(50);

	cfg.dashPhase = file["PlayerActions"]["dashPhase"].value_or(10);
	cfg.dashPerfect = file["PlayerActions"]["dashPerfect"].value_or(15);
	cfg.dashDuration = file["PlayerActions"]["dashDuration"].value_or(60);
	cfg.chargeDuration = file["PlayerActions"]["chargeDuration"].value_or(60);

	iss.str(file["PlayerSpeed"]["playerWalkSpeed"].value_or("0.8"));
	iss >> extract;
	cfg.playerWalkSpeed = (extract / num_det{ 60 });
	iss.str(file["PlayerSpeed"]["playerWalkAccel"].value_or("0.4"));
	iss.seekg(0, iss.beg);
	iss >> extract;
	cfg.playerWalkAccel = extract / num_det{ 1800 };
	iss.str(file["PlayerSpeed"]["playerWalkFric"].value_or("0.4"));
	iss.seekg(0, iss.beg);
	iss >> extract;
	cfg.playerWalkFric = extract / num_det{ 1800 };
	iss.str(file["PlayerSpeed"]["playerDashSpeed"].value_or("1.2"));
	iss.seekg(0, iss.beg);
	iss >> extract;
	cfg.playerDashSpeed = (extract / num_det{ 60 });

	iss.str(file["Projectiles"]["projSpeed"].value_or("0.6"));
	iss.seekg(0, iss.beg);
	iss >> extract;
	cfg.projSpeed = extract / num_det{ 60 };
	iss.str(file["Projectiles"]["projCounterMultiply"].value_or("1.2"));
	iss.seekg(0, iss.beg);
	iss >> extract;
	cfg.projCounterMultiply = extract;

	iss.str(file["Radius"]["playerRadius"].value_or("0.5"));
	iss.seekg(0, iss.beg);
	iss >> extract;
	cfg.playerRadius = extract;
	iss.str(file["Radius"]["grazeRadius"].value_or("0.7"));
	iss.seekg(0, iss.beg);
	iss >> extract;
	cfg.grazeRadius = extract;
	iss.str(file["Radius"]["projRadius"].value_or("0.3"));
	iss.seekg(0, iss.beg);
	iss >> extract;
	cfg.projRadius = extract;
	iss.str(file["Radius"]["comboRadius"].value_or("3.0"));
	iss.seekg(0, iss.beg);
	iss >> extract;
	cfg.comboRadius = extract;
	iss.str(file["Radius"]["arenaRadius"].value_or("12.0"));
	iss.seekg(0, iss.beg);
	iss >> extract;
	cfg.arenaRadius = extract;
	iss.str(file["Radius"]["spawnRadius"].value_or("10.0"));
	iss.seekg(0, iss.beg);
	iss >> extract;
	cfg.spawnRadius = extract;

	iss.str(file["HitStrength"]["weakForce"].value_or("1.0"));
	iss.seekg(0, iss.beg);
	iss >> extract;
	cfg.weakForce = extract / num_det{ 60 };
	cfg.weakHitstop = file["HitStrength"]["weakHitstop"].value_or(5);

	iss.str(file["HitStrength"]["midForce"].value_or("2.5"));
	iss.seekg(0, iss.beg);
	iss >> extract;
	cfg.midForce = extract / num_det{ 60 };
	cfg.midHitstop = file["HitStrength"]["midHitstop"].value_or(10);

	iss.str(file["HitStrength"]["strongForce"].value_or("4.0"));
	iss.seekg(0, iss.beg);
	iss >> extract;
	cfg.strongForce = extract / num_det{ 60 };
	cfg.strongHitstop = file["HitStrength"]["strongHitstop"].value_or(20);

	return cfg;
};

#endif