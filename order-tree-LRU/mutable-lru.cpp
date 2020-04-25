#include <stdio.h>
#include <unordered_map>

struct Node {
    Node *next, *last;
    int key, value;
    Node() {
        next = nullptr;
        last = nullptr;
    }
};

class LRUCache {
private:
    int capacity;
    int used;
    std::unordered_map<int, Node*> map;
    Node *lru_head; // 最新
    Node *lru_tail; // 最舊
    void show() {
        printf("forward: ");
        for (Node *iter = lru_head; iter != nullptr; iter = iter->next) {
            printf("(%d, %d)", iter->key, iter->value);
        }
        printf("\n");
        printf("backward: ");
        for (Node *iter = lru_tail; iter != nullptr; iter = iter->last) {
            printf("(%d, %d)", iter->key, iter->value);
        }
        printf("\n");
    }
    void to_head(Node *node) {
        if (lru_head == lru_tail || lru_head == node) {
            // do nothing
        } else if (lru_tail == node) {
            lru_tail = node->last;
            
            node->last->next = nullptr;
            node->last = nullptr;
            node->next = lru_head;
            lru_head->last = node;
            
            lru_head = node;
        } else {
            node->last->next = node->next;
            node->next->last = node->last;
            
            node->last = nullptr;
            node->next = lru_head;
            lru_head->last = node;
            lru_head = node;
        }
    }
public:
    LRUCache(int capacity) {
        this->capacity = capacity;
        this->used = 0;
        lru_head = nullptr;
        lru_tail = nullptr;
    }
    
    
    int get(int key) {
        Node *node = this->map[key];
        if (node == nullptr) {
            return -1;
        }
        this->to_head(node);
        // this->show();
        return node->value;
    }
    
    void put(int key, int value) {
        Node *node = this->map[key];
        if (node == nullptr) {
            if (this->used == 0) {
                Node* node = new Node();
                this->map[key] = node;
                node->key = key;
                node->value = value;
                
                this->used += 1;
                lru_tail = node;
                lru_head = node;
            } else if (this->used < this->capacity) {
                Node *node = new Node();
                this->map[key] = node;
                node->key = key;
                node->value = value;
                
                this->used += 1;
                node->next = lru_head;
                lru_head->last = node;
                lru_head = node;
            } else {
                // printf("put %d, %d\n", key, value);
                this->map[lru_tail->key] = nullptr;
                lru_tail->key = key;
                lru_tail->value = value;
                this->map[key] = lru_tail;
                this->to_head(lru_tail);
            }
        } else {
            node->value = value;
            this->to_head(node);
        }
        // this->show();
        return;
    }
};