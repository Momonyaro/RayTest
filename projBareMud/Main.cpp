#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"
#include <vector>
#include <iostream>

#define H_SCREEN_SPLIT 0.666f
#define H_PIXEL_RES 640
#define V_PIXEL_RES 360

#define DEG_TO_RAD 0.0174532925f
#define PI 3.141592653f

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
		if (GetKey(olc::Key::D).bHeld)
		{
			fPlayerA = fPlayerA + (150.f * fElapsedTime);
			if (fPlayerA >= 360.0f) fPlayerA = fPlayerA - 360.0f;
		}
		if (GetKey(olc::Key::A).bHeld)
		{
			fPlayerA = fPlayerA - (150.f * fElapsedTime);
			if (fPlayerA < 0.0f) fPlayerA = 360.0f + fPlayerA;
		}

		fPlayerDX = cos(fPlayerA * DEG_TO_RAD) * 4.f;
		fPlayerDY = sin(fPlayerA * DEG_TO_RAD) * 4.f;
		if (GetKey(olc::Key::W).bHeld)
		{
			fPlayerX += fPlayerDX * fElapsedTime;
			fPlayerY += fPlayerDY * fElapsedTime;
		}
		if (GetKey(olc::Key::S).bHeld)
		{
			fPlayerX -= fPlayerDX * fElapsedTime;
			fPlayerY -= fPlayerDY * fElapsedTime;
		}

		if (horScreenSplit >= 1.0f) horScreenSplit = 1.0f;
		else if (horScreenSplit <= 0.0f) horScreenSplit = 0.0f;

		Clear(olc::BLACK);
		int boxSize = ScreenWidth() - (ScreenWidth() * horScreenSplit);
		boxSize /= mWidth;
		int vBoxOri = ScreenHeight() - (boxSize * mHeight);

		// This is where we later render the actual raycasting section
		DrawRaycasting(vBoxOri, boxSize);

		DrawStringDecal({ H_PIXEL_RES * horScreenSplit + 25, 25 }, "Delta Time: " + std::to_string(fElapsedTime), olc::WHITE, { 1.f, 1.f });
		DrawStringDecal({ H_PIXEL_RES * horScreenSplit + 25, 40 }, "FPS: " + std::to_string(GetFPS()), olc::WHITE, { 1.f, 1.f });
		DrawStringDecal({ H_PIXEL_RES * horScreenSplit + 25, 55 }, "H Split: " + std::to_string(horScreenSplit), olc::WHITE, { 1.f, 1.f });
		DrawStringDecal({ H_PIXEL_RES * horScreenSplit + 25, 75 }, "* Player Rot: " + std::to_string((int)fPlayerA), olc::DARK_GREEN, { 1.f, 1.f });
		DrawStringDecal({ H_PIXEL_RES * horScreenSplit + 25, 90 }, "* X: " + std::to_string((int)fPlayerX) + ", Y: " + std::to_string((int)fPlayerY), olc::DARK_GREEN, { 1.f, 1.f });

		olc::vi2d l1 = {};
		olc::vi2d l2 = {};

		for (int x = 0; x < mWidth; x++)
			for (int y = 0; y < mHeight; y++)
			{
				olc::vi2d vPos = { (int)std::floor(ScreenWidth() * horScreenSplit) + (boxSize * x), vBoxOri + (boxSize * y) };
				DrawRect(vPos, { boxSize - 1, boxSize - 1 }, (mData[y * mWidth + x] == 0) ? olc::BLACK : olc::WHITE);
				if (x == (int)fPlayerX && y == (int)fPlayerY)
				{
					DrawRect({ vPos.x + 1, vPos.y + 1 }, { boxSize - 3, boxSize - 3 }, olc::DARK_GREEN);
					l1 = { vPos.x + (int)(boxSize * (fPlayerX - x)), vPos.y + (int)(boxSize * (fPlayerY - y)) };
					l2 = { vPos.x + (int)(cos(fPlayerA * DEG_TO_RAD) * boxSize) + (int)(boxSize * (fPlayerX - x)), vPos.y + (int)(sin(fPlayerA * DEG_TO_RAD) * boxSize) + (int)(boxSize * (fPlayerY - y)) };
					//std::cout << "deltaX: " << (fPlayerX - x) << ", deltaY: " << (fPlayerY - y) << std::endl;
				}
			}

		DrawLine(l1, l2);

		return true;
	}

