#include "object_module.h"
#include <stdio.h>

int main() {
  ObjectArray *arr = create_object_array(5);

  add_object(arr, "Alice", 30, 65.5);
  add_object(arr, "Bob", 25, 70.3);
  add_object(arr, "Charlie", 35, 80.1);

  printf("Initial objects:\n");
  print_objects(arr, stdout);

  printf("\nSorting by age ascending:\n");
  sort_objects(arr, 1, 1);
  print_objects(arr, stdout);

  save_objects_to_file(arr, "objects.txt");

  ObjectArray *arr2 = create_object_array(5);
  load_objects_from_file(arr2, "objects.txt");
  printf("\nLoaded from file:\n");
  print_objects(arr2, stdout);

  printf("\nBar chart (Age):\n");
  print_bar_chart(arr, 1);

  printf("\nPie chart (Weight):\n");
  print_pie_chart(arr, 2);

  free_object_array(arr);
  free_object_array(arr2);
  return 0;
}
