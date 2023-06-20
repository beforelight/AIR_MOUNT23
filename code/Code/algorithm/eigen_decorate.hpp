#ifndef AVIS_RT_EIGEN_DECORATE_HPP
#define AVIS_RT_EIGEN_DECORATE_HPP
#include "drv/par_agent.hpp"
#include <Eigen/Eigen>


template<typename Scalar, int Rows, int Cols, int Options, int MaxRows, int MaxCols>
class eigen_decorate : public par_agent::par {
private:
    using Matrix = Eigen::Matrix<Scalar, Rows, Cols, Options, MaxRows, MaxCols>;
    Matrix *m_mat;
public:
    explicit eigen_decorate(Matrix &mat, const char *id) : par_agent::par("eigen_dcrt", id), m_mat(&mat) {}
    explicit eigen_decorate(Matrix *mat, const char *id) : par_agent::par("eigen_dcrt", id), m_mat(mat) {}


    std::string str() override {
        std::string rtv;
        for (int r = 0; r < m_mat->rows(); ++r) {
            for (int c = 0; c < m_mat->cols(); ++c) {
                rtv += my::to_string((*m_mat)(r, c));
                rtv += '\t';
            }
        }
        rtv.pop_back();
        return rtv;
    }
    void scan(const std::string &str) override {
        std::vector<std::string> split;
        Stringsplit(str, '\t', split);
        decltype(split.size()) idx = 0;
        for (int r = 0; r < m_mat->rows(); ++r) {
            for (int c = 0; c < m_mat->cols(); ++c) {
                if (idx < split.size()) {
                    if constexpr (std::is_same_v<Scalar, double>)
                        (*m_mat)(r, c) = std::stod(split[idx]);
                    if constexpr (std::is_same_v<Scalar, float>)
                        (*m_mat)(r, c) = std::stof(split[idx]);
                    idx++;
                } else {
                    return;
                }
            }
        }
    }
};


#endif //AVIS_RT_EIGEN_DECORATE_HPP
