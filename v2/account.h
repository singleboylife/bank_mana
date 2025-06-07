#ifndef ACCOUNT_H
#define ACCOUNT_H

#include <vector>
#include <utility>
#include <cmath>
#include <iostream>
#include <iomanip>
#include <string>
#include "date.h"

class SavingsAccount {
private:
    std::string id;                // 账户ID（如"S3755217"）
    double balance;                // 账户余额（初始为0）
    double rate;                   // 年利率
    std::vector<std::pair<Date, double>> balanceHistory; // {日期, 余额}
    std::vector<std::string> descriptions;              // 交易描述
    static double total;           // 所有账户的总金额

public:
    SavingsAccount(const Date& date, const std::string& accountId, double interestRate); // 参数顺序：开户日、ID、利率

    void show() const;
    void deposit(const Date& date, double amount, const std::string& desc);
    void withdraw(const Date& date, double amount, const std::string& desc);
    void settle(const Date& date);
    void printBalance(double amount) const;
    static double getTotal() { return total; } // 获取所有账户的总金额

private:
    void recordTransaction(const Date& date, double amount, const std::string& desc);
    double calculateInterest(const Date& settlementDate) const;
    double roundToTwoDecimal(double value) const;
};

#endif