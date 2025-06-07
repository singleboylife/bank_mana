#ifndef ACCUMULATOR_H
#define ACCUMULATOR_H

#include "date.h"

class Accumulator {
private:
    Date lastDate;   // 上次变更数值的日期
    double value;    // 当前数值
    double sum;      // 数值按日累加之和

public:
    // 构造函数，初始化累加器
    Accumulator(const Date& date, double value);

    // 获取累加结果
    double getSum(const Date& date) const;

    // 变更数值
    void change(const Date& date, double value);

    // 重置累加器
    void reset(const Date& date, double value);
};

#endif 