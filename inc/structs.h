#pragma once

typedef struct
{
	u8 x;
	u8 y;
	string label;
}Option;

typedef struct
{
	u8 index;
	u8 frameAmount;
	u8 frameTime; // In NTSC frames (60 frames = 1 second)
}AnimMData;

typedef struct
{

}SaveMData;

typedef struct
{
	Sprite* sprite;
	Vect2D_f32 positions;
	Vect2D_f32 velocities;
	fix32 maxSpeed;
	fix32 jumpSpeed;
}PlayerMData;