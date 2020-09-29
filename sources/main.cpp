// Copyright 2018 Your Name <your_email>
#include <header.hpp>
int main(int argc, char **argv) {
    po::options_description desc(SHORT_DESCRIPTION);
    desc.add_options()
            (HELP, ans_HELP)
            (LOG_LEVEL, po::value<std::string>(), ans_LOG)
            (THREAD_COUNT, po::value<unsigned>(), ans_THREAD)
            (OUTPUT, po::value<std::string>(), ans_OUTPUT);

    po::variables_map vm;
    try {
        po::store(po::parse_command_line(argc, argv, desc), vm);
        po::notify(vm);
    }

    catch (po::error &e) {
        cout << e.what() << endl;
        cout << desc << endl;
        return 1;
    }
    if (vm.count(HELP)) {
        cout << desc << endl;
        return 1;
    }
    if (!vm.count(LOG_LEVEL)
        || !vm.count(THREAD_COUNT)
        || !vm.count(OUTPUT)) {
        cout << ERR << endl << desc << endl;
        return 1;
    }

    std::string logLevel = vm[LOG_LEVEL].as<std::string>();
    std::size_t threadCount = vm[THREAD_COUNT].as<unsigned>();
    std::string pathToDB = vm[OUTPUT].as<std::string>();
    std::string pathToResult = PATH_TO_OUTPUT;

    rocksdb_hash db(pathToDB, pathToResult);
    db.start(logLevel);
    return 0;
}
