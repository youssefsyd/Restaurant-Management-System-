#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include <ctime>
#include <sstream>
#include <cctype>
#include <regex>
#include <algorithm>


using namespace std;

// ============================================================================
// SHARED CODE — copy exactly, do not modify
// ============================================================================
enum class OrderType { DineIn, Takeaway, Delivery };
enum class OrderStatus { New, Preparing, Ready, Served, Completed, Cancelled };
enum class TableStatus { Available, Reserved, Occupied, OutOfService };
enum class ReservationStatus { Pending, Confirmed, Completed, Cancelled };
enum class PaymentMethod { Cash, Card, MobilePayment };
enum class PaymentStatus { Completed, Refunded };
enum class DeliveryStatus { Assigned, OnTheWay, Delivered, Cancelled };
enum class EmployeeRole { Manager, Chef, Waiter, Cashier, DeliveryDriver };
enum class MenuCategory { Appetizer, MainCourse, Dessert, Drink };

// ============================================================================
// Helper to print OrderStatus as text
// ============================================================================
string statusToString(OrderStatus s)
{
    switch (s)
    {
    case OrderStatus::New:
        return "New";
    case OrderStatus::Preparing:
        return "Preparing";
    case OrderStatus::Ready:
        return "Ready";
    case OrderStatus::Served:
        return "Served";
    case OrderStatus::Completed:
        return "Completed";
    case OrderStatus::Cancelled:
        return "Cancelled";
    }
    return "Unknown";
}

string typeToString(OrderType t)
{
    switch (t)
    {
    case OrderType::DineIn:
        return "Dine-In";
    case OrderType::Takeaway:
        return "Takeaway";
    case OrderType::Delivery:
        return "Delivery";
    }
    return "Unknown";
}

// ============================================================================
// Helpers to print enums as text 
// ============================================================================
string paymentMethodToString(PaymentMethod m) {
    switch (m) {
    case PaymentMethod::Cash: return "Cash";
    case PaymentMethod::Card: return "Card";
    case PaymentMethod::MobilePayment: return "Mobile Payment";
    }
    return "Unknown";
}

string paymentStatusToString(PaymentStatus s) {
    switch (s) {
    case PaymentStatus::Completed: return "Completed";
    case PaymentStatus::Refunded: return "Refunded";
    }
    return "Unknown";
}

string deliveryStatusToString(DeliveryStatus s) {
    switch (s) {
    case DeliveryStatus::Assigned: return "Assigned";
    case DeliveryStatus::OnTheWay: return "On The Way";
    case DeliveryStatus::Delivered: return "Delivered";
    case DeliveryStatus::Cancelled: return "Cancelled";
    }
    return "Unknown";
}

string employeeRoleToString(EmployeeRole role) {
    switch (role) {
    case EmployeeRole::Manager: return "Manager";
    case EmployeeRole::Chef: return "Chef";
    case EmployeeRole::Waiter: return "Waiter";
    case EmployeeRole::Cashier: return "Cashier";
    case EmployeeRole::DeliveryDriver: return "Delivery Driver";
    }
    return "Unknown";
}

string getCurrentDateTime() {
    time_t now = time(0);
    tm* ltm = localtime(&now);
    char buf[64];
    snprintf(buf, sizeof(buf), "%04d-%02d-%02d %02d:%02d",
        1900 + ltm->tm_year, 1 + ltm->tm_mon, ltm->tm_mday,
        ltm->tm_hour, ltm->tm_min);
    return string(buf);
}

void clearScreen() {//yousef
#ifdef _WIN32
    system("cls");
#else
    cout << "\033[2J\033[1;1H";
#endif
}


//centeralized validation functions








// ============================================================================
// CENTRALIZED INPUT / UI HELPERS
// UI-only additions: the existing domain classes remain mostly untouched.
// ============================================================================
namespace UI {
    const string RESET = "\033[0m";
    const string RED = "\033[31m";
    const string GREEN = "\033[32m";
    const string YELLOW = "\033[33m";
    const string CYAN = "\033[36m";
    const string BOLD = "\033[1m";

    void success(const string& message) { cout << GREEN << "[SUCCESS] " << message << RESET << "\n"; }
    void error(const string& message) { cout << RED << "[ERROR] " << message << RESET << "\n"; }
    void warning(const string& message) { cout << YELLOW << "[WARNING] " << message << RESET << "\n"; }

    void breadcrumb(const string& path) {
        cout << CYAN << "Restaurant > " << path << RESET << "\n\n";
    }

    void title(const string& text) {
        cout << CYAN << BOLD
            << "\n============================================================\n"
            << "  " << text << "\n"
            << "============================================================\n"
            << RESET;
    }

    void pause() {
        cout << YELLOW << "\nPress ENTER to continue..." << RESET;
        string dummy;
        getline(cin, dummy);
    }

    string trim(const string& value) {
        size_t first = value.find_first_not_of(" \t\r\n");
        if (first == string::npos) return "";
        size_t last = value.find_last_not_of(" \t\r\n");
        return value.substr(first, last - first + 1);
    }

    bool parseInt(const string& text, int& value) {
        stringstream ss(trim(text));
        char extra;
        if (!(ss >> value)) return false;
        return !(ss >> extra);
    }

    bool parseDouble(const string& text, double& value) {
        stringstream ss(trim(text));
        char extra;
        if (!(ss >> value)) return false;
        return !(ss >> extra);
    }

    int readInt(const string& prompt, int minValue, int maxValue) {
        while (true) {
            cout << prompt;
            string input;
            getline(cin, input);
            int value;
            if (!parseInt(input, value)) { error("Please enter a valid whole number."); continue; }
            if (value < minValue || value > maxValue) {
                error("Please enter a value between " + to_string(minValue) + " and " + to_string(maxValue) + ".");
                continue;
            }
            return value;
        }
    }

    int readPositiveInt(const string& prompt) { return readInt(prompt, 1, 2147483647); }

    double readDouble(const string& prompt, double minValue) {
        while (true) {
            cout << prompt;
            string input;
            getline(cin, input);
            double value;
            if (!parseDouble(input, value)) { error("Please enter a valid number."); continue; }
            if (value < minValue) { error("Value must be at least " + to_string(minValue) + "."); continue; }
            return value;
        }
    }

    double readPositiveDouble(const string& prompt) { return readDouble(prompt, 0.01); }

    string readRequired(const string& prompt) {
        while (true) {
            cout << prompt;
            string value;
            getline(cin, value);
            value = trim(value);
            if (value.empty()) { error("This field cannot be empty."); continue; }
            return value;
        }
    }

    bool isValidEmail(const string& email) {
        static const regex pattern(R"(^[A-Za-z0-9._%+-]+@[A-Za-z0-9.-]+\.[A-Za-z]{2,}$)");
        return regex_match(email, pattern);
    }

    string readRequiredString(const string& prompt) { return readRequired(prompt); }

    string readEmail(const string& prompt) {
        while (true) {
            string value = readRequired(prompt);
            if (!isValidEmail(value)) { error("Invalid email. Example: name@example.com"); continue; }
            return value;
        }
    }

    bool isValidPhone(const string& phone) {
        if (phone.size() < 7 || phone.size() > 15) return false;
        if (!all_of(phone.begin(), phone.end(), [](unsigned char c) { return isdigit(c); })) return false;
        return true;
    }

    string readPhone(const string& prompt) {
        while (true) {
            string value = readRequired(prompt);
            if (!isValidPhone(value)) { error("Phone must contain digits only and be 7-15 digits long."); continue; }
            return value;
        }
    }

    bool isValidDate(const string& date) {
        if (date.size() != 10 || date[4] != '-' || date[7] != '-') return false;
        for (size_t i = 0; i < date.size(); ++i) {
            if (i == 4 || i == 7) continue;
            if (!isdigit(static_cast<unsigned char>(date[i]))) return false;
        }
        int year = stoi(date.substr(0, 4));
        int month = stoi(date.substr(5, 2));
        int day = stoi(date.substr(8, 2));
        if (year < 2000 || month < 1 || month > 12 || day < 1) return false;
        int daysInMonth[] = { 31,28,31,30,31,30,31,31,30,31,30,31 };
        bool leap = (year % 400 == 0) || (year % 4 == 0 && year % 100 != 0);
        if (leap) daysInMonth[1] = 29;
        return day <= daysInMonth[month - 1];
    }

    string readDate(const string& prompt) {
        while (true) {
            string value = readRequired(prompt);
            if (!isValidDate(value)) { error("Invalid date. Use YYYY-MM-DD."); continue; }
            return value;
        }
    }

    bool isValidTime(const string& value) {
        if (value.size() != 5 || value[2] != ':') return false;
        for (size_t i = 0; i < value.size(); ++i) {
            if (i == 2) continue;
            if (!isdigit(static_cast<unsigned char>(value[i]))) return false;
        }
        int h = stoi(value.substr(0, 2)), m = stoi(value.substr(3, 2));
        return h >= 0 && h <= 23 && m >= 0 && m <= 59;
    }

    string readTime(const string& prompt) {
        while (true) {
            string value = readRequired(prompt);
            if (!isValidTime(value)) { error("Invalid time. Use HH:MM (24-hour format)."); continue; }
            return value;
        }
    }
}

// Customer

class Customer {
private:
    int customerID;
    string name;
    string phoneNumber;
    string address;
    string email;

public:
    // Constructor
    Customer(int id = 0, string n = "", string phone = "", string addr = "", string em = "")
        : customerID(id), name(n), phoneNumber(phone), address(addr), email(em) {   //recieve data then initialize variables
    }


    // Getters   (no modifications )
    int getCustomerID() const { return customerID; }
    string getName() const { return name; }
    string getPhoneNumber() const { return phoneNumber; }
    string getAddress() const { return address; }
    string getEmail() const { return email; }

    // Setters
    void setName(string n) { name = n; }                        //change and edit name
    void setPhoneNumber(string phone) { phoneNumber = phone; } // change and edit number
    void setAddress(string addr) { address = addr; }           //.................address
    void setEmail(string em) { email = em; }                   //................email


    // Display Methods
    void displayInfo() const {
        cout << "ID: " << customerID << "\n";
        cout << "Name: " << name << "\n";
        cout << "Phone: " << phoneNumber << "\n";
        cout << "Address: " << address << "\n";
        cout << "Email: " << email << "\n";

    }
};

//CustomerManager
class CustomerManager {
private:
    vector<Customer> customers;   //dynamic array.. هيتخزن فيها بيانات الcustomers

public:
    // Add Customer
    bool addCustomer(Customer c) {
        if (c.getCustomerID() <= 0) { UI::error("Customer ID must be greater than zero."); return false; }
        if (UI::trim(c.getName()).empty()) { UI::error("Customer name cannot be empty."); return false; }
        if (!UI::isValidPhone(c.getPhoneNumber())) { UI::error("Invalid phone number format."); return false; }
        if (!UI::isValidEmail(c.getEmail())) { UI::error("Invalid email format."); return false; }
        for (const auto& cust : customers) {
            if (cust.getCustomerID() == c.getCustomerID()) { UI::error("Customer ID already exists."); return false; }
            if (cust.getPhoneNumber() == c.getPhoneNumber()) { UI::error("Phone number already exists."); return false; }
            if (cust.getEmail() == c.getEmail()) { UI::error("Email already exists."); return false; }
        }
        customers.push_back(c);
        UI::success("Customer added successfully.");
        return true;
    }

    // Display All Customers
    void displayAllCustomers() const {
        if (customers.empty()) {
            UI::error("No customers found.");
            return;
        }

        cout << left
            << setw(8) << "ID"
            << setw(24) << "Name"
            << setw(18) << "Phone"
            << setw(32) << "Address"
            << setw(30) << "Email" << "\n";
        cout << string(112, '-') << "\n";
        for (const auto& cust : customers) {
            cout << left
                << setw(8) << cust.getCustomerID()
                << setw(24) << cust.getName()
                << setw(18) << cust.getPhoneNumber()
                << setw(32) << cust.getAddress()
                << setw(30) << cust.getEmail() << "\n";
        }
    }

    // Search Customer
    Customer* searchCustomer(int customerID) {
        for (auto& cust : customers) {
            if (cust.getCustomerID() == customerID) {
                return &cust;
            }
        }
        return nullptr;
    }

    //  Update Customer
    bool updateCustomer(int customerID, string newName, string newPhone, string newAddress, string newEmail) {
        Customer* cust = searchCustomer(customerID);
        if (!cust) { UI::error("Customer not found."); return false; }
        newName = UI::trim(newName); newPhone = UI::trim(newPhone); newAddress = UI::trim(newAddress); newEmail = UI::trim(newEmail);
        if (newName.empty()) { UI::error("Name cannot be empty."); return false; }
        if (!UI::isValidPhone(newPhone)) { UI::error("Invalid phone number format."); return false; }
        if (!UI::isValidEmail(newEmail)) { UI::error("Invalid email format."); return false; }
        if (isPhoneUsed(newPhone, customerID)) { UI::error("Phone number already exists."); return false; }
        if (isEmailUsed(newEmail, customerID)) { UI::error("Email already exists."); return false; }
        cust->setName(newName); cust->setPhoneNumber(newPhone); cust->setAddress(newAddress); cust->setEmail(newEmail);
        UI::success("Customer updated successfully.");
        return true;
    }

