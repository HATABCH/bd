#ifndef OBJECT_MODULE_H
#define OBJECT_MODULE_H
#include "SDL.h"
#include <stdio.h>
typedef struct {
  char name[50];
  int age;
  double weight;
} Object;

typedef struct {
  Object *objects;
  int size;
  int capacity;
} ObjectArray;

int find_object_by_name(ObjectArray *arr, const char *name);
ObjectArray *create_object_array(int initial_capacity);
void free_object_array(ObjectArray *arr);
int add_object(ObjectArray *arr, const char *name, int age, double weight);
void remove_object(ObjectArray *arr, int index);
void sort_objects(ObjectArray *arr, int field, int ascending);
void print_objects(const ObjectArray *arr, FILE *stream);
void save_objects_to_file(const ObjectArray *arr, const char *filename);
int load_objects_from_file(ObjectArray *arr, const char *filename);

#endif // OBJECT_MODULE_H
