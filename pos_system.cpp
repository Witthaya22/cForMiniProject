#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>
#include <ctime>
#include <vector>
#include <sstream>

using namespace std;

// โครงสร้างข้อมูลสินค้า
struct Product {
    string id;
    string name;
    string category;
    double price;
    int stock;
    int minStock; // สต็อกขั้นต่ำสำหรับแจ้งเตือน
};

// โครงสร้างข้อมูลผู้ใช้
struct User {
    string username;
    string password;
    string role; // Admin หรือ Cashier
};

// โครงสร้างข้อมูลการขาย
struct Sale {
    string date;
    string time;
    string productId;
    string productName;
    int quantity;
    double totalPrice;
    string cashier;
};

// ตัวแปร Global
User currentUser;
vector<Product> products;
vector<Sale> sales;
vector<User> users;

// ===== ฟังก์ชันช่วยเหลือทั่วไป =====

void clearScreen() {
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}

void pause() {
    cout << "\n\nกด Enter เพื่อดำเนินการต่อ...";
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

// ===== ฟังก์ชันจัดการไฟล์ =====

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
        // สร้างผู้ใช้เริ่มต้น
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

// ===== ระบบล็อกอิน =====

bool login() {
    clearScreen();
    cout << "╔════════════════════════════════════════╗\n";
    cout << "║     ระบบจัดการร้านค้า POS SYSTEM      ║\n";
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
    
    cout << "\n❌ ชื่อผู้ใช้หรือรหัสผ่านไม่ถูกต้อง!\n";
    pause();
    return false;
}

// ===== ระบบจัดการสินค้า (Admin เท่านั้น) =====

void addProduct() {
    clearScreen();
    cout << "╔════════════════════════════════════════╗\n";
    cout << "║          เพิ่มสินค้าใหม่               ║\n";
    cout << "╚════════════════════════════════════════╝\n\n";
    
    Product p;
    cin.ignore();
    
    cout << "รหัสสินค้า: ";
    getline(cin, p.id);
    
    // ตรวจสอบรหัสสินค้าซ้ำ
    for (const auto& prod : products) {
        if (prod.id == p.id) {
            cout << "\n❌ รหัสสินค้านี้มีอยู่แล้ว!\n";
            pause();
            return;
        }
    }
    
    cout << "ชื่อสินค้า: ";
    getline(cin, p.name);
    
    cout << "หมวดหมู่: ";
    getline(cin, p.category);
    
    cout << "ราคา: ";
    cin >> p.price;
    
    cout << "จำนวนสต็อก: ";
    cin >> p.stock;
    
    cout << "สต็อกขั้นต่ำ (แจ้งเตือน): ";
    cin >> p.minStock;
    
    products.push_back(p);
    saveProducts();
    
    cout << "\n✓ เพิ่มสินค้าเรียบร้อยแล้ว!\n";
    pause();
}

void editProduct() {
    clearScreen();
    cout << "╔════════════════════════════════════════╗\n";
    cout << "║          แก้ไขข้อมูลสินค้า             ║\n";
    cout << "╚════════════════════════════════════════╝\n\n";
    
    string id;
    cin.ignore();
    cout << "รหัสสินค้าที่ต้องการแก้ไข: ";
    getline(cin, id);
    
    for (auto& p : products) {
        if (p.id == id) {
            cout << "\nข้อมูลปัจจุบัน:\n";
            cout << "ชื่อ: " << p.name << "\n";
            cout << "หมวดหมู่: " << p.category << "\n";
            cout << "ราคา: " << p.price << " บาท\n";
            cout << "สต็อก: " << p.stock << "\n";
            cout << "สต็อกขั้นต่ำ: " << p.minStock << "\n\n";
            
            cout << "ชื่อสินค้าใหม่: ";
            getline(cin, p.name);
            
            cout << "หมวดหมู่ใหม่: ";
            getline(cin, p.category);
            
            cout << "ราคาใหม่: ";
            cin >> p.price;
            
            cout << "จำนวนสต็อกใหม่: ";
            cin >> p.stock;
            
            cout << "สต็อกขั้นต่ำใหม่: ";
            cin >> p.minStock;
            
            saveProducts();
            cout << "\n✓ แก้ไขข้อมูลเรียบร้อยแล้ว!\n";
            pause();
            return;
        }
    }
    
    cout << "\n❌ ไม่พบรหัสสินค้านี้!\n";
    pause();
}

void deleteProduct() {
    clearScreen();
    cout << "╔════════════════════════════════════════╗\n";
    cout << "║             ลบสินค้า                   ║\n";
    cout << "╚════════════════════════════════════════╝\n\n";
    
    string id;
    cin.ignore();
    cout << "รหัสสินค้าที่ต้องการลบ: ";
    getline(cin, id);
    
    for (size_t i = 0; i < products.size(); i++) {
        if (products[i].id == id) {
            cout << "\nจะลบสินค้า: " << products[i].name << " ใช่หรือไม่? (y/n): ";
            char confirm;
            cin >> confirm;
            
            if (confirm == 'y' || confirm == 'Y') {
                products.erase(products.begin() + i);
                saveProducts();
                cout << "\n✓ ลบสินค้าเรียบร้อยแล้ว!\n";
            } else {
                cout << "\n❌ ยกเลิกการลบ\n";
            }
            pause();
            return;
        }
    }
    
    cout << "\n❌ ไม่พบรหัสสินค้านี้!\n";
    pause();
}

void searchProduct() {
    clearScreen();
    cout << "╔════════════════════════════════════════╗\n";
    cout << "║           ค้นหาสินค้า                  ║\n";
    cout << "╚════════════════════════════════════════╝\n\n";
    
    string keyword;
    cin.ignore();
    cout << "ค้นหา (ชื่อหรือรหัสสินค้า): ";
    getline(cin, keyword);
    
    cout << "\nผลการค้นหา:\n";
    cout << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n";
    cout << left << setw(10) << "รหัส" << setw(20) << "ชื่อสินค้า" 
         << setw(15) << "หมวดหมู่" << setw(10) << "ราคา" << "สต็อก\n";
    cout << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n";
    
    bool found = false;
    for (const auto& p : products) {
        if (p.id.find(keyword) != string::npos || p.name.find(keyword) != string::npos) {
            cout << left << setw(10) << p.id << setw(20) << p.name 
                 << setw(15) << p.category << setw(10) << fixed << setprecision(2) 
                 << p.price << p.stock << "\n";
            found = true;
        }
    }
    
    if (!found) {
        cout << "ไม่พบสินค้าที่ตรงกับคำค้นหา\n";
    }
    
    pause();
}

void showAllProducts() {
    clearScreen();
    cout << "╔════════════════════════════════════════╗\n";
    cout << "║        รายการสินค้าทั้งหมด            ║\n";
    cout << "╚════════════════════════════════════════╝\n\n";
    
    if (products.empty()) {
        cout << "ไม่มีสินค้าในระบบ\n";
        pause();
        return;
    }
    
    cout << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n";
    cout << left << setw(10) << "รหัส" << setw(20) << "ชื่อสินค้า" 
         << setw(15) << "หมวดหมู่" << setw(10) << "ราคา" << "สต็อก\n";
    cout << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n";
    
    for (const auto& p : products) {
        cout << left << setw(10) << p.id << setw(20) << p.name 
             << setw(15) << p.category << setw(10) << fixed << setprecision(2) 
             << p.price << p.stock << "\n";
    }
    
    pause();
}

// ===== ระบบขายสินค้า =====

void sellProduct() {
    clearScreen();
    cout << "╔════════════════════════════════════════╗\n";
    cout << "║             ขายสินค้า                  ║\n";
    cout << "╚════════════════════════════════════════╝\n\n";
    
    string id;
    int quantity;
    
    cin.ignore();
    cout << "รหัสสินค้า: ";
    getline(cin, id);
    
    for (auto& p : products) {
        if (p.id == id) {
            cout << "\nสินค้า: " << p.name << "\n";
            cout << "ราคา: " << fixed << setprecision(2) << p.price << " บาท\n";
            cout << "สต็อกคงเหลือ: " << p.stock << "\n\n";
            
            cout << "จำนวนที่ต้องการขาย: ";
            cin >> quantity;
            
            if (quantity > p.stock) {
                cout << "\n❌ สต็อกสินค้าไม่เพียงพอ! (คงเหลือ " << p.stock << " ชิ้น)\n";
                pause();
                return;
            }
            
            if (quantity <= 0) {
                cout << "\n❌ จำนวนไม่ถูกต้อง!\n";
                pause();
                return;
            }
            
            double total = p.price * quantity;
            
            cout << "\n━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n";
            cout << "ยอดชำระทั้งหมด: " << fixed << setprecision(2) << total << " บาท\n";
            cout << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n";
            cout << "\nยืนยันการขาย? (y/n): ";
            char confirm;
            cin >> confirm;
            
            if (confirm == 'y' || confirm == 'Y') {
                // ลดสต็อก
                p.stock -= quantity;
                saveProducts();
                
                // บันทึกการขาย
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
                
                cout << "\n✓ ขายสินค้าเรียบร้อยแล้ว!\n";
                cout << "สต็อกคงเหลือ: " << p.stock << " ชิ้น\n";
                
                if (p.stock <= p.minStock) {
                    cout << "\n⚠️  เตือน: สินค้าใกล้หมด!\n";
                }
            } else {
                cout << "\n❌ ยกเลิกการขาย\n";
            }
            
            pause();
            return;
        }
    }
    
    cout << "\n❌ ไม่พบรหัสสินค้านี้!\n";
    pause();
}

// ===== ระบบรายงาน =====

void reportStockRemaining() {
    clearScreen();
    cout << "╔════════════════════════════════════════╗\n";
    cout << "║        รายงานสินค้าคงเหลือ            ║\n";
    cout << "╚════════════════════════════════════════╝\n\n";
    
    cout << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n";
    cout << left << setw(10) << "รหัส" << setw(20) << "ชื่อสินค้า" 
         << setw(15) << "หมวดหมู่" << setw(10) << "ราคา" << "สต็อก\n";
    cout << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n";
    
    for (const auto& p : products) {
        cout << left << setw(10) << p.id << setw(20) << p.name 
             << setw(15) << p.category << setw(10) << fixed << setprecision(2) 
             << p.price << p.stock;
        
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
    cout << "║        รายงานสินค้าใกล้หมด            ║\n";
    cout << "╚════════════════════════════════════════╝\n\n";
    
    cout << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n";
    cout << left << setw(10) << "รหัส" << setw(20) << "ชื่อสินค้า" 
         << setw(15) << "หมวดหมู่" << setw(10) << "ราคา" << "สต็อก\n";
    cout << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n";
    
    bool found = false;
    for (const auto& p : products) {
        if (p.stock <= p.minStock) {
            cout << left << setw(10) << p.id << setw(20) << p.name 
                 << setw(15) << p.category << setw(10) << fixed << setprecision(2) 
                 << p.price << p.stock << " ⚠️\n";
            found = true;
        }
    }
    
    if (!found) {
        cout << "ไม่มีสินค้าที่ใกล้หมด\n";
    }
    
    pause();
}

void reportDailySales() {
    clearScreen();
    cout << "╔════════════════════════════════════════╗\n";
    cout << "║         รายงานยอดขายรายวัน            ║\n";
    cout << "╚════════════════════════════════════════╝\n\n";
    
    string date;
    cin.ignore();
    cout << "วันที่ (dd/mm/yyyy): ";
    getline(cin, date);
    
    cout << "\n━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n";
    cout << left << setw(12) << "เวลา" << setw(12) << "รหัส" << setw(20) << "สินค้า" 
         << setw(8) << "จำนวน" << setw(12) << "ยอดรวม" << "พนักงาน\n";
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
        cout << "ไม่มีรายการขายในวันที่ระบุ\n";
    } else {
        cout << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n";
        cout << "ยอดขายรวมวันนี้: " << fixed << setprecision(2) << totalSales << " บาท\n";
    }
    
    pause();
}

void reportMonthlySales() {
    clearScreen();
    cout << "╔════════════════════════════════════════╗\n";
    cout << "║        รายงานยอดขายรายเดือน           ║\n";
    cout << "╚════════════════════════════════════════╝\n\n";
    
    string month;
    cin.ignore();
    cout << "เดือน/ปี (mm/yyyy): ";
    getline(cin, month);
    
    cout << "\n━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n";
    cout << left << setw(12) << "วันที่" << setw(12) << "เวลา" << setw(20) << "สินค้า" 
         << setw(8) << "จำนวน" << setw(12) << "ยอดรวม" << "พนักงาน\n";
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
        cout << "ไม่มีรายการขายในเดือนที่ระบุ\n";
    } else {
        cout << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n";
        cout << "ยอดขายรวมเดือนนี้: " << fixed << setprecision(2) << totalSales << " บาท\n";
    }
    
    pause();
}

