#include "main.h"
//#define SKILLS
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
#define SKILLS

//pid chassis controller.
static auto chassis = ChassisControllerBuilder()
    .withMotors({21, 2}, {12, 1})

    // Green gearset, 4 in wheel diam, 11.5 in wheel track
    .withDimensions(AbstractMotor::gearset::green, {{4_in, 13.25_in}, imev5GreenTPR})
    .withGains(
        {0.0025, 0.0005, 0.0001}, // Distance controller gains
        {0.003, 0, 0.0001}, // Turn controller gains
        {0.0025, 0.001, 0.0001})  // Angle controller gains (helps drive straight)
        .withOdometry()
    .buildOdometry();
    
    /*
#ifdef SKILLS
//pid chassis controller.
static auto chassis = ChassisControllerBuilder()
    .withMotors(21, 12)

    // Green gearset, 4 in wheel diam, 11.5 in wheel track
    .withDimensions(AbstractMotor::gearset::green, {{4_in, 13.25_in}, imev5GreenTPR})
    .withGains(
        {0.0025, 0.0005, 0.0001}, // Distance controller gains
        {0.003, 0, 0.0001}, // Turn controller gains
        {0.0025, 0.001, 0.0001})  // Angle controller gains (helps drive straight)
        .withOdometry()
    .buildOdometry();
#endif
*/
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
  while(slideRot.get_position() > -4750)
  {
    #ifdef SKILLS
    slideRot.move(-85);
    #endif

    #ifndef SKILLS
    slideRot.move(-100);
    #endif

    if(slideRot.get_position() < -3500)
    {
      while(slideRot.get_position() > -4775)
      {
        #ifdef SKILLS
        slideRot.move(-60);
        #endif

        #ifndef SKILLS
        slideRot.move(-70);
        #endif      
    }
    }
  }

  slideRot.move_velocity(0);

  pros::Task::delay(500);

  leftLift.move(-127);
  rightLift.move(-127);

  drive(E_backward, 40);

  pros::Task::delay(800);

  stopDrive(true);
  stopLoaders();
}

static void skills()
{
  slideRot.move_relative(-920, 200);

  //drive(E_backward, 127);

  loaderRot.move(-127);

  startLoaders(true);
    
  pros::Task::delay(1300);

  loaderRot.move(127);
  pros::Task::delay(1000);
  loaderRot.move_velocity(0);

  stopDrive(false);
  stopLoaders();
  slideRot.move_relative(920, 200);

  while(std::abs(imu.get_yaw()) > 1)
  {
    if(imu.get_yaw() < 0)
    {
      leftFront.move(25);
      leftBack.move(25);
    }
    else
    {
      rightFront.move(25);
      rightBack.move(25);
    }
  }
  stopDrive(true);
  startLoaders(false);
  chassis->setMaxVelocity(85);
  chassis->moveDistance(8.35_ft);

  rightBack.set_brake_mode(pros::E_MOTOR_BRAKE_BRAKE);
  rightFront.set_brake_mode(pros::E_MOTOR_BRAKE_BRAKE);

  leftBack.set_brake_mode(pros::E_MOTOR_BRAKE_BRAKE);
  leftFront.set_brake_mode(pros::E_MOTOR_BRAKE_BRAKE);

  while(imu.get_yaw() < 37)
  {
    turn(E_right, 72);
  }

  stopDrive(true);
  drive(E_forward, 80);

  pros::Task::delay(1200);
  stopLoaders();

  drive(E_forward, 80);

  pros::Task::delay(800);

  stopDrive(false);

  stack();
  slideRot.move_relative(4900, 150);
  chassis->moveDistance(-1.1_ft);
  chassis->setMaxVelocity(85);

  while(imu.get_yaw() > -82.5)
  {
    turn(E_left, 56);
  }

  stopDrive(true);

  pros::Task::delay(1000);
  startLoaders(false);
  chassis->setMaxVelocity(50);

  chassis->moveDistance(1.5_ft);
  chassis->moveDistance(-0.65_ft);
  startLoaders(true);
  pros::Task::delay(400);

  stopLoaders();

  slideRot.move_relative(-920, 150);
  loaderRot.move_relative(-2656, 150);

  pros::Task::delay(1000);
  chassis->setMaxVelocity(50);
  chassis->moveDistance(0.8_ft);
  //unload first tower
  startLoaders(true);
  pros::Task::delay(2000);
  stopLoaders();
  chassis->setMaxVelocity(90);

  chassis->moveDistance(-1.5_ft);

  pros::Task::delay(1000);
  slideRot.move_relative(920, 200);
  loaderRot.move_relative(2600, 200);
  while(imu.get_yaw() > -175.5)
  {
    turn(E_left, 55);
  }

  stopDrive(true);

  leftLift.move(60);
  rightLift.move(60);

  
  chassis->moveDistance(2.5_ft);
  stopLoaders();
  chassis->moveDistanceAsync(-0.75_ft);
  

  slideRot.move_relative(-920, 150);
  loaderRot.move_relative(-2000, 150);

  chassis->waitUntilSettled();

  chassis->moveDistance(0.7_ft);
//unload second
  startLoaders(true);
  pros::Task::delay(2000);
  stopLoaders();
    chassis->setMaxVelocity(100);
  slideRot.move_relative(920, 200);
  loaderRot.move_relative(2000, 200);
  pros::delay(400);
  chassis->moveDistance(-1_ft);
  
  while(imu.get_yaw() < 0)
  {
    turn(E_left, 55);
  }
  while(imu.get_yaw() > 145)
  {
    turn(E_left, 55);
  }
  stopDrive(true);
  leftLift.move(70);
  rightLift.move(70);
  chassis->moveDistance(1.80_ft);
  stopLoaders();
  slideRot.move_relative(-920, 150);
  loaderRot.move_relative(-2000, 150);
  while(imu.get_yaw() > 56)
  {
    turn(E_left, 55);
  }
  stopDrive(true);
  chassis->moveDistance(0.9_ft);
  startLoaders(true);
  pros::Task::delay(2000);
  stopLoaders();
}

