#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>
#include <algorithm>
#include <cctype>
using namespace std;

const int MAX_RECORDS = 500;
const string DATA_FILE = "lost_found_records.txt";
const string REPORT_FILE = "lost_found_report.txt";

// Stores the details of one lost or found item.
struct ItemRecord {
    int id;
    string type;       // LOST or FOUND
    string itemName;
    string category;
    string description;
    string location;
    string date;
    string reporter;
    string contact;
    bool active;
};

ItemRecord records[MAX_RECORDS];
int recordCount = 0;
int nextId = 1;

// Demo login credentials. Change these before submitting if required.
const string STUDENT_ID = "SHAAN";
const string STUDENT_PASSWORD = "SHAAN123";

string toLowerCase(string value) {
    for (size_t i = 0; i < value.length(); i++) {
        value[i] = static_cast<char>(tolower(static_cast<unsigned char>(value[i])));
    }
    return value;
}

string cleanField(string value) {
    // Keep the pipe character available as the file-field separator.
    for (size_t i = 0; i < value.length(); i++) {
        if (value[i] == '|') value[i] = '/';
    }
    return value;
}

string readLine(const string& prompt) {
    string value;
    cout << prompt;
    getline(cin, value);
    return cleanField(value);
}

bool login() {
    string id, password;
    cout << "\n========================================\n";
    cout << "       SMART CAMPUS LOST & FOUND\n";
    cout << "========================================\n";
    cout << "Student Login\n";
    cout << "Demo Student ID: " << STUDENT_ID << "\n";
    cout << "Demo Password : " << STUDENT_PASSWORD << "\n\n";

    for (int attempt = 1; attempt <= 3; attempt++) {
        cout << "Enter Student ID: ";
        getline(cin, id);
        cout << "Enter Password: ";
        getline(cin, password);

        if (id == STUDENT_ID && password == STUDENT_PASSWORD) {
            cout << "\nLogin successful. Welcome!\n";
            return true;
        }

        cout << "Incorrect ID or password. Attempts left: "
             << (3 - attempt) << "\n\n";
    }

    cout << "Maximum login attempts reached.\n";
    return false;
}

void loadRecords() {
    ifstream file(DATA_FILE.c_str());
    if (!file) return;

    ItemRecord item;
    string activeText;

    while (recordCount < MAX_RECORDS &&
           file >> item.id) {
        file.ignore();
        getline(file, item.type, '|');
        getline(file, item.itemName, '|');
        getline(file, item.category, '|');
        getline(file, item.description, '|');
        getline(file, item.location, '|');
        getline(file, item.date, '|');
        getline(file, item.reporter, '|');
        getline(file, item.contact, '|');
        getline(file, activeText);

        item.active = (activeText == "1");
        records[recordCount++] = item;

        if (item.id >= nextId) nextId = item.id + 1;
    }
    file.close();
}

void saveRecords() {
    ofstream file(DATA_FILE.c_str());
    if (!file) {
        cout << "Error: Could not save records.\n";
        return;
    }

    for (int i = 0; i < recordCount; i++) {
        file << records[i].id << '\n'
             << records[i].type << '|'
             << records[i].itemName << '|'
             << records[i].category << '|'
             << records[i].description << '|'
             << records[i].location << '|'
             << records[i].date << '|'
             << records[i].reporter << '|'
             << records[i].contact << '|'
             << (records[i].active ? "1" : "0") << '\n';
    }
    file.close();
}

void displayRecord(const ItemRecord& item) {
    cout << "\n----------------------------------------\n";
    cout << "Record ID   : " << item.id << '\n';
    cout << "Record Type : " << item.type << '\n';
    cout << "Item Name   : " << item.itemName << '\n';
    cout << "Category    : " << item.category << '\n';
    cout << "Description : " << item.description << '\n';
    cout << "Location    : " << item.location << '\n';
    cout << "Date        : " << item.date << '\n';
    cout << "Reported By : " << item.reporter << '\n';
    cout << "Contact     : " << item.contact << '\n';
    cout << "----------------------------------------\n";
}

