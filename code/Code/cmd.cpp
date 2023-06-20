#include "cmd.h"
#include "drv/arg_parser.h"
#include "shell_cpp.h"
#include "ctrl.hpp"
#include "board.hpp"
#include "drv/par_agent.hpp"
//#include "drv/drv_flash.h"
#include "lfs.h"
#include "board.hpp"


int algo_switch(int argn, const char **argv) {
    arg::parser ps(argn, argv);
    arg::exit ext;
    const char *help = "\r\n用法: %s <algo> 将运行的控制算法切换到algo";
    if (ps.size() <= 1) {
        ext.append(help, ps[0].c_str());
        return algo_list(argn, argv);
    } else {
        if (ctrl.AlgorithmSwitch(ps[1].c_str()))
            printf("已切换到%s (%s)\r\n", ps[1].c_str(), control::pair::find(ps[1].c_str())->helpInfo);
        else {
            printf("没有%s这个算法\r\n", ps[1].c_str());
            return algo_list(argn, argv);
        }
    }
    return 0;
}
SHELL_EXPORT_CMD(
        SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN),
        algo_switch, algo_switch, 切换到指定的控制算法);

int algo_list(int argn, const char **argv) {
    arg::parser ps(argn, argv);
    arg::exit ext;
    printf("支持的控制算法\r\n");
    const control::pair *currentPair = nullptr;
    for (auto i = control::pair::begin(); i != control::pair::end(); ++i) {
        printf("%s\t%s\r\n", i->intanceStr, i->helpInfo);
        if (ctrl.m_algorithmCurrent == i->intance)
            currentPair = i;
    }
    if (currentPair != nullptr)
        printf("\r\n当前的控制算法为%s (%s)\r\n", currentPair->intanceStr, currentPair->helpInfo);
    return 0;
}
SHELL_EXPORT_CMD(
        SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN),
        algo_list, algo_list, 列出支持的控制算法);

int cmd_dac(int argn, const char **argv) {
    arg::parser ps(argn, argv);
    arg::exit ext;
    const char *help = "\r\n用法: %s <channel> <val> 设置dac某通道的输出";
    if (ps.size() == 3) {
        int chnnl = std::stoi(ps[1]);
        int val = std::stoi(ps[2]);
        chnnl = std::max(chnnl, 0);
        chnnl = std::min(chnnl, int(16 - 1));
        val = std::max(val, -32767);
        val = std::min(val, 32767);
        brd.dac.Convert(chnnl, val);
        printf("dac[%d] = %d\r\n", chnnl, val);
    } else {
        ext.append(help, ps[0].c_str());
    }
    return 0;
}
SHELL_EXPORT_CMD(
        SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN),
        dac, cmd_dac, dac测试);


int cmd_time_get(int argn, const char **argv) {
    arg::parser ps(argn, argv);
    arg::exit ext;

    printf("具体含义请查询代码\r\n");
    auto base = brd.htim_counter_data[0];
    for (int i = 1; i < sizeof(board::htim_counter_data) / sizeof(uint32_t); ++i) {
        if (brd.htim_counter_data[i] == 0)
            break;
        auto us = (brd.htim_counter_data[i] - base);
        printf("[%d] = %luus \r\n", i, us);
    }
    return 0;
}
SHELL_EXPORT_CMD(
        SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN),
        time, cmd_time_get, 查看代码执行时间);

int board_rst(int argn, const char **argv) {
    printf("系统复位");
    __set_FAULTMASK(1);
    __NVIC_SystemReset();
    return 0;
}
SHELL_EXPORT_CMD(
        SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN),
        rst, board_rst, 复位);


