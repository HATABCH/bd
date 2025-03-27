#include "object_module.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NUM_COLORS 12
const char *colors[] = {"\x1B[31m", "\x1B[32m", "\x1B[33m", "\x1B[34m",
                        "\x1B[35m", "\x1B[36m", "\x1B[91m", "\x1B[92m",
                        "\x1B[93m", "\x1B[94m", "\x1B[95m", "\x1B[96m"};
#define RESET "\x1B[0m"

ObjectArray *create_object_array(int initial_capacity) {
  if (initial_capacity <= 0) {
    fprintf(stderr, "Error: Initial capacity must be positive\n");
    return NULL;
  }

  ObjectArray *arr = (ObjectArray *)malloc(sizeof(ObjectArray));
  if (!arr) {
    fprintf(stderr, "Memory allocation failed for ObjectArray\n");
    return NULL;
  }

  arr->objects = (Object *)malloc(initial_capacity * sizeof(Object));
  if (!arr->objects) {
    fprintf(stderr, "Memory allocation failed for objects\n");
    free(arr);
    return NULL;
  }

  arr->size = 0;
  arr->capacity = initial_capacity;
  return arr;
}

void free_object_array(ObjectArray *arr) {
  if (arr) {
    free(arr->objects);
    free(arr);
  }
}

void add_object(ObjectArray *arr, const char *name, int age, double weight) {
  if (!arr || !name) {
    fprintf(stderr, "Invalid arguments to add_object\n");
    return;
  }

  if (arr->size >= arr->capacity) {
    size_t new_capacity = arr->capacity * 2;
    Object *new_objects =
        (Object *)realloc(arr->objects, new_capacity * sizeof(Object));
    if (!new_objects) {
      fprintf(stderr, "Memory reallocation failed in add_object\n");
      return;
    }
    arr->objects = new_objects;
    arr->capacity = new_capacity;
  }

  Object *obj = &arr->objects[arr->size];
  strncpy(obj->name, name, sizeof(obj->name) - 1);
  obj->name[sizeof(obj->name) - 1] = '\0';
  obj->age = age;
  obj->weight = weight;
  arr->size++;
}

void remove_object(ObjectArray *arr, int index) {
  if (!arr || index < 0 || index >= arr->size) {
    fprintf(stderr, "Invalid index in remove_object: %d\n", index);
    return;
  }

  for (int i = index; i < arr->size - 1; i++) {
    arr->objects[i] = arr->objects[i + 1];
  }
  arr->size--;

  // Reduce capacity if utilization drops below 25%
  if (arr->size > 0 && arr->size <= arr->capacity / 4) {
    size_t new_capacity = arr->capacity / 2;
    Object *new_objects =
        (Object *)realloc(arr->objects, new_capacity * sizeof(Object));
    if (new_objects) {
      arr->objects = new_objects;
      arr->capacity = new_capacity;
    }
  }
}

int compare_name_asc(const void *a, const void *b) {
  return strcmp(((Object *)a)->name, ((Object *)b)->name);
}

int compare_name_desc(const void *a, const void *b) {
  return -compare_name_asc(a, b);
}

int compare_age_asc(const void *a, const void *b) {
  return ((Object *)a)->age - ((Object *)b)->age;
}

int compare_age_desc(const void *a, const void *b) {
  return -compare_age_asc(a, b);
}

int compare_weight_asc(const void *a, const void *b) {
  double diff = ((Object *)a)->weight - ((Object *)b)->weight;
  return (diff > 0) ? 1 : (diff < 0) ? -1 : 0;
}

int compare_weight_desc(const void *a, const void *b) {
  return -compare_weight_asc(a, b);
}
void sort_objects(ObjectArray *arr, int field, int ascending) {
  if (!arr || field < 0 || field > 2) {
    fprintf(stderr, "Invalid arguments to sort_objects\n");
    return;
  }

  int (*comp)(const void *, const void *) = NULL;
  switch (field) {
  case 0:
    comp = ascending ? compare_name_asc : compare_name_desc;
    break;
  case 1:
    comp = ascending ? compare_age_asc : compare_age_desc;
    break;
  case 2:
    comp = ascending ? compare_weight_asc : compare_weight_desc;
    break;
  default:
    return;
  }
  qsort(arr->objects, arr->size, sizeof(Object), comp);
}

