#ifndef RBST_PRESENTATION_HPP
#define RBST_PRESENTATION_HPP

//Raylib
#include <raylib.h>
//TOML++
#include <toml++/toml.h>
//-----
#include "Math.hpp"
#include "SecondarySim.hpp"
#include "GameState.hpp"

const int screenWidth = 1280;
const int screenHeight = 720;
const int centerX = screenWidth / 2;
const int centerY = screenHeight / 2;

struct Sprites
{
	Texture2D logo;

	struct {
		Texture2D atlas;
		Vector2 size;
	} chars;

	struct {
		Texture2D atlas;
		Rectangle red;
		Rectangle blue;
		Rectangle target;
	} projs;

	struct {
		Texture2D atlas;
		Rectangle heartRed;
		Rectangle heartBlue;
		Rectangle roundNo;
		Rectangle roundYes;
	} hearts;

	struct {
		Texture2D atlas;
		Rectangle graze;
		Rectangle alert;
	} effects;

	struct {
		Texture2D full;
		Texture2D dither;
		Shader shader;
		int maskLoc;
	} mask;

	struct
	{
		Shader shader;
		struct {
			Model plane;
			Texture2D texture;
		} radius;
		struct {
			Model plane;
			Texture2D texture;
		} arena;
	} circle;

	struct {
		struct {
			Model path;
			Texture2D texture;
			Shader shader;
			int scroll;
		} charge;
		struct {
			Model path;
			Texture2D texture;
		} hitscanRed;
		struct {
			Model path;
			Texture2D texture;
		} hitscanBlue;
	} path;

	struct {
		Model model;
		Shader shader;
		int filter;
		int pos;
		int fade;
		struct {
			Model model;
			Shader shader;
			int filter;
			int pos;
			int fade;
		} invert;
	} combo;
};

static Mesh GenMeshPath()
{
	Mesh mesh = { 0 };
	mesh.triangleCount = 2;
	mesh.vertexCount = 4;
	mesh.vertices  = (float*)MemAlloc(mesh.vertexCount * 3 * sizeof(float));
	mesh.texcoords = (float*)MemAlloc(mesh.vertexCount * 2 * sizeof(float));
	mesh.normals   = (float*)MemAlloc(mesh.vertexCount * 3 * sizeof(float));
	mesh.indices   = (unsigned short*)MemAlloc(mesh.triangleCount * 3 * sizeof(unsigned short));

	mesh.vertices[0] = 0;    mesh.vertices[1]  = 0; mesh.vertices[2]  = -0.5;
	mesh.vertices[3] = 0;    mesh.vertices[4]  = 0; mesh.vertices[5]  = 0.5;
	mesh.vertices[6] = 1000; mesh.vertices[7]  = 0; mesh.vertices[8]  = -0.5;
	mesh.vertices[9] = 1000; mesh.vertices[10] = 0; mesh.vertices[11] = 0.5;
	mesh.normals[0] = 0; mesh.normals[1]  = 1; mesh.normals[2]  = 0;
	mesh.normals[3] = 0; mesh.normals[4]  = 1; mesh.normals[5]  = 0;
	mesh.normals[6] = 0; mesh.normals[7]  = 1; mesh.normals[8]  = 0;
	mesh.normals[9] = 0; mesh.normals[10] = 1; mesh.normals[11] = 0;
	mesh.texcoords[0] = 0; mesh.texcoords[1] = 0;
	mesh.texcoords[2] = 0; mesh.texcoords[3] = 1;
	mesh.texcoords[4] = 1; mesh.texcoords[5] = 0;
	mesh.texcoords[6] = 1; mesh.texcoords[7] = 1;

	mesh.indices[0] = 0; mesh.indices[1] = 1; mesh.indices[2] = 3;
	mesh.indices[3] = 0; mesh.indices[4] = 3; mesh.indices[5] = 2;

	UploadMesh(&mesh, false);

	return mesh;
}

