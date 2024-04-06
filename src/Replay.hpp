#ifndef RBST_REPLAY_HPP
#define RBST_REPLAY_HPP

#include "Input.hpp"

//since GGPO as-is does not make transparent which is the earliest confirm frame saved
//here's a hardcoded window size from the latest confirm frame backwards
//as 15f = 250ms, you'll be fine even at a ping of 500ms
const int REPLAY_ROLLBACK_WINDOW = 15;

struct ReplayWriter
{
	long confirmFrame;
	std::vector<InputData> inputBuffer;
	std::ofstream fileStream;
	int32_t p1LastMouse;
	int32_t p2LastMouse;
};

void openReplayFile(ReplayWriter* replay, Config* cfg)
{
	replay->confirmFrame = 0;
	replay->p1LastMouse = 0;
	replay->p2LastMouse = 0;
	replay->inputBuffer.clear();

	struct tm currDate;
	time_t currTime;
	time(&currTime);
	localtime_s(&currDate, &currTime);
	std::ostringstream fileNameOSS("");
	fileNameOSS << currDate.tm_year + 1900 << "-" << currDate.tm_mon + 1 << "-" << currDate.tm_mday << "_";
	fileNameOSS << currDate.tm_hour << "-" << currDate.tm_min << "-" << currDate.tm_sec;
	fileNameOSS << ".rbst";
	replay->fileStream.open(fileNameOSS.str().c_str(), std::fstream::out | std::fstream::binary);
	//configs at the time of match get saved along with following inputs
	replay->fileStream.write((char*)cfg, sizeof(*cfg));
}

void overwriteReplayInput(ReplayWriter* replay, InputData input, long frame)
{
	int index = static_cast<int>(frame - replay->confirmFrame);
	replay->inputBuffer[index] = input;
}

void writeReplayInput(ReplayWriter* replay, InputData input, long frame)
{
	int index = static_cast<int>(frame - replay->confirmFrame);
	replay->inputBuffer.push_back(input);
}

void consumeReplayInput(ReplayWriter* replay, long confFrame)
{
	while (replay->confirmFrame < (confFrame-REPLAY_ROLLBACK_WINDOW))
	{
		InputData input = replay->inputBuffer.front();
		char zipHeaders[2] = { 0 };
		//only store mouse when it moves
		int32_t p1MouseRaw = input.p1Input.mouse.raw_value(), p2MouseRaw = input.p2Input.mouse.raw_value();
		bool p1MouseMoved = replay->p1LastMouse != p1MouseRaw, p2MouseMoved = replay->p2LastMouse != p2MouseRaw;
		//move and attack inputs get always stored since they fit packed in a byte
		//and since together they take 6 bits, in goes a bit on whether mouse moved
		zipHeaders[0] = (int(p1MouseMoved) << 6) |
			(static_cast<char>(input.p1Input.mov) << 2) |
			static_cast<char>(input.p1Input.atk);
		zipHeaders[1] = (int(p2MouseMoved) << 6) |
			(static_cast<char>(input.p2Input.mov) << 2) |
			static_cast<char>(input.p2Input.atk);
		replay->fileStream.write(zipHeaders, 2);
		if (p1MouseMoved)
		{
			replay->fileStream.write((char*)&p1MouseRaw, sizeof(p1MouseRaw));
			replay->p1LastMouse = p1MouseRaw;
		}
		if (p2MouseMoved)
		{
			replay->fileStream.write((char*)&p2MouseRaw, sizeof(p2MouseRaw));
			replay->p2LastMouse = p2MouseRaw;
		}
		replay->inputBuffer.erase(replay->inputBuffer.begin());
		replay->confirmFrame++;
	}
}

void closeReplayFile(ReplayWriter* replay)
{
	replay->fileStream.close();
	replay->confirmFrame = 0;
	replay->inputBuffer.clear();
}

struct ReplayReader
{
	std::ifstream fileStream;
	int fileSize;
	int32_t p1LastMouse;
	int32_t p2LastMouse;
};

void openReplayFile(ReplayReader* replay, Config* cfg, const char* fileName)
{
	replay->p1LastMouse = 0;
	replay->p2LastMouse = 0;
	replay->fileStream.open(fileName, std::fstream::in | std::fstream::binary);
	replay->fileStream.seekg(0, replay->fileStream.end);
	replay->fileSize = replay->fileStream.tellg();
	replay->fileStream.seekg(0, replay->fileStream.beg);
	//configs at time of match get read first, 100bytes in total
	replay->fileStream.read((char*)cfg, sizeof(*cfg));
}

bool replayFileEnd(ReplayReader* replay)
{
	return replay->fileStream.tellg() == replay->fileSize;
}

InputData readReplayFile(ReplayReader* replay)
{
	PlayerInput p1, p2;
	int32_t p1MouseRaw = 0, p2MouseRaw = 0;
	char movMask   = 0b00111100;
	char atkMask   = 0b00000011;
	char mouseMask = 0b01000000;

	char zipHeaders[2];
	replay->fileStream.read(zipHeaders, 2);

	p1.atk = static_cast<AttackInput>(zipHeaders[0] & atkMask);
	p1.mov = static_cast<MoveInput>((zipHeaders[0] & movMask) >> 2);
	p2.atk = static_cast<AttackInput>(zipHeaders[1] & atkMask);
	p2.mov = static_cast<MoveInput>((zipHeaders[1] & movMask) >> 2);

	if (zipHeaders[0] & mouseMask)
	{
		replay->fileStream.read((char*)&p1MouseRaw, sizeof(p1MouseRaw));
		replay->p1LastMouse = p1MouseRaw;
	}
	else p1MouseRaw = replay->p1LastMouse;

	if (zipHeaders[1] & mouseMask)
	{
		replay->fileStream.read((char*)&p2MouseRaw, sizeof(p2MouseRaw));
		replay->p2LastMouse = p2MouseRaw;
	}
	else p2MouseRaw = replay->p2LastMouse;

	p1.mouse = p1.mouse.from_raw_value(p1MouseRaw);
	p2.mouse = p2.mouse.from_raw_value(p2MouseRaw);

	return InputData{ p1,p2 };
}

void closeReplayFile(ReplayReader* replay)
{
	replay->fileStream.close();
}

#endif