    bool isPhoneUsed(const string& phone, int exceptCustomerID = -1) const {
        for (const auto& cust : customers)
            if (cust.getCustomerID() != exceptCustomerID && cust.getPhoneNumber() == phone) return true;
        return false;
    }

    bool isEmailUsed(const string& email, int exceptCustomerID = -1) const {
        for (const auto& cust : customers)
            if (cust.getCustomerID() != exceptCustomerID && cust.getEmail() == email) return true;
        return false;
    }

    Customer* searchCustomerByPhone(const string& phone) {
        for (auto& cust : customers) if (cust.getPhoneNumber() == phone) return &cust;
        return nullptr;
    }

    Customer* searchCustomerByEmail(const string& email) {
        for (auto& cust : customers) if (cust.getEmail() == email) return &cust;
        return nullptr;
    }

    // Display Customer Orders (Stub)
    void displayCustomerOrders(int customerID) {   // لسه هتتظبط بعد ال integration
        if (searchCustomer(customerID) == nullptr) {
            cout << "Error: Customer with ID " << customerID << " not found.\n";
            return;
        }

    }

    //  Display Customer Reservations (Stub)
    void displayCustomerReservations(int customerID) { // بردو هتتظبط بعد ال integration
        if (searchCustomer(customerID) == nullptr) {
            cout << "Error: Customer with ID " << customerID << " not found.\n";
            return;
        }

    }
};



// Reservation

class Reservation {
private:
    int reservationID;
    int customerID;
    int tableID;
    string date;
    string time;
    int numberOfGuests;
    ReservationStatus status;

public:
    // Constructor
    Reservation(int rID = 0, int cID = 0, int tID = 0, string d = "", string t = "", int guests = 0, ReservationStatus s = ReservationStatus::Pending)
        : reservationID(rID), customerID(cID), tableID(tID), date(d), time(t), numberOfGuests(guests), status(s) {  //recieve data and initialize
    }


    // Getters
    int getReservationID() const { return reservationID; }
    int getCustomerID() const { return customerID; }
    int getTableID() const { return tableID; }
    string getDate() const { return date; }
    string getTime() const { return time; }
    int getNumberOfGuests() const { return numberOfGuests; }
    ReservationStatus getStatus() const { return status; }


    // Setters
    void setCustomerID(int cID) { customerID = cID; }    //update or change id
    void setTableID(int tID) { tableID = tID; }
    void setDate(string d) { date = d; }
    void setTime(string t) { time = t; }
    void setNumberOfGuests(int guests) { numberOfGuests = guests; }
    void setStatus(ReservationStatus s) { status = s; }


    // Display Method

    void displayInfo() const {
        cout << "Reservation ID: " << reservationID << "\n";
        cout << "Customer ID: " << customerID << "\n";
        cout << "Table ID: " << tableID << "\n";
        cout << "Date: " << date << "\n";
        cout << "Time: " << time << "\n";
        cout << "Guests: " << numberOfGuests << "\n";
        cout << "Status: ";
        switch (status) {
        case ReservationStatus::Pending: cout << "Pending\n"; break;
        case ReservationStatus::Confirmed: cout << "Confirmed\n"; break;
        case ReservationStatus::Completed: cout << "Completed\n"; break;
        case ReservationStatus::Cancelled: cout << "Cancelled\n"; break;
        }

    }
};


// ReservationManager
class ReservationManager {
private:
    vector<Reservation> reservations;

public:
    // Create Reservation
    bool createReservation(Reservation r) {
        if (r.getNumberOfGuests() <= 0) {
            cout << "Error: Number of guests must be greater than zero.\n";
            return false;
        }

        for (const auto& res : reservations) {
            if (res.getReservationID() == r.getReservationID()) {
                cout << "Error: Reservation ID " << r.getReservationID() << " already exists.\n";
                return false;
            }
        }

        if (!checkTableAvailability(r.getTableID(), r.getDate(), r.getTime())) {
            cout << "Error: Table " << r.getTableID() << " is already reserved at this date and time \n";
            return false;
        }

        reservations.push_back(r);
        cout << "Reservation created successfully \n";
        return true;
    }

    // Confirm Reservation
    bool confirmReservation(int reservationID) {
        for (auto& res : reservations) {
            if (res.getReservationID() == reservationID) {
                if (res.getStatus() == ReservationStatus::Cancelled) {
                    cout << "Error: Cannot confirm a cancelled reservation.\n";
                    return false;
                }
                res.setStatus(ReservationStatus::Confirmed);
                cout << "Reservation " << reservationID << " confirmed successfully.\n";
                return true;
            }
        }
        cout << "Error: Reservation ID " << reservationID << " not found.\n";
        return false;
    }

    //  Cancel Reservation
    bool cancelReservation(int reservationID) {
        for (auto& res : reservations) {
            if (res.getReservationID() == reservationID) {
                if (res.getStatus() == ReservationStatus::Completed) {
                    cout << "Error: Cannot cancel a completed reservation.\n";
                    return false;
                }
                res.setStatus(ReservationStatus::Cancelled);
                cout << "Reservation " << reservationID << " cancelled successfully.\n";
                return true;
            }
        }
        cout << "Error: Reservation ID " << reservationID << " not found.\n";
        return false;
    }

    //  Display Reservation
    void displayReservations() const {
        if (reservations.empty()) {
            cout << "No reservations found.\n";
            return;
        }

        cout << "\n RESERVATION LIST \n";
        for (const auto& res : reservations) {
            res.displayInfo();
        }
    }

    // Search Reservation
    Reservation* searchReservation(int reservationID) {
        for (auto& res : reservations) {          //بيلف علي كل الحجوزات يتأكد هل ال id موجود او لا
            if (res.getReservationID() == reservationID) {
                return &res;
            }
        }
        return nullptr;
    }

    // Check Table Availability
    bool checkTableAvailability(int tableID, string date, string time) const {
        for (const auto& res : reservations) {
            bool isActive = (res.getStatus() == ReservationStatus::Pending || res.getStatus() == ReservationStatus::Confirmed);
            if (isActive && res.getTableID() == tableID && res.getDate() == date && res.getTime() == time) {  //بيشوف لو الtable دي محجوزة في نفس التاريخ والوقت او لا 
                return false; // Table is not available
            }
        }
        return true; // Table is available
    }
};




// Table class
class Table {
public:
    // Constructor
    Table(int tableID, int capacity, string location, TableStatus status) {
        setTableID(tableID);
        setCapacity(capacity);
        setlocation(location);
        setStatus(status);
    }
    // Default constructor
    Table() {
        setTableID(0);
        setCapacity(1);
        setlocation("Undefined");
        setStatus(TableStatus::Available);
    }
    // Setters
    void setTableID(int tableID) {
        if (tableID > 0) {
            this->tableID = tableID;
        }
        else {
            cout << "Error: Table ID must be a positive number. \n";
        }
    }

    void setCapacity(int capacity) {
        if (capacity > 0) {
            this->capacity = capacity;
        }
        else {
            cout << "Error: Capacity must be greater than Zero. \n";
        }
    }

    void setlocation(string location) {
        if (location.length() >= 3) {
            this->location = location;
        }
        else {
            cout << "Error: Location must be longer than 3 charcaters. \n";
        }
    }

    void setStatus(TableStatus status) {
        this->status = status;
    }


    // Getters
    int getTableID() const { return tableID; }

    int getCapacity() const { return capacity; }

    string getLocation() const { return location; }

    string getStatusString() const {
        switch (status) {
        case TableStatus::Available:
            return "Available";
        case TableStatus::Reserved:
            return "Reserved";
        case TableStatus::Occupied:
            return "Occupied";
        case TableStatus::OutOfService:
            return "Out of service";
        default:
            return "Unknown";
        }
    }

    TableStatus getStatus() const {
        return status;
    }

private:
    int tableID;
    int capacity;
    string location;
    TableStatus status;
};
// Table Manager class
class TableManager {
public:
    // Constructor

    // Functions
    bool addTable(Table t) {

        for (const Table& item : tables) {
            if (item.getTableID() == t.getTableID()) {
                return false;
            }
        }
        tables.push_back(t);
        cout << "Table added successfully.\n";
        return true;
    }

    void displayAvailableTables() {
        if (tables.empty()) {
            cout << "Error: There are no tables added yet. \n";
        }
        else {
            cout << string(70, '_') << endl;
            cout << string(27, ' ') + "available tables" + string(27, ' ') << endl;
            cout << string(70, '_') << endl;
            cout << left << setw(10) << "ID"
                << setw(14) << "Capacity"
                << setw(26) << "Location"
                << setw(20) << "Status" << endl;
            for (const Table& item : tables) {
                if (item.getStatus() == TableStatus::Available) {
                    cout << left << setw(10) << item.getTableID()
                        << setw(14) << item.getCapacity()
                        << setw(26) << item.getLocation()
                        << setw(20) << item.getStatusString() << endl;
                }
            }
        }

    }

    Table* searchTable(int tableID) {
        for (Table& item : tables) {
            if (item.getTableID() == tableID) {
                return &item;
            }
        }
        return nullptr;
    }

    bool updateTableStatus(int tableID, TableStatus newStatus) {
        for (Table& item : tables) {
            if (item.getTableID() == tableID) {
                item.setStatus(newStatus);
                return true;
            }
        }
        return false;
    }

    bool isTableAvailable(int tableID) {
        for (const Table& item : tables) {
            if (item.getTableID() == tableID) {
                if (item.getStatus() == TableStatus::Available)
                    return true;
            }
        }
        return false;
    }

    // ========================================================================
    // ADDED — required by the specification (and the original project spec,
    // section 2.2 "Assign a table to a customer/order"), was missing.
    // Marks the table Occupied only if it was actually Available; rejects
    // assigning a table that's Reserved / Occupied / Out of Service already.
    // ========================================================================
    bool assignTable(int tableID) {
        for (Table& item : tables) {
            if (item.getTableID() == tableID) {
                if (item.getStatus() != TableStatus::Available) {
                    cout << "Error: Table " << tableID << " is not available ("
                        << item.getStatusString() << ").\n";
                    return false;
                }
                item.setStatus(TableStatus::Occupied);
                cout << "Table " << tableID << " assigned successfully.\n";
                return true;
            }
        }
        cout << "Error: Table " << tableID << " not found.\n";
        return false;
    }

private:
    vector <Table> tables;
};

// MenuItem class
class MenuItem {
public:
    // Constructor
    MenuItem(int itemID, string name, double price, MenuCategory category, string description, bool isAvailable) {
        setItemID(itemID);
        setName(name);
        setPrice(price);
        setCategory(category);
        setDescription(description);
        setIsAvailable(isAvailable);
    }
    // Setters
    void setItemID(int itemID) {
        if (itemID > 0) {
            this->itemID = itemID;
        }
        else {
            cout << "Error: Item ID must be a positive number. \n";
        }
    }

    void setName(string name) {
        if (name.length() >= 3) {
            this->name = name;
        }
        else {
            cout << "Error: Name must be longer than 3 charcaters. \n";
        }
    }

    void setPrice(double price) {
        if (price <= 0) {
            cout << "Error: Price must be more than Zero. \n";
        }
        else {
            this->price = price;
        }
    }

    void setCategory(MenuCategory category) {
        this->category = category;
    }

    void setDescription(string description) {
        if (description.length() >= 3) {
            this->description = description;
        }
        else {
            cout << "Error: Description must be longer than 3 charcaters. \n";
        }
    }

    void setIsAvailable(bool isavailable) {
        this->isavailable = isavailable;
    }

    // Getters
    int getItemID() const { return itemID; }

    string getName() const { return name; }

    double getPrice() const { return price; }

    MenuCategory getCategory() const { return category; }

    string getCategoryString() const {
        switch (category) {
        case MenuCategory::Appetizer:
            return "Appetizers";
        case MenuCategory::Dessert:
            return "Desserts";
        case MenuCategory::Drink:
            return "Drinks";
        case MenuCategory::MainCourse:
            return "Main Course";
        default:
            return "Unknown";
        }
    }

    string getDescription() const { return description; }

    bool getIsAvailable() const { return isavailable; }

    string getIsAvailableString() const {
        if (isavailable) {
            return "Yes";
        }
        else {
            return "No";
        }
    }

private:
    int itemID;
    string name;
    double price;
    MenuCategory category;
    string description;
    bool isavailable;
};

class MenuManager {
public:
    // Functions
    bool addItem(MenuItem item) {
        if (item.getPrice() <= 0) {
            cout << "Error: Price must be greater than zero to add an item.\n";
            return false;
        }

        for (const MenuItem& it : items) {
            if (it.getItemID() == item.getItemID()) {
                return false;
            }
        }
        items.push_back(item);
        cout << "Item added successfully.\n";
        return true;
    }

    bool removeItem(int itemID) {
        for (size_t i = 0; i < items.size(); ++i) {
            if (items[i].getItemID() == itemID) {

                items.erase(items.begin() + i);
                cout << "Item removed successfully.\n";
                return true;
            }
        }
        return false;
    }

    bool updateItem(int itemID, string name, double price, MenuCategory category, string description, bool isavailable) {
        for (MenuItem& item : items) {
            if (item.getItemID() == itemID) {

                item.setName(name);
                item.setPrice(price);
                item.setCategory(category);
                item.setDescription(description);
                item.setIsAvailable(isavailable);

                cout << "Item updated successfully.\n";
                return true;
            }
        }
        cout << "Error: Item ID not found.\n";
        return false;
    }

