#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include <pthread.h>
#include <unistd.h>
#define MAX_CUSTOMERS 100
#define NUMBER_OF_NOTE_TYPE sizeof(atm_data) / sizeof(atm_data[0])

struct ATM_data
{
    int type_of_notes;
    int no_of_notes;
};

struct ATM_data atm_data[] = {
    {2000, 0},
    {500, 0},
    {100, 0}};

struct customer_data
{
    int account_number;
    char account_holder[100];
    int pin;
    float account_balance;
};
struct customer_data customers[MAX_CUSTOMERS];
int number_of_customers = 0;
struct transaction_data
{
    int account_number;
    char description[30];
    char credit_debit[10];
    float amount;
    float closing_balance;
};

// 1.Loading cash to atm by me
void loading_cash_to_atm()
{
    int type_of_notes, no_of_notes, i;
    printf("\n\n *-----------------------------------LOAD-CASH-------------------------------------------*\n\n");

    for (i = 0; i < NUMBER_OF_NOTE_TYPE; i++)
    {

        printf("Enter the number of Rs. %d notes to load in the atm machine: ", atm_data[i].type_of_notes);
        scanf("%d", &no_of_notes);

        if (no_of_notes >= 0)
        {
            atm_data[i].no_of_notes += no_of_notes;
            printf("Rs. %d notes loaded in the atm machine: %d\n", atm_data[i].type_of_notes, no_of_notes);
        }
        else
        {
            printf("\n*------------------------------MESSAGE------------------------------------*\n\n");
            printf("Invalid input. Number of notes should be non-negative.\n");
            printf("\n*-------------------------------------------------------------------------*\n\n");
            i--;
        }
    }
    printf("\n\n *---------------------------------------------------------------------------------------*");
}

// saving the data loaded to a file
void save_atm_data()
{
    FILE *file = fopen("atm_data.txt", "w");
    int i;
    if (file == NULL)
    {
        printf("\n*------------------------------MESSAGE------------------------------------*\n\n");
        printf("Error opening the atm data file for writing.\n");
        printf("\n*-------------------------------------------------------------------------*\n\n");
        return;
    }

    for (i = 0; i < NUMBER_OF_NOTE_TYPE; i++)
    {
        fprintf(file, "%d %d\n", atm_data[i].type_of_notes, atm_data[i].no_of_notes);
    }

    fclose(file);
}

// loading atm data when withdrawn occurs
void load_atm_data()
{
    FILE *file = fopen("atm_data.txt", "r");
    int i;
    if (file == NULL)
    {
        printf("\n*------------------------------MESSAGE------------------------------------*\n\n");
        printf("Error opening atm data file for loading.\n");
        printf("\n*-------------------------------------------------------------------------*\n\n");
        return;
    }

    for (i = 0; i < NUMBER_OF_NOTE_TYPE; i++)
    {
        if (fscanf(file, "%d %d", &atm_data[i].type_of_notes, &atm_data[i].no_of_notes) != 2)
        {
            printf("\n*------------------------------MESSAGE------------------------------------*\n\n");
            printf("Error reading atm data from file for scanning.\n");
            printf("\n*-------------------------------------------------------------------------*\n\n");
            break;
        }
    }

    fclose(file);
}

// 2.loading the customer data
void load_customer_data()
{
    FILE *file = fopen("customer_data.txt", "r");
    if (file == NULL)
    {
        printf("\n*------------------------------MESSAGE------------------------------------*\n\n");
        printf("Error opening customer data because there is no data.\n");
        printf("\n*-------------------------------------------------------------------------*\n\n");
        return;
    }

    while (fscanf(file, "%d %s %d %f", &customers[number_of_customers].account_number,
                  customers[number_of_customers].account_holder, &customers[number_of_customers].pin,
                  &customers[number_of_customers].account_balance) == 4)
    {
        number_of_customers++;
        if (number_of_customers >= MAX_CUSTOMERS)
        {
            break;
        }
    }

    fclose(file);
}

