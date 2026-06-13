#include <iostream>
#include <string>
#include <iomanip>
#include <fstream>
#include <limits>
using namespace std;

const int MAX_ITEMS = 20;
const int MAX_TABLES = 10;

// ---------------- MenuItem Struct ----------------
struct MenuItem {
    string name;
    double price;
};

// ---------------- Menu Class ----------------
class Menu {
    MenuItem items[MAX_ITEMS];
    int count;

public:
    Menu() {
        count = 12;
        items[0] = {"Burger", 250};
        items[1] = {"Pizza", 700};
        items[2] = {"Fries", 150};
        items[3] = {"Sandwich", 200};
        items[4] = {"Cold Drink", 80};
        items[5] = {"Nuggets", 180};
        items[6] = {"Chicken Roll", 220};
        items[7] = {"Biryani", 300};
        items[8] = {"Ice Cream", 100};
        items[9] = {"Tea", 50};
        items[10] = {"Coffee", 120};
        items[11] = {"Pasta", 350};
    }

    void displayMenu() {
        cout << "\n-------- MENU --------\n";
        for (int i = 0; i < count; ++i) {
            cout << i + 1 << ". " << items[i].name << " - Rs." << fixed << setprecision(2) << items[i].price << endl;
        }
    }

    MenuItem getItem(int index) {
        return items[index];
    }

    int getCount() {
        return count;
    }
};

// ---------------- TableManager Class ----------------
class TableManager {
    bool tables[MAX_TABLES];

public:
    TableManager() {
        for (int i = 0; i < MAX_TABLES; i++)
            tables[i] = false;
    }

    bool reserveTable(int num) {
        if (num > 0 && num <= MAX_TABLES && !tables[num - 1]) {
            tables[num - 1] = true;
            return true;
        }
        return false;
    }

    void releaseTable(int num) {
        if (num > 0 && num <= MAX_TABLES)
            tables[num - 1] = false;
    }

    void showStatus() {
        cout << "\n--- Table Status ---\n";
        for (int i = 0; i < MAX_TABLES; i++)
            cout << "Table " << i + 1 << ": " << (tables[i] ? "Occupied" : "Free") << endl;
    }
};

// ---------------- Abstract Base Class User ----------------
class User {
protected:
    string name;
public:
    virtual void interact(Menu& menu, TableManager& tables, double& revenue, int& totalCustomers) = 0;
    virtual ~User() {}
};

// ---------------- Customer Class (inherits User) ----------------
class Customer : public User {
public:
    void interact(Menu& menu, TableManager& tables, double& revenue, int& totalCustomers) override {
        cout << "\nEnter your name: ";
        getline(cin, name);
        totalCustomers++;

        char delivery;
        int tableNum = -1;

        while (true) {
            cout << "Home delivery? (y/n): ";
            cin >> delivery;
            delivery = tolower(delivery);
            if (delivery == 'y' || delivery == 'n') break;
            else cout << "Enter 'y' or 'n' only!\n";
        }
        cin.ignore();

        if (delivery == 'n') {
            tables.showStatus();
            tableNum = getValidatedInt("Choose table (1-10): ", 1, MAX_TABLES);
            if (!tables.reserveTable(tableNum)) {
                cout << "Table is already occupied!\n";
                return;
            }
        }

        menu.displayMenu();
        int items = getValidatedInt("How many items to order? ", 1, 10);

        double total = 0;
        ofstream receipt("orders.txt", ios::app);
        receipt << "Customer: " << name << "\n";

        for (int i = 0; i < items; ++i) {
            int choice = getValidatedInt("Enter item number: ", 1, menu.getCount());
            MenuItem item = menu.getItem(choice - 1);
            total += item.price;
            receipt << "- " << item.name << " - Rs." << fixed << setprecision(2) << item.price << endl;
        }

        cout << "\nTotal bill: Rs." << fixed << setprecision(2) << total << endl;
        cout << "Payment received. Thank you!\n";
        receipt << "Total: Rs." << fixed << setprecision(2) << total << "\n---\n";
        revenue += total;

        string feedback;
        cout << "Leave feedback: ";
        getline(cin, feedback);
        ofstream fout("feedback.txt", ios::app);
        fout << name << ": " << feedback << endl;

        if (tableNum != -1) tables.releaseTable(tableNum);
    }

private:
    int getValidatedInt(string prompt, int min, int max) {
        int value;
        while (true) {
            cout << prompt;
            if (cin >> value && value >= min && value <= max)
                break;
            else {
                cout << "Invalid input! Try again.\n";
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
            }
        }
        cin.ignore();
        return value;
    }
};

// ---------------- Manager Class (inherits User) ----------------
class Manager : public User {
public:
    void interact(Menu& menu, TableManager& tables, double& revenue, int& totalCustomers) override {
        cout << "Enter manager password: ";
        getline(cin, name);
        if (name != "admin123") {
            cout << "Access denied.\n";
            return;
        }

        int choice;
        do {
            cout << "\n--- MANAGER MENU ---\n";
            cout << "1. Show Menu\n2. Show Tables\n3. Show Summary\n4. View Feedback\n5. Exit\nChoice: ";
            cin >> choice;
            cin.ignore();

            switch (choice) {
                case 1: menu.displayMenu(); break;
                case 2: tables.showStatus(); break;
                case 3:
                    cout << "\n--- Summary Report ---\n";
                    cout << "Total Customers: " << totalCustomers << endl;
                    cout << "Total Revenue: Rs." << fixed << setprecision(2) << revenue << endl;
                    break;
                case 4: {
                    ifstream fin("feedback.txt");
                    string line;
                    cout << "\n--- Feedback ---\n";
                    while (getline(fin, line)) {
                        cout << line << endl;
                    }
                    fin.close();
                    break;
                }
                case 5: cout << "Exiting manager mode.\n"; break;
                default: cout << "Invalid choice.\n";
            }
        } while (choice != 5);
    }
};

// ---------------- Main Function ----------------
int main() {
    Menu menu;
    TableManager tables;
    double revenue = 0;
    int totalCustomers = 0;

    int userType;
    do {
        cout << "\n--- Welcome to Restaurant Management System ---\n";
        cout << "1. Customer\n2. Manager\n3. Exit\nChoice: ";
        if (!(cin >> userType)) {
            cout << "Invalid input! Enter a number.\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            continue;
        }
        cin.ignore();

        User* user = nullptr;
        switch (userType) {
            case 1:
                user = new Customer();
                break;
            case 2:
                user = new Manager();
                break;
            case 3:
                cout << "Exiting system. Goodbye!\n";
                break;
            default:
                cout << "Invalid choice.\n";
        }

        if (user) {
            user->interact(menu, tables, revenue, totalCustomers);
            delete user;
        }

    } while (userType != 3);

    return 0;
}
