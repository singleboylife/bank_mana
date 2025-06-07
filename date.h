#ifndef DATE_H
#define DATE_H

#include <iostream>
#include <string>

class Date {
private:
    int year;
    int month;
    int day;

public:
    // 构造函数
    Date(int year, int month, int day);

    // 获取日期的字符串表示
    std::string toString() const;

    // 计算两个日期相差的天数
    int distance(const Date& date) const;

    // 显示日期
    void show() const;
};

#endif // DATE_H 