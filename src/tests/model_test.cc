#include "model_test.h"

/*
  ============================ CALCULATOR ACCESSORS ===========================
*/
double s21::model::get_res() const noexcept { return result_; }
bool s21::model::get_error() const noexcept { return is_error_; }
/*
  ============================ CALCULATOR ACCESSORS ===========================
*/

/*
  ============================ CALCULATOR MUTATORS ============================
*/
void s21::model::set_x(const double &num) noexcept { x_value_ = num; }
void s21::model::set_rad(const bool &graph) noexcept { is_rad_ = graph; }
/*
  ============================ CALCULATOR MUTATORS ============================
*/

/*
  ============================ C A L C U L A T O R ============================
*/
void s21::model::GetNums(double &x) noexcept {
  x = num_buffer_.top();
  num_buffer_.pop();
}

void s21::model::GetNums(double &x, double &y) noexcept {
  x = num_buffer_.top();
  num_buffer_.pop();
  y = num_buffer_.top();
  num_buffer_.pop();
}

void s21::model::GetNums() noexcept {
  if (option_ == 1) {
    GetNums(x_);
  } else if (option_ == 2) {
    GetNums(x_, y_);
  } else if (option_ == 3) {
    GetNums(x_);
    x_ = is_rad_ ? x_ : x_ * M_PI / 180;
  } else if (option_ == 4) {
    const double n_num_ = num_buffer_.top();
    num_buffer_.pop();
    num_buffer_.push(-n_num_);
  }
}

void s21::model::InsertNumOutput(size_t &index) noexcept {
  if (isdigit(str_[index]) || str_[index] == 'x' || str_[index] == 'P') {
    int i = 0;
    bool is_negative_ = false;
    char char_str_[255] = {'\0'};
    if (str_[index - 1] == '-' && str_[index - 2] == '(') is_negative_ = true;
    while (isdigit(str_[index]) || str_[index] == '.' || str_[index] == 'x' 
    || str_[index] == 'P' || str_[index] == 'i') {
      char_str_[i++] = str_[index++];
    }
    output_[pos_++] = is_negative_ ? std::string(char_str_) + "-" : std::string(char_str_);
  }
}

void s21::model::PushLogic(const std::string &str) noexcept {
  if (str == "mod" || str == "*" || str == "/") {
    while (!stack_.empty() && (stack_.top() == "mod" || stack_.top() == "*" 
    || stack_.top() == "/" || stack_.top() == "^" || stack_.top() == "!")) {
      output_[pos_++] = stack_.top();
      stack_.pop();
    }
  } else if (str == "+" || str == "-") {
    while (!stack_.empty() && (stack_.top() == "mod" || stack_.top() == "*" 
    || stack_.top() == "/" || stack_.top() == "+" || stack_.top() == "-" 
    || stack_.top() == "^" || stack_.top() == "!")) {
      output_[pos_++] = stack_.top();
      stack_.pop();
    }
  }
  stack_.push(str);
}

void s21::model::PushFunctions(size_t &index) noexcept {
  if (option_ == 1) {
    if (str_[index] == '^') {
      stack_.push("^");
    } else {
      char buffer_[255] = {'\0'};
      for (size_t i = 0; str_[index] != '('; i++)
        buffer_[i] = str_[index++];
      if (std::string(buffer_).size())
        stack_.push(std::string(buffer_));
      stack_.push("(");
    }
  } else if (option_ == 2) {
    if (str_[index] == 'm') {
      PushLogic("mod");
      index += 2;
    } else if (str_[index] == '*') {
      PushLogic("*");
    } else if (str_[index] == '/') {
      PushLogic("/");
    } else if (str_[index] == '+') {
      PushLogic("+");
    } else if (str_[index] == '-') {
      if (str_[index - 1] == '(' && !isdigit(str_[index + 1])
       && str_[index + 1] != 'P' && str_[index + 1] != 'x')
        PushLogic("!");
      else if (str_[index - 1] != '(')
        PushLogic("-");
    }
  }
}

