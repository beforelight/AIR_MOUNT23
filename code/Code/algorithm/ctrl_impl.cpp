#include"ctrl_impl.hpp"
#include"eigen_decorate.hpp"
#include"drv/par_agent.hpp"

void control_impl::Convst() {
    /*
     * ADC通道映射
     * (adc通道和dac通道都是本工程中的逻辑编号)
     *
     * adc通道(从1计数) —— SMA板编号 —— 传感器(编号) —— 线缆编号
     */
    // 平台的位移传感器
//    sensor_cd22_raw[0] = control_impl::adc_to_s * float(brd.adc[8]);
//    sensor_cd22_raw[1] = control_impl::adc_to_s * float(brd.adc[9]);
//    sensor_cd22_raw[2] = control_impl::adc_to_s * float(brd.adc[10]);
//    sensor_cd22_raw[3] = control_impl::adc_to_s * float(brd.adc[11]);

    sensor_cd22_lowpass = 0.9995f * sensor_cd22_lowpass + 0.0005f * sensor_cd22_raw;
    sensor_cd22 = sensor_cd22_raw - sensor_cd22_bias;
    sensor_cd22_xyz = Tss_mat * sensor_cd22;

    UPLOAD_DATA_Ix(0) = sensor_cd22[0];
    UPLOAD_DATA_Ix(1) = sensor_cd22[1];
    UPLOAD_DATA_Ix(2) = sensor_cd22[2];
    UPLOAD_DATA_Ix(3) = sensor_cd22[3];

    UPLOAD_DATA_Ix(4) = sensor_cd22_xyz[0];
    UPLOAD_DATA_Ix(5) = sensor_cd22_xyz[1];
    UPLOAD_DATA_Ix(6) = sensor_cd22_xyz[2];

    // 平台的速度传感器
//    sensor_941b_raw[0] = control_impl::adc_to_941b[0] * brd.adc[0];
//    sensor_941b_raw[1] = control_impl::adc_to_941b[1] * brd.adc[1];
//    sensor_941b_raw[2] = control_impl::adc_to_941b[2] * brd.adc[2];
//    sensor_941b_raw[3] = control_impl::adc_to_941b[3] * brd.adc[3];

    sensor_941b_lowpass = 0.9995f * sensor_941b_lowpass + 0.0005f * sensor_941b_raw;
    sensor_941b = sensor_941b_raw - sensor_941b_bias;
    sensor_941b_xyz = Tsv_mat * sensor_941b;

    UPLOAD_DATA_Ix(8) = sensor_941b[0];
    UPLOAD_DATA_Ix(9) = sensor_941b[1];
    UPLOAD_DATA_Ix(10) = sensor_941b[2];
    UPLOAD_DATA_Ix(11) = sensor_941b[3];
    UPLOAD_DATA_Ix(12) = sensor_941b_xyz[0];
    UPLOAD_DATA_Ix(13) = sensor_941b_xyz[1];
    UPLOAD_DATA_Ix(14) = sensor_941b_xyz[2];

    // 基座的速度传感器
//    base_sensor_941b_raw[0] = control_impl::adc_to_base_941b[0] * brd.adc[4];//19613
//    base_sensor_941b_raw[1] = control_impl::adc_to_base_941b[1] * brd.adc[5];//19612
//    base_sensor_941b_raw[2] = control_impl::adc_to_base_941b[2] * brd.adc[6];//19607
//    base_sensor_941b_raw[3] = control_impl::adc_to_base_941b[3] * brd.adc[7];//19608

    base_sensor_941b_lowpass = 0.9995f * base_sensor_941b_lowpass + 0.0005f * base_sensor_941b_raw;
    base_sensor_941b = base_sensor_941b_raw - base_sensor_941b_bias;
    base_sensor_941b_xyz = Tsvg_mat * base_sensor_941b;

    UPLOAD_DATA_Ix(16) = base_sensor_941b[0];
    UPLOAD_DATA_Ix(17) = base_sensor_941b[1];
    UPLOAD_DATA_Ix(18) = base_sensor_941b[2];
    UPLOAD_DATA_Ix(19) = base_sensor_941b[3];
    UPLOAD_DATA_Ix(20) = base_sensor_941b_xyz[0];
    UPLOAD_DATA_Ix(21) = base_sensor_941b_xyz[1];
    UPLOAD_DATA_Ix(22) = base_sensor_941b_xyz[2];
}
void control_impl::Output() {
    //显示输出力矩被离散后的结果
    ta115_LAC08[0] = control_impl::dac_to_f * float(int(control_impl::f_to_dac * ta115_LAC08[0]));
    ta115_LAC08[1] = control_impl::dac_to_f * float(int(control_impl::f_to_dac * ta115_LAC08[1]));
    ta115_LAC08[2] = control_impl::dac_to_f * float(int(control_impl::f_to_dac * ta115_LAC08[2]));
    ta115_LAC08[3] = control_impl::dac_to_f * float(int(control_impl::f_to_dac * ta115_LAC08[3]));
//    UPLOAD_DATA_Ix(24) = ta115_LAC08[0];
//    UPLOAD_DATA_Ix(25) = ta115_LAC08[1];
//    UPLOAD_DATA_Ix(26) = ta115_LAC08[2];
//    UPLOAD_DATA_Ix(27) = ta115_LAC08[3];
//    UPLOAD_DATA_Ix(28) = ta115_LAC08_xyz[0];
//    UPLOAD_DATA_Ix(29) = ta115_LAC08_xyz[1];
//    UPLOAD_DATA_Ix(30) = ta115_LAC08_xyz[2];

    //控制量溢出警告
    if (ta115_LAC08_xyz != ta115_LAC08_xyz) {
        static int count = 0;
        count++;
        if (count > brd.ctrl_freq.val) {
            printf("控制量溢出!!!\r\n");
            count = 0;
        }
        ta115_LAC08_output = ta115_LAC08_bias;
        ta115_LAC08_lowpass = {0, 0, 0, 0};
    } else {
        /*
         * dac通道映射
         *
         * dac通道（从0计数） —— SMA板编号 —— 电机机柜端子号 —— 电机编号 —— 电机线编号
         *
         */
        ta115_LAC08_output = ta115_LAC08 + ta115_LAC08_bias; //
        ta115_LAC08_lowpass = 0.9995f * ta115_LAC08_lowpass + 0.0005f * ta115_LAC08_output;
    }

    // 输出给DAC
    brd.dac[3] = -int(control_impl::f_to_dac * ta115_LAC08_output[0]);
    brd.dac[0] = -int(control_impl::f_to_dac * ta115_LAC08_output[1]);
    brd.dac[15] = -int(control_impl::f_to_dac * ta115_LAC08_output[2]);
    brd.dac[12] = -int(control_impl::f_to_dac * ta115_LAC08_output[3]);
}

