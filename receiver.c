#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#define LOCAL_SOCKET_PATH "/tmp/command_socket" // Change this to the same path as in sender.c

typedef struct
{
  uint8_t command[5];
} Command;

// Function handler array indexed by function code
FunctionCodeHandler functionHandlers[16] = {
    NULL,            // Function code 0 is not used
    adjustBacklight, // Function code 1
    queryBacklight,  // Function code 2
                     // Add other function handlers here
};

当你想要为每个命令添加处理程序时，可以根据命令的功能码（第3个字节）来调用相应的处理函数。以下是一个示例 receiver.c，演示了如何根据接收到的命令调用对应的处理函数：

    c
        Copy code
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#define LOCAL_SOCKET_PATH "/tmp/command_socket" // Change this to the same path as in sender.c

    typedef struct
{
  uint8_t command[5];
} Command;

// Define function code handlers
typedef void (*FunctionCodeHandler)(uint8_t data1, uint8_t data2);

void adjustBacklight(uint8_t data1, uint8_t data2)
{
  printf("Adjusting backlight with data1: %02X, data2: %02X\n", data1, data2);
  // Implementation to adjust backlight based on data1 and data2
}

void queryBacklight(uint8_t data1, uint8_t data2)
{
  printf("Querying backlight setting\n");
  // Implementation to respond with backlight setting
}

// Add more function handlers for other commands here

// Function handler array indexed by function code
FunctionCodeHandler functionHandlers[16] = {
    NULL,            // Function code 0 is not used
    adjustBacklight, // Function code 1
    queryBacklight,  // Function code 2
                     // Add other function handlers here
};

void handleCommand(Command *command)
{
  uint8_t functionCode = command->command[2];

  if (functionCode >= sizeof(functionHandlers) / sizeof(functionHandlers[0]))
  {
    printf("Invalid function code: %02X\n", functionCode);
    return;
  }

  FunctionCodeHandler handler = functionHandlers[functionCode];
  if (handler != NULL)
  {
    handler(command->command[3], command->command[4]);
  }
  else
  {
    printf("Unhandled function code: %02X\n", functionCode);
  }
}

void receiveCommands()
{
  int sockfd;
  struct sockaddr_un addr;

  sockfd = socket(AF_UNIX, SOCK_STREAM, 0);
  if (sockfd == -1)
  {
    perror("socket");
    return;
  }

  addr.sun_family = AF_UNIX;
  strcpy(addr.sun_path, LOCAL_SOCKET_PATH);

  if (bind(sockfd, (struct sockaddr *)&addr, sizeof(addr)) == -1)
  {
    perror("bind");
    close(sockfd);
    return;
  }

  if (listen(sockfd, 1) == -1)
  {
    perror("listen");
    close(sockfd);
    return;
  }

  printf("Listening for commands...\n");

  while (1)
  {
    int clientfd = accept(sockfd, NULL, NULL);
    if (clientfd == -1)
    {
      perror("accept");
      continue;
    }

    Command receivedCommand;
    ssize_t bytesRead = recv(clientfd, &receivedCommand, sizeof(Command), 0);
    if (bytesRead == -1)
    {
      perror("recv");
      close(clientfd);
      continue;
    }

    handleCommand(&receivedCommand);

    close(clientfd);
  }

  close(sockfd);
}

int main()
{
  receiveCommands();
  return 0;
}