Sprites LoadSprites()
{
	auto atlasFile = toml::parse_file("sprite/atlas.toml");
	
	Sprites sprs;
	sprs.logo = LoadTexture("sprite/logo.png");

	sprs.chars.atlas = LoadTexture("sprite/chars.png");
	auto charsSize = atlasFile["chars"]["size"];
	sprs.chars.size = Vector2{ charsSize[0].value_or(0.0f), charsSize[1].value_or(0.0f) };

	sprs.projs.atlas = LoadTexture("sprite/projs.png");
	auto projRed = atlasFile["projs"]["red"];
	sprs.projs.red = Rectangle{ projRed[0].value_or(0.0f), projRed[1].value_or(0.0f), projRed[2].value_or(0.0f), projRed[3].value_or(0.0f) };
	auto projBlue = atlasFile["projs"]["blue"];
	sprs.projs.blue = Rectangle{ projBlue[0].value_or(0.0f), projBlue[1].value_or(0.0f), projBlue[2].value_or(0.0f), projBlue[3].value_or(0.0f) };
	auto target = atlasFile["projs"]["target"];
	sprs.projs.target = Rectangle{ target[0].value_or(0.0f), target[1].value_or(0.0f), target[2].value_or(0.0f), target[3].value_or(0.0f) };

	sprs.hearts.atlas = LoadTexture("sprite/hearts.png");
	auto heartRed = atlasFile["hearts"]["heartRed"];
	sprs.hearts.heartRed = Rectangle{ heartRed[0].value_or(0.0f), heartRed[1].value_or(0.0f), heartRed[2].value_or(0.0f), heartRed[3].value_or(0.0f) };
	auto heartBlue = atlasFile["hearts"]["heartBlue"];
	sprs.hearts.heartBlue = Rectangle{ heartBlue[0].value_or(0.0f), heartBlue[1].value_or(0.0f), heartBlue[2].value_or(0.0f), heartBlue[3].value_or(0.0f) };
	auto roundNo = atlasFile["hearts"]["roundNo"];
	sprs.hearts.roundNo = Rectangle{ roundNo[0].value_or(0.0f), roundNo[1].value_or(0.0f), roundNo[2].value_or(0.0f), roundNo[3].value_or(0.0f) };
	auto roundYes = atlasFile["hearts"]["roundYes"];
	sprs.hearts.roundYes = Rectangle{ roundYes[0].value_or(0.0f), roundYes[1].value_or(0.0f), roundYes[2].value_or(0.0f), roundYes[3].value_or(0.0f) };

	sprs.effects.atlas = LoadTexture("sprite/effects.png");
	auto graze = atlasFile["effects"]["graze"];
	sprs.effects.graze = Rectangle{ graze[0].value_or(0.0f), graze[1].value_or(0.0f), graze[2].value_or(0.0f), graze[3].value_or(0.0f) };
	auto alert = atlasFile["effects"]["alert"];
	sprs.effects.alert = Rectangle{ alert[0].value_or(0.0f), alert[1].value_or(0.0f), alert[2].value_or(0.0f), alert[3].value_or(0.0f) };

	Image mask = GenImageChecked(sprs.chars.atlas.width, sprs.chars.atlas.height, 1, 1, WHITE, Color{ 0,0,0,0 });
	sprs.mask.dither = LoadTextureFromImage(mask);
	ImageClearBackground(&mask, WHITE);
	sprs.mask.full = LoadTextureFromImage(mask);
	UnloadImage(mask);
	sprs.mask.shader = LoadShader(0, "shader/mask.fs");
	sprs.mask.maskLoc = GetShaderLocation(sprs.mask.shader, "mask");

	sprs.circle.shader = LoadShader(0, "shader/circle.fs");
	sprs.circle.radius.texture = LoadTexture("sprite/radius.png");
	sprs.circle.radius.plane = LoadModelFromMesh(GenMeshPlane(1.0f, 1.0f, 1, 1));
	sprs.circle.radius.plane.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = sprs.circle.radius.texture;
	sprs.circle.radius.plane.materials[0].shader = sprs.circle.shader;
	sprs.circle.arena.texture = LoadTexture("sprite/arena.png");
	sprs.circle.arena.plane = LoadModelFromMesh(GenMeshPlane(1.0f, 1.0f, 1, 1));
	sprs.circle.arena.plane.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = sprs.circle.arena.texture;
	sprs.circle.arena.plane.materials[0].shader = sprs.circle.shader;

	sprs.path.charge.shader = LoadShader(0,"shader/path.fs");
	sprs.path.charge.scroll = GetShaderLocation(sprs.path.charge.shader, "scroll");
	sprs.path.charge.texture = LoadTexture("sprite/charge.png");
	sprs.path.charge.path = LoadModelFromMesh(GenMeshPath());
	sprs.path.charge.path.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = sprs.path.charge.texture;
	sprs.path.charge.path.materials[0].shader = sprs.path.charge.shader;
	sprs.path.hitscanRed.texture = LoadTexture("sprite/railRed.png");
	sprs.path.hitscanRed.path = LoadModelFromMesh(GenMeshPath());
	sprs.path.hitscanRed.path.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = sprs.path.hitscanRed.texture;
	sprs.path.hitscanBlue.texture = LoadTexture("sprite/railBlue.png");
	sprs.path.hitscanBlue.path = LoadModelFromMesh(GenMeshPath());
	sprs.path.hitscanBlue.path.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = sprs.path.hitscanBlue.texture;

	sprs.combo.shader = LoadShader("shader/combo.vs", "shader/combo.fs");
	sprs.combo.filter = GetShaderLocation(sprs.combo.shader, "filter");
	sprs.combo.pos = GetShaderLocation(sprs.combo.shader, "pos");
	sprs.combo.fade = GetShaderLocation(sprs.combo.shader, "fade");
	sprs.combo.model = LoadModelFromMesh(GenMeshSphere(1.0f, 20, 20));
	sprs.combo.model.materials[0].shader = sprs.combo.shader;
	sprs.combo.invert.shader = LoadShader("shader/combo_invert.vs", "shader/combo.fs");
	sprs.combo.invert.filter = GetShaderLocation(sprs.combo.invert.shader, "filter");
	sprs.combo.invert.pos = GetShaderLocation(sprs.combo.invert.shader, "pos");
	sprs.combo.invert.fade = GetShaderLocation(sprs.combo.invert.shader, "fade");
	sprs.combo.invert.model = LoadModelFromMesh(GenMeshSphere(1.0f, 20, 20));
	sprs.combo.invert.model.materials[0].shader = sprs.combo.invert.shader;

	return sprs;
}

