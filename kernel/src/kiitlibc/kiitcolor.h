union color {
  struct {
    unsigned char a;
    unsigned char b;
    unsigned char g;
    unsigned char r;
  } rgb;
  unsigned int value;
};

#define RED 0xffff0000 
#define GREEN 0xff00ff00 
#define BLUE 0xff0000ff 
