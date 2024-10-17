#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char *argv[]) {// Định nghĩa hàm chính với số lượng và giá trị đối số.

    if (argc != 1) { //Kiểm tra nếu số lượng đối số không phải là 1.
        printf("error!\n");
        exit(1);
    }

    char mesg[] = "a";
    int p1[2], p2[2];//hai ống dẫn để giao tiếp với cha và con, p1 từ cha sang con và p2 else
	//tạo 2 ống dẫn nếu thất baị in ra lỗi
   if (pipe(p1) == -1) {
		printf("error creating pipe from parent to child\n");
		exit(1);
   }
   if (pipe(p2) == -1) {
		printf("error creating pipe from child to parent\n");
		exit(1);
   }

	// tạo tién trinhf con 
    int pid = fork();
    if (pid < 0) {
        printf("fork error\n");
        exit(1);
    } else if (pid == 0) {  // child
	// đóng đầu ghi của p1 và đầu đọc của p2
        close(p1[1]);
        close(p2[0]);

        if (read(p1[0], mesg, 1) == 1) {// đọc 1 byte từ p1 và ghi nó vào p2
            printf("%d: received ping\n", getpid());
            write(p2[1], mesg, 1);
        } else {
            printf("receiving error!\n");
            exit(0);
        }

        exit(0);
    } else {  // parent
	// đóng dầu đocj của p1 và đầu ghi của p2
        close(p1[0]);
        close(p2[1]);

        write(p1[1], mesg, 1);// ghi 1 byte và chờ con kết thúc
        wait(0);

        if (read(p2[0], mesg, 1) == 1) {// đọc 1 byte từ p2
            printf("%d: received pong\n", getpid());
        } else {
            printf("receiving error\n");
            exit(1);
        }

        exit(0);
    }
}
