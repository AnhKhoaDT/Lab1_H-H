#include "kernel/types.h"// chứa khai báo cho các hàm và các kiểu dữ liệu sử dụng trong chương trình    
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
  int ticks;  // time to sleep

  if(argc <= 1){
    fprintf(2, "usage: sleep ticks\n");
    exit(1);
  }
  ticks = atoi(argv[1]);// chuyển đổi số dòng lệnh thành số nguyên

  sleep(ticks);// cho số giây ngủ trong chương trình 

  exit(0);
}