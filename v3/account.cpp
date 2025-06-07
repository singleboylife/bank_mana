#include "account.h"
#include <iostream>
#include <iomanip>
#include <cmath>

using namespace std;

// 实现Account类的静态成员和构造函数
double Account::total = 0;

Account::Account(const Date& date, const std::string& id) : id(id), balance(0) {
}

void Account::record(const Date& date, double amount, const std::string& desc) {
    amount = floor(amount * 100 + 0.5) / 100; // 保留小数点后两位
    balance += amount;
    total += amount;

    // 打印交易记录
    cout << date;
    cout << "\t#" << id << "\t";
    if (amount >= 0)
        cout << setw(7) << left << amount;
    else
        cout << "-" << setw(6) << left << -amount;
    cout << "\t" << setw(7) << left << balance << "\t" << desc << endl;
}

void Account::error(const std::string& msg) const {
    cout << "Error(#" << id << "): " << msg << endl;
}

void Account::show() const {
    cout << id << "\tBalance: " << balance;
}

// 实现SavingsAccount类
SavingsAccount::SavingsAccount(const Date& date, const std::string& id, double rate)
    : Account(date, id), rate(rate), acc(date, 0) {
    cout << date << "\t#" << id << " created" << endl;
}

void SavingsAccount::deposit(const Date& date, double amount, const std::string& desc) {
    record(date, amount, desc);
    acc.change(date, getBalance());
}

void SavingsAccount::withdraw(const Date& date, double amount, const std::string& desc) {
    if (amount > getBalance()) {
        error("not enough money");
        return;
    }
    record(date, -amount, desc);
    acc.change(date, getBalance());
}

void SavingsAccount::settle(const Date& date) {
    if (date.getMonth() == 1 && date.getDay() == 1) { // 每年1月1日进行结算
        double interest = acc.getSum(date) * rate / 366; // 计算年利息
        if (interest != 0)
            record(date, interest, "interest");
        acc.reset(date, getBalance());
    }
}

// 实现CreditAccount类
CreditAccount::CreditAccount(const Date& date, const std::string& id,
    double credit, double rate, double fee)
    : Account(date, id), credit(credit), rate(rate), fee(fee), acc(date, 0) {
    cout << date << "\t#" << id << " created" << endl;
}

double CreditAccount::getDebt() const {
    double balance = getBalance();
    return (balance < 0 ? -balance : 0);
}

void CreditAccount::deposit(const Date& date, double amount, const std::string& desc) {
    record(date, amount, desc);
    if (getBalance() > 0) { // 如果存款后有正余额，就不计息了
        acc.reset(date, 0);
    }
    else { // 否则，存款减少了欠款
        acc.change(date, getDebt());
    }
}

void CreditAccount::withdraw(const Date& date, double amount, const std::string& desc) {
    if (amount - getBalance() > credit) { // 如果取款后超过信用额度
        error("not enough credit");
        return;
    }
    record(date, -amount, desc);
    if (getBalance() < 0) { // 如果取款后有欠款
        acc.change(date, getDebt());
    }
}

void CreditAccount::settle(const Date& date) {
    double interest = 0;

    // 每月1日结算利息
    if (date.getDay() == 1) {
        // 计算上月欠款总额
        interest = acc.getSum(date) * rate;
        if (interest != 0)
            record(date, -interest, "interest");
        acc.reset(date, getDebt());
    }

    // 每年1月1日收取年费
    if (date.getMonth() == 1 && date.getDay() == 1) {
        record(date, -fee, "annual fee");
    }
}

void CreditAccount::show() const {
    Account::show();
    cout << "\tAvailable credit: " << getAvailableCredit();
}