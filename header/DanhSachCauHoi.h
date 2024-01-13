#ifndef DANHSACHCAUHOI_H
#define DANHSACHCAUHOI_H

#include <string>
#include <fstream>
#include "Queue.h"
#include "DArray.h"

#define MAX_ID 100000
#define DIVISION 100

class DanhSachCauHoi{
    public:
        struct CauHoi
        {
            int Id;
            char ma_mon_hoc[16];
            std::string noi_dung;
            std::string dap_an_a;
            std::string dap_an_b;
            std::string dap_an_c;
            std::string dap_an_d;
            std::string dap_an;

            CauHoi();
            CauHoi(int _Id, char _ma_mon_hoc[16],
                   std::string _noi_dung, std::string _dap_an_a,
                   std::string _dap_an_b, std::string _dap_an_c,
                   std::string _dap_an_d, std::string _dap_an);

            CauHoi(char _ma_mon_hoc[15],
                   std::string _noi_dung, std::string _dap_an_a,
                   std::string _dap_an_b, std::string _dap_an_c,
                   std::string _dap_an_d, std::string _dap_an);

            CauHoi &operator=(CauHoi &other);
        };

        struct Node
        {
            CauHoi data;
            Node *left;
            Node *right;

            Node();
            Node(CauHoi _cau_hoi);
            ~Node();
        };

        DanhSachCauHoi();
        DanhSachCauHoi(std::string path);
        ~DanhSachCauHoi();
        void insert(CauHoi _cau_hoi);
        void remove(int id);
        void output();
        int getId();
        CauHoi &getQuestion(int id);
        Node *&getRoot();

        void getQuestionList(DArray<CauHoi*> &list, std::string mon_hoc);

    private:
        Node *root;
        Queue<IntPair> id;

        void output(Node *cur);
        void getQuestionList(Node *&cur, DArray<CauHoi*> &list, std::string mon_hoc);
        void update(Queue<Node*> &queue,std::ofstream &out);
        void insert(Node *&cur, CauHoi _cau_hoi);
        void remove(Node *&cur, int id);
        void removeWithTwoChildren(Node *&cur);
        CauHoi &getQuestion(int id, Node *&cur);
};

typedef typename DanhSachCauHoi::CauHoi CauHoi;
typedef typename DanhSachCauHoi::Node Node;

#endif