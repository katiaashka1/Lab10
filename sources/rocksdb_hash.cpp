// Copyright 2018 Your Name <your_email>

#include <header.hpp>
rocksdb_hash::rocksdb_hash(const std::string &_path_to_db,
                           const std::string &_path_to_output_db){
    log_init();
    path_to_output_db = _path_to_output_db;
    Status s = input_db->ListColumnFamilies(
            DBOptions(), _path_to_db, &list_column_families);

    for (auto it : list_column_families)
    {
        column_families.push_back(ColumnFamilyDescriptor(
                it.c_str(), ColumnFamilyOptions()));
    }

    Status status = DB::Open(
            DBOptions(), _path_to_db, column_families, &handles, &input_db);
    if (!status.ok()) {
        if (log_lvl == ERROR) {
            BOOST_LOG_TRIVIAL(error) << "Error" << std::endl;
        }
    }
    assert(s.ok());
}

void rocksdb_hash::print_db(){
    for (unsigned i = 0; i < handles.size(); ++i)
    {
        BOOST_LOG_TRIVIAL(trace) << "Name: "
                                 << column_families[i].name << std::endl;
        auto it = input_db->NewIterator(
                ReadOptions(), handles[i]);
        it->SeekToFirst();

        for (; it->Valid(); it->Next())
        {
            BOOST_LOG_TRIVIAL(trace) << it->key().ToString()
                                     << " : " << it->value().ToString()
                                     << std::endl;
        }
    }
}
void rocksdb_hash::print_output_db(){
    for (unsigned i = 0; i < o_handles.size(); ++i)
    {
        BOOST_LOG_TRIVIAL(trace) << "Name: "
                                 << o_column_families[i].name << std::endl;
        auto it = hash_db->NewIterator(
                ReadOptions(), o_handles[i]);
        it->SeekToFirst();

        for (; it->Valid(); it->Next())
        {
            BOOST_LOG_TRIVIAL(trace) << it->key().ToString()
                                     << " : " << it->value().ToString()
                                     << std::endl;
        }
    }
}
void rocksdb_hash::create_hash_db(){
    open_output();

    for (unsigned i = 0; i < handles.size(); ++i)
    {
        auto it = input_db->NewIterator(ReadOptions(), handles[i]);
        it->SeekToFirst();
        while (it->Valid())
        {
            std::string value;
            hash_db->Put(WriteOptions(), o_handles[i], it->key().ToString(),
                         picosha2::hash256_hex_string(
                                 it->key().ToString() +
                                 it->value().ToString()));
            it->Next();
        }
    }
    auto iter = input_db->NewIterator(ReadOptions());
    for (iter->SeekToFirst(); iter->Valid(); iter->Next()) {
        hash_db->Put(WriteOptions(), iter->key().ToString(),
                     picosha2::hash256_hex_string(
                             iter->key().ToString() +
                             iter->value().ToString()));
    }
}

void rocksdb_hash::close_db(){
    for (auto handle : handles) {
        Status s = input_db->DestroyColumnFamilyHandle(handle);
        if (!s.ok()) {
            if (log_lvl == ERROR) {
                BOOST_LOG_TRIVIAL(error) << "Error" << std::endl;
            }
        }
        assert(s.ok());
    }
    delete input_db;

    for (auto o_handle : o_handles) {
        if (o_handle->GetName() == DEFAULT) continue;
        Status s = hash_db->DestroyColumnFamilyHandle(o_handle);
        if (!s.ok()) {
            if (log_lvl == ERROR) {
                BOOST_LOG_TRIVIAL(error) << "Error" << std::endl;
            }
        }
        assert(s.ok());
    }
    delete hash_db;
}

void rocksdb_hash::open_output(){
    Options options;
    options.create_if_missing = true;
    Status s = DB::Open(options, path_to_output_db, &hash_db);
    if (!s.ok()) {
        if (log_lvl == ERROR) {
            BOOST_LOG_TRIVIAL(error) << "Error" << std::endl;
        }
    }
    assert(s.ok());
    for (auto it : list_column_families)
    {
        if (it == "default") {
            o_column_families.push_back(ColumnFamilyDescriptor(
                    it.c_str(), ColumnFamilyOptions()));
            ColumnFamilyHandle* cf;
            cf = hash_db->DefaultColumnFamily();
            o_handles.emplace_back(cf);
            continue;
        }
        o_column_families.push_back(ColumnFamilyDescriptor(
                it.c_str(), ColumnFamilyOptions()));

        ColumnFamilyHandle* cf;
        Status status = hash_db->CreateColumnFamily(
                            ColumnFamilyOptions(), it, &cf);
        o_handles.emplace_back(cf);
        if (!status.ok()) {
            if (log_lvl == ERROR) {
                BOOST_LOG_TRIVIAL(error) << "Error" << std::endl;
            }
        }

        assert(s.ok());
    }
}

void rocksdb_hash::log_init()
{
    boost::log::add_console_log
            (
                    std::cout,
                    boost::log::keywords::format =
                            output_format);
    boost::log::add_common_attributes();
}

void rocksdb_hash::start(std::string log_level){
    log_lvl = log_level;
    this->print_db();
    this->create_hash_db();
    this->print_output_db();
    this->close_db();
}
