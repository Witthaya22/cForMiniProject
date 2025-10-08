#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>
#include <ctime>
#include <vector>
#include <sstream>

using namespace std;

// Product Data Structure
struct Product {
    string id;
    string name;
    string category;
    double price;
    int stock;
    int minStock; // Minimum stock for alert
};

// User Data Structure
struct User {
    string username;
    string password;
    string role; // Admin or Cashier
};

// Sale Data Structure
struct Sale {
    string date;
    string time;
    string productId;
    string productName;
    int quantity;
    double totalPrice;
    string cashier;
};

// Global Variables
User currentUser;
vector<Product> products;
vector<Sale> sales;
vector<User> users;

// ===== Helper Functions =====

void clearScreen() {
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}

void pause() {
    cout << "\n\nPress Enter to continue...";
    cin.ignore();
    cin.get();
}

string getCurrentDate() {
    time_t now = time(0);
    tm *ltm = localtime(&now);
    char buffer[11];
    sprintf(buffer, "%02d/%02d/%04d", ltm->tm_mday, 1 + ltm->tm_mon, 1900 + ltm->tm_year);
    return string(buffer);
}

string getCurrentTime() {
    time_t now = time(0);
    tm *ltm = localtime(&now);
    char buffer[9];
    sprintf(buffer, "%02d:%02d:%02d", ltm->tm_hour, ltm->tm_min, ltm->tm_sec);
    return string(buffer);
}

// ===== File Management Functions =====

void loadUsers() {
    users.clear();
    ifstream file("users.txt");
    if (file.is_open()) {
        string line;
        while (getline(file, line)) {
            stringstream ss(line);
            User u;
            getline(ss, u.username, '|');
            getline(ss, u.password, '|');
            getline(ss, u.role, '|');
            users.push_back(u);
        }
        file.close();
    } else {
        // Create default users
        User admin = {"admin", "admin123", "Admin"};
        User cashier = {"cashier", "cash123", "Cashier"};
        users.push_back(admin);
        users.push_back(cashier);
        
        ofstream outFile("users.txt");
        outFile << "admin|admin123|Admin\n";
        outFile << "cashier|cash123|Cashier\n";
        outFile.close();
    }
}

void loadProducts() {
    products.clear();
    ifstream file("products.txt");
    if (file.is_open()) {
        string line;
        while (getline(file, line)) {
            stringstream ss(line);
            Product p;
            getline(ss, p.id, '|');
            getline(ss, p.name, '|');
            getline(ss, p.category, '|');
            string priceStr, stockStr, minStockStr;
            getline(ss, priceStr, '|');
            getline(ss, stockStr, '|');
            getline(ss, minStockStr, '|');
            p.price = stod(priceStr);
            p.stock = stoi(stockStr);
            p.minStock = stoi(minStockStr);
            products.push_back(p);
        }
        file.close();
    }
}

void saveProducts() {
    ofstream file("products.txt");
    for (const auto& p : products) {
        file << p.id << "|" << p.name << "|" << p.category << "|" 
             << p.price << "|" << p.stock << "|" << p.minStock << "\n";
    }
    file.close();
}

void loadSales() {
    sales.clear();
    ifstream file("sales.txt");
    if (file.is_open()) {
        string line;
        while (getline(file, line)) {
            stringstream ss(line);
            Sale s;
            getline(ss, s.date, '|');
            getline(ss, s.time, '|');
            getline(ss, s.productId, '|');
            getline(ss, s.productName, '|');
            string qtyStr, priceStr;
            getline(ss, qtyStr, '|');
            getline(ss, priceStr, '|');
            getline(ss, s.cashier, '|');
            s.quantity = stoi(qtyStr);
            s.totalPrice = stod(priceStr);
            sales.push_back(s);
        }
        file.close();
    }
}

void saveSale(const Sale& sale) {
    ofstream file("sales.txt", ios::app);
    file << sale.date << "|" << sale.time << "|" << sale.productId << "|" 
         << sale.productName << "|" << sale.quantity << "|" 
         << sale.totalPrice << "|" << sale.cashier << "\n";
    file.close();
}

// ===== Login System =====

bool login() {
    clearScreen();
    cout << "╔════════════════════════════════════════╗\n";
    cout << "║        POS SYSTEM - MANAGEMENT         ║\n";
    cout << "╚════════════════════════════════════════╝\n\n";
    
    string username, password;
    cout << "Username: ";
    cin >> username;
    cout << "Password: ";
    cin >> password;
    
    for (const auto& user : users) {
        if (user.username == username && user.password == password) {
            currentUser = user;
            return true;
        }
    }
    
    cout << "\n❌ Invalid username or password!\n";
    pause();
    return false;
}