void addRecord(const string& type) {
    if (recordCount >= MAX_RECORDS) {
        cout << "Record limit reached. Cannot add more records.\n";
        return;
    }

    ItemRecord item;
    item.id = nextId++;
    item.type = type;
    item.itemName = readLine("Enter item name: ");
    item.category = readLine("Enter category (Electronics/ID Card/Keys/Books/etc.): ");
    item.description = readLine("Enter item description: ");
    item.location = readLine("Enter lost/found location: ");
    item.date = readLine("Enter date (DD-MM-YYYY): ");
    item.reporter = readLine("Enter your name: ");
    item.contact = readLine("Enter contact information: ");
    item.active = true;

    if (item.itemName.empty() || item.category.empty()) {
        cout << "Item name and category cannot be empty. Record not saved.\n";
        nextId--;
        return;
    }

    records[recordCount++] = item;
    saveRecords();

    cout << "\n" << type << " item reported successfully!\n";
    cout << "Your Record ID is: " << item.id << '\n';
}

void reportLostItem() {
    cout << "\n========== REPORT LOST ITEM ==========\n";
    addRecord("LOST");
}

void reportFoundItem() {
    cout << "\n========== REPORT FOUND ITEM =========\n";
    addRecord("FOUND");
}

bool containsText(const string& source, const string& keyword) {
    return toLowerCase(source).find(toLowerCase(keyword)) != string::npos;
}

void searchItems() {
    cout << "\n========== SEARCH ITEMS ==========\n";
    string keyword = readLine("Enter item name, category, description or location: ");

    if (keyword.empty()) {
        cout << "Search keyword cannot be empty.\n";
        return;
    }

    int foundCount = 0;
    for (int i = 0; i < recordCount; i++) {
        if (records[i].active &&
            (containsText(records[i].itemName, keyword) ||
             containsText(records[i].category, keyword) ||
             containsText(records[i].description, keyword) ||
             containsText(records[i].location, keyword))) {
            displayRecord(records[i]);
            foundCount++;
        }
    }

    if (foundCount == 0) cout << "No matching records found.\n";
    else cout << "\nTotal matching records: " << foundCount << '\n';
}

bool itemsMayMatch(const ItemRecord& lost, const ItemRecord& found) {
    // A simple keyword-based match. Students should verify the item in person.
    bool sameName = (toLowerCase(lost.itemName) == toLowerCase(found.itemName));
    bool sameCategory = (toLowerCase(lost.category) == toLowerCase(found.category));
    bool nameInDescription =
        containsText(found.description, lost.itemName) ||
        containsText(lost.description, found.itemName);

    return sameName || (sameCategory && nameInDescription);
}

void autoMatchItems() {
    cout << "\n========== AUTOMATIC LOST & FOUND MATCH ==========\n";
    int matches = 0;

    for (int i = 0; i < recordCount; i++) {
        if (!records[i].active || records[i].type != "LOST") continue;

        for (int j = 0; j < recordCount; j++) {
            if (!records[j].active || records[j].type != "FOUND") continue;

            if (itemsMayMatch(records[i], records[j])) {
                cout << "\nPossible match found!\n";
                cout << "LOST ITEM:";
                displayRecord(records[i]);
                cout << "FOUND ITEM:";
                displayRecord(records[j]);
                matches++;
            }
        }
    }

    if (matches == 0) {
        cout << "No possible matches found at the moment.\n";
    } else {
        cout << "\nPossible matches found: " << matches << '\n';
        cout << "Please verify ownership with the reporter. This is not proof of ownership.\n";
    }
}

void viewAllRecords() {
    cout << "\n========== ALL ACTIVE RECORDS ==========\n";

    // Copy active records into a temporary array and sort by item name.
    ItemRecord sortedRecords[MAX_RECORDS];
    int count = 0;

    for (int i = 0; i < recordCount; i++) {
        if (records[i].active) sortedRecords[count++] = records[i];
    }

    for (int i = 0; i < count - 1; i++) {
        for (int j = 0; j < count - i - 1; j++) {
            if (toLowerCase(sortedRecords[j].itemName) >
                toLowerCase(sortedRecords[j + 1].itemName)) {
                ItemRecord temp = sortedRecords[j];
                sortedRecords[j] = sortedRecords[j + 1];
                sortedRecords[j + 1] = temp;
            }
        }
    }

    if (count == 0) {
        cout << "No active records available.\n";
        return;
    }

    cout << "Records are sorted alphabetically by item name.\n";
    for (int i = 0; i < count; i++) displayRecord(sortedRecords[i]);
    cout << "\nTotal active records: " << count << '\n';
}

