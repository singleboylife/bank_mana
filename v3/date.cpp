#include "date.h"
#include <iostream>
#include <cstring>

using namespace std;

// 存储平年中每个月的天数
const int DAYS_PER_MONTH[] = { 0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

// 构造函数
Date::Date(int year, int month, int day) : year(year), month(month), day(day) {
    // 验证日期是否合法
    if (day <= 0 || day > getMaxDay()) {
        cout << "Invalid date: " << year << "-" << month << "-" << day << endl;
        throw "Invalid date";
    }

    // 计算距离公元元年1月1日的天数
    totalDays = 0;
    int y = year - 1;
    // 加上从公元元年到去年经过的天数
    totalDays = y * 365 + y / 4 - y / 100 + y / 400;
    // 加上今年已经过去的天数
    for (int i = 1; i < month; i++) {
        totalDays += ((i == 2 && isLeapYear()) ? 29 : DAYS_PER_MONTH[i]);
    }
    totalDays += day;
}

int Date::getYear() const {
    return year;
}

int Date::getMonth() const {
    return month;
}

int Date::getDay() const {
    return day;
}

int Date::getMaxDay() const {
    if (month == 2 && isLeapYear())
        return 29;
    return DAYS_PER_MONTH[month];
}

bool Date::isLeapYear() const {
    return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
}

int Date::distance(const Date& date) const {
    return abs(totalDays - date.totalDays);
}

void Date::show() const {
    cout << year << "-" << month << "-" << day;
}

// 日期转换为字符串
Date::operator const char* () const {
    static char str[11];
    sprintf(str, "%d-%d-%d", year, month, day);
    return str;
}
