#include "accumulator.h"

// 构造函数
Accumulator::Accumulator(const Date& date, double value)
    : lastDate(date), value(value), sum(0) {
}

// 获取累加结果
double Accumulator::getSum(const Date& date) const {
    // 计算从上次变更日到当前日期的天数
    int days = date.distance(lastDate);
    return sum + days * value;
}

// 变更数值
void Accumulator::change(const Date& date, double value) {
    // 先累加上次变更日到当前日期的值
    sum = getSum(date);
    // 更新最后变更日和当前值
    lastDate = date;
    this->value = value;
}

// 重置累加器
void Accumulator::reset(const Date& date, double value) {
    // 重置累加器，清零累加和
    lastDate = date;
    this->value = value;
    sum = 0;
}