void reportTotalSales() {
    clearScreen();
    cout << "╔════════════════════════════════════════╗\n";
    cout << "║        ยอดขายรวมทั้งหมด               ║\n";
    cout << "╚════════════════════════════════════════╝\n\n";
    
    double totalSales = 0;
    int totalTransactions = 0;
    
    for (const auto& s : sales) {
        totalSales += s.totalPrice;
        totalTransactions++;
    }
    
    cout << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n";
    cout << "จำนวนรายการขายทั้งหมด: " << totalTransactions << " รายการ\n";
    cout << "ยอดขายรวมทั้งหมด: " << fixed << setprecision(2) << totalSales << " บาท\n";
    cout << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n";
    
    pause();
}

// ===== ระบบค้นหาข้อมูลอย่างละเอียด =====

void advancedSearch() {
    clearScreen();
    cout << "╔════════════════════════════════════════╗\n";
    cout << "║      ค้นหาข้อมูลอย่างละเอียด           ║\n";
    cout << "╚════════════════════════════════════════╝\n\n";
    
    cout << "1. ค้นหาตามชื่อสินค้า\n";
    cout << "2. ค้นหาตามหมวดหมู่\n";
    cout << "3. ค้นหาตามช่วงราคา\n";
    cout << "4. ค้นหาสินค้าใกล้หมด\n";
    cout << "\nเลือกประเภทการค้นหา: ";
    
    int choice;
    cin >> choice;
    cin.ignore();
    
    vector<Product> results;
    
    if (choice == 1) {
        string name;
        cout << "ชื่อสินค้า: ";
        getline(cin, name);
        
        for (const auto& p : products) {
            if (p.name.find(name) != string::npos) {
                results.push_back(p);
            }
        }
    }
    else if (choice == 2) {
        string category;
        cout << "หมวดหมู่: ";
        getline(cin, category);
        
        for (const auto& p : products) {
            if (p.category.find(category) != string::npos) {
                results.push_back(p);
            }
        }
    }
    else if (choice == 3) {
        double minPrice, maxPrice;
        cout << "ราคาต่ำสุด: ";
        cin >> minPrice;
        cout << "ราคาสูงสุด: ";
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
        cout << "\n❌ ตัวเลือกไม่ถูกต้อง!\n";
        pause();
        return;
    }
    
    cout << "\n━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n";
    cout << "ผลการค้นหา: " << results.size() << " รายการ\n";
    cout << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n";
    
    if (results.empty()) {
        cout << "ไม่พบสินค้าที่ตรงกับเงื่อนไข\n";
    } else {
        cout << left << setw(10) << "รหัส" << setw(20) << "ชื่อสินค้า" 
             << setw(15) << "หมวดหมู่" << setw(10) << "ราคา" << "สต็อก\n";
        cout << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n";
        
        for (const auto& p : results) {
            cout << left << setw(10) << p.id << setw(20) << p.name 
                 << setw(15) << p.category << setw(10) << fixed << setprecision(2) 
                 << p.price << p.stock;
            
            if (p.stock <= p.minStock) {
                cout << " ⚠️ ";
            }
            cout << "\n";
        }
    }
    
    pause();
}

