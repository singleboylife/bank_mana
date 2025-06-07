#ifndef DATE_H
#define DATE_H

#include <iostream>
#include <string>

class Date {
private:
    int year;
    int month;
    int day;
    int totalDays;

public:
    // 构造函数
    Date(int year, int month, int day);
    Date();
    // 获取日期的年、月、日
    int getYear() const;
    int getMonth() const;
    int getDay() const;
    int getMaxDay() const;  // 获取当月最大天数

    // 判断是否是闰年
    bool isLeapYear() const;

    // 获取日期的字符串表示
    std::string toString() const;

    // 计算两个日期相差的天数
    int distance(const Date& date) const;

    // 显示日期
    void show() const;

    // 重载<运算符，用于日期比较
    bool operator<(const Date& date) const;

    // 静态方法，从输入读取日期
    static Date read();

    // 转成字符串
    operator const char* () const;
};

#endif // DATE_H 