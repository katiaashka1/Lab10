// Copyright 2018 Your Name <your_email>

#ifndef INCLUDE_HEADER_HPP_
#define INCLUDE_HEADER_HPP_
#include <iostream>
#include <rocksdb/db.h>
#include <rocksdb/slice.h>
#include <rocksdb/options.h>
#include <picosha2.h>
#include <boost/program_options.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/utility/setup/console.hpp>
#include <string>
#include <vector>


using rocksdb::DB;
using rocksdb::DBOptions;
using rocksdb::ColumnFamilyOptions;
using rocksdb::ReadOptions;
using rocksdb::Status;
using rocksdb::WriteOptions;
using rocksdb::Options;
using rocksdb::ColumnFamilyDescriptor;
using rocksdb::ColumnFamilyHandle;
using std::cout;
using std::endl;

namespace po = boost::program_options;

const char SHORT_DESCRIPTION[] = "short description";
const char HELP[] = "help";
const char LOG_LEVEL[] = "log_level";
const char THREAD_COUNT[] = "thread_count";
const char OUTPUT[] = "output";
const char ERR[] = "error: bad format";
const char DEFAULT[] = "default";
const char ans_HELP[] = "help";
const char ans_LOG[] = "trace|info|error default: trace";
const char ans_THREAD[] = "default: count of logical core";
const char ans_OUTPUT[] = "path to output";
const char PATH_TO_OUTPUT[] = "/home/mlkwov/Desktop/res";
const char ERROR[] = "error";

const char output_format[] = "[%Severity%] %Message%";

class rocksdb_hash{
public:
    rocksdb_hash(const std::string &_path_to_db,
                 const std::string &_path_to_output_db);

    void print_db();
    void print_output_db();
    void create_hash_db();

    void close_db();

    void open_output();
    void log_init();
    void start(std::string log_level);


private:
    DB* input_db;
    DB* hash_db;
    std::string path_to_output_db;
    std::vector<ColumnFamilyDescriptor> column_families;
    std::vector<ColumnFamilyHandle*> handles;
    std::vector<std::string> list_column_families;
    std::vector<ColumnFamilyDescriptor> o_column_families;
    std::vector<ColumnFamilyHandle*> o_handles;
    std:: string log_lvl;
};
#endif // INCLUDE_HEADER_HPP_
