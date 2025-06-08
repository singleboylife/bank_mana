#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <windows.h>

struct SearchResult {
    std::string filename;
    std::string bookname;
    std::string keyword;
    int page;
    std::string chapter;
    std::string context;
    size_t position;
};

class TextSearcher {
private:
    std::vector<std::string> file_contents;
    std::vector<std::string> filenames;

    // 从文件名提取书名
    std::string extractBookName(const std::string& filename) {
        std::string bookname = filename;
        if (bookname.size() > 4 && bookname.substr(bookname.size() - 4) == ".txt") {
            bookname = bookname.substr(0, bookname.size() - 4);
        }
        return bookname;
    }

    // 简单的章节查找
    std::string findChapter(const std::string& content, size_t position) {
        size_t start = (position > 1000) ? position - 1000 : 0;
        std::string searchText = content.substr(start, position - start);

        size_t chapterPos = searchText.rfind("Chapter");
        if (chapterPos == std::string::npos) {
            chapterPos = searchText.rfind("CHAPTER");
        }

        if (chapterPos != std::string::npos) {
            size_t lineStart = chapterPos;
            size_t lineEnd = searchText.find('\n', chapterPos);
            if (lineEnd == std::string::npos) lineEnd = searchText.length();

            std::string chapterLine = searchText.substr(lineStart, lineEnd - lineStart);
            return chapterLine.length() > 50 ? "Chapter" : chapterLine;
        }

        return "Chapter 1";
    }

    // 估算页码
    int calculatePage(size_t position) {
        return (position / 2000) + 1;
    }

    // 提取更好的上下文（按句子边界）
    std::string extractBetterContext(const std::string& content, size_t position) {
        // 向前查找句子开始
        size_t start = position;
        int sentences_back = 0;
        while (start > 0 && sentences_back < 3) {
            start--;
            if (content[start] == '.' || content[start] == '!' || content[start] == '?') {
                sentences_back++;
                if (sentences_back == 2) {
                    // 跳过标点和空格
                    while (start < content.length() - 1 &&
                        (content[start + 1] == ' ' || content[start + 1] == '\n' || content[start + 1] == '\r')) {
                        start++;
                    }
                    start++;
                    break;
                }
            }
            if (position - start > 400) break; // 限制最大长度
        }

        // 向后查找句子结束
        size_t end = position;
        int sentences_forward = 0;
        while (end < content.length() && sentences_forward < 3) {
            if (content[end] == '.' || content[end] == '!' || content[end] == '?') {
                sentences_forward++;
                if (sentences_forward == 2) {
                    end++; // 包含标点符号
                    break;
                }
            }
            end++;
            if (end - position > 400) break; // 限制最大长度
        }

        // 确保不超出边界
        if (start >= content.length()) start = 0;
        if (end > content.length()) end = content.length();
        if (end <= start) return "";

        std::string result = content.substr(start, end - start);

        // 清理格式但保持可读性
        std::string cleaned;
        bool lastWasSpace = false;
        for (char c : result) {
            if (c == '\r') continue; // 跳过回车符
            if (c == '\n') {
                if (!lastWasSpace) {
                    cleaned += ' ';
                    lastWasSpace = true;
                }
            }
            else if (c == '\t') {
                if (!lastWasSpace) {
                    cleaned += ' ';
                    lastWasSpace = true;
                }
            }
            else if (c == ' ') {
                if (!lastWasSpace) {
                    cleaned += ' ';
                    lastWasSpace = true;
                }
            }
            else {
                cleaned += c;
                lastWasSpace = false;
            }
        }

        // 去掉开头和结尾的空格
        while (!cleaned.empty() && cleaned[0] == ' ') {
            cleaned.erase(0, 1);
        }
        while (!cleaned.empty() && cleaned.back() == ' ') {
            cleaned.pop_back();
        }

        return cleaned;
    }

public:
    void loadAllTxtFiles() {
        std::cout << "正在扫描txt文件..." << std::endl;

        WIN32_FIND_DATAA findFileData;
        HANDLE hFind = FindFirstFileA("*.txt", &findFileData);

        if (hFind == INVALID_HANDLE_VALUE) {
            std::cout << "没有找到txt文件" << std::endl;
            return;
        }

        do {
            std::string filename = findFileData.cFileName;
            std::cout << "正在读取: " << filename << std::endl;

            // 使用默认文本模式读取文件
            std::ifstream file(filename);
            if (file.is_open()) {
                std::string content((std::istreambuf_iterator<char>(file)),
                    std::istreambuf_iterator<char>());
                file_contents.push_back(content);
                filenames.push_back(filename);
                std::cout << "已加载: " << filename << " (大小: " << content.size() << " 字节)" << std::endl;
                file.close();
            }
            else {
                std::cout << "无法打开文件: " << filename << std::endl;
            }
        } while (FindNextFileA(hFind, &findFileData) != 0);

        FindClose(hFind);
        std::cout << "加载完成，共 " << filenames.size() << " 个文件" << std::endl << std::endl;
    }