// ===== Product Management (Admin Only) =====

void addProduct() {
    clearScreen();
    cout << "╔════════════════════════════════════════╗\n";
    cout << "║            ADD NEW PRODUCT             ║\n";
    cout << "╚════════════════════════════════════════╝\n\n";
    
    Product p;
    cin.ignore();
    
    cout << "Product ID: ";
    getline(cin, p.id);
    
    // Check duplicate ID
    for (const auto& prod : products) {
        if (prod.id == p.id) {
            cout << "\n❌ This product ID already exists!\n";
            pause();
            return;
        }
    }
    
    cout << "Product Name: ";
    getline(cin, p.name);
    
    cout << "Category: ";
    getline(cin, p.category);
    
    cout << "Price: ";
    cin >> p.price;
    
    cout << "Stock Quantity: ";
    cin >> p.stock;
    
    cout << "Minimum Stock (Alert): ";
    cin >> p.minStock;
    
    products.push_back(p);
    saveProducts();
    
    cout << "\n✓ Product added successfully!\n";
    pause();
}

void editProduct() {
    clearScreen();
    cout << "╔════════════════════════════════════════╗\n";
    cout << "║            EDIT PRODUCT                ║\n";
    cout << "╚════════════════════════════════════════╝\n\n";
    
    // Show all products first
    if (products.empty()) {
        cout << "❌ No products in system\n";
        pause();
        return;
    }
    
    cout << "All Products:\n";
    cout << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n";
    cout << left << setw(12) << "ID" << setw(25) << "Product Name" 
         << setw(18) << "Category" << right << setw(12) << "Price" << setw(10) << "Stock" << "\n";
    cout << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n";
    
    for (const auto& p : products) {
        cout << left << setw(12) << p.id << setw(25) << p.name 
             << setw(18) << p.category << right << setw(12) << fixed << setprecision(2) 
             << p.price << setw(10) << p.stock << "\n";
    }
    cout << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n\n";
    
    string id;
    cin.ignore();
    cout << "Product ID to edit (type 0 to cancel): ";
    getline(cin, id);
    
    if (id == "0" || id == "cancel") {
        cout << "\n❌ Edit cancelled\n";
        pause();
        return;
    }
    
    for (auto& p : products) {
        if (p.id == id) {
            cout << "\nCurrent Information:\n";
            cout << "Name: " << p.name << "\n";
            cout << "Category: " << p.category << "\n";
            cout << "Price: " << p.price << " $\n";
            cout << "Stock: " << p.stock << "\n";
            cout << "Min Stock: " << p.minStock << "\n\n";
            
            cout << "New Product Name: ";
            getline(cin, p.name);
            
            cout << "New Category: ";
            getline(cin, p.category);
            
            cout << "New Price: ";
            cin >> p.price;
            
            cout << "New Stock Quantity: ";
            cin >> p.stock;
            
            cout << "New Min Stock: ";
            cin >> p.minStock;
            
            saveProducts();
            cout << "\n✓ Product updated successfully!\n";
            pause();
            return;
        }
    }
    
    cout << "\n❌ Product ID not found!\n";
    pause();
}

void deleteProduct() {
    clearScreen();
    cout << "╔════════════════════════════════════════╗\n";
    cout << "║           DELETE PRODUCT               ║\n";
    cout << "╚════════════════════════════════════════╝\n\n";
    
    // Show all products first
    if (products.empty()) {
        cout << "❌ No products in system\n";
        pause();
        return;
    }
    
    cout << "All Products:\n";
    cout << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n";
    cout << left << setw(12) << "ID" << setw(25) << "Product Name" 
         << setw(18) << "Category" << right << setw(12) << "Price" << setw(10) << "Stock" << "\n";
    cout << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n";
    
    for (const auto& p : products) {
        cout << left << setw(12) << p.id << setw(25) << p.name 
             << setw(18) << p.category << right << setw(12) << fixed << setprecision(2) 
             << p.price << setw(10) << p.stock << "\n";
    }
    cout << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n\n";
    
    string id;
    cin.ignore();
    cout << "Product ID to delete (type 0 to cancel): ";
    getline(cin, id);
    
    if (id == "0" || id == "cancel") {
        cout << "\n❌ Delete cancelled\n";
        pause();
        return;
    }
    
    for (size_t i = 0; i < products.size(); i++) {
        if (products[i].id == id) {
            cout << "\nDelete product: " << products[i].name << "? (y/n): ";
            char confirm;
            cin >> confirm;
            
            if (confirm == 'y' || confirm == 'Y') {
                products.erase(products.begin() + i);
                saveProducts();
                cout << "\n✓ Product deleted successfully!\n";
            } else {
                cout << "\n❌ Delete cancelled\n";
            }
            pause();
            return;
        }
    }
    
    cout << "\n❌ Product ID not found!\n";
    pause();
}

