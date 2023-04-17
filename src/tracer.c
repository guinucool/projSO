#include <unistd.h>
#include <sys/wait.h>
#include "../includes/tools/message.h"
#include "../includes/utils.h"

int main()
{
    Message msg = createMessage(getpid(), TYPE_PROCESS, "cat", getTimeMilliseconds());

    messageSend(msg);
}