#ifndef HELPER_H
#define HELPER_H

#include <string>
#include <fstream>
#include <cstdint>
#include "DanhSachCauHoi.h"
#include "DanhSachSinhVien.h"
#include "DanhSachLop.h"
#include "DArray.h"

#define ENTER 13
#define BACKSPACE 8
#define MAX_LENGTH 50

extern bool Number(char);  
extern bool Word(char);
extern bool All(char);

extern char* Input(bool (*funcptr)(char) = All, bool secure = false, int64_t *convertible = nullptr);

extern void ThiTracNghiem(DanhSachCauHoi &list, std::string mon_thi, int so_cau_hoi);

extern int login(DanhSachLopHoc &dslh, SinhVien *&sv, string id, string pass);

extern uint64_t find(std::string str, char c);

extern std::string standardization(std::string str);

bool is_number(const std::string& s);

#endif