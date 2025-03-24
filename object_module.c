#include "object_module.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NUM_COLORS 6
const char *colors[] = {"\x1B[31m", "\x1B[32m", "\x1B[33m",
                        "\x1B[34m", "\x1B[35m", "\x1B[36m"};
#define RESET "\x1B[0m"

ObjectArray *create_object_array(int initial_capacity) {
  ObjectArray *arr = (ObjectArray *)malloc(sizeof(ObjectArray));
  arr->objects = (Object *)malloc(initial_capacity * sizeof(Object));
  arr->size = 0;
  arr->capacity = initial_capacity;
  return arr;
}

void free_object_array(ObjectArray *arr) {
  free(arr->objects);
  free(arr);
}

void add_object(ObjectArray *arr, const char *name, int age, double weight) {
  if (arr->size >= arr->capacity) {
    arr->capacity *= 2;
    arr->objects =
        (Object *)realloc(arr->objects, arr->capacity * sizeof(Object));
  }
  Object *obj = &arr->objects[arr->size];
  strncpy(obj->name, name, 49);
  obj->name[49] = '\0';
  obj->age = age;
  obj->weight = weight;
  arr->size++;
}

void remove_object(ObjectArray *arr, int index) {
  if (index < 0 || index >= arr->size)
    return;
  for (int i = index; i < arr->size - 1; i++) {
    arr->objects[i] = arr->objects[i + 1];
  }
  arr->size--;
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
  }
  if (comp)
    qsort(arr->objects, arr->size, sizeof(Object), comp);
}

void print_objects(const ObjectArray *arr, FILE *stream) {
  for (int i = 0; i < arr->size; i++) {
    fprintf(stream, "Name: %s, Age: %d, Weight: %.2f\n", arr->objects[i].name,
            arr->objects[i].age, arr->objects[i].weight);
  }
}

void save_objects_to_file(const ObjectArray *arr, const char *filename) {
  FILE *file = fopen(filename, "w");
  if (file) {
    print_objects(arr, file);
    fclose(file);
  }
}

void load_objects_from_file(ObjectArray *arr, const char *filename) {
  FILE *file = fopen(filename, "r");
  if (!file)
    return;

  char line[256];
  while (fgets(line, sizeof(line), file)) {
    char name[50];
    int age;
    double weight;
    if (sscanf(line, "Name: %49[^,], Age: %d, Weight: %lf", name, &age,
               &weight) == 3) {
      add_object(arr, name, age, weight);
    }
  }
  fclose(file);
}

void print_bar_chart(const ObjectArray *arr, int field) {
  if (arr->size == 0)
    return;

  double max_val = 0;
  for (int i = 0; i < arr->size; i++) {
    double val = (field == 1) ? arr->objects[i].age : arr->objects[i].weight;
    if (val > max_val)
      max_val = val;
  }
  if (max_val <= 0)
    max_val = 1;

  const int chart_width = 80;
  int column_width = chart_width / arr->size;
  if (column_width < 1)
    column_width = 1;

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
  if (arr->size == 0)
    return;

  double total = 0;
  for (int i = 0; i < arr->size; i++) {
    total += (field == 1) ? arr->objects[i].age : arr->objects[i].weight;
  }
  if (total <= 0)
    total = 1;

  printf("Pie Chart:\n");
  for (int i = 0; i < arr->size; i++) {
    double val = (field == 1) ? arr->objects[i].age : arr->objects[i].weight;
    double percent = (val / total) * 100.0;
    printf("%s█%s %s: %.2f%%\n", colors[i % NUM_COLORS], RESET,
           arr->objects[i].name, percent);
  }
}
