#include "main.h"

pros::Imu imu(7);

enum direction
{
  E_left = 1, E_right = -1,
  E_forward = 1, E_backward = -1
};

static void drive(const direction dir, const int speed)
{
  leftBack.move(speed * dir);
  leftFront.move(speed * dir);
  rightBack.move(speed * dir);
  rightFront.move(speed * dir);
}

static void turn(const direction dir, const int speed)
{
  leftBack.move(speed * -dir);
  leftFront.move(speed * -dir);
  rightBack.move(speed * dir);
  rightFront.move(speed * dir);
}

static void stopDrive(const bool brake)
{
  if(!brake)
  {
    rightBack.move(0);
    rightFront.move(0);

    leftBack.move(0);
    leftFront.move(0);
  }
  else
  {
    rightBack.move_velocity(0);
    rightFront.move_velocity(0);

    leftBack.move_velocity(0);
    leftFront.move_velocity(0);
  } 
}
//pid chassis controller.
static auto chassis = ChassisControllerBuilder()
    .withMotors({21, 2}, {12, 11})

    // Green gearset, 4 in wheel diam, 11.5 in wheel track
    .withDimensions(AbstractMotor::gearset::green, {{4_in, 13.25_in}, imev5GreenTPR})
    .withGains(
        {0.0025, 0.0005, 0.0001}, // Distance controller gains
        {0.003, 0, 0.0001}, // Turn controller gains
        {0.00175, 0, 0.0001})  // Angle controller gains (helps drive straight)
        .withOdometry()
    .buildOdometry();

static void strafeLeft(const int speed)
{
  rightFront.move(speed);
  rightBack.move(speed * -1);

  leftFront.move(speed * -1);
  leftBack.move(speed);
}

static void strafeRight(const int speed)
{
  rightFront.move(speed * -1);
  rightBack.move(speed);

  leftFront.move(speed);
  leftBack.move(speed * -1);
}
void startLoaders(const bool reverse)
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

void stopLoaders()
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
  while(slideRot.get_position() > -4800)
  {
    slideRot.move(-100);

    if(slideRot.get_position() < -3500)
    {
      while(slideRot.get_position() > -4800)
      {
          slideRot.move(-80);
      }
    }
  }
  slideRot.move_velocity(0);

  pros::Task::delay(500);

  leftLift.move(-110);
  rightLift.move(-110);

  drive(E_backward, 60);

  pros::Task::delay(800);

  stopDrive(true);
  stopLoaders();
}

