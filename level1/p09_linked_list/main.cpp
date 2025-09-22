#include <iostream>

// 定义链表节点结构体
struct ListNode {
    int value;
    ListNode* next;

    // 构造函数
    ListNode(int val) : value(val), next(nullptr) {}
};


class LinkedList {
private:
    ListNode* head;

public:
    // 构造函数
    LinkedList() : head(nullptr) {}

    // 析构函数 - 释放所有节点内存
    ~LinkedList() {
        ListNode* current = head;
        while (current != nullptr) {
            ListNode* next = current->next;
            delete current;
            current = next;
        }
    }

    // 1. 添加节点到链表尾部
    void append(int value) {
        ListNode* newNode = new ListNode(value);

        if (head == nullptr) {
            head = newNode;
        } else {
            ListNode* current = head;
            while (current->next != nullptr) {
                current = current->next;
            }
            current->next = newNode;
        }
    }

    // 2. 遍历链表并显示各节点的值
    void display() {
        ListNode* current = head;
        std::cout << "链表内容: ";
        while (current != nullptr) {
            std::cout << current->value;
            if (current->next != nullptr) {
                std::cout << " -> ";
            }
            current = current->next;
        }
        std::cout << " -> nullptr" << std::endl;
    }

    // 3. 反转链表
    void reverse() {
        ListNode* prev = nullptr;
        ListNode* current = head;
        ListNode* next = nullptr;

        while (current != nullptr) {
            next = current->next;  // 保存下一个节点
            current->next = prev;  // 反转指针
            prev = current;        // 移动prev
            current = next;        // 移动current
        }

        head = prev;  // 更新头节点
    }

    // 4. 查找第一个值为5的节点，返回序号（从0开始）
    int findFirst(int target) {
        ListNode* current = head;
        int index = 0;

        while (current != nullptr) {
            if (current->value == target) {
                return index;
            }
            current = current->next;
            index++;
        }

        return -1;  // 未找到
    }

    // 5. 从指定位置开始查找下一个值为5的节点
    int findNext(int target, int startIndex) {
        ListNode* current = head;
        int index = 0;

        // 移动到起始位置
        while (current != nullptr && index < startIndex) {
            current = current->next;
            index++;
        }

        // 从起始位置开始查找
        while (current != nullptr) {
            if (current->value == target) {
                return index;
            }
            current = current->next;
            index++;
        }

        return -1;  // 未找到
    }

    // 获取链表头节点（用于测试）
    ListNode* getHead() {
        return head;
    }
};

int main() {
    // 创建单向链表
    LinkedList myList;

    // 向链表中添加节点
    std::cout << "=== 创建链表 ===" << std::endl;
    myList.append(1);
    myList.append(3);
    myList.append(5);
    myList.append(7);
    myList.append(5);
    myList.append(9);
    myList.append(5);

    // 遍历显示链表
    myList.display();

    // 查找第一个值为5的节点
    std::cout << "\n=== 查找第一个值为5的节点 ===" << std::endl;
    int firstIndex = myList.findFirst(5);
    if (firstIndex != -1) {
        std::cout << "第一个值为5的节点序号: " << firstIndex << std::endl;
    } else {
        std::cout << "未找到值为5的节点" << std::endl;
    }

    // 查找下一个值为5的节点
    std::cout << "\n=== 查找下一个值为5的节点 ===" << std::endl;
    if (firstIndex != -1) {
        int nextIndex = myList.findNext(5, firstIndex + 1);
        if (nextIndex != -1) {
            std::cout << "下一个值为5的节点序号: " << nextIndex << std::endl;
        } else {
            std::cout << "没有找到下一个值为5的节点" << std::endl;
        }

        // 继续查找再下一个
        int thirdIndex = myList.findNext(5, nextIndex + 1);
        if (thirdIndex != -1) {
            std::cout << "再下一个值为5的节点序号: " << thirdIndex << std::endl;
        } else {
            std::cout << "没有找到第三个值为5的节点" << std::endl;
        }
    }

    // 反转链表
    std::cout << "\n=== 反转链表 ===" << std::endl;
    myList.reverse();
    myList.display();

    // 在反转后的链表中查找值为5的节点
    std::cout << "\n=== 反转后查找值为5的节点 ===" << std::endl;
    firstIndex = myList.findFirst(5);
    if (firstIndex != -1) {
        std::cout << "反转后第一个值为5的节点序号: " << firstIndex << std::endl;

        int nextIndex = myList.findNext(5, firstIndex + 1);
        if (nextIndex != -1) {
            std::cout << "反转后下一个值为5的节点序号: " << nextIndex << std::endl;
        }
    }

    // 测试查找不存在的值
    std::cout << "\n=== 测试查找不存在的值 ===" << std::endl;
    int notFound = myList.findFirst(100);
    std::cout << "查找值为100的节点结果: " << notFound << std::endl;

    return 0;
}