/*
 Дан массив неотрицательных целых 64-разрядных чисел.
 Количество чисел не больше 10^6. Отсортировать массив
 методом MSD по битам (бинарный QuickSort).
*/
#include <cassert>
#include <iostream>
#include <vector>

template <class T>
inline size_t ZeroCount(const size_t element_count, const uint64_t mask,
                        const std::vector<T>& array,
                        const size_t index_of_first = 0) {
  size_t zero_count = 0;
  for (size_t j = 0; j < element_count; ++j) {
    if ((array[index_of_first + j] & mask) == 0) {
      ++zero_count;
    }
  }
  return zero_count;
}

template <class T>
inline size_t SortByBit(const size_t element_count, const uint64_t mask,
                        std::vector<T>& array,
                        const size_t index_of_first = 0) {
  size_t zero_count = ZeroCount(element_count, mask, array, index_of_first);

  size_t index_of_0 = 0;
  size_t index_of_1 = zero_count;
  T* sorted_array = new T[element_count];
  for (int j = 0; j < element_count; ++j) {
    if ((array[index_of_first + j] & mask) == 0) {
      sorted_array[index_of_0] = array[index_of_first + j];
      ++index_of_0;
    } else {
      sorted_array[index_of_1] = array[index_of_first + j];
      ++index_of_1;
    }
  }

  // Copying sorted array to array
  for (int j = 0; j < element_count; ++j) {
    array[index_of_first + j] = sorted_array[j];
  }
  delete[] sorted_array;

  return zero_count;
}

template <class T>
void MSDSortOfSubarray(const size_t element_count, uint64_t mask,
                       std::vector<T>& array, const size_t index_of_first = 0) {
  if (element_count <= 1 || mask == 0) {
    return;
  }

  // First element that equals 1 (in this bit)
  size_t border_index = SortByBit(element_count, mask, array, index_of_first);

  MSDSortOfSubarray(border_index, mask >> 1, array,
                    index_of_first);  // Sort left part
  MSDSortOfSubarray(element_count - border_index, mask >> 1, array,
                    index_of_first + border_index);  // Sort right part
}

template <class T>
void MSDSort(const size_t element_count, std::vector<T>& array) {
  uint64_t mask = 1;
  mask <<= 63;
  MSDSortOfSubarray(element_count, mask, array);
}

template <class T>
void PrintArray(const size_t element_count, const std::vector<T>& array) {
  for (size_t i = 0; i < element_count; ++i) {
    std::cout << array[i] << ' ';
  }
}

int main() {
  size_t number_count = 0;
  std::cin >> number_count;
  assert(number_count <= 1000 * 1000);
  std::vector<uint64_t> array(number_count);
  for (size_t i = 0; i < number_count; ++i) {
    std::cin >> array[i];
  }

  MSDSort(number_count, array);
  PrintArray(number_count, array);

  return 0;
}