void UnloadSprites(Sprites sprs)
{
	UnloadTexture(sprs.logo);
	UnloadTexture(sprs.chars.atlas);
	UnloadTexture(sprs.projs.atlas);
	UnloadTexture(sprs.hearts.atlas);
	UnloadTexture(sprs.effects.atlas);

	UnloadTexture(sprs.mask.dither);
	UnloadTexture(sprs.mask.full);
	UnloadShader(sprs.mask.shader);

	UnloadShader(sprs.path.charge.shader);
	UnloadTexture(sprs.path.charge.texture);
	UnloadModel(sprs.path.charge.path);
	UnloadTexture(sprs.path.hitscanRed.texture);
	UnloadModel(sprs.path.hitscanRed.path);
	UnloadTexture(sprs.path.hitscanBlue.texture);
	UnloadModel(sprs.path.hitscanBlue.path);

	UnloadShader(sprs.circle.shader);
	UnloadTexture(sprs.circle.radius.texture);
	UnloadModel(sprs.circle.radius.plane);
	UnloadTexture(sprs.circle.arena.texture);
	UnloadModel(sprs.circle.arena.plane);

	UnloadModel(sprs.combo.model);
	UnloadShader(sprs.combo.shader);
	UnloadModel(sprs.combo.invert.model);
	UnloadShader(sprs.combo.invert.shader);
}

//state: 0=default, 1=dashing, 2=stunned
Rectangle CharAtlas(const Sprites* sprs, playerid player, bool your, bool flipped, int state)
{
	int x = state * 2 * sprs->chars.size.x;
	int y = (player - 1) * sprs->chars.size.y;
	if (your) y = y + 2 * sprs->chars.size.y;
	if (flipped) x = x + sprs->chars.size.x;
	return Rectangle{
		static_cast<float>(x),
		static_cast<float>(y),
		sprs->chars.size.x,
		sprs->chars.size.y
	};
}

enum POV
{
	Spectator = 0,
	Player1 = 1,
	Player2 = 2
};

Camera3D initialCamera()
{
	Camera3D cam = { 0 };
	cam.position = Vector3{ 0.0f, 0.0f, 0.0f };
	cam.target = Vector3{ 0.0f, 0.0f, 0.0f };
	cam.up = Vector3{ 0.0f, 1.0f, 0.0f };
	cam.fovy = 60.0f;
	cam.projection = CAMERA_PERSPECTIVE;
	return cam;
}

void setCamera(Camera3D* cam, Vec2* lazyCam, const GameState* state, POV pov)
{
	num_det camBack{ -8 };
	float camHeight = 7;
	num_det tgtFront{ 3 };
	float tgtHeight = 0;
	num_det laziness{ 0.2 };
	float specHeight = 6;
	num_det standBackMult{ 1.5 };

	switch (pov)
	{
	case Player1:
		cam->position = fromDetVec2(v2::add(state->p1.pos, v2::scalarMult(state->p1.dir, camBack)), camHeight);
		cam->target = fromDetVec2(v2::add(state->p1.pos, v2::scalarMult(state->p1.dir, tgtFront)), tgtHeight);
		break;
	case Player2:
		cam->position = fromDetVec2(v2::add(state->p2.pos, v2::scalarMult(state->p2.dir, camBack)), camHeight);
		cam->target = fromDetVec2(v2::add(state->p2.pos, v2::scalarMult(state->p2.dir, tgtFront)), tgtHeight);
		break;
	case Spectator:
		Vec2 midDist = v2::scalarDiv(v2::sub(state->p1.pos, state->p2.pos), num_det{ 2 });
		Vec2 actionCenter = v2::add(state->p2.pos, midDist);
		Vec2 standBack = v2::rotate(v2::scalarMult(midDist, standBackMult), -camBack.half_pi());
		if (v2::length(standBack) < fpm::abs(camBack))
			standBack = v2::normalizeMult(standBack, fpm::abs(camBack));
		*lazyCam = v2::lerp(*lazyCam, v2::add(actionCenter, standBack), laziness);
		cam->position = fromDetVec2(*lazyCam, specHeight);
		cam->target = fromDetVec2(actionCenter, tgtHeight);
		break;
	}
}

void drawBars(const Player* player, const Config* cfg)
{
	DrawRectangle((screenWidth / 2) - 150, screenHeight - 60, (300 * player->ammo) / cfg->ammoMax, 20, GREEN);
	for (int i = 1; i <= (cfg->ammoMax / cfg->shotCost); i++)
	{
		int lineX = (screenWidth / 2) - 150 + (300 * i * cfg->shotCost) / cfg->ammoMax;
		DrawLine(lineX, screenHeight - 65, lineX, screenHeight - 25, BLACK);
	}
	DrawRectangle((screenWidth / 2) - 150, screenHeight - 30, (300 * player->stamina) / cfg->staminaMax, 20, YELLOW);
	for (int i = 1; i <= (cfg->staminaMax / cfg->dashCost); i++)
	{
		int lineX = (screenWidth / 2) - 150 + (300 * i * cfg->dashCost) / cfg->staminaMax;
		DrawLine(lineX, screenHeight - 35, lineX, screenHeight - 5, BLACK);
	}
}

