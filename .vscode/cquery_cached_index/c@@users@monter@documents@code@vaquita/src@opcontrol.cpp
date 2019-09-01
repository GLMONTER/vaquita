#include "main.h"
#include"display/lvgl.h"

static bool buttonPressed = false;
static bool buttonToggle = false;


pros::Motor rightFront(10, pros::E_MOTOR_GEARSET_18, true);
pros::Motor rightBack(19, pros::E_MOTOR_GEARSET_18, true);

pros::Motor leftFront(1, pros::E_MOTOR_GEARSET_18, false);
pros::Motor leftBack(12, pros::E_MOTOR_GEARSET_18, false);

pros::Motor leftLift(20, pros::E_MOTOR_GEARSET_36, false);
pros::Motor rightLift(11, pros::E_MOTOR_GEARSET_36, true);

pros::Motor liftRot(21, pros::E_MOTOR_GEARSET_36, true);

pros::Controller controller(pros::E_CONTROLLER_MASTER);


//a function that polls the controller for lift related controlls
void pollLift()
{
	//if the left top buttton, move lift down
	if(controller.get_digital(pros::E_CONTROLLER_DIGITAL_L1) && !controller.get_digital(pros::E_CONTROLLER_DIGITAL_R1))
	{
		leftLift.move(-127);
		rightLift.move(-127);
	}
	else
	//if the right top button, move lift up
	if(!controller.get_digital(pros::E_CONTROLLER_DIGITAL_L1) && controller.get_digital(pros::E_CONTROLLER_DIGITAL_R1))
	{
		leftLift.move(127);
		rightLift.move(127);
	}
	else
	//if no life related input, make the life hold it's position, (Hold brake mode)
	if(!controller.get_digital(pros::E_CONTROLLER_DIGITAL_L1) && !controller.get_digital(pros::E_CONTROLLER_DIGITAL_R1))
	{
		leftLift.move_velocity(0);
		rightLift.move_velocity(0);
	}
	if(controller.get_digital(pros::E_CONTROLLER_DIGITAL_A))
	{
		liftRot.move(60);
	}
	if(controller.get_digital(pros::E_CONTROLLER_DIGITAL_B))
	{
		liftRot.move(-60);
	}
	if(!controller.get_digital(pros::E_CONTROLLER_DIGITAL_B) && !controller.get_digital(pros::E_CONTROLLER_DIGITAL_A) && !buttonToggle)
	{
		liftRot.move_velocity(0);
	}
}

void opcontrol()
{
	//load the vaquita image from vaquita.c
	/*
	extern const lv_img_t vaquita;
	lv_obj_t * im = lv_img_create(lv_scr_act(), NULL);
	lv_img_set_src(im, &vaquita);
	lv_obj_set_pos(im, 0, 0);
	lv_obj_set_drag(im, true);
	*/

	lv_obj_t* label = lv_label_create(lv_scr_act(), NULL);
	lv_label_set_text(label, "test");
	lv_obj_align(label, NULL, LV_ALIGN_CENTER, 0, 0);

	lv_obj_t* label2 = lv_label_create(lv_scr_act(), NULL);
	lv_label_set_text(label2, "test");
	lv_obj_align(label2, NULL, LV_ALIGN_CENTER, 0, 50);

	liftRot.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);

	while(true)
	{
		rightFront.move(controller.get_analog(pros::E_CONTROLLER_ANALOG_RIGHT_Y));
		rightBack.move(controller.get_analog(pros::E_CONTROLLER_ANALOG_RIGHT_Y));

		leftFront.move(controller.get_analog(pros::E_CONTROLLER_ANALOG_LEFT_Y));
		leftBack.move(controller.get_analog(pros::E_CONTROLLER_ANALOG_LEFT_Y));

		std::string temp  = "Left Lift Temp : " + std::to_string(leftLift.get_temperature()) +  " C°";
		lv_label_set_text(label, temp.c_str());

		temp  = "Right Lift Temp : " + std::to_string(rightLift.get_temperature())  +  " C°";
		lv_label_set_text(label2, temp.c_str());
		pollLift();


		//go forward with drum
	if(controller.get_digital(pros::E_CONTROLLER_DIGITAL_L2))
	{
		//if the forward button toggle isn't on then continute
		if(!buttonPressed)
		{
			//actaully flip the toggle, this is why the type has to be int
			buttonToggle = 1 - buttonToggle;
			//changed button pressed to true
			buttonPressed = 1;
			//change the backward toggle to false so we don't try to go backwards and forwards
			buttonToggle = false;
		}
	}
	//switch back to normal buttton state but leave toggle on if button isn't pressed.
	else
		buttonPressed = 0;
	if(buttonToggle)
	{
		liftRot.move(controller.get_analog(pros::E_CONTROLLER_ANALOG_LEFT_Y));
	}


		//se we can update the lvgl thread
		pros::Task::delay(10);
	}
}
