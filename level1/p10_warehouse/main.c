#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_ITEMS 100
#define FILENAME "inventory.dat"

// 商品结构体
typedef struct {
    char model[50];  // 型号
    int quantity;    // 数量
} Product;

Product inventory[MAX_ITEMS];
int item_count = 0;


void loadInventory();
void saveInventory();
void displayMenu();
void showInventory();
void addStock();
void removeStock();

int main() {
    // 程序启动时加载库存数据
    loadInventory();

    int choice;

    do {
        displayMenu();
        printf("请选择功能 (1-4): ");
        scanf("%d", &choice);

        switch(choice) {
            case 1:
                showInventory();
                break;
            case 2:
                addStock();
                break;
            case 3:
                removeStock();
                break;
            case 4:
                saveInventory();
                printf("程序已退出，库存数据已保存。\n");
                break;
            default:
                printf("无效选择，请重新输入！\n");
        }

        printf("\n");
    } while(choice != 4);

    return 0;
}

// 显示菜单
void displayMenu() {
    printf("\n=== 简单进销存系统 ===\n");
    printf("1. 显示存货列表\n");
    printf("2. 入库\n");
    printf("3. 出库\n");
    printf("4. 退出程序\n");
    printf("=====================\n");
}

// 从文件加载库存数据
void loadInventory() {
    FILE *file = fopen(FILENAME, "rb");
    if (file == NULL) {
        printf("未找到库存文件，将创建新文件。\n");
        return;
    }

    // 读取商品数量
    fread(&item_count, sizeof(int), 1, file);

    // 读取商品数据
    for(int i = 0; i < item_count; i++) {
        fread(&inventory[i], sizeof(Product), 1, file);
    }

    fclose(file);
    printf("库存数据加载成功，共 %d 种商品。\n", item_count);
}

// 保存库存数据到文件
void saveInventory() {
    FILE *file = fopen(FILENAME, "wb");
    if (file == NULL) {
        printf("错误：无法创建库存文件！\n");
        return;
    }

    // 保存商品数量
    fwrite(&item_count, sizeof(int), 1, file);

    // 保存商品数据
    for(int i = 0; i < item_count; i++) {
        fwrite(&inventory[i], sizeof(Product), 1, file);
    }

    fclose(file);
    printf("库存数据保存成功。\n");
}

// 显示存货列表
void showInventory() {
    if (item_count == 0) {
        printf("库存为空！\n");
        return;
    }

    printf("\n=== 存货列表 ===\n");
    printf("%-4s %-20s %-8s\n", "序号", "型号", "数量");
    printf("--------------------------------\n");

    for(int i = 0; i < item_count; i++) {
        printf("%-4d %-20s %-8d\n",
               i + 1,
               inventory[i].model,
               inventory[i].quantity);
    }
}

// 入库功能
void addStock() {
    char model[50];
    int quantity;

    printf("\n=== 入库操作 ===\n");
    printf("请输入商品型号: ");
    scanf("%s", model);
    printf("请输入入库数量: ");
    scanf("%d", &quantity);

    if (quantity <= 0) {
        printf("错误：入库数量必须大于0！\n");
        return;
    }

    // 检查商品是否已存在
    for(int i = 0; i < item_count; i++) {
        if (strcmp(inventory[i].model, model) == 0) {
            inventory[i].quantity += quantity;
            printf("商品 '%s' 入库成功！当前库存: %d\n", model, inventory[i].quantity);
            return;
        }
    }

    // 如果商品不存在，添加新商品
    if (item_count < MAX_ITEMS) {
        strcpy(inventory[item_count].model, model);
        inventory[item_count].quantity = quantity;
        item_count++;
        printf("新商品 '%s' 添加成功！库存数量: %d\n", model, quantity);
    } else {
        printf("错误：库存已满，无法添加新商品！\n");
    }
}

// 出库功能
void removeStock() {
    char model[50];
    int quantity;

    if (item_count == 0) {
        printf("库存为空，无法出库！\n");
        return;
    }

    printf("\n=== 出库操作 ===\n");
    printf("请输入商品型号: ");
    scanf("%s", model);
    printf("请输入出库数量: ");
    scanf("%d", &quantity);

    if (quantity <= 0) {
        printf("错误：出库数量必须大于0！\n");
        return;
    }

    // 查找商品
    for(int i = 0; i < item_count; i++) {
        if (strcmp(inventory[i].model, model) == 0) {
            if (inventory[i].quantity >= quantity) {
                inventory[i].quantity -= quantity;
                printf("商品 '%s' 出库成功！剩余库存: %d\n", model, inventory[i].quantity);

                // 如果库存为0，删除该商品记录
                if (inventory[i].quantity == 0) {
                    // 将最后一个商品移到当前位置
                    inventory[i] = inventory[item_count - 1];
                    item_count--;
                    printf("商品 '%s' 库存为0，已从列表中移除。\n", model);
                }
            } else {
                printf("错误：库存不足！当前库存: %d，出库需求: %d\n",
                       inventory[i].quantity, quantity);
            }
            return;
        }
    }

    printf("错误：未找到商品 '%s'！\n", model);
}