    MenuItem* searchItem(int itemID) {
        for (MenuItem& item : items) {
            if (item.getItemID() == itemID) {
                return &item;
            }
        }
        return nullptr;
    }

    void displayMenu() {
        if (items.empty()) {
            cout << "Error: There are no items added yet. \n";
        }
        else {
            cout << string(90, '_') << endl;
            cout << string(40, ' ') + "Menu" + string(40, ' ') << endl;
            cout << string(90, '_') << endl;
            cout << left << setw(8) << "Item Id"
                << setw(27) << "Name"
                << setw(10) << "Price"
                << setw(35) << "Description"
                << setw(10) << "Avilability" << endl;
            cout << string(39, ' ') + "Appetizers" + string(39, ' ') << endl;
            for (const MenuItem& item : items) {
                if (item.getCategory() == MenuCategory::Appetizer) {
                    printItemRow(item);
                }
            }
            cout << string(39, ' ') + "Main Course" + string(39, ' ') << endl;
            for (const MenuItem& item : items) {
                if (item.getCategory() == MenuCategory::MainCourse) {
                    printItemRow(item);

                }
            }
            cout << string(39, ' ') + "Desserts" + string(39, ' ') << endl;
            for (const MenuItem& item : items) {
                if (item.getCategory() == MenuCategory::Dessert) {
                    printItemRow(item);
                }
            }
            cout << string(39, ' ') + "Drinks" + string(39, ' ') << endl;
            for (const MenuItem& item : items) {
                if (item.getCategory() == MenuCategory::Drink) {
                    printItemRow(item);
                }
            }
        }
    }

    bool changeAvailability(int itemID, bool available) {
        for (MenuItem& item : items) {
            if (itemID == item.getItemID()) {
                item.setIsAvailable(available);
                return true;
            }
        }
        return false;
    }


private:
    vector<MenuItem> items;
    // Helper print row

    void printItemRow(const MenuItem& item) const {
        cout << left << setw(8) << item.getItemID()
            << setw(27) << item.getName()
            << setw(10) << item.getPrice()
            << setw(35) << item.getDescription()
            << setw(10) << item.getIsAvailableString() << endl;
    }
};



// ============================================================================
// OrderItem struct
// ============================================================================
struct OrderItem
{
    int itemID;
    string itemName;
    int quantity;
    double price;

    OrderItem() : itemID(0), quantity(0), price(0.0) {}
    OrderItem(int id, string name, int qty, double p)
        : itemID(id), itemName(name), quantity(qty), price(p) {
    }
};

// ============================================================================
// Order class
// ============================================================================
class Order
{
private:
    int orderID;
    int customerID;
    int tableID;
    vector<OrderItem> items;
    string orderDateTime;
    OrderType orderType;
    OrderStatus status;
    double subtotal;
    double taxAmount;
    double serviceCharge;
    double totalPrice;

public:
    // constructors
    Order() : orderID(0), customerID(0), tableID(0),
        orderType(OrderType::DineIn), status(OrderStatus::New),
        subtotal(0.0), taxAmount(0.0), serviceCharge(0.0), totalPrice(0.0) {
    }

    Order(int oid, int cid, int tid, OrderType type, string dt)
        : orderID(oid), customerID(cid), tableID(tid),
        orderType(type), status(OrderStatus::New),
        subtotal(0.0), taxAmount(0.0), serviceCharge(0.0), totalPrice(0.0)
    {
        orderDateTime = dt;
    }

    // getters
    int getOrderID() const { return orderID; }
    int getCustomerID() const { return customerID; }
    int getTableID() const { return tableID; }
    string getOrderDateTime() const { return orderDateTime; }
    OrderType getOrderType() const { return orderType; }
    OrderStatus getStatus() const { return status; }
    double getSubtotal() const { return subtotal; }
    double getTaxAmount() const { return taxAmount; }
    double getServiceCharge() const { return serviceCharge; }
    double getTotalPrice() const { return totalPrice; }
    vector<OrderItem>& getItems() { return items; }
    const vector<OrderItem>& getItems() const { return items; }

    // setters
    void setOrderID(int id) { orderID = id; }
    void setCustomerID(int id) { customerID = id; }
    void setTableID(int id) { tableID = id; }
    void setOrderDateTime(string dt) { orderDateTime = dt; }
    void setOrderType(OrderType t) { orderType = t; }
    void setStatus(OrderStatus s) { status = s; }
    void setSubtotal(double val) { subtotal = val; }
    void setTaxAmount(double val) { taxAmount = val; }
    void setServiceCharge(double val) { serviceCharge = val; }
    void setTotalPrice(double val) { totalPrice = val; }

    // add item to order
    void addItem(const OrderItem& it)
    {
        // if same item exists, just increase quantity
        for (auto& existing : items)
        {
            if (existing.itemID == it.itemID)
            {
                existing.quantity += it.quantity;
                return;
            }
        }
        items.push_back(it);
    }

    // remove item by itemID
    bool removeItem(int itemID)
    {
        for (auto it = items.begin(); it != items.end(); ++it)
        {
            if (it->itemID == itemID)
            {
                items.erase(it);
                return true;
            }
        }
        return false;
    }

    // change quantity of an item
    bool changeQuantity(int itemID, int newQty)
    {
        if (newQty <= 0)
            return false;
        for (auto& it : items)
        {
            if (it.itemID == itemID)
            {
                it.quantity = newQty;
                return true;
            }
        }
        return false;
    }

    // recalculate financials
    void recalculate()
    {
        subtotal = 0.0;
        for (const auto& it : items)
        {
            subtotal += it.price * it.quantity;
        }
        taxAmount = subtotal * 0.14;     // 14% tax
        serviceCharge = subtotal * 0.12; // 12% service
        totalPrice = subtotal + taxAmount + serviceCharge;
    }

    // display order header info
    void displayInfo() const
    {
        cout << "\n--- Order #" << orderID << " ---\n";
        cout << "Customer ID : " << customerID << "\n";
        if (tableID != 0)
            cout << "Table ID    : " << tableID << "\n";
        cout << "Type        : " << typeToString(orderType) << "\n";
        cout << "Status      : " << statusToString(status) << "\n";
        cout << "Date/Time   : " << orderDateTime << "\n";
    }

    // display items
    void displayItems() const
    {
        if (items.empty())
        {
            cout << "  (no items)\n";
            return;
        }
        cout << "\n  Items:\n";
        cout << "  " << left << setw(6) << "ID"
            << setw(20) << "Name"
            << setw(10) << "Qty"
            << setw(10) << "Unit"
            << setw(10) << "Total" << "\n";
        cout << "  " << string(56, '-') << "\n";
        for (const auto& it : items)
        {
            cout << "  " << left << setw(6) << it.itemID
                << setw(20) << it.itemName
                << setw(10) << it.quantity
                << setw(10) << fixed << setprecision(2) << it.price
                << setw(10) << it.price * it.quantity << "\n";
        }
    }

    // display financial summary
    void displayFinancials() const
    {
        cout << "\n  Financials:\n";
        cout << "  Subtotal      : " << fixed << setprecision(2) << subtotal << "\n";
        cout << "  Tax (14%)     : " << taxAmount << "\n";
        cout << "  Service (12%) : " << serviceCharge << "\n";
        cout << "  TOTAL         : " << totalPrice << "\n";
    }
};

// ============================================================================
// OrderManager class
// ============================================================================
class OrderManager
{
private:
    vector<Order> orders;
    int nextOrderID;

    // check if status transition is valid
    bool canTransition(OrderStatus from, OrderStatus to)
    {
        if (from == to)
            return true;

        // allowed forward flow
        if (from == OrderStatus::New && to == OrderStatus::Preparing)
            return true;
        if (from == OrderStatus::New && to == OrderStatus::Cancelled)
            return true;

        if (from == OrderStatus::Preparing && to == OrderStatus::Ready)
            return true;
        if (from == OrderStatus::Preparing && to == OrderStatus::Cancelled)
            return true;

        if (from == OrderStatus::Ready && to == OrderStatus::Served)
            return true;
        if (from == OrderStatus::Ready && to == OrderStatus::Cancelled)
            return true;

        if (from == OrderStatus::Served && to == OrderStatus::Completed)
            return true;

        // no other transitions allowed
        return false;
    }

public:
    OrderManager() : nextOrderID(1000) {}

    // ------------------------------------------------------------------------
    // create a new order, returns the new orderID or -1 on failure
    // ------------------------------------------------------------------------
    int createOrder(int customerID, int tableID, OrderType type)
    {
        if (customerID <= 0)
        {
            cout << "Error: invalid customer ID.\n";
            return -1;
        }

        // get current date/time as string
        time_t now = time(0);
        tm* ltm = localtime(&now);
        char buf[64];
        snprintf(buf, sizeof(buf), "%04d-%02d-%02d %02d:%02d",
            1900 + ltm->tm_year, 1 + ltm->tm_mon, ltm->tm_mday,
            ltm->tm_hour, ltm->tm_min);

        Order o(nextOrderID, customerID, tableID, type, string(buf));
        orders.push_back(o);
        nextOrderID++;
        return o.getOrderID();
    }

    // ------------------------------------------------------------------------
    // add item to an existing order
    // ------------------------------------------------------------------------
    bool addItem(int orderID, OrderItem item)
    {
        if (item.quantity <= 0)
        {
            cout << "Error: quantity must be greater than zero.\n";
            return false;
        }
        if (item.price < 0)
        {
            cout << "Error: price cannot be negative.\n";
            return false;
        }

        Order* o = searchOrder(orderID);
        if (!o)
        {
            cout << "Error: order not found.\n";
            return false;
        }

        if (o->getStatus() != OrderStatus::New)
        {
            cout << "Error: cannot modify items after order is no longer New.\n";
            return false;
        }

        o->addItem(item);
        o->recalculate();
        return true;
    }

    // ------------------------------------------------------------------------
    // remove item from order
    // ------------------------------------------------------------------------
    bool removeItem(int orderID, int itemID)
    {
        Order* o = searchOrder(orderID);
        if (!o)
        {
            cout << "Error: order not found.\n";
            return false;
        }
        if (o->getStatus() != OrderStatus::New)
        {
            cout << "Error: cannot modify items after order is no longer New.\n";
            return false;
        }

        bool ok = o->removeItem(itemID);
        if (ok)
            o->recalculate();
        return ok;
    }

    // ------------------------------------------------------------------------
    // change quantity of an item
    // ------------------------------------------------------------------------
    bool changeQuantity(int orderID, int itemID, int newQty)
    {
        if (newQty <= 0)
        {
            cout << "Error: new quantity must be greater than zero.\n";
            return false;
        }

        Order* o = searchOrder(orderID);
        if (!o)
        {
            cout << "Error: order not found.\n";
            return false;
        }
        if (o->getStatus() != OrderStatus::New)
        {
            cout << "Error: cannot modify items after order is no longer New.\n";
            return false;
        }

        bool ok = o->changeQuantity(itemID, newQty);
        if (ok)
            o->recalculate();
        return ok;
    }

    // ------------------------------------------------------------------------
    // calculate subtotal
    // ------------------------------------------------------------------------
    double calculateSubtotal(int orderID)
    {
        Order* o = searchOrder(orderID);
        if (!o)
            return -1.0;
        o->recalculate();
        return o->getSubtotal();
    }

    // ------------------------------------------------------------------------
    // calculate tax (14%)
    // ------------------------------------------------------------------------
    double calculateTax(int orderID)
    {
        Order* o = searchOrder(orderID);
        if (!o)
            return -1.0;
        o->recalculate();
        return o->getTaxAmount();
    }

    // ------------------------------------------------------------------------
    // calculate total (subtotal + tax + service)
    // ------------------------------------------------------------------------
    double calculateTotal(int orderID)
    {
        Order* o = searchOrder(orderID);
        if (!o)
            return -1.0;
        o->recalculate();
        return o->getTotalPrice();
    }

    // ------------------------------------------------------------------------
    // cancel order — only from New, Preparing, or Ready
    // ------------------------------------------------------------------------
    bool cancelOrder(int orderID)
    {
        Order* o = searchOrder(orderID);
        if (!o)
        {
            cout << "Error: order not found.\n";
            return false;
        }

        OrderStatus s = o->getStatus();
        if (s == OrderStatus::Served || s == OrderStatus::Completed || s == OrderStatus::Cancelled)
        {
            cout << "Error: cannot cancel order in status '" << statusToString(s) << "'.\n";
            return false;
        }

        o->setStatus(OrderStatus::Cancelled);
        return true;
    }

    // ------------------------------------------------------------------------
    // update order status with validation
    // ------------------------------------------------------------------------
    bool updateOrderStatus(int orderID, OrderStatus newStatus)
    {
        Order* o = searchOrder(orderID);
        if (!o)
        {
            cout << "Error: order not found.\n";
            return false;
        }

        OrderStatus current = o->getStatus();
        if (!canTransition(current, newStatus))
        {
            cout << "Error: invalid status transition from '"
                << statusToString(current) << "' to '"
                << statusToString(newStatus) << "'.\n";
            return false;
        }

        o->setStatus(newStatus);
        return true;
    }

