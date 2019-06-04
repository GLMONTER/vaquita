#include "main.h"
#include"display/lvgl.h"
/**
 * Runs the operator control code. This function will be started in its own task
 * with the default priority and stack size whenever the robot is enabled via
 * the Field Management System or the VEX Competition Switch in the operator
 * control mode.
 *
 * If no competition control is connected, this function will run immediately
 * following initialize().
 *
 * If the robot is disabled or communications is lost, the
 * operator control task will be stopped. Re-enabling the robot will restart the
 * task, not resume it from where it left off.
 */
pros::Motor rightFront(13, pros::E_MOTOR_GEARSET_18, true);
pros::Motor rightBack(11, pros::E_MOTOR_GEARSET_18, true);

pros::Motor leftFront(14, pros::E_MOTOR_GEARSET_18, false);
pros::Motor leftBack(12, pros::E_MOTOR_GEARSET_18, false);

pros::Controller con(pros::E_CONTROLLER_MASTER);

void opcontrol() 
{
	extern const lv_img_t vaquita;
	std::cout<<vaquita.header.w;
	lv_obj_t * im = lv_img_create(lv_scr_act(), NULL);
	lv_img_set_src(im, &vaquita);
	lv_obj_set_pos(im, 0, 0);
	lv_obj_set_drag(im, true);

	while(true)
	{
		rightFront.move(con.get_analog(pros::E_CONTROLLER_ANALOG_RIGHT_Y));
		rightBack.move(con.get_analog(pros::E_CONTROLLER_ANALOG_RIGHT_Y));

		leftFront.move(con.get_analog(pros::E_CONTROLLER_ANALOG_LEFT_Y));
		leftBack.move(con.get_analog(pros::E_CONTROLLER_ANALOG_LEFT_Y));

		//se we can update the lvgl thread
		pros::Task::delay(10);
	}
}
