#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include <chrono>
#include <fstream>
#include <limits>
#include <algorithm>
#include <sstream>

// --- Structs for data management ---
enum VehicleType {
    MOTORCYCLE,
    CAR
};

struct Vehicle {
    std::string licensePlate;
    std::string ownerName;
    VehicleType type;
};

struct ParkingSlot {
    int slotNumber;
    bool isOccupied;
    Vehicle vehicle;
    long long entryTimeEpoch; 
};

// --- Function Definitions ---
void clearInputBuffer() {
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

void saveRecords(const std::vector<ParkingSlot>& parkingLot) {
    std::ofstream outFile("parking_records.txt");
    if (!outFile.is_open()) {
        std::cerr << "Error: Could not open file for writing." << std::endl;
        return;
    }
    for (const auto& slot : parkingLot) {
        outFile << slot.slotNumber << "|"
                << slot.isOccupied << "|"
                << (slot.isOccupied ? slot.vehicle.licensePlate : "N/A_LICENSE") << "|"
                << (slot.isOccupied ? slot.vehicle.ownerName : "N/A_OWNER") << "|"
                << static_cast<int>(slot.vehicle.type) << "|"
                << slot.entryTimeEpoch << "\n";
    }
    outFile.close();
}

void loadRecords(std::vector<ParkingSlot>& parkingLot) {
    std::ifstream inFile("parking_records.txt");
    if (!inFile.is_open()) {
        std::cout << "No previous records found. Starting with an empty lot." << std::endl;
        return;
    }

    parkingLot.clear();
    std::string line;
    while (std::getline(inFile, line)) {
        std::stringstream ss(line);
        std::string part;
        int slotNum, vehicleTypeInt;
        bool occupied;
        std::string license, owner;
        long long entryTime;

        std::getline(ss, part, '|');
        slotNum = std::stoi(part);
        
        std::getline(ss, part, '|');
        occupied = (part == "1");

        std::getline(ss, license, '|');
        std::getline(ss, owner, '|');
        
        std::getline(ss, part, '|');
        vehicleTypeInt = std::stoi(part);

        std::getline(ss, part, '|');
        entryTime = std::stoll(part);

        if (license == "N/A_LICENSE") license = "";
        if (owner == "N/A_OWNER") owner = "";

        VehicleType type = static_cast<VehicleType>(vehicleTypeInt);
        parkingLot.push_back({slotNum, occupied, {license, owner, type}, entryTime});
    }
    inFile.close();
    std::cout << "Parking records loaded successfully." << std::endl;
}

void displayMenu() {
    std::cout << "\n======================================" << std::endl;
    std::cout << "      PARKING LOT MANAGEMENT SYSTEM" << std::endl;
    std::cout << "======================================" << std::endl;
    std::cout << "1. Park Vehicle" << std::endl;
    std::cout << "2. Unpark Vehicle" << std::endl;
    std::cout << "3. Find Vehicle" << std::endl;
    std::cout << "4. Display Available Slots" << std::endl;
    std::cout << "5. Display All Slots" << std::endl;
    std::cout << "6. Admin Panel" << std::endl;
    std::cout << "7. Exit" << std::endl;
    std::cout << "======================================" << std::endl;
}

void displayAdminMenu() {
    std::cout << "\n======================================" << std::endl;
    std::cout << "           ADMIN PANEL" << std::endl;
    std::cout << "======================================" << std::endl;
    std::cout << "1. Add Parking Slot" << std::endl;
    std::cout << "2. Remove Parking Slot" << std::endl;
    std::cout << "3. Back to Main Menu" << std::endl;
    std::cout << "======================================" << std::endl;
}

void calculateAndDisplayFee(const ParkingSlot& slot) {
    auto now = std::chrono::system_clock::now();
    auto entryTime = std::chrono::system_clock::from_time_t(slot.entryTimeEpoch);
    std::chrono::duration<double> duration = now - entryTime;
    int minutes = std::chrono::duration_cast<std::chrono::minutes>(duration).count();
    
    double baseRate = 1.0; 
    if (slot.vehicle.type == CAR) {
        baseRate = 2.0;
    }

    double fee = (static_cast<double>(minutes) / 15.0) * baseRate;
    
    std::cout << "\n--- Parking Fee Details ---" << std::endl;
    std::cout << "Duration: " << minutes << " minutes" << std::endl;
    std::cout << "Total Fee: Rs." << std::fixed << std::setprecision(2) << fee << std::endl;
    std::cout << "--------------------------" << std::endl;
}

void displayAvailableSlots(const std::vector<ParkingSlot>& parkingLot) {
    std::cout << "\n--- Available Parking Slots ---" << std::endl;
    bool found = false;
    for (const auto& slot : parkingLot) {
        if (!slot.isOccupied) {
            std::cout << "Slot " << slot.slotNumber << std::endl;
            found = true;
        }
    }
    if (!found) {
        std::cout << "Sorry, no slots are currently available." << std::endl;
    }
}

void parkVehicle(std::vector<ParkingSlot>& parkingLot) {
    int slotChoice;
    std::string license, owner;
    char vehicleTypeChar;

    displayAvailableSlots(parkingLot); 

    std::cout << "\nEnter the slot number to park in: ";
    std::cin >> slotChoice;
    clearInputBuffer();

    auto it = std::find_if(parkingLot.begin(), parkingLot.end(), 
        [slotChoice](const ParkingSlot& slot){ return slot.slotNumber == slotChoice && !slot.isOccupied; });

    if (it != parkingLot.end()) {
        std::cout << "Is this a car or a motorcycle? (C/M): ";
        std::cin >> vehicleTypeChar;
        clearInputBuffer();

        VehicleType type;
        if (toupper(vehicleTypeChar) == 'C') {
            type = CAR;
        } else if (toupper(vehicleTypeChar) == 'M') {
            type = MOTORCYCLE;
        } else {
            std::cout << "Invalid vehicle type. Parking failed." << std::endl;
            return;
        }

        std::cout << "Enter vehicle license plate: ";
        std::getline(std::cin, license);
        std::cout << "Enter owner's name: ";
        std::getline(std::cin, owner);

        it->vehicle = {license, owner, type};
        it->isOccupied = true;
        
        it->entryTimeEpoch = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());

        std::cout << "\nParking Confirmed!" << std::endl;
        std::cout << "Vehicle: " << it->vehicle.licensePlate << " parked in slot " << it->slotNumber << "." << std::endl;
        saveRecords(parkingLot);
    } else {
        std::cout << "Invalid slot number or slot is already occupied. Please try again." << std::endl;
    }
}