void gameScene(POV pov, const GameState* state, const SecSimParticles* particles, const Config* cfg, const Camera3D* cam, const Sprites* sprs)
{
	BeginMode3D(*cam);
	{
		//DRAW ARENA
		{
			DrawModel(
				sprs->circle.arena.plane,
				Vector3{ 0.0f, -0.01f, 0.0f },
				fromDetNum(cfg->arenaRadius) * 2,
				WHITE);
			DrawGrid(10, static_cast<float>(cfg->arenaRadius) / 10.0f);
		}

		//DRAW PLAYERS
		{
			Vec2 camAngle{
			num_det{ cam->target.x - cam->position.x },
			num_det{ cam->target.z - cam->position.z }
			};
			Vec2 camRight = v2::normalize(v2::rotate(camAngle, camAngle.x.half_pi()));

			float p1Shake = 0, p2Shake = 0;
			int p1State = 0, p2State = 0;
			bool p1Mirror = false, p2Mirror = false;
			if (state->p1.stunned)
			{
				p1State = 2;
				p1Shake = state->p1.hitstopCount * GetRandomValue(-10, 10) / 300.0f;
				p1Mirror = v2::dot(camRight, state->p1.vel) < num_det{ 0 };
			}
			else if (state->p1.pushdown.top() == Dashing)
			{
				p1State = 1;
				p1Mirror = v2::dot(camRight, state->p1.dashVel) < num_det{ 0 };
			}
			else p1Mirror = (!pov == Player1) && v2::dot(camRight, state->p1.dir) < num_det{ 0 };

			if (state->p2.stunned)
			{
				p2State = 2;
				p2Shake = state->p2.hitstopCount * GetRandomValue(-10, 10) / 300.0f;
				p2Mirror = v2::dot(camRight, state->p2.vel) < num_det{ 0 };
			}
			else if (state->p2.pushdown.top() == Dashing)
			{
				p2State = 1;
				p2Mirror = v2::dot(camRight, state->p2.dashVel) < num_det{ 0 };
			}
			else p2Mirror = (!pov == Player2) && v2::dot(camRight, state->p2.dir) < num_det{ 0 };

			//player sprites
			BeginShaderMode(sprs->mask.shader);
			if (pov == Player1)
				SetShaderValueTexture(sprs->mask.shader, sprs->mask.maskLoc, sprs->mask.dither);
			else
				SetShaderValueTexture(sprs->mask.shader, sprs->mask.maskLoc, sprs->mask.full);
			DrawBillboardPro(*cam,
				(sprs->chars.atlas),
				CharAtlas(sprs, 1, (pov == Player1), p1Mirror, p1State),
				fromDetVec2WithShake(state->p1.pos, camRight, fromDetNum(cfg->playerRadius) * 1.5, p1Shake),
				Vector3{ 0,1,0 },
				Vector2{ fromDetNum(cfg->playerRadius) * 3, fromDetNum(cfg->playerRadius) * 3 },
				Vector2{ 0.0f, 0.0f },
				0.0f, WHITE);
			EndShaderMode();

			BeginShaderMode(sprs->mask.shader);
			if (pov == Player2)
				SetShaderValueTexture(sprs->mask.shader, sprs->mask.maskLoc, sprs->mask.dither);
			else
				SetShaderValueTexture(sprs->mask.shader, sprs->mask.maskLoc, sprs->mask.full);
			DrawBillboardPro(*cam,
				(sprs->chars.atlas),
				CharAtlas(sprs, 2, (pov == Player2), p2Mirror, p2State),
				fromDetVec2WithShake(state->p2.pos, camRight, fromDetNum(cfg->playerRadius) * 1.5, p2Shake),
				Vector3{ 0,1,0 },
				Vector2{ fromDetNum(cfg->playerRadius) * 3, fromDetNum(cfg->playerRadius) * 3 },
				Vector2{ 0.0f, 0.0f },
				0.0f, WHITE);
			EndShaderMode();
		}

		BeginShaderMode(sprs->mask.shader);
		SetShaderValueTexture(sprs->mask.shader, sprs->mask.maskLoc, sprs->mask.full);

		//DRAW PLAYER RADIUS AND RAIL PATH
		{
			//player radius
			DrawModel(
				sprs->circle.radius.plane,
				fromDetVec2(state->p1.pos, .01f),
				fromDetNum(cfg->playerRadius) * 2,
				WHITE);
			DrawModel(
				sprs->circle.radius.plane,
				fromDetVec2(state->p2.pos, .01f),
				fromDetNum(cfg->playerRadius) * 2,
				WHITE);

			//charge paths
			if (state->p1.pushdown.top() == PState::Charging)
			{
				float angle = RAD2DEG * angleFromDetVec2(state->p1.dir);
				float divert = 1.0f - (static_cast<float>(state->p1.chargeCount) / static_cast<float>(cfg->chargeDuration));
				float scroll = (divert * divert);
				SetShaderValue(sprs->path.charge.shader, sprs->path.charge.scroll, &scroll, SHADER_UNIFORM_FLOAT);
				DrawModelEx(sprs->path.charge.path,
					fromDetVec2(state->p1.pos, .01f),
					Vector3{ 0,-1,0 },
					angle,
					Vector3{ 1,1,.25 },
					WHITE);
				DrawModelEx(sprs->path.charge.path,
					fromDetVec2(state->p1.pos, .005f),
					Vector3{ 0,-1,0 },
					angle + (divert * 30),
					Vector3{ 1,1,.1 },
					WHITE);
				DrawModelEx(sprs->path.charge.path,
					fromDetVec2(state->p1.pos, .005f),
					Vector3{ 0,-1,0 },
					angle - (divert * 30),
					Vector3{ 1,1,.1 },
					WHITE);
			}
			if (state->p2.pushdown.top() == PState::Charging)
			{
				float angle = RAD2DEG * angleFromDetVec2(state->p2.dir);
				float divert = 1.0f - (static_cast<float>(state->p2.chargeCount) / static_cast<float>(cfg->chargeDuration));
				float scroll = (divert * divert);
				SetShaderValue(sprs->path.charge.shader, sprs->path.charge.scroll, &scroll, SHADER_UNIFORM_FLOAT);
				DrawModelEx(sprs->path.charge.path,
					fromDetVec2(state->p2.pos, .01f),
					Vector3{ 0,-1,0 },
					angle,
					Vector3{ 1,1,.25 },
					WHITE);
				DrawModelEx(sprs->path.charge.path,
					fromDetVec2(state->p2.pos, .005f),
					Vector3{ 0,-1,0 },
					angle + (divert * 30),
					Vector3{ 1,1,.1 },
					WHITE);
				DrawModelEx(sprs->path.charge.path,
					fromDetVec2(state->p2.pos, .005f),
					Vector3{ 0,-1,0 },
					angle - (divert * 30),
					Vector3{ 1,1,.1 },
					WHITE);
			}
		}

		//DRAW PROJECTILE
		{
			bool showCombos = false;
			int framesToAltShot = 0;
			switch (pov)
			{
			case Player1:
				showCombos = state->p1.ammo >= cfg->altShotCost;
				framesToAltShot = state->p1.pushdown.top() == Charging ? (cfg->chargeDuration - state->p1.chargeCount) : cfg->chargeDuration;
				break;
			case Player2:
				showCombos = state->p2.ammo >= cfg->altShotCost;
				framesToAltShot = state->p2.pushdown.top() == Charging ? (cfg->chargeDuration - state->p2.chargeCount) : cfg->chargeDuration;
				break;
			}
			for (auto it = state->projs.begin(); it != state->projs.end(); ++it)
			{
				Vec2 futurePos = v2::add(it->pos, v2::scalarMult(it->vel, num_det{ framesToAltShot }));
				bool withinReach = pov != Spectator && v2::length(futurePos) < cfg->arenaRadius;
				//draw projectile
				switch (it->owner)
				{
				case 1:
					DrawBillboardPro(*cam,
						sprs->projs.atlas,
						sprs->projs.red, //source rect
						fromDetVec2(it->pos, fromDetNum(cfg->playerRadius) * 2), //world pos
						Vector3{ 0.0f,1.0f,0.0f }, //up vector
						Vector2{ fromDetNum(cfg->projRadius) * 4, fromDetNum(cfg->projRadius) * 4 }, //size (proj size is defined by circle with half dimensions of sprite)
						Vector2{ 0.0f, 0.0f }, //anchor for rotation and scaling
						12 * it->lifetime, //rotation (degrees per frame)
						WHITE);
					break;
				case 2:
					DrawBillboardPro(*cam,
						sprs->projs.atlas,
						sprs->projs.blue, //source rect
						fromDetVec2(it->pos, fromDetNum(cfg->playerRadius) * 2), //world pos
						Vector3{ 0.0f,1.0f,0.0f }, //up vector
						Vector2{ fromDetNum(cfg->projRadius) * 4, fromDetNum(cfg->projRadius) * 4 }, //size (proj size is defined by circle with half dimensions of sprite)
						Vector2{ 0.0f, 0.0f }, //anchor for rotation and scaling
						12 * it->lifetime, //rotation (degrees per frame)
						WHITE);
					break;
				}
				//draw future position and combo radius
				if (withinReach)
				{
					DrawBillboardPro(*cam,
						sprs->projs.atlas,
						sprs->projs.target, //source rect
						fromDetVec2(futurePos, fromDetNum(cfg->playerRadius) * 2), //world pos
						Vector3{ 0.0f,1.0f,0.0f }, //up vector
						Vector2{ fromDetNum(cfg->projRadius) * 2, fromDetNum(cfg->projRadius) * 2 }, //size
						Vector2{ 0.0f, 0.0f }, //anchor for rotation and scaling
						0.0f,
						WHITE);
					if (showCombos)
					{
						float filter = 0.01;
						float pos[2] = { fromDetNum(futurePos.x), fromDetNum(futurePos.y) };
						float fade = 0.0;
						SetShaderValue(sprs->combo.shader, sprs->combo.filter, &filter, SHADER_UNIFORM_FLOAT);
						SetShaderValue(sprs->combo.shader, sprs->combo.pos, pos, SHADER_UNIFORM_VEC2);
						SetShaderValue(sprs->combo.shader, sprs->combo.fade, &fade, SHADER_UNIFORM_FLOAT);
						DrawModel(sprs->combo.model,
							fromDetVec2(futurePos, fromDetNum(cfg->playerRadius) * 2),
							fromDetNum(cfg->comboRadius),
							WHITE);
						SetShaderValue(sprs->combo.invert.shader, sprs->combo.invert.filter, &filter, SHADER_UNIFORM_FLOAT);
						SetShaderValue(sprs->combo.invert.shader, sprs->combo.invert.pos, pos, SHADER_UNIFORM_VEC2);
						SetShaderValue(sprs->combo.invert.shader, sprs->combo.invert.fade, &fade, SHADER_UNIFORM_FLOAT);
						DrawModel(sprs->combo.invert.model,
							fromDetVec2(futurePos, fromDetNum(cfg->playerRadius) * 2),
							fromDetNum(cfg->comboRadius),
							WHITE);
					}
				}
				//draw ground radius
				DrawModel(
					sprs->circle.radius.plane,
					fromDetVec2(it->pos, .01f),
					fromDetNum(cfg->projRadius) * 2,
					WHITE);
			}
		}
		
		//DRAW PARTICLES
		//projectile collisions
		for (auto it = particles->projs.cbegin(); it != particles->projs.cend(); it++)
		{
			float lifetime_in_secs = it->lifetime / 60.0f;
			float size = std::max(0.0f, -12 * lifetime_in_secs * lifetime_in_secs + 4 * lifetime_in_secs + 1) * fromDetNum(cfg->projRadius) * 4;
			switch (it->owner)
			{
			case 1:
				DrawBillboardPro(*cam,
					sprs->projs.atlas,
					sprs->projs.red, //source rect
					fromDetVec2(it->pos, fromDetNum(cfg->playerRadius) * 2), //world pos
					Vector3{ 0.0f,1.0f,0.0f }, //up vector
					Vector2{ size, size }, //size (proj size is defined by circle with half dimensions of sprite)
					Vector2{ 0.0f, 0.0f }, //anchor for rotation and scaling
					24 * it->lifetime, //rotation (degrees per frame)
					WHITE);
				break;
			case 2:
				DrawBillboardPro(*cam,
					sprs->projs.atlas,
					sprs->projs.blue, //source rect
					fromDetVec2(it->pos, fromDetNum(cfg->playerRadius) * 2), //world pos
					Vector3{ 0.0f,1.0f,0.0f }, //up vector
					Vector2{ size, size }, //size (proj size is defined by circle with half dimensions of sprite)
					Vector2{ 0.0f, 0.0f }, //anchor for rotation and scaling
					24 * it->lifetime, //rotation (degrees per frame)
					WHITE);
				break;
			}
		}
		//grazes
		for (auto it = particles->grazes.cbegin(); it != particles->grazes.cend(); it++)
		{
			float fadeAlpha = std::max(0.0f, 1.0f - it->lifetime / 60.0f);
			Color fade{255,255,255,255.0*fadeAlpha};
			for (auto subIt = it->subParts.cbegin(); subIt != it->subParts.cend(); subIt++)
			{
				float size = subIt->w * fromDetNum(cfg->projRadius) * 2;
				float x = std::min(it->lifetime / 60.0f, 1.0f);
				float leap = -x*x + 2*x;
				Vector3 pos{
					fromDetNum(it->pos.x) + subIt->x * leap,
					subIt->z * fromDetNum(cfg->playerRadius) * 2,
					fromDetNum(it->pos.y) + subIt->y * leap
				};
				DrawBillboardPro(*cam,
					sprs->effects.atlas,
					sprs->effects.graze, //source rect
					pos, //world pos
					Vector3{ 0.0f,1.0f,0.0f }, //up vector
					Vector2{ size, size }, //size (proj size is defined by circle with half dimensions of sprite)
					Vector2{ 0.0f, 0.0f }, //anchor for rotation and scaling
					0.0f, //rotation (degrees per frame)
					fade);
			}
		}
		//alerts
		for (auto it = particles->alerts.cbegin(); it != particles->alerts.cend(); it++)
		{
			float fadeAlpha = std::max(0.0f, 1.0f - (it->lifetime * 1.0f) / 60.0f);
			Color fade{ 255,255,255,255.0 * fadeAlpha };
			for (auto subIt = it->subParts.cbegin(); subIt != it->subParts.cend(); subIt++)
			{
				float size = subIt->w * fromDetNum(cfg->projRadius) * 2;
				float x = std::min(it->lifetime / 60.0f, 1.0f);
				float leap = 2*(-x * x + 2 * x);
				Vector3 pos{
					fromDetNum(it->pos.x) + subIt->x * leap,
					subIt->z * fromDetNum(cfg->playerRadius) * 2,
					fromDetNum(it->pos.y) + subIt->y * leap
				};
				DrawBillboardPro(*cam,
					sprs->effects.atlas,
					sprs->effects.alert, //source rect
					pos, //world pos
					Vector3{ 0.0f,1.0f,0.0f }, //up vector
					Vector2{ size, size }, //size (proj size is defined by circle with half dimensions of sprite)
					Vector2{ 0.0f, 0.0f }, //anchor for rotation and scaling
					0.0f, //rotation (degrees per frame)
					fade);
			}
		}
		//hitscan trails
		for (auto it = particles->hitscans.cbegin(); it != particles->hitscans.cend(); it++)
		{
			float angle = RAD2DEG * angleFromDetVec2(it->dir);
			float lifetime_in_secs = it->lifetime / 60.0f;
			float size = std::max(0.0f, -12 * lifetime_in_secs * lifetime_in_secs + 4 * lifetime_in_secs + 1) * fromDetNum(cfg->projRadius) * 3;
			Color fade{ 255,255,255,std::min(255.0f, 255.0f * size) };
			switch (it->owner)
			{
			case 1:
				DrawModelEx(sprs->path.hitscanRed.path,
					fromDetVec2(it->pos, fromDetNum(cfg->playerRadius) * 2),
					Vector3{ 0,-1,0 },
					angle,
					Vector3{ 1,1,.15 },
					fade);
				DrawBillboardPro(*cam,
					sprs->projs.atlas,
					sprs->projs.red, //source rect
					fromDetVec2(it->pos, fromDetNum(cfg->playerRadius) * 2), //world pos
					Vector3{ 0.0f,1.0f,0.0f }, //up vector
					Vector2{ size, size }, //size (proj size is defined by circle with half dimensions of sprite)
					Vector2{ 0.0f, 0.0f }, //anchor for rotation and scaling
					6 * it->lifetime, //rotation (degrees per frame)
					WHITE);
				break;
			case 2:
				DrawModelEx(sprs->path.hitscanBlue.path,
					fromDetVec2(it->pos, fromDetNum(cfg->playerRadius) * 2),
					Vector3{ 0,-1,0 },
					angle,
					Vector3{ 1,1,.15 },
					fade);
				DrawBillboardPro(*cam,
					sprs->projs.atlas,
					sprs->projs.blue, //source rect
					fromDetVec2(it->pos, fromDetNum(cfg->playerRadius) * 2), //world pos
					Vector3{ 0.0f,1.0f,0.0f }, //up vector
					Vector2{ size, size }, //size (proj size is defined by circle with half dimensions of sprite)
					Vector2{ 0.0f, 0.0f }, //anchor for rotation and scaling
					12 * it->lifetime, //rotation (degrees per frame)
					WHITE);
				break;
			}
		}
		//combos
		for (auto it = particles->combos.cbegin(); it != particles->combos.cend(); it++)
		{
			float filter = 1.0f;
			float pos[2] = { fromDetNum(it->pos.x), fromDetNum(it->pos.y) };
			float fade = it->lifetime / 30.0f;
			SetShaderValue(sprs->combo.shader, sprs->combo.filter, &filter, SHADER_UNIFORM_FLOAT);
			SetShaderValue(sprs->combo.shader, sprs->combo.pos, pos, SHADER_UNIFORM_VEC2);
			SetShaderValue(sprs->combo.shader, sprs->combo.fade, &fade, SHADER_UNIFORM_FLOAT);
			DrawModel(sprs->combo.model,
				fromDetVec2(it->pos, fromDetNum(cfg->playerRadius) * 2),
				fromDetNum(cfg->comboRadius),
				WHITE);
			SetShaderValue(sprs->combo.invert.shader, sprs->combo.invert.filter, &filter, SHADER_UNIFORM_FLOAT);
			SetShaderValue(sprs->combo.invert.shader, sprs->combo.invert.pos, pos, SHADER_UNIFORM_VEC2);
			SetShaderValue(sprs->combo.invert.shader, sprs->combo.invert.fade, &fade, SHADER_UNIFORM_FLOAT);
			DrawModel(sprs->combo.invert.model,
				fromDetVec2(it->pos, fromDetNum(cfg->playerRadius) * 2),
				fromDetNum(cfg->comboRadius),
				WHITE);
		}

		EndShaderMode();
	}
	EndMode3D();
}

