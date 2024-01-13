#include "../header/DanhSachLop.h"
#include <iostream>
#include <fstream>
#include <string.h>
#include <sstream>
#include <cstdio>

Lop::Lop(){
    malop = tenlop = nienkhoa = "N/A";
    dssv = nullptr;
}

void Lop::setMaLop(string malop){
    updatePath(dssv->getPath(), "../data/DSSV/" + malop + "-" + this->nienkhoa + ".txt");

    this->malop = malop;
}

string Lop::getMaLop(){
    return malop;
}

void Lop::setTenLop(string tenlop){
    this->tenlop = tenlop;
}

string Lop::getTenLop(){
    return tenlop;
}

void Lop::setNienKhoa(string nienkhoa){
    updatePath(dssv->getPath(), "../data/DSSV/" + this->malop + "-" + nienkhoa + ".txt");

    this->nienkhoa = nienkhoa;
}

string Lop::getNienKhoa(){
    return nienkhoa;
}

Lop::Lop(string ma_lop, string ten_lop, string nien_khoa){
    malop = ma_lop;
    tenlop = ten_lop;
    nienkhoa = nien_khoa;

    string file_name = "../data/DSSV/" + malop + "-" + nien_khoa + ".txt";
    dssv = new DanhSachSinhVien(file_name);
}

bool Lop::isEmpty(){
    return dssv->isEmpty();
}

void Lop::updatePath(string old_path, string new_path){
    std::ifstream input(old_path);
    std::ofstream output(new_path);
    std::string line;

    while(getline(input, line)){
        output<<line<<std::endl;
    }

    input.close();
    output.close();
    remove(old_path.c_str());

    dssv->setPath(new_path);
}

SinhVien &Lop::operator[](string _mssv){
    return (*dssv)[_mssv];
}

void Lop::insert(SinhVien sv){
    dssv->insertLast(sv);
}

DanhSachSinhVien *Lop::getDSSV()
{
    return dssv;
}

Lop::~Lop(){
    delete dssv;
}

DanhSachLopHoc::DanhSachLopHoc(){
    soluong = 0;
    for(int i = 0; i<MAX_DSL; i++){
        List[i] = nullptr;
    }
}
 DanhSachLopHoc::~DanhSachLopHoc(){
    update();
    for(int i=0;i<soluong;i++){
        delete List[i];
    }
 }

int DanhSachLopHoc::getSoLuong(){
    return soluong;
}

void DanhSachLopHoc::removeClass(string malopcanxoa){
    for (int i = 0; i < soluong; i++){
        if (malopcanxoa.compare(List[i]->getMaLop()) == 0){
        
                delete List[i];
                string path = "../data/DSSV/" + List[i]->getMaLop() + "-" + List[i]->getNienKhoa() + ".txt";
                remove(path.c_str());
                for (int j = i; j < soluong; j++)
                {
                    List[j] = List[j + 1];
                }
                soluong--;
                break;
            
        }
    }
}

void DanhSachLopHoc::inLopTheoNienKhoa(string nienkhoa){
    for(int i=0;i<soluong;i++){
        if(nienkhoa.compare(List[i]->getNienKhoa()) == 0){
            cout<<List[i]->getMaLop()<<" "<<List[i]->getTenLop()<<" "<<List[i]->getNienKhoa()<<endl;
        }
    }
}

