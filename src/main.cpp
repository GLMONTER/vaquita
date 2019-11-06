#include "main.h"
#include"display/lvgl.h"


static bool buttonPressed = false;
static bool buttonToggle = false;

const static pros::Motor rightBack(10, pros::E_MOTOR_GEARSET_18, true);
const static pros::Motor rightFront(9, pros::E_MOTOR_GEARSET_18, true);

const static pros::Motor leftFront(19, pros::E_MOTOR_GEARSET_18, false);
const static pros::Motor leftBack(20, pros::E_MOTOR_GEARSET_18, false);

//lifter
const static pros::Motor leftLift(12, pros::E_MOTOR_GEARSET_36, false);
const static pros::Motor rightLift(2, pros::E_MOTOR_GEARSET_36, true);

//slide mover
const static pros::Motor liftRot(1, pros::E_MOTOR_GEARSET_36, true);
const static pros::Motor loaderRot(11, pros::E_MOTOR_GEARSET_36, true);

static pros::Controller controller(pros::E_CONTROLLER_MASTER);


static ChassisControllerIntegrated chassis = ChassisControllerFactory::create(
  {19, 20}, {-9, -10},
  AbstractMotor::gearset::green,
  {4.25_in, 11.5_in}
);


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
	time += 5;
	if(time == 500)
	{
	time = 0;
	}

}

//a function that polls the controller for lift related controlls
void pollLift()
{
	static bool slow = false;
	if(controller.get_digital(pros::E_CONTROLLER_DIGITAL_DOWN))
		slow = true;
	else
	if(controller.get_digital(pros::E_CONTROLLER_DIGITAL_UP))
		slow = false;

	//if the left top buttton, move lift down
	if(controller.get_digital(pros::E_CONTROLLER_DIGITAL_R1) && !controller.get_digital(pros::E_CONTROLLER_DIGITAL_L1))
	{
		if(!slow)
		{
			leftLift.move(127);
			rightLift.move(127);
		}
		else
		{
			leftLift.move(65);
			rightLift.move(65);
		}
	}
	else
	//if the right top button, move lift up
	if(!controller.get_digital(pros::E_CONTROLLER_DIGITAL_R1) && controller.get_digital(pros::E_CONTROLLER_DIGITAL_L1))
	{
		if(!slow)
		{
			leftLift.move(-127);
			rightLift.move(-127);
		}
		else
		{
			leftLift.move(-65);
			rightLift.move(-65);
		}
	}
	else
	//if no life related input, make the lift hold it's position, (Hold brake mode)
	if(!controller.get_digital(pros::E_CONTROLLER_DIGITAL_L1) && !controller.get_digital(pros::E_CONTROLLER_DIGITAL_R1))
	{
		leftLift.move(0);
		rightLift.move(0);
	}

	if(controller.get_digital(pros::E_CONTROLLER_DIGITAL_L2))
	{
		if(!slow)
			liftRot.move(127);
		else
		{
			liftRot.move(60);
		}

	}
	else
	if(controller.get_digital(pros::E_CONTROLLER_DIGITAL_R2))
	{
		if(!slow)
			liftRot.move(-127);
		else
		{
			liftRot.move(-60);
		}
	}
	else
	if(!controller.get_digital(pros::E_CONTROLLER_DIGITAL_L2) && !controller.get_digital(pros::E_CONTROLLER_DIGITAL_R2) && !buttonToggle)
	{
		liftRot.move_velocity(0);
	}


	if(controller.get_digital(pros::E_CONTROLLER_DIGITAL_X))
	{
		loaderRot.move(127);
	}
	else
	if(controller.get_digital(pros::E_CONTROLLER_DIGITAL_Y))
	{
		loaderRot.move(-127);
	}
	else
	if(!controller.get_digital(pros::E_CONTROLLER_DIGITAL_Y) && !controller.get_digital(pros::E_CONTROLLER_DIGITAL_X) && !buttonToggle)
	{
		loaderRot.move_velocity(0);
	}
}

void set2()
{
	liftRot.tare_position();
	//liftRot.set_zero_position(liftRot.get_position());
	while(liftRot.get_position() >-2600)
	{
		liftRot.move(-60);
	}
	liftRot.move_velocity(0);
	pros::Task::delay(1000);
	chassis.setMaxVelocity(15);

	leftLift.move(-90);
	rightLift.move(-90);

	chassis.moveDistance(-1.5_ft);
	leftLift.move(0);
	rightLift.move(0);

}

void opcontrol()
{
	lv_obj_t* label = lv_label_create(lv_scr_act(), NULL);
	lv_label_set_text(label, "test");
	lv_obj_align(label, NULL, LV_ALIGN_CENTER, 0, 0);

  //lock the loaders down when the program starts.
  loaderRot.move(-127);
  pros::Task::delay(200);
  loaderRot.move_velocity(0);

	while(true)
	{
		rightBack.move(controller.get_analog(pros::E_CONTROLLER_ANALOG_RIGHT_Y));
		leftBack.move(controller.get_analog(pros::E_CONTROLLER_ANALOG_LEFT_Y));

		rightFront.move(controller.get_analog(pros::E_CONTROLLER_ANALOG_RIGHT_Y));
		leftFront.move(controller.get_analog(pros::E_CONTROLLER_ANALOG_LEFT_Y));

		std::string temp  = "Lift Temp : " + std::to_string(liftRot.get_position());
		lv_label_set_text(label, temp.c_str());

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
void initialize()
{
	loaderRot.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
}

void disabled() {}

void competition_initialize() {}

void red()
{
	loaderRot.move(127);
	chassis.setMaxVelocity(50);

	chassis.moveDistanceAsync(10_in);
	pros::Task::delay(1000);
	loaderRot.move(-127);
	pros::Task::delay(1100);
	loaderRot.move_velocity(0);

	leftLift.move(127);
	rightLift.move(127);
	chassis.setMaxVelocity(45);
	chassis.moveDistance(26_in);

	chassis.setMaxVelocity(90);

	chassis.moveDistance(-17_in);
	chassis.turnAngle(135_deg);
	chassis.setMaxVelocity(35);
	chassis.moveDistance(14_in);
	leftLift.move(-80);
	rightLift.move(-80);
	pros::Task::delay(200);
	leftLift.move(0);
	rightLift.move(0);


	set2();
}

void blue()
{
	loaderRot.move(127);
	chassis.setMaxVelocity(50);

	chassis.moveDistanceAsync(10_in);
	pros::Task::delay(1000);
	loaderRot.move(-127);
	pros::Task::delay(1100);
	loaderRot.move_velocity(0);

	leftLift.move(127);
	rightLift.move(127);
	chassis.setMaxVelocity(45);
	chassis.moveDistance(26_in);

	chassis.setMaxVelocity(90);

	chassis.moveDistance(-17_in);
	chassis.turnAngle(-135_deg);
	chassis.setMaxVelocity(35);
	chassis.moveDistance(13_in);
	leftLift.move(-80);
	rightLift.move(-80);
	pros::Task::delay(200);
	leftLift.move(0);
	rightLift.move(0);


	set2();
}
void autonomous()
{
	red();
}
