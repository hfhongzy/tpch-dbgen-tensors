#include <torch/torch.h>
#include <iostream>
#include <vector>
#include <thread>

const uint64_t rows = 6000'000'00;
const uint64_t cols = 25;
// 50s for SF100 lineitem 25-char str

char *data;

// 线程函数：拷贝一部分到 flat_data
void thread_copy(std::vector<int8_t>& flat_data, int start_row, int end_row) {
    for (uint64_t i = start_row; i < end_row; ++i) {
        for (uint64_t j = 0; j < cols; ++j) {
            flat_data[i * cols + j] = static_cast<int8_t>('a' + ((i ^ j ^ 31) % 26));
        }
    }
}

int main() {

  std::vector<int8_t> flat_data(rows * cols);
  int num_threads = std::thread::hardware_concurrency();
  std::cout << "Using " << num_threads << " threads.\n";
  std::vector<std::thread> threads;
  uint64_t rows_per_thread = rows / num_threads;
  // for (int i = 0; i < rows; ++i) {
  //     for (int j = 0; j < cols; ++j) {
  //         flat_data[i * cols + j] = static_cast<int8_t>(data[i][j]);
  //         printf("%c ", data[i][j]);
  //     }
  //     printf("\n");
  // }

  for (uint64_t t = 0; t < num_threads; ++t) {
      uint64_t start_row = t * rows_per_thread;
      uint64_t end_row = (t == num_threads - 1) ? rows : (t + 1) * rows_per_thread;
      threads.emplace_back(thread_copy, std::ref(flat_data), start_row, end_row);
  }

  for (auto& th : threads) th.join();

  torch::Tensor tensor = torch::from_blob(flat_data.data(), {rows, cols}, torch::kInt8);
  torch::save(tensor, "t.pth");

  std::cout << "Saved to t.pth\n";
  return 0;
}