// Display customer data
void show_customer_details()
{
    int i;
    printf("\n\n *---------------------------------CUSTOMER-DETAILS--------------------------------------*\n\n");
    printf("Acc No\tAccount Holder\tPin Number\tAccount Balance\n");
    for (i = 0; i < number_of_customers; i++)
    {
        printf("%d\t%s\t\t%d\t\tRs.%.2f \n", customers[i].account_number,
               customers[i].account_holder, customers[i].pin,
               customers[i].account_balance);
    }
    printf("\n\n *---------------------------------------------------------------------------------------*");
}

// saving the customer data
void save_customer_data()
{
    FILE *file = fopen("customer_data.txt", "w");
    int i;
    if (file == NULL)
    {
        printf("\n*------------------------------MESSAGE------------------------------------*\n\n");
        printf("Error opening customer data file for writing.\n");
        printf("\n*-------------------------------------------------------------------------*\n\n");
        return;
    }

    for (i = 0; i < number_of_customers; i++)
    {
        fprintf(file, "%d %s %d %.2f\n", customers[i].account_number,
                customers[i].account_holder, customers[i].pin,
                customers[i].account_balance);
    }

    fclose(file);
}

// verifing entered customer data with loaded customer datya
bool validate_customer(int account_number, int pin)
{
    int i;
    for (i = 0; i < number_of_customers; i++)
    {
        if (customers[i].account_number == account_number && customers[i].pin == pin)
        {
            return true;
        }
    }
    return false;
}

// Loading transactions history in each account holders file
void *log_transaction_async(void *args)
{
    struct transaction_data *data = (struct transaction_data *)args;

    sleep(5);

    log_transaction(data->account_number, data->description, data->credit_debit, data->amount, data->closing_balance);

    free(data);
    pthread_exit(NULL);
}
void log_transaction(int account_number, const char *description, const char *credit_debit, float amount, float closing_balance)
{
    char filename[100];
    snprintf(filename, sizeof(filename), "%d_transactions.txt", account_number);

    FILE *file = fopen(filename, "a");
    if (file == NULL)
    {
        printf("\n*------------------------------MESSAGE------------------------------------*\n\n");
        printf("Error opening the transaction file.\n");
        printf("\n*-------------------------------------------------------------------------*\n\n");
        return;
    }

    fprintf(file, "%d  %s %s %.2f %.2f\n", account_number, description, credit_debit, amount, closing_balance);

    fclose(file);
}

// 3.1 Check account balance
void check_balance(int account_number)
{
    int index = -1, i;
    for (i = 0; i < number_of_customers; i++)
    {
        if (customers[i].account_number == account_number)
        {
            index = i;
            break;
        }
    }

    if (index != -1)
    {
        printf("\n\n *---------------------------------ACCOUNT-BALANCE---------------------------------------*\n\n");
        printf("Account Holder: %s\n", customers[index].account_holder);
        printf("Account Number: %d\n", customers[index].account_number);
        printf("Account Balance: Rs. %.2f \n", customers[index].account_balance);
        printf("\n\n *---------------------------------------------------------------------------------------*");
    }
    else
    {
        printf("\n*------------------------------MESSAGE------------------------------------*\n\n");
        printf("Customer with account number %d was not found.\n", account_number);
        printf("\n*-------------------------------------------------------------------------*\n\n");
    }
}

// 3.2 withdaw money from account balance

