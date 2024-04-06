//Raylib
#include <raylib.h>
//TOML++
#include <toml++/toml.h>
//-----
#include "Math.hpp"
#include "Config.hpp"
#include "Input.hpp"
#include "Replay.hpp"
#include "Player.hpp"
#include "SecondarySim.hpp"
#include "GameState.hpp"
#include "Presentation.hpp"
#include "GGPOController.hpp"

//int main(int argc, char* argv[])
int WINAPI WinMain(HINSTANCE, HINSTANCE, PSTR, int)
{
	InitWindow(screenWidth, screenHeight, "RBST");
	SetTargetFPS(60);
	SetWindowState(FLAG_WINDOW_ALWAYS_RUN);

	HomeInfo home;

	bool cleanMode = false;
	
	auto homeFile = toml::parse_file("RBST_home.toml");
	home.remoteAddress = homeFile["Network"]["remoteAddress"].value_or("127.0.0.1");
	unsigned short port = homeFile["Network"]["port"].value_or(8001);
	int demos = homeFile["HomeScreen"]["demoFiles"].as_array()->size();

	Config demoCfg;
	GameState demoState;
	SecSimFlux demoFlux;
	SecSimParticles demoParticles;
	ReplayReader replayR;

	std::string newDemo = homeFile["HomeScreen"]["demoFiles"][GetRandomValue(0, demos-1)].value_or("demo.rbst");
	openReplayFile(&replayR, &demoCfg, newDemo.c_str());
	if (!replayR.fileStream.is_open())
	{
		//a little fallback
		demoCfg = readTOMLForCfg();
	}

	demoState = initialState(&demoCfg);
	
	POV demoPOV = Spectator;
	Camera3D demoCam = initialCamera();
	std::ostringstream demoOSS;

	home.bgTarget = LoadRenderTexture(screenWidth, screenHeight);
	home.bgShader = LoadShader(0, "shader/bg.fs");
	Sprites sprs = LoadSprites();

	//home screen
	while (!WindowShouldClose())
	{
		home.freshUpdate = std::max(0.0f, home.freshUpdate - (1.0f / 60.0f));
		
		//show/hide background replay
		if (IsKeyReleased(KEY_F4))
		{
			home.homeScreen = !home.homeScreen;
			if (home.homeScreen)
			{
				demoPOV = Spectator;
				cleanMode = false;
			}
		}

		if (!home.homeScreen)
		{
			//REPLAY CONTROLS
			if (IsKeyPressed(KEY_F1))
				demoPOV = Player1;
			else if (IsKeyPressed(KEY_F2))
				demoPOV = Player2;
			else if (IsKeyPressed(KEY_F3))
				demoPOV = Spectator;
			else if (IsKeyPressed(KEY_C))
				cleanMode = !cleanMode;
		}
		else
		{
			//MENU CONTROLS
			if (IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_V))
			{
				home.remoteAddress = std::string(GetClipboardText());
				home.freshUpdate = 1;
			}
			else if (IsKeyPressed(KEY_F1))
			{
				DisableCursor();
				NetworkedMain(&sprs, home.remoteAddress, port, 1);
				//back from match
				EnableCursor();
			}
			else if (IsKeyPressed(KEY_F2))
			{
				DisableCursor();
				NetworkedMain(&sprs, home.remoteAddress, port, 2);
				//back from match
				EnableCursor();
			}
		}
		if (replayFileEnd(&replayR))
		{
			closeReplayFile(&replayR);
			demoParticles.projs.clear();
			demoParticles.combos.clear();
			demoParticles.grazes.clear();
			demoParticles.alerts.clear();
			demoParticles.hitscans.clear();
			//repeat
			openReplayFile(&replayR, &demoCfg, newDemo.c_str());
			demoState = initialState(&demoCfg);
		}
		InputData input = readReplayFile(&replayR);
		//simulation
		demoState = simulate(demoState, &demoFlux, &demoCfg, input);
		//secondary simulation
		increaseParticleLifetime(&demoParticles);
		currentFrameSecSim(&demoFlux, &demoParticles, demoState.frame);
		demoFlux.projs.clear();
		demoFlux.combos.clear();
		demoFlux.grazes.clear();
		demoFlux.alerts.clear();
		demoFlux.hitscans.clear();

		int currentFps = GetFPS();
		demoOSS.str("");
		demoOSS << "FPS: " << currentFps << std::endl;
		if (home.homeScreen)
		{
			demoOSS << std::endl << "/// A game by Thiago da Fonte ///" << std::endl;
		}
		else
		{
			demoOSS << "Press F1 and F2 for player POVs," << std::endl;
			demoOSS << "F3 for spectator POV," << std::endl;
			demoOSS << "C for (clean? camera? cinematic?) mode," << std::endl;
			demoOSS << "or F4 to go back to menu." << std::endl;
		}
		if (cleanMode) demoOSS.str("");
		//presentation
		presentMenu(demoPOV, &demoState, &demoParticles, &demoCfg, &demoCam, &sprs, &demoOSS, &home);
	}
	UnloadRenderTexture(home.bgTarget);
	UnloadShader(home.bgShader);
	UnloadSprites(sprs);
	closeReplayFile(&replayR);
	CloseWindow();
}