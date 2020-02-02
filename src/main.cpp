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

pros::Imu imu(7);
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
          slideRot.move(60);
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
    imu.reset();
    while(imu.is_calibrating())
    {
        pros::Task::delay(10);
    }
}

void disabled() {}

void competition_initialize() {}

//aton function for actually stacking the cubes
static void stack()
{
    slideRot.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
    //zero the slide position
    slideRot.tare_position();
    while(slideRot.get_position() < 3050)
    {
        slideRot.move(80);
    }
    slideRot.move_velocity(0);

    pros::Task::delay(500);

    leftLift.move(-110);
    rightLift.move(-110);

    rightBack.move(-60);
    rightFront.move(-60);

    leftBack.move(-60);
    leftFront.move(-60);

    pros::Task::delay(1000);

    rightBack.move_velocity(0);
    rightFront.move_velocity(0);

    leftBack.move_velocity(0);
    leftFront.move_velocity(0);
    leftLift.move(0);
    rightLift.move(0);
}

static void skills()
{

}

static void red()
{
  rightBack.move(-127);
  rightFront.move(-127);

  leftBack.move(-127);
  leftFront.move(-127);


  leftLift.move(-127);
  rightLift.move(-127);
  pros::Task::delay(1100);
  leftLift.move(0);
  rightLift.move(0);
    pros::Task::delay(600);


  rightBack.move(0);
  rightFront.move(0);

  leftBack.move(0);
  leftFront.move(0);

  chassis->setMaxVelocity(95);
  
  loaderRot.move(-50);
  pros::Task::delay(190);
  loaderRot.move_velocity(0);
  
  leftLift.move(127);
  rightLift.move(127);
  chassis->moveDistance(3.45_ft);

  leftLift.move(0);
  rightLift.move(0);

  rightBack.set_brake_mode(pros::E_MOTOR_BRAKE_BRAKE);
  rightFront.set_brake_mode(pros::E_MOTOR_BRAKE_BRAKE);

  leftBack.set_brake_mode(pros::E_MOTOR_BRAKE_BRAKE);
  leftFront.set_brake_mode(pros::E_MOTOR_BRAKE_BRAKE);
  while(imu.get_heading() > 20)
  {
    rightBack.move(-55);
    rightFront.move(-55);

    leftBack.move(55);
    leftFront.move(55);
    if(imu.get_heading() < 20)
      break;
  }

  while(imu.get_heading() < 145)
  {
    static int test = 0;
    if(test == 100)
    {
      controller.print(0, 0, "%f", imu.get_heading());
      test = 0;
    }
    else
      test += 10;

    rightBack.move(-55);
    rightFront.move(-55);

    leftBack.move(55);
    leftFront.move(55);
  }

  rightBack.move_velocity(0);
  rightFront.move_velocity(0);

  leftBack.move_velocity(0);
  leftFront.move_velocity(0);
  pros::Task::delay(200);

  chassis->setMaxVelocity(120);
  leftLift.move(100);
  rightLift.move(100);

  chassis->moveDistance(2.9_ft);
  

    rightBack.move(50);
    rightFront.move(50);

    leftBack.move(-50);
    leftFront.move(-50);

    pros::Task::delay(500);


    rightBack.move(50);
    rightFront.move(50);

    leftBack.move(50);
    leftFront.move(50);

    pros::Task::delay(250);
    leftLift.move(0);
    rightLift.move(0);
    pros::Task::delay(250);


    
    rightBack.move(0);
  rightFront.move(0);

  leftBack.move(0);
  leftFront.move(0);

   
  stack();
}

static void blue()
{
    rightBack.move(-127);
  rightFront.move(-127);

  leftBack.move(-127);
  leftFront.move(-127);


  leftLift.move(-127);
  rightLift.move(-127);
  pros::Task::delay(1700);

  rightBack.move(0);
  rightFront.move(0);

  leftBack.move(0);
  leftFront.move(0);


  chassis->setMaxVelocity(95);
  
  loaderRot.move(-50);
  pros::Task::delay(190);
  loaderRot.move_velocity(0);
  
  leftLift.move(127);
  rightLift.move(127);
  chassis->moveDistance(3.2_ft);

  leftLift.move(0);
  rightLift.move(0);

  rightBack.set_brake_mode(pros::E_MOTOR_BRAKE_BRAKE);
  rightFront.set_brake_mode(pros::E_MOTOR_BRAKE_BRAKE);

  leftBack.set_brake_mode(pros::E_MOTOR_BRAKE_BRAKE);
  leftFront.set_brake_mode(pros::E_MOTOR_BRAKE_BRAKE);
  while(imu.get_heading() < 20)
  {
    rightBack.move(55);
    rightFront.move(55);

    leftBack.move(-55);
    leftFront.move(-55);
    if(imu.get_heading() > 20)
      break;
  }

  while(imu.get_heading() > 220)
  {
    static int test = 0;
    if(test == 100)
    {
      controller.print(0, 0, "%f", imu.get_heading());
      test = 0;
    }
    else
      test += 10;

    rightBack.move(60);
    rightFront.move(60);

    leftBack.move(-60);
    leftFront.move(-60);
  }

  rightBack.move_velocity(0);
  rightFront.move_velocity(0);

  leftBack.move_velocity(0);
  leftFront.move_velocity(0);
  pros::Task::delay(200);

  chassis->setMaxVelocity(120);
  leftLift.move(100);
  rightLift.move(100);

  chassis->moveDistance(2.9_ft);
  

  rightBack.move(-60);
rightFront.move(-60);

    leftBack.move(50);
    leftFront.move(50);

    pros::Task::delay(350);


  rightBack.move(60);
rightFront.move(60);

    leftBack.move(50);
    leftFront.move(50);

    pros::Task::delay(250);
     leftLift.move(0);
  rightLift.move(0);
        pros::Task::delay(250);


    
    rightBack.move(0);
  rightFront.move(0);

  leftBack.move(0);
  leftFront.move(0);

   
  stack();
}

void autonomous()
{
    leftLift.move(-127);
  rightLift.move(-127);
  pros::Task::delay(1700);

    rightBack.move(60);
    rightFront.move(60);

    leftBack.move(60);
    leftFront.move(60);

    pros::Task::delay(2000);

  
    rightBack.move(-60);
    rightFront.move(-60);

    leftBack.move(-60);
    leftFront.move(-60);

        pros::Task::delay(3000);


      rightBack.move(0);
    rightFront.move(0);

    leftBack.move(0);
    leftFront.move(0);

    leftLift.move(0);
  rightLift.move(0);


}
