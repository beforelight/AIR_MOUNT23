#ifndef CAPTURE_CONTROL_ALGO_SS_HPP
#define CAPTURE_CONTROL_ALGO_SS_HPP
#include "Eigen"
namespace ss
{
template <std::size_t N>
class siso
{
public:
  Eigen::Matrix<float, N, N> AsubGC;
  Eigen::Matrix<float, N, 1> BsubGD;
  Eigen::Matrix<float, N, 1> G;
  Eigen::Matrix<float, 1, N> K;
  Eigen::Matrix<float, N, 1> Xg; // 观测的状态变量
  float u_max = 0;
  float u_min = 0;
  float u = 0;
  float K0 = 1.0f; // 传递函数校正比例

public:
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
      Eigen::Matrix<float, 1, N> K_)
      : u_max(u_max_), u_min(u_min_), K0(k0),
        AsubGC(AsubGC_), BsubGD(BsubGD_), G(G_), K(K_)
  {
    Xg.setZero();
  }

  /// 更新一次控制
  /// \param r 参考输入
  /// \param y 传感器测量反馈
  /// \return 控制输出u
  inline float update(float r, float y)
  {
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
};
} // namespace ss
#endif // CAPTURE_CONTROL_ALGO_SS_HPP
