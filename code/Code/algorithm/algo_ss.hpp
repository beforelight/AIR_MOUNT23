#ifndef CAPTURE_CONTROL_ALGO_SS_HPP
#define CAPTURE_CONTROL_ALGO_SS_HPP
#include "Eigen/Eigen"
namespace ss {

    /// \brief 状态空间离散化
    /// \tparam N
    /// \param cA
    /// \param cB
    /// \param dt
    /// \param dA
    /// \param dB
    template<std::size_t N>
    void c2d(
            const Eigen::Matrix<float, N, N> &cA, const Eigen::Matrix<float, N, 1> &cB, float dt,
            Eigen::Matrix<float, N, N> &dA, Eigen::Matrix<float, N, 1> &dB) {
        //计算5\6次基本到头了
        //dA = sum{(cA*dt)^k/k!} from k=0;
        //dB = sum{[(cA*dt)^k/k!]*[dt/(k+1)]}*cB from k=0;
        // 令积分项的为M
        Eigen::Matrix<float, N, N> M = Eigen::Matrix<float, N, N>::Identity();//k=0;
        dA = M;
        Eigen::Matrix<float, N, N> tempB = Eigen::Matrix<float, N, N>::Identity();
        tempB = tempB * dt;
        for (float k = 1.0f; k < 6.0f; k += 1.0f) {
            M = M * (cA * (dt / k));
            dA = dA + M;
            tempB = tempB + M * (dt / (k + 1.0f));
        }
        dB = tempB * cB;
    }

    /// \brief 状态空间实现的传递函数
    /// \tparam N
    template<std::size_t N>
    class ss {
    public:
        float y = 0;
        Eigen::Matrix<float, N, 1> X;
        Eigen::Matrix<float, N, N> A;//这俩都是离散后的
        Eigen::Matrix<float, N, 1> B;//这俩都是离散后的
        Eigen::Matrix<float, 1, N> C;//依旧是连续
        Eigen::Matrix<float, 1, 1> D;//依旧是连续
    public:
        ss() = default;
        /// \brief 构造状态空间
        /// \param dt
        /// \param A
        /// \param B
        /// \param C
        /// \param D
        ss(
                float dt,
                Eigen::Matrix<float, N, N> A,
                Eigen::Matrix<float, N, 1> B,
                Eigen::Matrix<float, 1, N> C,
                Eigen::Matrix<float, 1, 1> D
        ) : C(C), D(D) {
            c2d<N>(A, B, dt, this->A, this->B);
            X.setZero();
        }
        inline float update(float u) {
            y = (C * X + D * u)[0];
            X = A * X + B * u;
            return y;
        }
    };

    template<std::size_t N>
    class siso {
    public:
        Eigen::Matrix<float, N, N> AsubGC;//这三都是离散后的
        Eigen::Matrix<float, N, 1> BsubGD;//这三都是离散后的
        Eigen::Matrix<float, N, 1> G;//这三都是离散后的
        Eigen::Matrix<float, 1, N> K;//依旧是连续
        Eigen::Matrix<float, 1, N> C;//依旧是连续
        Eigen::Matrix<float, 1, 1> D;//依旧是连续
        Eigen::Matrix<float, N, 1> Xg; // 观测的状态变量
        float u_max = 0;
        float u_min = 0;
        float u = 0;
        float K0 = 1.0f; // 传递函数校正比例

    public:
        siso() {
            AsubGC = AsubGC.eval();
            BsubGD = BsubGD.eval();
            G = G.eval();
            K = K.eval();
            C = C.eval();
            D = D.eval();
            Xg.setZero();
        }

        /// \brief 部分变量需要离散化，参考代码:
        ///  [AsubGC, BsubGD] = c2d(A - G * C, B - G * D, dt);
        ///  [AsubGC, G_] = c2d(A - G * C, G, dt);
        ///  format long g;
        ///  % %n = max(size(G_))
        ///  % AsubGC
        ///  % BsubGD
        ///  % G_
        ///  % K
        ///
        /// \param u_max_
        /// \param u_min_
        /// \param k0
        /// \param AsubGC_ 需要在Matlab中离散化
        /// \param BsubGD_ 需要在Matlab中离散化
        /// \param G_ 需要在Matlab中离散化
        /// \param K_
        siso(
                float u_max_, float u_min_, float k0,
                Eigen::Matrix<float, N, N> AsubGC_,
                Eigen::Matrix<float, N, 1> BsubGD_,
                Eigen::Matrix<float, N, 1> G_,
                Eigen::Matrix<float, 1, N> K_) :
                u_max(u_max_), u_min(u_min_), K0(k0),
                AsubGC(AsubGC_), BsubGD(BsubGD_), G(G_), K(K_) {
            Xg.setZero();
        }

