#include "account.h"
#include <iomanip>
#include <cmath>

// 初始化静态成员变量
double SavingsAccount::total = 0;

SavingsAccount::SavingsAccount(const Date& date, const std::string& accountId, double interestRate)
    : id(accountId), balance(0), rate(interestRate) {
    std::cout << date.toString()
        << "\t#" << id << " created" << std::endl;
    balanceHistory.push_back(std::make_pair(date, 0));
    descriptions.push_back(""); 
}


void SavingsAccount::printBalance(double amount) const {
    
    int amountCents = (int)(amount * 100 + 0.5); 
    int secondDecimal = amountCents % 10; 

    if (amountCents % 100 == 0) {
        
        std::cout << "#" << id << "\tBalance: " << std::fixed << std::setprecision(0) << amount;
    }
    else if (secondDecimal < 5) {
     
        std::cout << "#" << id << "\tBalance: " << std::fixed << std::setprecision(1) << amount;
    }
    else {
       
        std::cout << "#" << id << "\tBalance: " << std::fixed << std::setprecision(2) << amount;
    }
}

void SavingsAccount::show() const {
    printBalance(balance);
}

void SavingsAccount::deposit(const Date& date, double amount, const std::string& desc) {
    amount = roundToTwoDecimal(amount);
    balance += amount;
    total += amount; // 更新总金额

    std::cout << date.toString()
        << "\t#" << id << "\t"
        << std::setw(7) << std::left << std::setprecision(0) << std::fixed << amount << "\t"
        << std::setw(7) << std::left << balance << "\t" << desc << std::endl;

    recordTransaction(date, amount, desc);
}

void SavingsAccount::withdraw(const Date& date, double amount, const std::string& desc) {
    amount = roundToTwoDecimal(amount);
    if (amount > balance) {
        std::cerr << "Error: Insufficient balance in account #" << id << std::endl;
        return;
    }
    balance -= amount;
    total -= amount; // 更新总金额

    std::cout << date.toString()
        << "\t#" << id << "\t"
        << std::setw(1) << std::right << "-" << std::setprecision(0) << std::fixed << amount << "\t"
        << std::setw(1) << std::right << balance << "\t" << desc << std::endl;

    recordTransaction(date, -amount, desc);
}

void SavingsAccount::settle(const Date& date) {
    double interest = calculateInterest(date);
    interest = roundToTwoDecimal(interest);
    balance += interest;
    total += interest; // 更新总金额

    
    std::cout << date.toString()
        << "\t#" << id << "\t";

    // 输出利息
    int interestCents = (int)(interest * 100 + 0.5); // 四舍五入到分
    int interestSecondDecimal = interestCents % 10; // 获取第二位小数

    if (interestSecondDecimal < 5) {
        std::cout << std::setw(4) << std::right << std::setprecision(1) << std::fixed << interest << "\t";
    }
    else {
        std::cout << std::setw(4) << std::right << std::setprecision(2) << std::fixed << interest << "\t";
    }

    // 格式化余额输出
    int balanceCents = (int)(balance * 100 + 0.5); // 四舍五入到分
    int secondDecimal = balanceCents % 10; // 获取第二位小数

    std::cout << std::setw(7) << std::right;

    if (balanceCents % 100 == 0) {
        
        std::cout << std::setprecision(0) << std::fixed << balance;
    }
    else if (secondDecimal < 5) {
       
        std::cout << std::setprecision(1) << std::fixed << balance;
    }
    else {
        
        std::cout << std::setprecision(2) << std::fixed << balance;
    }

    std::cout << " interest" << std::endl;

    recordTransaction(date, interest, "interest");
}

void SavingsAccount::recordTransaction(const Date& date, double amount, const std::string& desc) {
    balanceHistory.push_back(std::make_pair(date, balance));
    descriptions.push_back(desc);
}

double SavingsAccount::calculateInterest(const Date& settlementDate) const {
    if (balanceHistory.empty()) return 0.0;

    double weightedSum = 0;

    for (size_t i = 0; i < balanceHistory.size() - 1; ++i) {
        int days = balanceHistory[i + 1].first.distance(balanceHistory[i].first);
        weightedSum += days * balanceHistory[i].second;
    }

    Date lastDate = balanceHistory.back().first;
    int days = settlementDate.distance(lastDate);
    if (days > 0) {
        weightedSum += days * balanceHistory.back().second;
    }
    
    return (weightedSum * rate) / 366.0;
}

double SavingsAccount::roundToTwoDecimal(double value) const {
    return floor(value * 100 + 0.5) / 100;
}