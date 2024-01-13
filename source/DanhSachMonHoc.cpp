#include "../header/DanhSachMonHoc.h"
#include <string.h>
#include <fstream>

DanhSachMonHoc::MonHoc::MonHoc(){
    strncpy(ma_mon_hoc,"", 15);
    ma_mon_hoc[15] = '\0';
    ten_mon_hoc = "";
}

DanhSachMonHoc::MonHoc::MonHoc(char ma_mon_hoc[15], std::string ten_mon_hoc){
    strncpy(this->ma_mon_hoc, ma_mon_hoc, 15);
    ma_mon_hoc[15] = '\0';
    this->ten_mon_hoc = ten_mon_hoc;
}

std::ostream &operator<<(std::ostream &out, MonHoc mon_hoc){
    out<<mon_hoc.ma_mon_hoc<<"|"<<mon_hoc.ten_mon_hoc;
    return out;
}

DanhSachMonHoc::DanhSachMonHoc(){
    length = 0;
}

DanhSachMonHoc::DanhSachMonHoc(std::string path) : DanhSachMonHoc(){
    std::string rawline = "";
    std::ifstream input(path);
    while(std::getline(input, rawline)){
        std::string ma_mon = rawline.substr(0, rawline.find("|"));
        std::string ten_mon = rawline.substr(rawline.find("|") + 1, rawline.size() - 1);

        this->insert({(char*)ma_mon.c_str(), ten_mon});
    }
}

void DanhSachMonHoc::move(int index, int offset){
    if(offset == 1){
        MonHoc temp1 = data[index];
        for(int i = index + offset; i <= length; i++){
            MonHoc temp2 = data[i];
            data[i] = temp1;
            temp1 = temp2;
        }
    }
    else if(offset == -1){
        MonHoc temp1 = data[length - 1];
        for(int i = length - 2; i >= index; i--){
            MonHoc temp2 = data[i];
            data[i] = temp1;
            temp1 = temp2;
        }
    }
}

/**
 * @brief Thêm môn học vào danh sách
 * @param mon_hoc Môn học cần thêm
 * @param write_to_file Nếu là true thì sẽ cập nhật dữ liệu vào file
 * @throw "Danh sách môn học đã đầy" Nếu danh sách đã đầy
 * @throw "Mã môn học đã tồn tại" Nếu mã môn học đã tồn tại
*/
void DanhSachMonHoc::insert(MonHoc mon_hoc){ 
    if(length == MAX_MON){
        throw "Danh sách môn học đã đầy";
        return;
    }
    if(search(mon_hoc.ma_mon_hoc) != -1){
        std::string error = "Mã môn học đã tồn tại: " + std::string(mon_hoc.ma_mon_hoc);
        throw error;
        return;
    } 
    for(int i = 0; i<length; i++){
        if(strcmp(data[i].ma_mon_hoc, mon_hoc.ma_mon_hoc) > 0){
            this->move(i, 1);
            data[i] = mon_hoc;
            length++;
            return;
        }
    }
    data[length++] = mon_hoc;
}

/**
 * @brief Tìm kiếm môn học theo mã môn học
 * @param ma_mon_hoc Mã môn học cần tìm
 * @return Vị trí của môn học trong danh sách
*/
int DanhSachMonHoc::search(char ma_mon_hoc[]){
    for(int i = 0; i<length; i++){
        if(strcmp(data[i].ma_mon_hoc, ma_mon_hoc) == 0){
            return i;
        }
    }
    return -1;
}

/**
 * @brief Xóa môn học khỏi danh sách
 * @param ma_mon_hoc Mã môn học cần xóa
 * @throw "Không tìm thấy mã môn học" Nếu không tìm thấy mã môn học
*/
void DanhSachMonHoc::remove(char ma_mon_hoc[]){
    int index;
    if((index = search(ma_mon_hoc)) == -1){
        std::string error = "Không tìm thấy mã môn học: " + std::string(ma_mon_hoc);
        throw error;
        return;
    }
    this->move(index, -1);
    length--;
}

/**
 * @brief Cập nhật dữ liệu vào file
 * @param mon_hoc Môn học cần cập nhật
 * @throw "Không thể mở file" Nếu không thể mở file
*/
void DanhSachMonHoc::update(){
    std::ofstream output("../data/DANHSACHMON.txt");
    if(!output.is_open()){
        throw "Không thể mở file";
        return;
    }
    for(int i = 0; i<length; i++){
        output<<data[i]<<std::endl;
    }
    output.close();
}

void DanhSachMonHoc::output(){
    printf("%d\n",length);
    for(int i = 0; i<length;  i++){
        printf("%s      %s\n", data[i].ma_mon_hoc, data[i].ten_mon_hoc.c_str());
    }
}

MonHoc &DanhSachMonHoc::operator[](int index){
    return data[index];
}

MonHoc &DanhSachMonHoc::operator[](char _ma_mon_hoc[]){
    return data[search(_ma_mon_hoc)];
}

short DanhSachMonHoc::getLength(){
    return length;
}

DanhSachMonHoc::~DanhSachMonHoc(){
    update();
}