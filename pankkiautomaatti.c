#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

enum View
{
  MAIN,
  WITHDRAW,
  BALANCE
};

typedef struct
{
  int number;
  int pin;
  float balance;
  FILE *file;
} Account;

void login(Account *account);

void updateFileBalance(Account *account);

void getAccountFileName(int accNumber, char* dest);

bool showView(enum View *view, Account *account);

bool mainMenu(enum View *view, Account *account);
bool withdrawMenu(enum View *view, Account *account);
bool balanceMenu(enum View *view, Account *account);

void withdrawCustomAmount(Account *account);

void clearInputBuffer(char *buf);
int getNumOfDigits(int number);

FILE* openAccountFile();
int readPinFromFile(FILE* file);
float readBalanceFromFile(FILE* file);

void readNumber(int *input);
void readString(char *dest);

/**
 * @brief 
 * 
 * Main function 
 * 
 */
int main()
{
  bool quit = false;
  enum View view = MAIN;
  Account account;
  account.file = NULL;

  login(&account);

  while(!quit)
    quit = showView(&view, &account);

  printf("\nLopetetaan...\n");

  fclose(account.file);

  return 0;
}

/**
 * @brief 
 * 
 * Login. Asks for account number and pin code. 
 *  
 * @param account Account number 
 */
void login(Account *account)
{
  int accNumber = 0;
  int pin = 0;
  bool success = false; 

  while (!success)
  {
    if(accNumber == 0)
    {
      printf("Syota tilinumero: ");
      readNumber(&accNumber);

      account->file = openAccountFile(accNumber);
      if(account->file == NULL)
      {
        printf("- Vaara tilinumero. Yrita uudelleen.\n");
        accNumber = 0;
        continue;
      }

      account->number = accNumber;
    }

    printf("Syota 4-numeroinen PIN-koodi: ");
    readNumber(&pin);

    int digitCount = getNumOfDigits(pin);
    if(digitCount != 4)
    {
      printf("- PIN-koodi pitaa olla 4-numeroinen luku!\n");
      continue;
    }

    int correctPin;

    correctPin = readPinFromFile(account->file);

    if(pin == correctPin)
      success = true;
    else
      printf("- Vaara PIN-koodi. Yrita uudelleen\n");
  }

  float balance = readBalanceFromFile(account->file);

  account->pin = pin;
  account->balance = balance;

  fclose(account->file);
  account->file = NULL;
}

/**
 * @brief 
 * 
 * Update file balance 
 *  
 * @param account Account number 
 */
void updateFileBalance(Account *account)
{
  int balanceRow = 2;
  int currentLine = 1;

  if(account->file == NULL)
    account->file = openAccountFile(account->number);

  rewind(account->file);

  FILE *tempFile = fopen(".temp", "w");

  if(tempFile == NULL)
  {
    printf("- Saldon paivitys epaonnistui\n");
    return;
  }

  char ch;
  while((ch = fgetc(account->file)) != EOF)
  {
    if(currentLine != balanceRow)
      fputc(ch, tempFile);
    else
    {
      fprintf(tempFile, "%.2f", account->balance);
      while ((ch = fgetc(account->file)) != '\n' && ch != EOF);
    }

    if(ch == '\n')
      currentLine++;
  }

  fclose(account->file);
  fclose(tempFile);

  account->file = NULL;

  char filename[20];
  getAccountFileName(account->number, filename);

  if(remove(filename) != 0)
  {
    printf("- Saldon paivitys epaonnistui\n");
    return;
  }

  if(rename(".temp", filename) != 0)
  {
    printf("- Saldon paivitys epaonnistui\n");
    return;
  }
}

/**
 * @brief 
 * 
 * Make account filename from the acc number 
 *  
 * @param accNumber Account number 
 * @param dest Destination 
 */
void getAccountFileName(int accNumber, char* dest)
{
  char filename[20];

  sprintf(filename, "%d", accNumber);
  strcat(filename, ".acc");

  memcpy(dest, filename, strlen(filename));

  dest[strlen(filename)] = '\0';
}

/**
 * @brief 
 * 
 * Decide what menu to show. 
 *  
 * @param view Current view 
 * @param account Account struct 
 * @returns If should quit the app 
 */