// ===== เมนูสำหรับ Admin =====

void adminMenu() {
    while (true) {
        clearScreen();
        cout << "╔════════════════════════════════════════╗\n";
        cout << "║          เมนู ADMIN                    ║\n";
        cout << "║  ผู้ใช้: " << left << setw(28) << currentUser.username << "║\n";
        cout << "╚════════════════════════════════════════╝\n\n";
        
        cout << "【 จัดการสินค้า 】\n";
        cout << "  1. เพิ่มสินค้าใหม่\n";
        cout << "  2. แก้ไขข้อมูลสินค้า\n";
        cout << "  3. ลบสินค้า\n";
        cout << "  4. ค้นหาสินค้า\n";
        cout << "  5. แสดงสินค้าทั้งหมด\n\n";
        
        cout << "【 ขายสินค้า 】\n";
        cout << "  6. ขายสินค้า\n\n";
        
        cout << "【 รายงาน 】\n";
        cout << "  7. รายงานสินค้าคงเหลือ\n";
        cout << "  8. รายงานสินค้าใกล้หมด\n";
        cout << "  9. รายงานยอดขายรายวัน\n";
        cout << "  10. รายงานยอดขายรายเดือน\n";
        cout << "  11. ยอดขายรวมทั้งหมด\n\n";
        
        cout << "【 ค้นหาข้อมูล 】\n";
        cout << "  12. ค้นหาข้อมูลอย่างละเอียด\n\n";
        
        cout << "  0. ออกจากระบบ\n\n";
        cout << "เลือกเมนู: ";
        
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
                cout << "\n❌ ตัวเลือกไม่ถูกต้อง!\n";
                pause();
        }
    }
}

