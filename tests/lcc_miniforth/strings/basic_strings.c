/* Test basic string operations */

int test_string_literals() {
    char *str = "Hello";
    return str[1];  // Expected: 'e' = 101 (ASCII)
}

int test_char_array() {
    char str[6] = {'H', 'e', 'l', 'l', 'o', '\0'};
    return str[4];  // Expected: 'o' = 111 (ASCII)
}

int my_strlen(char *str) {
    int len = 0;
    while (str[len] != '\0') {
        len++;
    }
    return len;
}

int test_string_length() {
    char *str = "Test";
    return my_strlen(str);  // Expected: 4
}

int my_strcmp(char *str1, char *str2) {
    int i = 0;
    while (str1[i] != '\0' && str2[i] != '\0') {
        if (str1[i] != str2[i]) {
            return str1[i] - str2[i];
        }
        i++;
    }
    return str1[i] - str2[i];
}

int test_string_comparison() {
    char *str1 = "abc";
    char *str2 = "abc";
    return my_strcmp(str1, str2);  // Expected: 0 (equal)
}

int test_char_manipulation() {
    char ch = 'A';
    ch = ch + 32;  // Convert to lowercase
    return ch;     // Expected: 'a' = 97 (ASCII)
}

int main() {
    return test_string_length();  // Expected: 4
}