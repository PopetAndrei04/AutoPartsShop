#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>''
#include <map>

class Part {
public:
    std::string name;
    std::string carModel;
    double price;

    Part(const std::string& n, const std::string& cm, double p) : name(n), carModel(cm), price(p) {}
};

class UserInterface {
public:
    virtual void menu() = 0; // Pure virtual function
};


class Catalog {
private:
    std::vector<Part> parts;
    std::string filename;
public:
    Catalog(const std::string& fn) : filename(fn) {
        loadFromFile();
    }

    void loadFromFile() {
        std::ifstream file(filename);
        std::string line;
        while (std::getline(file, line)) {
            std::stringstream ss(line);
            std::string name, carModel, priceStr;
            std::getline(ss, name, ',');
            std::getline(ss, carModel, ',');
            std::getline(ss, priceStr, ',');
            double price = std::stod(priceStr);
            parts.push_back(Part(name, carModel, price));
        }
    }

    void saveToFile() {
        std::ofstream file(filename, std::ofstream::trunc);
        for (const auto& part : parts) {
            file << part.name << "," << part.carModel << "," << part.price << "\n";
        }
    }

    void addPart(const Part& part) {
        parts.push_back(part);
        saveToFile();
    }

    std::vector<Part> searchByName(const std::string& name) {
        std::vector<Part> result;
        for (const auto& part : parts) {
            if (part.name.find(name) != std::string::npos) {
                result.push_back(part);
            }
        }
        return result;
    }

    std::vector<Part> searchByCarModel(const std::string& carModel) {
        std::vector<Part> result;
        for (const auto& part : parts) {
            if (part.carModel.find(carModel) != std::string::npos) {
                result.push_back(part);
            }
        }
        return result;
    }

    std::vector<Part> searchByNameAndCarModel(const std::string& name, const std::string& carModel) {
        std::vector<Part> result;
        for (const auto& part : parts) {
            if (part.name.find(name) != std::string::npos && part.carModel.find(carModel) != std::string::npos) {
                result.push_back(part);
            }
        }
        return result;
    }
};

class ShoppingCart {
private:
    std::vector<Part> cart;
public:
    void addToCart(const Part& part) {
        cart.push_back(part);
    }

    double calculateTotal() {
        double total = 0;
        for (const auto& part : cart) {
            total += part.price;
        }
        return total;
    }

    void checkout() {
        double total = calculateTotal();
        std::cout << "Total amount: $" << total << std::endl;
        cart.clear();
        std::cout << "Thank you for your purchase!" << std::endl;
    }

    void viewCart() {
        if (cart.empty()) {
            std::cout << "Your cart is empty." << std::endl;
        } else {
            std::cout << "Items in your cart:" << std::endl;
            for (const auto& part : cart) {
                std::cout << "Part: " << part.name << ", Car Model: " << part.carModel << ", Price: $" << part.price << std::endl;
            }
        }
    }
};

class User {
public:
    virtual void menu() = 0;
};

class Admin : public UserInterface {
private:
    Catalog& catalog;
    std::string username;
public:
    Admin(Catalog& c, const std::string& u) : catalog(c), username(u) {}
    void menu() override {
        int choice;
        do {
            std::cout << "1. Add part to catalog\n2. Logout\nEnter choice: ";
            std::cin >> choice;
            if (choice == 1) {
                std::string name, carModel;
                double price;
                std::cout << "Enter part name: ";
                std::cin >> name;
                std::cout << "Enter car model: ";
                std::cin >> carModel;
                std::cout << "Enter price: ";
                std::cin >> price;
                catalog.addPart(Part(name, carModel, price));
            }
        } while (choice != 2);
    }
};



class Shopper : public UserInterface {
private:
    Catalog& catalog;
    ShoppingCart cart;
public:
    Shopper(Catalog& c) : catalog(c) {}

public:
    void menu() override {
        int choice;
        do {
            std::cout << "1. Search part by name\n2. Search part by car model\n3. Search part by name and car model\n4. View cart\n5. Checkout\n6. Logout\nEnter choice: ";
            std::cin >> choice;
            if (choice == 1) {
                std::string name;
                std::cout << "Enter part name: ";
                std::cin >> name;
                auto results = catalog.searchByName(name);
                displayResults(results);
            } else if (choice == 2) {
                std::string carModel;
                std::cout << "Enter car model: ";
                std::cin >> carModel;
                auto results = catalog.searchByCarModel(carModel);
                displayResults(results);
            } else if (choice == 3) {
                std::string name, carModel;
                std::cout << "Enter part name: ";
                std::cin >> name;
                std::cout << "Enter car model: ";
                std::cin >> carModel;
                auto results = catalog.searchByNameAndCarModel(name, carModel);
                displayResults(results);
            } else if (choice == 4) {
                cart.viewCart();
            } else if (choice == 5) {
                cart.checkout();
            }
        } while (choice != 6);
    }

    void displayResults(const std::vector<Part>& results) {
        if (results.empty()) {
            std::cout << "No parts found." << std::endl;
        } else {
            for (size_t i = 0; i < results.size(); ++i) {
                const auto& part = results[i];
                std::cout << i + 1 << ". Part: " << part.name << ", Car Model: " << part.carModel << ", Price: $" << part.price << std::endl;
            }
            int choice;
            std::cout << "Enter the number of the part you want to add to the cart (0 to cancel): ";
            std::cin >> choice;
            if (choice > 0 && choice <= results.size()) {
                cart.addToCart(results[choice - 1]);
                std::cout << "Part added to cart." << std::endl;
            }
        }
    }
};

class AdminManager {
private:
    std::map<std::string, std::string> admins;
    std::string filename;
public:
    AdminManager(const std::string& fn) : filename(fn) {
        loadFromFile();
    }

    void loadFromFile() {
        std::ifstream file(filename);
        std::string line;
        while (std::getline(file, line)) {
            std::stringstream ss(line);
            std::string username, password;
            std::getline(ss, username, ',');
            std::getline(ss, password, ',');
            admins[username] = password;
        }
    }

    bool validateAdmin(const std::string& username, const std::string& password) {
        return admins.find(username) != admins.end() && admins[username] == password;
    }
};

class Shop {
private:
    Catalog catalog;
    AdminManager adminManager;
public:
    Shop(const std::string& partsFile, const std::string& adminsFile) : catalog(partsFile), adminManager(adminsFile) {}

    void run() {
        int choice;
        std::cout << "1. Login as Admin\n2. Login as Shopper\nEnter choice: ";
        std::cin >> choice;
        if (choice == 1) {
            std::string username, password;
            std::cout << "Enter admin username: ";
            std::cin >> username;
            std::cout << "Enter admin password: ";
            std::cin >> password;
            if (adminManager.validateAdmin(username, password)) {
                Admin admin(catalog, username);
                admin.menu();
            } else {
                std::cout << "Invalid admin username or password!" << std::endl;
            }
        } else if (choice == 2) {
            Shopper shopper(catalog);
            shopper.menu();
        } else {
            std::cout << "Invalid choice!" << std::endl;
        }
    }
};

int main() {
    Shop shop("parts.csv", "admins.csv");
    shop.run();
    return 0;
}