void unparkVehicle(std::vector<ParkingSlot>& parkingLot) {
    std::string license;
    std::cout << "\nEnter the license plate of the vehicle to unpark: ";
    std::getline(std::cin, license);

    for (auto& slot : parkingLot) {
        if (slot.isOccupied && slot.vehicle.licensePlate == license) {
            std::cout << "\nVehicle found. Unparking..." << std::endl;
            
            calculateAndDisplayFee(slot);
            
            slot.isOccupied = false;
            slot.vehicle = {"", "", MOTORCYCLE};
            slot.entryTimeEpoch = 0;
            std::cout << "Vehicle " << license << " has been unparked from slot " << slot.slotNumber << "." << std::endl;
            saveRecords(parkingLot);
            return;
        }
    }
    std::cout << "Vehicle with license plate " << license << " not found." << std::endl;
}

void findVehicle(const std::vector<ParkingSlot>& parkingLot) {
    std::string license;
    std::cout << "\nEnter the license plate to search for: ";
    std::getline(std::cin, license);

    for (const auto& slot : parkingLot) {
        if (slot.isOccupied && slot.vehicle.licensePlate == license) {
            std::cout << "\nVehicle Found!" << std::endl;
            std::cout << "License Plate: " << slot.vehicle.licensePlate << std::endl;
            std::cout << "Owner Name: " << slot.vehicle.ownerName << std::endl;
            std::cout << "Parked in Slot: " << slot.slotNumber << std::endl;
            return;
        }
    }
    std::cout << "Vehicle with license plate " << license << " not found." << std::endl;
}