    std::vector<SearchResult> search(const std::string& query) {
        std::vector<SearchResult> results;
        std::cout << "开始搜索 \"" << query << "\"..." << std::endl;

        for (size_t i = 0; i < file_contents.size(); ++i) {
            std::cout << "搜索文件 " << (i + 1) << "/" << file_contents.size() << ": " << filenames[i] << std::endl;

            const std::string& content = file_contents[i];
            size_t pos = 0;
            int count = 0;

            while ((pos = content.find(query, pos)) != std::string::npos) {
                SearchResult result;
                result.filename = filenames[i];
                result.bookname = extractBookName(filenames[i]);
                result.keyword = query;
                result.position = pos;
                result.page = calculatePage(pos);
                result.chapter = findChapter(content, pos);

                // 使用改进的上下文提取
                result.context = extractBetterContext(content, pos);

                results.push_back(result);
                pos += query.length();
                count++;

                if (count % 100 == 0) {
                    std::cout << "已找到 " << count << " 个结果..." << std::endl;
                }
            }

            std::cout << "在 " << filenames[i] << " 中找到 " << count << " 个结果" << std::endl;
        }

        std::cout << "搜索完成，总共找到 " << results.size() << " 个结果" << std::endl;

        // 按页码排序
        std::cout << "正在排序结果..." << std::endl;
        std::sort(results.begin(), results.end(),
            [](const SearchResult& a, const SearchResult& b) {
                if (a.bookname != b.bookname) return a.bookname < b.bookname;
                return a.page < b.page;
            });

        return results;
    }

    void displayResults(const std::vector<SearchResult>& results) {
        std::cout << "\n人名/地名输出:" << std::endl;

        if (results.size() > 50) {
            std::cout << "找到 " << results.size() << " 个结果，是否显示所有？(y/n，默认显示前50个): ";
            std::string choice;
            std::getline(std::cin, choice);

            size_t displayCount = (choice == "y" || choice == "Y") ? results.size() : 50;

            for (size_t i = 0; i < displayCount; ++i) {
                std::cout << (i + 1) << ". " << results[i].keyword
                    << " 页码" << results[i].page
                    << " " << results[i].chapter
                    << " " << results[i].bookname << std::endl;
            }

            if (displayCount < results.size()) {
                std::cout << "... (还有 " << (results.size() - displayCount) << " 个结果未显示)" << std::endl;
            }
        }
        else {
            for (size_t i = 0; i < results.size(); ++i) {
                std::cout << (i + 1) << ". " << results[i].keyword
                    << " 页码" << results[i].page
                    << " " << results[i].chapter
                    << " " << results[i].bookname << std::endl;
            }
        }
        std::cout << std::endl;
    }

    void showContext(const SearchResult& result) {
        // 临时切换到UTF-8编码显示 英文的 上下文免得乱码 
        UINT oldOutputCP = GetConsoleOutputCP();
        UINT oldInputCP = GetConsoleCP();

        SetConsoleOutputCP(65001);  // UTF-8
        SetConsoleCP(65001);        // UTF-8

        std::cout << result.context << std::endl << std::endl;

        // 切换回ANSI编码
        SetConsoleOutputCP(oldOutputCP);
        SetConsoleCP(oldInputCP);
    }

    bool hasFiles() const {
        return !filenames.empty();
    }
};

int main() {
    // 设置控制台为ANSI编码（中文GBK）
    SetConsoleOutputCP(936);  // GBK编码
    SetConsoleCP(936);        // GBK编码

    std::cout << "文本搜索程序启动" << std::endl;

    TextSearcher searcher;
    searcher.loadAllTxtFiles();

    if (!searcher.hasFiles()) {
        std::cout << "没有找到txt文件！" << std::endl;
        system("pause");
        return 0;
    }

    while (true) {
        std::string query;
        std::cout << "请输入搜索词 (输入exit退出): ";
        std::getline(std::cin, query);

        if (query == "exit") {
            break;
        }

        if (query.empty()) {
            continue;
        }

        auto results = searcher.search(query);

        if (results.empty()) {
            std::cout << "没有找到结果" << std::endl;
            continue;
        }

        searcher.displayResults(results);

        while (true) {
            std::cout << "输入序号查看上下文 (0返回): ";
            int choice;
            std::cin >> choice;
            std::cin.ignore();

            if (choice == 0) {
                break;
            }
            else if (choice > 0 && choice <= static_cast<int>(results.size())) {
                searcher.showContext(results[choice - 1]);
            }
            else {
                std::cout << "无效序号" << std::endl;
            }
        }
    }

    return 0;
}