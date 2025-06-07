#ifndef ACCOUNT_H
#define ACCOUNT_H

#include <vector>
#include <utility>
#include <cmath>
#include <iostream>
#include <iomanip>

class SavingsAccount {
private:
    int id;          // 账户ID（如21325302）
    double balance;  // 账户余额（初始为0）
    double rate;     // 年利率
    std::vector<std::pair<int, double>> balanceHistory; // {天数, 余额}

public:
    SavingsAccount(int openDay, int accountId, double interestRate); // 参数顺序：开户日、ID、利率

    void show() const;
    void deposit(int day, double amount);
    void withdraw(int day, double amount);
    void settle(int day);
    void printBalance(double amount) const;
private:
    void recordTransaction(int day, double amount);
    double calculateInterest(int settlementDay) const;
    double roundToTwoDecimal(double value) const;
};

#endif