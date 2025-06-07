#ifndef ACCOUNT_H
#define ACCOUNT_H

#include "date.h"
#include "accumulator.h"
#include <string>

class Account { // 账户类
private:
    std::string id;      // 账户ID
    double balance;      // 账户余额
    static double total; // 所有账户的总金额

protected:
    // 供派生类调用的构造函数
    Account(const Date& date, const std::string& id);
    // 记录一笔账目
    void record(const Date& date, double amount, const std::string& desc);
    // 报告错误
    void error(const std::string& msg) const;

public:
    // 获取账户ID
    const std::string& getId() const { return id; }
    // 获取账户余额
    double getBalance() const { return balance; }
    // 获取所有账户的总金额
    static double getTotal() { return total; }
    // 存入现金
    virtual void deposit(const Date& date, double amount, const std::string& desc) = 0;
    // 取出现金
    virtual void withdraw(const Date& date, double amount, const std::string& desc) = 0;
    // 结算（计算利息等）
    virtual void settle(const Date& date) = 0;
    // 显示账户信息
    virtual void show() const;
};

class SavingsAccount : public Account { // 储蓄账户类
private:
    Accumulator acc;     // 辅助计算利息的累加器
    double rate;         // 存款的年利率

public:
    // 构造函数
    SavingsAccount(const Date& date, const std::string& id, double rate);
    // 存入现金
    virtual void deposit(const Date& date, double amount, const std::string& desc);
    // 取出现金
    virtual void withdraw(const Date& date, double amount, const std::string& desc);
    // 结算利息，每年1月1日调用一次该函数
    virtual void settle(const Date& date);
};

class CreditAccount : public Account { // 信用卡账户类
private:
    Accumulator acc;     // 辅助计算利息的累加器
    double credit;       // 信用额度
    double rate;         // 欠款的日利率
    double fee;          // 信用卡年费

    double getDebt() const;  // 获取欠款额

public:
    // 构造函数
    CreditAccount(const Date& date, const std::string& id, double credit, double rate, double fee);
    // 获取信用额度
    double getCredit() const { return credit; }
    // 获取欠款额
    double getAvailableCredit() const { return credit + getBalance(); }
    // 存入现金
    virtual void deposit(const Date& date, double amount, const std::string& desc);
    // 取出现金
    virtual void withdraw(const Date& date, double amount, const std::string& desc);
    // 结算利息，每月1日调用一次该函数
    virtual void settle(const Date& date);
    // 显示账户信息
    virtual void show() const;
};

#endif