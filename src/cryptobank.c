#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// colors
#define CBOLD "\x1b[1m"
#define CRED "\x1b[31m"
#define CGREEN "\x1b[32m"
#define CYELLOW "\x1b[33m"
#define CBLUE "\x1b[34m"
#define COFF "\x1b[0m"

typedef enum {
    BitCoin = 1,
    Ether = 2,
    PizzaCoin = 3,
    InsaneCoin = 4,
} Currency;

// forward declaration
typedef struct Account Account;

typedef struct {
    Account* src;
    Account* dest;
    char* comment;
    unsigned amount;
} Transaction;

typedef struct {
    Transaction* ptr;
    size_t size;
    size_t capacity;
} TransactionArray;

struct Account {
    char* username;
    char* password;
    Currency currency;
    unsigned balance;
    Account* favorite_recipient;
    TransactionArray transactions;
};

typedef struct {
    Account* ptr;
    size_t size;
    size_t capacity;
} AccountArray;

// Global variables

static AccountArray accounts;

// Accounts management

static void init_bank() {
    accounts.size = 0;
    accounts.capacity = 4;
    accounts.ptr = (Account*)malloc(sizeof(Account) * accounts.capacity);
}

static void free_bank() {
    for (size_t i = 0; i < accounts.size; i++) {
        free(accounts.ptr[i].username);
        free(accounts.ptr[i].password);
        free(accounts.ptr[i].transactions.ptr);
    }
    free(accounts.ptr);
}

static void add_account(Account account) {
    if (accounts.size == accounts.capacity) {
        accounts.capacity *= 2;
        accounts.ptr = (Account*)realloc(accounts.ptr,
                                         sizeof(Account) * accounts.capacity);
    }
    accounts.ptr[accounts.size] = account;
    accounts.size++;
}

static Account* find_account(char* username) {
    for (size_t i = 0; i < accounts.size; i++) {
        Account* account = &accounts.ptr[i];
        if (strcmp(account->username, username) == 0) {
            return account;
        }
    }

    return NULL;
}

static Account* login(char* username, char* password) {
    Account* account = find_account(username);

    if (account == NULL) {
        return NULL;
    }

    if (strcmp(account->password, password) != 0) {
        return NULL;
    }

    return account;
}

// Accounts management

static TransactionArray new_transaction_array() {
    TransactionArray transactions;
    transactions.size = 0;
    transactions.capacity = 4;
    transactions.ptr =
        (Transaction*)malloc(sizeof(Transaction) * transactions.capacity);
    return transactions;
}

static void add_transaction(TransactionArray* array, Transaction tr) {
    if (array->size == array->capacity) {
        array->capacity *= 2;
        array->ptr = (Transaction*)realloc(
            array->ptr, sizeof(Transaction) * array->capacity);
    }
    array->ptr[array->size] = tr;
    array->size++;
}

// Helper

static const char* currency_name(Currency currency) {
    switch (currency) {
        case BitCoin:
            return "BitCoin";
        case Ether:
            return "Ether";
        case PizzaCoin:
            return "PizzaCoin";
        case InsaneCoin:
            return "InsaneCoin";
        default:
            return "???";
    }
}

// Logged-in user menu and actions

typedef enum {
    ShowStatement = 1,
    MakeTransfer = 2,
    SetFavoriteRecipient = 3,
    MakeTransferFavoriteRecipient = 4,
    UpdateTransactionComment = 5,
    LogOff = 6
} UserAction;

static void user_menu() {
    fputs(
        "\n"
        "Available commands:\n"
        "  1. Show my statement\n"
        "  2. Make a transfer\n"
        "  3. Set your favorite recipient for transfers\n"
        "  4. Make a quick transfer to your favorite recipent\n"
        "  5. Update a comment on a transaction\n"
        "  6. Log Off\n",
        stdout);
}

static UserAction user_action() {
    char input[10];

    fputs("> ", stdout);
    fflush(stdout);

    scanf("%s", input);
    int n = atoi(input);
    return (UserAction)n;
}

