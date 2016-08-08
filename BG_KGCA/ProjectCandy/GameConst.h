#pragma once

#define CLIENT_W	(600)
#define CLIENT_H	(950)
#define BOARD_W		(9)
#define BOARD_H		(9)
#define TILE_W		((CLIENT_W - 24) / BOARD_W)
#define TILE_H		(TILE_W)
#define BOARD_X		((CLIENT_W - (BOARD_W * TILE_W)) / 2)
#define BOARD_Y		((CLIENT_H - (BOARD_H * TILE_H)) / 2)