bool showView(enum View *view, Account *account)
{
  switch(*view)
  {
    case MAIN: {
      printf("\n---\n");
      return mainMenu(view, account);
    } break;
    case BALANCE: {
      printf("\n---\n");
      return balanceMenu(view, account);
    } break;
    case WITHDRAW: {
      printf("\n---\n");
      return withdrawMenu(view, account);
    } break;
  }
}

/**
 * @brief 
 * 
 * Main menu. There is three options: Go to withdraw, check balance or quit 
 *  
 * @param view Current view 
 * @param account Account struct 
 * @returns If should quit the app 
 */
bool mainMenu(enum View *view, Account *account)
{
  bool invalid = true;
  int input = -1;

  do
  {
    printf("Valitse\n");
    printf("  1 - Otto\n");
    printf("  2 - Saldo\n");
    printf("  0 - Lopeta\n\n");

    printf("Valinta: ");
    readNumber(&input);

    switch(input)
    {
      case 1: {
        invalid = false;
        *view = WITHDRAW;
      } break;
      case 2: {
        invalid = false;
        *view = BALANCE;
      } break;
      case 0: {
        invalid = false;
        return true;
      } break;
      default: {
        invalid = true;
        printf("- Valintaa ei ole!\n\n");
      } break;
    }

  } while (invalid);

  return false;
}

/**
 * @brief 
 * 
 * Withdraw custom amount.
 * Also calculate the number of 50e and 20e bills to be given
 * 
 * @param account Account struct
 */
void withdrawCustomAmount(Account *account)
{
  int withdraw = 0;
  bool back = false;

  printf("- Nostomaara pitaa olla 20, 40 tai isompi jos jaollinen 10:lla. Maksiminosto on 1000 euroa.\n");

  do
  {
    printf("- Palaa paavalikkoon syottamalla -1.\n");

    printf("\nPaljonko haluat nostaa: ");
    readNumber(&withdraw); 

    if(withdraw > 1000)
    {
      printf("- Maksiminostomaara on 1000 euroa!\n");
      continue;
    }

    if(withdraw == -1)
    {
      back = true;
      break;
    } else if(withdraw < 0)
    {
      printf("- Kelvoton nostomaara!\n");
      continue;
    }
    
    if(account->balance < withdraw)
    {
      printf("- Tilillasi ei ole tarpeeksi saldo nostoon!\n");
      continue;
    }

    if(withdraw % 10 != 0)
    {
      printf("- Nostomaara ei ole tasaluku\n");
      continue;
    }

    // Laske setelimäärät

    int bill50 = withdraw / 50; 
    int bill20 = 0;
    int remainder = withdraw % 50;

    // Jos jäljelle jäänyt on jaollinen 20
    if(remainder % 20 == 0)
      bill20 = remainder / 20;
    else {
      int newRemainder = 0;
      for(int i = bill50 - 1; i > 0; i--)
      {
        newRemainder = withdraw - i * 50;
        if(newRemainder % 20 == 0)
        {
          bill20 = newRemainder / 20;
          bill50 = i;
          break;
        }
      }
    }

    if(bill50 == 0 && bill20 == 0)
    {
      printf("- Liian pieni nostomaara\n");
      continue;
    }

    account->balance -= withdraw;

    printf("- Nostit %d euroa. Kone antoi %d kpl 50e setelia ja %d kpl 20e setelia.\n", withdraw, bill50, bill20);

    back = true;
  } while (!back);
}

/**
 * @brief 
 * 
 * Withdraw menu. There is three options: withdraw 20 or 40 euros, 
 * or withdraw custom amount.
 *  
 * @param view Current view 
 * @param account Account struct 
 * @returns If should quit the app 
 */
bool withdrawMenu(enum View *view, Account *account)
{
  float oldBalance = account->balance;
  int valinta = -1;
  bool success = false;

  do
  {
    printf("Otto\n");
    printf("  1 - 20 euroa\n");
    printf("  2 - 40 euroa\n");
    printf("  3 - Muu summa\n");
    printf("  0 - Alkuun\n\n");

    printf("Valinta: ");
    readNumber(&valinta);

    switch(valinta)
    {
      case 1: 
      {
        if(account->balance < 20)
        {
          printf("- Tilillasi ei ole tarpeeksi saldoa nostoon!\n");
          continue;
        }
        account->balance -= 20;
        printf("- Nostit 20 euroa 1 kpl 20e seteleilla.\n");
        success = true;
      } break;

      case 2: 
      {
        if(account->balance < 40)
        {
          printf("- Tilillasi ei ole tarpeeksi saldoa nostoon!\n");
          continue;
        }
        account->balance -= 40;
        printf("- Nostit 40 euroa 2 kpl 20e seteleilla.\n");
        success = true;
      } break;

      case 3: 
      {
        withdrawCustomAmount(account);
        success = true;
      } break;
      
      case 0:
      {
        success = true;
      } break;

      default: 
      {
        printf("- Valintaa ei ole!\n\n");
      } break;
    }

  } while(!success);

  if(oldBalance != account->balance)
    updateFileBalance(account);

  *view = MAIN;
  return false;
}

