#include "controller.h"

bool s21::controller::IsGraph(const std::string str) noexcept {
    bool status_ = false;
    for (size_t i = 0; i < str.size(); i++) {
        if (str[i] == 'x') {
            status_ = true;
        }
    }
    return status_;
}

void s21::controller::SetRad(const bool is_rad) noexcept {
    model_.set_rad(is_rad);
}

bool s21::controller::IsCorrect(const std::string str) noexcept {
    bool status_ = true;
    if (str.back() != ')' && (str.back() < '0' || str.back() > '9') && str.back() != 'x') {
        status_ = false;
    }
    int l_bracket = 0, r_bracket = 0;
    for (size_t i = 0; i < str.size(); i++) {
        if (str[i] == '(') l_bracket++;
        if (str[i] == ')') r_bracket++;
    }
    if (l_bracket != r_bracket) status_ = false;
    return status_;
}

bool s21::controller::IsError() noexcept {
    return model_.get_error();
}

double s21::controller::Calculator(const std::string str) noexcept {
    model_.Notation(str);
    model_.Calculations();
    model_.ClearOutput();
    return model_.get_res();
}

void s21::controller::GraphStart(const std::string str) noexcept {
    model_.Notation(str);
}

double s21::controller::Graph(const double x) noexcept {
    model_.set_x(x);
    model_.Calculations();
    return model_.get_res();
}

void s21::controller::GraphEnd() noexcept {
    model_.ClearOutput();
}

double* s21::controller::AnnuityCredit(double sum, int term, double percent) noexcept {
    double *result_ = new double[4]{};
    model_.AnnuityCredit(sum, term, percent);
    result_[0] = model_.get_overpay();
    result_[1] = model_.get_result_sum();
    result_[2] = model_.get_month_pay();
    return result_;
}

double* s21::controller::DifferentiatedCredit(double sum, int term, double percent) noexcept {
    double *result_ = new double[4]{};
    model_.DifferentiatedCredit(sum, term, percent);
    result_[0] = model_.get_overpay();
    result_[1] = model_.get_result_sum();
    result_[2] = model_.get_f_payment();
    result_[3] = model_.get_l_payment();
    return result_;
}
