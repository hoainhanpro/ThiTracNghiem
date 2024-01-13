#include "../header/DanhSachCauHoi.h"
#include <cstdint>
#include <sstream>
#include <cstring>
#include <iostream>

// void RandomID::binaryDistribution(Queue<IntPair> &attribute){
//     IntPair front = attribute.pop();

//     if(front.first <= front.second){
//         long long middle = (front.first + front.second) / 2;
//         to.push_back(from[middle]);

//         attribute.push({front.first, middle - 1});
//         attribute.push({middle + 1, front.second});
//     }

//     if(attribute.empty()) return;
//     else binaryDistribution(attribute);
// }

// void RandomID::randomize(){
//     srand(time(NULL));
//     std::ofstream out("../data/ID.txt");
//     Queue<IntPair> attribute;
//     attribute.push({0, MAX_ID - 1});
//     std::string attach;

//     binaryDistribution(attribute);

//     for (int i = 0; i < MAX_ID / DIVISION; i++){
//         for(int j = 0; j < DIVISION; j++){            
//             attach = (j == DIVISION - 1)? "|\n" : "|";
//             out << id_data[i * DIVISION + j] << attach;
//         }
//     }

//     out.close();
// }
Node *temporary = nullptr;

int DanhSachCauHoi::getId(){
    IntPair cur = id.pop();
    int result = (cur.first + cur.second)/2;

    id.push({cur.first, result - 1});
    id.push({result + 1, cur.second});

    return result; 
}

DanhSachCauHoi::CauHoi::CauHoi(){
    Id = -1;
    strncpy(ma_mon_hoc, "", 15);
    noi_dung = "";
    dap_an_a = "";
    dap_an_b = "";
    dap_an_c = "";
    dap_an_d = "";
    dap_an = "";
}

DanhSachCauHoi::CauHoi::CauHoi(int _Id, 
                                char _ma_mon_hoc[15], 
                                std::string _noi_dung, 
                                std::string _dap_an_a, 
                                std::string _dap_an_b, 
                                std::string _dap_an_c, 
                                std::string _dap_an_d, 
                                std::string _dap_an){
    Id = _Id;
    strncpy(ma_mon_hoc, _ma_mon_hoc, 15);
    noi_dung = _noi_dung;
    dap_an_a = _dap_an_a;
    dap_an_b = _dap_an_b;
    dap_an_c = _dap_an_c;
    dap_an_d = _dap_an_d;
    dap_an = _dap_an;
}

DanhSachCauHoi::CauHoi::CauHoi( char _ma_mon_hoc[15], 
                                std::string _noi_dung, 
                                std::string _dap_an_a, 
                                std::string _dap_an_b, 
                                std::string _dap_an_c, 
                                std::string _dap_an_d, 
                                std::string _dap_an){
    Id = -1;
    strncpy(ma_mon_hoc, _ma_mon_hoc, 15);
    ma_mon_hoc[15] = '\0';
    noi_dung = _noi_dung;
    dap_an_a = _dap_an_a;
    dap_an_b = _dap_an_b;
    dap_an_c = _dap_an_c;
    dap_an_d = _dap_an_d;
    dap_an = _dap_an;
}

DanhSachCauHoi::CauHoi &DanhSachCauHoi::CauHoi::operator=(CauHoi &other){
    this->Id = other.Id;
    strcpy(this->ma_mon_hoc, other.ma_mon_hoc);
    this->noi_dung = other.noi_dung;
    this->dap_an_a = other.dap_an_a;
    this->dap_an_b = other.dap_an_b;
    this->dap_an_c = other.dap_an_c;
    this->dap_an_d = other.dap_an_d;
    this->dap_an = other.dap_an;
    return *this;
}

DanhSachCauHoi::Node::Node() : data(){
    left = nullptr;
    right = nullptr;
}

DanhSachCauHoi::Node::Node(CauHoi _cau_hoi){
    data = _cau_hoi;
    left = nullptr;
    right = nullptr;
}
 
DanhSachCauHoi::Node::~Node(){
    delete left;
    delete right;
}

DanhSachCauHoi::DanhSachCauHoi(){
    root = nullptr;
    std::ifstream in("../data/ID.txt");
    if(in.good()){
        std::string line;
        while(getline(in, line)){
            std::stringstream ss(line);
            std::string token;
            while(getline(ss, token, '|')){
                std::stringstream ss2(token);
                std::string token2;
                getline(ss2, token2, ',');
                int first = std::stoi(token2);
                getline(ss2, token2, ',');
                int second = std::stoi(token2);
                id.push({first, second});
            }
        }
    }
    else id.push({0, MAX_ID - 1});
}

