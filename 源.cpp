#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <windows.h>

struct SearchResult {
    std::string filename;
    std::string keyword;
    std::string context;
    size_t position;
};

class TextSearcher {
private:
    std::vector<std::string> file_contents;
    std::vector<std::string> filenames;

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

            std::ifstream file(filename);
            if (file.is_open()) {
                std::string content((std::istreambuf_iterator<char>(file)),
                    std::istreambuf_iterator<char>());
                file_contents.push_back(content);
                filenames.push_back(filename);
                std::cout << "已加载: " << filename << std::endl;
                file.close();
            }
        } while (FindNextFileA(hFind, &findFileData) != 0);

        FindClose(hFind);
        std::cout << "加载完成，共 " << filenames.size() << " 个文件" << std::endl;
    }

    std::vector<SearchResult> search(const std::string& query) {
        std::vector<SearchResult> results;

        for (size_t i = 0; i < file_contents.size(); ++i) {
            const std::string& content = file_contents[i];
            size_t pos = 0;

            while ((pos = content.find(query, pos)) != std::string::npos) {
                SearchResult result;
                result.filename = filenames[i];
                result.keyword = query;
                result.position = pos;

                // 简单的上下文提取
                size_t start = (pos > 50) ? pos - 50 : 0;
                size_t length = 100;
                if (start + length > content.length()) {
                    length = content.length() - start;
                }
                result.context = content.substr(start, length);

                results.push_back(result);
                pos += query.length();
            }
        }

        return results;
    }

    void displayResults(const std::vector<SearchResult>& results) {
        std::cout << "搜索结果:" << std::endl;

        for (size_t i = 0; i < results.size(); ++i) {
            std::cout << (i + 1) << ". " << results[i].keyword
                << " - " << results[i].filename << std::endl;
        }
    }

    void showContext(const SearchResult& result) {
        std::cout << "上下文: " << result.context << std::endl;
    }

    bool hasFiles() const {
        return !filenames.empty();
    }
};

int main() {
    std::cout << "文本搜索程序" << std::endl;

    TextSearcher searcher;
    searcher.loadAllTxtFiles();

    if (!searcher.hasFiles()) {
        std::cout << "没有找到文件！" << std::endl;
        return 0;
    }

    std::string query;
    std::cout << "请输入搜索词: ";
    std::getline(std::cin, query);

    auto results = searcher.search(query);

    if (results.empty()) {
        std::cout << "没有找到结果" << std::endl;
        return 0;
    }

    searcher.displayResults(results);

    int choice;
    std::cout << "输入序号查看详情: ";
    std::cin >> choice;

    if (choice > 0 && choice <= static_cast<int>(results.size())) {
        searcher.showContext(results[choice - 1]);
    }

    return 0;
}