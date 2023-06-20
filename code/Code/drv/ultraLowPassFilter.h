#ifndef CAPTURE_CONTROL_ULTRALOWPASSFILTER_H
#define CAPTURE_CONTROL_ULTRALOWPASSFILTER_H

/// \brief 极低频率的低通滤波器，原理是一个矩形窗函数和原信号卷积，
/// 矩形窗的宽度为2^(B*A)
/// 空间开销为 A*(2^B)
/// \tparam A 阶数A
/// \tparam B 阶数B 其大小最好在5-8之间
template <int A, int B>
class ultraLowPassFilter
{
public:
  const float &update(float in)
  {
    m_avg[0] = in;
    for (int i = 0; i < A; ++i) {
      m_sum[i] = m_sum[i] + m_avg[i] - m_f[i][m_idx[i]]; // 更新第i级别的和
      m_f[i][m_idx[i]] = m_avg[i];                       // 存档新值
      m_idx[i] = (m_idx[i] + 1) & m_idx_mask;            // 更新索引

      if (m_idx[i] == 0) {
        // 说明更新完第i级了，则更新下一级
        m_avg[i + 1] = m_sum[i] * m_sum_rate;
      } else {
        break;
      }
    }
    return m_avg[A];
  }

  /// 设置初始值，使滤波器快速收敛
  /// \param val
  void setInitVal(float val)
  {
    for (int i = 0; i < A; ++i) {
      for (int j = 0; j < (1 << B); ++j) {
        m_f[i][j] = val;
      }
      m_idx[i] = 0;
      m_sum[i] = val * (1 << B);
      m_avg[i] = val;
    }
    m_avg[A] = val;
  }

  float forceOutput()
  {
    int j = 0;
    for (int i = 0; i < A; ++i) {
      if (m_idx[i] != 0) {
        j = i;
      }
    }
    // 计算第J级别的平均值作为强制输出
    float sum = 0;
    for (int i = 0; i < m_idx[j]; ++i) {
      sum += m_f[j][i];
    }
    sum /= m_idx[j];
    return sum;
  }

private:
  float m_f[A][1 << B]; // A是第一级
  unsigned int m_idx[A];
  constexpr static const unsigned int m_idx_mask = (1 << B) - 1;
  constexpr static const float m_sum_rate = 1.0 / (float) (1 << B);
  float m_sum[A];
  float m_avg[A + 1]; // 第i位代表这是第i极的输入，同时也是第i-1级的平均
};
#endif // CAPTURE_CONTROL_ULTRALOWPASSFILTER_H
