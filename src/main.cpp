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

//old chassis controller
/*
static ChassisControllerIntegrated chassis = ChassisControllerFactory::create(
  {19, 20}, {-9, -10},
  AbstractMotor::gearset::green,
  {3_in, 10.25_in}
);
*/

//pid chassis controller.
static auto chassis = ChassisControllerFactory::create(
  {19, 20}, {-9, -10},
  IterativePosPIDController::Gains{0.015, 0.0022, 0.0001},
  IterativePosPIDController::Gains{0.001, 0, 0.0001},
  IterativePosPIDController::Gains{0.015, 0.0022, 0.0001},
  AbstractMotor::gearset::green,
  {3_in, 10.25_in}
);

//time variables for rumble sequence
static int isTime = 0;
static int time = 0;

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
    time += 5;
    if(time == 500)
    {
        time = 0;
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
        if(!slow)
            slideRot.move(127);
        else
        {
            slideRot.move(60);
        }

    }
    else
    if(controller.get_digital(pros::E_CONTROLLER_DIGITAL_R2))
    {
        if(!slow)
            slideRot.move(-127);
        else
        {
            slideRot.move(-60);
        }
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

        std::string temp  = "Lift Temp : " + std::to_string(slideRot.get_position());
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
}

void disabled() {}

void competition_initialize() {}

//aton function for actually stacking the cubes
static void stack()
{
    slideRot.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
    //zero the slide position
    slideRot.tare_position();
    while(slideRot.get_position() >-2500)
    {
        slideRot.move(80);
    }
    slideRot.move_velocity(0);

    pros::Task::delay(500);
    chassis.setMaxVelocity(15);

    leftLift.move(-90);
    rightLift.move(-90);

    chassis.moveDistance(-1.5_ft);
    leftLift.move(0);
    rightLift.move(0);
}

static void skills()
{

}

static void red()
{
    chassis.setMaxVelocity(30);

    loaderRot.move(127);
    pros::Task::delay(1400);
    loaderRot.move(-127);

    leftBack.move(-127);
    rightBack.move(-127);
    pros::Task::delay(600);
    leftBack.move(0);
    rightBack.move(0);

    pros::Task::delay(800);
    loaderRot.move_velocity(0);

    chassis.setMaxVelocity(60);
    leftLift.move(127);
    rightLift.move(127);
    chassis.moveDistance(40_in);

    chassis.setMaxVelocity(110);
    leftLift.move(0);
    rightLift.move(0);

    chassis.moveDistance(-17_in);
    chassis.setMaxVelocity(80);

    chassis.turnAngle(155_deg);
    chassis.moveDistance(20_in);

    stack();
}

static void blue()
{
    leftLift.move(-127);
    rightLift.move(-127);
    chassis.setMaxVelocity(30);

    loaderRot.move(127);
    pros::Task::delay(1400);
    loaderRot.move(-127);

    leftBack.move(-127);
    rightBack.move(-127);
    pros::Task::delay(600);
    leftBack.move(0);
    rightBack.move(0);

    pros::Task::delay(800);
    loaderRot.move_velocity(0);
    leftLift.move(0);
    rightLift.move(0);



    chassis.setMaxVelocity(90);
    leftLift.move(127);
    rightLift.move(127);
    chassis.moveDistance(8_in);

    chassis.setMaxVelocity(90);
    leftLift.move(0);
    rightLift.move(0);
    chassis.resetSensors();
    chassis.turnAngle(130_deg);

    leftLift.move(127);
    rightLift.move(127);
    chassis.setMaxVelocity(90);
    chassis.moveDistance(18_in);

    chassis.turnAngle(45_deg);
    chassis.moveDistance(18_in);

    stack();
}

static void shitaton()
{
  leftBack.move(127);
  rightBack.move(127);
  pros::Task::delay(1000);

  leftBack.move(-127);
  rightBack.move(-127);
  leftFront.move(-127);
  rightFront.move(-127);

  pros::Task::delay(1000);

  leftBack.move(0);
  rightBack.move(0);
  leftFront.move(0);
  rightFront.move(0);
}
void autonomous()
{
  chassis.setMaxVelocity(90);
  chassis.moveDistance(1_ft);
  pros::Task::delay(500);

}