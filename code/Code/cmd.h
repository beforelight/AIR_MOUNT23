#ifndef CAPTURE_CONTROL_CMD_H
#define CAPTURE_CONTROL_CMD_H
#ifdef __cplusplus
extern "C"{
#endif

int board_rst(int argn, char **argv);
int cmd_time_get(int argn, char **argv);
int algo_switch(int argn, char **argv);
int algo_list(int argn, char **argv);
int cmd_pid_list(int argn, char **argv);
int cmd_pid_set(int argn, char **argv);
int file_list(int argn, char **argv);
int file_cat(int argn, char **argv);
int file_del(int argn, char **argv);
int par_load(int argn, char **argv);
int par_save(int argn, char **argv);
int motor_on(int argn, char **argv);
int motor_off(int argn, char **argv);
int cmd_dac(int argn, char **argv);
int motor_output(int argn, char **argv);
int sensor_zero(int argn, char **argv);
int motor_zero(int argn, char **argv);

#ifdef __cplusplus
};
#endif
#endif // CAPTURE_CONTROL_CMD_H
