#include "main.h"

#define PID

pros::Imu imu(7);

//pid chassis controller.
static auto chassis = ChassisControllerBuilder()
    .withMotors({19, 20}, {9, 10})
    // Green gearset, 4 in wheel diam, 11.5 in wheel track
    .withDimensions(AbstractMotor::gearset::green, {{3_in, 8.5_in}, imev5GreenTPR})
    .withGains(
        {0.00175, 0, 0.0001}, // Distance controller gains
        {0.002, 0, 0.0001}, // Turn controller gains
        {0.00175, 0, 0.0001})  // Angle controller gains (helps drive straight)
    .build();


    /*
#ifndef PID
    //normal chassis controller.
    static auto chassis = ChassisControllerBuilder()
        .withMotors({19, 20}, {9, 10})
        // Green gearset, 4 in wheel diam, 11.5 in wheel track
        .withDimensions(AbstractMotor::gearset::green, {{3_in, 8.5_in}, imev5GreenTPR})

        .build();
  #endif
*/


static void startLoaders(const bool reverse)
{
  if(!reverse)
  {
    leftLift.move(127);
    rightLift.move(127);
  }
  else
  {
    leftLift.move(-127);
    rightLift.move(-127);
  }
}

static void stopLoaders()
{
  leftLift.move(0);
  rightLift.move(0);
}

//aton function for actually stacking the cubes
static void stack()
{
  slideRot.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
  //zero the slide position
  slideRot.tare_position();
  while(slideRot.get_position() < 3050)
  {
    #ifndef PID
    slideRot.move(95);
    #endif

    #ifdef PID
    slideRot.move(95);
    #endif

    if(slideRot.get_position() > 1300)
    {
      while(slideRot.get_position() < 3050)
      {
        #ifndef PID
          slideRot.move(70);
          #endif
          #ifdef PID
          slideRot.move(70);
          #endif

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

  pros::Task::delay(800);

  rightBack.move_velocity(0);
  rightFront.move_velocity(0);

  leftBack.move_velocity(0);
  leftFront.move_velocity(0);
  leftLift.move(0);
  rightLift.move(0);
}

static void skills()
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

  chassis->moveDistance(8.2_ft);

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

  while(imu.get_heading() < 36.5)
  {
    rightBack.move(-72);
    rightFront.move(-72);

    leftBack.move(72);
    leftFront.move(72);
  }

  chassis->moveDistance(2.75_ft);

  leftLift.move(0);
  rightLift.move(0);

   rightBack.move(80);
    rightFront.move(80);

    leftBack.move(80);
    leftFront.move(80);

    pros::Task::delay(600);

    rightBack.move(0);
    rightFront.move(0);

    leftBack.move(0);
    leftFront.move(0);

  stack();
  slideRot.move_relative(-2900, 150);
  chassis->moveDistance(-0.50_ft);
  chassis->setMaxVelocity(85);

  while(imu.get_yaw() > -76)
  {
    rightBack.move(40);
    rightFront.move(40);

    leftBack.move(-40);
    leftFront.move(-40);
  }

  rightBack.move_velocity(0);
  rightFront.move_velocity(0);
  leftBack.move_velocity(0);
  leftFront.move_velocity(0);

  pros::Task::delay(1000);
  startLoaders(false);

  chassis->moveDistance(1.6_ft);
  chassis->moveDistance(-0.75_ft);
  startLoaders(true);
  pros::Task::delay(400);

  stopLoaders();

  slideRot.move_relative(950, 150);
  loaderRot.move_relative(-2500, 150);
  pros::Task::delay(1000);
  chassis->setMaxVelocity(50);
  chassis->moveDistance(1.1_ft);

  startLoaders(true);
  pros::Task::delay(2000);
  stopLoaders();
  chassis->setMaxVelocity(90);

  chassis->moveDistance(-1.6_ft);

  pros::Task::delay(1000);

  while(imu.get_yaw() > -167)
  {
    rightBack.move(40);
    rightFront.move(40);

    leftBack.move(-40);
    leftFront.move(-40);
  }

  rightBack.move_velocity(0);
  rightFront.move_velocity(0);
  leftBack.move_velocity(0);
  leftFront.move_velocity(0);

  startLoaders(false);
  slideRot.move_relative(-900, 150);
  loaderRot.move_relative(2400, 150);
  chassis->moveDistance(2.5_ft);
  stopLoaders();
  chassis->moveDistanceAsync(-0.75_ft);
  startLoaders(true);
  pros::Task::delay(300);

  stopLoaders();

  slideRot.move_relative(775, 150);
  loaderRot.move_relative(-1775, 150);

  chassis->waitUntilSettled();

  chassis->moveDistance(0.65_ft);

  startLoaders(true);
  pros::Task::delay(2000);
  stopLoaders();

  chassis->moveDistance(-1.5_ft);
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

  chassis->moveDistance(2.1_ft);

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
static void rightBlue()
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

  chassis->setMaxVelocity(85);

  leftLift.move(127);
  rightLift.move(127);
  loaderRot.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);

  chassis->moveDistance(1.5_ft);


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

  while(imu.get_heading() > 285)
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
  pros::Task::delay(300);

  chassis->moveDistance(2.4_ft);

  rightBack.move(60);
  rightFront.move(60);

  leftBack.move(-60);
  leftFront.move(-60);
  pros::Task::delay(250);

  rightBack.move(60);
  rightFront.move(60);

  leftBack.move(60);
  leftFront.move(60);
  pros::Task::delay(400);

  //chassis->turnAngle(-90_deg);
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
  pros::Task::delay(300);

  chassis->setMaxVelocity(100);
  leftLift.move(100);
  rightLift.move(100);

  chassis->moveDistance(2.1_ft);

  leftBack.move(80);
  leftFront.move(80);

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
  skills();
}
