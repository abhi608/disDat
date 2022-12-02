#include <iostream>
#include "sitemanager.h"
#include "transactionmanager.h"
#include "io.h"
#include "locktable.h"

int main() {
    constexpr int32_t num_sites{10};
    constexpr int32_t num_variables{20};
    const std::string input_file{"./test.in"};
    SiteManager* site_manager = new SiteManager(num_sites, num_variables);
    LockTable* lock_table = new LockTable();
    TransactionManager* transaction_manager = new TransactionManager(num_variables, num_sites, lock_table, site_manager);
    IO io{input_file, site_manager, transaction_manager, lock_table};
    io.run();
}

