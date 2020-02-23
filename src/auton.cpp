#include "main.h"

#define PID

pros::Imu imu(7);
MotorGroup leftDrive({19, 20});
MotorGroup rightDrive({9, 10});

//pid chassis controller.
static auto chassis = ChassisControllerBuilder()
    .withMotors({19, 20}, {9, 10})
    
    // Green gearset, 4 in wheel diam, 11.5 in wheel track
    .withDimensions(AbstractMotor::gearset::green, {{3_in, 7.5_in}, imev5GreenTPR})
    .withGains(
        {0.0025, 0.0005, 0.0001}, // Distance controller gains
        {0.003, 0, 0.0001}, // Turn controller gains
        {0.00175, 0, 0.0001})  // Angle controller gains (helps drive straight)
        .withOdometry()
    .buildOdometry();

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
    slideRot.move(100);
    #endif

    if(slideRot.get_position() > 1300)
    {
      while(slideRot.get_position() < 2950)
      {
        #ifndef PID
          slideRot.move(75);
          #endif
          #ifdef PID
          slideRot.move(80);
          #endif
      }
    }
    pros::Task::delay(270);
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
  loaderRot.move(-127);
  
  pros::Task::delay(750);
  loaderRot.move(127);
  pros::Task::delay(1100);
  loaderRot.move_velocity(0);
  loaderRot.move(-127);
  pros::Task::delay(100);
  loaderRot.move_velocity(0);

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
  
  while(imu.get_yaw() < 36)
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
  slideRot.move_relative(-3100, 150);
  chassis->moveDistance(-0.50_ft);
  chassis->setMaxVelocity(85);

  while(imu.get_yaw() > -76)
  {
    rightBack.move(45);
    rightFront.move(45);

    leftBack.move(-45);
    leftFront.move(-45);
  }

  rightBack.move_velocity(0);
  rightFront.move_velocity(0);
  leftBack.move_velocity(0);
  leftFront.move_velocity(0);

  pros::Task::delay(1000);
  startLoaders(false);

  chassis->moveDistance(1.5_ft);
  chassis->moveDistance(-0.65_ft);
  startLoaders(true);
  pros::Task::delay(400);

  stopLoaders();
//fist
slideRot.move_relative(950, 150);
  loaderRot.move_relative(-2500, 150);

  pros::Task::delay(1000);
  chassis->setMaxVelocity(50);
  chassis->moveDistance(0.8_ft);

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

  slideRot.move_relative(-950, 150);
  loaderRot.move_relative(2500, 150);
  chassis->moveDistance(2.5_ft);
  stopLoaders();
  chassis->moveDistanceAsync(-0.75_ft);
  startLoaders(true);
  pros::Task::delay(300);

  stopLoaders();

  slideRot.move_relative(775, 150);
loaderRot.move_relative(-1775, 150);


  chassis->waitUntilSettled();

  chassis->moveDistance(0.725_ft);

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
  loaderRot.move(-127);
  
  pros::Task::delay(750);
  loaderRot.move(127);
  pros::Task::delay(1100);
  loaderRot.move_velocity(0);
  loaderRot.move(-127);
  pros::Task::delay(100);
  loaderRot.move_velocity(0);

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

  while(imu.get_heading() < 134)
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

  chassis->moveDistance(2_ft);

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
  pros::Task::delay(2000);

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
  loaderRot.move(-127);
  
  pros::Task::delay(750);
  loaderRot.move(127);
  pros::Task::delay(1100);
  loaderRot.move_velocity(0);
  loaderRot.move(-127);
  pros::Task::delay(100);
  loaderRot.move_velocity(0);

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

  while(imu.get_heading() > 232)
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

  chassis->moveDistance(2_ft);

  leftBack.move(85);
  leftFront.move(85);  

  pros::Task::delay(300);

  rightBack.move(60);
  rightFront.move(60);

  leftBack.move(60);
  leftFront.move(60);

  pros::Task::delay(350);
  stopLoaders();
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
static void blueEXPR()
{
  rightBack.move(-127);
  rightFront.move(-127);

  leftBack.move(-127);
  leftFront.move(-127);

  leftLift.move(-127);
  rightLift.move(-127);
  loaderRot.move(-127);
  
  pros::Task::delay(750);
  loaderRot.move(127);
  pros::Task::delay(1100);
  loaderRot.move_velocity(0);
  loaderRot.move(-127);
  pros::Task::delay(100);
  loaderRot.move_velocity(0);

  rightBack.move(0);
  rightFront.move(0);

  leftBack.move(0);
  leftFront.move(0);

  chassis->setMaxVelocity(117);

  leftLift.move(127);
  rightLift.move(127);
  loaderRot.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);

  chassis->moveDistance(3.35_ft);
  chassis->setMaxVelocity(120);

rightBack.move(-50);
    rightFront.move(-50);

    leftBack.move(50);
    leftFront.move(50);
    pros::Task::delay(300);

    rightBack.move_velocity(0);
  rightFront.move_velocity(0);

  leftBack.move_velocity(0);
  leftFront.move_velocity(0);
  //chassis->turnAngle(-20_deg);

  chassis->setMaxVelocity(90);

  chassis->moveDistance(0.5_ft);
  chassis->setMaxVelocity(120);
