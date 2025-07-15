/* Merge sort algorithm - Tests divide and conquer with memory allocation */

void merge(int arr[], int temp[], int left, int mid, int right) {
    int i = left, j = mid + 1, k = left;
    
    /* Merge two sorted subarrays */
    while (i <= mid && j <= right) {
        if (arr[i] <= arr[j]) {
            temp[k++] = arr[i++];
        } else {
            temp[k++] = arr[j++];
        }
    }
    
    /* Copy remaining elements */
    while (i <= mid) {
        temp[k++] = arr[i++];
    }
    while (j <= right) {
        temp[k++] = arr[j++];
    }
    
    /* Copy back to original array */
    for (i = left; i <= right; i++) {
        arr[i] = temp[i];
    }
}

void merge_sort_helper(int arr[], int temp[], int left, int right) {
    if (left < right) {
        int mid = left + (right - left) / 2;
        
        merge_sort_helper(arr, temp, left, mid);
        merge_sort_helper(arr, temp, mid + 1, right);
        merge(arr, temp, left, mid, right);
    }
}

void merge_sort(int arr[], int n) {
    int temp[20];  /* Temporary array for merging */
    merge_sort_helper(arr, temp, 0, n - 1);
}

int test_merge_sort() {
    int arr[12] = {38, 27, 43, 3, 9, 82, 10, 15, 7, 23, 44, 1};
    int n = 12;
    int i;
    
    merge_sort(arr, n);
    
    /* Verify sorted order and return sum */
    int sum = 0;
    int is_sorted = 1;
    
    for (i = 0; i < n; i++) {
        sum += arr[i];
        if (i > 0 && arr[i] < arr[i-1]) {
            is_sorted = 0;
        }
    }
    
    return is_sorted ? sum : -1;  /* Expected: 312 if sorted correctly */
}

int main() {
    return test_merge_sort();
}