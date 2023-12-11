//메인 페이지의 포스트를 자동 기재하기 위한 프로그램
#include <fstream>
#include <sstream>
#include <iostream>
#include <string>
#include <filesystem>
#include <chrono>
#include <vector>
#include <algorithm>

namespace std_fs = std::filesystem;

struct PostInfo {
    std_fs::path path;
    std::string title;
    std::string category;
    std_fs::file_time_type last_modified;
};

std::vector<std::pair<std::string, std::string>> convert_to_category_list(std_fs::path const& path) {
    std::vector<std::pair<std::string, std::string>> rtrn_v;
    std::ifstream stream(path);
    std::string code, name;
    while (stream >> code >> name) {
        std::replace(name.begin(), name.end(), '_', ' ');
        rtrn_v.push_back(std::make_pair(code, name));
    }
    return rtrn_v;
}

std::string get_category_name(std::vector<std::pair<std::string, std::string>> const& list, std::string const& code) {
    auto found_it = std::find_if(list.begin(), list.end(), [&code](auto const& a) {
        return a.first == code;
    });
    if (found_it != list.cend()) return found_it->second;
    else return "";
}

std::string file_time_to_string(std_fs::file_time_type const& tp) {
    auto converted = std_fs::file_time_type::clock::to_sys(tp);
    auto floored = std::chrono::floor<std::chrono::days>(converted);
    std::chrono::year_month_day ymd = std::chrono::year_month_day{floored};
    std::ostringstream oss;
    oss << (int)ymd.year() << "/";
    oss << (unsigned int)ymd.month() << "/";
    oss << (unsigned int)ymd.day();
    return oss.str();
}

int main() {
    std::vector<PostInfo> posts;

    //Get category map
    auto category_map = convert_to_category_list("Posts/categories.txt");

    std_fs::recursive_directory_iterator posts_iterator("Posts");
    for (auto p : posts_iterator) {
        if (!p.is_directory()) continue;
        if (!std_fs::exists(p.path() / "content.html")) continue;
        PostInfo info;
        info.path = p.path();

        std::ifstream input(p.path() / "content.html");
        std::string input_line;
        std::getline(input, input_line);
        if (input_line != "<!--TITLE") continue;
        std::getline(input, input_line);
        input.close();
        info.title = input_line;

        info.category = p.path().parent_path().filename().string();

        info.last_modified = std_fs::last_write_time(p.path() / "content.html");
        posts.push_back(std::move(info));
    }
    std::sort(posts.begin(), posts.end(), [](auto const& a, auto const& b) {
        return a.last_modified < b.last_modified;
    });
    std::reverse(posts.begin(), posts.end());
    std::ofstream output_stream("content.html");
    output_stream << "<header>🕞 최근 포스트</header>\n"
    "<hr>\n"
    "<div>\n";
    for (std::size_t i = 0; i < std::min<size_t>(5, posts.size()); i++) {
        output_stream << "<section class='post_list'>\n"
        "<header>\n"
        "<a href='/";
        output_stream << posts[i].path.string() << "'>" << posts[i].title << "</a>\n"
        "</header>\n"
        "<footer>" << file_time_to_string(posts[i].last_modified) << "</footer>\n"
        "</section>\n";
    }
    output_stream << "</div>\n<hr>\n<br><br>\n";
    for (auto& category : category_map) {
        output_stream << "<header>" << category.second << "</header>\n"
        "<hr>\n"
        "<div>\n";
        std::size_t posts_count = 0;
        for (std::size_t i = 0; i < posts.size() and posts_count != 5; i++) {
            if (posts[i].category != category.first) continue;
            output_stream << "<section class='post_list'>\n"
            "<header>\n"
            "<a href='/";
            output_stream << posts[i].path.string() << "'>" << posts[i].title << "</a>\n"
            "</header>\n"
            "<footer>" << file_time_to_string(posts[i].last_modified) << "</footer>\n"
            "</section>\n";
        }
        output_stream << "</div>\n<hr>\n<br><br>\n";
    }
    output_stream.close();

    std_fs::copy_file("style.css", "Posts/style.css", std_fs::copy_options::overwrite_existing);
    output_stream.open("Posts/content.html");
    output_stream << "<header>📝 모든 포스트</header>\n"
    "<hr>\n"
    "<div>\n";
    for (std::size_t i = 0; i < posts.size(); i++) {
        output_stream << "<section class='post_list'>\n"
        "<header>\n"
        "<a href='/";
        output_stream << posts[i].path.string() << "'>" << posts[i].title << "</a>\n"
        "</header>\n"
        "<footer>" << file_time_to_string(posts[i].last_modified) << "</footer>\n"
        "</section>\n";
    }
    output_stream << "</div>\n<hr>\n<br><br>\n";
    output_stream.close();

    for (auto const& category : category_map) {
        if (!std_fs::exists("Posts/" + category.first)) {
            std_fs::create_directories("Posts/" + category.first);
            std_fs::copy_file("style.css", "Posts/" + category.first + "/style.css", std_fs::copy_options::overwrite_existing);
        }
        output_stream.open("Posts/" + category.first + "/content.html");
        output_stream << "<header>" << category.second << "</header>\n"
        "<hr>\n"
        "<div>\n";
        for (std::size_t i = 0; i < posts.size(); i++) {
            if (posts[i].category != category.first) continue;
            output_stream << "<section class='post_list'>\n"
            "<header>\n"
            "<a href='/";
            output_stream << posts[i].path.string() << "'>" << posts[i].title << "</a>\n"
            "</header>\n"
            "<footer>" << file_time_to_string(posts[i].last_modified) << "</footer>\n"
            "</section>\n";
        }
        output_stream << "</div>\n<hr>\n<br><br>\n";
        output_stream.close();
    }
return 0;
}