int par_set(int argn, const char **argv) {
    arg::parser ps(argn, argv);
    arg::exit ext;
    const char *help = "\r\n用法: %s <type> <id> {str} 设置type类型的对象id的值为{str}";
    if (ps.size() > 3 && !par_agent::get_instance().find(ps[2], ps[1]).empty()) {
        std::string str = ps[3];
        for (int i = 4; i < ps.size(); ++i) {
            str += '\t';
            str += ps[i];
        }
        for (auto &i: par_agent::get_instance().find(ps[2], ps[1])) {
            i->scan(str);
        }
    } else {
        ext.append(help, ps[0].c_str());
    }
    return 0;
}
SHELL_EXPORT_CMD(
        SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN),
        par_set, par_set, 设置参数);


int par_load(int argn, const char **argv) {
    par_agent::get_instance().load();
    return 0;
}
SHELL_EXPORT_CMD(
        SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN),
        par_load, par_load, 从存储器读取参数);

int par_save(int argn, const char **argv) {
    par_agent::get_instance().save();
    return 0;
}
SHELL_EXPORT_CMD(
        SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN),
        par_save, par_save, 保存参数到存储器);

int par_ls(int argn, const char **argv) {
    arg::parser ps(argn, argv);
    arg::exit ext;
    const char *help = "\r\n用法: %s <type> 查询type类型对象值为,type=all时显示所有";
    if (ps.size() == 1 || ps.size() == 2)
        for (auto &type: par_agent::get_instance().getTypeList()) {
            if (ps.size() == 1 || ps[1] == "all" || ps[1] == type) {
                printf("以下是class %s的参数\r\n", type.c_str());
                for (auto &i: par_agent::get_instance().findByType(type)) {
                    printf("%s\t%s\r\n", i->m_id.c_str(), i->str().c_str());
                }
                printf("\r\n");
            }
        }
    else
        ext.append(help, ps[0].c_str());
    return 0;
}
SHELL_EXPORT_CMD(
        SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN),
        par_ls, par_ls, 列出当前所有参数);

extern lfs_t *hlfs;
extern "C" void get_file_cfg(struct lfs_file_config *);
int file_cat(int argn, const char **argv) {
    arg::parser ps(argn, argv);
    arg::exit ext;
    lfs_file_t lff;
    struct lfs_file_config fcfg;
    get_file_cfg(&fcfg);
    for (auto &file: par_agent::get_instance().getTypeList()) {
//        if (0 == lfs_file_open(hlfs, &lff, file.c_str(), LFS_O_RDWR)) {
        if (0 == lfs_file_opencfg(hlfs, &lff, file.c_str(), LFS_O_RDWR, &fcfg)) {
            printf("%s的文件大小为%d\r\n", file.c_str(), (int) lfs_file_size(hlfs, &lff));
            char buf;
            while (lfs_file_read(hlfs, &lff, &buf, 1) > 0) {
                printf("%c", buf);
            }
            lfs_file_close(hlfs, &lff);
            printf("\r\n");
        }
    }
    return 0;
}
SHELL_EXPORT_CMD(
        SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN),
        file_cat, file_cat, 列出存储器中的所有参数);

extern "C" void get_file_cfg(struct lfs_file_config *);
int file_del(int argn, const char **argv) {
    arg::parser ps(argn, argv);
    arg::exit ext;
    lfs_file_t lff;
    struct lfs_file_config fcfg;
    get_file_cfg(&fcfg);
    for (auto &file: par_agent::get_instance().getTypeList()) {
//        if (0 == lfs_file_open(hlfs, &lff, file.c_str(), LFS_O_RDWR | LFS_O_CREAT | LFS_O_TRUNC)) {
        if (0 == lfs_file_opencfg(hlfs, &lff, file.c_str(), LFS_O_RDWR | LFS_O_CREAT | LFS_O_TRUNC, &fcfg)) {
            lfs_file_close(hlfs, &lff);
        }
    }
    return 0;
}
SHELL_EXPORT_CMD(
        SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN),
        file_del, file_del, 删除存储器中的参数);


int motor_on(int argn, const char **argv) {
    for (auto &i: brd.MD) {
        i = 1;
    }
    return 0;
}
SHELL_EXPORT_CMD(
        SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN),
        motor_on, motor_on, 电机开);