void searchProduct() {
    clearScreen();
    cout << "╔════════════════════════════════════════╗\n";
    cout << "║          SEARCH PRODUCT                ║\n";
    cout << "╚════════════════════════════════════════╝\n\n";
    
    string keyword;
    cin.ignore();
    cout << "Search (Name or ID, type 0 to cancel): ";
    getline(cin, keyword);
    
    if (keyword == "0" || keyword == "cancel") {
        return;
    }
    
    cout << "\nSearch Results:\n";
    cout << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n";
    cout << left << setw(12) << "ID" << setw(25) << "Product Name" 
         << setw(18) << "Category" << right << setw(12) << "Price" << setw(10) << "Stock" << "\n";
    cout << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n";
    
    bool found = false;
    for (const auto& p : products) {
        if (p.id.find(keyword) != string::npos || p.name.find(keyword) != string::npos) {
            cout << left << setw(12) << p.id << setw(25) << p.name 
                 << setw(18) << p.category << right << setw(12) << fixed << setprecision(2) 
                 << p.price << setw(10) << p.stock << "\n";
            found = true;
        }
    }
    
    if (!found) {
        cout << "No products found matching search\n";
    }
    
    pause();
}

void showAllProducts() {
    clearScreen();
    cout << "╔════════════════════════════════════════╗\n";
    cout << "║          ALL PRODUCTS                  ║\n";
    cout << "╚════════════════════════════════════════╝\n\n";
    
    if (products.empty()) {
        cout << "No products in system\n";
        pause();
        return;
    }
    
    cout << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n";
    cout << left << setw(12) << "ID" << setw(25) << "Product Name" 
         << setw(18) << "Category" << right << setw(12) << "Price" << setw(10) << "Stock" << "\n";
    cout << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n";
    
    for (const auto& p : products) {
        cout << left << setw(12) << p.id << setw(25) << p.name 
             << setw(18) << p.category << right << setw(12) << fixed << setprecision(2) 
             << p.price << setw(10) << p.stock << "\n";
    }
    
    pause();
}

// ===== Sales System =====

void sellProduct() {
    clearScreen();
    cout << "╔════════════════════════════════════════╗\n";
    cout << "║            SELL PRODUCT                ║\n";
    cout << "╚════════════════════════════════════════╝\n\n";
    
    // Show available products first
    cout << "Available Products:\n";
    cout << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n";
    cout << left << setw(12) << "ID" << setw(25) << "Product Name" 
         << setw(18) << "Category" << right << setw(12) << "Price" << setw(10) << "Stock" << "\n";
    cout << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n";
    
    bool hasStock = false;
    for (const auto& p : products) {
        if (p.stock > 0) {
            cout << left << setw(12) << p.id << setw(25) << p.name 
                 << setw(18) << p.category << right << setw(12) << fixed << setprecision(2) 
                 << p.price << setw(10) << p.stock;
            if (p.stock <= p.minStock) {
                cout << " ⚠️";
            }
            cout << "\n";
            hasStock = true;
        }
    }
    
    if (!hasStock) {
        cout << "No products available for sale\n";
        pause();
        return;
    }
    cout << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n\n";
    
    string id;
    int quantity;
    
    cin.ignore();
    cout << "Product ID (type 0 to cancel): ";
    getline(cin, id);
    
    if (id == "0" || id == "cancel") {
        cout << "\n❌ Sale cancelled\n";
        pause();
        return;
    }
    
    for (auto& p : products) {
        if (p.id == id) {
            cout << "\nProduct: " << p.name << "\n";
            cout << "Price: " << fixed << setprecision(2) << p.price << " $\n";
            cout << "Stock Available: " << p.stock << "\n\n";
            
            cout << "Quantity to sell: ";
            cin >> quantity;
            
            if (quantity > p.stock) {
                cout << "\n❌ Insufficient stock! (Available: " << p.stock << " units)\n";
                pause();
                return;
            }
            
            if (quantity <= 0) {
                cout << "\n❌ Invalid quantity!\n";
                pause();
                return;
            }
            
            double total = p.price * quantity;
            
            cout << "\n━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n";
            cout << "Total Amount: " << fixed << setprecision(2) << total << " $\n";
            cout << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n";
            cout << "\nConfirm sale? (y/n): ";
            char confirm;
            cin >> confirm;
            
            if (confirm == 'y' || confirm == 'Y') {
                // Reduce stock
                p.stock -= quantity;
                saveProducts();
                
                // Record sale
                Sale sale;
                sale.date = getCurrentDate();
                sale.time = getCurrentTime();
                sale.productId = p.id;
                sale.productName = p.name;
                sale.quantity = quantity;
                sale.totalPrice = total;
                sale.cashier = currentUser.username;
                
                saveSale(sale);
                sales.push_back(sale);
                
                cout << "\n✓ Sale completed successfully!\n";
                cout << "Remaining Stock: " << p.stock << " units\n";
                
                if (p.stock <= p.minStock) {
                    cout << "\n⚠️  Warning: Low stock alert!\n";
                }
            } else {
                cout << "\n❌ Sale cancelled\n";
            }
            
            pause();
            return;
        }
    }
    
    cout << "\n❌ Product ID not found!\n";
    pause();
}

