#ifndef MATLAB_IIR_IIR_DF2SOS_HPP
#define MATLAB_IIR_IIR_DF2SOS_HPP
/// \brief
/// \tparam float_t
/// \tparam N_order 必须为偶数
template<class float_t, int N_order>
class iir_df2sos {
private:
    struct sos_block {
        sos_block() { clear(); }
        /// \brief
        /// \param arg 参数数组，分别为b0 b1 b2 1 a1 a2
        sos_block(const float_t *arg) {
            b0 = arg[0];
            b1 = arg[1];
            b2 = arg[2];
            a1 = -arg[4];
            a2 = -arg[5];
            clear();
        }

        float_t y[2];//历史记录

        float_t b0, b1, b2;
        float_t a1, a2;//已经优化带了负号了
        void clear() {
            y[0] = 0;
            y[1] = 0;
        }
    };
public:
    iir_df2sos() { clear(); }

    /// \brief 注意sosMatrix和ScaleValues由matlab直接导出
    /// \tparam N
    /// \param sosMatrix
    /// \param ScaleValue 所有ScaleValues的乘积
    template<unsigned int N>
    iir_df2sos(const float_t(&sosMatrix)[N], float_t ScaleValue) {
        static_assert(N == N_order * 3, "检查数组尺寸");
        for (int i = 0; i < N_order / 2; ++i) {
            m_block[i] = sos_block(&sosMatrix[i * 6]);
        }
        m_gain = ScaleValue;
        clear();
    }

    /// \brief 注意sosMatrix和ScaleValues由matlab直接导出
    /// \tparam N
    /// \tparam NSC
    /// \param sosMatrix
    /// \param ScaleValues
    template<unsigned int N, unsigned int NSC>
    iir_df2sos(const float_t(&sosMatrix)[N], const float_t(&ScaleValues)[NSC]) {
        static_assert(N == N_order * 3, "检查数组尺寸");
        for (int i = 0; i < N_order / 2; ++i) {
            m_block[i] = sos_block(&sosMatrix[i * 6]);
        }
        m_gain = ScaleValues[0];
        for (int i = 1; i < NSC; ++i) {
            m_gain *= ScaleValues[i];
        }
        clear();
    }
    float_t update(float_t input) {
        //计算简化的宏定义
        #define OUT_BLOCK0(i, in, u) \
                                (m_block[i].a1 * m_block[i].y[0] + m_block[i].a2 * m_block[i].y[1] \
                                + m_block[i].b0 * in + m_block[i].b1 * u[0] + m_block[i].b2 * u[1])
        #define OUT_BLOCK(i) OUT_BLOCK0(i,m_y0[i-1],m_block[i-1].y)
        #define OUT_BLOCK_B(i) (m_y0[i]=OUT_BLOCK0(i,m_y0[i-1],m_block[i-1].y))

        //计算第一级的输出
        m_y0[0] = OUT_BLOCK0(0, input, m_u);
        //计算后几级的输出
        if constexpr(N_order >= 4)
        { OUT_BLOCK_B(1); }
        if constexpr(N_order >= 6)
        { OUT_BLOCK_B(2); }
        if constexpr(N_order >= 8)
        { OUT_BLOCK_B(3); }
        if constexpr(N_order >= 10)
        { OUT_BLOCK_B(4); }
        if constexpr(N_order >= 12)
        { OUT_BLOCK_B(5); }
        if constexpr(N_order >= 14)
        { OUT_BLOCK_B(6); }
        if constexpr(N_order >= 16)
        { OUT_BLOCK_B(7); }
        if constexpr(N_order >= 18)
        { OUT_BLOCK_B(8); }
        if constexpr(N_order >= 20)
        { OUT_BLOCK_B(9); }

        //计算简化的宏定义
        #define Z_DELAY(u, new_val) do{ \
            u[1] = u[0];                \
            u[0] = new_val;         \
        }while(0)
        #define Z_DELAY_B(i) Z_DELAY(m_block[i].y, m_y0[i])
        //计算第一级的差分
        Z_DELAY(m_u, input);
        Z_DELAY(m_block[0].y, m_y0[0]);
        //计算后几级的差分
        if constexpr(N_order >= 4)
        { Z_DELAY_B(1); }
        if constexpr(N_order >= 6)
        { Z_DELAY_B(2); }
        if constexpr(N_order >= 8)
        { Z_DELAY_B(3); }
        if constexpr(N_order >= 10)
        { Z_DELAY_B(4); }
        if constexpr(N_order >= 12)
        { Z_DELAY_B(5); }
        if constexpr(N_order >= 14)
        { Z_DELAY_B(6); }
        if constexpr(N_order >= 16)
        { Z_DELAY_B(7); }
        if constexpr(N_order >= 18)
        { Z_DELAY_B(8); }
        if constexpr(N_order >= 20)
        { Z_DELAY_B(9); }
        output = m_y0[(N_order / 2) - 1] * m_gain;
        return output;
    }
    void clear() {
        m_u[0] = 0;
        m_u[1] = 0;
        for (int i = 0; i < N_order / 2; ++i) {
            m_block[i].clear();
            m_y0[0] = 0;
        }
    }
public:
    float_t output;//当次的输出
private:
    float_t m_y0[N_order / 2];//每一级的y，下一级的u
    float_t m_u[2];//第一级的u
    sos_block m_block[N_order / 2];
    float_t m_gain;
};


#endif //MATLAB_IIR_IIR_DF2SOS_HPP
