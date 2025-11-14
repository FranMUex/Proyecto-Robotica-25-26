//
// Created by usuario on 25/9/25.
//
#include <random>
#include <algorithm>
#include <iostream>
#include <vector>
#include <ranges>

void vrand(){
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> num(0, 10);
    std::vector<std::tuple<std::string, int>> v;

    auto show = [](auto &vec)
    {
        for (const auto &[n, i] : vec)
            std::cout << n << " " << i << std::endl;
    };

    for (int i = 0; i < 1000; i++)
    {
        int n = num(gen);
        v.push_back(std::make_tuple(std::to_string(n), n));
    }

    auto sorting = [](auto &tuple1, auto &tuple2)
    {
        return (std::get<1>(tuple1) < std::get<1>(tuple2));
    };

    auto [n, i] = std::ranges::min(v, sorting);

    auto min_elem = std::min_element(v.begin(), v.end(), sorting);

    auto [m, j] = *min_elem;

    std::cout << i << " " << std::distance(v.begin(), min_elem) << std::endl;

    std::ranges::transform(v.begin(), v.end(), v.begin(),[](auto &t)
    {
        auto [name, value] = t;
        value *= 2;
        return std::make_tuple(name, value);
    });

    show(v);

}

int main() {
    vrand();
    return 0;
}