#pragma once
#include "valve_sdk/csgostructs.hpp"
#include <array>

struct Frame {
	std::array<float, 2> viewangles;
	float forwardmove;
	float sidemove;
	float upmove;
	int buttons;
	unsigned char impulse;
	short mousedx;
	short mousedy;

	Frame(CUserCmd* cmd) {
		this->viewangles[0] = cmd->viewangles.pitch;
		this->viewangles[1] = cmd->viewangles.yaw;
		this->forwardmove = cmd->forwardmove;
		this->sidemove = cmd->sidemove;
		this->upmove = cmd->upmove;
		this->buttons = cmd->buttons;
		this->impulse = cmd->impulse;
		this->mousedx = cmd->mousedx;
		this->mousedy = cmd->mousedy;
	}

	void Replay(CUserCmd* cmd) {
		cmd->viewangles.pitch = this->viewangles[0];
		cmd->viewangles.yaw = this->viewangles[1];
		cmd->forwardmove = this->forwardmove;
		cmd->sidemove = this->sidemove;
		cmd->upmove = this->upmove;
		cmd->buttons = this->buttons;
		cmd->impulse = this->impulse;
		cmd->mousedx = this->mousedx;
		cmd->mousedy = this->mousedy;
	}
};

typedef std::vector<Frame> frame_container;

class Recorder {
public:
	void start_recording() 
	{
		this->is_recording_active = true;
	}

	void stop_recording() 
	{
		this->is_recording_active = false;
	}

	bool recording_active() const 
	{
		return this->is_recording_active;
	}

	frame_container& get_active_recording() 
	{
		return this->recording_frames;
	}

	void start_recording(CUserCmd * cmd)
	{
		frame_container& rerecording = get_active_rerecording();
		if (is_recording_active)
			get_active_recording().push_back(cmd);
		else if (is_rerecording_active)
			rerecording.push_back(cmd);
	};

	void start_rerecording(size_t start_frame) {
		this->is_rerecording_active = true;
		this->rerecording_start_frame = start_frame;
	}

	void stop_rerecording(bool merge = false) {
		if (merge) {
			this->recording_frames.erase(this->recording_frames.begin() + (this->rerecording_start_frame + 1), this->recording_frames.end());
			this->recording_frames.reserve(this->recording_frames.size() + this->rerecording_frames.size());
			this->recording_frames.insert(this->recording_frames.end(), this->rerecording_frames.begin(), this->rerecording_frames.end());
		}

		this->is_rerecording_active = false;
		this->rerecording_start_frame = 0;
		this->rerecording_frames.clear();
	}

	bool rerecording_active() const {
		return this->is_rerecording_active;
	}

	frame_container& get_active_rerecording() {
		return this->rerecording_frames;
	}


private:
	bool is_recording_active = false;
	bool is_rerecording_active = false;

	size_t rerecording_start_frame;


	frame_container recording_frames;
	frame_container rerecording_frames;
};
extern Recorder r;


class Playback {
private:
	bool is_playback_active = false;
	frame_container& active_demo = frame_container();
	size_t current_frame = 0;
public:

	void start_playback(frame_container& frames)
	{
		this->is_playback_active = true;
		this->active_demo = frames;
	};

	void stop_playback()
	{
		this->is_playback_active = false;
		this->current_frame = 0;
	};

	bool playback_active() const 
	{
		return this->is_playback_active;
	}

	size_t get_current_frame() const
	{
		return this->current_frame;
	};

	void set_current_frame(size_t frame)
	{
		this->current_frame = frame;
	};

	frame_container& get_active_demo() const
	{
		return this->active_demo;
	}

	void start_playback(CUserCmd * cmd)
	{
		if (!is_playback_active)
			return;

		const size_t current_frame = get_current_frame();

		r.get_active_recording().at(current_frame).Replay(cmd);
		g_EngineClient->SetViewAngles(&cmd->viewangles);

		if (current_frame + 1 >= r.get_active_recording().size())
		{
			stop_playback();
			return;
		}

		set_current_frame(current_frame + 1);
	};
};

extern Playback p;