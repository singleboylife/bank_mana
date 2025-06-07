#include "account.h"
#include <iostream>
#include <vector>
#include <algorithm>
#include <fstream>
#include <sstream>
using namespace std;

struct deleter {
    template <class T> void operator () (T* p) { delete p; }
};

// 处理一条命令
void processCommand(char cmd, Date& date, vector<Account*>& accounts, ofstream& cmdFile) {
    char type;
    int index, day;
    double amount, credit, rate, fee;
    string id, desc;
    Account* account;
    Date date1, date2;

    stringstream command;
    command << cmd << ' ';

    switch (cmd) {
    case 'a': // 增加账户
        cin >> type >> id;
        command << type << ' ' << id << ' ';
        if (type == 's') {
            cin >> rate;
            command << rate;
            account = new SavingsAccount(date, id, rate);
        }
        else {
            cin >> credit >> rate >> fee;
            command << credit << ' ' << rate << ' ' << fee;
            account = new CreditAccount(date, id, credit, rate, fee);
        }
        accounts.push_back(account);
        break;
    case 'd': // 存入现金
        cin >> index >> amount;
        getline(cin, desc);
        command << index << ' ' << amount << desc;
        accounts[index]->deposit(date, amount, desc);
        break;
    case 'w': // 取出现金
        cin >> index >> amount;
        getline(cin, desc);
        command << index << ' ' << amount << desc;
        accounts[index]->withdraw(date, amount, desc);
        break;
    case 's': // 查询各账户信息
        for (size_t i = 0; i < accounts.size(); i++) {
            cout << "[" << i << "] ";
            accounts[i]->show();
            cout << endl;
        }
        break;
    case 'c': // 改变日期
        cin >> day;
        command << day;
        if (day < date.getDay())
            cout << "You cannot specify a previous day";
        else if (day > date.getMaxDay())
            cout << "Invalid day";
        else
            date = Date(date.getYear(), date.getMonth(), day);
        break;
    case 'n': // 进入下个月
        if (date.getMonth() == 12)
            date = Date(date.getYear() + 1, 1, 1);
        else
            date = Date(date.getYear(), date.getMonth() + 1, 1);
        for (vector<Account*>::iterator iter = accounts.begin(); iter != accounts.end(); ++iter)
            (*iter)->settle(date);
        break;
    case 'q': // 查询一段时间内的账目
        date1 = Date::read();
        date2 = Date::read();
        Account::query(date1, date2);
        break;
    }

    // 将命令写入文件
    if (cmd != 's' && cmd != 'q') {
        cmdFile << command.str() << endl;
    }
}

int main() {
    Date date(2008, 11, 1); // 起始日期
    vector<Account*> accounts; // 创建账户数组，元素个数为0

    // 从命令文件中恢复账户状态
    ifstream inFile("commands.txt");
    if (inFile) {
        string cmdLine;
        ofstream tempFile; // 不写入文件

        while (getline(inFile, cmdLine)) {
            if (!cmdLine.empty()) {
                stringstream ss(cmdLine);
                char cmd;
                ss >> cmd;

                // 使用缓冲的命令代替用户输入
                stringstream buffer;
                buffer << ss.rdbuf();

                // 重定向 cin 到缓冲
                streambuf* old = cin.rdbuf(buffer.rdbuf());

                // 处理命令
                processCommand(cmd, date, accounts, tempFile);

                // 恢复 cin
                cin.rdbuf(old);
            }
        }
        inFile.close();
    }

    // 打开命令文件以追加新命令
    ofstream cmdFile("commands.txt", ios::app);

    cout << "(a)add account (d)deposit (w)withdraw (s)show (c)change day (n)next month (q)query (e)exit" << endl;
    char cmd;
    do {
        // 显示日期和总金额
        date.show();
        cout << "\tTotal: " << Account::getTotal() << "\tcommand> ";

        cin >> cmd;
        if (cmd != 'e') {
            processCommand(cmd, date, accounts, cmdFile);
        }
    } while (cmd != 'e');

    cmdFile.close();

    for_each(accounts.begin(), accounts.end(), deleter());
    return 0;
}