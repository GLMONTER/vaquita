#include "main.h"
#include"display/lvgl.h"

//button toggle variables
static bool buttonPressed = false;
static bool buttonToggle = false;

//MOTORS
const static pros::Motor rightBack(10, pros::E_MOTOR_GEARSET_18, true);
const static pros::Motor rightFront(9, pros::E_MOTOR_GEARSET_18, true);

const static pros::Motor leftFront(19, pros::E_MOTOR_GEARSET_18, false);
const static pros::Motor leftBack(20, pros::E_MOTOR_GEARSET_18, false);

//lifter
const static pros::Motor leftLift(12, pros::E_MOTOR_GEARSET_36, false);
const static pros::Motor rightLift(2, pros::E_MOTOR_GEARSET_36, true);

//slide rotation
const static pros::Motor slideRot(1, pros::E_MOTOR_GEARSET_36, true);
//loader rotation
const static pros::Motor loaderRot(11, pros::E_MOTOR_GEARSET_36, true);

//main controller object
static pros::Controller controller(pros::E_CONTROLLER_MASTER);


//pid chassis controller.
static auto chassis = ChassisControllerBuilder()
    .withMotors({19, 20}, {9, 10})
    // Green gearset, 4 in wheel diam, 11.5 in wheel track
    .withDimensions(AbstractMotor::gearset::green, {{3_in, 8.5_in}, imev5GreenTPR})
    .build();



//time variables for rumble sequence
static int isTime = 0;
static int Time = 0;

static void controllerPoll()
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
    Time += 5;
    if(Time == 500)
    {
        Time = 0;
    }
}

//a function that polls the controller for all mechanism related input like the loaders, slide, and arms.
static void pollLift()
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
        if(slideRot.get_position() < 1500)
        {
          slideRot.move(127);
        }
        else
        {
          slideRot.move(50);
        }

    }
    else
    if(controller.get_digital(pros::E_CONTROLLER_DIGITAL_R2))
    {
        slideRot.move(-100);
    }
    else
    if(!controller.get_digital(pros::E_CONTROLLER_DIGITAL_L2) && !controller.get_digital(pros::E_CONTROLLER_DIGITAL_R2) && !buttonToggle)
    {
        slideRot.move_velocity(0);
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

void opcontrol()
{
    lv_obj_t* label = lv_label_create(lv_scr_act(), NULL);
    lv_label_set_text(label, "test");
    lv_obj_align(label, NULL, LV_ALIGN_CENTER, 0, 0);

    while(true)
    {
        rightBack.move(controller.get_analog(pros::E_CONTROLLER_ANALOG_RIGHT_Y));
        leftBack.move(controller.get_analog(pros::E_CONTROLLER_ANALOG_LEFT_Y));

        rightFront.move(controller.get_analog(pros::E_CONTROLLER_ANALOG_RIGHT_Y));
        leftFront.move(controller.get_analog(pros::E_CONTROLLER_ANALOG_LEFT_Y));

        std::string temp  = "Slide Temp : " + std::to_string(slideRot.get_position());
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
            slideRot.move(controller.get_analog(pros::E_CONTROLLER_ANALOG_LEFT_Y));
        }
        controllerPoll();
        pros::Task::delay(10);
    }
}

void initialize()
{
    loaderRot.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
    slideRot.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
}

void disabled() {}

void competition_initialize() {}

//aton function for actually stacking the cubes
static void stack()
{
    slideRot.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
    //zero the slide position
    slideRot.tare_position();
    while(slideRot.get_position() < 2800)
    {
        slideRot.move(80);
    }
    slideRot.move_velocity(0);

    pros::Task::delay(500);
    chassis->setMaxVelocity(70);

    leftLift.move(-90);
    rightLift.move(-90);

    chassis->moveDistance(-1.5_ft);
    leftLift.move(0);
    rightLift.move(0);
}

static void skills()
{

}

static void red()
{
  loaderRot.move(-50);
  pros::Task::delay(220);
  loaderRot.move_velocity(0);
  chassis->setMaxVelocity(95);
  leftLift.move(127);
  rightLift.move(127);
  chassis->moveDistance(3.9_ft);

  leftLift.move(0);
  rightLift.move(0);
  chassis->setMaxVelocity(60);

  chassis->turnAngle(138_deg);
  chassis->setMaxVelocity(100);

  chassis->moveDistance(3.1_ft);
  stack();
}

static void redLeft()
{
  loaderRot.move(-50);
  pros::Task::delay(220);
  loaderRot.move_velocity(0);
  chassis->setMaxVelocity(95);
  leftLift.move(127);
  rightLift.move(127);
  chassis->moveDistance(1_ft);
  leftLift.move(0);
  rightLift.move(0);
  chassis->turnAngle(65_deg);
  chassis->setMaxVelocity(150);

  chassis->moveDistance(3.8_ft);
  chassis->setMaxVelocity(110);

  chassis->turnAngle(-160_deg);

  leftLift.move(127);
  rightLift.move(127);
  chassis->setMaxVelocity(150);

  chassis->moveDistance(2.35_ft);

  chassis->turnAngle(-75_deg);

  chassis->moveDistance(3.75_ft);

  stack();
}

static void blue()
{

}

void autonomous()
{

red();

}
