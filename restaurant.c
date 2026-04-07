#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_WAIT 10
#define TABLES 5
#define MIN_DINE 1   // minimum 1 hour
#define MAX_DINE 2   // max 2 hours

struct Item {
    char name[30];
    float price;
};

struct Order {
    int itemNo;
    int quantity;
    float total;
    struct Order *next;
};

struct Customer {
    int token;
    char name[50];
    int vip;
    int dineTime;          // dining time in hours
    int waitTime;          // waiting time estimate
    struct Order *orderList;
    float totalBill;
    struct Customer *next;
};

struct Customer *front = NULL, *rear = NULL;
struct Customer *seated = NULL;

int tokenCounter = 1;
int availableTables = TABLES;

struct Item menu[] = {
    {"Pizza", 200},
    {"Burger", 150},
    {"Pasta", 180},
    {"Sandwich", 120},
    {"Noodles", 160},
    {"Fried Rice", 170},
    {"Ice Cream", 90},
    {"Cold Drink", 50},
    {"Coffee", 80},
    {"Soup", 110}
};

int menuSize = 10;

void displayMenu() {
    printf("\n------ MENU ------\n");
    for (int i = 0; i < menuSize; i++)
        printf("%d. %-15s Rs. %.2f\n", i + 1, menu[i].name, menu[i].price);
    printf("------------------\n");
}

struct Customer* createCustomer() {
    struct Customer *newCust = (struct Customer*)malloc(sizeof(struct Customer));
    newCust->token = tokenCounter++;
    newCust->orderList = NULL;
    newCust->totalBill = 0;
    newCust->next = NULL;

    printf("\nEnter customer name: ");
    scanf("%s", newCust->name);

    printf("VIP Customer? (1-Yes / 0-No): ");
    scanf("%d", &newCust->vip);

    displayMenu();

    int totalItems;
    printf("\nEnter how many different items you want to order: ");
    scanf("%d", &totalItems);

    for (int i = 0; i < totalItems; i++) {
        int choice, qty;
        printf("Enter item number: ");
        scanf("%d", &choice);

        if (choice < 1 || choice > menuSize) {
            printf("Invalid item! Try again.\n");
            i--;
            continue;
        }

        printf("Enter quantity for %s: ", menu[choice - 1].name);
        scanf("%d", &qty);

        struct Order *newOrder = (struct Order*)malloc(sizeof(struct Order));
        newOrder->itemNo = choice;
        newOrder->quantity = qty;
        newOrder->total = qty * menu[choice - 1].price;
        newOrder->next = newCust->orderList;
        newCust->orderList = newOrder;
        newCust->totalBill += newOrder->total;
    }

    // Random dining time between 1 to 2 hours
    newCust->dineTime = (rand() % (MAX_DINE - MIN_DINE + 1)) + MIN_DINE;

    return newCust;
}

void enqueue(struct Customer *newCust) {
    if (rear == NULL)
        front = rear = newCust;
    else {
        rear->next = newCust;
        rear = newCust;
    }
}

struct Customer* dequeue() {
    if (front == NULL)
        return NULL;
    struct Customer *temp = front;
    front = front->next;
    if (front == NULL)
        rear = NULL;
    temp->next = NULL;
    return temp;
}

void updateWaitingTimes() {
    struct Customer *temp = front;
    int position = 1;

    while (temp) {
        temp->waitTime = position * MAX_DINE; // 2 hrs per position
        temp = temp->next;
        position++;
    }
}

void bookTable() {
    struct Customer *newCust = createCustomer();

    if (availableTables > 0) {
        newCust->next = seated;
        seated = newCust;
        availableTables--;
        printf("\nTable booked successfully for %s (Token %d).\n", newCust->name, newCust->token);
        printf("Dining time: %d hours\n", newCust->dineTime);
    } else if (tokenCounter <= MAX_WAIT + TABLES) {
        enqueue(newCust);
        updateWaitingTimes();
        printf("\nAll tables full! %s is added to waiting list (Token %d).\n", newCust->name, newCust->token);
        printf("Expected wait time: %d hours\n", newCust->waitTime);
    } else {
        printf("\nWaiting list full. Please visit later!\n");
        free(newCust);
    }
}

void serveCustomer() {
    if (seated == NULL) {
        printf("\nNo seated customers to serve.\n");
        return;
    }

    struct Customer *served = seated;
    seated = seated->next;
    availableTables++;

    printf("\nServing customer: %s (Token %d)\n", served->name, served->token);
    printf("Dining Time Completed: %d hr\n", served->dineTime);
    printf("------ Bill Details ------\n");

    struct Order *ord = served->orderList;
    while (ord) {
        printf("%s x%d = Rs. %.2f\n", menu[ord->itemNo - 1].name, ord->quantity, ord->total);
        ord = ord->next;
    }

    printf("Total Bill: Rs. %.2f\n", served->totalBill);
    printf("--------------------------\n");

    free(served);

    if (front != NULL) {
        struct Customer *nextCust = dequeue();
        nextCust->next = seated;
        seated = nextCust;
        availableTables--;

        printf("\nNext waiting customer %s (Token %d) got a table.\n", nextCust->name, nextCust->token);
        printf("Dining time: %d hours\n", nextCust->dineTime);
    }

    updateWaitingTimes();
}

void displayWaitingList() {
    printf("\n------ Waiting List ------\n");
    if (front == NULL)
        printf("No customers in waiting.\n");
    else {
        struct Customer *temp = front;
        while (temp) {
            printf("Token %d - %s | VIP: %d | Wait: %d hrs\n",
                   temp->token, temp->name, temp->vip, temp->waitTime);
            temp = temp->next;
        }
    }
    printf("--------------------------\n");
    printf("Available Tables: %d / %d\n", availableTables, TABLES);
}

int main() {
    srand(time(NULL));
    int choice;
    printf("\n=== RESTAURANT BOOKING SYSTEM ===\n");

    while (1) {
        printf("\n1. Book Table\n2. Serve Customer\n3. Display Waiting List\n4. Exit\nEnter your choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1: bookTable(); break;
            case 2: serveCustomer(); break;
            case 3: displayWaitingList(); break;
            case 4: exit(0);
            default: printf("Invalid choice!\n");
        }
    }
    return 0;
}