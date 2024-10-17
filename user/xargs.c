#include "kernel/types.h"   // Thư viện chứa các kiểu dữ liệu cơ bản.
#include "kernel/stat.h"    // Thư viện chứa các định nghĩa về trạng thái tệp.
#include "user/user.h"      // Thư viện chứa các hàm và định nghĩa liên quan đến người dùng.
#include "kernel/param.h"   // Thư viện chứa các hằng số như MAXARG.

#define is_blank(chr) (chr == ' ' || chr == '\t') // Macro kiểm tra xem ký tự có phải là khoảng trắng hay không.

int main(int argc, char *argv[]) {  // Hàm main, điểm bắt đầu của chương trình.
	char buf[2048], ch;              // Khai báo bộ đệm (buf) cho dữ liệu đầu vào và biến ký tự (ch) để đọc từng ký tự.
	char *p = buf;                   // Con trỏ (p) để theo dõi vị trí trong bộ đệm.
	char *v[MAXARG];                 // Mảng (v) để chứa các đối số cho lệnh (tối đa MAXARG).
	int c;                           // Biến đếm (c) cho số lượng đối số.
	int blanks = 0;                  // Biến đếm số khoảng trắng liên tiếp.
	int offset = 0;                  // Biến để theo dõi vị trí hiện tại trong bộ đệm (offset).

	if(argc <= 1){                    // Kiểm tra số lượng đối số.
		fprintf(2, "usage: xargs <command> [argv...]\n"); // Thông báo cách sử dụng nếu không đủ đối số.
		exit(1);                      // Kết thúc chương trình với mã lỗi.
	}

	for (c = 1; c < argc; c++) {     // Vòng lặp để sao chép các đối số lệnh từ argv vào mảng v.
		v[c-1] = argv[c];            // Gán các đối số lệnh vào mảng v (bắt đầu từ v[0]).
	}
	--c;                             // Giảm c để tính toán đúng số lượng đối số (với c-1).

	while (read(0, &ch, 1) > 0) {    // Đọc từng ký tự từ đầu vào chuẩn (stdin) cho đến khi không còn dữ liệu.
		if (is_blank(ch)) {          // Kiểm tra xem ký tự có phải là khoảng trắng hay không.
			blanks++;                 // Tăng biến đếm khoảng trắng.
			continue;                // Tiếp tục vòng lặp nếu là khoảng trắng.
		}

		if (blanks) {                 // Nếu đã gặp khoảng trắng trước đó:
			buf[offset++] = 0;       // Đánh dấu kết thúc chuỗi hiện tại trong buf bằng ký tự null (0).

			v[c++] = p;               // Gán địa chỉ chuỗi vừa tạo vào mảng v.
			p = buf + offset;        // Cập nhật con trỏ p tới vị trí mới trong buf.

			blanks = 0;              // Đặt lại biến đếm khoảng trắng.
		}

		if (ch != '\n') {             // Nếu ký tự không phải là ký tự newline:
			buf[offset++] = ch;      // Thêm ký tự vào buf và tăng offset.
		} else {                      // Nếu gặp ký tự newline:
			v[c++] = p;               // Gán chuỗi hiện tại vào mảng v.
			p = buf + offset;        // Cập nhật con trỏ p.

			if (!fork()) {            // Tạo tiến trình con:
				exit(exec(v[0], v)); // Thực thi lệnh với đối số trong v và kết thúc tiến trình con.
			}
			wait(0);                  // Chờ tiến trình con hoàn thành.

			c = argc - 1;            // Đặt lại c về số lượng đối số ban đầu (trừ đi lệnh).
		}
	}

	exit(0);                        // Kết thúc chương trình chính với mã 0 (thành công).
}
