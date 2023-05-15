#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

enum View
{
  MAIN,
  WITHDRAW,
  BALANCE
};

typedef struct
{
  int pin;
  float balance;
} Account;

void login(Account *account);

bool showView(enum View *view, Account *account);

bool mainMenu(enum View *view, Account *account);
bool withdrawMenu(enum View *view, Account *account);
bool balanceMenu(enum View *view, Account *account);

void clearInputBuffer(char *buf);
int getNumOfDigits(int number);

FILE* openAccountFile();
int readPinFromFile(FILE* file);
float readBalanceFromFile(FILE* file);

void readNumber(int *input);
void readString(char *dest);

int main()
{
  Account account;
  enum View view = MAIN;
  bool quit = false;

  login(&account);

  do
  {
    quit = showView(&view, &account);
  } while (!quit);

  printf("\nLopetetaan...\n");

  return 0;
}

void login(Account *account)
{
  int pin = 0;
  bool success = false; 
  FILE* accountFile = 0;

  do 
  {
    printf("Syota 4-numeroinen PIN-koodi: ");
    readNumber(&pin);

    int digitCount = getNumOfDigits(pin);
    if(digitCount != 4)
    {
      printf("- PIN-koodi pitaa olla 4-numeroinen!\n");
      continue;
    }

    int correctPin;
    if((accountFile = openAccountFile()) != 0)
    {
      correctPin = readPinFromFile(accountFile);
    }

    if(pin == correctPin)
      success = true;
    else
    {
      printf("- Vaara PIN-koodi. Yrita uudelleen\n");
      fclose(accountFile);
    }
  } while (!success);

  float balance = readBalanceFromFile(accountFile);

  account->pin = pin;
  account->balance = balance;
  fclose(accountFile);
}

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
        printf("- Valintaa ei ole!\n");
      } break;
    }

  } while (invalid);

  return false;
}

bool withdrawMenu(enum View *view, Account *account)
{
  return false;
}

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

    printf("- Valintaa ei ole!\n");
  } while (invalid);

  return false; 
}

void readNumber(int *input)
{
  char buffer[10];
  char *endPtr;
  int number = -1;
  
  fgets(buffer, 10, stdin);
  
  clearInputBuffer(buffer);
  
  number = strtol(buffer, &endPtr, 10);

  if(buffer == endPtr)
    number = -1;
  else if(*endPtr != '\0')
    number = -1;

  memcpy(input, &number, sizeof(int));
}

void readString(char *dest)
{
  char buffer[100];
  fgets(buffer, 100, stdin);

  clearInputBuffer(buffer);

  memcpy(dest, buffer, strlen(buffer));
}

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

void clearInputBuffer(char *buf)
{
  if(buf[strlen(buf) - 1] == '\n')
    buf[strlen(buf) - 1] = '\0'; 
  else
    while(getc(stdin) != '\n');
}

FILE* openAccountFile()
{
  FILE *file;
  int code;

  file = fopen(".tili", "r");
  if(file == NULL)
  {
    printf("- Tilin avaus epaonnistui!\n");
    return 0;
  }

  return file;
}

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
      return -1;
    }
  }

  int result = fscanf(file, "%f", &balance);
  if(!result)
  {
    printf("- Saldon hakeminen epaonnistui!\n");
    return -1;
  }

  return balance;
}