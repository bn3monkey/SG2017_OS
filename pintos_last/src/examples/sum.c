#include<stdio.h>
#include<stdlib.h>
#include<syscall.h>
int main(int argc, char** argv)
{
    int int_arg[5];
    int fib, sum;
    int i;
    /*
    for(i=0;i<argc;i++)
    {
        printf("arg[%d] : %s\n",i,argv[i]);
    }
    */
    if(argc!=5)
    {
        write(1, "Argument Error!\n", 16);
        return 0;
    }
    for(i=1;i<=4;i++)
        int_arg[i] = atoi(argv[i]);
    fib = fibonacci(int_arg[1]);
    sum = sum_of_four_integers(int_arg[1], int_arg[2], int_arg[3], int_arg[4]);
    printf("%d %d\n", fib, sum);
    return EXIT_SUCCESS;
}
