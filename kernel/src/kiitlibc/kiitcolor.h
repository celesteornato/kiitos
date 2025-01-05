union color {
  struct {
    unsigned char b;
    unsigned char g;
    unsigned char r;
    unsigned char a;
  } rgb;
  unsigned int value;
};

#define RED 0xffff0000 
#define GREEN 0xff00ff00 
#define BLUE 0xff0000ff 
