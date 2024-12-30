#include <iostream>
#include <vector>
#include <cmath>
#include <iterator>
using namespace std;

struct rectangle {
    int x_min, y_min, x_max, y_max;
    bool is_in(rectangle rec) {
        if (rec.x_min >= x_min && rec.x_max <= x_max && rec.y_min >= y_min && rec.y_max <= y_max) return true;
        else return false;
    }
};
class Node {
public:
    Node* parent;
    string name;
    static int max;
    rectangle mbr;
    vector<Node*> children;
    Node(string name, int x, int y) {
        mbr.x_max = mbr.x_min = x;
        mbr.y_max = mbr.y_min = y;
        this->name = name;
        parent = nullptr;
    }
    Node(Node const& n) {
        name = n.name;
        mbr = n.mbr;
        children = n.children;
        parent = nullptr;
    }
    bool insert(Node* t) {
        if (mbr.is_in(t->mbr)) {
            if (children.size() != 0) {
                if (!(children[0]->mbr.x_max == children[0]->mbr.x_min && children[0]->mbr.y_max == children[0]->mbr.y_min)) {
                    bool c = true;
                    for (int i = 0; i < children.size(); i++)
                    {
                        if (children[i]->mbr.is_in(t->mbr)) {
                            children[i]->insert(t);
                            c = false;
                            break;
                        }
                    }
                    if (c) {
                        double min_dist = 100000000;
                        int pos;
                        for (int i = 0; i < children.size(); i++) {
                            if (distance(children[i]->mbr, t) < min_dist) min_dist = distance(children[i]->mbr, t);
                            pos = i;
                        }
                        children[pos]->insert(t);
                    }
                }
                else {
                    children.push_back(t);
                    if (children.size() > max) split();
                }
            }
            else {
                children.push_back(t);
                t->parent = this;
                return true;
            }
            if (children.size() > max) split();
        }
        else {
            if (mbr.x_max == mbr.x_min && mbr.y_max == mbr.y_min) {
                Node rt(name + char(rand() % 20 + 65), mbr.x_min, mbr.y_min);//naming
                rt.children.push_back(new Node(name, mbr.x_min, mbr.y_min));
                rt.children.push_back(t);
                rt.resize_mbr();
                name = rt.name;
                mbr = rt.mbr;
                children = rt.children;
            }
            else {
                if (children.size() != 0) {
                    if ((children[0]->mbr.x_max == children[0]->mbr.x_min && children[0]->mbr.y_max == children[0]->mbr.y_min)) {
                        children.push_back(t);//а если не точки...
                        resize_mbr();
                        if (children.size() > max) split();
                    }
                    else {
                        children.push_back(t);
                        resize_mbr();
                        children.pop_back();
                        double min_dist = 100000000;
                        int pos;
                        for (int i = 0; i < children.size(); i++) {
                            if (distance(children[i]->mbr, t) < min_dist) min_dist = distance(children[i]->mbr, t);
                            pos = i;
                        }
                        children[pos]->insert(t);
                    }
                }
                else {
                    children.push_back(t);//а если не точки...
                    resize_mbr();
                    //if (children.size() > max) split();
                }
            }
        }
        return false;
    }
    double distance(rectangle rect, Node* p) {
        double dx = fmax(rect.x_min - p->mbr.x_min, p->mbr.x_min - rect.x_max);
        double dy = fmax(rect.y_min - p->mbr.y_min, p->mbr.y_min - rect.y_max);
        return sqrt(dx*dx + dy*dy);
    }
    double length(Node* n1, Node* n2) {
        return sqrt((n1->mbr.x_min - n2->mbr.x_min) * (n1->mbr.x_min - n2->mbr.x_min) + (n1->mbr.y_min - n2->mbr.y_min) * (n1->mbr.y_min - n2->mbr.y_min));
    }
    double length(rectangle p1, rectangle p2) {
        return sqrt((p1.x_min - p2.x_min) * (p1.x_min - p2.x_min) + (p1.y_min - p2.y_min) * (p1.y_min - p2.y_min));
    }
    void split() {
        vector<Node*> buf(children);
        int ind1, ind2;
        double max_length = 0;
        for (int i = 0; i < buf.size(); i++)
        {
            for (int j = i; j < buf.size(); j++)
            {
                double length = sqrt((buf[i]->mbr.x_min - buf[j]->mbr.x_min) * (buf[i]->mbr.x_min - buf[j]->mbr.x_min) + (buf[i]->mbr.y_min - buf[j]->mbr.y_min) * (buf[i]->mbr.y_min - buf[j]->mbr.y_min));
                if (max_length < length) {
                    ind1 = i;
                    ind2 = j;
                    max_length = length; 
                }
            }
        }//поиск идексов самых удаленных точек
        int size = children.size();
        for (int i = 0; i < size; i++)
        {
            auto iter = children.begin();
            children.erase(iter);
        }
        children.resize(0);
        Node new_vec(name+char(name[name.length()-1] + 1), buf[ind2]->mbr.x_min, buf[ind2]->mbr.y_min);
        for (int i = 0; i < buf.size(); i++)
        {
            if (length(buf[ind1], buf[i]) < length(buf[ind2], buf[i])) children.push_back(buf[i]);
            else new_vec.children.push_back(buf[i]);
        }
        mbr.x_max = mbr.x_min = children[0]->mbr.x_min;
        mbr.y_max = mbr.y_min = children[0]->mbr.y_min;
        resize_mbr();
        new_vec.resize_mbr();
        if (parent!=nullptr) parent->insert(new Node(new_vec));// new Node
        else { // same as insert
            Node rt(name + char(rand()%20+65), new_vec.mbr.x_min, new_vec.mbr.x_min);//naming
            rt.children.push_back(new Node(*this));
            rt.children.push_back(new Node(new_vec));
            rt.resize_mbr();
            name = rt.name;
            mbr = rt.mbr;
            children = rt.children;
        }
    }
    void resize_mbr() {
        for (int i = 0; i < children.size(); i++)
        {
            if (mbr.x_max < children[i]->mbr.x_max) mbr.x_max = children[i]->mbr.x_max;
            if (mbr.x_min > children[i]->mbr.x_min) mbr.x_min = children[i]->mbr.x_min;
            if (mbr.y_max < children[i]->mbr.y_max) mbr.y_max = children[i]->mbr.y_max;
            if (mbr.y_min > children[i]->mbr.y_min) mbr.y_min = children[i]->mbr.y_min;
        }
    }
    string search(int x, int y) {
        rectangle point;
        point.x_max = point.x_min = x;
        point.y_max = point.y_min = y;
        if (mbr.is_in(point)) {
            if (mbr.x_max == mbr.x_min && mbr.y_max == mbr.y_min) {
                if (mbr.x_min == x && mbr.y_min == y) return name;
            }
            else {
                for (int i = 0; i < children.size(); i++) {
                    string result = children[i]->search(x, y);
                    if (!result.empty()) {
                        return result;
                    }
                }
            }
        }
        return "";
    }
    void printTree() {
        if (this == nullptr) {
            return;
        }
        if (mbr.x_max == mbr.x_min && mbr.y_max == mbr.y_min) {
            cout << name << "(" << mbr.x_min << ", " << mbr.y_min << ")" << endl;
        }
        else {
            for (int i = 0; i < children.size(); i++)
            {
                children[i]->printTree();
            }
        }
    }
    void remove(int x, int y) {
        rectangle point;
        point.x_max = point.x_min = x;
        point.y_max = point.y_min = y;
        if (mbr.is_in(point)) {
            for (auto i = children.begin(); i != children.end(); i++) {
                if ((*i)->mbr.x_max == (*i)->mbr.x_min && (*i)->mbr.y_max == (*i)->mbr.y_min) {
                    if ((*i)->mbr.x_min == x && (*i)->mbr.y_min == y) { children.erase(i); break; }
                }
                else {
                    {
                        (*i)->remove(x, y);
                    }
                }
            }
        }
    }
    void search(int x1, int y1, int x2, int y2) {
        vector<string> result;
        rectangle square;
        square.x_min = min(x1, x2);
        square.x_max = std::max(x1, x2);
        square.y_min = min(y1, y2);
        square.y_max = std::max(y1, y2);

        if (mbr.is_in(square)) {
            if (children[0]->mbr.x_max == children[0]->mbr.x_min && children[0]->mbr.y_max == children[0]->mbr.y_min) {
                for (auto i = children.begin(); i != children.end(); i++) {
                    if (square.is_in((*i)->mbr)) cout << (*i)->name << endl;
                }
            }
            else{
                for (auto i = children.begin(); i != children.end(); i++) {
                    (*i)->search(x1, y1, x2, y2);
                }
            }
        }
        else {
            for (auto i = children.begin(); i != children.end(); i++) {
                if ((*i)->mbr.x_max == (*i)->mbr.x_min && (*i)->mbr.y_max == (*i)->mbr.y_min) {
                    if (square.is_in((*i)->mbr)) cout << (*i)->name << endl;
                }
                else {
                    (*i)->search(x1, y1, x2, y2);
                }
            }
        }
    }
    Node* findClosest(int x, int y) {
        rectangle point;
        point.x_max = point.x_min = x;
        point.y_max = point.y_min = y;
        double minLength = 10000000;
        Node* ind = nullptr;
        if (children[0]->mbr.x_max == children[0]->mbr.x_min && children[0]->mbr.y_max == children[0]->mbr.y_min)
        {
            for (auto i = children.begin(); i != children.end(); i++) {
                double length = sqrt(pow((*i)->mbr.x_min - x, 2) + pow((*i)->mbr.y_min - y, 2));
                if (length < minLength) {
                    minLength = length;
                    ind = (*i);
                }
            }
            return ind;
        }
        else
        {
            for (auto i = children.begin(); i != children.end(); i++) {
                if (mbr.is_in(point)) {
                    Node* result = (*i)->findClosest(x, y);
                    if (sqrt(pow(result->mbr.x_min - x, 2) + pow(result->mbr.y_min - y, 2)) < minLength) {
                        minLength = sqrt(pow(result->mbr.x_min - x, 2) + pow(result->mbr.y_min - y, 2));
                        ind = result;
                    }
                }
                else {
                    Node* result = (*i)->findClosest(x, y);
                    if (sqrt(pow(result->mbr.x_min - x, 2) + pow(result->mbr.y_min - y, 2)) < minLength) {
                        minLength = sqrt(pow(result->mbr.x_min - x, 2) + pow(result->mbr.y_min - y, 2));
                        ind = result;
                    }
                }
            }
            return ind;
        }
    }
};

