//This code is supposed to be run every time you write a new post.
//실행하면 Posts의 모든 하위 list.txt를 갱신합니다.

#include <filesystem>
#include <fstream>
#include <iostream>
#include <vector>
#include <algorithm>
#include <chrono>

//Returns all the posts in the given directory and modify list.txt
std::vector<std::pair<std::filesystem::path, std::filesystem::file_time_type>> getPosts(const std::filesystem::path& path) {
    std::filesystem::directory_iterator dit(path);
    std::vector<std::pair<std::filesystem::path, std::filesystem::file_time_type>> rtrn;
    //Check if there's a 'list.txt' file
    bool listExists = false;
    for (auto& d : dit) {
        if (d.is_directory()) {
            auto subposts = getPosts(d.path());
            for (auto& subpost : subposts) {
                subpost.first = d.path().stem() / subpost.first;
            }
            rtrn.insert(rtrn.cend(), subposts.cbegin(), subposts.cend());
        }
        else if (d.is_regular_file()) {
            if (d.path().extension().string() == ".html") {
                rtrn.push_back({d.path().filename(), d.last_write_time()});
            }
            else if (!listExists and d.path().filename().string() == "list.txt") {
                listExists = true;
            }
        }
    }
    //Sort the file directories by the last write times
    std::sort(rtrn.begin(), rtrn.end(), [](const std::pair<std::filesystem::path, std::filesystem::file_time_type>& a, const std::pair<std::filesystem::path, std::filesystem::file_time_type>& b)->bool {
        return a.second < b.second;
    });
    //list.txt 초기화 혹은 생성
    std::ofstream ofs(path / "list.txt");
    for (auto crit = rtrn.crbegin(); crit < rtrn.crend(); crit++) {
        ofs << crit->first.string() << '\n';
    }
    ofs.close();
    return rtrn;
}

int main(int argc, char* argv[]) {
    getPosts("./Posts");
    return 0;
}