void withdraw_money(int account_number)
{
    int index = -1, i;
    for (i = 0; i < number_of_customers; i++)
    {
        if (customers[i].account_number == account_number)
        {
            index = i;
            break;
        }
    }

    if (index != -1)
    {
        float amount;
        int v_pin;
        printf("\n\n *---------------------------------------WITHDRAW-MONEY----------------------------------*\n\n");
        printf("Enter the amount to withdraw: ");
        scanf("%f", &amount);
        printf("Enter the pin: ");
        scanf("%d", &v_pin);
        if (customers[index].pin == v_pin)
        {
            if (amount >= 100 && amount <= 10000)
            {
                float account_balance = customers[index].account_balance;
                if (amount <= account_balance)
                {
                    int remaining_amount = amount;
                    int type_2000 = 0;
                    int type_500 = 0;
                    int type_100 = 0;
                    if (amount <= 5000)
                    {
                        if (amount >= 1000)
                        {
                            if (atm_data[1].no_of_notes >= 1)
                            {
                                type_2000 = remaining_amount / 2000;
                                remaining_amount = remaining_amount % 2000;
                            }
                        }
                        else if (amount >= 3000)
                        {
                            if (atm_data[0].no_of_notes >= 1)
                            {
                                type_500 = remaining_amount / 500;
                                remaining_amount = remaining_amount % 500;
                            }
                        }
                        else if (amount >= 1500)
                        {
                            if (atm_data[2].no_of_notes >= 10)
                            {
                                type_100 = remaining_amount / 100;
                                remaining_amount = remaining_amount % 100;
                            }
                        }
                        else
                        {
                            printf("\n*------------------------------MESSAGE------------------------------------*\n\n");
                            printf("Insuficient fund in atm");
                            printf("\n*-------------------------------------------------------------------------*\n\n");
                        }
                    }
                    else if (amount > 5000)
                    {
                        if (atm_data[0].no_of_notes >= 2 && atm_data[1].no_of_notes >= 2 && atm_data[2].no_of_notes >= 10)
                        {

                            type_2000 = remaining_amount / 2000;
                            remaining_amount = remaining_amount % 2000;
                            type_500 = remaining_amount / 500;
                            remaining_amount = remaining_amount % 500;
                            type_100 = remaining_amount / 100;
                            remaining_amount = remaining_amount % 100;
                        }
                        else
                        {
                            printf("\n*------------------------------MESSAGE------------------------------------*\n\n");
                            printf("Insuficient fund in atm");
                            printf("\n*-------------------------------------------------------------------------*\n\n");
                        }
                    }

                    customers[index].account_balance -= amount;
                    atm_data[0].no_of_notes -= type_2000;
                    atm_data[1].no_of_notes -= type_500;
                    atm_data[2].no_of_notes -= type_100;

                    save_customer_data();
                    save_atm_data();
                    float closing_balance = customers[index].account_balance;
                    log_transaction(account_number, "Cash_Withdrawal", "Debit", amount, closing_balance);
                    printf("Amount Rs. %.2f withdrawn successfully.\n", amount);
                    printf("\n\n *---------------------------------------------------------------------------------------*");
                }
                else
                {
                    printf("\n*------------------------------MESSAGE------------------------------------*\n\n");
                    printf("Insufficient account balance.\n");
                    printf("\n*-------------------------------------------------------------------------*\n\n");
                }
            }
            else
            {
                printf("\n*------------------------------MESSAGE------------------------------------*\n\n");
                printf("Invalid amount. Withdrawal amount should be between Rs. 100 and Rs. 10,000.\n");
                printf("\n*-------------------------------------------------------------------------*\n\n");
            }
        }
        else
        {
            printf("\n*------------------------------MESSAGE------------------------------------*\n\n");
            printf("Incorrect pin entered");
            printf("\n*-------------------------------------------------------------------------*\n\n");
        }
    }
    else
    {
        printf("\n*------------------------------MESSAGE------------------------------------*\n\n");
        printf("Customer with account number %d was not found.\n", account_number);
        printf("\n*-------------------------------------------------------------------------*\n\n");
    }
}