// ===== Report System =====

void reportStockRemaining() {
    clearScreen();
    cout << "╔════════════════════════════════════════╗\n";
    cout << "║        STOCK REMAINING REPORT          ║\n";
    cout << "╚════════════════════════════════════════╝\n\n";
    
    cout << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n";
    cout << left << setw(12) << "ID" << setw(25) << "Product Name" 
         << setw(18) << "Category" << right << setw(12) << "Price" << setw(10) << "Stock" << "\n";
    cout << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n";
    
    for (const auto& p : products) {
        cout << left << setw(12) << p.id << setw(25) << p.name 
             << setw(18) << p.category << right << setw(12) << fixed << setprecision(2) 
             << p.price << setw(10) << p.stock;
        
        if (p.stock <= p.minStock) {
            cout << " ⚠️ ";
        }
        cout << "\n";
    }
    
    pause();
}

void reportLowStock() {
    clearScreen();
    cout << "╔════════════════════════════════════════╗\n";
    cout << "║          LOW STOCK REPORT              ║\n";
    cout << "╚════════════════════════════════════════╝\n\n";
    
    cout << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n";
    cout << left << setw(12) << "ID" << setw(25) << "Product Name" 
         << setw(18) << "Category" << right << setw(12) << "Price" << setw(10) << "Stock" << "\n";
    cout << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n";
    
    bool found = false;
    for (const auto& p : products) {
        if (p.stock <= p.minStock) {
            cout << left << setw(12) << p.id << setw(25) << p.name 
                 << setw(18) << p.category << right << setw(12) << fixed << setprecision(2) 
                 << p.price << setw(10) << p.stock << " ⚠️\n";
            found = true;
        }
    }
    
    if (!found) {
        cout << "No low stock products\n";
    }
    
    pause();
}

void reportDailySales() {
    clearScreen();
    cout << "╔════════════════════════════════════════╗\n";
    cout << "║         DAILY SALES REPORT             ║\n";
    cout << "╚════════════════════════════════════════╝\n\n";
    
    string date;
    cin.ignore();
    cout << "Date (dd/mm/yyyy, type 0 to cancel): ";
    getline(cin, date);
    
    if (date == "0" || date == "cancel") {
        return;
    }
    
    cout << "\n━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n";
    cout << left << setw(12) << "Time" << setw(12) << "ID" << setw(20) << "Product" 
         << setw(8) << "Qty" << setw(12) << "Total" << "Cashier\n";
    cout << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n";
    
    double totalSales = 0;
    bool found = false;
    
    for (const auto& s : sales) {
        if (s.date == date) {
            cout << left << setw(12) << s.time << setw(12) << s.productId 
                 << setw(20) << s.productName << setw(8) << s.quantity 
                 << setw(12) << fixed << setprecision(2) << s.totalPrice 
                 << s.cashier << "\n";
            totalSales += s.totalPrice;
            found = true;
        }
    }
    
    if (!found) {
        cout << "No sales for specified date\n";
    } else {
        cout << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n";
        cout << "Daily Total Sales: " << fixed << setprecision(2) << totalSales << " $\n";
    }
    
    pause();
}

