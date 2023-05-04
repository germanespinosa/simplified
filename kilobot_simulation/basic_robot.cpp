#pragma once
#include "kilolib.h"

class basic_robot : public kilobot
{

	// behavior

	enum behavior
	{
		wandering,
		seeding,
		gradient
	};

	int behavior = 0;

	//waiting
	int wait = 0;
	int time_scale = 1000;

	//gradient
	int hopcount = 0;

	// end waiting

	// disk info

	int disk_id = -1;
	int disk_size = -1;

	// end disk info

	// movement

	int turn_left = 0;
	int turn_right = 0;

	// end movement

	// communication

	enum msg
	{
		empty,
		recruiting_seed,
		recruiting,
		border
	};

	unsigned char distance;
	message_t out_message;

	void kilobot::setup() {
		out_message.type = NORMAL;
		out_message.data[0] = 0;
		out_message.data[1] = 0;
		out_message.data[2] = 0;
		out_message.crc = message_crc(&out_message);
		spinup_motors();
		// put your setup code here, to be run only once
	}

	message_t *kilobot::message_tx()
	{
		static int count = 0;
		count++;
		if (!(count % 20) && out_message.data[0])
		{
			return &out_message;
		}
		return 0;
	}

	void kilobot::message_rx(message_t *message, distance_measurement_t *distance_measurement)
	{
		distance = estimate_distance(distance_measurement);
		if (distance < 40 && behavior == 0)
		{
			turn_left = turn_right = 0;
			switch (message->data[0])
			{
			case 1:
			{
				behavior = 1;
				disk_id = message->data[1];
				disk_size = message->data[2];
				wait = message->data[3];
				break;
			}
			case 2:
			{
				behavior = 2;
				disk_id = message->data[1];
				hopcount = message->data[2];
				break;
			}
			}
		}
	}

	void recruit()
	{
		if (hopcount)
		{
			out_message.data[0] = 2;
			out_message.data[1] = disk_id;
			out_message.data[2] = hopcount - 1;
		}
	}

	void seed()
	{
		static int steps = 0;
		steps++;
		if (steps < wait * time_scale) // waiting
		{
			return;
		}
		if (!(steps % time_scale) && hopcount<disk_size)
		{
			hopcount++;
		}
		out_message.data[0] = 2;
		out_message.data[1] = disk_id;
		out_message.data[2] = hopcount;
	}

	void randomize_behavior()
	{
		static int steps = 0;
		if (kilo_ticks>=steps)
		{
			spinup_motors();
			int dir = 1 + rand_soft() * 3 / 255;
			int dur = 32;
			turn_left = (dir & 1) * kilo_turn_left;
			turn_right = (dir & 2) / 2 * kilo_turn_right;
			if (dir == 3)
				dur = 128;
			steps = kilo_ticks + dur + rand_soft() * 50 / 255;
		}
	}

	void kilobot::loop()
	{
		out_message.data[0] = 0;
		set_color(RGB(1, 1, 1));
		switch (behavior)
		{
		case 0:
		{
			set_color(RGB(0, 1, 0));
			randomize_behavior();
			break;
		}
		case 1:
		{
			set_color(RGB(1, 0, 0));
			seed();
			break;
		}
		case 2:
		{
			set_color(RGB(0, 0, 1));
			recruit();
			break;
		}
		default:
			break;
		}
		set_motors(turn_left, turn_right);
	}

	void kilobot::message_tx_success()
	{

	}
};