static void user_statement(Account* account) {
    printf("\n");
    printf("Username: '%s'\n", account->username);
    printf("Balance: %d %s\n", account->balance,
           currency_name(account->currency));
    printf("\n");
    if (account->transactions.size == 0) {
        printf("No transactions\n");
    } else {
        printf("Transactions:\n");
        for (size_t i = 0; i < account->transactions.size; i++) {
            Transaction* tr = &account->transactions.ptr[i];
            printf("  From: '%s' To: '%s' Amount: %d Comment: '%s'\n",
                   tr->src->username, tr->dest->username, tr->amount,
                   tr->comment);
        }
    }
}

static void user_transfer(Account* account) {
    char username[30];
    unsigned amount;

    fputs(
        "\n"
        "Transfer form:\n"
        "\n",
        stdout);

    fputs("Username of recipient: ", stdout);
    fflush(stdout);
    scanf("%s", username);

    fputs("Amount: ", stdout);
    fflush(stdout);
    scanf("%d", &amount);

    char* comment = (char*)malloc(sizeof(char) * 30);
    fputs("Comment (optional): ", stdout);
    fflush(stdout);
    getchar();  // read '\n'
    fgets(comment, 30, stdin);
    comment[strlen(comment) - 1] = '\0';  // remove extra '\n'

    Account* recipient = find_account(username);

    account->balance = account->balance - amount;
    recipient->balance = recipient->balance + amount;

    Transaction tr = {.src = account,
                      .dest = recipient,
                      .amount = amount,
                      .comment = comment};
    add_transaction(&account->transactions, tr);
    add_transaction(&recipient->transactions, tr);

    fputs(
        "\n"
        "Transfer done successfully.\n",
        stdout);
}

static void user_set_favorite_recipient(Account* account) {
    char username[30];

    fputs(
        "\n"
        "Setting your favorite recipient can be used to facilitate transfers.\n"
        "\n",
        stdout);

    fputs("Username of your favorite recipient: ", stdout);
    fflush(stdout);
    scanf("%s", username);

    Account* recipient = find_account(username);
    account->favorite_recipient = recipient;

    fputs(
        "\n"
        "Favorite recipient set successfully.\n",
        stdout);
}

static void user_transfert_favorite_recipient(Account* account) {
    unsigned amount;
    Account* recipient = account->favorite_recipient;

    if (recipient == NULL) {
        fputs("Please set your favorite recipient first.\n", stdout);
        return;
    }

    printf("Your favorite recipient: %s\n", recipient->username);

    fputs("Amount: ", stdout);
    fflush(stdout);
    scanf("%d", &amount);

    char* comment = (char*)malloc(sizeof(char) * 30);
    fputs("Comment (optional): ", stdout);
    fflush(stdout);
    getchar();  // read '\n'
    fgets(comment, 30, stdin);
    comment[strlen(comment) - 1] = '\0';  // remove extra '\n'

    account->balance = account->balance - amount;
    recipient->balance = recipient->balance + amount;

    Transaction tr = {.src = account,
                      .dest = recipient,
                      .amount = amount,
                      .comment = comment};
    add_transaction(&account->transactions, tr);
    add_transaction(&recipient->transactions, tr);

    fputs(
        "\n"
        "Transfer done successfully.\n",
        stdout);
}

static void user_update_transaction_comment(Account* account) {
    if (account->transactions.size == 0) {
        printf("No transaction to update.\n");
        return;
    }

    printf("\nTransactions:\n");
    for (size_t i = 0; i < account->transactions.size; i++) {
        Transaction* tr = &account->transactions.ptr[i];
        printf("  %zu. From: '%s' To: '%s' Amount: %d Comment: '%s'\n", i,
               tr->src->username, tr->dest->username, tr->amount, tr->comment);
    }

    size_t index;
    fputs("\nIndex of transaction: ", stdout);
    fflush(stdout);
    scanf("%zu", &index);

    Transaction* transaction = &account->transactions.ptr[index];
    char* comment = transaction->comment;

    fputs("New comment: ", stdout);
    fflush(stdout);
    getchar();  // read '\n'
    fgets(comment, 30, stdin);
    comment[strlen(comment) - 1] = '\0';  // remove extra '\n'

    puts("Comment updated successfully.\n");
}