private:

	float Distance(olc::vf2d a, olc::vf2d b)
	{
		float dx = a.x - b.x;
		float dy = a.y - b.y;
		return sqrtf(dx * dx + dy * dy);
	}
	
	void DrawRaycasting(int vBoxOri, int boxSize)
	{

		float r = 0;
		float rDist = 0;
		float rX = 0, rY = 0;
		bool hitWall = false;
		for (r = fPlayerA - fHalfFov; r < fPlayerA + fHalfFov; r++)
		{
			hitWall = false;
			rDist = 0;
			rX = fPlayerX; rY = fPlayerY;
			int vDir = (r >= 0 && r <= 180) ? -1 : 1; //1 = up, -1 = down
			int hDir = (r <= 90 || r >= 270) ? 1 : -1; //1 = right, -1 = left

			while (fDrawDist > rDist)
			{
				rX += cos(r * DEG_TO_RAD) * 0.1f;	//Walk a small unit
				rY += sin(r * DEG_TO_RAD) * 0.1f;

				rDist = Distance({ fPlayerX, fPlayerY }, { rX, rY });

				if (rX < 0 || rX >= mWidth || rY < 0 || rY >= mHeight) { std::cout << "Ray went out of bounds, x:" << rX << " y:" << rY << " d:" << rDist << std::endl; break; }
				if (mData[(int)floorf(rY) * mWidth + (int)floorf(rX)] != 0)
				{
					//Collided with a wall, let's smooth it out?
					// How would we do this, we have the shortest delta for both axis but that doen't say at what angle we hit the wall

					hitWall = true;
					break;
				}
				
			}

			int hBoxOri = H_PIXEL_RES * H_SCREEN_SPLIT;

			DrawLine({ hBoxOri + (int)(fPlayerX * boxSize), vBoxOri + (int)(fPlayerY * boxSize) }, { hBoxOri + (int)(rX * boxSize), vBoxOri + (int)(rY * boxSize) }, olc::RED);
			float wallHeight = 150 / rDist;
			if (hitWall)
			{
				olc::Pixel columnCol = olc::BLACK;

				if (rDist / fDrawDist < 0.35f)
				{ columnCol = olc::WHITE; }
				else if (rDist / fDrawDist < 0.55f )
				{ columnCol = olc::GREY; }
				else if (rDist / fDrawDist < 0.8f )
				{ columnCol = olc::DARK_GREY; }

				DrawRect({ (int)((r - fPlayerA) + fHalfFov) * (int)fPixelBlockW, V_PIXEL_RES / 2 - (int)wallHeight / 2 }, { (int)fPixelBlockW - 1, (int)wallHeight }, columnCol);
			}
			// Draw the shortest ray
		}

	}

	float horScreenSplit = H_SCREEN_SPLIT;
	int mWidth = 25, mHeight = 30;
	float fPlayerX = 2.5f, fPlayerY = 2.5f;
	float fPlayerDX = 0.f, fPlayerDY = 0.f;
	float fPlayerA = 0.f;
	float fDrawDist = 6.0f;
	float fHalfFov = 42.0f;
	float fPixelBlockW = 5.0f; // How wide a column is in the FPS view.
	std::vector<int> mData = 
	{
		1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
		1,0,0,0,0,1,1,1,1,1,0,0,0,1,1,0,0,0,1,1,0,0,0,0,1,
		1,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
		1,0,0,0,0,1,1,0,0,0,0,0,0,1,1,0,0,0,0,1,0,0,0,0,1,
		1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,1,
		1,1,1,0,0,0,0,0,0,0,1,1,1,0,0,0,1,1,1,1,1,1,0,1,1,
		1,1,1,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,1,0,0,0,0,1,1,
		1,1,1,0,0,0,0,0,0,0,1,1,1,0,0,0,1,0,0,0,1,1,1,1,1,
		1,1,1,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
		1,1,1,1,0,1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
		1,1,1,1,0,0,0,0,0,1,1,1,1,1,0,0,0,0,0,1,1,1,1,1,1,
		1,1,1,1,1,1,0,1,1,1,1,1,1,1,0,1,0,1,0,1,1,1,1,1,1,
		1,1,1,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,1,1,1,1,1,1,
		1,1,1,0,1,0,0,0,1,0,1,1,0,1,0,1,0,1,0,1,1,1,1,1,1,
		1,1,1,0,0,0,0,0,0,0,1,1,0,1,0,0,0,0,0,1,1,1,1,1,1,
		1,1,1,0,1,0,0,0,1,0,1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,
		1,1,1,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,0,0,0,1,1,1,1,
		1,1,1,0,1,0,0,0,1,0,1,1,1,1,0,0,0,0,0,0,0,1,1,1,1,
		1,1,1,0,0,0,0,0,0,0,1,1,1,1,0,1,1,1,0,0,0,1,1,1,1,
		1,1,1,0,1,0,0,0,1,0,1,1,1,1,0,1,0,0,0,0,0,1,1,1,1,
		1,1,1,0,0,0,0,0,0,0,1,1,1,1,0,1,0,0,0,0,1,1,1,1,1,
		1,1,1,1,1,1,0,1,1,1,1,1,1,1,0,1,0,0,0,0,1,1,1,1,1,
		1,1,1,1,1,1,0,1,1,1,1,1,1,1,0,1,1,1,1,0,1,1,1,1,1,
		1,1,1,0,0,0,0,1,1,1,1,1,1,0,0,0,1,1,1,0,1,1,1,1,1,
		1,1,1,0,1,1,1,1,1,1,1,1,1,0,0,0,1,1,1,0,1,1,1,1,1,
		1,0,0,0,0,0,1,1,1,1,1,1,1,0,0,0,1,0,0,0,0,0,0,0,1,
		1,0,0,0,0,0,0,0,0,0,1,1,1,0,0,0,0,0,0,0,0,0,0,0,1,
		1,0,0,0,0,0,1,1,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,
		1,0,0,0,0,0,1,1,1,1,1,1,1,0,0,0,1,1,0,0,0,0,0,0,1,
		1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1
	};
};

int main()
{
	Example demo;
	if (demo.Construct(H_PIXEL_RES, V_PIXEL_RES, 2, 2, true, true, true))
		demo.Start();
	return 0;
}