/**
 * @brief 
 * 
 * Balance menu. Shows the current balance, 
 * and two options: Return to mainmenu or quit
 * 
 * @param view Current view
 * @param account Account struct
 * @returns If should quit the app 
 */
bool balanceMenu(enum View *view, Account *account)
{
  printf("Tilin saldo: %.2f\n", account->balance);
  printf("  1 - Alkuun\n");
  printf("  0 - Lopeta\n\n");

  int input = -1;
  bool invalid = true;

  do
  {
    printf("Valinta: ");
    readNumber(&input);

    if(input == 1) {
      *view = MAIN;
      return false;
    } else if(input == 0) {
      return true;
    } 

    printf("- Valintaa ei ole!\n\n");
  } while (invalid);

  return false; 
}

/**
 * @brief 
 * 
 * Reads from stdin and converts them to int 
 * and copies the input to variable
 * 
 * @param input Where the input will be copied. -2 if invalid input
 */
void readNumber(int *input)
{
  char buffer[10];
  char *endPtr;
  int number = -2; // -2 = invalid input
  
  fgets(buffer, 10, stdin);
  clearInputBuffer(buffer);
  
  number = strtol(buffer, &endPtr, 10);
  if(buffer == endPtr)
    number = -2;
  else if(*endPtr != '\0')
    number = -2;

  if(isspace(*buffer))
    number = -2;

  memcpy(input, &number, sizeof(int));
}

/**
 * @brief 
 * 
 * Reads from stdin and copies the input to variable
 * 
 * @param dest Destination of the input string. 
 */
void readString(char *dest)
{
  char buffer[100];
  fgets(buffer, 100, stdin);

  clearInputBuffer(buffer);

  memcpy(dest, buffer, strlen(buffer));
}

/**
 * @brief 
 * 
 * Get the number of digits in a number 
 * 
 * @param number The number to check
 * @returns Number of digits
 *
 */
int getNumOfDigits(int number)
{
  int digits = 0;
  while (number != 0)
  {
    number /= 10;
    digits++;
  }

  return digits;
}

/**
 * @brief 
 * 
 * Puts null pointer if last character of a buffer is newline.
 * Also clears the remaining characters from the input buffer
 * 
 * @param buf The string to check for newline
 */
void clearInputBuffer(char *buf)
{
  if(buf[strlen(buf) - 1] == '\n')
    buf[strlen(buf) - 1] = '\0'; 
  else
    while(getc(stdin) != '\n');
}

/**
 * @brief 
 * 
 * Opens the account file.
 * 
 * @returns The file pointer or NULL on failure
 */
FILE* openAccountFile(int accNumber)
{
  FILE *file;

  char filename[20];
  getAccountFileName(accNumber, filename);

  file = fopen(filename, "r");
  if(file == NULL)
    return NULL;

  return file;
}

/**
 * @brief 
 * 
 * Reads the pin row of the account file.
 *  
 * @param file The account file pointer 
 * @returns The pin code.
 */
int readPinFromFile(FILE* file)
{
  rewind(file);
  int code;

  int result = fscanf(file, "%d", &code);
  if(!result)
  {
    printf("- PIN-koodin luku epaonnistui!");
    return 0;
  }

  return code;
}

/**
 * @brief 
 * 
 * Reads the balance row of the account file.
 *  
 * @param file The account file pointer 
 * @returns Account balance.
 */
float readBalanceFromFile(FILE* file)
{
  rewind(file);
  float balance;

  // Skip pin row
  char c;
  while((c = fgetc(file)) != '\n')
  {
    if(c == EOF)
    {
      printf("- Saldon hakeminen epaonnistui!\n");
      return 0;
    }
  }

  int result = fscanf(file, "%f", &balance);
  if(!result)
  {
    printf("- Saldon hakeminen epaonnistui!\n");
    return 0;
  }

  return balance;
}