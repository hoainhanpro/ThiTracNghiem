#ifndef DANHSACHLOP_H
#define DANHSACHLOP_H
#include <string>
#include "DArray.h"
#include "DanhSachSinhVien.h"
#define MAX_DSL 500
using namespace std;

class Lop{
    private:
        string malop;
        string tenlop;
        string nienkhoa;
        DanhSachSinhVien *dssv;
    public:
        Lop();
        ~Lop();
        Lop(string ma_lop, string ten_lop, string nien_khoa );
        DanhSachSinhVien *getDSSV();
        void setMaLop(string malop);
        string getMaLop();
        void setTenLop(string tenlop);
        string getTenLop();
        void setNienKhoa(string nienkhoa);
        string getNienKhoa();
        void updatePath(string old_path, string new_path);
        bool isEmpty();

        void insert(SinhVien sv);
        SinhVien &operator[](string _mssv);
};

class DanhSachLopHoc{
    private:
        int soluong; 
        Lop *List[MAX_DSL];

    public:
        DanhSachLopHoc();
        ~DanhSachLopHoc();
        int getSoLuong();
        DanhSachLopHoc(string path);
        void removeClass(string malop);
        void inLopTheoNienKhoa(string nienkhoa);
        void hieuChinh();
        void insert(Lop *lop_hoc);
        int searchClass(string malop);
        void update();
        Lop *operator[](int index);
        Lop *operator[](string malop);
        DArray<SinhVien> getAllSV();
        
};

#endif