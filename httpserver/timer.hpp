#ifndef HTTP_SREVER_BUFFER_HPP_
#define HTTP_SREVER_BUFFER_HPP_

#include <chrono>
#include <queue>
#include <vector>
#include <thread>
#include <functional>
#include <utility>

namespace httpserver {

using pair_t = std::pair<std::chrono::high_resolution_clock::time_point,std::function<void()>>;
using container_t = std::vector<pair_t>;
auto comp = [](const pair_t& e1, const pair_t& e2)
            { return e1.first > e2.first; };

class Timer {
public:
  Timer(const Timer&) = delete;
  Timer& operator=(const Timer&) = delete;

  explicit Timer(std::chrono::seconds sec, std::function<void()> fun) : time_(comp) {
    time_.push(std::make_pair(sec + std::chrono::high_resolution_clock::now(), fun));
  }
  void insert(std::chrono::seconds sec, std::function<void()> fun) {
    time_.push(std::make_pair(sec + std::chrono::high_resolution_clock::now(), fun));
  }
  void run() {
    while(!time_.empty()) {
      auto interval = time_.top().first - std::chrono::high_resolution_clock::now();
      if (interval.count() < 0) {
        time_.top().second();
        time_.pop();
      }
      else {
        std::this_thread::sleep_for(interval);
      }
    }
  }

private:
  std::priority_queue<pair_t, container_t, decltype(comp)> time_;
};
} // namespace httpserver
#endif // HTTP_SREVER_BUFFER_HPP_
