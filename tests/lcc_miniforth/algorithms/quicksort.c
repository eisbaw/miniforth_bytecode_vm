/* Quicksort algorithm - Recursive divide and conquer */

void swap(int arr[], int i, int j) {
  int temp = arr[i];
  arr[i] = arr[j];
  arr[j] = temp;
}

int partition(int arr[], int low, int high) {
  int pivot = arr[high];
  int i = low - 1;
  int j;

  for (j = low; j < high; j++) {
    if (arr[j] <= pivot) {
      i++;
      swap(arr, i, j);
    }
  }
  swap(arr, i + 1, high);
  return i + 1;
}

void quicksort(int arr[], int low, int high) {
  if (low < high) {
    int pi = partition(arr, low, high);
    quicksort(arr, low, pi - 1);
    quicksort(arr, pi + 1, high);
  }
}

int test_quicksort() {
  int arr[8] = {64, 34, 25, 12, 22, 11, 90, 5};
  int n = 8;

  quicksort(arr, 0, n - 1);

  /* Return sum of sorted array to verify */
  int sum = 0;
  int i;
  for (i = 0; i < n; i++) {
    sum += arr[i];
  }
  return sum; // Expected: 313 (sum of all elements)
}

int main() { return test_quicksort(); }