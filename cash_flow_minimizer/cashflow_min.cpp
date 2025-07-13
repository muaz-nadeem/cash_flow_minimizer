#include <bits/stdc++.h>
#include <ctime> // For timestamps
using namespace std;

class Entity {
public:
    string name;
    int netAmount;
};

class Transaction {
public:
    string debtor;
    string creditor;
    int amount;
    time_t timestamp; // Timestamp for the transaction
    Transaction* next;

    Transaction(string debtor, string creditor, int amount) :
        debtor(debtor), creditor(creditor), amount(amount), next(nullptr) {
        timestamp = time(0); // Record current time when the transaction is created
    }

};

// Node for the BST
class TransactionNodeBST{
public:
    Transaction* transaction;
    TransactionNodeBST* left;
    TransactionNodeBST* right;

    TransactionNodeBST(Transaction* t) : transaction(t), left(nullptr), right(nullptr) {}
};

// BST Class for Sorting Transactions
class TransactionBST {
private:
    TransactionNodeBST* root;

    // Helper function to insert into the BST
    TransactionNodeBST* insert(TransactionNodeBST* node, Transaction* transaction) {
        if (!node) return new TransactionNodeBST(transaction);

        if (transaction->amount < node->transaction->amount) {
            node->left = insert(node->left, transaction);
        } else {
            node->right = insert(node->right, transaction);
        }
        return node;
    }

    // Helper function for in-order traversal
    void inOrderTraversal(TransactionNodeBST* node) {
        if (!node) return;

        inOrderTraversal(node->left);
        Transaction* t = node->transaction;
        cout << t->debtor << " owes " << t->creditor << " amount: " << t->amount
             << " at " << ctime(&(t->timestamp));
        inOrderTraversal(node->right);
    }

    // Helper function to delete the tree
    void deleteTree(TransactionNodeBST* node) {
        if (!node) return;

        deleteTree(node->left);
        deleteTree(node->right);
        delete node;
    }

public:
    TransactionBST() : root(nullptr) {}

    // Insert a transaction into the BST
    void insertTransaction(Transaction* transaction) {
        root = insert(root, transaction);
    }

    // Display transactions in sorted order
    void displaySortedTransactions() {
        cout << "\nTransactions Sorted by Amount (Ascending):\n";
        inOrderTraversal(root);
    }

    // Destructor to clean up the tree
    ~TransactionBST() {
        deleteTree(root);
    }
};

class TransactionManager {
private:
	Transaction* head; // Head of the linked list
    stack<Transaction*> undoStack; // Stack for undo history
    stack<Transaction*> redoStack; // Stack for redo history
    vector<Transaction> transactions;     // Current transactions

public:
	TransactionManager() : head(nullptr) {}
	
    // Add a new transaction to the linked list
    Transaction* addTransaction(const string& debtor, const string& creditor, int amount) {
        // Save the current head to the undo stack
        undoStack.push(head);

        // Clear redo stack since future redo history is invalidated
        while (!redoStack.empty()) {
            redoStack.pop();
        }

        // Create a new transaction and add it to the front of the linked list
        Transaction* newTransaction = new Transaction(debtor, creditor, amount);
        newTransaction->next = head;
        head = newTransaction;

        cout << "Transaction added: " << debtor << " owes " << amount << " to " << creditor << endl;
	return newTransaction;
    }

    // Undo the last transaction
    void undo() {
        if (undoStack.empty()) {
            cout << "No actions to undo.\n";
            return;
        }

        // Save current head to the redo stack
        redoStack.push(head);

        // Restore the previous state
        head = undoStack.top();
        undoStack.pop();

        cout << "Undo successful. Current transactions:\n";
        displayTransactions();
    }

    // Redo the last undone transaction
    void redo() {
        if (redoStack.empty()) {
            cout << "No actions to redo.\n";
            return;
        }

        // Save current head to the undo stack
        undoStack.push(head);

        // Restore the last undone state
        head = redoStack.top();
        redoStack.pop();

        cout << "Redo successful. Current transactions:\n";
        displayTransactions();
    }

    // Display all transactions
    void displayTransactions() {
        if (!head) {
            cout << "No transactions found.\n";
            return;
        }

        Transaction* current = head;
        while (current) {
            cout << current->debtor << " owes " << current->creditor << " amount: " << current->amount
                 << " at " << ctime(&(current->timestamp));
            current = current->next;
        }
    }

