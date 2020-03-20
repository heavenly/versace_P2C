#pragma once

class CLobbyInfo
{
public:
	char pad_0000[16]; //0x0000
	int32_t num_players; //0x0010
	char pad_0014[92]; //0x0014
	int32_t lobby_id; //0x0070
};