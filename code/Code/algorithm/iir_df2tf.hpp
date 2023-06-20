#ifndef MATLAB_IIR_IIR_TF_HPP
#define MATLAB_IIR_IIR_TF_HPP
// 传递函数形式的iir滤波器，亦可作为传递函数的实现
// N_order是滤波器阶数
template<class float_t, int N_order>
class iir_df2tf {
public:
    iir_df2tf() = default;
    /*
     * brief 示例
     *  sys = c2d(tf(1,[1 2]),0.001); % matlab
     *  auto iir = iir_df2tf<float,2>( // c++
     *                  cell2mat(sys.Numerator),cell2mat(sys.Denominator) % matlab
     *  ); // c++
     *
     */
    template<int NN, int ND>
    iir_df2tf(
            const float_t (&Numerator)[NN],
            const float_t (&Denominator)[ND]) {
        static_assert(NN == N_order, "Numerator尺寸和阶数要一致");
        static_assert(ND == N_order, "Denominator尺寸和阶数要一致");
        for (int i = 0; i < N_order; i++) {
            Den[i] = Denominator[i];
            Num[i] = Numerator[i];
        }
        for (int i = 0; i < N_order - 1; i++) {
            Buff[i] = 0;
        }
    }

    inline float_t update(float in) {
        // direct-form II 转置算法 详见https://ww2.mathworks.cn/help/matlab/ref/filter.html
        Output = Num[0] * in + Buff[0];
        for (int i = 1; i <= N_order - 2; i++) {
            Buff[i - 1] = Num[i] * in + Buff[i] - Den[i] * Output;
        }
        Buff[N_order - 2] = Num[N_order - 1] * in - Den[N_order - 1] * Output;
        return Output;
    }
public:
    float_t Output;//当此更新的输出
private:
    float_t Den[N_order]; // Den[0]固定为1
    float_t Num[N_order];
    float_t Buff[N_order - 1];
};

#endif // MATLAB_IIR_IIR_TF_HPP