void deleteRecord() {
    cout << "\n========== DELETE RECORD ==========\n";
    int id;
    cout << "Enter Record ID to delete: ";

    if (!(cin >> id)) {
        cin.clear();
        cin.ignore(10000, '\n');
        cout << "Invalid ID. Please enter a number.\n";
        return;
    }
    cin.ignore(10000, '\n');

    for (int i = 0; i < recordCount; i++) {
        if (records[i].id == id && records[i].active) {
            records[i].active = false;
            saveRecords();
            cout << "Record " << id << " deleted successfully.\n";
            return;
        }
    }

    cout << "Active record with this ID was not found.\n";
}

void itemStatistics() {
    cout << "\n========== ITEM STATISTICS ==========\n";
    int lostCount = 0, foundCount = 0, total = 0;

    for (int i = 0; i < recordCount; i++) {
        if (records[i].active) {
            total++;
            if (records[i].type == "LOST") lostCount++;
            else if (records[i].type == "FOUND") foundCount++;
        }
    }

    cout << "Total active records : " << total << '\n';
    cout << "Lost items reported  : " << lostCount << '\n';
    cout << "Found items reported : " << foundCount << '\n';
}

void exportReport() {
    ofstream file(REPORT_FILE.c_str());
    if (!file) {
        cout << "Error: Could not create report file.\n";
        return;
    }

    int lostCount = 0, foundCount = 0, total = 0;
    file << "SMART CAMPUS LOST & FOUND MANAGEMENT SYSTEM\n";
    file << "===========================================\n\n";

    for (int i = 0; i < recordCount; i++) {
        if (!records[i].active) continue;

        total++;
        if (records[i].type == "LOST") lostCount++;
        else if (records[i].type == "FOUND") foundCount++;

        file << "Record ID: " << records[i].id << '\n'
             << "Type: " << records[i].type << '\n'
             << "Item: " << records[i].itemName << '\n'
             << "Category: " << records[i].category << '\n'
             << "Description: " << records[i].description << '\n'
             << "Location: " << records[i].location << '\n'
             << "Date: " << records[i].date << '\n'
             << "Reporter: " << records[i].reporter << '\n'
             << "Contact: " << records[i].contact << '\n'
             << "-------------------------------------------\n";
    }

    file << "\nSUMMARY\n";
    file << "Total active records: " << total << '\n';
    file << "Lost items: " << lostCount << '\n';
    file << "Found items: " << foundCount << '\n';
    file.close();

    cout << "Report exported successfully to " << REPORT_FILE << '\n';
}

void showMenu() {
    cout << "\n========================================\n";
    cout << "       SMART CAMPUS LOST & FOUND\n";
    cout << "========================================\n";
    cout << "1.  Report Lost Item\n";
    cout << "2.  Report Found Item\n";
    cout << "3.  Search Item\n";
    cout << "4.  Auto Match Lost & Found\n";
    cout << "5.  View All Records\n";
    cout << "6.  Delete Record\n";
    cout << "7.  Item Statistics\n";
    cout << "8.  Export Report\n";
    cout << "9.  Exit\n";
    cout << "========================================\n";
    cout << "Enter your choice: ";
}

int main() {
    if (!login()) return 0;

    loadRecords();
    int choice;

    do {
        showMenu();

        if (!(cin >> choice)) {
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "Invalid input. Please enter a number from 1 to 9.\n";
            continue;
        }
        cin.ignore(10000, '\n');

        switch (choice) {
            case 1: reportLostItem(); break;
            case 2: reportFoundItem(); break;
            case 3: searchItems(); break;
            case 4: autoMatchItems(); break;
            case 5: viewAllRecords(); break;
            case 6: deleteRecord(); break;
            case 7: itemStatistics(); break;
            case 8: exportReport(); break;
            case 9:
                saveRecords();
                cout << "Thank you for using Smart Campus Lost & Found!\n";
                break;
            default:
                cout << "Invalid choice. Please select 1 to 9.\n";
        }
    } while (choice != 9);

    return 0;
}