void control_impl::Init() {
    par_agent::get_instance().reg(new eigen_decorate(sensor_cd22_bias, "cd22_bias"));
    par_agent::get_instance().reg(new eigen_decorate(ta115_LAC08_bias, "LAC08_bias"));
    par_agent::get_instance().reg(new eigen_decorate(sensor_941b_bias, "941b_bias"));
    par_agent::get_instance().reg(new eigen_decorate(base_sensor_941b_bias, "base_941b_bias"));

//    if (brd.sensor_941b_mode.val >= 4)
//        brd.sensor_941b_mode.val = SENSOR_941B_MODE_DEFAULT;
//    adc_to_941b[0] = (brd.sensor_941b_mode.val <= 1 ? 0.01 : 0.0003125) * 1000.0 * (((SENSOR_941B_ADC_10V_MODE ? 10.0 : 5.0) / 32768.0) / S941B_H19971[brd.sensor_941b_mode.val]);
//    adc_to_941b[1] = (brd.sensor_941b_mode.val <= 1 ? 0.01 : 0.0003125) * 1000.0 * (((SENSOR_941B_ADC_10V_MODE ? 10.0 : 5.0) / 32768.0) / S941B_H19974[brd.sensor_941b_mode.val]);
//    adc_to_941b[2] = (brd.sensor_941b_mode.val <= 1 ? 0.01 : 0.0003125) * 1000.0 * (((SENSOR_941B_ADC_10V_MODE ? 10.0 : 5.0) / 32768.0) / S941B_H19972[brd.sensor_941b_mode.val]);
//    adc_to_941b[3] = (brd.sensor_941b_mode.val <= 1 ? 0.01 : 0.0003125) * 1000.0 * (((SENSOR_941B_ADC_10V_MODE ? 10.0 : 5.0) / 32768.0) / S941B_H19973[brd.sensor_941b_mode.val]);
//
//    adc_to_base_941b[0] = (brd.sensor_941b_mode.val <= 1 ? 0.01 : 0.0003125) * 1000.0 * (((SENSOR_941B_ADC_10V_MODE ? 10.0 : 5.0) / 32768.0) / S941B_H19613[brd.sensor_941b_mode.val]);
//    adc_to_base_941b[1] = (brd.sensor_941b_mode.val <= 1 ? 0.01 : 0.0003125) * 1000.0 * (((SENSOR_941B_ADC_10V_MODE ? 10.0 : 5.0) / 32768.0) / S941B_H19608[brd.sensor_941b_mode.val]);
//    adc_to_base_941b[2] = (brd.sensor_941b_mode.val <= 1 ? 0.01 : 0.0003125) * 1000.0 * (((SENSOR_941B_ADC_10V_MODE ? 10.0 : 5.0) / 32768.0) / S941B_H19607[brd.sensor_941b_mode.val]);
//    adc_to_base_941b[3] = (brd.sensor_941b_mode.val <= 1 ? 0.01 : 0.0003125) * 1000.0 * (((SENSOR_941B_ADC_10V_MODE ? 10.0 : 5.0) / 32768.0) / S941B_H19612[brd.sensor_941b_mode.val]);


}