void displayAllSlots(const std::vector<ParkingSlot>& parkingLot) {
    std::cout << "\n--- All Parking Slots ---" << std::endl;
    std::cout << std::left << std::setw(10) << "Slot No."
              << std::setw(15) << "Status"
              << std::setw(15) << "Vehicle Type"
              << std::setw(20) << "License Plate"
              << std::setw(20) << "Owner Name" << std::endl;
    std::cout << "----------------------------------------------------------------------" << std::endl;

    for (const auto& slot : parkingLot) {
        std::cout << std::left << std::setw(10) << slot.slotNumber
                  << std::setw(15) << (slot.isOccupied ? "Occupied" : "Available");
        if (slot.isOccupied) {
            std::string typeStr = (slot.vehicle.type == CAR) ? "Car" : "Motorcycle";
            std::cout << std::setw(15) << typeStr
                      << std::setw(20) << slot.vehicle.licensePlate
                      << std::setw(20) << slot.vehicle.ownerName;
        } else {
            std::cout << std::setw(15) << "N/A"
                      << std::setw(20) << "N/A"
                      << std::setw(20) << "N/A";
        }
        std::cout << std::endl;
    }
}

void addSlot(std::vector<ParkingSlot>& parkingLot) {
    int newSlotNumber;
    std::cout << "Enter the number for the new slot: ";
    std::cin >> newSlotNumber;
    clearInputBuffer();
    
    for(const auto& slot : parkingLot) {
        if(slot.slotNumber == newSlotNumber) {
            std::cout << "Error: Slot number " << newSlotNumber << " already exists." << std::endl;
            return;
        }
    }
    parkingLot.push_back({newSlotNumber, false, {"", "", MOTORCYCLE}, 0});
    std::cout << "Slot " << newSlotNumber << " has been added successfully." << std::endl;
    saveRecords(parkingLot);
}

void removeSlot(std::vector<ParkingSlot>& parkingLot) {
    int slotToRemove;
    std::cout << "Enter the slot number to remove: ";
    std::cin >> slotToRemove;
    clearInputBuffer();

    auto it = std::find_if(parkingLot.begin(), parkingLot.end(), 
        [slotToRemove](const ParkingSlot& slot){ return slot.slotNumber == slotToRemove; });

    if (it != parkingLot.end()) {
        if (it->isOccupied) {
            std::cout << "Error: Cannot remove an occupied slot." << std::endl;
        } else {
            parkingLot.erase(it);
            std::cout << "Slot " << slotToRemove << " has been removed." << std::endl;
            saveRecords(parkingLot);
        }
    } else {
        std::cout << "Slot " << slotToRemove << " not found." << std::endl;
    }
}

void adminLogin(std::vector<ParkingSlot>& parkingLot) {
    std::string password;
    std::cout << "\nEnter admin password: ";
    std::getline(std::cin, password);

    if (password == "admin123") {
        int adminChoice;
        do {
            displayAdminMenu();
            std::cout << "Enter your choice: ";
            std::cin >> adminChoice;
            clearInputBuffer();
            switch (adminChoice) {
                case 1: addSlot(parkingLot); break;
                case 2: removeSlot(parkingLot); break;
                case 3: std::cout << "Returning to main menu." << std::endl; break;
                default: std::cout << "Invalid choice. Please try again." << std::endl; break;
            }
        } while (adminChoice != 3);
    } else {
        std::cout << "Incorrect password. Access denied." << std::endl;
    }
}

// --- Main function ---
int main() {
    std::vector<ParkingSlot> parkingLot;
    
    loadRecords(parkingLot);

    if (parkingLot.empty()) {
        for (int i = 1; i <= 10; ++i) {
            parkingLot.push_back({i, false, {"", "", MOTORCYCLE}, 0});
        }
    }

    int choice;
    do {
        displayMenu();
        std::cout << "Enter your choice: ";
        std::cin >> choice;
        clearInputBuffer();

        switch (choice) {
            case 1: parkVehicle(parkingLot); break;
            case 2: unparkVehicle(parkingLot); break;
            case 3: findVehicle(parkingLot); break;
            case 4: displayAvailableSlots(parkingLot); break;
            case 5: displayAllSlots(parkingLot); break;
            case 6: adminLogin(parkingLot); break;
            case 7: 
                std::cout << "Saving records and exiting. Goodbye!" << std::endl;
                saveRecords(parkingLot); 
                break;
            default: std::cout << "Invalid choice. Please try again." << std::endl; break;
        }
    } while (choice != 7);

    return 0;
}