DanhSachCauHoi::~DanhSachCauHoi(){
    Queue<Node*> queue;
    queue.push(root);
    std::ofstream out("../data/DANHSACHCAUHOI.txt");
    update(queue, out);
    out.close();

    out.open("../data/ID.txt");
    int size = id.size()/DIVISION + 1;
    for(int i = 0; i < size; i++){
        for(int j = 0; j < DIVISION; j++){
            if(id.empty()) break;
            IntPair current_id = id.pop();
            std::string attach = (j == DIVISION - 1)? "|\n" : "|";
            out <<current_id.first<<","<<current_id.second<< attach;
        }
    }
    out.close();
    delete root;
}

DanhSachCauHoi::Node *&DanhSachCauHoi::getRoot(){
    return root;
}

DanhSachCauHoi::DanhSachCauHoi(std::string path) : DanhSachCauHoi(){
    std::ifstream input(path);

    if(!input.is_open()){
        std::string error = "Không thể mở file: " + path;
        throw error;
    }

    std::string line;
    while(getline(input, line)){
        std::stringstream _line(line);
        std::string id, ma_mon, noi_dung, cau_a, cau_b, cau_c, cau_d, dap_an;
        getline(_line, id, '|');
        getline(_line, ma_mon, '|');
        getline(_line, noi_dung, '|');
        getline(_line, cau_a, '|');
        getline(_line, cau_b, '|');
        getline(_line, cau_c, '|');
        getline(_line, cau_d, '|');
        getline(_line, dap_an);
        
        insert({
            stoi(id),
            (char*)ma_mon.c_str(),
            noi_dung,
            cau_a,
            cau_b,
            cau_c,
            cau_d,
            dap_an
        });
    }
}

void DanhSachCauHoi::insert(CauHoi _cau_hoi){
    if(_cau_hoi.Id == -1) _cau_hoi.Id = getId();
    insert(root, _cau_hoi);    
}

void DanhSachCauHoi::insert(Node *&cur, CauHoi _cau_hoi){ 
    if(cur == nullptr) cur = new Node(_cau_hoi);
    else{
        if(cur->data.Id > _cau_hoi.Id) insert(cur->left, _cau_hoi);
        else insert(cur->right, _cau_hoi);
    }
}

void DanhSachCauHoi::remove(int id){
    remove(root, id); 
}

void DanhSachCauHoi::remove(Node *&cur, int id){
    if(cur == nullptr){
        std::string error = "Không tìm thấy câu hỏi có ID: " + std::to_string(id);
        throw error;
    }
    else if(id < cur->data.Id) remove(cur->left, id);
    else if(id > cur->data.Id) remove(cur->right, id);
    else{
        temporary = cur;
        if(temporary->right == nullptr) cur = temporary->left;
        else if(temporary->left == nullptr) cur = temporary->right;
        else removeWithTwoChildren(cur->right);
        delete temporary;
    }
}

void DanhSachCauHoi::removeWithTwoChildren(Node *&cur){
    if(cur->left != nullptr)
        removeWithTwoChildren(cur->left);
    else{
        temporary->data = cur->data;
        temporary = cur;
        cur = temporary->right;
    }
}

void DanhSachCauHoi::update(Queue<Node*> &queue, std::ofstream &out){
    Node *cur = queue.pop();

    if(cur != nullptr){
        out<<cur->data.Id<<"|"<<cur->data.ma_mon_hoc<<"|"<<cur->data.noi_dung<<"|"<<cur->data.dap_an_a<<"|"<<cur->data.dap_an_b<<"|"<<cur->data.dap_an_c<<"|"<<cur->data.dap_an_d<<"|"<<cur->data.dap_an<<std::endl;
        queue.push(cur->left);
        queue.push(cur->right);
    }

    if(!queue.empty()) update(queue, out);
}

void DanhSachCauHoi::output(){
    output(root);
}

void DanhSachCauHoi::output(Node *cur){
    if(cur != nullptr){
        std::cout<<cur->data.Id<<" ";
        output(cur->left);
        output(cur->right);
    }
}

void DanhSachCauHoi::getQuestionList(DArray<CauHoi*> &list, std::string mon_hoc){
    getQuestionList(root, list, mon_hoc);
}

void DanhSachCauHoi::getQuestionList(Node *&cur, DArray<CauHoi*> &list, std::string mon_hoc){
    if(cur != nullptr){
        getQuestionList(cur->left, list, mon_hoc);
        if(strcmp(cur->data.ma_mon_hoc, mon_hoc.c_str()) == 0){
            list.push_back(&cur->data);
        }
        getQuestionList(cur->right, list, mon_hoc);
    }
}

CauHoi &DanhSachCauHoi::getQuestion(int id){
    return getQuestion(id, root);
}

CauHoi &DanhSachCauHoi::getQuestion(int id, Node *&cur){
    if(id == cur->data.Id) return cur->data;
    else if(id < cur->data.Id) return getQuestion(id, cur->left);
    else return getQuestion(id, cur->right);
}