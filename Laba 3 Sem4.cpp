// Laba 3 Sem4.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//
#include <iostream>
#include <vector>
#include <algorithm>
#include <random>
#include <chrono>
#include <cassert>
#include <thread>

// Функція для генерації випадкових чисел у заданому діапазоні
void fillArrayWithRandomNumbers(std::vector<int>& arr, int min, int max)
{
    std::random_device rd;
    std::mt19937 rng(rd());
    std::uniform_int_distribution<int> uni(min, max);

    for (int& num : arr) {
        num = uni(rng);
    }
}

// Функція для обміну значень двох елементів масиву
void swap(int& a, int& b)
{
    int temp = a;
    a = b;
    b = temp;
}

// Функція для розділення масиву у Quicksort
int partition(std::vector<int>& arr, int low, int high)
{
    int pivot = arr[high];
    int i = low - 1;

    for (int j = low; j <= high - 1; j++) {
        if (arr[j] < pivot) {
            i++;
            swap(arr[i], arr[j]);
        }
    }

    swap(arr[i + 1], arr[high]);
    return i + 1;
}

// Послідовне сортування Quicksort
void quickSortSequential(std::vector<int>& arr, int low, int high)
{
    if (low < high) {
        int pi = partition(arr, low, high);

        quickSortSequential(arr, low, pi - 1);
        quickSortSequential(arr, pi + 1, high);
    }
}

// Паралельне сортування Quicksort з використанням паралельних регіонів OpenMP
void quickSortParallel(std::vector<int>& arr, int low, int high, int numThreads)
{
    if (low < high) {
        int pi = partition(arr, low, high);

        if (numThreads > 1) {
            if (numThreads >= 2) {
#pragma omp parallel sections
                {
#pragma omp section
                    quickSortParallel(arr, low, pi - 1, numThreads / 2);
#pragma omp section
                    quickSortParallel(arr, pi + 1, high, numThreads - numThreads / 2);
                }
            }
            else {
                quickSortParallel(arr, low, pi - 1, 1);
                quickSortParallel(arr, pi + 1, high, 1);
            }
        }
        else {
            quickSortSequential(arr, low, pi - 1);
            quickSortSequential(arr, pi + 1, high);
        }
    }
}

// Послідовне сортування Merge Sort
void mergeSortSequential(std::vector<int>& arr, int low, int high)
{
    if (low < high) {
        int mid = low + (high - low) / 2;

        mergeSortSequential(arr, low, mid);
        mergeSortSequential(arr, mid + 1, high);

        std::vector<int> mergedArr(high - low + 1);

        int i = low;
        int j = mid + 1;
        int k = 0;

        while (i <= mid && j <= high) {
            if (arr[i] <= arr[j]) {
                mergedArr[k] = arr[i];
                i++;
            }
            else {
                mergedArr[k] = arr[j];
                j++;
            }
            k++;
        }

        while (i <= mid) {
            mergedArr[k] = arr[i];
            i++;
            k++;
        }

        while (j <= high) {
            mergedArr[k] = arr[j];
            j++;
            k++;
        }

        for (int m = 0; m < k; m++) {
            arr[low + m] = mergedArr[m];
        }
    }
}

// Паралельне сортування Merge Sort з використанням паралельних регіонів OpenMP
void mergeSortParallel(std::vector<int>& arr, int low, int high, int numThreads)
{
    if (low < high) {
        if (numThreads > 1) {
            int mid = low + (high - low) / 2;

#pragma omp parallel sections
            {
#pragma omp section
                mergeSortParallel(arr, low, mid, numThreads / 2);
#pragma omp section
                mergeSortParallel(arr, mid + 1, high, numThreads - numThreads / 2);
            }

            std::vector<int> mergedArr(high - low + 1);

            int i = low;
            int j = mid + 1;
            int k = 0;

            while (i <= mid && j <= high) {
                if (arr[i] <= arr[j]) {
                    mergedArr[k] = arr[i];
                    i++;
                }
                else {
                    mergedArr[k] = arr[j];
                    j++;
                }
                k++;
            }

            while (i <= mid) {
                mergedArr[k] = arr[i];
                i++;
                k++;
            }

            while (j <= high) {
                mergedArr[k] = arr[j];
                j++;
                k++;
            }

            for (int m = 0; m < k; m++) {
                arr[low + m] = mergedArr[m];
            }
        }
        else {
            mergeSortSequential(arr, low, high);
        }
    }
}

