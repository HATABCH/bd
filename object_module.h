#ifndef OBJECT_MODULE_H
#define OBJECT_MODULE_H
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

ObjectArray *create_object_array(int initial_capacity);
void free_object_array(ObjectArray *arr);
void add_object(ObjectArray *arr, const char *name, int age, double weight);
void remove_object(ObjectArray *arr, int index);
void sort_objects(ObjectArray *arr, int field, int ascending);
void print_objects(const ObjectArray *arr, FILE *stream);
void save_objects_to_file(const ObjectArray *arr, const char *filename);
void load_objects_from_file(ObjectArray *arr, const char *filename);
void print_bar_chart(const ObjectArray *arr, int field);
void print_pie_chart(const ObjectArray *arr, int field);

#endif // OBJECT_MODULE_H
