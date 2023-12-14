#include <iostream>
#include <unordered_map>
#include <vector>
using namespace std;

struct Transaction {
    int value;
    bool commit;
};

class InMemoryDB {
private:
    unordered_map<string, Transaction> data;
    vector<unordered_map<string, Transaction>> transactionStack;
    bool inProgress = false;

public:
    void begin_transaction() {
        transactionStack.push_back(data);
        inProgress = true;
    }

    void put(const string& key, int value) {
        if (!inProgress) {
            std::cout << "Error: Transaction not in progress" << std::endl;
            return;
        }
        Transaction temp;
        temp.commit = false;
        temp.value = value;
        data[key] = temp;
    }

    int get(const string& key) const {
        auto it = data.find(key);
        if (it != data.end()) {
            if (it->second.commit) {
                return it->second.value;
            }
            else{
                return -1;
            }
        } else {
            return -1; // Assuming default value is -1 if key not found
        }
    }

    void commit() {
        if (!inProgress) {
            std::cout << "Error: No transaction to commit" << std::endl;
            return;
        }

        for (auto it = data.begin(); it != data.end(); ++it) {
            if (!it->second.commit) {
                it->second.commit = true;
            }
        }

        inProgress = false;
    }

    void rollback() {
        if (!inProgress) {
            std::cout << "Error: No transaction to rollback" << std::endl;
            return;
        }
        else {
            data = transactionStack.at(transactionStack.size() - 2);
            transactionStack.pop_back();
        }
    }
};

int main() {
    InMemoryDB inmemoryDB;

    // should return null, because A doesn’t exist in the DB yet
    cout << inmemoryDB.get("A") << endl;

// should throw an error because a transaction is not in progress
    inmemoryDB.put("A", 5);

// starts a new transaction
    inmemoryDB.begin_transaction();

// set’s value of A to 5, but its not committed yet
    inmemoryDB.put("A", 5);

// should return null, because updates to A are not committed yet
    cout << inmemoryDB.get("A") << endl;

// update A’s value to 6 within the transaction
    inmemoryDB.put("A", 6);

// commits the open transaction
    inmemoryDB.commit();

// should return 6, that was the last value of A to be committed
    cout << inmemoryDB.get("A") << endl;

// throws an error, because there is no open transaction
    inmemoryDB.commit();

// throws an error because there is no ongoing transaction
    inmemoryDB.rollback();

// should return null because B does not exist in the database
   cout << inmemoryDB.get("B") << endl;

// starts a new transaction
    inmemoryDB.begin_transaction();

// Set key B’s value to 10 within the transaction
    inmemoryDB.put("B", 10);

// Rollback the transaction - revert any changes made to B
    inmemoryDB.rollback();

// Should return null because changes to B were rolled back
    cout << inmemoryDB.get("B") << endl;


    return 0;
}