// 3.3 Transfer money from one acount to another
void transfer_money(int account_number)
{
    int sender_index = -1, i;
    for (i = 0; i < number_of_customers; i++)
    {
        if (customers[i].account_number == account_number)
        {
            sender_index = i;
            break;
        }
    }

    if (sender_index != -1)
    {
        int recipient_account_number, amount;

        printf("\n\n *----------------------------------TRANSFER-MONEY---------------------------------------*\n\n");
        printf("Enter the recipient's account number: ");
        scanf("%d", &recipient_account_number);

        int recipient_index = -1, v_pin;
        for (i = 0; i < number_of_customers; i++)
        {
            if (customers[i].account_number == recipient_account_number)
            {
                recipient_index = i;
                break;
            }
        }

        if (recipient_index != -1)
        {
            printf("Enter the amount to transfer: ");
            scanf("%d", &amount);
            printf("Enter the pin number: ");
            scanf("%d", &v_pin);
            if (customers[sender_index].pin == v_pin)
            {

                if (amount > 1000 && amount <= 10000)
                {
                    if (amount <= customers[sender_index].account_balance)
                    {
                        customers[sender_index].account_balance -= amount;
                        customers[recipient_index].account_balance += amount;
                        save_customer_data();
                        float sender_closing_balance = customers[sender_index].account_balance;
                        log_transaction(account_number, "Transfer_to_", "Debit", amount, sender_closing_balance);

                        float recipient_closing_balance = customers[recipient_index].account_balance;
                        log_transaction(recipient_account_number, "Transfer_from_", "Credit", amount, recipient_closing_balance);
                        printf("Amount Rs. %d transferred successfully to Account Number %d.\n", amount, recipient_account_number);
                        printf("\n\n *---------------------------------------------------------------------------------------*");
                    }
                    else
                    {
                        printf("\n*------------------------------MESSAGE------------------------------------*\n\n");
                        printf("Insufficient account balance for the transfer.\n");
                        printf("\n*-------------------------------------------------------------------------*\n\n");
                    }
                }
                else
                {
                    printf("\n*------------------------------MESSAGE------------------------------------*\n\n");
                    printf("Invalid amount. Transfer amount should be between Rs. 1000 and Rs. 10,000.\n");
                    printf("\n*-------------------------------------------------------------------------*\n\n");
                }
            }
            else
            {
                printf("\n*------------------------------MESSAGE------------------------------------*\n\n");
                printf("Incorrect pin entered");
                printf("\n*-------------------------------------------------------------------------*\n\n");
            }
        }
        else
        {
            printf("\n*------------------------------MESSAGE------------------------------------*\n\n");
            printf("Recipient with account number %dwas not found.\n", recipient_account_number);
            printf("\n*-------------------------------------------------------------------------*\n\n");
        }
    }
    else
    {
        printf("\n*------------------------------MESSAGE------------------------------------*\n\n");
        printf("Customer with account number %d was not found.\n", account_number);
        printf("\n*-------------------------------------------------------------------------*\n\n");
    }
}

// 3.4 Checking ATM balance for availability of cash
void check_atm_balance()
{
    int i;
    printf("\n\n *---------------------------------CHECK-ATM-BALANCE-------------------------------------*\n\n");
    printf("Type of Note\tNumber\tValue\n");
    for (i = 0; i < NUMBER_OF_NOTE_TYPE; i++)
    {
        printf("%d\t\t%d\t%d\n", atm_data[i].type_of_notes, atm_data[i].no_of_notes, atm_data[i].type_of_notes * atm_data[i].no_of_notes);
    }

    int total_amount = 0;
    for (i = 0; i < NUMBER_OF_NOTE_TYPE; i++)
    {
        total_amount += atm_data[i].type_of_notes * atm_data[i].no_of_notes;
    }

    printf("Total Amount available in the ATM = Rs. %d \n", total_amount);
    printf("\n\n *---------------------------------------------------------------------------------------*");
}

