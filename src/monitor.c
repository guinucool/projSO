#include <unistd.h>
#include <sys/wait.h>
#include "../includes/tools/message.h"
#include "../includes/utils.h"

int main()
{
    unlink("/tmp/serverListen");

    messageListen();
}