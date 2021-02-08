#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"
#include <vector>
#include <iostream>

#define H_SCREEN_SPLIT 0.666f
#define H_PIXEL_RES 640
#define V_PIXEL_RES 360

#define DEG_TO_RAD 0.0174532925f

// Override base class with your custom functionality
class Example : public olc::PixelGameEngine
{
public:
	Example()
	{
		// Name you application
		sAppName = "Example";
	}

public:
	bool OnUserCreate() override
	{
		// Called once at the start, so create things here
		return true;
	}

	bool OnUserUpdate(float fElapsedTime) override
	{
		if (GetKey(olc::Key::D).bPressed)
			horScreenSplit = horScreenSplit + 0.08f;
		if (GetKey(olc::Key::A).bPressed)
			horScreenSplit = horScreenSplit - 0.08f;
		if (GetKey(olc::Key::Q).bHeld)
			fPlayerA = fPlayerA + (50.f * fElapsedTime);
		if (GetKey(olc::Key::E).bHeld)
			fPlayerA = fPlayerA - (50.f * fElapsedTime);

		if (horScreenSplit >= 1.0f) horScreenSplit = 1.0f;
		else if (horScreenSplit <= 0.0f) horScreenSplit = 0.0f;

		if (fPlayerA >= 360.0f) fPlayerA = fPlayerA - 360.0f;
		else if (fPlayerA < 0.0f) fPlayerA = 360.0f + fPlayerA;

		Clear(olc::BLACK);

		// This is where we later render the actual raycasting section
		/*for (int x = 0; x < ScreenWidth() * horScreenSplit; x++)
			for (int y = 0; y < ScreenHeight(); y++)
				Draw(x, y, olc::Pixel(rand() % 256, rand() % 256, rand() % 256));*/

		DrawStringDecal({ H_PIXEL_RES * horScreenSplit + 25, 25 }, "Delta Time: " + std::to_string(fElapsedTime), olc::WHITE, { 1.f, 1.f });
		DrawStringDecal({ H_PIXEL_RES * horScreenSplit + 25, 40 }, "FPS: " + std::to_string(GetFPS()), olc::WHITE, { 1.f, 1.f });
		DrawStringDecal({ H_PIXEL_RES * horScreenSplit + 25, 55 }, "H Split: " + std::to_string(horScreenSplit), olc::WHITE, { 1.f, 1.f });
		DrawStringDecal({ H_PIXEL_RES * horScreenSplit + 25, 75 }, "* Player Rot: " + std::to_string((int)fPlayerA), olc::DARK_GREEN, { 1.f, 1.f });
		DrawStringDecal({ H_PIXEL_RES * horScreenSplit + 25, 90 }, "* X: " + std::to_string((int)fPlayerX) + ", Y: " + std::to_string((int)fPlayerY), olc::DARK_GREEN, { 1.f, 1.f });

		int boxSize = ScreenWidth() - (ScreenWidth() * horScreenSplit);
		boxSize /= mWidth;
		int vBoxOri = ScreenHeight() - (boxSize * mHeight);

		for (int x = 0; x < mWidth; x++)
			for (int y = 0; y < mHeight; y++)
			{
				olc::vi2d vPos = { (int)std::floor(ScreenWidth() * horScreenSplit) + (boxSize * x), vBoxOri + (boxSize * y) };
				DrawRect(vPos, { boxSize - 1, boxSize - 1 }, (mData[y * mWidth + x] == 0) ? olc::BLACK : olc::WHITE);
				if (x == (int)fPlayerX && y == (int)fPlayerY)
				{
					DrawRect({ vPos.x + 1, vPos.y + 1 }, { boxSize - 3, boxSize - 3 }, olc::DARK_GREEN);
					DrawLine({ vPos.x + (int)(boxSize * (fPlayerX - x)), vPos.y + (int)(boxSize * (fPlayerY - y)) },
						     { vPos.x + (int)(cos(fPlayerA * DEG_TO_RAD) * boxSize) + (int)(boxSize * (fPlayerX - x)), vPos.y + (int)(sin(fPlayerA * DEG_TO_RAD) * boxSize) + (int)(boxSize * (fPlayerY - y)) });
					std::cout << "deltaX: " << (fPlayerX - x) << ", deltaY: " << (fPlayerY - y) << std::endl;
				}
			}

		return true;
	}

private:
	float horScreenSplit = H_SCREEN_SPLIT;
	int mWidth = 10, mHeight = 10;
	float fPlayerX = 7.5f, fPlayerY = 2.5f;
	float fPlayerA = 90.0f;
	std::vector<int> mData = 
	{
		1,1,1,1,1,1,1,1,1,1,
		1,0,0,0,1,0,0,0,0,1,
		1,0,0,0,0,0,0,0,0,1,
		1,0,0,0,1,0,0,0,0,1,
		1,0,0,0,1,1,1,0,1,1,
		1,0,0,0,0,0,0,0,0,1,
		1,0,0,0,0,1,0,0,0,1,
		1,0,0,0,0,1,1,0,0,1,
		1,0,0,0,0,0,0,0,0,1,
		1,1,1,1,1,1,1,1,1,1,
	};
};

int main()
{
	Example demo;
	if (demo.Construct(H_PIXEL_RES, V_PIXEL_RES, 2, 2, false, true, true))
		demo.Start();
	return 0;
}