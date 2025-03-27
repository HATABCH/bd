#include "object_module.h"
#include <stdio.h>

int main() {
  ObjectArray *arr = create_object_array(5);

  add_object(arr, "Ibragim", 30, 65);
  add_object(arr, "Ramazan", 20, 60.2);
  add_object(arr, "Dmitriy", 35, 81.1);

  printf("Initial objects:\n");
  print_objects(arr, stdout);

  printf("\nSorting by age ascending:\n");
  sort_objects(arr, 1, 1);
  print_objects(arr, stdout);

  save_objects_to_file(arr, "saved_file.txt");

  ObjectArray *arr2 = create_object_array(5);
  load_objects_from_file(arr2, "objects.txt");
  printf("\nLoaded from file:\n");
  print_objects(arr2, stdout);

  printf("\nBar chart (Age):\n");
  print_bar_chart(arr, 1);

  print_pie_chart(arr, 2);

  free_object_array(arr);
  free_object_array(arr2);
  return 0;
}
