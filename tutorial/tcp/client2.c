#include "tcplib.c"

int main()
{
    http_get("127.0.0.1", 5555, "/?ggg=12345");
    return 0;
}