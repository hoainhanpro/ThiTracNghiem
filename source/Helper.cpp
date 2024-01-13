#include "../header/Helper.h"
#include <conio.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <fstream>

bool Number(char c){
    return c >= '0' && c <= '9';
}

bool Word(char c){
    return (c >= 'a' && c <= 'z') ||
           (c >= 'A' && c <= 'Z') ||
           (c == ' ');
}

bool All(char c){
    return c >= 32 && c <= 126;
}

char* Input(bool (*funcptr)(char), bool secure, int64_t *convertible){
    char *str = (char*)malloc(MAX_LENGTH);
    int i = 0;
    int barrier;  // Có 2 công dụng, cản kí tự đặc biệt và lấy kí tự vừa bấm

    while((barrier = getch()) != ENTER){
        if (barrier == 0 || barrier == 224){
            getch();
            continue;
        }
        
        if(funcptr(barrier)) printf(secure? "X" : "%c" ,str[i++] = barrier);  // Hiện kí tự vừa bấm

        if(barrier == BACKSPACE && i > 0){  // xóa kí tự
            printf("\b \b");
            i--;
        }
    }

    str[i] = '\0';

    if(convertible != nullptr){  // Đổi char* thành số, dùng khi có truyền vào biến
        (*convertible) = -1;
        if(i != 0){
            i = 0;
            (*convertible) = 0;
            while(str[i + 1] != '\0'){
                (*convertible) += (str[i] - '0');
                (*convertible) *= 10;
                i++;
            }
            (*convertible) += (str[i] - '0');
        }
    }
    return str;
}

// void ThiTracNghiem(DanhSachCauHoi &list, std::string mon_thi, int so_cau_hoi){
//     CauHoi *list_cau_hoi_thi[so_cau_hoi];
//     for(int i = 0; i<so_cau_hoi; i++) list_cau_hoi_thi[i] = nullptr;
//     int length = 0;
//     list.getQuestionList(list_cau_hoi_thi, length, so_cau_hoi, mon_thi);
//     bool result[length];
//     for(int i = 0; i<length; i++){
//         std::string ans;
//         std::cout<<list_cau_hoi_thi[i]->noi_dung<<std::endl;
//         std::cout<<list_cau_hoi_thi[i]->dap_an_a<<"                        "<<list_cau_hoi_thi[i]->dap_an_b<<std::endl;
//         std::cout<<list_cau_hoi_thi[i]->dap_an_c<<"                        "<<list_cau_hoi_thi[i]->dap_an_d<<std::endl;
//         std::cout<<"chon dap an: ";
//         std::cin>>ans;
//         result[i] = ans.compare(list_cau_hoi_thi[i]->dap_an) == 0;
//     }
//     for(int i = 0; i<length; i++){
//         std::cout<<result[i]<<" ";
//     }
// }

uint64_t find(std::string str, char c){
    for(int i = str.length() - 1; i >= 0; i--){
        if(str[i] == ' '){
            return std::string::npos;
        }
        if(str[i] == c){
            if(c == 'u' || c == 'U'){
                for(int j = i - 1; j >= 0; j--){
                    if(str[j] == ' ') return i;
                    if(str[j] == c) return j;
                }
            }
            return i;
        }
    }
    return std::string::npos;
}

int login(DanhSachLopHoc &dslh, SinhVien *&sv, string id, string pass){
    if(id == "GV" && pass == "GV") return 2;

    for(int i = 0; i<dslh.getSoLuong(); i++){
        DanhSachSinhVien *dssv = dslh[i]->getDSSV();
        SVPtr first = dssv->getFirst();

        while(first != nullptr){
            if(first->sv_data.MASV == id && first->sv_data.password == pass){
                sv = &(first->sv_data);
                return 1;
            }
            first = first->next;
        }
    }

    return 0;
}

std::string standardization(std::string input) {
    // Convert all characters to lowercase
    for (char& c : input) {
        if (c >= 'A' && c <= 'Z') {
            c += ('a' - 'A');
        }
    }

    // Remove extra spaces and leading zeros
    std::string output;
    bool last_char_was_space = false;
    bool leading_zeros = true;
    for (char c : input) {
        if (c == ' ') {
            if (!last_char_was_space) {
                output += ' ';
                leading_zeros = true;
                last_char_was_space = true;
            }
        } else if (std::isdigit(c)) {
            if (c != '0' || !leading_zeros) {
                output += c;
                leading_zeros = false;
            }
            last_char_was_space = false;
        } else {
            output += c;
            leading_zeros = false;
            last_char_was_space = false;
        }
    }

    return output;
}

bool is_number(const std::string& s)
{
    std::string::const_iterator it = s.begin();
    while (it != s.end() && std::isdigit(*it)) ++it;
    return !s.empty() && it == s.end();
}