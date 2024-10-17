#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h" // Cung cấp các định nghĩa liên quan đến hệ thống tập tin (như cấu trúc thư mục).
#include "kernel/fcntl.h" // fcntl.h: Cung cấp các hằng số và hàm liên quan đến mở tệp.

// Hàm này nhận vào một chuỗi path (đường dẫn tệp) và trả về tên của tệp nằm ở cuối đường dẫn.
char* fmtname(char *path) {
    char *p;

    // Tìm ký tự đầu tiên sau dấu '/'
    for(p = path + strlen(path); p >= path && *p != '/'; p--) // Di chuyển ngược lại đến khi gặp dấu '/'
        ;
    p++; // Tăng con trỏ lên để trả về tệp cuối cùng (sau dấu '/')

    return p; // Trả về tên tệp cuối cùng trong đường dẫn.
}

// Hàm này thực hiện việc tìm kiếm đệ quy qua các thư mục con (đường dẫn thư mục hiện tại và tên bạn muốn tìm)
void find(char *path, char *targetname) {
    char buf[512], *p; // buf là bộ đệm để chứa đường dẫn, p là con trỏ để thao tác với chuỗi đường dẫn.
    int fd; // fd (file descriptor) là biến lưu trữ kết quả mở tệp hoặc thư mục.
    struct dirent de; // Cấu trúc lưu trữ thông tin về các tệp hoặc thư mục con trong thư mục hiện tại.
    struct stat st; // Cấu trúc lưu trữ thông tin về tệp hoặc thư mục (loại tệp, kích thước, quyền truy cập, v.v.).

    // Nếu tên của tệp hiện tại trùng với tên tệp mục tiêu, in ra đường dẫn đầy đủ.
    if (!strcmp(fmtname(path), targetname)) {
        printf("%s\n", path); // In ra đường dẫn đầy đủ của tệp đã tìm thấy.
    }

    // Mở thư mục với quyền chỉ đọc (O_RDONLY).
    if ((fd = open(path, O_RDONLY)) < 0) { // Nếu không thể mở được, in thông báo lỗi và kết thúc hàm.
        fprintf(2, "find: không thể mở [%s], fd=%d\n", path, fd);
        return;
    }

    // Sử dụng fstat để lấy thông tin của tệp tại path.
    if (fstat(fd, &st) < 0) { // Nếu không thể lấy thông tin, in thông báo lỗi và đóng thư mục.
        fprintf(2, "find: không thể lấy thông tin %s\n", path);
        close(fd);
        return;
    }

    // Nếu loại tệp không phải là thư mục, không tiếp tục tìm kiếm và đóng thư mục.
    if (st.type != T_DIR) {
        close(fd);
        return;
    }

    // Nếu đường dẫn quá dài để chứa trong bộ đệm, in thông báo lỗi và đóng thư mục.
    if (strlen(path) + 1 + DIRSIZ + 1 > sizeof buf) {
        printf("find: đường dẫn quá dài\n");
        close(fd);
        return;
    }

    // Sao chép đường dẫn hiện tại vào buf, thêm dấu '/' để chuẩn bị cho đường dẫn mới.
    strcpy(buf, path);
    p = buf + strlen(buf);
    *p++ = '/';

    // Đọc từng mục trong thư mục và đệ quy tìm kiếm.
    while (read(fd, &de, sizeof(de)) == sizeof(de)) {
        if (de.inum == 0) // Nếu mục trong thư mục không hợp lệ, bỏ qua.
            continue;
        memmove(p, de.name, DIRSIZ); // Sao chép tên tệp/thư mục vào p (đường dẫn đầy đủ).
        p[DIRSIZ] = 0; // Đảm bảo chuỗi kết thúc đúng.

        // Bỏ qua mục '.' và '..' để tránh đệ quy vô hạn (vì chúng trỏ đến chính thư mục hiện tại và thư mục cha).
        if (!strcmp(de.name, ".") || !strcmp(de.name, ".."))
            continue;

        // Gọi lại hàm find với đường dẫn mới (buf) để tiếp tục tìm kiếm trong thư mục con.
        find(buf, targetname);
    }
    close(fd); // Đóng thư mục sau khi đã duyệt qua hết các mục.
}

// Hàm main nhận vào các tham số từ dòng lệnh và gọi hàm find để tìm tệp.
int main(int argc, char *argv[]) {
    // Kiểm tra xem người dùng có nhập đúng số lượng tham số (cần ít nhất 3: chương trình, đường dẫn và tên tệp).
    if (argc < 3) {
        fprintf(2, "usage: find path filename\n");
        exit(1);
    }

    // Gọi hàm find với đường dẫn và tên tệp mà người dùng cung cấp.
    find(argv[1], argv[2]);

    // Thoát chương trình khi đã hoàn thành.
    exit(0);
}
