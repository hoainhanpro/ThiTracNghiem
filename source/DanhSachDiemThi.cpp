#include "../header/DanhSachDiemThi.h"
#include "../header/DanhSachSinhVien.h"
#include "../header/DanhSachLop.h"
using namespace std;


DanhSachDiemThi::DanhSachDiemThi(){
    // khoiTaoDTPtr(&First);
    First = nullptr;
    _path = "";
}

DanhSachDiemThi::DanhSachDiemThi(string path){
    First = nullptr;
    _path = path;
    ifstream in(_path);
    if (!in){
        ofstream out(_path);
        out.close();
        in.open(_path);
    }

    string line = "";
    while(getline(in, line)){
        stringstream ss(line);
        string _maMon, _diem;
        getline(ss, _maMon, '|');
        getline(ss, _diem, '|');
        insertOrderDT(DiemThi((char*)_maMon.c_str(), stof(_diem)));
    }
    in.close();
}

DanhSachDiemThi::DanhSachDiemThi(const DanhSachDiemThi &dsdt){
    First = nullptr;
    _path = dsdt._path;
    DTPtr p = dsdt.First;
    while(p != nullptr){
        insertOrderDT(p->data);
        p = p->next;
    }
}

DanhSachDiemThi& DanhSachDiemThi::operator=(const DanhSachDiemThi &dsdt){
    if(this == &dsdt){
        return *this;
    }

    this->~DanhSachDiemThi();
    First = nullptr;
    _path = dsdt._path;
    DTPtr p = dsdt.First;
    while(p != nullptr){
        insertOrderDT(p->data);
        p = p->next;
    }
    return *this;
}

string DanhSachDiemThi::getPath(){
    return _path;
}

DTPtr &DanhSachDiemThi::getFirst(){
    return First;
}   

DanhSachDiemThi::~DanhSachDiemThi(){
    update();

    DTPtr p = First;
    while(p != NULL){
        DTPtr q = p;
        p = p->next;
        delete q;
    }
}
    
/*
Các hàm insert để test chương trình
void DanhSachDiemThi::insertFirst(DiemThi dt){
    DTPtr p = new DiemThiNode;
    p->data = dt;
    p->next = First;
    First = p;
}

Hàm thêm điểm thi vào sau nút địa chỉ First
void DanhSachDiemThi::insertAfterDT (DiemThi dt, DTPtr p){
    if (p == NULL){
        cout << "Khong the them phan tu vao danh sach" << endl;
        return;
    } else {
        DTPtr q = new DiemThiNode;
        q->data = dt;
        q->next = p->next;
        p->next = q;
        // p = q; //Cập nhật con trỏ đến nút mới chèn vào
    }
}

void DanhSachDiemThi::insertDiem(DTPtr &First, DiemThi dt){
    if (First==NULL){
        insertFirst(dt);
    }
    else {
        DTPtr p;
        p = new DiemThiNode;
        p = First;
        while(p->next != NULL){
            p = p->next;
        }
        insertAfterDT(dt, p);
    }
}
// Hàm kiểm tra xem môn học đã thi chưa
bool DanhSachDiemThi::kiemTraDaThi(DTPtr First,char Mamh[15]){
    DTPtr p;
    p = new DiemThiNode;
    p = First;
    while(p != NULL){
        if(strcmp(p->data.Mamh, Mamh) == 0){
            return true;
        }
        p = p->next;
    }
    return false;
}
*/


void DanhSachDiemThi::insertOrderDT(DiemThi dt) {
    DTPtr s, t;
    DTPtr p = new DiemThiNode;
    p->data = dt;
    // Tìm vị trí chèn của nút mới trong danh sách liên kết
    for (s = First; s != NULL && strcmp(s->data.Mamh, p->data.Mamh) < 0; t = s, s = s->next);

    //Chèn đầu danh sách
    if (s == First) {
        p->next = First;
        First = p;
    }
    //Chèn giữa danh sách
    else {
        p->next = s;
        t->next = p;
    }
}



/*Hàm in điểm theo lớp để test
void DanhSachDiemThi::xuatDiemTheoLop(DanhSachSinhVien &list, char *Malop){
    ofstream out;

    char filename[50];
    char dinhdang[5];
    //Định dạng và tên file
    strcpy(dinhdang, ".txt");
    strcpy(filename, "../data/DIEM_");
    //Nối tên file với mã lớp dạng (VD: DIEM_D21CQCN02-N.txt) 
    strcat(filename, Malop);
    strcat(filename, dinhdang);
    out.open(filename, ios::app);
    if (!out) {
        cout << "Khong mo duoc file";
        return;
    }
    DanhSachSinhVien::SVPtr p = list.getFirst();
    while (p != NULL) {

        out << p->sv_data.MASV << "|" ;
        out << p->sv_data.HO << " " << p->sv_data.TEN << "|";
            DTPtr q = First;
        while (q != NULL){
            out << q->data.Mamh << "|";
            // if (DanhSachDiemThi::kiemTraDaThi(p->sv_data.ptr,q->data.Mamh) == false) {
            //     out << "Chua thi"; // Nếu chưa thi thì ghi "Chua thi"
            // } else {
                out << q->data.Diem; 
            // }
            if (q->next != NULL){
                out << "|";
            }
            q = q->next;
            }
        // }
        out << endl;
        p = p->next;
    }
    out.close();
}*/

void DanhSachDiemThi::update(){
    ofstream out(_path);
    DTPtr p = First;

    while(p != NULL){
        out<<p->data.Mamh<<"|"<<p->data.Diem<<endl;
        p = p->next;
    }
}

DTPtr DanhSachDiemThi::operator[](string _maMH){
    DTPtr p = First;
    while(p != NULL){
        if(strcmp(p->data.Mamh, (char*)_maMH.c_str()) == 0){
            return p;
        }
        p = p->next;
    }
    return p;
}