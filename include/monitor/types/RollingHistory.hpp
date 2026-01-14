#pragma once
#include <vector>
#include <cstddef>

namespace monitor::types {
    template <typename T>
    class RollingHistory {
    public:
        explicit RollingHistory(std::size_t max_size)
            : max_size_(max_size) {}

        bool empty() const { 
            return data_.empty(); 
        }
        std::size_t size() const { 
            return data_.size(); 
        }
        const T& back() const { 
            return data_.back(); 
        }

        const std::vector<T>& values() const { 
            return data_; 
        }

        void push(T value) {
            if (max_size_ == 0) return;

            if (data_.size() == max_size_) {
                data_.erase(data_.begin()); 
            }
            data_.push_back(value); 
        }

    private:
        std::size_t max_size_;
        std::vector<T> data_;
    };

}