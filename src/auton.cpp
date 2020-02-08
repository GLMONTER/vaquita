#include "main.h"



pros::Imu imu(7);
/*
//pid chassis controller.
static auto chassis = ChassisControllerBuilder()
    .withMotors({19, 20}, {9, 10})
    // Green gearset, 4 in wheel diam, 11.5 in wheel track
    .withDimensions(AbstractMotor::gearset::green, {{3_in, 8.5_in}, imev5GreenTPR})
    .withGains(
        {0.015, 0.0022, 0.0001}, // Distance controller gains
        {0.001, 0, 0.0001}, // Turn controller gains
        {0.015, 0.0022, 0.0001})  // Angle controller gains (helps drive straight)
    .build();
    */

    //normal chassis controller.
    static auto chassis = ChassisControllerBuilder()
        .withMotors({19, 20}, {9, 10})
        // Green gearset, 4 in wheel diam, 11.5 in wheel track
        .withDimensions(AbstractMotor::gearset::green, {{3_in, 8.5_in}, imev5GreenTPR})

        .build();


void testing()
{

}

//aton function for actually stacking the cubes
static void stack()
{
  slideRot.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
  //zero the slide position
  slideRot.tare_position();
  while(slideRot.get_position() < 3050)
  {
    slideRot.move(100);

    if(slideRot.get_position() > 1500)
    {
      while(slideRot.get_position() < 3050)
      {
          slideRot.move(80);
      }
    }
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
  pros::Task::delay(1700);

  rightBack.move(0);
  rightFront.move(0);

  leftBack.move(0);
  leftFront.move(0);

  chassis->setMaxVelocity(95);

  leftLift.move(127);
  rightLift.move(127);
  loaderRot.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);

  chassis->moveDistance(3.2_ft);

  leftLift.move(0);
  rightLift.move(0);

  rightBack.set_brake_mode(pros::E_MOTOR_BRAKE_BRAKE);
  rightFront.set_brake_mode(pros::E_MOTOR_BRAKE_BRAKE);

  leftBack.set_brake_mode(pros::E_MOTOR_BRAKE_BRAKE);
  leftFront.set_brake_mode(pros::E_MOTOR_BRAKE_BRAKE);

  while(imu.get_heading() > 20)
  {
    /*
    static int test = 0;
    if(test == 100)
    {
      controller.print(0, 0, "%f", imu.get_heading();
      test = 0;
    }
    else
      test += 10;
      */
    rightBack.move(-55);
    rightFront.move(-55);

    leftBack.move(55);
    leftFront.move(55);
    if(imu.get_heading() < 20)
      break;
  }

  while(imu.get_heading() < 135)
  {
    rightBack.move(-60);
    rightFront.move(-60);

    leftBack.move(60);
    leftFront.move(60);
  }

  rightBack.move_velocity(0);
  rightFront.move_velocity(0);

  leftBack.move_velocity(0);
  leftFront.move_velocity(0);
  pros::Task::delay(300);

  chassis->setMaxVelocity(100);
  leftLift.move(100);
  rightLift.move(100);

  chassis->moveDistance(2.25_ft);

  rightBack.move(80);
  rightFront.move(80);

  pros::Task::delay(400);

  rightBack.move(60);
  rightFront.move(60);

  leftBack.move(60);
  leftFront.move(60);

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

  chassis->setMaxVelocity(70);

  leftLift.move(127);
  rightLift.move(127);
  loaderRot.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);

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

  while(imu.get_heading() > 230)
  {
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

  chassis->moveDistance(1.9_ft);

  leftBack.move(80);
  leftFront.move(80);

  pros::Task::delay(500);

  rightBack.move(60);
  rightFront.move(60);

  leftBack.move(60);
  leftFront.move(60);

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

static void one()
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

void externAuton()
{
  red();
}
