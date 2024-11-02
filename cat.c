#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <json-c/json.h>

// Function to convert a number from a given base to decimal
long long decode_value(const char *value, int base) {
    long long result = 0;
    int length = strlen(value);
    for (int i = 0; i < length; i++) {
        char digit = value[i];
        int numeric_value = (digit >= '0' && digit <= '9') ? (digit - '0') :
                            (digit >= 'A' && digit <= 'Z') ? (digit - 'A' + 10) :
                            (digit - 'a' + 10);
        result = result * base + numeric_value;
    }
    return result;
}

// Function to parse JSON and retrieve roots
void parse_json(const char *filename, int *n, int *k, int *x_vals, long long *y_vals) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        fprintf(stderr, "Could not open file\n");
        exit(1);
    }

    struct json_object *parsed_json;
    struct json_object *keys_obj, *n_obj, *k_obj, *root_obj, *base_obj, *value_obj;
    struct json_object *root;

    // Read and parse JSON
    char buffer[1024];
    fread(buffer, 1024, 1, file);
    fclose(file);
    parsed_json = json_tokener_parse(buffer);

    // Extract 'n' and 'k'
    json_object_object_get_ex(parsed_json, "keys", &keys_obj);
    json_object_object_get_ex(keys_obj, "n", &n_obj);
    json_object_object_get_ex(keys_obj, "k", &k_obj);
    *n = json_object_get_int(n_obj);
    *k = json_object_get_int(k_obj);

    // Extract roots (x, y)
    for (int i = 1; i <= *n; i++) {
        char key[3];
        snprintf(key, sizeof(key), "%d", i);
        json_object_object_get_ex(parsed_json, key, &root_obj);
        
        json_object_object_get_ex(root_obj, "base", &base_obj);
        json_object_object_get_ex(root_obj, "value", &value_obj);

        int base = json_object_get_int(base_obj);
        const char *value_str = json_object_get_string(value_obj);
        
        x_vals[i - 1] = i;
        y_vals[i - 1] = decode_value(value_str, base);
    }
}

// Function to calculate the constant term 'c' using Lagrange interpolation
long long calculate_constant(int k, int *x_vals, long long *y_vals) {
    long long c = 0;
    for (int i = 0; i < k; i++) {
        long long term = y_vals[i];
        for (int j = 0; j < k; j++) {
            if (i != j) {
                term = term * x_vals[j] / (x_vals[j] - x_vals[i]);
            }
        }
        c += term;
    }
    return c;
}

int main() {
    int n, k;
    int x_vals[10];               // assuming n <= 10
    long long y_vals[10];

    // Parse JSON to get roots
    parse_json("input.json", &n, &k, x_vals, y_vals);

    // Calculate the constant term 'c'
    long long constant_term = calculate_constant(k, x_vals, y_vals);

    // Output the result
    printf("The constant term (secret) is: %lld\n", constant_term);

    return 0;
}

