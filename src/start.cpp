/**
 * Author: Abhishek Verma
 * Author: Shiv Sinha
 * Date: 12/01/2022
 */

#include <iostream>
#include "sitemanager.h"
#include "transactionmanager.h"
#include "io.h"
#include "locktable.h"

#define NUM_SITES 10
#define NUM_VARIABLES 20

int main(int argc, char *argv[]) {
    if(argc != 2) {
        std::cout << "Usage: " << argv[0] << " <filename>" << std::endl;
        exit(1);
    }
    const std::string input_file{argv[1]};
    SiteManager* site_manager = new SiteManager(NUM_SITES, NUM_VARIABLES);
    LockTable* lock_table = new LockTable();
    TransactionManager* transaction_manager = new TransactionManager(NUM_VARIABLES, NUM_SITES, lock_table, site_manager);
    IO io{input_file, site_manager, transaction_manager, lock_table};
    io.run();
}