// Послідовне сортування Heap Sort
void heapify(std::vector<int>& arr, int n, int i)
{
    int largest = i;
    int left = 2 * i + 1;
    int right = 2 * i + 2;

    if (left < n && arr[left] > arr[largest]) {
        largest = left;
    }

    if (right < n && arr[right] > arr[largest]) {
        largest = right;
    }

    if (largest != i) {
        swap(arr[i], arr[largest]);
#pragma omp parallel
        {
#pragma omp single nowait
            heapify(arr, n, largest);
        }
    }
}
void heapSortSequential(std::vector<int>& arr)
{
    int n = arr.size();

    for (int i = n / 2 - 1; i >= 0; --i) {
        heapify(arr, n, i);
    }

    for (int i = n - 1; i > 0; --i) {
        std::swap(arr[0], arr[i]);

        heapify(arr, i, 0);
    }
}

// Паралельне сортування Heap Sort з використанням паралельних регіонів OpenMP
void heapSortParallel(std::vector<int>& arr, int numThreads)
{
    int n = arr.size();

#pragma omp parallel num_threads(numThreads)
    {
#pragma omp single
        {
            // Побудова купи (heapify)
            for (int i = n / 2 - 1; i >= 0; i--) {
#pragma omp task
                heapify(arr, n, i);
            }
        }
    }
}

int main()
{
    int size;
    std::cout << "Enter the number of random numbers to generate and sort: ";
    std::cin >> size;

    std::vector<int> arr(size);
    fillArrayWithRandomNumbers(arr, 1, size);

    std::vector<int> arr1 = arr;
    std::vector<int> arr2 = arr;
    std::vector<int> arr3 = arr;

    std::cout << "Sorting " << size << " numbers sequentially:\n";

    auto start = std::chrono::high_resolution_clock::now();
    quickSortSequential(arr1, 0, size - 1);
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end - start;
    std::cout << "Quicksort: " << duration.count() << " seconds\n";

    start = std::chrono::high_resolution_clock::now();
    mergeSortSequential(arr2, 0, size - 1);
    end = std::chrono::high_resolution_clock::now();
    duration = end - start;
    std::cout << "Merge Sort: " << duration.count() << " seconds\n";

    start = std::chrono::high_resolution_clock::now();
    heapSortSequential(arr3);
    end = std::chrono::high_resolution_clock::now();
    duration = end - start;
    std::cout << "Heap Sort: " << duration.count() << " seconds\n";

    std::cout << "\nSorting " << size << " numbers in parallel:\n";

    int numThreads = std::thread::hardware_concurrency();

    arr1 = arr;
    arr2 = arr;
    arr3 = arr;

    start = std::chrono::high_resolution_clock::now();
    quickSortParallel(arr1, 0, size - 1, numThreads);
    end = std::chrono::high_resolution_clock::now();
    duration = end - start;
    std::cout << "Quicksort: " << duration.count() << " seconds\n";

    start = std::chrono::high_resolution_clock::now();
    mergeSortParallel(arr2, 0, size - 1, numThreads);
    end = std::chrono::high_resolution_clock::now();
    duration = end - start;
    std::cout << "Merge Sort: " << duration.count() << " seconds\n";

    start = std::chrono::high_resolution_clock::now();
    heapSortParallel(arr3, numThreads);
    end = std::chrono::high_resolution_clock::now();
    duration = end - start;
    std::cout << "Heap Sort: " << duration.count() << " seconds\n";

    return 0;
}