static void skills()
{
  drive(E_backward, 127);

  startLoaders(true);
  loaderRot.move(-127);

  pros::Task::delay(750);
  loaderRot.move(127);
  pros::Task::delay(1100);
  loaderRot.move_velocity(0);
  loaderRot.move(-127);
  pros::Task::delay(100);
  loaderRot.move_velocity(0);

  stopDrive(false);

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
    turn(E_right, 72);
  }

  chassis->moveDistance(2.75_ft);

  stopLoaders();

   drive(E_forward, 80);

  pros::Task::delay(600);

  stopDrive(false);

  stack();
  slideRot.move_relative(-3100, 150);
  chassis->moveDistance(-0.50_ft);
  chassis->setMaxVelocity(85);

  while(imu.get_yaw() > -76)
  {
    turn(E_left, 45);
  }

  stopDrive(true);

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
    turn(E_left, 40);
  }

  stopDrive(true);

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
  drive(E_backward, 127);

  loaderRot.move(-127);
  pros::Task::delay(80);
  loaderRot.move_velocity(0);

  startLoaders(true);
  //loaderRot.move(-127);

  pros::Task::delay(750);
  /*
  loaderRot.move(127);
  pros::Task::delay(1100);
  loaderRot.move_velocity(0);
  loaderRot.move(-127);
  pros::Task::delay(100);
  loaderRot.move_velocity(0);
*/
  stopDrive(false);
  stopLoaders();
 
  while(std::abs(imu.get_yaw()) > 2)
  {
    if(imu.get_yaw() < 0)
    {
      leftFront.move(30);
      leftBack.move(30);
    }
    else
    {
      rightFront.move(30);
      rightBack.move(30);
    }
  }
  stopDrive(false);

  chassis->setMaxVelocity(85);

  startLoaders(false);
  loaderRot.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);

  chassis->moveDistance(2.8_ft);
  stopLoaders();
  while(imu.get_yaw() > -25)
  {
    turn(E_left, 40);
  }
  startLoaders(false);

  stopDrive(false);
  chassis->moveDistance(1_ft);
  while(imu.get_yaw() < 0)
  {
    turn(E_right, 40);
  }
  stopLoaders();

  rightBack.set_brake_mode(pros::E_MOTOR_BRAKE_BRAKE);
  rightFront.set_brake_mode(pros::E_MOTOR_BRAKE_BRAKE);

  leftBack.set_brake_mode(pros::E_MOTOR_BRAKE_BRAKE);
  leftFront.set_brake_mode(pros::E_MOTOR_BRAKE_BRAKE);

  while(imu.get_yaw() < 136)
  {
    turn(E_right, 70);
  }

  stopDrive(true);
  pros::Task::delay(300);

  chassis->setMaxVelocity(100);
  leftLift.move(100);
  rightLift.move(100);

  chassis->moveDistance(2.5_ft);

  rightBack.move(80);
  rightFront.move(80);

  pros::Task::delay(400);

  drive(E_forward, 70);

  pros::Task::delay(600);
  stopLoaders();
  pros::Task::delay(250);

  stopDrive(false);
  /*
  strafeRight(100);
  pros::Task::delay(500);
  stopDrive(false);
  */
  stack();
}
static void rightBlue()
{
  drive(E_backward, 127);

  startLoaders(true);
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
  drive(E_backward, 127);

  loaderRot.move(-127);
  pros::Task::delay(80);
  loaderRot.move_velocity(0);

  startLoaders(true);
  //loaderRot.move(-127);

  pros::Task::delay(750);
  /*
  loaderRot.move(127);
  pros::Task::delay(1100);
  loaderRot.move_velocity(0);
  loaderRot.move(-127);
  pros::Task::delay(100);
  loaderRot.move_velocity(0);
*/
  stopDrive(false);
  stopLoaders();

  while(std::abs(imu.get_yaw()) > 2)
  {
    if(imu.get_yaw() < 0)
    {
      leftFront.move(30);
      leftBack.move(30);
    }
    else
    {
      rightFront.move(30);
      rightBack.move(30);
    }
  }
  stopDrive(false);

  chassis->setMaxVelocity(80);

  startLoaders(false);
  loaderRot.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);

  chassis->moveDistance(3.15_ft);
  stopLoaders();
  while(imu.get_yaw() < 25)
  {
    turn(E_right, 40);
  }
  startLoaders(false);

  stopDrive(false);
  chassis->moveDistance(0.75_ft);
  while(imu.get_yaw() > 0)
  {
    turn(E_left, 40);
  }
  stopLoaders();

  while(imu.get_yaw() > -136)
  {
    turn(E_left, 80);
  }

  stopDrive(true);
  pros::Task::delay(300);

  chassis->setMaxVelocity(100);
  leftLift.move(100);
  rightLift.move(100);
  rightBack.set_brake_mode(pros::E_MOTOR_BRAKE_COAST);
  rightFront.set_brake_mode(pros::E_MOTOR_BRAKE_COAST);

  leftBack.set_brake_mode(pros::E_MOTOR_BRAKE_COAST);
  leftFront.set_brake_mode(pros::E_MOTOR_BRAKE_COAST);
  chassis->moveDistance(2.65_ft);

  leftBack.move(70);
  leftFront.move(70);
  pros::delay(200);
  stopDrive(false);
  pros::delay(200);

  drive(E_forward, 85);

  pros::Task::delay(400);
  startLoaders(true);
  pros::delay(450);
  stopLoaders();

  stopDrive(false);
  stack();
}

static void one()
{
  startLoaders(true);
  pros::Task::delay(1700);

  drive(E_forward, 60);

  pros::Task::delay(2000);

  drive(E_backward, 60);

  pros::Task::delay(3000);

  stopDrive(false);

  stopLoaders();
}

void externAuton()
{
  imu.reset();
  pros::Task::delay(2000);
  blue();
}