int motor_off(int argn, const char **argv) {
    for (auto &i: brd.MD) {
        i = 0;
    }
    return 0;
}
SHELL_EXPORT_CMD(
        SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN),
        motor_off, motor_off, 电机关);

int motor_output(int argn, const char **argv) {
    arg::parser ps(argn, argv);
    arg::exit ext;
    const char *help = "\r\n用法: %s <channel> <val(N)> 设置电机的出力，单位N";
    if (ps.size() == 3) {
        int chnnl = std::stoi(ps[1]);
        float val = std::stof(ps[2]);
        chnnl = std::max(chnnl, 0);
        chnnl = std::min(chnnl, 3);
        val = std::max(val, -1.0f);
        val = std::min(val, 1.0f);
        ctrl.set_motor_chnnl_output(chnnl, val);
        printf("ctrl.ta115_LAC08[%d] = %f\r\n", chnnl, val);
    } else {
        ext.append(help, ps[0].c_str());
    }
    return 0;
}
SHELL_EXPORT_CMD(
        SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN),
        motor_output, motor_output, 设置电机的出力);

int motor_xyz_output(int argn, const char **argv) {
    arg::parser ps(argn, argv);
    arg::exit ext;
    const char *help = "\r\n用法: %s <channel> <val(N)> 设置电机的出力，单位N";
    if (ps.size() == 3) {
        int chnnl = std::stoi(ps[1]);
        float val = std::stof(ps[2]);
        chnnl = std::max(chnnl, 0);
        chnnl = std::min(chnnl, 2);
        val = std::max(val, -1.0f);
        val = std::min(val, 1.0f);
        ctrl.set_motor_xyz_output(chnnl, val);
        printf("ctrl.ta115_LAC08_xyz[%d] = %f\r\n", chnnl, val);
    } else {
        ext.append(help, ps[0].c_str());
    }
    return 0;
}
SHELL_EXPORT_CMD(
        SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN),
        motor_xyz_output, motor_xyz_output, 设置电机xyz向的出力);

int sensor_zero(int argn, const char **argv) {
    ctrl.sensor_zero();
    return 0;
}
SHELL_EXPORT_CMD(
        SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN),
        sensor_zero, sensor_zero, 设置当前一段时间的平均值作为传感器的直流偏置);

int sensor_zero_941b(int argn, const char **argv) {
    ctrl.sensor_zero_for_941b();
    return 0;
}
SHELL_EXPORT_CMD(
        SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN),
        sensor_zero_941b, sensor_zero_941b, 设置当前一段时间的平均值作为传感器的直流偏置);

int motor_zero(int argn, const char **argv) {
    ctrl.motor_zero();
    return 0;
}

SHELL_EXPORT_CMD(
        SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN),
        motor_zero, motor_zero, 设置当前一段时间的平均值作为电机的直流偏置);


int pid_set(int argn, const char **argv) {
    arg::parser ps(argn, argv);
    arg::exit ext;
    const char *help = "\r\n用法: %s <id> {str} 设置pid的值为{str}";
    if (ps.size() == 5 && !par_agent::get_instance().find(ps[1], "pid").empty()) {
        std::string str = ps[2];
        for (int i = 3; i < ps.size(); ++i) {
            str += '\t';
            str += ps[i];
        }
        for (auto &i: par_agent::get_instance().find(ps[1], "pid")) {
            i->scan(str);
        }
    } else {
        ext.append(help, ps[0].c_str());
    }
    return 0;
}
SHELL_EXPORT_CMD(
        SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN),
        pid_set, pid_set, 设置PID);

int pid_list(int argn, const char **argv) {
    const char *x[] = {
            "par_ls",
            "pid"
    };
    return par_ls(2, x);
}

SHELL_EXPORT_CMD(
        SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN),
        pid_list, pid_list, 查询pid参数);
