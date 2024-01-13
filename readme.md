# Đồ án CNTT khóa 2021
Thi trắc nghiệm: Ta tổ chức các danh sách sau: 
- Danh sách môn học: danh sách  tuyến tính có tối đa 300 môn (MAMH (C15), TENMH), và luôn có sẵn thứ tự theo mã môn học
- Danh sách Lop : mảng con trỏ  có tối đa 500 lớp(MALOP, TENLOP, niên khóa ,  con trỏ dssv): con trỏ dssv  trỏ đến danh sách sinh viên thuộc lớp đó.
- Danh sách sinh viên : danh sách liên kết đơn (MASV, HO, TEN,  PHAI, password, con trỏ ds_điểm): con trỏ ds_điểm sẽ quản lý điểm các môn đã thi trắc nghiệm.
- Danh sách Điểm thi (danh sách liên kết đơn) (Mamh, Diem)
- Danh sách Câu hỏi thi : chứa các câu hỏi nguồn của các môn học (Id, Mã MH, Nội dung, A, B, C, D, Đáp án); trong đó A, B, C, D là 4 chọn lựa tương ứng với nội dung câu hỏi. Danh sách câu hỏi thi là  cây nhị phân tìm kiếm.

Chương trình có các chức năng sau : 

- a/ Đăng nhập dựa vào mã sinh viên, password. Nếu tài khoản đăng nhập là GV, pass là GV thì sẽ có toàn quyền.
- b/ Nhập Lớp : thêm / xóa / hiệu chỉnh thông tin các lớp
- c/ In danh sách lớp theo 1 niên khóa.
- d/ Nhập sinh viên của lớp : nhập vào mã lớp trước, sau đó nhập các sinh viên vào lớp đó. 
- e/ Nhập môn học: cho phép cập nhật (thêm / xóa / hiệu chỉnh ) thông tin của môn học.
- f/ Nhập câu hỏi thi (Id là số nguyên dương ngẫu nhiên do chương trình tự tạo)
- g/ Thi Trắc nghiệm trước khi thi hỏi người thi môn thi, số câu hỏi thi, số phút thi-sau đó lấy ngẫu nhiên các câu hỏi trong danh sách câu hỏi thi của môn.
- h/ In chi tiết các câu hỏi đã thi 1 môn học của  1 sinh viên. 
- i/ In bảng điểm thi trắc nghiệm môn học của 1 lớp nếu có sinh viên chưa thi thì ghi “Chưa thi”.

Lưu ý: Chương trình cho phép lưu các danh sách vào file; Kiểm tra các điều kiện làm dữ liệu bị sai. Sinh viên có thể tự thiết kế thêm danh sách để đáp ứng yêu cầu của đề tài.

# Cách chạy code
```
cd build
make
make run

or

cd build
make
.\output
```
Nếu có pwsh từ microsoft store thì
```
make && make run

or

make && .\output
```

# Cấu trúc các thư mục
- build: chứa các file object và file exe
- data: chứa cái dữ liệu input(file .csv)
- header: chứa các file header(file .h)
- source: chứa main và file .cpp