// ===== เมนูสำหรับ Cashier =====

void cashierMenu() {
    while (true) {
        clearScreen();
        cout << "╔════════════════════════════════════════╗\n";
        cout << "║          เมนู CASHIER                  ║\n";
        cout << "║  ผู้ใช้: " << left << setw(28) << currentUser.username << "║\n";
        cout << "╚════════════════════════════════════════╝\n\n";
        
        cout << "【 ขายสินค้า 】\n";
        cout << "  1. ขายสินค้า\n";
        cout << "  2. แสดงสินค้าทั้งหมด\n";
        cout << "  3. ค้นหาสินค้า\n\n";
        
        cout << "【 รายงาน 】\n";
        cout << "  4. รายงานสินค้าคงเหลือ\n";
        cout << "  5. รายงานสินค้าใกล้หมด\n";
        cout << "  6. รายงานยอดขายรายวัน\n";
        cout << "  7. รายงานยอดขายรายเดือน\n";
        cout << "  8. ยอดขายรวมทั้งหมด\n\n";
        
        cout << "【 ค้นหาข้อมูล 】\n";
        cout << "  9. ค้นหาข้อมูลอย่างละเอียด\n\n";
        
        cout << "  0. ออกจากระบบ\n\n";
        cout << "เลือกเมนู: ";
        
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
                cout << "\n❌ ตัวเลือกไม่ถูกต้อง!\n";
                pause();
        }
    }
}

// ===== ฟังก์ชันหลัก =====

int main() {
    // โหลดข้อมูลทั้งหมด
    loadUsers();
    loadProducts();
    loadSales();
    
    // ล็อกอิน
    while (true) {
        if (login()) {
            clearScreen();
            cout << "\n✓ เข้าสู่ระบบสำเร็จ!\n";
            cout << "ยินดีต้อนรับ " << currentUser.username << " (" << currentUser.role << ")\n";
            pause();
            
            // เข้าเมนูตามสิทธิ์
            if (currentUser.role == "Admin") {
                adminMenu();
            } else if (currentUser.role == "Cashier") {
                cashierMenu();
            }
            
            cout << "\nออกจากระบบแล้ว\n";
            pause();
        }
    }
    
    return 0;
}