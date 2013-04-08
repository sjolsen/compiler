int foo(int a, int b, char c){
int d[5];
d[4] = 1;
d[2] = 4;
d[2*2] = 8;
d[2*2*2*2*2+4] = 10;
d[2+4*4]=9;
d[a]=10;
d[2]=7-8;
}

int charlie(int j){
    return j+j;
}

int main(){
int a;
int b;
int c;
int d;
a=2;
b=7;
c=foo(a,3+3,'c');
d=foo(foo(a,c,'b'),2,'3');
a=charlie(2);
}

