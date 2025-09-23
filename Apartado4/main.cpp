#include <algorithm>
#include <iostream>
#include <vector>
void apt4a() {
    std::vector<std::tuple<std::string, int>> v;
    for (int i = 0; i < 10000000; i++) {
        v.push_back(std::make_tuple(std::to_string(i), i));
    }

    auto sorting = [](std::tuple<std::string, int> tuple1, std::tuple<std::string, int> tuple2) {
        if (std::get<1>(tuple1) <= std::get<1>(tuple2)) {
            return true;
        } else  {
            return false;
        }
    };
    std::sort(v.begin(), v.end(), sorting);
    auto show = [](std::vector<std::tuple<std::string, int>> vec) {
        for (int i = 0; i < vec.size(); i++) {
            std::cout << std::get<0>(vec[i]) << " " << std::get<1>(vec[i]) << std::endl;
        }
    };
    show(v);
}

void apt4b() {
    float degree = 0.0;
    std::cout << "Please enter a degree: ";
    std::cin >> degree;
    auto raddegree = [](float degree) {
        float rad = (degree*(std::numbers::pi))/180.0;
        return rad;
    };
    float rad = raddegree(degree);
    std::cout << rad << "radians";
}
// TIP To <b>Run</b> code, press <shortcut actionId="Run"/> or click the <icon src="AllIcons.Actions.Execute"/> icon in the gutter.
int main() {
    apt4a();
    return 0;
    // TIP See CLion help at <a href="https://www.jetbrains.com/help/clion/">jetbrains.com/help/clion/</a>. Also, you can try interactive lessons for CLion by selecting 'Help | Learn IDE Features' from the main menu.
}