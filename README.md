# TPC-H DBGen for PyTorch Tensors

This is a modified version of classic ssb-dbgen that directly generates `.pth` tensor files for each column used in TPC-H queries.

The data format follows the TQP paper.

- Integer: `torch.int64`
- Floating-point: `torch.float64`
- String: `torch.int8` (ASCII code)
- Date: `torch.int32` (the number of days since `1990-01-01`)


It is functionally equivalent to first generating `.csv` files and then converting them, but significantly faster and consumes less memory.

Here's an example of generating scale factor (SF) 1.

Before start, modify the `const std::string SAVE_ROOT` in `print.cpp`. Then, download [libtorch](https://pytorch.org/get-started/locally/) and config your own path in `CMakeLists.txt`.

```bash
mkdir -p build/
cd build/
cmake ..
make -j
./dbgen -s 1
```

