#include "main.h"
#include"display/lvgl.h"

pros::Motor rightFront(10, pros::E_MOTOR_GEARSET_18, true);
pros::Motor rightBack(1, pros::E_MOTOR_GEARSET_18, true);

pros::Motor leftFront(14, pros::E_MOTOR_GEARSET_18, false);
pros::Motor leftBack(12, pros::E_MOTOR_GEARSET_18, false);


pros::Motor leftLift(10, pros::E_MOTOR_GEARSET_36, false);
pros::Motor rightLift(15, pros::E_MOTOR_GEARSET_36, true);

pros::Controller con(pros::E_CONTROLLER_MASTER);

//a function that polls the controller for lift related controlls
void pollLife()
{
	//if the left top buttton, move lift down
	if(con.get_digital(pros::E_CONTROLLER_DIGITAL_L1) && !con.get_digital(pros::E_CONTROLLER_DIGITAL_R1))
	{
		leftLift.move(-127);
		rightLift.move(-127);
	}
	else
	//if the right top button, move lift up
	if(!con.get_digital(pros::E_CONTROLLER_DIGITAL_L1) && con.get_digital(pros::E_CONTROLLER_DIGITAL_R1))
	{
		leftLift.move(127);
		rightLift.move(127);
	}
	else
	//if no life related input, make the life hold it's position, (Hold brake mode)
	if(!con.get_digital(pros::E_CONTROLLER_DIGITAL_L1) && !con.get_digital(pros::E_CONTROLLER_DIGITAL_R1))
	{
		leftLift.move_velocity(0);
		rightLift.move_velocity(0);
	}
}

void opcontrol() 
{
	//load the vaquita image from vaquita.c
	extern const lv_img_t vaquita;
	lv_obj_t * im = lv_img_create(lv_scr_act(), NULL);
	lv_img_set_src(im, &vaquita);
	lv_obj_set_pos(im, 0, 0);
	lv_obj_set_drag(im, true);

	//set the lift to hold it's current position when stopped
	leftLift.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
	rightLift.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);

	while(true)
	{
		rightFront.move(con.get_analog(pros::E_CONTROLLER_ANALOG_RIGHT_Y));
		rightBack.move(con.get_analog(pros::E_CONTROLLER_ANALOG_RIGHT_Y));

		leftFront.move(con.get_analog(pros::E_CONTROLLER_ANALOG_LEFT_Y));
		leftBack.move(con.get_analog(pros::E_CONTROLLER_ANALOG_LEFT_Y));

		//se we can update the lvgl thread
		pros::Task::delay(10);
	}
}