void reportMonthlySales() {
    clearScreen();
    cout << "╔════════════════════════════════════════╗\n";
    cout << "║        MONTHLY SALES REPORT            ║\n";
    cout << "╚════════════════════════════════════════╝\n\n";
    
    string month;
    cin.ignore();
    cout << "Month/Year (mm/yyyy, type 0 to cancel): ";
    getline(cin, month);
    
    if (month == "0" || month == "cancel") {
        return;
    }
    
    cout << "\n━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n";
    cout << left << setw(12) << "Date" << setw(12) << "Time" << setw(20) << "Product" 
         << setw(8) << "Qty" << setw(12) << "Total" << "Cashier\n";
    cout << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n";
    
    double totalSales = 0;
    bool found = false;
    
    for (const auto& s : sales) {
        if (s.date.substr(3) == month) {
            cout << left << setw(12) << s.date << setw(12) << s.time 
                 << setw(20) << s.productName << setw(8) << s.quantity 
                 << setw(12) << fixed << setprecision(2) << s.totalPrice 
                 << s.cashier << "\n";
            totalSales += s.totalPrice;
            found = true;
        }
    }
    
    if (!found) {
        cout << "No sales for specified month\n";
    } else {
        cout << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n";
        cout << "Monthly Total Sales: " << fixed << setprecision(2) << totalSales << " $\n";
    }
    
    pause();
}

void reportTotalSales() {
    clearScreen();
    cout << "╔════════════════════════════════════════╗\n";
    cout << "║          TOTAL SALES REPORT            ║\n";
    cout << "╚════════════════════════════════════════╝\n\n";
    
    double totalSales = 0;
    int totalTransactions = 0;
    
    for (const auto& s : sales) {
        totalSales += s.totalPrice;
        totalTransactions++;
    }
    
    cout << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n";
    cout << "Total Transactions: " << totalTransactions << " sales\n";
    cout << "Total Sales Amount: " << fixed << setprecision(2) << totalSales << " $\n";
    cout << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n";
    
    pause();
}

// ===== Advanced Search System =====

void advancedSearch() {
    clearScreen();
    cout << "╔════════════════════════════════════════╗\n";
    cout << "║          ADVANCED SEARCH               ║\n";
    cout << "╚════════════════════════════════════════╝\n\n";
    
    cout << "1. Search by Product Name\n";
    cout << "2. Search by Category\n";
    cout << "3. Search by Price Range\n";
    cout << "4. Search Low Stock Products\n";
    cout << "0. Cancel\n";
    cout << "\nSelect search type: ";
    
    int choice;
    cin >> choice;
    cin.ignore();
    
    if (choice == 0) {
        return;
    }
    
    vector<Product> results;
    
    if (choice == 1) {
        string name;
        cout << "Product Name: ";
        getline(cin, name);
        
        for (const auto& p : products) {
            if (p.name.find(name) != string::npos) {
                results.push_back(p);
            }
        }
    }
    else if (choice == 2) {
        string category;
        cout << "Category: ";
        getline(cin, category);
        
        for (const auto& p : products) {
            if (p.category.find(category) != string::npos) {
                results.push_back(p);
            }
        }
    }
    else if (choice == 3) {
        double minPrice, maxPrice;
        cout << "Minimum Price: ";
        cin >> minPrice;
        cout << "Maximum Price: ";
        cin >> maxPrice;
        
        for (const auto& p : products) {
            if (p.price >= minPrice && p.price <= maxPrice) {
                results.push_back(p);
            }
        }
    }
    else if (choice == 4) {
        for (const auto& p : products) {
            if (p.stock <= p.minStock) {
                results.push_back(p);
            }
        }
    }
    else {
        cout << "\n❌ Invalid option!\n";
        pause();
        return;
    }
    
    cout << "\n━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n";
    cout << "Search Results: " << results.size() << " items\n";
    cout << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n";
    
    if (results.empty()) {
        cout << "No products match the criteria\n";
    } else {
        cout << left << setw(12) << "ID" << setw(25) << "Product Name" 
             << setw(18) << "Category" << right << setw(12) << "Price" << setw(10) << "Stock" << "\n";
        cout << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n";
        
        for (const auto& p : results) {
            cout << left << setw(12) << p.id << setw(25) << p.name 
                 << setw(18) << p.category << right << setw(12) << fixed << setprecision(2) 
                 << p.price << setw(10) << p.stock;
            
            if (p.stock <= p.minStock) {
                cout << " ⚠️ ";
            }
            cout << "\n";
        }
    }
    
    pause();
}

