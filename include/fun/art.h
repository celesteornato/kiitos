#ifndef ART_H_
#define ART_H_

#include <stdint.h>
[[gnu::used]]
constexpr char foomp_art[] = "  /\\______/\\\n"
                             " /..O....O..\\\n"
                             " \\....ww..../\n"
                             "  )........(\n"
                             " (.( )..( ).)\n"
                             "(__(_)__(_)__)";

constexpr char image[] = {
  #embed "../../src/assets/cat.kiit_img_fmt"
};
#endif // ART_H_
