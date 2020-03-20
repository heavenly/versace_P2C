#include "chat_spam.h"
#include "options.hpp"
#include <chrono>
#include <vector>
#include "valve_sdk/sdk.hpp"
void c_chat_spam::do_spam()
{
	if (!g_vars.misc_chatspam)
		return;

	const static std::vector<std::string> sayings = 
	{
	"*DEAD*",
	"ownage by VER$ACE",
	"https://discord.gg/hc7Jphr",
	"owned by top hax",
	"sorry russki, maybe get better hax",
	"https://www.youtube.com/channel/UCD2M8Rb2_XJm52qwOSucTTw",
	"versacehack.xyz - superior hack"
	};

	auto say_in_chat = [=](const char* text) -> void
	{
		char buffer[8192];
		sprintf_s(buffer, "say \"%s\"", text);
		g_EngineClient->ExecuteClientCmd(buffer);
	};

	auto epoch_time = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();

	static decltype(epoch_time) last_epoch = 0;

	//super FA$T chatspam
	if (epoch_time - last_epoch < 850)
		return;

	const std::string spam = sayings.at(rand() % sayings.size());

	say_in_chat(spam.c_str());

	last_epoch = epoch_time;
}

