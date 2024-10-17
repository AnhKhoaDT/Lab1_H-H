//sử dụng cơ chế pipes và fork để tạo ra các process con liên tiếp lọc các số không phải là số nguyên tố.
#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#define MAX 281
#define FIRST_PRIME 2//2->35    

int generate_natural();  // tạo ra một dãy số tự nhiên
int prime_filter(int in_fd, int prime);  // ->lọc các số chia hết cho số đó

int
main(int argc, char *argv[])// tạo ra 1 pipeline để thể hiện sàng nguyên tố 
{
	int prime; // chứa giá trị số nguyên tố được đọc được từ pipeline
	
	int in = generate_natural();
	while (read(in, &prime, sizeof(int))) {// đọc giá trị từ pipe , số này xacs định là số nguyên tố 
		// printf("prime %d: in_fd: %d\n", prime, in);  // debug
		printf("prime %d\n", prime); //in
		in = prime_filter(in, prime);// tạo một filter mới cho số nguyên tố tiếp theo. Hàm này sẽ trả về file descriptor của một pipe mới, 
        //từ đó chương trình sẽ tiếp tục đọc và lọc các số không phải bội của prime.
	}

	exit(0);
}


int
generate_natural() {
	int out_pipe[2];// mảng pipe
	
	pipe(out_pipe);// tạo pipe 2 đầu ,một đầu để ghi (out_pipe[1]) và một đầu để đọc (out_pipe[0]).

	if (!fork()) {// nếu con được tạo
		for (int i = FIRST_PRIME; i < MAX; i++) {//ghi tưngf số từ 2-35vào out_pipe[1]
			write(out_pipe[1], &i, sizeof(int));
		}
		close(out_pipe[1]);//Việc đóng đầu ghi của pipe là rất quan trọng vì khi đóng,
    // hệ điều hành sẽ báo cho tiến trình đọc rằng không còn dữ liệu để đọc nữa 
    //(bằng cách trả về 0 khi tiến trình đọc từ đầu đọc của pipe). 

		exit(0);
	}

	close(out_pipe[1]);//Tiến trình cha không cần ghi vào pipe nữa 
    //(chỉ tiến trình con mới ghi dãy số vào), nên tiến trình cha đóng đầu ghi của pipe ngay lập tức, để giải phóng tài nguyên.

	return out_pipe[0];// trả về đầu đọc của pipe 
    //Điều này có nghĩa là tiến trình cha (hoặc các tiến trình tiếp theo) 
    //sẽ đọc dữ liệu từ đầu đọc của pipe (out_pipe[0]) để nhận được dãy số do tiến trình con ghi vào pipe.
}


int             //File descriptor của pipe mà qua đó các số đang được truyền đến.
prime_filter(int in_fd, int prime) // lọc ra các số ko phải là bội của số nguyên tố hiện tại 
{                       // số nguyên tố hiện tại
	int num;
	int out_pipe[2];

	pipe(out_pipe);

	if (!fork()) {// sẽ thực hiện công việc lọc
		while (read(in_fd, &num, sizeof(int))) {
			if (num % prime) {//!0
				write(out_pipe[1], &num, sizeof(int));// ghi nó vào cais đó 
			}
		}
		close(in_fd);// đóng ds
		close(out_pipe[1]);// đóng ghi
		
		exit(0); 
	}

	close(in_fd);//Đóng pipe in_fd (đầu đọc pipe từ quá trình trước) vì không còn cần thiết nữa.
	close(out_pipe[1]);//Đóng đầu ghi của pipe mới (out_pipe[1]) vì quá trình cha không cần ghi thêm số vào pipe này.

	return out_pipe[0];//Trả về file descriptor của đầu đọc của ống dẫn mới để tiếp tục quá trình sàn lọc.
}