//MATCH PRESENTATION

//returns semaphore idle time
double present(POV pov, const GameState* state, const SecSimParticles* particles, const Config* cfg, Camera3D* cam, const Sprites* sprs, std::ostringstream* gameInfoOSS)
{
	setCamera(cam, NULL, state, pov);
	
	BeginDrawing();

	ClearBackground(Color{ 128,224,255,255 });

	gameScene(pov, state, particles, cfg, cam, sprs);

	float size = 6;
	for (int i = 0; i < state->health1; i++)
	{
		DrawTexturePro(sprs->hearts.atlas,
			sprs->hearts.heartRed,
			Rectangle{
				5 + 8 * size * i,
				5,
				8 * size,
				8 * size
			},
			Vector2{ 0,0 }, 0.0f, WHITE);
	}
	for (int i = 0; i < state->health2; i++)
	{
		DrawTexturePro(sprs->hearts.atlas,
			sprs->hearts.heartBlue,
			Rectangle{
				screenWidth - 5 - 8 * size * (i+1),
				5,
				8 * size,
				8 * size
			},
			Vector2{ 0,0 }, 0.0f, WHITE);
	}
	for (int i = 0; i < cfg->roundsToWin; i++)
	{
		DrawTexturePro(sprs->hearts.atlas,
			(i + 1 <= state->rounds1 ? sprs->hearts.roundYes : sprs->hearts.roundNo),
			Rectangle{
				5 + 8 * size * i,
				5 + 8 * size,
				8 * size,
				8 * size
			},
			Vector2{ 0,0 }, 0.0f, WHITE);
		DrawTexturePro(sprs->hearts.atlas,
			(i + 1 <= state->rounds2 ? sprs->hearts.roundYes : sprs->hearts.roundNo),
			Rectangle{
				screenWidth - 5 - 8 * size * (i + 1),
				5 + 8 * size,
				8 * size,
				8 * size
			},
			Vector2{ 0,0 }, 0.0f, WHITE);
	}
	//crosshair
	DrawLineEx(
		Vector2{ centerX - 20 , centerY - 150 },
		Vector2{ centerX - 5, centerY - 150 },
		3.f, RED);
	DrawLineEx(
		Vector2{ centerX + 5 , centerY - 150 },
		Vector2{ centerX + 20, centerY - 150 },
		3.f, RED);
	DrawLineEx(
		Vector2{ centerX, centerY - 170 },
		Vector2{ centerX, centerY - 155 },
		3.f, RED);
	DrawLineEx(
		Vector2{ centerX, centerY - 145 },
		Vector2{ centerX, centerY - 130 },
		3.f, RED);
	DrawLineEx(
		Vector2{ centerX, centerY - 70 },
		Vector2{ centerX, centerY - 20 },
		3.f, RED);

	if (pov == Player1)
		drawBars(&state->p1, cfg);
	else if (pov == Player2)
		drawBars(&state->p2, cfg);

	switch (state->phase)
	{
	case Countdown:
		DrawText(std::to_string(static_cast<int>(ceil(state->roundCountdown / 60.0))).c_str(),
			screenWidth / 2 - 50, screenHeight / 2 - 100, 150, BLACK);
		break;
	case Play:
		DrawText(std::to_string(state->roundCountdown / 60).c_str(),
			screenWidth / 2 - 40, 5, 60, BLACK);
		break;
	case End:
		if (state->health1 > state->health2)
			DrawText("P1 WIN!", screenWidth / 2 - 200, screenHeight / 2 - 100, 150, RED);
		else if (state->health1 < state->health2)
			DrawText("P2 WIN!", screenWidth / 2 - 200, screenHeight / 2 - 100, 150, BLUE);
		else
			DrawText("It's a tie.", screenWidth / 2 - 240, screenHeight / 2 - 100, 150, BLACK);
		break;
	}

	DrawText(gameInfoOSS->str().c_str(), 5, 5 + 16 * size, 20, GRAY);

	//I figure this is also the timing semaphore
	double beforeSemaphore = GetTime();
	EndDrawing();
	return GetTime() - beforeSemaphore;
}