int Node::max = 4;

int main()
{
    setlocale(LC_ALL,"Rus");
    srand(time(NULL));
    Node root(string(1,char(65)),5,7);
    int i = 1;
    for (i; i < 10; i++)
    {
        root.insert(new Node(string(1, char(65 + i)), rand() % 100, rand() % 100));
    }
    root.printTree();
    int x, y, q;
    int x1, y1, x2, y2;
    vector<string> res;
    do {
        cout << "Введите команду:\n1 - Добавить обьект\n2 - Удалить обьект\n3 - Найти обьект\n4 - Поиск по региону\n5 - Поиск ближайшего";
        cin >> q;
        switch (q) {
        case 1:
            cout << "Введите координаты: ";
            cin >> x >> y;
            root.insert(new Node(string(1, char(65 + i++)), x, y));
            root.printTree();
            break;
        case 2:
            cout << "Введите координаты: ";
            cin >> x >> y;
            root.remove(x, y);
            root.printTree();
            break;
        case 3:
            cout << "Введите координаты: ";
            cin >> x >> y;
            cout << root.search(x, y) << endl;
            break;
        case 4:
            cout << "Введите координаты первой точки: ";
            cin >> x1 >> y1;
            cout << "Введите координаты второй точки: ";
            cin >> x2 >> y2;
            root.search(x1, y1, x2, y2);
            break;
        case 5:
            cout << "Введите координаты: ";
            cin >> x >> y;
            cout << root.findClosest(x, y)->name << endl;
            break;
        default:;
        }
    } while (q != 0);
    return 0;
}

