# Find Duplicate File Program
## Mô tả

Find Duplicate File Program là một chương trình C++ dùng để tìm các file trùng lặp trên hệ thống, bất kể chúng có tên khác nhau hay lưu trữ ở các thư mục khác nhau. Chương trình có thể tùy chọn kiểm tra toàn bộ máy tính hoặc chỉ định thư mục, ổ đĩa cần kiểm tra.

## Yêu cầu phần mềm

- C++17
- Visual Studio Comunity 2022
- Chạy chương trình với quyền Admin

## Nguyên lý hoạt động
Chương trình hoạt động theo các bước sau:

1. **Xác định nơi quét và tính số lượng:**

   - Nếu chọn quét toàn bộ máy tính, chương trình sẽ lấy danh sách các ổ đĩa logic và chia thành các thread để quét đồng thời.
   - Nếu chỉ định thư mục hoặc ổ đĩa, chương trình sẽ thực hiện quét trên địa chỉ đó.
   - Đếm tổng số file có thể truy cập đọc trong thư mục hoặc ổ đĩa được chọn.
2. **Duyệt qua các file:**

   - Duyệt qua tất cả các file trong thư mục cha và tính toán hash MD5 của file.
   - Lưu trữ giá trị hash và đường dẫn file trong một bảng băm.
5. **Tìm các file trùng lặp:**

   - Duyệt qua bảng băm để tìm các hash trùng lặp, các file có nội dung giống nhau sẽ cho ra cùng 1 giá trị hash và đường dẫn rới file sẽ được lưu ở cùng 1 vector
   - Lưu danh sách các file trùng lặp vào 1 vector để thực hiện các bước xử lý.
6. **Hiển thị và xử lý file trùng lặp:**

   - Hiển thị các file trùng lặp và cho phép người dùng chọn các file để xóa hoặc lưu trữ danh sách tệp trùng lặp vào file.


## Hướng dẫn cài đặt và chạy

1. **Cài đặt các công cụ cần thiết:**
   - Khởi chạy Visual Studio với quyền Admin
   - Đảm bảo đã cài đặt thông tin C++ Language Standard của dự án là IOS C++ 17 Standard

2. **Biên dịch chương trình:**
   - Sử dụng tính năng Start Without Debug của Visual Studio hoặc nhấn Ctr + F5
   - Thực hiện tương tác với chương trình thông qua màn hình console
     
<div style="display: flex; justify-content: space-between;">
    <img src="https://github.com/anhzo12/finddup/assets/93477140/f9761d77-a869-43c7-947b-77985307f93d" alt="Image 1" style="width: 47%;">
    <img src="https://github.com/anhzo12/finddup/assets/93477140/b996f5ea-c75b-40ad-b710-77f04a28f099" alt="Image 2" style="width: 47%;">
</div>

<div style="display: flex; justify-content: space-between;">
    <img src="https://github.com/anhzo12/finddup/assets/93477140/8c767dec-be27-4d37-9b9a-9eb502196d7a" alt="Image 1" style="width: 48%;">
    <img src="https://github.com/anhzo12/finddup/assets/93477140/327ae5a6-9fdd-4b24-b4cd-cae1c7aae5fe" alt="Image 2" style="width: 48%;">
</div>