    // ------------------------------------------------------------------------
    // display full order details
    // ------------------------------------------------------------------------
    void displayOrderDetails(int orderID)
    {
        Order* o = searchOrder(orderID);
        if (!o)
        {
            cout << "Order #" << orderID << " not found.\n";
            return;
        }
        o->displayInfo();
        o->displayItems();
        o->displayFinancials();
        cout << "\n";
    }

    // ------------------------------------------------------------------------
    // search order by ID (returns pointer or nullptr)
    // ------------------------------------------------------------------------
    Order* searchOrder(int orderID)
    {
        for (auto& o : orders)
        {
            if (o.getOrderID() == orderID)
                return &o;
        }
        return nullptr;
    }

    // ------------------------------------------------------------------------
    // display all orders (brief list)
    // ------------------------------------------------------------------------
    void displayAllOrders()
    {
        if (orders.empty())
        {
            cout << "No orders yet.\n";
            return;
        }
        cout << "\n=== All Orders ===\n";
        cout << left << setw(10) << "Order#"
            << setw(12) << "CustID"
            << setw(12) << "Type"
            << setw(14) << "Status"
            << setw(12) << "Total" << "\n";
        cout << string(60, '-') << "\n";
        for (const auto& o : orders)
        {
            cout << left << setw(10) << o.getOrderID()
                << setw(12) << o.getCustomerID()
                << setw(12) << typeToString(o.getOrderType())
                << setw(14) << statusToString(o.getStatus())
                << setw(12) << fixed << setprecision(2) << o.getTotalPrice() << "\n";
        }
        cout << "\n";
    }

    // ------------------------------------------------------------------------
    // get reference to all orders (for integration with Kitchen/Reports)
    // ------------------------------------------------------------------------
    vector<Order>& getOrders() { return orders; }
};





//Employee class
class Employee
{
protected:
    int employeeID;
    string name;
    string phoneNumber;
    double salary;
    bool isActive;
    EmployeeRole role;

public:
    Employee(int employeeID, string name, string phoneNumber, double salary, bool isActive, EmployeeRole role)
    {
        this->employeeID = employeeID;
        this->name = name;
        this->phoneNumber = phoneNumber;
        this->salary = salary;
        this->isActive = isActive;
        this->role = role;
    }
    int getEmployeeID() { return employeeID; }
    string getName() { return name; }
    bool getIsActive() { return isActive; }
    EmployeeRole getRole() { return role; }
    void setName(string name) { this->name = name; }
    void setPhoneNumber(string phoneNumber) { this->phoneNumber = phoneNumber; }
    void setSalary(double salary) { this->salary = salary; }
    void setIsActive(bool isActive) { this->isActive = isActive; }
    virtual void displayInfo() = 0;
    virtual ~Employee() {}
};


//chef
class Chef : public Employee
{
public:
    Chef(int employeeID, string name, string phoneNumber, double salary, bool isActive)
        : Employee(employeeID, name, phoneNumber, salary, isActive, EmployeeRole::Chef) {
    }
    void displayInfo() override
    {
        cout << "Employee ID: " << employeeID << endl;
        cout << "Name: " << name << endl;
        cout << "Phone: " << phoneNumber << endl;
        cout << "Role: " << employeeRoleToString(role) << endl;
        cout << "Salary: " << salary << endl;
    }
};


//waiter
class Waiter : public Employee
{
public:
    Waiter(int employeeID, string name, string phoneNumber, double salary, bool isActive)
        : Employee(employeeID, name, phoneNumber, salary, isActive, EmployeeRole::Waiter) {
    }
    void displayInfo() override
    {
        cout << "Employee ID: " << employeeID << endl;
        cout << "Name: " << name << endl;
        cout << "Phone: " << phoneNumber << endl;
        cout << "Role: " << employeeRoleToString(role) << endl;
        cout << "Salary: " << salary << endl;
    }
};


//manager
class Manager : public Employee
{
public:
    Manager(int employeeID, string name, string phoneNumber, double salary, bool isActive)
        : Employee(employeeID, name, phoneNumber, salary, isActive, EmployeeRole::Manager) {
    }
    void displayInfo() override
    {
        cout << "Employee ID: " << employeeID << endl;
        cout << "Name: " << name << endl;
        cout << "Phone: " << phoneNumber << endl;
        cout << "Role: " << employeeRoleToString(role) << endl;
        cout << "Salary: " << salary << endl;
    }
};


//cashier
class Cashier : public Employee
{
public:
    Cashier(int employeeID, string name, string phoneNumber, double salary, bool isActive)
        : Employee(employeeID, name, phoneNumber, salary, isActive, EmployeeRole::Cashier) {
    }
    void displayInfo() override
    {
        cout << "Employee ID: " << employeeID << endl;
        cout << "Name: " << name << endl;
        cout << "Phone: " << phoneNumber << endl;
        cout << "Role: " << employeeRoleToString(role) << endl;
        cout << "Salary: " << salary << endl;
    }
};

//delivery driver
class DeliveryDriver : public Employee
{
public:
    DeliveryDriver(int employeeID, string name, string phoneNumber, double salary, bool isActive)
        : Employee(employeeID, name, phoneNumber, salary, isActive, EmployeeRole::DeliveryDriver) {
    }
    void displayInfo() override
    {
        cout << "Employee ID: " << employeeID << endl;
        cout << "Name: " << name << endl;
        cout << "Phone: " << phoneNumber << endl;
        cout << "Role: " << employeeRoleToString(role) << endl;
        cout << "Salary: " << salary << endl;
    }
};


//employee manager
class EmployeeManager
{
private:
    vector<Employee*> employees;

public:
    ~EmployeeManager()
    {
        for (Employee* employee : employees)
        {
            delete employee;
        }
    }

    void displayEmployees()
    {
        bool found = false;
        for (Employee* employee : employees)
        {
            employee->displayInfo();
            cout << "--------------------" << endl;
            found = true;
        }
        if (!found)
        {
            cout << "No employees found." << endl;
        }
    }

    void changeAvailability()
    {
        int id;
        int status;
        cout << "Enter Employee ID: ";
        cin >> id;
        for (Employee* employee : employees)
        {
            if (employee->getEmployeeID() == id)
            {
                cout << "Enter new availability (1 = Available, 0 = Not Available): ";
                cin >> status;
                if (status != 0 && status != 1)
                {
                    cout << "Invalid availability." << endl;
                    return;
                }
                employee->setIsActive(status);
                cout << "Availability updated successfully." << endl;
                return;
            }
        }
        cout << "Employee not found." << endl;
    }

    void searchEmployee()
    {
        int id;
        cout << "Enter Employee ID: ";
        cin >> id;
        for (Employee* employee : employees)
        {
            if (employee->getEmployeeID() == id)
            {
                employee->displayInfo();
                return;
            }
        }
        cout << "Employee not found." << endl;
    }

    bool isEmployeeIDUsed(int id)
    {
        for (Employee* employee : employees)
        {
            if (employee->getEmployeeID() == id)
            {
                return true;
            }
        }
        return false;
    }

    void updateEmployee()
    {
        int id;
        cout << "Enter Employee ID: ";
        cin >> id;
        for (Employee* employee : employees)
        {
            if (employee->getEmployeeID() == id)
            {
                int choice;
                cout << "1. Update Name" << endl;
                cout << "2. Update Phone Number" << endl;
                cout << "3. Update Salary" << endl;
                cin >> choice;
                switch (choice)
                {
                case 1:
                {
                    string name;
                    cout << "Enter new name: ";
                    cin >> name;
                    employee->setName(name);
                    break;
                }
                case 2:
                {
                    string phoneNumber;
                    cout << "Enter new phone number: ";
                    cin >> phoneNumber;
                    employee->setPhoneNumber(phoneNumber);
                    break;
                }
                case 3:
                {
                    double salary;
                    cout << "Enter new salary: ";
                    cin >> salary;
                    employee->setSalary(salary);
                    break;
                }
                default:
                    cout << "Invalid choice." << endl;
                }
                return;
            }
        }
        cout << "Employee not found." << endl;
    }

    void addEmployee()
    {
        int choice;
        cout << "1. Manager" << endl;
        cout << "2. Chef" << endl;
        cout << "3. Waiter" << endl;
        cout << "4. Cashier" << endl;
        cout << "5. Delivery Driver" << endl;
        cin >> choice;
        if (choice < 1 || choice > 5)
        {
            cout << "Invalid choice." << endl;
            return;
        }
        int employeeID;
        string name;
        string phoneNumber;
        double salary;
        bool isActive;
        cout << "Enter Employee ID: ";
        cin >> employeeID;
        if (isEmployeeIDUsed(employeeID))
        {
            cout << "Employee ID already exists." << endl;
            return;
        }
        cout << "Enter Name: ";
        cin >> name;
        cout << "Enter Phone Number: ";
        cin >> phoneNumber;
        cout << "Enter Salary: ";
        cin >> salary;
        cout << "Is Active? (1 = Yes, 0 = No): ";
        cin >> isActive;
        switch (choice)
        {
        case 1:
            employees.push_back(new Manager(employeeID, name, phoneNumber, salary, isActive));
            break;
        case 2:
            employees.push_back(new Chef(employeeID, name, phoneNumber, salary, isActive));
            break;
        case 3:
            employees.push_back(new Waiter(employeeID, name, phoneNumber, salary, isActive));
            break;
        case 4:
            employees.push_back(new Cashier(employeeID, name, phoneNumber, salary, isActive));
            break;
        case 5:
            employees.push_back(new DeliveryDriver(employeeID, name, phoneNumber, salary, isActive));
            break;
        default:
            cout << "Invalid choice." << endl;
        }
    }

    // ========================================================================
    // ADDED — required by the specification, was missing:
    // ========================================================================

    // display all employees of a given role only
    void displayByRole(EmployeeRole role)
    {
        bool found = false;
        for (Employee* employee : employees)
        {
            if (employee->getRole() == role)
            {
                employee->displayInfo();
                cout << "--------------------" << endl;
                found = true;
            }
        }
        if (!found)
        {
            cout << "No employees found for this role." << endl;
        }
    }

    // gives other modules (Reports, Delivery) read access to the employee list
    // without going through cin — needed so Delivery can check a driver is
    // active before assigning them, and so Reports can loop over employees.
    vector<Employee*>& getEmployees() { return employees; }

    // non-interactive lookup by ID — returns nullptr if not found.
    // Same purpose as searchEmployee(), but usable from code (not just from
    // the console), which searchEmployee() couldn't do since it does cin itself.
    Employee* findEmployeeByID(int id)
    {
        for (Employee* employee : employees)
        {
            if (employee->getEmployeeID() == id)
            {
                return employee;
            }
        }
        return nullptr;
    }
};


//kitchen class
class Kitchen
{
private:
    OrderManager& orderManager;

public:
    Kitchen(OrderManager& orderManager) : orderManager(orderManager) {}

    void viewPendingOrders()
    {
        vector<Order>& orders = orderManager.getOrders();
        bool found = false;
        for (Order& order : orders)
        {
            if (order.getStatus() == OrderStatus::New)
            {
                found = true;
                cout << "Order ID: " << order.getOrderID() << endl;
                cout << "Status: New" << endl;
                cout << "--------------------" << endl;
            }
        }
        if (!found)
        {
            cout << "No pending orders." << endl;
        }
    }

    void startPreparing()
    {
        int orderID;
        cout << "Enter Order ID: ";
        cin >> orderID;
        if (orderManager.updateOrderStatus(orderID, OrderStatus::Preparing))
        {
            cout << "Order is now being prepared." << endl;
        }
    }

    void markReady()
    {
        int orderID;
        cout << "Enter Order ID: ";
        cin >> orderID;
        if (orderManager.updateOrderStatus(orderID, OrderStatus::Ready))
        {
            cout << "Order is ready." << endl;
        }
    }

    // ========================================================================
    // ADDED — required by the specification (and the original project spec,
    // section 5.2), was missing:
    // ========================================================================

    // display orders that are currently being prepared / ready to serve
    void displayPreparedOrders()
    {
        vector<Order>& orders = orderManager.getOrders();
        bool found = false;
        for (Order& order : orders)
        {
            if (order.getStatus() == OrderStatus::Preparing || order.getStatus() == OrderStatus::Ready)
            {
                found = true;
                cout << "Order ID: " << order.getOrderID() << endl;
                cout << "Status: " << (order.getStatus() == OrderStatus::Preparing ? "Preparing" : "Ready") << endl;
                cout << "--------------------" << endl;
            }
        }
        if (!found)
        {
            cout << "No orders currently being prepared." << endl;
        }
    }
};

// ============================================================================
// Payment class
// ============================================================================
class Payment {
private:
    int paymentID;
    int orderID;
    double amount;
    PaymentMethod method;
    string paymentDateTime;
    PaymentStatus status;

public:
    Payment(int pID = 0, int oID = 0, double amt = 0.0, PaymentMethod m = PaymentMethod::Cash,
        string dt = "", PaymentStatus s = PaymentStatus::Completed)
        : paymentID(pID), orderID(oID), amount(amt), method(m), paymentDateTime(dt), status(s) {
    }

    // Getters
    int getPaymentID() const { return paymentID; }
    int getOrderID() const { return orderID; }
    double getAmount() const { return amount; }
    PaymentMethod getMethod() const { return method; }
    string getPaymentDateTime() const { return paymentDateTime; }
    PaymentStatus getStatus() const { return status; }

    // Setter (status is the only thing that legitimately changes after creation)
    void setStatus(PaymentStatus s) { status = s; }