static void user_log_off(Account* account) { fputs("Logging off..\n", stdout); }

static void user_loop(Account* account) {
    printf(
        "\n"
        "Successfully logged in as '%s'.\n",
        account->username);

    while (1) {
        user_menu();

        UserAction action = user_action();

        switch (action) {
            case ShowStatement:
                user_statement(account);
                break;
            case MakeTransfer:
                user_transfer(account);
                break;
            case SetFavoriteRecipient:
                user_set_favorite_recipient(account);
                break;
            case MakeTransferFavoriteRecipient:
                user_transfert_favorite_recipient(account);
                break;
            case UpdateTransactionComment:
                user_update_transaction_comment(account);
                break;
            default:
                user_log_off(account);
                return;
        };
    }
}

// Guest menu and actions

typedef enum { Login = 1, CreateAccount = 2, Exit = 3 } GuestAction;

static void guest_menu() {
    fputs(
        "\n"
        "Available commands:\n"
        "  1. Login\n"
        "  2. Create an account\n"
        "  3. Exit\n",
        stdout);
}

static GuestAction guest_action() {
    char input[10];

    fputs("> ", stdout);
    fflush(stdout);

    scanf("%s", input);
    int n = atoi(input);
    return (GuestAction)n;
}

static void guest_login() {
    char username[30];
    char password[30];

    fputs(
        "\n"
        "Login form:\n"
        "\n",
        stdout);

    fputs("Username: ", stdout);
    fflush(stdout);

    scanf("%s", username);

    fputs("Password: ", stdout);
    fflush(stdout);

    scanf("%s", password);

    Account* account = login(username, password);

    if (account == NULL) {
        fputs(
            "\n"
            "Invalid username/password\n",
            stdout);
        return;
    }

    user_loop(account);
}

static void guest_create_account() {
    fputs(
        "\n"
        "Thank you for choosing us. Please fill the following form.\n"
        "\n",
        stdout);

    fputs("Username: ", stdout);
    fflush(stdout);

    char* username = (char*)malloc(sizeof(char) * 30);
    scanf("%s", username);

    fputs("Password: ", stdout);
    fflush(stdout);

    char* password = (char*)malloc(sizeof(char) * 30);
    scanf("%s", password);

    fputs(
        "\n"
        "Choose the crypto currency for this account:\n"
        "  1. BitCoin\n"
        "  2. Ethereum\n"
        "  3. PizzaCoin\n"
        "  4. InsaneCoin\n"
        "\n"
        "Currency: ",
        stdout);
    fflush(stdout);

    int n;
    scanf("%d", &n);

    Account account = {.username = username,
                       .password = password,
                       .currency = (Currency)n,
                       .balance = 10,
                       .favorite_recipient = NULL,
                       .transactions = new_transaction_array()};
    add_account(account);

    fputs(
        "\n"
        "Account successfully created.\n"
        "As a reward for using our bank, we gave you 10 coins.\n",
        stdout);
};

static void guest_loop() {
    while (1) {
        guest_menu();

        GuestAction action = guest_action();

        switch (action) {
            case Login:
                guest_login();
                break;
            case CreateAccount:
                guest_create_account();
                break;
            default:
                return;
        };
    }
}

int main() {
    init_bank();

    fputs(
        "Welcome to the " CBOLD CRED "THC" COFF CBOLD CBLUE " Crypto Bank" COFF
        ".\n"
        "  We keep your crypto currencies " CYELLOW "safe" COFF
        " here.\n"
        "  Our system is designed to be " CGREEN "fast" COFF " and " CGREEN
        "robust" COFF
        ".\n"
        "  Supported currencies: Bitcoin, Ethereum, PizzaCoin and InsaneCoin\n",
        stdout);

    guest_loop();

    fputs("Thanks for using the THC Crypto Bank. See you soon.\n", stdout);

    free_bank();

    return 0;
}