static void red()
{
  slideRot.move_relative(-920, 200);

  //drive(E_backward, 127);

  loaderRot.move(-127);

  startLoaders(true);
    
  pros::Task::delay(1300);

  loaderRot.move(127);
  pros::Task::delay(1000);
  loaderRot.move_velocity(0);

  stopDrive(false);
  stopLoaders();
  slideRot.move_relative(920, 200);
  while(std::abs(imu.get_yaw()) > 1)
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
  startLoaders(false);
  chassis->setMaxVelocity(80);

  startLoaders(false);
  loaderRot.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);

  chassis->moveDistance(3.4_ft);
  stopLoaders();

  rightBack.set_brake_mode(pros::E_MOTOR_BRAKE_BRAKE);
  rightFront.set_brake_mode(pros::E_MOTOR_BRAKE_BRAKE);

  leftBack.set_brake_mode(pros::E_MOTOR_BRAKE_BRAKE);
  leftFront.set_brake_mode(pros::E_MOTOR_BRAKE_BRAKE);
    chassis->setMaxVelocity(120);

  //chassis->moveDistance(-0.5_ft);
  while(imu.get_yaw() < 123)
  {
    leftFront.move(20);
    leftBack.move(20);

    rightFront.move(-127);
    rightBack.move(-127);
  }
  

  stopLoaders();

  stopDrive(true);
  pros::Task::delay(300);

  
  
  startLoaders(false);

  drive(E_forward, 80);

  pros::Task::delay(1400);
  stopDrive(false);
  pros::delay(600);
  stopLoaders();
  pros::delay(500);
  drive(E_forward, 127);
  pros::delay(800);
  stopDrive(false);

  drive(E_backward, 30);
  pros::delay(200);
  stopDrive(false);

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
slideRot.move_relative(-920, 200);

  //drive(E_backward, 127);

  loaderRot.move(-127);

  startLoaders(true);
    
  pros::Task::delay(1300);

  loaderRot.move(127);
  pros::Task::delay(1000);
  loaderRot.move_velocity(0);

  stopDrive(false);
  stopLoaders();
  slideRot.move_relative(920, 200);
  while(std::abs(imu.get_yaw()) > 1)
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
  startLoaders(false);
  chassis->setMaxVelocity(80);

  startLoaders(false);
  loaderRot.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);

  chassis->moveDistance(3.4_ft);
  stopLoaders();

  rightBack.set_brake_mode(pros::E_MOTOR_BRAKE_BRAKE);
  rightFront.set_brake_mode(pros::E_MOTOR_BRAKE_BRAKE);

  leftBack.set_brake_mode(pros::E_MOTOR_BRAKE_BRAKE);
  leftFront.set_brake_mode(pros::E_MOTOR_BRAKE_BRAKE);
  chassis->setMaxVelocity(120);
  drive(E_backward, 30);
  pros::Task::delay(200);
  stopDrive(false);
  //chassis->moveDistance(-0.5_ft);
  while(imu.get_yaw() > -123)
  {
    rightFront.move(20);
    rightBack.move(20);

    leftFront.move(-127);
    leftBack.move(-127);
  }

  stopLoaders();

  stopDrive(true);
  pros::Task::delay(300);

  startLoaders(false);

  drive(E_forward, 80);

  pros::Task::delay(1400);
  stopDrive(false);
  pros::delay(450);
  stopLoaders();
  pros::delay(500);
  drive(E_forward, 127);
  pros::delay(800);
  stopDrive(false);

  drive(E_backward, 30);
  pros::delay(80);
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
  blue();
}
