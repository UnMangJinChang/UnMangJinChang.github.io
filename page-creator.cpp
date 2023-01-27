//주어진 템플릿의 스타일과 컨텐츠 자리에 작성한 컨텐츠를 삽입하는 역할을 합니다.
//filesystem을 사용하므로 컴파일에 C++17 필요
#include <string>
#include <iostream>
#include <filesystem>
#include <fstream>

void write_content(std::filesystem::path path) {
    //하위 폴더에 페이지 작성
    std::filesystem::directory_iterator directory_iterator(path);
    for (auto entry : directory_iterator) {
        if (entry.is_directory()) write_content(entry.path());
    }
    //현재 경로의 컨텐츠 파일 확인
    std::ifstream content_fs(path / "content.html");
    std::stringstream content_ss;
    std::ifstream style_fs(path / "style.css");
    std::stringstream style_ss;
    if (content_fs) {
        //스타일과 컨텐츠를 문자열로 변환
        content_ss << content_fs.rdbuf();
        if (style_fs) style_ss << style_fs.rdbuf();
        std::ifstream template_fs("template.html");
        std::ofstream index_fs(path / "index.html");
        if (index_fs and template_fs) {
            std::string template_line;
            while (std::getline(template_fs, template_line)) {
                if (template_line.find("/*CONTENT STYLE*/") != std::string::npos) {
                    index_fs << style_ss.str() << '\n';
                }
                else if (template_line.find("<!--CONTENT HTML-->") != std::string::npos) {
                    index_fs << content_ss.str() << '\n';
                }
                else {
                    index_fs << template_line << '\n';
                }
            }
        }
        else {
            std::cerr << "Couldn't create a index.html file.";
        }
    }
    else if (std::filesystem::exists(path / "index.html")) std::filesystem::remove(path / "index.html");
}

int main(int argc, char* argv[]) {
    write_content(".");
    return 0;
}