//HOME SCREEN PRESENTATION

struct HomeInfo
{
	Vec2 lazyCam = v2::zero();
	bool homeScreen = true;
	std::string remoteAddress = "";
	float freshUpdate = 0;
	RenderTexture2D bgTarget;
	Shader bgShader;
};

void presentMenu(POV pov, const GameState* state, const SecSimParticles* particles, const Config* cfg, Camera3D* cam, const Sprites* sprs, std::ostringstream* gameInfoOSS, HomeInfo* home)
{
	setCamera(cam, &home->lazyCam, state, pov);

	BeginDrawing();

	BeginTextureMode(home->bgTarget);
	ClearBackground(Color{ 128,224,255,255 });
	gameScene(pov, state, particles, cfg, cam, sprs);
	EndTextureMode();

	if (home->homeScreen)
	{
		BeginShaderMode(home->bgShader);
		// NOTE: Render texture must be y-flipped due to default OpenGL coordinates (left-bottom)
		DrawTextureRec(
			home->bgTarget.texture,
			Rectangle{ 0, 0, (float)home->bgTarget.texture.width, (float)-home->bgTarget.texture.height },
			Vector2 { 0, 0 }, WHITE);
		EndShaderMode();
		
		//LOGO
		int logoSize = 12;
		Vector2 logoPos = {
			screenWidth / 2 - (logoSize * sprs->logo.width / 2),
			screenHeight / 2 - (logoSize * sprs->logo.height / 2)
		};
		DrawTextureEx(
			sprs->logo,
			logoPos,
			0.0f,
			logoSize,
			WHITE);

		//MENU
		float xCenter = screenWidth / 2;
		float btnTop = screenHeight - 60;
		float btnWidth = 240;
		float btnHeight = 40;

		float p1X      = xCenter - 120 - 320;
		float p2X      = xCenter - 120;
		float addressX = xCenter - 120 + 320;
		Rectangle p1Btn = Rectangle{ p1X, btnTop, btnWidth, btnHeight };
		Rectangle p2Btn = Rectangle{ p2X, btnTop, btnWidth, btnHeight };
		Rectangle addressField = Rectangle{ addressX, btnTop, btnWidth, btnHeight };

		Color addressColor = Color{
			static_cast<unsigned char>(130 + home->freshUpdate * (0-130)),
			static_cast<unsigned char>(130 + home->freshUpdate * (158-130)),
			static_cast<unsigned char>(130 + home->freshUpdate * (47-130)),
			255
		};

		DrawRectangleRec(p1Btn, LIGHTGRAY);
		DrawRectangleLinesEx(p1Btn, 4, RED);
		DrawText("Connect as Player 1", p1Btn.x + 10, p1Btn.y + 10, 20, BLACK);
		DrawText("F1", p1Btn.x+1, p1Btn.y+1 - 20, 20, RED);
		DrawText("F1", p1Btn.x, p1Btn.y - 20, 20, WHITE);

		DrawRectangleRec(p2Btn, LIGHTGRAY);
		DrawRectangleLinesEx(p2Btn, 4, BLUE);
		DrawText("Connect as Player 2", p2Btn.x + 10, p2Btn.y + 10, 20, BLACK);
		DrawText("F2", p2Btn.x+1, p2Btn.y+1 - 20, 20, BLUE);
		DrawText("F2", p2Btn.x, p2Btn.y - 20, 20, WHITE);

		DrawRectangleRec(addressField, WHITE);
		DrawRectangleLinesEx(addressField, 4, addressColor);
		DrawText(home->remoteAddress.c_str(), addressField.x + 10, addressField.y + 10, 20, BLACK);
		DrawText("Remote IP Address\nCtrl+V to paste it here", addressField.x+1, addressField.y+1 - 50, 20, BLACK);
		DrawText("Remote IP Address\nCtrl+V to paste it here", addressField.x, addressField.y - 50, 20, WHITE);


		DrawText("Press F4 to show background demo", 880, 5, 20, BLACK);
	}
	else
	{
		DrawTextureRec(
			home->bgTarget.texture,
			Rectangle{ 0, 0, (float)home->bgTarget.texture.width, (float)-home->bgTarget.texture.height },
			Vector2{ 0, 0 }, WHITE);
	}

	DrawText(gameInfoOSS->str().c_str(), 5, 5, 20, BLACK);

	EndDrawing();
}

#endif // !RBST_PRESENTATION_HPP
