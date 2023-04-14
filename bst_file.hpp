//
// Created by juandiego on 4/13/23.
//

#ifndef BST_FILE_HPP
#define BST_FILE_HPP

#include <iostream>
#include <sstream>
#include <fstream>
#include <utility>

#define null (-1)
#define initial_record (0)

/// Console clear command
#if defined(_WIN32) || defined(_WIN64)
inline const char * clear_console = "cls";
#else
inline const char *clear_console = "clear";
#endif

using text = char *;

void read_from_console(char buffer[], int size) {
    std::string temp;
    std::getline(std::cin >> std::ws, temp, '\n');
    std::cin.clear();

    for (int i = 0; i < size; i++) {
        buffer[i] = (i < temp.size()) ? temp[i] : ' ';
    }

    buffer[size - 1] = '\0';
}

struct Record {
    char cod[5];
    char name[20];
    int cycle;

    std::string to_string() {
        std::stringstream ss;
        ss << "(" << cod << ", " << name << ", " << cycle << ")";
        return ss.str();
    }
};

constexpr int long_sz = sizeof(long);
constexpr int record_sz = sizeof(Record);

class BSTFile {
private:

    long root;
    std::fstream file;
    std::string file_name;

    Record find(long record_pos, char value[5]) {
        if (record_pos == null) {
            file.close();
            throw std::runtime_error("The record with code " + std::string(value) + " do not exists");
        }

        Record current{};
        long left = null, right = null;

        file.seekg(record_pos);
        file.read((text) &current, record_sz);
        file.read((text) &left, long_sz);
        file.read((text) &right, long_sz);

        if (std::string(current.cod) > std::string(value)) {
            return find(left, value);
        } else if (std::string(current.cod) < std::string(value)) {
            return find(right, value);
        }

        return current;
    }

    long insert(long record_pos, Record &record) {
        long left = null, right = null;

        if (record_pos == null) {
            file.open(file_name, std::ios::app | std::ios::binary);
            long insertion_position = file.tellp();
            file.write((text) &record, record_sz);
            file.write((text) &left, long_sz);
            file.write((text) &right, long_sz);
            file.close();
            return insertion_position;
        }

        Record current{};
        file.open(file_name, std::ios::ate | std::ios::in | std::ios::binary);
        file.seekg(record_pos);
        file.read((text) &current, record_sz);
        file.read((text) &left, long_sz);
        file.read((text) &right, long_sz);
        file.close();

        long inserted_pos;
        if (std::string(record.cod) < std::string(current.cod)) {
            inserted_pos = insert(left, record);

            if (left == null) {
                file.open(file_name, std::ios::in | std::ios::out | std::ios::binary);
                file.seekp(record_pos + record_sz);
                file.write((text) &inserted_pos, long_sz);
                file.close();
            }
            return EXIT_SUCCESS;
        } else if (std::string(record.cod) > std::string(current.cod)) {
            inserted_pos = insert(right, record);

            if (right == null) {
                file.open(file_name, std::ios::in | std::ios::out | std::ios::binary);
                file.seekp(record_pos + record_sz + long_sz);
                file.write((text) &inserted_pos, long_sz);
                file.close();
            }
            return EXIT_SUCCESS;
        }

        throw std::runtime_error("Repeated primary key: " + std::string(record.cod));
    }

public:

    explicit BSTFile(std::string file_name) : file_name(std::move(file_name)) {
        file.open(this->file_name, std::ios::in);
        this->root = (file.peek() == std::ifstream::traits_type::eof()) ? null : initial_record;
        file.close();
    }

    Record find(char value[5]) {
        file.open(file_name, std::ios::in | std::ios::binary);
        Record result = find(this->root, value);
        file.close();
        return result;
    }

    void insert(Record &record) {
        long inserted_position = insert(this->root, record);
        root = ((root == null) ? inserted_position : root);
    }
};

#endif //BST_FILE_HPP