    // Clear all transactions
    void clearTransactions() {
        // Save current head to the undo stack
        undoStack.push(head);

        // Clear the linked list
        while (head) {
            Transaction* temp = head;
            head = head->next;
            delete temp;
        }

        // Clear redo stack
        while (!redoStack.empty()) {
            redoStack.pop();
        }

        cout << "All transactions have been cleared.\n";
    }

    // Destructor to clean up the linked list
    ~TransactionManager() {
        while (head) {
            Transaction* temp = head;
            head = head->next;
            delete temp;
        }
    }
};

// Global Variables for File Handling
const string DATA_FILE = "new_optimized_transactions.txt";
const string HISTORY_FILE = "new_transaction_history.txt";
const string USER_FILE = "user_accounts.txt";

bool authenticateUser() {
    ifstream userFile(USER_FILE);
    map<string, string> userAccounts; // Map to store username-password pairs

    if (userFile) {
        string username, password;
        while (userFile >> username >> password) {
            userAccounts[username] = password;
        }
        userFile.close();
    }

    cout << "1. Login\n2. Create New Account\nEnter your choice: ";
    int choice;
    cin >> choice;

    string username, password;
    switch (choice) {
        case 1:
        {
            cout << "Enter username: ";
            cin >> username;
            cout << "Enter password: ";
            cin >> password;
            if (userAccounts.find(username) != userAccounts.end() && userAccounts[username] == password) {
                cout << "Login successful. Welcome, " << username << "!\n";
                return true;
            } else {
                cout << "Invalid username or password.\n";
                return false;
            }
        }

        case 2:
        {
            cout << "Enter new username: ";
            cin >> username;
            if (userAccounts.find(username) != userAccounts.end()) {
                cout << "Username already exists.\n";
                return false;
            }
            cout << "Enter new password: ";
            cin >> password;
            userAccounts[username] = password;

            ofstream userFileOut(USER_FILE);
            for (const auto& account : userAccounts) {
                userFileOut << account.first << " " << account.second << "\n";
            }
            userFileOut.close();

            cout << "Account created successfully. Please login to continue.\n";
            return false;
        }

        default:
            cout << "Invalid choice.\n";
            return false;
    }
}


void saveData(const vector<vector<int>>& graph, const vector<string>& entityNames) {
    ofstream file(DATA_FILE);
    if (!file) {
        cerr << "Error: Unable to open file for saving data.\n";
        return;
    }

    // Save entities
    file << entityNames.size() << "\n";
    for (const string& name : entityNames) {
        file << name << "\n";
    }

    // Save graph
    int size = graph.size();
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            file << graph[i][j] << " ";
        }
        file << "\n";
    }

    file.close();
}

void loadData(vector<vector<int>>& graph, vector<string>& entityNames) {
    ifstream file(DATA_FILE);
    if (!file) {
        cerr << "\nNo previous data found. Starting fresh...\n";
        return;
    }

    int numEntities;
    file >> numEntities;
    entityNames.resize(numEntities);

    // Load entity names
    for (int i = 0; i < numEntities; i++) {
        file >> entityNames[i];
    }

    // Load graph
    graph.resize(numEntities, vector<int>(numEntities, 0));
    for (int i = 0; i < numEntities; i++) {
        for (int j = 0; j < numEntities; j++) {
            file >> graph[i][j];
        }
    }

    file.close();
}
// Merge Sort Functions for Sorting Entities by Net Amount
void merge(vector<Entity>& entities, int left, int mid, int right) {
    int n1 = mid - left + 1;
    int n2 = right - mid;

    vector<Entity> leftEntities(n1);
    vector<Entity> rightEntities(n2);

    for (int i = 0; i < n1; i++)
        leftEntities[i] = entities[left + i];
    for (int i = 0; i < n2; i++)
        rightEntities[i] = entities[mid + 1 + i];

    int i = 0, j = 0, k = left;
    while (i < n1 && j < n2) {
        if (leftEntities[i].netAmount <= rightEntities[j].netAmount) {
            entities[k] = leftEntities[i];
            i++;
        } else {
            entities[k] = rightEntities[j];
            j++;
        }
        k++;
    }

    while (i < n1) {
        entities[k] = leftEntities[i];
        i++;
        k++;
    }

    while (j < n2) {
        entities[k] = rightEntities[j];
        j++;
        k++;
    }
}
void mergeSort(vector<Entity>& entities, int left, int right) {
    if (left < right) {
        int mid = left + (right - left) / 2;

        mergeSort(entities, left, mid);
        mergeSort(entities, mid + 1, right);

        merge(entities, left, mid, right);
    }
}

