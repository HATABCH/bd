#include "object_module.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

int add_object(ObjectArray *arr, const char *name, int age, double weight) {
  if (!arr || !name) {
    fprintf(stderr, "Invalid arguments to add_object\n");
    return 0;
  }

  if (arr->size >= arr->capacity) {
    size_t new_capacity = arr->capacity * 2;
    Object *new_objects =
        (Object *)realloc(arr->objects, new_capacity * sizeof(Object));
    if (!new_objects) {
      fprintf(stderr, "Memory reallocation failed in add_object\n");
      return -1;
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
  return 1;
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
  if (!arr || !stream || arr->size == 0) {
    fprintf(stream, "No objects to display\n");
    return;
  }

  // Заголовок таблицы
  fprintf(stream, "┌─────┬─────────────────┬───────┬─────────┐\n");
  fprintf(stream, "│ %-3s │ %-15s │ %-5s │ %-7s │\n", "ID", "Name", "Age",
          "Weight");
  fprintf(stream, "├─────┼─────────────────┼───────┼─────────┤\n");

  // Данные
  for (int i = 0; i < arr->size; i++) {
    fprintf(stream, "│ %-3d │ %-15s │ %-5d │ %-7.1f │\n", i,
            arr->objects[i].name, arr->objects[i].age, arr->objects[i].weight);
  }

  // Нижняя граница таблицы
  fprintf(stream, "└─────┴─────────────────┴───────┴─────────┘\n");
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

int load_objects_from_file(ObjectArray *arr, const char *filename) {
  if (!arr || !filename)
    return 0;

  FILE *file = fopen(filename, "r");
  if (!file) {
    fprintf(stderr, "Failed to open %s: %s\n", filename, strerror(errno));
    return -1;
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
  return 1;
}
