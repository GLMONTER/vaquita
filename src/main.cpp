#include "main.h"
#include"display/lvgl.h"

//button toggle variables
static bool buttonPressed = false;
static bool buttonToggle = false;

//MOTORS
const pros::Motor rightBack(10, pros::E_MOTOR_GEARSET_18, true);
const pros::Motor rightFront(9, pros::E_MOTOR_GEARSET_18, true);

const pros::Motor leftFront(19, pros::E_MOTOR_GEARSET_18, false);
const pros::Motor leftBack(20, pros::E_MOTOR_GEARSET_18, false);

//lifter
const pros::Motor leftLift(12, pros::E_MOTOR_GEARSET_36, false);
const pros::Motor rightLift(2, pros::E_MOTOR_GEARSET_36, true);

//slide rotation
const pros::Motor slideRot(1, pros::E_MOTOR_GEARSET_36, true);
//loader rotation
const pros::Motor loaderRot(11, pros::E_MOTOR_GEARSET_36, true);

//main controller object
pros::Controller controller(pros::E_CONTROLLER_MASTER);
pros::Controller partner(pros::E_CONTROLLER_PARTNER);

//time variables for rumble sequence
static int isTime = 0;
static int Time = 0;

static void controllerPoll(const bool masterEnable)
{
    if(isTime == 45000)
    {
      if(masterEnable)
        controller.rumble(". . .");
      else
        partner.rumble(". . .");
    }
    else
    if(isTime == 75000)
    {
      if(masterEnable)
        controller.rumble("- -");
      else
        partner.rumble("- -");
    }
    isTime += 10;
    Time += 5;
    if(Time == 500)
    {
      Time = 0;
    }
}

//a function that polls the controller for all mechanism related input like the loaders, slide, and arms.
static void pollLift(const bool masterEnable)
{
  //start of master
  if(masterEnable)
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
    if(!controller.get_digital(pros::E_CONTROLLER_DIGITAL_L2) && !controller.get_digital(pros::E_CONTROLLER_DIGITAL_R2) && !buttonToggle && !controller.get_digital(pros::E_CONTROLLER_DIGITAL_Y))
    {
      if(slideRot.get_position() < 400)
      {
        std::cout<<"test"<<std::endl;
        slideRot.move(0);
      }
      else
        slideRot.move_velocity(0);
    }

    if(controller.get_digital(pros::E_CONTROLLER_DIGITAL_X))
    {
      loaderRot.move(127);
      if(slideRot.get_position() > 100)
      {
        slideRot.move(-127);
      }
    }
    else
    if(controller.get_digital(pros::E_CONTROLLER_DIGITAL_Y))
    {
      loaderRot.move(-127);
      if(loaderRot.get_position() < -700 && (slideRot.get_position() < 800))
      {
        slideRot.move(80);
      }
    }
    else
    if(!controller.get_digital(pros::E_CONTROLLER_DIGITAL_Y) && !controller.get_digital(pros::E_CONTROLLER_DIGITAL_X) && !buttonToggle)
    {
        loaderRot.move_velocity(0);
    }
  }
  //end of master

  //start of partner
  if(!masterEnable)
  {
    static bool slow = false;

    if(partner.get_digital(pros::E_CONTROLLER_DIGITAL_DOWN))
      slow = true;
    else
    if(partner.get_digital(pros::E_CONTROLLER_DIGITAL_UP))
      slow = false;

    //if the left top buttton, move lift down
    if(partner.get_digital(pros::E_CONTROLLER_DIGITAL_R1) && !partner.get_digital(pros::E_CONTROLLER_DIGITAL_L1))
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
    if(!partner.get_digital(pros::E_CONTROLLER_DIGITAL_R1) && partner.get_digital(pros::E_CONTROLLER_DIGITAL_L1))
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
    if(!partner.get_digital(pros::E_CONTROLLER_DIGITAL_L1) && !partner.get_digital(pros::E_CONTROLLER_DIGITAL_R1))
    {
      leftLift.move(0);
      rightLift.move(0);
    }

    if(partner.get_digital(pros::E_CONTROLLER_DIGITAL_L2))
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
    if(partner.get_digital(pros::E_CONTROLLER_DIGITAL_R2))
    {
      slideRot.move(-100);
    }
    else
    if(!partner.get_digital(pros::E_CONTROLLER_DIGITAL_L2) && !partner.get_digital(pros::E_CONTROLLER_DIGITAL_R2) && !buttonToggle && !partner.get_digital(pros::E_CONTROLLER_DIGITAL_Y))
    {
      slideRot.move_velocity(0);
    }

    if(partner.get_digital(pros::E_CONTROLLER_DIGITAL_X))
    {
      loaderRot.move(127);
      if(slideRot.get_position() > 100)
      {
        slideRot.move(-127);
      }
    }
    else
    if(partner.get_digital(pros::E_CONTROLLER_DIGITAL_Y))
    {
        loaderRot.move(-127);
        if(loaderRot.get_position() < -700 && (slideRot.get_position() < 800))
        {
          slideRot.move(80);
        }
    }
    else
    if(!partner.get_digital(pros::E_CONTROLLER_DIGITAL_Y) && !partner.get_digital(pros::E_CONTROLLER_DIGITAL_X) && !buttonToggle)
    {
      loaderRot.move_velocity(0);
    }
  }
  //end of partner
}

