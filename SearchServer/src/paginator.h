#pragma once

#include <iostream>
#include <vector>
#include <string>

#include "../src/document.h"
#include "../src/read_input_functions.h"

template <class RandomIterator>
class IteratorRange {
public:
    IteratorRange(const RandomIterator begin, const RandomIterator end)
        : start_range_(begin), end_range_(end)
    {
    }

    auto begin() const {
        return start_range_;
    }

    auto end() const {
        return end_range_;
    }

    size_t size() const {
        return distance(start_range_, end_range_);
    }

private:
    const RandomIterator start_range_;
    const RandomIterator end_range_;
};

template <typename Iterator>
class Paginator {
public:

    Paginator() = default;

    void Initialization(Iterator begin, Iterator end, size_t page_size){
        auto it_start = begin;
        for(auto it_end = begin; it_end != end; ++it_end){
            int check_page = distance(it_start, it_end);

            if(check_page > 0 && check_page % page_size == 0){
               it_pages_.push_back({it_start, it_end});
               it_start = it_end;
            }
        }
        it_pages_.push_back({it_start, end});
    }

    auto begin() const {
        return it_pages_.begin();
    }

    auto end() const {
        return it_pages_.end();
    }

    size_t size() const {
       return it_pages_.size();
    }

private:
    std::vector<IteratorRange<Iterator>> it_pages_;
};

template <class RandomIterator>
std::ostream& operator<<(std::ostream& output, const IteratorRange<RandomIterator>& range){
    for(auto it = range.begin(); it != range.end(); ++it){
        output << *it;
    }
    return output;
}

template <typename Container>
auto Paginate(const Container& container, size_t page_size) {
    Paginator <decltype(std::begin(container))> paginator;
    paginator.Initialization(container.begin(), container.end(), page_size);

    return paginator;
}