    // Display
    void displayInfo() const {
        cout << "\n--- Payment #" << paymentID << " ---\n";
        cout << "Order ID   : " << orderID << "\n";
        cout << "Amount     : " << fixed << setprecision(2) << amount << "\n";
        cout << "Method     : " << paymentMethodToString(method) << "\n";
        cout << "Date/Time  : " << paymentDateTime << "\n";
        cout << "Status     : " << paymentStatusToString(status) << "\n";
    }
};

// ============================================================================
// PaymentManager class
// ============================================================================
class PaymentManager {
private:
    vector<Payment> payments;
    int nextPaymentID;

public:
    PaymentManager() : nextPaymentID(3000) {}

    // ------------------------------------------------------------------------
    // process a payment for an order, returns new paymentID or -1 on failure
    // ------------------------------------------------------------------------
    int processPayment(int orderID, double amount, PaymentMethod method) {
        if (orderID <= 0) {
            cout << "Error: invalid order ID.\n";
            return -1;
        }
        if (amount <= 0) {
            cout << "Error: amount must be greater than zero.\n";
            return -1;
        }

        for (const auto& existing : payments) {
            if (existing.getOrderID() == orderID &&
                existing.getStatus() == PaymentStatus::Completed) {
                cout << "Error: this order has already been paid.\n";
                return -1;
            }
        }

        Payment p(nextPaymentID, orderID, amount, method, getCurrentDateTime(), PaymentStatus::Completed);
        payments.push_back(p);
        cout << "Payment recorded successfully! Payment ID: " << nextPaymentID << "\n";
        nextPaymentID++;
        return p.getPaymentID();
    }

    // ------------------------------------------------------------------------
    // display details of one payment
    // ------------------------------------------------------------------------
    void displayPaymentDetails(int paymentID) {
        Payment* p = searchPayment(paymentID);
        if (!p) {
            cout << "Error: payment not found.\n";
            return;
        }
        p->displayInfo();
    }

    // ------------------------------------------------------------------------
    // refund a payment
    // ------------------------------------------------------------------------
    bool refundPayment(int paymentID) {
        Payment* p = searchPayment(paymentID);
        if (!p) {
            cout << "Error: payment not found.\n";
            return false;
        }
        if (p->getStatus() == PaymentStatus::Refunded) {
            cout << "Error: payment is already refunded.\n";
            return false;
        }
        p->setStatus(PaymentStatus::Refunded);
        cout << "Payment " << paymentID << " refunded successfully.\n";
        return true;
    }

    // ------------------------------------------------------------------------
    // generate and print a simple receipt
    // ------------------------------------------------------------------------
    void generateReceipt(int paymentID) {
        Payment* p = searchPayment(paymentID);
        if (!p) {
            cout << "Error: payment not found.\n";
            return;
        }
        cout << "\n========== RECEIPT ==========\n";
        cout << "Payment ID : " << p->getPaymentID() << "\n";
        cout << "Order ID   : " << p->getOrderID() << "\n";
        cout << "Amount Paid: " << fixed << setprecision(2) << p->getAmount() << "\n";
        cout << "Method     : " << paymentMethodToString(p->getMethod()) << "\n";
        cout << "Date/Time  : " << p->getPaymentDateTime() << "\n";
        cout << "==============================\n";
    }

    // ------------------------------------------------------------------------
    // view full payment history
    // ------------------------------------------------------------------------
    void viewPaymentHistory() {
        if (payments.empty()) {
            cout << "No payments recorded yet.\n";
            return;
        }
        cout << "\n=== Payment History ===\n";
        cout << left << setw(10) << "Pay#"
            << setw(10) << "Order#"
            << setw(12) << "Amount"
            << setw(16) << "Method"
            << setw(12) << "Status" << "\n";
        cout << string(60, '-') << "\n";
        for (const auto& p : payments) {
            cout << left << setw(10) << p.getPaymentID()
                << setw(10) << p.getOrderID()
                << setw(12) << fixed << setprecision(2) << p.getAmount()
                << setw(16) << paymentMethodToString(p.getMethod())
                << setw(12) << paymentStatusToString(p.getStatus()) << "\n";
        }
        cout << "\n";
    }

    // ------------------------------------------------------------------------
    // search payment by ID
    // ------------------------------------------------------------------------
    Payment* searchPayment(int paymentID) {
        for (auto& p : payments) {
            if (p.getPaymentID() == paymentID)
                return &p;
        }
        return nullptr;
    }

    // ADDED — needed by ReportManager
    vector<Payment>& getPayments() { return payments; }
};

// ============================================================================
// Delivery class
// ============================================================================
class Delivery {
private:
    int deliveryID;
    int orderID;
    int customerID;
    string deliveryAddress;
    int driverEmployeeID;
    DeliveryStatus status;
    string estimatedTime;
    string actualDeliveryTime;
    double deliveryFee;

public:
    Delivery(int dID = 0, int oID = 0, int cID = 0, string addr = "", int driverID = 0,
        DeliveryStatus s = DeliveryStatus::Assigned, string est = "", string actual = "", double fee = 0.0)
        : deliveryID(dID), orderID(oID), customerID(cID), deliveryAddress(addr),
        driverEmployeeID(driverID), status(s), estimatedTime(est), actualDeliveryTime(actual), deliveryFee(fee) {
    }

    // Getters
    int getDeliveryID() const { return deliveryID; }
    int getOrderID() const { return orderID; }
    int getCustomerID() const { return customerID; }
    string getDeliveryAddress() const { return deliveryAddress; }
    int getDriverEmployeeID() const { return driverEmployeeID; }
    DeliveryStatus getStatus() const { return status; }
    string getEstimatedTime() const { return estimatedTime; }
    string getActualDeliveryTime() const { return actualDeliveryTime; }
    double getDeliveryFee() const { return deliveryFee; }

    // Setters
    void setStatus(DeliveryStatus s) { status = s; }
    void setActualDeliveryTime(string t) { actualDeliveryTime = t; }

    // Display
    void displayInfo() const {
        cout << "\n--- Delivery #" << deliveryID << " ---\n";
        cout << "Order ID    : " << orderID << "\n";
        cout << "Customer ID : " << customerID << "\n";
        cout << "Address     : " << deliveryAddress << "\n";
        cout << "Driver ID   : " << driverEmployeeID << "\n";
        cout << "Status      : " << deliveryStatusToString(status) << "\n";
        cout << "Est. Time   : " << estimatedTime << "\n";
        cout << "Fee         : " << fixed << setprecision(2) << deliveryFee << "\n";
    }
};

// ============================================================================
// DeliveryManager class
// ============================================================================
class DeliveryManager {
private:
    vector<Delivery> deliveries;
    int nextDeliveryID;

public:
    DeliveryManager() : nextDeliveryID(5000) {}

    // ------------------------------------------------------------------------
    // assign a delivery to a driver, returns new deliveryID or -1 on failure
    // ------------------------------------------------------------------------
    int assignDelivery(int orderID, int driverEmployeeID, string address, int customerID = 0) {
        if (orderID <= 0) {
            cout << "Error: invalid order ID.\n";
            return -1;
        }
        if (driverEmployeeID <= 0) {
            cout << "Error: invalid driver employee ID.\n";
            return -1;
        }
        if (address.empty()) {
            cout << "Error: delivery address cannot be empty.\n";
            return -1;
        }

        // NOTE: checking that the driver is actually free/active happens during
        // integration, once Eman's EmployeeManager is connected.

        double fee = 25.0; // flat fee for now — can be made distance-based later
        Delivery d(nextDeliveryID, orderID, customerID, address, driverEmployeeID,
            DeliveryStatus::Assigned, "35 minutes", "", fee);
        deliveries.push_back(d);
        cout << "Delivery assigned successfully! Delivery ID: " << nextDeliveryID << "\n";
        nextDeliveryID++;
        return d.getDeliveryID();
    }

    // ------------------------------------------------------------------------
    // track (display) a single delivery's status
    // ------------------------------------------------------------------------
    void trackDeliveryStatus(int deliveryID) {
        Delivery* d = searchDelivery(deliveryID);
        if (!d) {
            cout << "Error: delivery not found.\n";
            return;
        }
        d->displayInfo();
    }

    // ------------------------------------------------------------------------
    // update delivery status
    // ------------------------------------------------------------------------
    bool updateDeliveryStatus(int deliveryID, DeliveryStatus newStatus) {
        Delivery* d = searchDelivery(deliveryID);
        if (!d) {
            cout << "Error: delivery not found.\n";
            return false;
        }
        if (d->getStatus() == DeliveryStatus::Delivered || d->getStatus() == DeliveryStatus::Cancelled) {
            cout << "Error: cannot change status of a delivery that is already '"
                << deliveryStatusToString(d->getStatus()) << "'.\n";
            return false;
        }

        DeliveryStatus current = d->getStatus();
        bool validTransition =
            (current == DeliveryStatus::Assigned && (newStatus == DeliveryStatus::OnTheWay || newStatus == DeliveryStatus::Cancelled)) ||
            (current == DeliveryStatus::OnTheWay && (newStatus == DeliveryStatus::Delivered || newStatus == DeliveryStatus::Cancelled));
        if (!validTransition && newStatus != current) {
            cout << "Error: invalid delivery status transition from '"
                << deliveryStatusToString(current) << "' to '"
                << deliveryStatusToString(newStatus) << "'.\n";
            return false;
        }

        d->setStatus(newStatus);
        if (newStatus == DeliveryStatus::Delivered) {
            d->setActualDeliveryTime(getCurrentDateTime());
        }
        cout << "Delivery status updated to " << deliveryStatusToString(newStatus) << ".\n";
        return true;
    }

    // ------------------------------------------------------------------------
    // view all deliveries
    // ------------------------------------------------------------------------
    void viewAllDeliveries() {
        if (deliveries.empty()) {
            cout << "No deliveries yet.\n";
            return;
        }
        cout << "\n=== All Deliveries ===\n";
        cout << left << setw(10) << "Del#"
            << setw(10) << "Order#"
            << setw(10) << "Driver#"
            << setw(14) << "Status"
            << setw(10) << "Fee" << "\n";
        cout << string(56, '-') << "\n";
        for (const auto& d : deliveries) {
            cout << left << setw(10) << d.getDeliveryID()
                << setw(10) << d.getOrderID()
                << setw(10) << d.getDriverEmployeeID()
                << setw(14) << deliveryStatusToString(d.getStatus())
                << setw(10) << fixed << setprecision(2) << d.getDeliveryFee() << "\n";
        }
        cout << "\n";
    }

    // ------------------------------------------------------------------------
    // calculate delivery fee
    // ------------------------------------------------------------------------
    double calculateDeliveryFee(int deliveryID) {
        Delivery* d = searchDelivery(deliveryID);
        if (!d) return -1.0;
        return d->getDeliveryFee();
    }

    // ------------------------------------------------------------------------
    // search delivery by ID
    // ------------------------------------------------------------------------
    Delivery* searchDelivery(int deliveryID) {
        for (auto& d : deliveries) {
            if (d.getDeliveryID() == deliveryID)
                return &d;
        }
        return nullptr;
    }

    // ADDED — needed by ReportManager
    vector<Delivery>& getDeliveries() { return deliveries; }
};





// ============================================================================
// ReportManager class 
// Reads data from OrderManager, PaymentManager, EmployeeManager, DeliveryManager.
// Built last because it needs everyone else's data to already exist.
// ============================================================================
class ReportManager {
private:
    OrderManager& orderManager;
    PaymentManager& paymentManager;
    EmployeeManager& employeeManager;
    DeliveryManager& deliveryManager;

public:
    ReportManager(OrderManager& om, PaymentManager& pm, EmployeeManager& em, DeliveryManager& dm)
        : orderManager(om), paymentManager(pm), employeeManager(em), deliveryManager(dm) {
    }

    // date format expected: "YYYY-MM-DD" (matches the first 10 chars of orderDateTime)
    void generateDailySalesReport(string date) {
        vector<Order>& orders = orderManager.getOrders();
        int totalOrders = 0;
        double totalRevenue = 0.0;
        for (auto& o : orders) {
            if (o.getOrderDateTime().substr(0, 10) == date && o.getStatus() != OrderStatus::Cancelled) {
                totalOrders++;
                totalRevenue += o.getTotalPrice();
            }
        }
        cout << "\n----- Daily Sales Report (" << date << ") -----\n";
        cout << "Total Orders  : " << totalOrders << "\n";
        cout << "Total Revenue : " << fixed << setprecision(2) << totalRevenue << "\n";
    }

    // month format expected: "YYYY-MM"
    void generateMonthlyRevenueReport(string month) {
        vector<Order>& orders = orderManager.getOrders();
        int count = 0;
        double totalRevenue = 0.0;
        for (auto& o : orders) {
            if (o.getOrderDateTime().substr(0, 7) == month && o.getStatus() != OrderStatus::Cancelled) {
                count++;
                totalRevenue += o.getTotalPrice();
            }
        }
        cout << "\n----- Monthly Revenue Report (" << month << ") -----\n";
        cout << "Total Orders  : " << count << "\n";
        cout << "Total Revenue : " << fixed << setprecision(2) << totalRevenue << "\n";
    }

