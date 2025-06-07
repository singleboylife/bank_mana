#include "account.h"
#include <iomanip>
#include <cmath>

SavingsAccount::SavingsAccount(int openDay, int accountId, double interestRate)
    : id(accountId), balance(0), rate(interestRate) {
    std::cout << openDay << "\t#" << id << " is created\n";
    balanceHistory.push_back({ openDay, 0 });
}


// 辅助函数：按特殊规则格式化余额输出
void SavingsAccount::printBalance(double amount) const {
    // 将amount转换为分（整数），避免浮点数精度问题
    int amountCents = (int)(amount * 100 + 0.5); // 四舍五入到分
    int secondDecimal = amountCents % 10; // 获取第二位小数

    if (amountCents % 100 == 0) {
        // 如果是整数（没有小数部分）
        std::cout << "#" << id <<"\tBalance: " << std::fixed << std::setprecision(0) << amount;
    }
    else if (secondDecimal < 5) {
        // 第二位小数小于5，只显示一位小数
        std::cout << "#" << id << "\tBalance: " << std::fixed << std::setprecision(1) << amount;
    }
    else {
        // 第二位小数大于等于5，显示两位小数
        std::cout << "#" << id << "\tBalance: " << std::fixed << std::setprecision(2) << amount;
    }
}


void SavingsAccount::show() const {
    //std::cout << "#" << id << "\tBalance: " << balance;
    printBalance(balance);
    
}

void SavingsAccount::deposit(int day, double amount) {
    amount = roundToTwoDecimal(amount);
    balance += amount;
    std::cout << day << "\t#" << id << "\t"
        << std::setprecision(0) << std::fixed << amount << "\t"
        << balance << std::endl;
    balanceHistory.push_back({ day, balance });
}

void SavingsAccount::withdraw(int day, double amount) {
    amount = roundToTwoDecimal(amount);
    if (amount > balance) {
        std::cerr << "Error: Insufficient balance in account #" << id << std::endl;
        return;
    }
    balance -= amount;
    std::cout << day << "\t#" << id << "\t"
        << "-" << std::setprecision(0) << std::fixed << amount << "\t"
        << balance << std::endl;
    balanceHistory.push_back({ day, balance });
}

void SavingsAccount::settle(int day) {
    double interest = calculateInterest(day);
    interest = roundToTwoDecimal(interest);
    balance += interest;

    // 特殊处理输出格式以匹配示例
    std::cout << day << "\t#" << id << "\t";
    std::cout << std::fixed << std::setprecision(2) << interest << "\t";

    // 余额按特殊规则输出：第二位小数<5则不显示，>=5则显示两位
    // 将balance转换为分（整数），避免浮点数精度问题
    int balanceCents = (int)(balance * 100 + 0.5); // 四舍五入到分
    int secondDecimal = balanceCents % 10; // 获取第二位小数
    int firstDecimal = (balanceCents / 10) % 10; // 获取第一位小数

    if (balanceCents % 100 == 0) {
        // 如果是整数（没有小数部分）
        std::cout << std::setprecision(0) << balance;
    }
    else if (secondDecimal < 5) {
        // 第二位小数小于5，只显示一位小数
        std::cout << std::setprecision(1) << balance;
    }
    else {
        // 第二位小数大于等于5，显示两位小数
        std::cout << std::setprecision(2) << balance;
    }

    std::cout << std::endl;
    balanceHistory.push_back({ day, balance });
}

double SavingsAccount::calculateInterest(int settlementDay) const {
    if (balanceHistory.empty()) return 0.0;

    double weightedSum = 0;
    int totalDays = 0;

    for (size_t i = 0; i < balanceHistory.size() - 1; ++i) {
        int days = balanceHistory[i + 1].first - balanceHistory[i].first;
        weightedSum += days * balanceHistory[i].second;
        totalDays += days;
    }

    int lastDay = balanceHistory.back().first;
    if (settlementDay > lastDay) {
        int days = settlementDay - lastDay;
        weightedSum += days * balanceHistory.back().second;
        totalDays += days;
    }

    return (weightedSum * rate) / 365.0;
}

double SavingsAccount::roundToTwoDecimal(double value) const {
    return floor(value * 100 + 0.5) / 100;
}