void logTransaction(const string& log) {
    ofstream historyFile(HISTORY_FILE, ios::app);
    if (!historyFile) {
        cerr << "Error: Unable to open transaction history file.\n";
        return;
    }
    time_t now = time(0);
    char* dt = ctime(&now); // Convert time to a readable string
    historyFile << log << " | Time: " << dt; // Add timestamp to the log
    historyFile.close();
}

void viewTransactionHistory() {
    ifstream historyFile(HISTORY_FILE);
    if (!historyFile) {
        cerr << "No transaction history found.\n";
        return;
    }

    cout << "\nTransaction History with Timestamps:\n";
    string line;
    while (getline(historyFile, line)) {
        cout << line << "\n";
    }

    historyFile.close();
}

void minimizeCashFlow(vector<vector<int>>& graph, const vector<string>& entityNames) {
    int numEntities = graph.size();
    vector<int> netAmounts(numEntities, 0);
    vector<Entity> transactionList; // To store transactions

    // Calculate net amounts
    for (int i = 0; i < numEntities; i++) {
        for (int j = 0; j < numEntities; j++) {
            netAmounts[j] += graph[i][j];
            netAmounts[j] -= graph[j][i];
        }
    }

    bool allZero = all_of(netAmounts.begin(), netAmounts.end(), [](int amount) { return amount == 0; });
    if (allZero) {
        cout << "\nNo transactions are required as all debts are already settled.\n";
        return;
    }

    cout << "\nOptimized Transactions:\n";
    while (true) {
        int maxCreditor = max_element(netAmounts.begin(), netAmounts.end()) - netAmounts.begin();
        int maxDebtor = min_element(netAmounts.begin(), netAmounts.end()) - netAmounts.begin();

        if (netAmounts[maxCreditor] == 0 && netAmounts[maxDebtor] == 0) break;

        int minTransaction = min(abs(netAmounts[maxDebtor]), netAmounts[maxCreditor]);
        netAmounts[maxDebtor] += minTransaction;
        netAmounts[maxCreditor] -= minTransaction;

        string log = entityNames[maxDebtor] + " pays " + to_string(minTransaction) + " to " + entityNames[maxCreditor];
        cout << log << "\n";

        // Add transaction to list for sorting later
        transactionList.push_back({entityNames[maxDebtor] + " pays " + entityNames[maxCreditor], minTransaction});

        logTransaction(log);
    }

    // Sort transactions by amount using mergeSort
    mergeSort(transactionList, 0, transactionList.size() - 1);

    // Display sorted transactions
    cout << "\nSorted Optimized Transactions by Amount:\n";
    for (const auto& t : transactionList) {
        cout << t.name << " amount: " << t.netAmount << "\n";
    }
}