void print_objects(const ObjectArray *arr, FILE *stream) {
  if (!arr || !stream)
    return;
  for (int i = 0; i < arr->size; i++) {
    fprintf(stream, "Name: %s, Age: %d, Weight: %.2f\n", arr->objects[i].name,
            arr->objects[i].age, arr->objects[i].weight);
  }
}

void save_objects_to_file(const ObjectArray *arr, const char *filename) {
  if (!arr || !filename)
    return;

  FILE *file = fopen(filename, "w");
  if (!file) {
    fprintf(stderr, "Failed to open %s: %s\n", filename, strerror(errno));
    return;
  }

  print_objects(arr, file);
  if (fclose(file) != 0) {
    fprintf(stderr, "Failed to close %s: %s\n", filename, strerror(errno));
  }
}

void load_objects_from_file(ObjectArray *arr, const char *filename) {
  if (!arr || !filename)
    return;

  FILE *file = fopen(filename, "r");
  if (!file) {
    fprintf(stderr, "Failed to open %s: %s\n", filename, strerror(errno));
    return;
  }

  char line[512];
  int line_num = 0;
  while (fgets(line, sizeof(line), file)) {
    line_num++;
    char name[50] = {0};
    int age = 0;
    double weight = 0.0;

    if (sscanf(line, "Name: %49[^,], Age: %d, Weight: %lf", name, &age,
               &weight) == 3) {
      add_object(arr, name, age, weight);
    } else {
      fprintf(stderr, "Invalid format in line %d: %s", line_num, line);
    }
  }

  if (ferror(file)) {
    fprintf(stderr, "Error reading file %s\n", filename);
  }
  fclose(file);
}

void print_bar_chart(const ObjectArray *arr, int field) {
  if (!arr || arr->size == 0) {
    fprintf(stderr, "No data for bar chart\n");
    return;
  }

  double max_val = 0;
  for (int i = 0; i < arr->size; i++) {
    double val = (field == 1) ? arr->objects[i].age : arr->objects[i].weight;
    if (val > max_val)
      max_val = val;
  }

  if (max_val <= 0) {
    printf("All values are zero or negative\n");
    return;
  }

  const int chart_width = 80;
  int column_width = chart_width / arr->size;
  column_width = column_width > 0 ? column_width : 1;

  const int height = 20;
  for (int h = height; h > 0; h--) {
    for (int i = 0; i < arr->size; i++) {
      double val = (field == 1) ? arr->objects[i].age : arr->objects[i].weight;
      int bar_height = (int)((val / max_val) * height);

      printf("%s", colors[i % NUM_COLORS]);
      for (int w = 0; w < column_width; w++) {
        printf(bar_height >= h ? "█" : " ");
      }
      printf(RESET);
    }
    printf("\n");
  }

  printf("\nLegend:\n");
  for (int i = 0; i < arr->size; i++) {
    printf("%s█%s %s: %s (%.2f)\n", colors[i % NUM_COLORS], RESET,
           arr->objects[i].name, field == 1 ? "Age" : "Weight",
           field == 1 ? (double)arr->objects[i].age : arr->objects[i].weight);
  }
}

void print_pie_chart(const ObjectArray *arr, int field) {
  if (!arr || arr->size == 0) {
    fprintf(stderr, "No data for pie chart\n");
    return;
  }

  if (field != 1 && field != 2) {
    fprintf(stderr, "Invalid field. Use 1 for Age or 2 for Weight\n");
    return;
  }

  double total = 0;
  for (int i = 0; i < arr->size; i++) {
    double val = (field == 1) ? arr->objects[i].age : arr->objects[i].weight;
    if (val < 0) {
      fprintf(stderr, "Negative values are not supported\n");
      return;
    }
    total += val;
  }

  if (total <= 0) {
    fprintf(stderr, "Total must be positive\n");
    return;
  }

  printf("\nPie Chart (%s):\n", field == 1 ? "Age" : "Weight");
  for (int i = 0; i < arr->size; i++) {
    double val = (field == 1) ? arr->objects[i].age : arr->objects[i].weight;
    double percent = (val / total) * 100.0;
    printf("%s█%s %-20s: %6.2f%% %s(%g)%s\n", colors[i % NUM_COLORS], RESET,
           arr->objects[i].name, percent, "\x1B[2m", val, RESET);
  }
  printf("\n");
}
