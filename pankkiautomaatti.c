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

void login(Account *account, FILE *accountFile);

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

int main()
{
  Account account;
  FILE* accountFile = 0;
  enum View view = MAIN;
  bool quit = false;

  login(&account, accountFile);

  do
  {
    quit = showView(&view, &account);
  } while (!quit);

  printf("\nLopetetaan...\n");

  fclose(accountFile);

  return 0;
}

void login(Account *account, FILE *accountFile)
{
  int pin = 0;
  bool success = false; 

  do 
  {
    printf("Syota 4-numeroinen PIN-koodi: ");
    readNumber(&pin);

    int digitCount = getNumOfDigits(pin);
    if(digitCount != 4)
    {
      printf("- PIN-koodi pitaa olla 4-numeroinen luku!\n");
      continue;
    }

    int correctPin;
    if(accountFile == 0)
      accountFile = openAccountFile();

    correctPin = readPinFromFile(accountFile);

    if(pin == correctPin)
      success = true;
    else
      printf("- Vaara PIN-koodi. Yrita uudelleen\n");
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
        printf("- Valintaa ei ole!\n\n");
      } break;
    }

  } while (invalid);

  return false;
}

void withdrawCustomAmount(Account *account)
{
  int withdraw = 0;
  bool back = false;

  printf("- Nostomaara pitaa olla 20, 40 tai isompi jos jaollinen 10:lla. Maksiminosto on 1000 euroa.\n");

  do
  {
    printf("- Palaa takaisin Otto -valikkoon syottamalla -1.\n");
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

    if(withdraw == 20)
    {
      account->balance -= withdraw;
      printf("- Nostit 20 euroa 1 kpl 20e seteleilla.\n");
      back = true;
      break;
    } 
    if(withdraw == 40)
    {
      account->balance -= withdraw;
      printf("- Nostit 40 euroa 2 kpl 20e seteleilla.\n");
      back = true;
      break;
    }
    
    if(withdraw < 40)
    {
      printf("- Liian pieni nostomaara\n");
      continue;
    }

    if(withdraw % 10 != 0)
    {
      printf("- Nostomaara ei ole tasaluku\n");
      continue;
    }

    int bill50 = withdraw / 50; 
    int bill20 = 0;
    int remainder = withdraw % 50;

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

    account->balance -= withdraw;

    printf("- Nostit %d euroa. Kone antoi %d kpl 50e setelia ja %d kpl 20e setelia.\n", withdraw, bill50, bill20);
    back = true;
  } while (!back);
}
bool withdrawMenu(enum View *view, Account *account)
{
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
        *view = MAIN;
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

  *view = MAIN;
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

    printf("- Valintaa ei ole!\n\n");
  } while (invalid);

  return false; 
}

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