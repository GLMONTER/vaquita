#include "main.h"

#include "main.h"
#include"display/lvgl.h"

static bool buttonPressed = false;
static bool buttonToggle = false;

pros::Motor rightFront(10, pros::E_MOTOR_GEARSET_18, true);
pros::Motor rightBack(19, pros::E_MOTOR_GEARSET_18, true);

pros::Motor leftFront(1, pros::E_MOTOR_GEARSET_18, false);
pros::Motor leftBack(12, pros::E_MOTOR_GEARSET_18, false);

pros::Motor leftLift(11, pros::E_MOTOR_GEARSET_36, false);
pros::Motor rightLift(20, pros::E_MOTOR_GEARSET_36, true);

pros::Motor liftRot(14, pros::E_MOTOR_GEARSET_36, true);
pros::Motor liftRot2(15, pros::E_MOTOR_GEARSET_36, false);

pros::Motor loaderRot(16, pros::E_MOTOR_GEARSET_36, true);

pros::ADIDigitalOut sens2('H', false);


pros::Controller controller(pros::E_CONTROLLER_MASTER);

int isTime = 0;
int time = 0;
void controllerPoll()
{
	if(isTime == 45000)
	{
		controller.rumble(". . .");
	}
	else
	if(isTime == 75000)
	{
		controller.rumble("- -");
	}
	isTime += 10;
	time+= 5;
	if(time == 500)
	{
	sens2.set_value(true);
	time = 0;
	}
	else
	sens2.set_value(false);

}

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
	//if no life related input, make the lift hold it's position, (Hold brake mode)
	if(!controller.get_digital(pros::E_CONTROLLER_DIGITAL_L1) && !controller.get_digital(pros::E_CONTROLLER_DIGITAL_R1))
	{
		leftLift.move(0);
		rightLift.move(0);
	}

	if(controller.get_digital(pros::E_CONTROLLER_DIGITAL_A))
	{
		liftRot.move(127);
		liftRot2.move(127);

	}
	else
	if(controller.get_digital(pros::E_CONTROLLER_DIGITAL_B))
	{
		liftRot.move(-127);
		liftRot.move(-127);

	}
	else
	if(!controller.get_digital(pros::E_CONTROLLER_DIGITAL_B) && !controller.get_digital(pros::E_CONTROLLER_DIGITAL_A) && !buttonToggle)
	{
		liftRot.move_velocity(0);
		liftRot2.move_velocity(0);

	}

	//for the loaders
	if(controller.get_digital(pros::E_CONTROLLER_DIGITAL_X))
	{
		loaderRot.move(60);
	}
	else
	if(controller.get_digital(pros::E_CONTROLLER_DIGITAL_Y))
	{
		loaderRot.move(-60);
	}
	else
	if(!controller.get_digital(pros::E_CONTROLLER_DIGITAL_Y) && !controller.get_digital(pros::E_CONTROLLER_DIGITAL_X) && !buttonToggle)
	{
		loaderRot.move_velocity(0);
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
	loaderRot.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);

	while(true)
	{
		rightFront.move(controller.get_analog(pros::E_CONTROLLER_ANALOG_RIGHT_Y));
		rightBack.move(controller.get_analog(pros::E_CONTROLLER_ANALOG_RIGHT_Y));

		leftFront.move(controller.get_analog(pros::E_CONTROLLER_ANALOG_LEFT_Y));
		leftBack.move(controller.get_analog(pros::E_CONTROLLER_ANALOG_LEFT_Y));

		std::string temp  = "Lift Temp : " + std::to_string(liftRot.get_temperature()) +  " C°";
		lv_label_set_text(label, temp.c_str());

		temp  = "Lift2 Temp : " + std::to_string(liftRot2.get_temperature())  +  " C°";
		lv_label_set_text(label2, temp.c_str());
		pollLift();

		if(controller.get_digital(pros::E_CONTROLLER_DIGITAL_L2))
		{
			if(!buttonPressed)
			{
				buttonToggle = 1 - buttonToggle;
				buttonPressed = 1;
				buttonToggle = false;
			}
		}
		else
			buttonPressed = 0;

		if(buttonToggle)
		{
			liftRot.move(controller.get_analog(pros::E_CONTROLLER_ANALOG_LEFT_Y));
		}
		controllerPoll();
		pros::Task::delay(10);
	}
}


/**
 * Runs initialization code. This occurs as soon as the program is started.
 *
 * All other competition modes are blocked by initialize; it is recommended
 * to keep execution time for this mode under a few seconds.
 */
void initialize() {

}

/**
 * Runs while the robot is in the disabled state of Field Management System or
 * the VEX Competition Switch, following either autonomous or opcontrol. When
 * the robot is enabled, this task will exit.
 */
void disabled() {}

/**
 * Runs after initialize(), and before autonomous when connected to the Field
 * Management System or the VEX Competition Switch. This is intended for
 * competition-specific initialization routines, such as an autonomous selector
 * on the LCD.
 *
 * This task will exit when the robot is enabled and autonomous or opcontrol
 * starts.
 */
void competition_initialize() {}

/**
 * Runs the user autonomous code. This function will be started in its own task
 * with the default priority and stack size whenever the robot is enabled via
 * the Field Management System or the VEX Competition Switch in the autonomous
 * mode. Alternatively, this function may be called in initialize or opcontrol
 * for non-competition testing purposes.
 *
 * If the robot is disabled or communications is lost, the autonomous task
 * will be stopped. Re-enabling the robot will restart the task, not re-start it
 * from where it left off.
 */
void autonomous() {}