// ===== Admin Menu =====

void adminMenu() {
    while (true) {
        clearScreen();
        cout << "╔════════════════════════════════════════╗\n";
        cout << "║            ADMIN MENU                  ║\n";
        cout << "║  User: " << left << setw(28) << currentUser.username << "║\n";
        cout << "╚════════════════════════════════════════╝\n\n";
        
        cout << "【 Product Management 】\n";
        cout << "  1. Add New Product\n";
        cout << "  2. Edit Product\n";
        cout << "  3. Delete Product\n";
        cout << "  4. Search Product\n";
        cout << "  5. Show All Products\n\n";
        
        cout << "【 Sales 】\n";
        cout << "  6. Sell Product\n\n";
        
        cout << "【 Reports 】\n";
        cout << "  7. Stock Remaining Report\n";
        cout << "  8. Low Stock Report\n";
        cout << "  9. Daily Sales Report\n";
        cout << "  10. Monthly Sales Report\n";
        cout << "  11. Total Sales Report\n\n";
        
        cout << "【 Search 】\n";
        cout << "  12. Advanced Search\n\n";
        
        cout << "  0. Logout\n\n";
        cout << "Select menu: ";
        
        int choice;
        cin >> choice;
        
        switch (choice) {
            case 1: addProduct(); break;
            case 2: editProduct(); break;
            case 3: deleteProduct(); break;
            case 4: searchProduct(); break;
            case 5: showAllProducts(); break;
            case 6: sellProduct(); break;
            case 7: reportStockRemaining(); break;
            case 8: reportLowStock(); break;
            case 9: reportDailySales(); break;
            case 10: reportMonthlySales(); break;
            case 11: reportTotalSales(); break;
            case 12: advancedSearch(); break;
            case 0: return;
            default:
                cout << "\n❌ Invalid option!\n";
                pause();
        }
    }
}

// ===== Cashier Menu =====

void cashierMenu() {
    while (true) {
        clearScreen();
        cout << "╔════════════════════════════════════════╗\n";
        cout << "║           CASHIER MENU                 ║\n";
        cout << "║  User: " << left << setw(28) << currentUser.username << "║\n";
        cout << "╚════════════════════════════════════════╝\n\n";
        
        cout << "【 Sales 】\n";
        cout << "  1. Sell Product\n";
        cout << "  2. Show All Products\n";
        cout << "  3. Search Product\n\n";
        
        cout << "【 Reports 】\n";
        cout << "  4. Stock Remaining Report\n";
        cout << "  5. Low Stock Report\n";
        cout << "  6. Daily Sales Report\n";
        cout << "  7. Monthly Sales Report\n";
        cout << "  8. Total Sales Report\n\n";
        
        cout << "【 Search 】\n";
        cout << "  9. Advanced Search\n\n";
        
        cout << "  0. Logout\n\n";
        cout << "Select menu: ";
        
        int choice;
        cin >> choice;
        
        switch (choice) {
            case 1: sellProduct(); break;
            case 2: showAllProducts(); break;
            case 3: searchProduct(); break;
            case 4: reportStockRemaining(); break;
            case 5: reportLowStock(); break;
            case 6: reportDailySales(); break;
            case 7: reportMonthlySales(); break;
            case 8: reportTotalSales(); break;
            case 9: advancedSearch(); break;
            case 0: return;
            default:
                cout << "\n❌ Invalid option!\n";
                pause();
        }
    }
}

// ===== Main Function =====

int main() {
    // Load all data
    loadUsers();
    loadProducts();
    loadSales();
    
    // Login
    while (true) {
        if (login()) {
            clearScreen();
            cout << "\n✓ Login successful!\n";
            cout << "Welcome " << currentUser.username << " (" << currentUser.role << ")\n";
            pause();
            
            // Enter menu based on role
            if (currentUser.role == "Admin") {
                adminMenu();
            } else if (currentUser.role == "Cashier") {
                cashierMenu();
            }
            
            cout << "\nLogged out successfully\n";
            pause();
        }
    }
    
    return 0;
}