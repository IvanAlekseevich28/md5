#include <iostream>
#include "md5.hpp"

using namespace std;

int main(int argc, char** argv)
{
    if (argc != 2)
    {
        return -1;
    }

    cout << MD5().stringHash(argv[1])<< endl;

    return 0;
}
