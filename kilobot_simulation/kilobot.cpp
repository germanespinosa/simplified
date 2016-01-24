#pragma once
#include "kilolib.h"

class mykilobot : public kilobot
{
	unsigned char distance;
	message_t out_message;


	struct mydata {
		unsigned int data1;
		unsigned int data2;
	};


	void mykilobot::loop()
	{
		set_motors(kilo_straight_left, kilo_straight_right);
		set_color(RGB((double)out_message.data[0] / 255, (double)out_message.data[1] / 255, (double)out_message.data[2] / 255));

	}

	void mykilobot::setup()
	{
		out_message.type = NORMAL;
		out_message.data[0] = 55 + rand() * 200 / RAND_MAX;
		out_message.data[1] = 55 + rand() * 200 / RAND_MAX;
		out_message.data[2] = 55 + rand() * 200 / RAND_MAX;
		out_message.crc = message_crc(&out_message);
		spinup_motors();
	}

	void mykilobot::message_tx_success()
	{
		
	}

	message_t *mykilobot::message_tx()
	{
		static int count = rand();
		count--;
		if (!(count % 50))
		{
			return &out_message;
		}
		return NULL;
	}

	void mykilobot::message_rx(message_t *message, distance_measurement_t *distance_measurement)
	{
		distance = estimate_distance(distance_measurement);
		out_message.data[0] = message->data[0];
		out_message.data[1] = message->data[1];
		out_message.data[2] = message->data[2];
	}
};