    void generateTopSellingItemsReport() {
        vector<Order>& orders = orderManager.getOrders();
        vector<int> ids;
        vector<string> names;
        vector<int> qtys;

        for (auto& o : orders) {
            if (o.getStatus() == OrderStatus::Cancelled) continue;
            for (auto& it : o.getItems()) {
                bool found = false;
                for (size_t i = 0; i < ids.size(); i++) {
                    if (ids[i] == it.itemID) { qtys[i] += it.quantity; found = true; break; }
                }
                if (!found) { ids.push_back(it.itemID); names.push_back(it.itemName); qtys.push_back(it.quantity); }
            }
        }

        // simple selection sort, descending by quantity sold
        for (size_t i = 0; i < qtys.size(); i++) {
            size_t maxIdx = i;
            for (size_t j = i + 1; j < qtys.size(); j++)
                if (qtys[j] > qtys[maxIdx]) maxIdx = j;
            swap(qtys[i], qtys[maxIdx]);
            swap(ids[i], ids[maxIdx]);
            swap(names[i], names[maxIdx]);
        }

        cout << "\n----- Top Selling Items -----\n";
        if (qtys.empty()) {
            cout << "No items sold yet.\n";
            return;
        }
        for (size_t i = 0; i < qtys.size(); i++) {
            cout << left << setw(4) << (i + 1) << setw(24) << names[i] << "Qty Sold: " << qtys[i] << "\n";
        }
    }

    // NOTE: the current data model only links a delivery driver to an order
    // (via driverEmployeeID), so this report can only measure completed
    // deliveries per employee — not waiter/cashier order handling, since
    // Order does not store which staff member served it.
    void generateEmployeePerformanceReport() {
        vector<Employee*>& employees = employeeManager.getEmployees();
        vector<Delivery>& deliveries = deliveryManager.getDeliveries();

        cout << "\n----- Employee Performance Report -----\n";
        if (employees.empty()) {
            cout << "No employees yet.\n";
            return;
        }
        for (auto* e : employees) {
            int completedDeliveries = 0;
            for (auto& d : deliveries) {
                if (d.getDriverEmployeeID() == e->getEmployeeID() && d.getStatus() == DeliveryStatus::Delivered)
                    completedDeliveries++;
            }
            cout << "Employee ID: " << e->getEmployeeID()
                << " | Name: " << e->getName()
                << " | Completed Deliveries: " << completedDeliveries << "\n";
        }
    }

    void generateDeliveryReport() {
        vector<Delivery>& deliveries = deliveryManager.getDeliveries();
        cout << "\n----- Delivery Report -----\n";
        if (deliveries.empty()) {
            cout << "No deliveries yet.\n";
            return;
        }
        int assigned = 0, onWay = 0, delivered = 0, cancelled = 0;
        for (auto& d : deliveries) {
            switch (d.getStatus()) {
            case DeliveryStatus::Assigned:   assigned++;  break;
            case DeliveryStatus::OnTheWay:   onWay++;     break;
            case DeliveryStatus::Delivered:  delivered++; break;
            case DeliveryStatus::Cancelled:  cancelled++; break;
            }
        }
        cout << "Total Deliveries : " << deliveries.size() << "\n";
        cout << "Assigned         : " << assigned << "\n";
        cout << "On The Way       : " << onWay << "\n";
        cout << "Delivered        : " << delivered << "\n";
        cout << "Cancelled        : " << cancelled << "\n";
    }
};

// ============================================================================
// RestaurantSystem class — Owner: Kareem
// Owns one instance of every manager and runs the main menu.
// This is the only class allowed to make different managers work together.
// ============================================================================
class RestaurantSystem {
private:
    CustomerManager customerManager;
    TableManager tableManager;
    MenuManager menuManager;
    ReservationManager reservationManager;
    OrderManager orderManager;
    EmployeeManager employeeManager;
    Kitchen kitchen;
    PaymentManager paymentManager;
    DeliveryManager deliveryManager;
    ReportManager reportManager;

    void menuTitle(const string& text)
    {
        UI::title(text);
    }

    void pause()
    {
        UI::pause();
    }

    Customer* readCustomerByID()
    {
        while (true)
        {
            int id = UI::readInt("Enter Customer ID: ", 1, 2147483647);
            Customer* customer = customerManager.searchCustomer(id);

            if (customer)
                return customer;

            UI::error("Customer not found.");
            cout << "Please enter a valid Customer ID.\n";
        }
    }

    Table* readTableByID()
    {
        while (true)
        {
            int id = UI::readInt("Enter Table ID: ", 1, 2147483647);
            Table* table = tableManager.searchTable(id);

            if (table)
                return table;

            UI::error("Table not found.");
            cout << "Please enter a valid Table ID.\n";
        }
    }

    MenuItem* readMenuItemByID()
    {
        while (true)
        {
            int id = UI::readInt("Enter Item ID: ", 1, 2147483647);
            MenuItem* item = menuManager.searchItem(id);

            if (item)
                return item;

            UI::error("Menu item not found.");
            cout << "Please enter a valid Item ID.\n";
        }
    }

    Order* readOrderByID()
    {
        while (true)
        {
            int id = UI::readInt("Enter Order ID: ", 1, 2147483647);
            Order* order = orderManager.searchOrder(id);

            if (order)
                return order;

            UI::error("Order not found.");
            cout << "Please enter a valid Order ID.\n";
        }
    }

    Reservation* readReservationByID()
    {
        while (true)
        {
            int id = UI::readInt("Enter Reservation ID: ", 1, 2147483647);
            Reservation* reservation = reservationManager.searchReservation(id);

            if (reservation)
                return reservation;

            UI::error("Reservation not found.");
            cout << "Please enter a valid Reservation ID.\n";
        }
    }

    Employee* readEmployeeByID()
    {
        while (true)
        {
            int id = UI::readInt("Enter Employee ID: ", 1, 2147483647);
            Employee* employee = employeeManager.findEmployeeByID(id);

            if (employee)
                return employee;

            UI::error("Employee not found.");
            cout << "Please enter a valid Employee ID.\n";
        }
    }

    Payment* readPaymentByID()
    {
        while (true)
        {
            int id = UI::readInt("Enter Payment ID: ", 1, 2147483647);
            Payment* payment = paymentManager.searchPayment(id);

            if (payment)
                return payment;

            UI::error("Payment not found.");
            cout << "Please enter a valid Payment ID.\n";
        }
    }

    Delivery* readDeliveryByID()
    {
        while (true)
        {
            int id = UI::readInt("Enter Delivery ID: ", 1, 2147483647);
            Delivery* delivery = deliveryManager.searchDelivery(id);

            if (delivery)
                return delivery;

            UI::error("Delivery not found.");
            cout << "Please enter a valid Delivery ID.\n";
        }
    }

public:
    RestaurantSystem()
        : kitchen(orderManager),
        reportManager(orderManager, paymentManager, employeeManager, deliveryManager)
    {
    }

    void run()
    {
        while (true)
        {
            clearScreen();
            menuTitle("RESTAURANT MANAGEMENT SYSTEM");

            cout << "1. Customer Management\n"
                << "2. Table Management\n"
                << "3. Menu Management\n"
                << "4. Reservation Management\n"
                << "5. Order Management\n"
                << "6. Employee Management\n"
                << "7. Kitchen Management\n"
                << "8. Payment Management\n"
                << "9. Delivery Management\n"
                << "10. Reports\n"
                << "0. Exit\n";

            int choice = UI::readInt("Choice: ", 0, 10);

            if (choice == 0)
            {
                UI::success("Goodbye!");
                return;
            }

            switch (choice)
            {
            case 1:
                customerMenu();
                break;
            case 2:
                tableMenu();
                break;
            case 3:
                menuMenu();
                break;
            case 4:
                reservationMenu();
                break;
            case 5:
                orderMenu();
                break;
            case 6:
                employeeMenu();
                break;
            case 7:
                kitchenMenu();
                break;
            case 8:
                paymentMenu();
                break;
            case 9:
                deliveryMenu();
                break;
            case 10:
                reportsMenu();
                break;
            }
        }
    }

private:
    void customerMenu()
    {
        while (true)
        {
            clearScreen();
            menuTitle("CUSTOMER MANAGEMENT");
            UI::breadcrumb("Customer Management");

            cout << "1. Add Customer\n"
                << "2. Display All\n"
                << "3. Search Customer\n"
                << "4. Update Customer\n"
                << "0. Back\n";

            int choice = UI::readInt("Choice: ", 0, 4);

            if (choice == 0)
                return;

            clearScreen();

            if (choice == 1)
            {
                int id = UI::readInt("Enter Customer ID: ", 1, 2147483647);

                if (customerManager.searchCustomer(id))
                {
                    UI::error("Customer ID already exists.");
                    pause();
                    continue;
                }

                string name = UI::readRequired("Enter Name: ");
                string phone = UI::readPhone("Enter Phone: ");

                if (customerManager.isPhoneUsed(phone))
                {
                    UI::error("Phone number already exists.");
                    pause();
                    continue;
                }

                string address = UI::readRequired("Enter Address: ");
                string email = UI::readEmail("Enter Email: ");

                if (customerManager.isEmailUsed(email))
                {
                    UI::error("Email already exists.");
                    pause();
                    continue;
                }

                customerManager.addCustomer(Customer(id, name, phone, address, email));
                pause();
            }
            else if (choice == 2)
            {
                menuTitle("CUSTOMERS");
                customerManager.displayAllCustomers();
                pause();
            }
            else if (choice == 3)
            {
                menuTitle("SEARCH CUSTOMER BY");
                cout << "1. Customer ID\n"
                    << "2. Phone Number\n"
                    << "3. Email\n"
                    << "0. Back\n";

                int searchChoice = UI::readInt("Choice: ", 0, 3);
                Customer* customer = nullptr;

                if (searchChoice == 1)
                {
                    customer = readCustomerByID();
                }
                else if (searchChoice == 2)
                {
                    string phone = UI::readPhone("Enter Phone: ");
                    customer = customerManager.searchCustomerByPhone(phone);
                }
                else if (searchChoice == 3)
                {
                    string email = UI::readEmail("Enter Email: ");
                    customer = customerManager.searchCustomerByEmail(email);
                }

                if (searchChoice != 0)
                {
                    if (customer)
                    {
                        cout << left
                            << setw(8) << "ID"
                            << setw(24) << "Name"
                            << setw(18) << "Phone"
                            << setw(32) << "Address"
                            << setw(30) << "Email"
                            << "\n"
                            << string(112, '-') << "\n";

                        cout << left
                            << setw(8) << customer->getCustomerID()
                            << setw(24) << customer->getName()
                            << setw(18) << customer->getPhoneNumber()
                            << setw(32) << customer->getAddress()
                            << setw(30) << customer->getEmail()
                            << "\n";
                    }
                    else
                    {
                        UI::error("Customer not found.");
                    }

                    pause();
                }
            }
            else if (choice == 4)
            {
                UI::breadcrumb("Customer Management > Update Customer");

                while (true)
                {
                    int id = UI::readInt("Enter Customer ID: ", 1, 2147483647);

                    if (!customerManager.searchCustomer(id))
                    {
                        UI::error("Customer not found.");
                        cout << "Please enter a valid Customer ID.\n";
                        continue;
                    }

                    cout << "1. Update Name\n"
                        << "2. Update Phone Number\n"
                        << "3. Update Address\n"
                        << "4. Update Email\n"
                        << "5. Update All Information\n"
                        << "0. Back/Cancel\n";

                    int field = UI::readInt("Choice: ", 0, 5);

                    if (field == 0)
                        break;

                    Customer* customer = customerManager.searchCustomer(id);

                    if (field == 1)
                    {
                        customer->setName(UI::readRequired("New Name: "));
                        UI::success("Customer name updated successfully.");
                    }
                    else if (field == 2)
                    {
                        string phone = UI::readPhone("New Phone: ");

                        if (customerManager.isPhoneUsed(phone, id))
                        {
                            UI::error("Phone number already exists.");
                            continue;
                        }

                        customer->setPhoneNumber(phone);
                        UI::success("Customer phone number updated successfully.");
                    }
                    else if (field == 3)
                    {
                        customer->setAddress(UI::readRequired("New Address: "));
                        UI::success("Customer address updated successfully.");
                    }
                    else if (field == 4)
                    {
                        string email = UI::readEmail("New Email: ");

                        if (customerManager.isEmailUsed(email, id))
                        {
                            UI::error("Email already exists.");
                            continue;
                        }

                        customer->setEmail(email);
                        UI::success("Customer email updated successfully.");
                    }
                    else
                    {
                        string name = UI::readRequired("New Name: ");
                        string phone = UI::readPhone("New Phone: ");

                        if (customerManager.isPhoneUsed(phone, id))
                        {
                            UI::error("Phone number already exists.");
                            continue;
                        }

                        string address = UI::readRequired("New Address: ");
                        string email = UI::readEmail("New Email: ");

                        if (customerManager.isEmailUsed(email, id))
                        {
                            UI::error("Email already exists.");
                            continue;
                        }

                        customerManager.updateCustomer(id, name, phone, address, email);
                    }

                    pause();
                    break;
                }
            }
        }
    }

