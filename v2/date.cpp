#include "date.h"
#include <iostream>
#include <iomanip>
#include <cmath>

using namespace std;

// 存储平年中每个月的天数
const int DAYS_PER_MONTH[] = { 0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

// 构造函数
Date::Date(int year, int month, int day) : year(year), month(month), day(day) {}


bool isLeapYear(int year) {
    return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
}


int daysInMonth(int year, int month) {
    if (month == 2 && isLeapYear(year))
        return 29;
    return DAYS_PER_MONTH[month];
}

int dateToDays(const Date& date) {
    int y = date.toString().substr(0, 4).c_str() ? atoi(date.toString().substr(0, 4).c_str()) : 0;
    int m = date.toString().substr(5, 2).c_str() ? atoi(date.toString().substr(5, 2).c_str()) : 0;
    int d = date.toString().substr(8, 2).c_str() ? atoi(date.toString().substr(8, 2).c_str()) : 0;

    int days = d;
    for (int i = 1; i < m; i++) {
        days += daysInMonth(y, i);
    }

    days += 365 * (y - 1);
    days += (y - 1) / 4;
    days -= (y - 1) / 100;
    days += (y - 1) / 400;

    return days;
}

int Date::distance(const Date& date) const {

    int year1 = this->year;
    int month1 = this->month;
    int day1 = this->day;

    int year2 = date.year;
    int month2 = date.month;
    int day2 = date.day;

    // 计算两个日期距离公元元年1月1日的天数，然后相减
    int days1 = 0, days2 = 0;

    // 计算整年的天数
    days1 = (year1 - 1) * 365 + (year1 - 1) / 4 - (year1 - 1) / 100 + (year1 - 1) / 400;
    days2 = (year2 - 1) * 365 + (year2 - 1) / 4 - (year2 - 1) / 100 + (year2 - 1) / 400;

    // 加上当年过去的天数
    for (int i = 1; i < month1; i++) {
        days1 += (i == 2 && isLeapYear(year1)) ? 29 : DAYS_PER_MONTH[i];
    }
    days1 += day1;

    for (int i = 1; i < month2; i++) {
        days2 += (i == 2 && isLeapYear(year2)) ? 29 : DAYS_PER_MONTH[i];
    }
    days2 += day2;

    // 返回差值的绝对值
    return abs(days2 - days1);
}



// 获取日期的字符串表示
string Date::toString() const {
    char buffer[11];
    sprintf(buffer, "%d-%d-%d", year, month, day);
    return string(buffer);
}

// 显示日期（用于调试）
void Date::show() const {
    cout << year << "-" << month << "-" << day;
}
