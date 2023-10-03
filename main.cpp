
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <linux/input.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <unordered_map>

const std::string kDir = "/dev/input/by-id/";
const std::string kPrefix = "usb-Logitech_Gaming_Mouse_G600_";
const std::string kSuffix = "-if01-event-kbd";

const std::string mousePrefix = "-event-mouse";

struct input_event events[64];

enum error_codes {
  kSuccess = 0,
  kError = 1,
  kFileNotFound = 2,

};

// mouse buttons
std::string get_input_g600_path() {
  printf("checking for G600 \n");
  if (!std::filesystem::exists(kDir.c_str())) {
    printf("directory does not exist \n");
    return "";
  }

  for (auto& file :
       std::filesystem::directory_iterator{
           kDir})  // loop through the current folder
  {
    std::string file_name = file.path().string();

    bool isKPrefix = file_name.find(kPrefix) != std::string::npos;
    bool isKSuffix = file_name.find(kSuffix) != std::string::npos;

    if ((isKPrefix && isKSuffix)) {
      printf("directory exists \n");
      return file.path().c_str();
    }
  };

  return "";
}

int main() {
  std::map<std::string, std::string> downCommands;
  //[scancode] = "command to run",
  downCommands["30"] = "echo 'todo'";  // G9
  downCommands["31"] = "echo 'todo'";  // G10
  downCommands["32"] = "echo 'todo'";  // G11
  downCommands["33"] = "echo 'todo'";  // G12
  downCommands["34"] = "echo 'todo'";  // G13
  downCommands["35"] = "echo 'todo'";  // G14
  downCommands["36"] = "echo 'todo'";  // G15
  downCommands["37"] = "echo 'todo'";  // G16
  downCommands["38"] = "echo 'todo'";  // G17
  downCommands["39"] = "echo 'todo'";  // G18
  downCommands["45"] = "echo 'todo'";  // G19
  downCommands["46"] = "echo 'todo'";  // G20

  std::map<std::string, std::string> upCommands;

  printf("starting G600 linux contoller \n");

  std::string path = get_input_g600_path();
  printf("dir: %s \n", path.c_str());

  std::ifstream inputFile;

  int fd = open(path.c_str(), O_RDONLY);

  if (fd < 0) {
    printf("could not open file, trying using sudo \n");
    return error_codes::kError;
  } else {
    printf("opened file \n");
  }
  // grab the linux input event
  ioctl(fd, EVIOCGRAB, 1);
  printf("grabbed input \n");

  while (1) {
    size_t n = read(fd, events, sizeof(events));
    if (n <= 0) return 2;
    if (n < sizeof(struct input_event) * 2) continue;
    if (events[0].type != 4) continue;
    if (events[0].code != 4) continue;
    if (events[1].type != 1) continue;
    int pressed = events[1].value;
    int scancode = events[0].value & ~0x70000;

    std::string actionStr = (pressed) ? "Pressed" : "Released";

    std::string downCmd = downCommands[std::to_string(scancode)];
    std::string upCmd = upCommands[std::to_string(scancode)];

    std::string cmdToRun = (pressed) ? downCmd : upCmd;

    printf("cmd: %s \n", cmdToRun.c_str());
    system(cmdToRun.c_str());
  }
  close(fd);
  return error_codes::kSuccess;
}
