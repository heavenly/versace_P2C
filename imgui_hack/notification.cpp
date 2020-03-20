#include "notification.h"
#include "local_info.h"
#include "visuals_pt.h"

std::vector<Notification> Notifications;

void push_notifications::Update()
{
	int screen_width, screen_height;
	g_EngineClient->GetScreenSize(screen_width, screen_height);

	int i = 0;
	while (i < Notifications.size())
	{
		auto delta = Utils::get_epoch_time() - Notifications[i].time_of_message;
		if (delta < EXPIRY_TIME)
		{
			if (i < 16)
			{
				visuals_pt::draw_string(Notifications[i].clr, Notifications[i].Message.c_str(), 10, (6 * i) + info_lp::spec_list_size);
				info_lp::spec_list_size += 6;
			}

			i++;
		}
		else
			Notifications.erase(Notifications.begin() + i);
	}
}

void push_notifications::Push(std::string Message, Color c)
{
	Notification n;
	n.clr = c;
	n.Message = "[V$] " + Message;
	
	Notifications.push_back(n);
}