    void tableMenu()
    {
        while (true)
        {
            clearScreen();
            menuTitle("TABLE MANAGEMENT");
            UI::breadcrumb("Table Management");

            cout << "1. Add Table\n"
                << "2. Display Available\n"
                << "3. Search Table\n"
                << "4. Update Status\n"
                << "5. Assign Table\n"
                << "0. Back\n";

            int choice = UI::readInt("Choice: ", 0, 5);

            if (choice == 0)
                return;

            clearScreen();

            if (choice == 1)
            {
                int id = UI::readInt("Enter Table ID: ", 1, 2147483647);

                if (tableManager.searchTable(id))
                {
                    UI::error("Table ID already exists.");
                    pause();
                    continue;
                }

                int capacity = UI::readInt("Enter Capacity: ", 1, 1000);
                string location = UI::readRequired("Enter Location: ");

                if (location.size() < 3)
                {
                    UI::error("Location must contain at least 3 characters.");
                    pause();
                    continue;
                }

                if (tableManager.addTable(
                    Table(id, capacity, location, TableStatus::Available)))
                {
                    UI::success("Table added successfully.");
                }
                else
                {
                    UI::error("Could not add table.");
                }

                pause();
            }
            else if (choice == 2)
            {
                tableManager.displayAvailableTables();
                pause();
            }
            else if (choice == 3)
            {
                UI::breadcrumb("Table Management > Search Table");
                while (true)
                {
                    int id = UI::readInt("Enter Table ID: ", 1, 2147483647);
                    Table* table = tableManager.searchTable(id);

                    if (!table)
                    {
                        UI::error("Table not found.");
                        cout << "Please enter a valid Table ID.\n";
                        continue;
                    }

                    cout << "ID: " << table->getTableID()
                        << " | Capacity: " << table->getCapacity()
                        << " | Location: " << table->getLocation()
                        << " | Status: " << table->getStatusString() << "\n";
                    break;
                }
                pause();
            }
            else if (choice == 4)
            {
                UI::breadcrumb("Table Management > Update Status");
                while (true)
                {
                    int id = UI::readInt("Enter Table ID: ", 1, 2147483647);

                    if (!tableManager.searchTable(id))
                    {
                        UI::error("Table not found.");
                        cout << "Please enter a valid Table ID.\n";
                        continue;
                    }

                    int status = UI::readInt(
                        "New Status (0=Available,1=Reserved,2=Occupied,3=OutOfService): ",
                        0, 3);

                    if (tableManager.updateTableStatus(id, (TableStatus)status))
                        UI::success("Table status updated successfully.");
                    break;
                }
                pause();
            }
            else if (choice == 5)
            {
                UI::breadcrumb("Table Management > Assign Table");
                while (true)
                {
                    int id = UI::readInt("Enter Table ID: ", 1, 2147483647);

                    if (!tableManager.searchTable(id))
                    {
                        UI::error("Table not found.");
                        cout << "Please enter a valid Table ID.\n";
                        continue;
                    }

                    tableManager.assignTable(id);
                    break;
                }
                pause();
            }
        }
    }

    void menuMenu()
    {
        while (true)
        {
            clearScreen();
            menuTitle("MENU MANAGEMENT");
            UI::breadcrumb("Menu Management");

            cout << "1. Add Item\n"
                << "2. Remove Item\n"
                << "3. Search Item\n"
                << "4. Display Menu\n"
                << "5. Change Availability\n"
                << "0. Back\n";

            int choice = UI::readInt("Choice: ", 0, 5);

            if (choice == 0)
                return;

            clearScreen();

            if (choice == 1)
            {
                int id = UI::readInt("Enter Item ID: ", 1, 2147483647);

                if (menuManager.searchItem(id))
                {
                    UI::error("Item ID already exists.");
                    pause();
                    continue;
                }

                double price = UI::readDouble("Enter Price: ", 0.01);
                int category = UI::readInt(
                    "Category (0=Appetizer,1=MainCourse,2=Dessert,3=Drink): ", 0, 3);
                string name = UI::readRequired("Enter Name: ");

                if (name.size() < 3)
                {
                    UI::error("Name must contain at least 3 characters.");
                    pause();
                    continue;
                }

                string description = UI::readRequired("Enter Description: ");

                if (description.size() < 3)
                {
                    UI::error("Description must contain at least 3 characters.");
                    pause();
                    continue;
                }

                if (menuManager.addItem(
                    MenuItem(id, name, price, (MenuCategory)category, description, true)))
                {
                    UI::success("Item added successfully.");
                }
                else
                {
                    UI::error("Could not add item.");
                }

                pause();
            }
            else if (choice == 2)
            {
                UI::breadcrumb("Menu Management > Remove Item");
                MenuItem* item = readMenuItemByID();

                if (menuManager.removeItem(item->getItemID()))
                    UI::success("Item removed successfully.");

                pause();
            }
            else if (choice == 3)
            {
                MenuItem* item = readMenuItemByID();

                if (item)
                {
                    cout << "ID: " << item->getItemID()
                        << " | " << item->getName()
                        << " | Price: " << fixed << setprecision(2) << item->getPrice()
                        << " | " << item->getCategoryString() << "\n";
                }
                else
                {
                    UI::error("Item not found.");
                }

                pause();
            }
            else if (choice == 4)
            {
                menuManager.displayMenu();
                pause();
            }
            else if (choice == 5)
            {
                UI::breadcrumb("Menu Management > Change Availability");
                MenuItem* item = readMenuItemByID();
                int available = UI::readInt("Available? (1=Yes,0=No): ", 0, 1);

                if (menuManager.changeAvailability(item->getItemID(), available == 1))
                    UI::success("Availability updated successfully.");

                pause();
            }
        }
    }

    void reservationMenu()
    {
        while (true)
        {
            clearScreen();
            menuTitle("RESERVATION MANAGEMENT");
            UI::breadcrumb("Reservation Management");

            cout << "1. Create Reservation\n"
                << "2. Confirm\n"
                << "3. Cancel\n"
                << "4. Display All\n"
                << "0. Back\n";

            int choice = UI::readInt("Choice: ", 0, 4);

            if (choice == 0)
                return;

            clearScreen();

            if (choice == 1)
            {
                UI::breadcrumb("Reservation Management > Create Reservation");
                int reservationId = UI::readInt("Enter Reservation ID: ", 1, 2147483647);

                if (reservationManager.searchReservation(reservationId))
                {
                    UI::error("Reservation ID already exists.");
                    pause();
                    continue;
                }

                UI::breadcrumb("Order Management > Create Order");
                int customerId = UI::readInt("Enter Customer ID: ", 1, 2147483647);

                if (!customerManager.searchCustomer(customerId))
                {
                    UI::error("Customer not found.");
                    pause();
                    continue;
                }

                int tableId = UI::readInt("Enter Table ID: ", 1, 2147483647);
                Table* table = tableManager.searchTable(tableId);

                if (!table)
                {
                    UI::error("Table not found.");
                    pause();
                    continue;
                }

                if (!tableManager.isTableAvailable(tableId))
                {
                    UI::error("Selected table is not available.");
                    pause();
                    continue;
                }

                int guests = UI::readInt("Number of Guests: ", 1, 1000);

                if (guests > table->getCapacity())
                {
                    UI::error("Number of guests exceeds table capacity.");
                    pause();
                    continue;
                }

                string date = UI::readDate("Enter Date (YYYY-MM-DD): ");
                string time = UI::readTime("Enter Time (HH:MM): ");

                reservationManager.createReservation(
                    Reservation(reservationId, customerId, tableId,
                        date, time, guests, ReservationStatus::Pending));

                pause();
            }
            else if (choice == 2)
            {
                UI::breadcrumb("Reservation Management > Confirm Reservation");
                while (true)
                {
                    int id = UI::readInt("Enter Reservation ID: ", 1, 2147483647);
                    if (!reservationManager.searchReservation(id))
                    {
                        UI::error("Reservation not found.");
                        cout << "Please enter a valid Reservation ID.\n";
                        continue;
                    }
                    reservationManager.confirmReservation(id);
                    break;
                }
                pause();
            }
            else if (choice == 3)
            {
                UI::breadcrumb("Reservation Management > Cancel Reservation");
                while (true)
                {
                    int id = UI::readInt("Enter Reservation ID: ", 1, 2147483647);
                    if (!reservationManager.searchReservation(id))
                    {
                        UI::error("Reservation not found.");
                        cout << "Please enter a valid Reservation ID.\n";
                        continue;
                    }
                    reservationManager.cancelReservation(id);
                    break;
                }
                pause();
            }
            else if (choice == 4)
            {
                reservationManager.displayReservations();
                pause();
            }
        }
    }

    void orderMenu()
    {
        while (true)
        {
            clearScreen();
            menuTitle("ORDER MANAGEMENT");
            UI::breadcrumb("Order Management");

            cout << "1. Create Order\n"
                << "2. Add Item\n"
                << "3. Update Status\n"
                << "4. Cancel Order\n"
                << "5. Display Order\n"
                << "6. List All\n"
                << "0. Back\n";

            int choice = UI::readInt("Choice: ", 0, 6);

            if (choice == 0)
                return;

            clearScreen();

            if (choice == 1)
            {
                int customerId = UI::readInt("Enter Customer ID: ", 1, 2147483647);

                if (!customerManager.searchCustomer(customerId))
                {
                    UI::error("Customer not found.");
                    pause();
                    continue;
                }

                int type = UI::readInt(
                    "Order Type (0=DineIn,1=Takeaway,2=Delivery): ", 0, 2);
                int tableId = 0;

                if (type == 0)
                {
                    tableId = UI::readInt("Enter Table ID: ", 1, 2147483647);

                    if (!tableManager.searchTable(tableId))
                    {
                        UI::error("Table not found.");
                        pause();
                        continue;
                    }

                    if (!tableManager.isTableAvailable(tableId))
                    {
                        UI::error("Selected table is not available.");
                        pause();
                        continue;
                    }
                }

                int orderId = orderManager.createOrder(
                    customerId, tableId, (OrderType)type);

                if (orderId != -1)
                    UI::success("Order created successfully. ID: " + to_string(orderId));

                pause();
            }
            else if (choice == 2)
            {
                int orderId = UI::readInt("Enter Order ID: ", 1, 2147483647);

                if (!orderManager.searchOrder(orderId))
                {
                    UI::error("Order not found.");
                    pause();
                    continue;
                }

                int itemId = UI::readInt("Enter Item ID: ", 1, 2147483647);
                MenuItem* menuItem = menuManager.searchItem(itemId);

                if (!menuItem)
                {
                    UI::error("Menu item not found.");
                    pause();
                    continue;
                }

                if (!menuItem->getIsAvailable())
                {
                    UI::error("This menu item is unavailable.");
                    pause();
                    continue;
                }

                int quantity = UI::readInt("Enter Quantity: ", 1, 1000000);

                if (orderManager.addItem(
                    orderId,
                    OrderItem(itemId, menuItem->getName(),
                        quantity, menuItem->getPrice())))
                {
                    UI::success("Item added to order.");
                }

                pause();
            }
            else if (choice == 3)
            {
                UI::breadcrumb("Order Management > Update Status");
                while (true)
                {
                    int orderId = UI::readInt("Enter Order ID: ", 1, 2147483647);
                    if (!orderManager.searchOrder(orderId))
                    {
                        UI::error("Order not found.");
                        cout << "Please enter a valid Order ID.\n";
                        continue;
                    }

                    int status = UI::readInt(
                        "New Status (0=New,1=Preparing,2=Ready,3=Served,4=Completed,5=Cancelled): ",
                        0, 5);

                    if (orderManager.updateOrderStatus(orderId, (OrderStatus)status))
                        UI::success("Order status updated successfully.");
                    break;
                }
                pause();
            }
            else if (choice == 4)
            {
                UI::breadcrumb("Order Management > Cancel Order");
                while (true)
                {
                    int orderId = UI::readInt("Enter Order ID: ", 1, 2147483647);
                    if (!orderManager.searchOrder(orderId))
                    {
                        UI::error("Order not found.");
                        cout << "Please enter a valid Order ID.\n";
                        continue;
                    }

                    if (orderManager.cancelOrder(orderId))
                        UI::success("Order cancelled successfully.");
                    break;
                }
                pause();
            }
            else if (choice == 5)
            {
                UI::breadcrumb("Order Management > Display Order");
                while (true)
                {
                    int orderId = UI::readInt("Enter Order ID: ", 1, 2147483647);
                    if (!orderManager.searchOrder(orderId))
                    {
                        UI::error("Order not found.");
                        cout << "Please enter a valid Order ID.\n";
                        continue;
                    }
                    orderManager.displayOrderDetails(orderId);
                    break;
                }
                pause();
            }
            else if (choice == 6)
            {
                orderManager.displayAllOrders();
                pause();
            }
        }
    }