rightBack.move(50);
    rightFront.move(50);

    leftBack.move(-50);
    leftFront.move(-50);
    pros::Task::delay(400);

    rightBack.move_velocity(0);
  rightFront.move_velocity(0);

  leftBack.move_velocity(0);
  leftFront.move_velocity(0);
  //chassis->turnAngle(29_deg);
  chassis->setMaxVelocity(90);

  chassis->moveDistance(0.65_ft);

  leftLift.move(0);
  rightLift.move(0);

  rightBack.set_brake_mode(pros::E_MOTOR_BRAKE_BRAKE);
  rightFront.set_brake_mode(pros::E_MOTOR_BRAKE_BRAKE);

  leftBack.set_brake_mode(pros::E_MOTOR_BRAKE_BRAKE);
  leftFront.set_brake_mode(pros::E_MOTOR_BRAKE_BRAKE);

  rightBack.move(-105);
  rightFront.move(-105);

  leftBack.move(-105);
  leftFront.move(-105);

  pros::Task::delay(650);
  while(imu.get_yaw() > -100)
  {
    rightBack.move(75);
    rightFront.move(75);

    leftBack.move(-75);
    leftFront.move(-75);
  }
    startLoaders(false);

  rightBack.move_velocity(0);
  rightFront.move_velocity(0);

  leftBack.move_velocity(0);
  leftFront.move_velocity(0);
  pros::Task::delay(250);

  rightBack.move(112);
  rightFront.move(112);

  leftBack.move(112);
  leftFront.move(112);
  
  pros::Task::delay(1200);
  
  rightBack.move(0);
  rightFront.move(0);

  leftBack.move(0);
  leftFront.move(0);
  leftLift.move(0);
  rightLift.move(0);
  stack();
}
static void redEXPR()
{
  rightBack.move(-127);
  rightFront.move(-127);

  leftBack.move(-127);
  leftFront.move(-127);

  leftLift.move(-127);
  rightLift.move(-127);
  loaderRot.move(-127);
  
  pros::Task::delay(750);
  loaderRot.move(127);
  pros::Task::delay(1100);
  loaderRot.move_velocity(0);
  loaderRot.move(-127);
  pros::Task::delay(100);
  loaderRot.move_velocity(0);

  rightBack.move(0);
  rightFront.move(0);

  leftBack.move(0);
  leftFront.move(0);

  chassis->setMaxVelocity(115);

  leftLift.move(127);
  rightLift.move(127);
  loaderRot.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);

  chassis->moveDistance(3.35_ft);
  chassis->setMaxVelocity(120);

rightBack.move(50);
    rightFront.move(50);

    leftBack.move(-50);
    leftFront.move(-50);
    pros::Task::delay(300);

    rightBack.move_velocity(0);
  rightFront.move_velocity(0);

  leftBack.move_velocity(0);
  leftFront.move_velocity(0);
  //chassis->turnAngle(-20_deg);

  chassis->setMaxVelocity(90);

  chassis->moveDistance(0.5_ft);
  chassis->setMaxVelocity(120);
rightBack.move(-50);
    rightFront.move(-50);

    leftBack.move(50);
    leftFront.move(50);
    pros::Task::delay(400);

    rightBack.move_velocity(0);
  rightFront.move_velocity(0);

  leftBack.move_velocity(0);
  leftFront.move_velocity(0);
  //chassis->turnAngle(29_deg);
  chassis->setMaxVelocity(90);

  chassis->moveDistance(0.65_ft);

  leftLift.move(0);
  rightLift.move(0);

  rightBack.set_brake_mode(pros::E_MOTOR_BRAKE_BRAKE);
  rightFront.set_brake_mode(pros::E_MOTOR_BRAKE_BRAKE);

  leftBack.set_brake_mode(pros::E_MOTOR_BRAKE_BRAKE);
  leftFront.set_brake_mode(pros::E_MOTOR_BRAKE_BRAKE);

  rightBack.move(-105);
  rightFront.move(-105);

  leftBack.move(-105);
  leftFront.move(-105);

  pros::Task::delay(650);
  startLoaders(false);
  while(imu.get_yaw() < 95)
  {
    rightBack.move(-75);
    rightFront.move(-75);

    leftBack.move(75);
    leftFront.move(75);
  }

  rightBack.move_velocity(0);
  rightFront.move_velocity(0);

  leftBack.move_velocity(0);
  leftFront.move_velocity(0);
  pros::Task::delay(250);

  rightBack.move(110);
  rightFront.move(110);

  leftBack.move(110);
  leftFront.move(110);
  
  pros::Task::delay(1200);
  
  rightBack.move(0);
  rightFront.move(0);

  leftBack.move(0);
  leftFront.move(0);
  leftLift.move(0);
  rightLift.move(0);
  stack();
}
void externAuton()
{
  blue();
}