// 3.5 displaying mini statement
void show_mini_statement(int account_number)
{
    char filename[100];
    snprintf(filename, sizeof(filename), "%d_transactions.txt", account_number);

    FILE *file = fopen(filename, "r");
    if (file == NULL)
    {
        printf("\n*------------------------------MESSAGE------------------------------------*\n\n");
        printf("No transaction history available for this account.\n");
        printf("\n*-------------------------------------------------------------------------*\n\n");
        return;
    }

    struct transaction_data transactions[100];
    int num_transactions = 0;

    int transaction_number;
    char description[20];
    char credit_debit[10];
    float amount;
    float closing_balance;

    while (fscanf(file, "%d %s %s %f %f\n", &transaction_number, description, credit_debit, &amount, &closing_balance) == 5)
    {
        struct transaction_data transaction;
        transaction.account_number = account_number;
        strcpy(transaction.description, description);
        strcpy(transaction.credit_debit, credit_debit);
        transaction.amount = amount;
        transaction.closing_balance = closing_balance;

        transactions[num_transactions++] = transaction;

        if (num_transactions >= 100)
        {
            break;
        }
    }

    fclose(file);

    int start_index = (num_transactions > 10) ? (num_transactions - 10) : 0, i;
    printf("\n\n *--------------------------------MINI-STATEMENT-----------------------------------------------------*\n\n");
    printf("Transaction Number\tDescription\t\tCredit / Debit\t\tAmount\t\tClosing Balance\n");
    for (i = start_index; i < num_transactions; i++)
    {
        printf("\t%d\t\t%s\t\t%s\t\t%.2f\t\t%.2f\n", transaction_number, transactions[i].description,
               transactions[i].credit_debit, transactions[i].amount, transactions[i].closing_balance);
    }
    printf("\n\n *-----------------------------------------------------------------------------------------------------*");
}

// 3.Operations in ATM
void operate_atm_process(int account_number)
{
    char atm_option;
    bool exit_atm_process = false;
    while (!exit_atm_process)
    {
        printf("\n\n *-----------------------------------ATM-OPERATIONS--------------------------------------*\n\n");

        printf("ATM Options:\n");
        printf("1. Check Balance\n");
        printf("2. Withdraw Money\n");
        printf("3. Transfer Money\n");
        printf("4. Check ATM Balance\n");
        printf("5. Mini Statement\n");
        printf("6. Exit\n");
        printf("Enter your choice (1/2/3/4/5/6): ");
        scanf("%s", &atm_option);

        switch (atm_option)
        {
        case '1':
            check_balance(account_number);
            break;
        case '2':
            withdraw_money(account_number);
            break;
        case '3':
            transfer_money(account_number);
            break;
        case '4':
            check_atm_balance();
            break;
        case '5':
            show_mini_statement(account_number);
            break;
        case '6':
            exit_atm_process = true;
            break;
        default:
            printf("\n*------------------------------MESSAGE------------------------------------*\n\n");
            printf("Invalid choice. Please try again.\n");
            printf("\n*-------------------------------------------------------------------------*\n\n");
        }
    }
}

void main_menu()
{
    printf("\n\n *-------------------------------------MAIN-MENU-----------------------------------------*\n\n");

    printf("1. Load Cash to ATM\n");
    printf("2. Show Customer Details\n");
    printf("3. Show ATM Operations\n");
    printf("4. Quit Application\n");
}

int main()
{

    load_customer_data();

    char choice;
    int account_number, pin, i;
    while (1)
    {
        main_menu();
        printf("Enter your choice (1/2/3/4): ");
        scanf("%s", &choice);

        switch (choice)
        {
        case '1':
            loading_cash_to_atm();
            save_atm_data();
            break;
        case '2':
            show_customer_details();
            break;
        case '3':
            printf("Enter your Account Number: ");
            scanf("%d", &account_number);
            printf("Enter your PIN: ");
            scanf("%d", &pin);
            load_atm_data();

            if (validate_customer(account_number, pin))
            {
                operate_atm_process(account_number);
            }
            else
            {
                printf("\n*------------------------------MESSAGE------------------------------------*\n\n");
                printf("Invalid Account Number or PIN. Please try again.\n");
                printf("\n*-------------------------------------------------------------------------*\n\n");
            }
            break;
        case '4':
            save_customer_data();
            printf("Exiting the application...  Done!\n");
            return 0;
        default:
            printf("\n*------------------------------MESSAGE------------------------------------*\n\n");
            printf("Invalid choice. Please try again.\n");
            printf("\n*-------------------------------------------------------------------------*\n\n");
        }
    }

    return 0;
}
