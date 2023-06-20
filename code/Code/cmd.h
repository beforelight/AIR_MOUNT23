#ifndef CAPTURE_CONTROL_CMD_H
#define CAPTURE_CONTROL_CMD_H
#ifdef __cplusplus
extern "C"{
#endif
int algo_switch(int argn, const char **argv);//切换控制算法
int algo_list(int argn, const char **argv);//列出控制算法
int cmd_dac(int argn, const char **argv);//设置DAC输出
int cmd_time_get(int argn, const char **argv);//获取执行时间
int board_rst(int argn, const char **argv);//复位
int par_set(int argn, const char **argv);
int par_load(int argn, const char **argv);
int par_save(int argn, const char **argv);
int par_ls(int argn, const char **argv);//列出所有参数
int file_cat(int argn, const char **argv);
int file_del(int argn, const char **argv);

int motor_on(int argn, const char **argv);
int motor_off(int argn, const char **argv);
int motor_output(int argn, const char **argv);
int motor_xyz_output(int argn, const char **argv);


int sensor_zero(int argn, const char **argv);
int sensor_zero_941b(int argn, const char **argv);
int motor_zero(int argn, const char **argv);


int pid_list(int argn, const char **argv);
int pid_set(int argn, const char **argv);


#ifdef __cplusplus
};
#endif
#endif // CAPTURE_CONTROL_CMD_H