void opcontrol()
{
    loaderRot.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);

    lv_obj_t* label = lv_label_create(lv_scr_act(), NULL);
    lv_label_set_text(label, "slide");
    lv_obj_align(label, NULL, LV_ALIGN_CENTER, 0, 0);

    lv_obj_t* labelA = lv_label_create(lv_scr_act(), NULL);
    lv_label_set_text(labelA, "arm");
    lv_obj_align(labelA, NULL, LV_ALIGN_CENTER, 0, 100);

    static bool masterEnable = true;

    while(true)
    {
      lv_label_set_text(label, std::to_string(slideRot.get_position()).c_str());

      if(controller.get_digital(pros::E_CONTROLLER_DIGITAL_LEFT) && partner.is_connected())
      {
        masterEnable = false;
      }
      if(partner.get_digital(pros::E_CONTROLLER_DIGITAL_LEFT) && controller.is_connected())
      {
        masterEnable = true;
      }
      if(masterEnable)
      {
        rightBack.move(controller.get_analog(pros::E_CONTROLLER_ANALOG_RIGHT_Y));
        leftBack.move(controller.get_analog(pros::E_CONTROLLER_ANALOG_LEFT_Y));

        rightFront.move(controller.get_analog(pros::E_CONTROLLER_ANALOG_RIGHT_Y));
        leftFront.move(controller.get_analog(pros::E_CONTROLLER_ANALOG_LEFT_Y));
      }
      else
      {
        rightBack.move(partner.get_analog(pros::E_CONTROLLER_ANALOG_RIGHT_Y));
        leftBack.move(partner.get_analog(pros::E_CONTROLLER_ANALOG_LEFT_Y));

        rightFront.move(partner.get_analog(pros::E_CONTROLLER_ANALOG_RIGHT_Y));
        leftFront.move(partner.get_analog(pros::E_CONTROLLER_ANALOG_LEFT_Y));
      }

        pollLift(masterEnable);

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
        controllerPoll(masterEnable);
        std::string temp  = "slide rot : " + std::to_string(slideRot.get_position());
        lv_label_set_text(label, temp.c_str());
        temp = "arm rot : " + std::to_string(loaderRot.get_position());
        lv_label_set_text(labelA, temp.c_str());


        pros::Task::delay(10);
    }
}

void initialize()
{
  //loaderRot.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
  slideRot.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);

/*
  extern pros::Imu imu;
  imu.reset();
  while(imu.is_calibrating())
  {
    pros::Task::delay(10);
  }
  */
  pros::Task::delay(2000);
}

void disabled() {}

void competition_initialize() {}

extern void externAuton();
void autonomous()
{
  externAuton();
}
