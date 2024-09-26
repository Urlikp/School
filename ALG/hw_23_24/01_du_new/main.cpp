#include "includes.h"
/* Resources:
  Prefix sum: https://www.youtube.com/watch?v=7pJo_rM0z_s
*/

// QUEUE is a lot faster than SET

// Rock is 2
// Tree is 1

int main()
{
  // Using printf vs cout https://stackoverflow.com/questions/2872543/printf-vs-cout-in-c
  // Using scanf vs cin https://stackoverflow.com/questions/1042110/using-scanf-in-c-programs-is-faster-than-using-cin

  input_t info = read_input();

  // print_input(info);

  prefixes_t pref{};

  // printf("Prefix sum for rocks:\n");
  // Calculating prefix sum for rocks
  calc_prefix_sum(info, &pref);
  print_2d_array(pref.rocks, info.n);

  // printf("Prefix sum for trees:\n");
  //  Calculating prefix sum for rocks
  //  pref.trees = calc_prefix_sum(info, 2);
  //  print_2d_array(pref.trees, info.n);

  //   printf("Sum of rocks in rectangle: %d\n", calc_sum_in_pref(pref.rocks, info.n, 1, 2, 1+info.c,2+info.c));

  int suitable = suitable_locs(info, pref); // ! Important
  printf("%d", suitable);
  return suitable;
}
