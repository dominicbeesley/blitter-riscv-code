

volatile int x = 4;

volatile int y = 999;

char bigfuckoffarray[100000];
char bigfuckoffarray2[100000];


int doit(int x);

void main(void) {

	for (int i = 0; i < y; i++) {
		x += doit(i);		
		bigfuckoffarray2[i] = x & 255;
	}

}


int doit(int xx) {
	return xx*x;
}