// void DanhSachLopHoc::hieuChinh(){
//     int luachon1, luachon2;
//     string _tenlop, _malop, _nienkhoa;
//     string a, lop;
//     do
//     {
//         cout << "Nhap lua chon cua ban: " << endl;
//         cout << "1-Them lop" << endl;
//         cout << "2-Xoa Lop" << endl;
//         cout << "3-Hieu chinh" << endl;
//         cout << "4-Thoat" << endl;
//         cin >> luachon1;
//         cin.ignore();
//         switch (luachon1)
//         {
//         case 1:
//             cout << "Nhap ma lop: ";
//             getline(cin, _malop);
//             cout << "Nhap ten lop: ";
//             getline(cin, _tenlop);
//             cout << "Nhap nien khoa: ";
//             getline(cin, _nienkhoa);
//             insert(new Lop{_malop, _tenlop, _nienkhoa});
//             break;
//         case 2:
//             cout << "Nhap ma lop can xoa: ";
//             getline(cin, a);
//             removeClass(a);
//             break;
//         case 3:
//             cout << "Nhap lop can hieu chinh: ";
//             getline(cin, lop);
//             do
//             {
//                 cout << "1-Ma Lop" << endl;
//                 cout << "2-Ten Lop" << endl;
//                 cout << "3-Nien Khoa" << endl;
//                 cout << "4-Thoat" << endl;
//                 cin >> luachon2;
//                 cin.ignore();
//                 switch (luachon2)
//                 {
//                 case 1:
//                     cout << "Nhap ma lop: ";
//                     getline(cin, a);
//                     (*this)[lop].setMaLop(a);
//                     cout << endl;
//                     break;
//                 case 2:
//                     cout << "Nhap ten lop: ";
//                     getline(cin, a);
//                     (*this)[lop].setTenLop(a);
//                     cout << endl;
//                     break;
//                 case 3:
//                     cout << "Nhap nien khoa: ";
//                     cin >> a;
//                     (*this)[lop].setNienKhoa(a);
//                     cout << endl;
//                     break;
//                 default:
//                     cout << "INVALID NUMBER" << endl;
//                 }
//             } while (luachon2 != 4);
//             break;
//         default:
//             cout << "INVALID NUMBER" << endl;
//         }
//     } while (luachon1 != 4);
// }

DanhSachLopHoc::DanhSachLopHoc(std::string path):DanhSachLopHoc(){
    ifstream input(path);
    if(!input.is_open()){
        string error = "Không thể mở file!: "+ path;
        throw error;
    }
    string line;
    while(getline(input, line)){
        stringstream _line(line);
        string MALOP, TENLOP, NIENKHOA;
        getline(_line,MALOP, '|');
        getline(_line,TENLOP, '|');
        getline(_line,NIENKHOA, '|');
        // int NIEN_KHOA = stoi(NIENKHOA);
        insert(new Lop{MALOP,TENLOP,NIENKHOA});
    }
}

void DanhSachLopHoc::insert(Lop *lop_hoc){ 
    if(soluong == MAX_DSL){
        throw "Danh sách lớp đã đầy";
        return;
    }

    if(searchClass(lop_hoc->getMaLop() ) != -1){
        std::string error = "Mã lớp đã tồn tại: " + std::string(lop_hoc->getMaLop());
        throw error;
        return;
    }

    List[soluong++] = lop_hoc;
}

int DanhSachLopHoc::searchClass(string malop){
    for(int i = 0; i<soluong; i++){
        if(strcmp(List[i]->getMaLop().c_str(), malop.c_str()) == 0){
            return i;
        }
    }
    return -1;
}
void DanhSachLopHoc::update(){
    std::ofstream output("../data/DANHSACHLOP.txt");
    for(int i=0;i<soluong;i++){
        output<<List[i]->getMaLop()<<"|"<<List[i]->getTenLop()<<"|"<<List[i]->getNienKhoa()<<endl;
    }
    output.close();
}

Lop *DanhSachLopHoc::operator[](int index){
    return List[index];
}

Lop *DanhSachLopHoc::operator[](string malop){
    int index = searchClass(malop);
    if(index == -1){
        throw "Không tìm thấy lớp";
    }
    return List[index];
}

DArray<SinhVien> DanhSachLopHoc::getAllSV()
{
    DArray<SinhVien> dssv;
    for (int i = 0; i < soluong; i++)
    {
        SVPtr ptr = List[i]->getDSSV()->getFirst();
        while (ptr != NULL)
        {
            dssv.push_back(ptr->sv_data);
            ptr = ptr->next;
        }
    }
    return dssv;
}