        /// \brief 输入连续的控制器参数，内部会进行离散化
        /// \param u_max_
        /// \param u_min_
        /// \param k0
        /// \param dt
        /// \param A
        /// \param B
        /// \param C
        /// \param D
        /// \param cG
        /// \param cK
        siso(
                float u_max_, float u_min_, float k0, float dt,
                Eigen::Matrix<float, N, N> A,
                Eigen::Matrix<float, N, 1> B,
                Eigen::Matrix<float, 1, N> C,
                Eigen::Matrix<float, 1, 1> D,
                Eigen::Matrix<float, N, 1> cG,
                Eigen::Matrix<float, 1, N> cK) :
                u_max(u_max_), u_min(u_min_), K0(k0), K(cK), C(C), D(D) {
            Xg.setZero();
            c2d<N>(A - cG * C, B - cG * D, dt, AsubGC, BsubGD);
            c2d<N>(A - cG * C, cG, dt, AsubGC, G);
            ///  [AsubGC, BsubGD] = c2d(A - G * C, B - G * D, dt);
            ///  [AsubGC, G_] = c2d(A - G * C, G, dt);
        }

        /// 更新一次控制
        /// \param r 参考输入
        /// \param y 传感器测量反馈
        /// \return 控制输出u
        inline float update(float r, float y) {
            // 放大K0倍参考信号
            r *= K0;
            // 更新观测器
            Xg = AsubGC * Xg + BsubGD * u + G * y;
            // 状态反馈
            u = r - K * Xg;
            // 限幅
            if (u > u_max)
                u = u_max;
            else if (u < u_min)
                u = u_min;
            return u;
        }

        /// \brief 返回观测器估计的输出是多少
        /// \return
        float y_est() {
            return (C * Xg + D * u)[0];
        }
    };

    template<std::size_t N>
    class siso_with_feed_forward : public siso<N> {
    public:
        Eigen::Matrix<float, N, 1> BsubGD_ff;

    public:
        siso_with_feed_forward() { BsubGD_ff = BsubGD_ff.eval(); }
        /// \brief 构造带有前馈环节的极点配置控制器
        /// \param u_max_
        /// \param u_min_
        /// \param k0
        /// \param dt
        /// \param A
        /// \param B
        /// \param C
        /// \param D
        /// \param cG
        /// \param cK
        /// \param Bf 前馈环节的B
        /// \param Df 前馈环节的D
        siso_with_feed_forward(
                float u_max_, float u_min_, float k0, float dt,
                Eigen::Matrix<float, N, N> A,
                Eigen::Matrix<float, N, 1> B,
                Eigen::Matrix<float, 1, N> C,
                Eigen::Matrix<float, 1, 1> D,
                Eigen::Matrix<float, N, 1> cG,
                Eigen::Matrix<float, 1, N> cK,
                Eigen::Matrix<float, N, 1> Bf,
                Eigen::Matrix<float, 1, 1> Df) :
                siso<N>(u_max_, u_min_, k0, dt,
                        A, B, C, D, cG, cK) {
            c2d<N>(A - cG * C, Bf - cG * Df, dt, this->AsubGC, BsubGD_ff);
        }

        /// \brief 更新一次控制
        /// \tparam H_ 前馈补偿环节类，必须重载float operator()(float)
        /// \param r 跟踪参考
        /// \param y 测量反馈
        /// \param uf 前馈信号
        /// \param H 前馈补偿环节
        /// \return 控制信号
        template<class H_>
        inline float update_forward(float r, float y, float uf, H_ &H) {
            // 放大K0倍参考信号
            r *= this->K0;
            // 更新观测器
            this->Xg =
                    this->AsubGC * this->Xg + this->BsubGD * this->u + this->G * y
                    + BsubGD_ff * uf;
            // 状态反馈
            this->u = r - this->K * this->Xg + H(uf);
            // 限幅
            if (this->u > this->u_max)
                this->u = this->u_max;
            else if (this->u < this->u_min)
                this->u = this->u_min;
            return this->u;
        }
    };


} // namespace ss
#endif // CAPTURE_CONTROL_ALGO_SS_HPP
