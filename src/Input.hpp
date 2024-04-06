#ifndef RBST_INPUT_HPP
#define RBST_INPUT_HPP

//Raylib
#include <raylib.h>
//TOML++
#include <toml++/toml.h>
//-----
#include "Math.hpp"

enum AttackInput
{
	None = 0,
	Shot = 1,
	AltShot = 2,
	Dash = 3
};

enum MoveInput
{
	BackLeft = 1,
	Back = 2,
	BackRight = 3,
	Left = 4,
	Neutral = 5,
	Right = 6,
	ForLeft = 7,
	Forward = 8,
	ForRight = 9
};

struct PlayerInput
{
	AttackInput atk = None;
	MoveInput mov = Neutral;
	//horizontal mouse movement (already in radians)
	num_det mouse{ 0 };
};

struct PlayerInputZip
{
	char movAtk = 0;
	int32_t mouseRaw = 0;
};

struct InputData
{
	PlayerInput p1Input;
	PlayerInput p2Input;
};

struct InputBindings
{
	num_det sensitivity{ 0 };
	int forward = 0;
	int back = 0;
	int left = 0;
	int right = 0;
	int shotKey = 0;
	int shotBtn = 6;
	int altShotKey = 0;
	int altShotBtn = 6;
	int dashKey = 0;
	int dashBtn = 6;
};

InputBindings readTOMLForBind()
{
	IsKeyPressed(KEY_A);
	
	InputBindings inputBind;
	auto file = toml::parse_file("RBST_controls.toml");
	std::istringstream iss;

	iss.str(file["Mouse"]["sensitivity"].value_or("1"));
	iss >> inputBind.sensitivity;
	
	inputBind.forward = file["Direction"]["forward"].value_or(0);
	inputBind.back = file["Direction"]["back"].value_or(0);
	inputBind.left = file["Direction"]["left"].value_or(0);
	inputBind.right = file["Direction"]["right"].value_or(0);

	inputBind.shotKey = file["Attack"]["fireKey"].value_or(0);
	inputBind.shotBtn = file["Attack"]["fireBtn"].value_or(6);
	inputBind.altShotKey = file["Attack"]["altFireKey"].value_or(0);
	inputBind.altShotBtn = file["Attack"]["altFireBtn"].value_or(6);
	inputBind.dashKey = file["Attack"]["dashKey"].value_or(0);
	inputBind.dashBtn = file["Attack"]["dashBtn"].value_or(6);

	return inputBind;
}

const InputBindings inputBind = readTOMLForBind();

//INPUT PROCESSING AND CONVERSION

PlayerInput processInput(const InputBindings* inputBind)
{
	if (IsWindowFocused())
	{
		PlayerInput input;

		input.mouse = inputBind->sensitivity * num_det{ DEG2RAD * GetMouseDelta().x };

		int8 direction = 5;
		if (IsKeyDown(inputBind->forward)) direction = direction + 3;
		if (IsKeyDown(inputBind->back)) direction = direction - 3;
		if (IsKeyDown(inputBind->left)) --direction;
		if (IsKeyDown(inputBind->right)) ++direction;
		input.mov = static_cast<MoveInput>(direction);

		if (IsMouseButtonPressed(inputBind->shotBtn) || IsKeyPressed(inputBind->shotKey)) input.atk = Shot;
		if (IsMouseButtonPressed(inputBind->altShotBtn) || IsKeyPressed(inputBind->altShotKey)) input.atk = AltShot;
		if (IsKeyPressed(inputBind->dashKey) || IsMouseButtonPressed(inputBind->dashBtn)) input.atk = Dash;

		return input;
	}
	else
	{
		return PlayerInput{ None, Neutral, num_det{0} };
	}
}

//GGPO does some weird shit with existing input to roll predictions
//if I knew how to override it with this I would
PlayerInput predictInput(PlayerInput prevInput)
{
	prevInput.atk = None;
	return prevInput; //simple as that lul
}

PlayerInputZip zipInput(PlayerInput input)
{
	PlayerInputZip zip = { 0 };
	//00mmmmaa
	zip.movAtk = (static_cast<char>(input.mov) << 2) | static_cast<char>(input.atk);
	zip.mouseRaw = input.mouse.raw_value();
	return zip;
}

PlayerInput unzipInput(PlayerInputZip zip)
{
	PlayerInput input;
	//00mmmmaa
	char movMask = 0b00111100;
	char atkMask = 0b00000011;
	input.atk = static_cast<AttackInput>(zip.movAtk & atkMask);
	input.mov = static_cast<MoveInput>((zip.movAtk & movMask) >> 2);
	input.mouse = input.mouse.from_raw_value(zip.mouseRaw);
	return input;
}

#endif