// Sizwe Tafane, Student # 53846273
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <map>
#include <iomanip>  // For percentage formatting
#include <algorithm> // For all_of
#include <cctype>    // For isdigit

using namespace std;

// Helper function to trim whitespace from both ends of a string
string trim(const string& str) {
    size_t first = str.find_first_not_of(" \t\n\r");
    size_t last = str.find_last_not_of(" \t\n\r");
    return (first == string::npos || last == string::npos) ? "" : str.substr(first, last - first + 1);
}

// Class to hold each traffic record
class TrafficRecord {
private:
    string timestamp;
    string vehicleType;
    int speed;

public:
    // Constructor
    TrafficRecord(const string& t, const string& v, int s) : timestamp(t), vehicleType(v), speed(s) {}

    // Getter methods
    string getTimestamp() const { return timestamp; }
    string getVehicleType() const { return vehicleType; }
    int getSpeed() const { return speed; }

    // Extract hour from timestamp (in format yyyy:mm:dd hh:mm)
    int getHour() const {
        string hourStr = timestamp.substr(11, 2); // Extract the hour from the timestamp
        return stoi(hourStr); // Convert hour string to integer
    }

    // Extract day from timestamp (in format yyyy:mm:dd hh:mm)
    int getDay() const {
        string dayStr = timestamp.substr(8, 2); // Extract the day from the timestamp
        return stoi(dayStr); // Convert day string to integer
    }
};

// Function to read the file and store traffic data, skipping the first line (header)
vector<TrafficRecord> readTrafficData(const string& filePath) {
    vector<TrafficRecord> records;
    ifstream file(filePath);
    string line;

    if (file.is_open()) {
        // Skip the first line (header)
        getline(file, line);

        // Read the rest of the lines
        while (getline(file, line)) {
            stringstream ss(line);
            string timestamp, vehicleType, speedStr;
            int speed;

            // Parsing the data
            getline(ss, timestamp, ';');
            getline(ss, vehicleType, ';');
            getline(ss, speedStr); // Read the remaining data, which is the speed (no ';' after it)

            // Trim any extra whitespace from the speed string
            speedStr = trim(speedStr);

            // Check if the speed string is valid (not empty and contains digits)
            if (!speedStr.empty() && all_of(speedStr.begin(), speedStr.end(), ::isdigit)) {
                try {
                    speed = stoi(speedStr); // Convert the trimmed speed string to an integer
                } catch (invalid_argument& e) {
                    cout << "Invalid speed value in the file: " << speedStr << endl;
                    continue; // Skip this line if conversion fails
                }
            } else {
                cout << "Invalid speed entry (empty or non-numeric): " << speedStr << endl;
                continue; // Skip the invalid entry
            }

            // Create an object of TrafficRecord and store it in the vector
            TrafficRecord record(timestamp, vehicleType, speed);
            records.push_back(record);
        }
        file.close();
    } else {
        cout << "Unable to open file!" << endl;
    }

    return records;
}

// Report 1: Traffic Summary Report
void generateTrafficSummaryReport(const vector<TrafficRecord>& records) {
    int totalVehicles = records.size();
    map<int, int> vehiclesPerDay;
    map<string, int> vehicleTypeCount;

    for (const auto& record : records) {
        vehiclesPerDay[record.getDay()]++;
        vehicleTypeCount[record.getVehicleType()]++;
    }

    // Summary
    cout << "======== Report 1: Traffic Summary Report ========" << endl;
    cout << "" << endl;
    cout << "Total vehicles: " << totalVehicles << endl;

    // Vehicle count per day
    for (const auto& entry : vehiclesPerDay) {
        cout << "Day " << entry.first << ": " << entry.second << " vehicles" << endl;
    }

    // Vehicle type distribution
    cout << "\nVehicle distribution:" << endl;
    for (const auto& entry : vehicleTypeCount) {
        double percentage = (entry.second / static_cast<double>(totalVehicles)) * 100;
        cout << "  - " << entry.first << ": " << entry.second << " (" << fixed << setprecision(2) << percentage << "%)" << endl;
    }
}

// Report 2: Peak Traffic Times Report
void generatePeakTrafficTimesReport(const vector<TrafficRecord>& records) {
    map<int, int> hourVehicleCount;
    map<int, map<int, int>> dailyTraffic;

    for (const auto& record : records) {
        int hour = record.getHour();
        hourVehicleCount[hour]++;
        dailyTraffic[record.getDay()][hour]++;
    }

    // Find peak hour overall
    int peakHour = 0;
    int maxVehicles = 0;
    for (const auto& entry : hourVehicleCount) {
        if (entry.second > maxVehicles) {
            peakHour = entry.first;
            maxVehicles = entry.second;
        }
    }

    // Peak hour by day
    cout << "\n======== Report 2: Peak Traffic Times Report ========\n" << endl;
    cout << "Peak traffic time (overall): " << peakHour << ":00 - "<< peakHour+1 << ":00, with " << maxVehicles << " vehicles" << endl;

    cout << "Peak traffic by day:" << endl;
    for (const auto& dailyEntry : dailyTraffic) {
        int peakHourForDay = 0;
        int maxVehiclesForDay = 0;
        for (const auto& hourEntry : dailyEntry.second) {
            if (hourEntry.second > maxVehiclesForDay) {
                peakHourForDay = hourEntry.first;
                maxVehiclesForDay = hourEntry.second;
            }
        }
        cout << "  - Day " << dailyEntry.first << ": " << peakHourForDay << ":00 - "<< peakHourForDay+1 << ":00, " << maxVehiclesForDay << " vehicles" << endl;
    }
}

// Report 3: Congestion Level Report
void generateCongestionLevelReport(const vector<TrafficRecord>& records) {
    map<int, int> vehiclesPerDay;
    map<int, map<int, int>> hourlyTraffic;

    for (const auto& record : records) {
        vehiclesPerDay[record.getDay()]++;
        hourlyTraffic[record.getDay()][record.getHour()]++;
    }

    cout << "\n======== Report 3: Congestion Level Report ========\n" << endl;

    // Calculate and display congestion by day
    for (const auto& entry : vehiclesPerDay) {
        string congestionLevel;
        if (entry.second > 300) {
            congestionLevel = "High";
        } else if (entry.second > 150) {
            congestionLevel = "Moderate";
        } else {
            congestionLevel = "Low";
        }
        cout << "Day " << entry.first << ": " << congestionLevel << " (" << entry.second << " vehicles)" << endl;
    }

    // Find the most congested hour
    int mostCongestedDay = 0, mostCongestedHour = 0, mostVehicles = 0;
    for (const auto& dayEntry : hourlyTraffic) {
        for (const auto& hourEntry : dayEntry.second) {
            if (hourEntry.second > mostVehicles) {
                mostCongestedDay = dayEntry.first;
                mostCongestedHour = hourEntry.first;
                mostVehicles = hourEntry.second;
            }
        }
    }

    cout << "\nMost congested period: Day " << mostCongestedDay << ", " << mostCongestedHour << ":00 - " << mostCongestedHour+1<<":00 ,with "<< mostVehicles << " vehicles\n" << endl;
}

// Main function
int main() {
    string filePath = "c:\\data\\trafficdata.txt"; // Update the file path accordingly
    vector<TrafficRecord> trafficData = readTrafficData(filePath);

    if (!trafficData.empty()) {
        generateTrafficSummaryReport(trafficData);
        generatePeakTrafficTimesReport(trafficData);
        generateCongestionLevelReport(trafficData);
    }

    return 0;
}