void s21::model::PopFunctions() noexcept {
  if (option_ == 1) {
    while (!stack_.empty() && stack_.top() != "(") {
      output_[pos_++] = stack_.top();
      stack_.pop();
    }
    if (stack_.empty()) {
      is_error_ = true;
    } else {
      stack_.pop();
      for (size_t i = 0; i < functions_.size() && !stack_.empty(); i++) {
        if (stack_.top() == functions_[i]) {
          output_[pos_++] = stack_.top();
          stack_.pop();
        }
      }
    }
  } else if (option_ == 2) {
    while (!stack_.empty() && !is_error_) {
      if (stack_.top() == "(") {
        is_error_ = true;
      } else {
        output_[pos_++] = stack_.top();
        stack_.pop();
      }
    }
  }
}

bool s21::model::ConvertNums(const size_t &i) noexcept {
  double num_ = 0.0;
  if (isdigit(output_[i].front()) || output_[i].front() == 'x' || output_[i].front() == 'P') {
    if (output_[i].front() == 'x')
      num_ = x_value_;
    else if (output_[i].front() == 'P')
      num_ = M_PI;
    else
      num_ = atof(output_[i].c_str());
    if (output_[i].back() == '-')
      num_ = -num_;
    num_buffer_.push(num_);
    return true;
  }
  return false;
}

void s21::model::Notation(const std::string &str) noexcept {
  str_ = str;
  for (size_t i = 0; i < str_.size() && !is_error_; i++) {
    InsertNumOutput(i);
    switch (str_[i]) {
      case 'c':
      case 's':
      case 't':
      case 'a':
      case 'l':
      case '^':
      case '(':
        option_ = 1;
        PushFunctions(i);
        break;
      case 'm':
      case '*':
      case '/':
      case '+':
      case '-':
        option_ = 2;
        PushFunctions(i);
        break;
      case ')':
        option_ = 1;
        PopFunctions();
        break;
      case '=':
        option_ = 2;
        PopFunctions();
        break;
    }
  }
}

void s21::model::DoCalculations() noexcept {
  if (func_ == "+")
    num_buffer_.push(y_ + x_);
  else if (func_ == "-")
    num_buffer_.push(y_ - x_);
  else if (func_ == "*")
    num_buffer_.push(y_ * x_);
  else if (func_ == "/")
    num_buffer_.push(y_ / x_);
  else if (func_ == "ln")
    num_buffer_.push(log(x_));
  else if (func_ == "cos")
    num_buffer_.push(cos(x_));
  else if (func_ == "sin")
    num_buffer_.push(sin(x_));
  else if (func_ == "tan")
    num_buffer_.push(tan(x_));
  else if (func_ == "abs")
    num_buffer_.push(fabs(x_));
  else if (func_ == "acos")
    num_buffer_.push(acos(x_));
  else if (func_ == "asin")
    num_buffer_.push(asin(x_));
  else if (func_ == "atan")
    num_buffer_.push(atan(x_));
  else if (func_ == "sqrt")
    num_buffer_.push(sqrt(x_));
  else if (func_ == "log")
    num_buffer_.push(log10(x_));
  else if (func_ == "^")
    num_buffer_.push(pow(y_, x_));
  else if (func_ == "mod")
    num_buffer_.push(fmod(y_, x_));
}

void s21::model::Calculations() noexcept {
  for (int i = 0; i < pos_; i++) {
    if (!ConvertNums(i)) {
      switch (output_[i].front()) {
        case '+':
        case '-':
        case '*':
        case '/':
        case '^':
        case 'm':
          option_ = 2;
          break;
        case 'c':
        case 's':
        case 't':
        case 'a':
          option_ = output_[i] == "sqrt" || output_[i] == "abs" ? 1 : 3;
          break;
        case 'l':
          option_ = 1;
          break;
        case '!':
          option_ = 4;
          break;
      }
      func_ = output_[i];
      GetNums();
      DoCalculations();
    }
  }
  result_ = num_buffer_.top();
  num_buffer_.pop();
}

void s21::model::ClearOutput() noexcept {
  pos_ = 0;
}

/*
  ============================ C A L C U L A T O R ============================
*/