    void employeeMenu()
    {
        while (true)
        {
            clearScreen();
            menuTitle("EMPLOYEE MANAGEMENT");
            UI::breadcrumb("Employee Management");

            cout << "1. Add Employee\n"
                << "2. Display All\n"
                << "3. Search\n"
                << "4. Update\n"
                << "5. Change Availability\n"
                << "6. Display By Role\n"
                << "0. Back\n";

            int choice = UI::readInt("Choice: ", 0, 6);
            vector<Employee*>& employees = employeeManager.getEmployees();

            if (choice == 0)
                return;

            clearScreen();

            if (choice == 1)
            {
                int role = UI::readInt(
                    "Role (1=Manager,2=Chef,3=Waiter,4=Cashier,5=DeliveryDriver): ",
                    1, 5);
                int id = UI::readInt("Enter Employee ID: ", 1, 2147483647);

                if (employeeManager.isEmployeeIDUsed(id))
                {
                    UI::error("Employee ID already exists.");
                    pause();
                    continue;
                }

                string name = UI::readRequired("Enter Name: ");
                string phone = UI::readPhone("Enter Phone Number: ");
                double salary = UI::readDouble("Enter Salary: ", 0.01);
                int active = UI::readInt("Is Active? (1=Yes,0=No): ", 0, 1);

                Employee* employee = nullptr;

                if (role == 1)
                    employee = new Manager(id, name, phone, salary, active);
                else if (role == 2)
                    employee = new Chef(id, name, phone, salary, active);
                else if (role == 3)
                    employee = new Waiter(id, name, phone, salary, active);
                else if (role == 4)
                    employee = new Cashier(id, name, phone, salary, active);
                else
                    employee = new DeliveryDriver(id, name, phone, salary, active);

                employees.push_back(employee);
                UI::success("Employee added successfully.");
                pause();
            }
            else if (choice == 2)
            {
                employeeManager.displayEmployees();
                pause();
            }
            else if (choice == 3)
            {
                UI::breadcrumb("Employee Management > Search Employee");
                Employee* employee = readEmployeeByID();
                employee->displayInfo();
                pause();
            }
            else if (choice == 4)
            {
                UI::breadcrumb("Employee Management > Update Employee");
                Employee* employee = readEmployeeByID();

                int field = UI::readInt(
                    "1. Update Name\n"
                    "2. Update Phone Number\n"
                    "3. Update Salary\n"
                    "Choice: ",
                    1, 3);

                if (field == 1)
                    employee->setName(UI::readRequired("New Name: "));
                else if (field == 2)
                    employee->setPhoneNumber(UI::readPhone("New Phone Number: "));
                else
                    employee->setSalary(UI::readDouble("New Salary: ", 0.01));

                UI::success("Employee updated successfully.");
                pause();
            }
            else if (choice == 5)
            {
                UI::breadcrumb("Employee Management > Change Availability");
                Employee* employee = readEmployeeByID();

                int active = UI::readInt(
                    "New Availability (1=Available,0=Not Available): ", 0, 1);
                employee->setIsActive(active);

                UI::success("Availability updated successfully.");
                pause();
            }
            else if (choice == 6)
            {
                int role = UI::readInt(
                    "Role (0=Manager,1=Chef,2=Waiter,3=Cashier,4=DeliveryDriver): ",
                    0, 4);
                employeeManager.displayByRole((EmployeeRole)role);
                pause();
            }
        }
    }

    void kitchenMenu()
    {
        while (true)
        {
            clearScreen();
            menuTitle("KITCHEN MANAGEMENT");
            UI::breadcrumb("Kitchen Management");

            cout << "1. View Pending\n"
                << "2. Start Preparing\n"
                << "3. Mark Ready\n"
                << "4. Display Prepared\n"
                << "0. Back\n";

            int choice = UI::readInt("Choice: ", 0, 4);

            if (choice == 0)
                return;

            clearScreen();

            if (choice == 1)
            {
                kitchen.viewPendingOrders();
            }
            else if (choice == 2)
            {
                UI::breadcrumb("Kitchen Management > Start Preparing");
                while (true)
                {
                    int id = UI::readInt("Enter Order ID: ", 1, 2147483647);
                    if (!orderManager.searchOrder(id))
                    {
                        UI::error("Order not found.");
                        cout << "Please enter a valid Order ID.\n";
                        continue;
                    }

                    if (orderManager.updateOrderStatus(id, OrderStatus::Preparing))
                        UI::success("Order is now being prepared.");
                    break;
                }
            }
            else if (choice == 3)
            {
                UI::breadcrumb("Kitchen Management > Mark Ready");
                while (true)
                {
                    int id = UI::readInt("Enter Order ID: ", 1, 2147483647);
                    if (!orderManager.searchOrder(id))
                    {
                        UI::error("Order not found.");
                        cout << "Please enter a valid Order ID.\n";
                        continue;
                    }

                    if (orderManager.updateOrderStatus(id, OrderStatus::Ready))
                        UI::success("Order is ready.");
                    break;
                }
            }
            else if (choice == 4)
            {
                kitchen.displayPreparedOrders();
            }

            pause();
        }
    }

    void paymentMenu()
    {
        while (true)
        {
            clearScreen();
            menuTitle("PAYMENT MANAGEMENT");
            UI::breadcrumb("Payment Management");

            cout << "1. Process Payment\n"
                << "2. Display Details\n"
                << "3. Refund\n"
                << "4. Receipt\n"
                << "5. History\n"
                << "0. Back\n";

            int choice = UI::readInt("Choice: ", 0, 5);

            if (choice == 0)
                return;

            clearScreen();

            if (choice == 1)
            {
                Order* order = readOrderByID();
                int orderId = order->getOrderID();

                if (!order)
                {
                    UI::error("Order not found.");
                    pause();
                    continue;
                }

                if (order->getStatus() == OrderStatus::Cancelled)
                {
                    UI::error("Cannot pay for a cancelled order.");
                    pause();
                    continue;
                }

                bool alreadyPaid = false;
                for (const auto& payment : paymentManager.getPayments())
                {
                    if (payment.getOrderID() == orderId &&
                        payment.getStatus() == PaymentStatus::Completed)
                    {
                        alreadyPaid = true;
                        break;
                    }
                }

                if (alreadyPaid)
                {
                    UI::error("This Order has already been paid.");
                    pause();
                    continue;
                }

                double expected = order->getTotalPrice();

                if (expected <= 0)
                {
                    UI::error("Order total must be greater than zero before payment.");
                    pause();
                    continue;
                }

                cout << "\n====================================\n"
                    << "          PAYMENT SUMMARY\n"
                    << "====================================\n"
                    << "Order ID      : " << orderId << "\n"
                    << "Amount to Pay : " << fixed << setprecision(2) << expected << "\n"
                    << "====================================\n";

                double amount = UI::readDouble("Enter Amount: ", 0.01);

                if (amount + 0.005 < expected)
                {
                    UI::error(
                        "Payment is insufficient. Required amount: " + to_string(expected));
                    pause();
                    continue;
                }

                if (amount > expected + 0.005)
                {
                    UI::error(
                        "Payment exceeds order total. Please enter the exact amount: " +
                        to_string(expected));
                    pause();
                    continue;
                }

                int method = UI::readInt(
                    "Method (0=Cash,1=Card,2=MobilePayment): ", 0, 2);

                int paymentId = paymentManager.processPayment(
                    orderId, amount, (PaymentMethod)method);

                if (paymentId != -1)
                {
                    UI::success(
                        "Payment processed successfully. Payment ID: " +
                        to_string(paymentId));
                }
            }
            else if (choice == 2)
            {
                UI::breadcrumb("Payment Management > Display Details");
                while (true)
                {
                    int paymentId = UI::readInt("Enter Payment ID: ", 1, 2147483647);
                    if (!paymentManager.searchPayment(paymentId))
                    {
                        UI::error("Payment not found.");
                        cout << "Please enter a valid Payment ID.\n";
                        continue;
                    }
                    paymentManager.displayPaymentDetails(paymentId);
                    break;
                }
            }
            else if (choice == 3)
            {
                UI::breadcrumb("Payment Management > Refund");
                while (true)
                {
                    int paymentId = UI::readInt("Enter Payment ID: ", 1, 2147483647);
                    if (!paymentManager.searchPayment(paymentId))
                    {
                        UI::error("Payment not found.");
                        cout << "Please enter a valid Payment ID.\n";
                        continue;
                    }
                    paymentManager.refundPayment(paymentId);
                    break;
                }
            }
            else if (choice == 4)
            {
                UI::breadcrumb("Payment Management > Receipt");
                while (true)
                {
                    int paymentId = UI::readInt("Enter Payment ID: ", 1, 2147483647);
                    if (!paymentManager.searchPayment(paymentId))
                    {
                        UI::error("Payment not found.");
                        cout << "Please enter a valid Payment ID.\n";
                        continue;
                    }
                    paymentManager.generateReceipt(paymentId);
                    break;
                }
            }
            else if (choice == 5)
            {
                paymentManager.viewPaymentHistory();
            }

            pause();
        }
    }

    void deliveryMenu()
    {
        while (true)
        {
            clearScreen();
            menuTitle("DELIVERY MANAGEMENT");
            UI::breadcrumb("Delivery Management");

            cout << "1. Assign Delivery\n"
                << "2. Track Status\n"
                << "3. Update Status\n"
                << "4. View All\n"
                << "0. Back\n";

            int choice = UI::readInt("Choice: ", 0, 4);

            if (choice == 0)
                return;

            clearScreen();

            if (choice == 1)
            {
                int orderId = UI::readInt("Enter Order ID: ", 1, 2147483647);
                Order* order = orderManager.searchOrder(orderId);

                if (!order)
                {
                    UI::error("Order not found.");
                    pause();
                    continue;
                }

                if (order->getOrderType() != OrderType::Delivery)
                {
                    UI::error("Delivery can only be assigned to a Delivery order.");
                    pause();
                    continue;
                }

                int driverId = UI::readInt(
                    "Enter Driver Employee ID: ", 1, 2147483647);
                Employee* driver = employeeManager.findEmployeeByID(driverId);

                if (!driver)
                {
                    UI::error("Employee not found.");
                    pause();
                    continue;
                }

                if (driver->getRole() != EmployeeRole::DeliveryDriver)
                {
                    UI::error("Selected employee is not a delivery driver.");
                    pause();
                    continue;
                }

                if (!driver->getIsActive())
                {
                    UI::error("Selected driver is not active.");
                    pause();
                    continue;
                }

                string address = UI::readRequired("Enter Delivery Address: ");
                int customerId = order->getCustomerID();

                int deliveryId = deliveryManager.assignDelivery(
                    orderId, driverId, address, customerId);

                if (deliveryId != -1)
                {
                    UI::success(
                        "Delivery assigned successfully. Delivery ID: " +
                        to_string(deliveryId));
                }
            }
            else if (choice == 2)
            {
                UI::breadcrumb("Delivery Management > Track Status");
                while (true)
                {
                    int deliveryId = UI::readInt("Enter Delivery ID: ", 1, 2147483647);
                    if (!deliveryManager.searchDelivery(deliveryId))
                    {
                        UI::error("Delivery not found.");
                        cout << "Please enter a valid Delivery ID.\n";
                        continue;
                    }
                    deliveryManager.trackDeliveryStatus(deliveryId);
                    break;
                }
            }
            else if (choice == 3)
            {
                UI::breadcrumb("Delivery Management > Update Status");
                while (true)
                {
                    int deliveryId = UI::readInt("Enter Delivery ID: ", 1, 2147483647);
                    if (!deliveryManager.searchDelivery(deliveryId))
                    {
                        UI::error("Delivery not found.");
                        cout << "Please enter a valid Delivery ID.\n";
                        continue;
                    }

                    int status = UI::readInt(
                        "New Status (0=Assigned,1=OnTheWay,2=Delivered,3=Cancelled): ",
                        0, 3);

                    if (deliveryManager.updateDeliveryStatus(
                        deliveryId, (DeliveryStatus)status))
                    {
                        UI::success("Delivery status updated successfully.");
                    }
                    break;
                }
            }
            else if (choice == 4)
            {
                deliveryManager.viewAllDeliveries();
            }

            pause();
        }
    }

    void reportsMenu()
    {
        while (true)
        {
            clearScreen();
            menuTitle("REPORTS");
            UI::breadcrumb("Reports");

            cout << "1. Daily Sales\n"
                << "2. Monthly Revenue\n"
                << "3. Top Selling Items\n"
                << "4. Employee Performance\n"
                << "5. Delivery Report\n"
                << "0. Back\n";

            int choice = UI::readInt("Choice: ", 0, 5);

            if (choice == 0)
                return;

            clearScreen();

            if (choice == 1)
            {
                string date = UI::readDate("Enter Date (YYYY-MM-DD): ");
                reportManager.generateDailySalesReport(date);
            }
            else if (choice == 2)
            {
                string month = UI::readRequired("Enter Month (YYYY-MM): ");

                bool validMonth =
                    month.size() == 7 &&
                    month[4] == '-' &&
                    isdigit((unsigned char)month[0]) &&
                    isdigit((unsigned char)month[1]) &&
                    isdigit((unsigned char)month[2]) &&
                    isdigit((unsigned char)month[3]) &&
                    isdigit((unsigned char)month[5]) &&
                    isdigit((unsigned char)month[6]);

                if (validMonth)
                {
                    int monthNumber = stoi(month.substr(5, 2));
                    validMonth = monthNumber >= 1 && monthNumber <= 12;
                }

                if (!validMonth)
                {
                    UI::error("Invalid month. Use YYYY-MM.");
                }
                else
                {
                    reportManager.generateMonthlyRevenueReport(month);
                }
            }
            else if (choice == 3)
            {
                reportManager.generateTopSellingItemsReport();
            }
            else if (choice == 4)
            {
                reportManager.generateEmployeePerformanceReport();
            }
            else if (choice == 5)
            {
                reportManager.generateDeliveryReport();
            }

            pause();
        }
    }
};

// ============================================================================
// main() 
// ============================================================================
int main() {
    clearScreen();
    cout << "Welcome to the Restaurant Management System!\n";

    RestaurantSystem system;
    system.run();

    return 0;
}