void addTransaction(vector<vector<int>>& graph, const vector<string>& entityNames, unordered_map<string, int>& indexMap, TransactionManager &manager, TransactionBST &bst) {
    string debtor, creditor;
    int amount;

    cout << "Enter Debtor Name: ";
    cin >> debtor;
    cout << "Enter Creditor Name: ";
    cin >> creditor;
    cout << "Enter Amount: ";
    cin >> amount;

    if (indexMap.find(debtor) == indexMap.end() || indexMap.find(creditor) == indexMap.end()) {
        cout << "Error: One or both entities not found.\n";
        return;
    }

    Transaction* newTransaction = manager.addTransaction(debtor, creditor, amount);
    bst.insertTransaction(newTransaction);

    graph[indexMap[debtor]][indexMap[creditor]] += amount;
    string log = debtor + " pays " + to_string(amount) + " to " + creditor;
    ofstream historyFile("final_transaction_history.txt", ios::app);
    historyFile << log << endl;
	
}
void addEntity(vector<string>& entityNames, vector<vector<int>>& graph, unordered_map<string, int>& indexMap) {
    string name;
    cout << "Enter New Entity Name: ";
    cin >> name;

    if (indexMap.find(name) != indexMap.end()) {
        cout << "Entity already exists.\n";
        return;
    }

    entityNames.push_back(name);
    indexMap[name] = entityNames.size() - 1;

    for (auto& row : graph) {
        row.push_back(0);
    }
    graph.push_back(vector<int>(entityNames.size(), 0));
}
void viewPersonTransactions(const string& person) {
    ifstream optimizedFile(DATA_FILE);
    if (!optimizedFile) {
        cout << "No optimized transaction data found.\n";
        return;
    }

    int numEntities;
    optimizedFile >> numEntities;

    vector<string> entityNames(numEntities);
    for (int i = 0; i < numEntities; i++) {
        optimizedFile >> entityNames[i];
    }

    vector<vector<int>> graph(numEntities, vector<int>(numEntities, 0));
    for (int i = 0; i < numEntities; i++) {
        for (int j = 0; j < numEntities; j++) {
            optimizedFile >> graph[i][j];
        }
    }

    optimizedFile.close();

    // Find transactions involving the person
    int personIndex = -1;
    for (int i = 0; i < numEntities; i++) {
        if (entityNames[i] == person) {
            personIndex = i;
            break;
        }
    }

    if (personIndex == -1) {
        cout << "Person " << person << " not found in the transaction data.\n";
        return;
    }

    cout << "\nPending Transactions involving " << person << ":\n";
    bool found = false;

    for (int i = 0; i < numEntities; i++) {
        if (graph[personIndex][i] > 0) {
            cout << person << " pays " << graph[personIndex][i] << " to " << entityNames[i] << "\n";
            found = true;
        }
        if (graph[i][personIndex] > 0) {
            cout << entityNames[i] << " pays " << graph[i][personIndex] << " to " << person << "\n";
            found = true;
        }
    }

    if (!found) {
        cout << "No pending transactions found involving " << person <<".\n";
        }
}

int main() {

    vector<vector<int>> graph;
    vector<string> entityNames;
    unordered_map<string, int> indexMap;
    TransactionManager manager;
    TransactionBST bst;

   cout << "\n\t\t********************* Welcome to CASH FLOW MINIMIZER SYSTEM ***********************\n\n";
   cout << "This system minimizes the number of transactions among multiple entities.\n\n";

   if (!authenticateUser()) {
        return 0;
    }

   loadData(graph, entityNames);
    for (int i = 0; i < entityNames.size(); i++) {
        indexMap[entityNames[i]] = i;
    }

    while (true) {
        cout << "\nMenu:\n";
        cout << "1. Add New Entity\n";
        cout << "2. Add Transaction\n";
        cout << "3. Minimize Transactions\n";
        cout << "4. View Transaction History\n";
        cout << "5. Undo Last Transaction\n";
        cout << "6. Redo Last Transaction\n";
        cout << "7. Clear All Transaction\n";
        cout << "8. View Latest Transaction\n";
        cout << "9. View Specific Person Transaction\n";
        cout << "10. Sorted Transactions using BST\n";
        cout << "11. Exit\n";

        cout << "\nEnter your choice: ";
        int choice;
        cin >> choice;

        switch (choice) {
            case 1:
                addEntity(entityNames, graph, indexMap);
                break;

            case 2:
                addTransaction(graph, entityNames, indexMap, manager, bst);
                break;
                
            case 3:
                minimizeCashFlow(graph, entityNames);
                break;

            case 4:
                viewTransactionHistory();
                break;
            
            case 5:
                manager.undo();
                break;

            case 6:
                manager.redo();
                break;

            case 7:
                manager.clearTransactions();
                break;

            case 8:
                cout << "\nCurrent Transactions:\n";
                manager.displayTransactions();
                break;
            
            case 9:
               {
                  string person;
                  cout << "Enter person's name: ";
                  cin >> person;
                  viewPersonTransactions(person);
               }
               break;

            case 10:
                bst.displaySortedTransactions();
                break;
            
            case 11:
                saveData(graph, entityNames);
                cout << "\nProgram Ended! Data is successfully saved.\n";
                return 0;

            default:
                cout << "Invalid choice. Please try again.\n";
      }
   }
}
