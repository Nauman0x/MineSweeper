#include<stdlib.h>
#include<iostream>
#include<Windows.h>
#include<fstream>
#include<string>


// struct Definitions
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct Block
{
    char HasFlag = 0;
    char IsHidden = 0;
    char Value = 0;
};

struct User
{
    char Name[20];
    char Id[20];
    char GamesPlayed = 0;
    char TotalWins = 0;
    char Score = 0;
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Constant variables
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define BLACK 0
#define BROWN 6
#define WHITE 15
#define GREEN 2
#define RED 4
#define LIGHT_BLUE 9
#define YELLOW 14

char GRID_SIZE = 8;
char NUM_MINES = 6;
const char FILE_NAME[] = { 'u', 's', 'e', 'r', 's', '.', 'b', 'i', 'n' };
const char MINE_SYMBOL = 'X';
const char SAFE_BOX_SYMBOL = '0';
const char FLAG_SYMBOL = '!';

const int D_X[] = { -1, -1, -1, 0, 0, 1, 1, 1 };
const int D_Y[] = { -1, 0, 1, -1, 1, -1, 0, 1 };

////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Time and Game win/loss functionality
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

time_t START, END;

int GetTimeTaken()
{
    return int(END - START);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Helper Functions
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

char* GetUserName(User& user)
{
    char* userData = (char*)&user;
    return userData + 0;
}

char* GetUserId(User& user)
{
    char* userData = (char*)&user;
    return userData + 20;
}

char GetGamesPlayed(User& user)
{
    char* userData = (char*)&user;
    return *(userData + 40);
}

char GetTotalWins(User& user)
{
    char* userData = (char*)&user;
    return *(userData + 41);
}

char GetUserScore(User& user)
{
    char* userData = (char*)&user;
    return *(userData + 42);
}

void SetGamesPlayed(User& user, char gamesPlayed)
{
    char* userData = (char*)&user;
    *(userData + 40) = gamesPlayed;
}

void SetTotalWins(User& user, char totalWins)
{
    char* userData = (char*)&user;
    *(userData + 41) = totalWins;
}

void SetUserScore(User& user, char score)
{
    char* userData = (char*)&user;
    *(userData + 42) = score;
}

Block GetBlock(bool hasFlag, bool isHidden, char value)
{
    Block b = Block();

    // Treat block as a char pointer
    char* blockCharArr = (char*)&b;

    *((char*)blockCharArr + 0) = hasFlag;
    *((char*)blockCharArr + 1) = isHidden;
    *((char*)blockCharArr + 2) = value;

    return b;
}

void SetBlock(Block& b, char value, bool hasFlag = false, bool isHidden = true)
{
    // Treat block as an array of characters
    char* blockCharArr = (char*)&b;

    *((char*)blockCharArr + 0) = hasFlag;
    *((char*)blockCharArr + 1) = isHidden;
    *((char*)blockCharArr + 2) = value;
}

char GetValue(Block& b)
{
    char* blockCharArr = (char*)&b;
    return *((char*)blockCharArr + 2);
}

bool GetHidden(Block& b)
{
    char* blockCharArr = (char*)&b;
    return *((char*)blockCharArr + 1);
}

bool GetFlag(Block& b)
{
    char* blockCharArr = (char*)&b;
    return *((char*)blockCharArr + 0);
}

bool IsValidIndex(unsigned char i, unsigned char j)
{
    return i >= 0 && i < GRID_SIZE && j >= 0 && j < GRID_SIZE;
}

void IncrementCount(Block& b)
{
    char currentValue = GetValue(b);
    if (currentValue != MINE_SYMBOL) SetBlock(b, ++currentValue);
}

void ShowMessage(const char message[], char newLine = 1)
{
    std::cout << message;
    if (newLine == 1) std::cout << std::endl;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Console Functions
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void SetColor(int tcl, int bcl)
{
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), (tcl + (bcl * 16)));
}


void GetRowColbyClick(int& rPos, int& cpos, bool& leftMouseButtonClicked)
{
    HANDLE hInput = GetStdHandle(STD_INPUT_HANDLE);
    DWORD events;
    INPUT_RECORD inputRecord;

    SetConsoleMode(hInput, ENABLE_PROCESSED_INPUT | ENABLE_MOUSE_INPUT | ENABLE_EXTENDED_FLAGS);

    do
    {
        ReadConsoleInput(hInput, &inputRecord, 1, &events);
        if (inputRecord.Event.MouseEvent.dwButtonState == FROM_LEFT_1ST_BUTTON_PRESSED)
        {
            cpos = inputRecord.Event.MouseEvent.dwMousePosition.X;
            rPos = inputRecord.Event.MouseEvent.dwMousePosition.Y;
            leftMouseButtonClicked = true;
            break;
        }

        if (inputRecord.Event.MouseEvent.dwButtonState == RIGHTMOST_BUTTON_PRESSED)
        {
            cpos = inputRecord.Event.MouseEvent.dwMousePosition.X;
            rPos = inputRecord.Event.MouseEvent.dwMousePosition.Y;
            leftMouseButtonClicked = false;
            break;
        }
    } while (true);
}

void GotoRowCol(int rPos, int cPos)
{
    COORD screen;
    HANDLE hOutput = GetStdHandle(STD_OUTPUT_HANDLE);
    screen.X = cPos;
    screen.Y = rPos;
    SetConsoleCursorPosition(hOutput, screen);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////


// Grid Logic
///////////////////////////////////////////////////////////////////////////////////////////////////////
Block** GetGrid()
{
    Block** grid = new Block * [GRID_SIZE];

    for (char i = 0; i < GRID_SIZE; i++)
    {
        *(grid + i) = new Block[GRID_SIZE];
        for (char j = 0; j < GRID_SIZE; j++)
        {
            *(*(grid + i) + j) = GetBlock(0, 1, SAFE_BOX_SYMBOL);
        }
    }

    return grid;
}

void DestroyGrid(Block**& grid)
{
    for (char i = 0; i < GRID_SIZE; i++)
    {
        delete[] * (grid + i);
    }

    delete[] grid;
}

void PlaceMines(Block** grid)
{
    srand(time(nullptr));

    char mineCount = 0;

    while (mineCount < NUM_MINES)
    {
        Block& b = *(*(grid + rand() % GRID_SIZE) + rand() % GRID_SIZE);

        if (GetValue(b) != MINE_SYMBOL)
        {
            SetBlock(b, MINE_SYMBOL);
            mineCount++;
        }
    }
}

void CountMines(Block** grid)
{
    for (unsigned char i = 0; i < GRID_SIZE; i++)
    {
        for (unsigned char j = 0; j < GRID_SIZE; j++)
        {
            Block& b = *(*(grid + i) + j);
            if (GetValue(b) == MINE_SYMBOL)
            {
                for (unsigned char k = 0; k < 8; k++)
                {
                    int x = i + *(D_X + k);
                    int y = j + *(D_Y + k);

                    if (IsValidIndex(x, y)) IncrementCount(grid[x][y]);
                }
            }
        }
    }
}


void PrintBorder(int row, int col, int cellLength, int cellWidth)
{
    int r = row * cellLength;
    int c = col * cellWidth;

    SetColor(13, BLACK);
    GotoRowCol(r, c);

    for (int width = 0; width < cellWidth; width++) std::cout << char(-37);

    for (int length = 0; length < cellWidth; length++)
    {
        GotoRowCol(r + length, c);
        std::cout << char(-37);
    }

    r += cellLength;
    GotoRowCol(r, c);

    for (int width = 0; width <= cellWidth; width++)
    {
        std::cout << char(-37);
    }

    r -= cellLength;
    c += cellWidth;

    for (int length = 0; length < cellWidth; length++)
    {
        GotoRowCol(r + length, c);
        std::cout << char(-37);
    }
}

void Alignment(char i, char j)
{
    int centerRow = i * GRID_SIZE / 2 + (GRID_SIZE / 4);
    int centerCol = j * GRID_SIZE / 2 + (GRID_SIZE / 4);
    PrintBorder(i, j, GRID_SIZE / 2, GRID_SIZE / 2);
    GotoRowCol(centerRow, centerCol);
}

void PrintGrid(Block** grid)
{
    for (unsigned char i = 0; i < GRID_SIZE; i++)
    {
        for (unsigned char j = 0; j < GRID_SIZE; j++)
        {
            Alignment(i, j);
            Block& block = *(*(grid + i) + j);

            // if flagged show flag symbol
            if (GetFlag(block))
            {
                SetColor(RED, BLACK);
                std::cout << FLAG_SYMBOL;
            }

            // if block is hidden show fill character
            else if (GetHidden(block))
            {
                SetColor(YELLOW, BLACK);
                std::cout << (char)-37;
            }
            else
            {
                char Symbol = GetValue(block);
                if (Symbol == MINE_SYMBOL) SetColor(RED, BLACK);
                else if (Symbol == SAFE_BOX_SYMBOL) SetColor(BLACK, BLACK);
                else SetColor(WHITE, BLACK);

                std::cout << Symbol;
            }
        }
        std::cout << std::endl;
    }

    SetColor(WHITE, BLACK);
    GotoRowCol(GRID_SIZE * GRID_SIZE / 2 + (GRID_SIZE / 4), 0);
}

void UnHideBlock(Block** grid, char row, char col)
{
    Block& block = *(*(grid + row) + col);

    if (GetFlag(block)) return;
    if (GetHidden(block)) SetBlock(block, GetValue(block), GetFlag(block), false);
}

char GetNumberOfFlags(Block** grid)
{
    char flagCount = 0;
    for (unsigned char i = 0; i < GRID_SIZE; i++)
    {
        for (unsigned char j = 0; j < GRID_SIZE; j++)
        {
            Block& b = *(*(grid + i) + j);
            if (GetFlag(b)) flagCount++;
        }
    }

    return flagCount;
}

void ToggleFlag(Block** grid, char row, char col)
{
    Block& block = *(*(grid + row) + col);
    if (GetNumberOfFlags(grid) + 1 > NUM_MINES && GetFlag(block) == false) return;
    if (GetHidden(block)) SetBlock(block, GetValue(block), !GetFlag(block), GetHidden(block));
}


///////////////////////////////////////////////////////////////////////////////////////////////////


// Flood Fill Starts here
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

char X_COORDINATES[200];
char Y_COORDINATES[200];

int NUM_COORDINATES = 0;

void PushCoordinates(char x, char y)
{
    *(X_COORDINATES + NUM_COORDINATES) = x;
    *(Y_COORDINATES + NUM_COORDINATES) = y;

    NUM_COORDINATES++;
}

void GetNextCoordinates(char& x, char& y)
{
    if (NUM_COORDINATES > 0)
    {
        NUM_COORDINATES--;

        x = *(X_COORDINATES + NUM_COORDINATES);
        y = *(Y_COORDINATES + NUM_COORDINATES);
    }
}

void CheckForBoundary(Block** grid, char x, char y)
{
    for (unsigned char i = 0; i < 8; i++)
    {
        char localX = x + *(D_X + i);
        char localY = y + *(D_Y + i);

        if (IsValidIndex(localX, localY))
        {
            Block& block = *(*(grid + localX) + localY);
            char value = GetValue(block);

            if (value != SAFE_BOX_SYMBOL && value != MINE_SYMBOL)
            {
                if (GetHidden(block) && GetFlag(block) == 0) PushCoordinates(localX, localY);
            }
        }
    }
}

void FloodFill(Block** grid, char startRow, char startCol)
{
    Block& b = *(*(grid + startRow) + startCol);

    if (GetValue(b) != SAFE_BOX_SYMBOL) return;
    if (GetHidden(b) == 0) return;

    // push current coordinates to global arrays
    PushCoordinates(startRow, startCol);

    while (NUM_COORDINATES != 0)
    {
        char x, y;
        GetNextCoordinates(x, y);
        UnHideBlock(grid, x, y);

        if (GetValue(*(*(grid + x) + y)) != SAFE_BOX_SYMBOL) continue;

        // push 8 surrounding coordinates to global arrays for later processing
        for (unsigned char k = 0; k < 8; k++)
        {
            char localX = x + *(D_X + k);
            char localY = y + *(D_Y + k);

            if (IsValidIndex(localX, localY))
            {
                Block& block = *(*(grid + localX) + localY);

                if (GetValue(block) == SAFE_BOX_SYMBOL && GetHidden(block) && GetFlag(block) == false)
                {
                    PushCoordinates(localX, localY);
                    CheckForBoundary(grid, localX, localY);
                }
            }
        }
    }
}


void HandleBlock(Block** grid, char row, char col, bool& gameOver)
{
    Block& b = *(*(grid + row) + col);

    char value = GetValue(b);

    if (GetFlag(b) == true) return;
    if (value == SAFE_BOX_SYMBOL) FloodFill(grid, row, col);
    else
    {
        if (value == MINE_SYMBOL) gameOver = true;
        UnHideBlock(grid, row, col);
    }
}

void ExtremeFlood(Block** grid, char row, char col, bool& isGameOver)
{
    Block& b = *(*(grid + row) + col);

    HandleBlock(grid, row, col, isGameOver);

    char value = GetValue(b);

    if (value != SAFE_BOX_SYMBOL && value != MINE_SYMBOL)
    {
        char countFlag = '0';

        for (unsigned char k = 0; k < 8; k++)
        {
            int x = row + *(D_X + k);
            int y = col + *(D_Y + k);

            if (IsValidIndex(x, y))
            {
                Block block = *(*(grid + x) + y);

                if (GetFlag(block) == 1) countFlag++;

                if (countFlag == value) break;
            }
        }

        if (countFlag != value) return;

        for (unsigned char k = 0; k < 8; k++)
        {
            int x = row + *(D_X + k);
            int y = col + *(D_Y + k);

            if (IsValidIndex(x, y))
            {
                Block nestedBlock = *(*(grid + x) + y);
                HandleBlock(grid, x, y, isGameOver);
            }
        }
    }
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Win/Loss Logic goes here
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ProcessGameOver(char result, User& user)
{
    SetGamesPlayed(user, GetGamesPlayed(user) + 1);

    if (result == 0)
    {
        ShowMessage("Game Over");
    }
    else
    {
        SetTotalWins(user, GetTotalWins(user) + 1);
        if (GetTimeTaken() < GetUserScore(user))SetUserScore(user, GetTimeTaken());
        if (GetTotalWins(user) == 1)SetUserScore(user, GetTimeTaken());
        ShowMessage("You Win");
    }

    std::cout << "Time Taken is: " << GetTimeTaken() << " Secs" << std::endl;
}

bool CheckWinCondition(Block** grid)
{
    for (unsigned char i = 0; i < GRID_SIZE; i++)
    {
        for (unsigned char j = 0; j < GRID_SIZE; j++)
        {
            Block& block = *(*(grid + i) + j);

            if (GetValue(block) != MINE_SYMBOL && GetHidden(block)) return false;
        }
    }

    return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void SelectDifficulty()
{
    char level;
    ShowMessage("Choose difficulty Level '0', '1', '2' = ", 0);
    std::cin >> level;

    system("cls");

    if (level == '1')
    {
        GRID_SIZE = GRID_SIZE + 2;
        NUM_MINES = NUM_MINES + 4;
    }
    else if (level == '2')
    {
        GRID_SIZE = GRID_SIZE + 4;
        NUM_MINES = NUM_MINES + 8;
    }
}

/// Main Game Play handles Mine Sweeper core logic
/// Returns 0 if Player loses, 1 otherwise

char PlayGame()
{
    int row, col;
    bool isLeftClick = false, isGameOver = false;

    SelectDifficulty();

    Block** grid = GetGrid();

    PlaceMines(grid);
    CountMines(grid);

    while (true)
    {
        PrintGrid(grid);

        if (CheckWinCondition(grid)) break;

        if (isGameOver) break;

        GetRowColbyClick(row, col, isLeftClick);

        row /= GRID_SIZE / 2;
        col /= GRID_SIZE / 2;

        if (row < GRID_SIZE + 2 && col < GRID_SIZE + 2)
        {
            if (isLeftClick)
            {
                ExtremeFlood(grid, row, col, isGameOver);
            }
            else ToggleFlag(grid, row, col);
        }
    }

    DestroyGrid(grid);

    if (isGameOver) return 0;
    return 1;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Load/Save from/to file goes here
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Load(User users[], char& userCount)
{
    std::ifstream fin(FILE_NAME, std::ifstream::binary | std::ifstream::in);
    if (!fin) return;

    userCount = 0;

    while (fin.read((char*)&users[userCount], sizeof(User))) userCount++;

    fin.close();
}

User LoadUser(char id[])
{
    char userCount = 0;
    User users[100];

    Load(users, userCount);

    for (char i = 0; i < userCount; i++)
    {
        if (strcmp(GetUserId(*(users + i)), id) == 0) return *(users + i);
    }

    // return empty object
    return User();
}

bool HasUser(char userId[])
{
    User users[100];
    char userCount = 0;

    Load(users, userCount);

    if (userCount)

        for (char i = 0; i < userCount; i++)
        {
            if (strcmp(GetUserId(users[i]), userId) == 0) return true;
        }

    return false;
}

void Save(User& user)
{
    char userAlreadyExists = 0;
    char userCount = 0;
    User users[100];

    Load(users, userCount);

    std::ofstream fOut(FILE_NAME, std::ofstream::binary | std::ofstream::out);

    for (char i = 0; i < userCount; i++)
    {
        if (strcmp(GetUserId(*(users + i)), user.Id) == 0)
        {
            userAlreadyExists = 1;
            *(users + i) = user;
        }

        fOut.write((char*)&*(users + i), sizeof(User));
    }

    // if user do not exists enter it
    if (userAlreadyExists == 0) fOut.write((char*)&user, sizeof(User));

    fOut.close();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Menu Logic goes here
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void PrintWelcomeMenu()
{
    std::cout << "Welcome" << std::endl;
    std::cout << "1:Login" << std::endl;
    std::cout << "2:Register" << std::endl;
    std::cout << "3:ShowLeaderboard" << std::endl;
    std::cout << "4:Quit" << std::endl;
}

void ShowChar(char number, char newLine = 1)
{
    std::cout << (int)number;
    if (newLine == 1) std::cout << std::endl;
}

void GetUserInput(User& user, char takeOnlyId = 1)
{
    char* userData = (char*)&user;

    ShowMessage("Please enter your ID:", 0);
    std::cin >> userData + 20;

    if (takeOnlyId) return;

    ShowMessage("Please enter your Name:", 0);
    std::cin >> userData;
}

void Swap(User& user1, User& user2)
{
    User temp = user1;
    user1 = user2;
    user2 = temp;
}

// bubble sort array 
void Sort(User users[], char userCount)
{
    for (char i = 0; i < userCount - 1; i++)
    {
        for (char j = 0; j < userCount - i - 1; j++)
        {
            if (GetUserScore(*(users + j)) > GetUserScore(*(users + j + 1)))
            {
                Swap(*(users + j), *(users + j + 1));
            }
        }
    }

    char LoseCount = 0;

    for (int i = 0; i < userCount; i++)
    {
        if (GetUserScore(*(users + i)) == 0) LoseCount++;
    }

    for (int i = 0; i < LoseCount; i++)
    {
        for (int j = 0; j + 1 < userCount; j++)
        {
            Swap(*(users + j), *(users + j + 1));
        }
    }
}

void PrintUserData(const User& user)
{
    ShowMessage("Name :", 0);
    ShowMessage(user.Name);
    std::cout << std::endl;
    ShowMessage("ID :", 0);
    ShowMessage(user.Id);
    std::cout << std::endl;
    ShowMessage("Games Played:", 0);
    ShowChar(user.GamesPlayed);
    std::cout << std::endl;
    ShowMessage("Total Wins :", 0);
    ShowChar(user.TotalWins);
    std::cout << std::endl;
    ShowMessage("Score :", 0);
    ShowChar(user.Score);
    std::cout << std::endl;

    ShowMessage("");
}

void ShowLeaderboard()
{
    User users[100];
    char userCount = 0;

    Load(users, userCount);
    Sort(users, userCount);

    if (userCount == 0) ShowMessage("No Players In Leaderboard");
    else ShowMessage("---------Printing Leaderboard---------");

    for (char i = 0; i < userCount; i++) PrintUserData(users[i]);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int main()
{
    // clear screen and start fresh
    system("cls");

    char choice = 0, result = -1, quit = 0;
    User currentUser;

    while (true)
    {
        PrintWelcomeMenu();

        ShowMessage("Please Enter Your Choice:", 0);
        std::cin >> choice;
        system("cls");

        if (choice == '1')
        {
            GetUserInput(currentUser);

            if (HasUser(GetUserId(currentUser)))
            {
                currentUser = LoadUser(currentUser.Id);

                time(&START);
                result = PlayGame();
                time(&END);

                break;
            }

            ShowMessage("User do not exist, try registering first");
        }
        else if (choice == '2')
        {
            GetUserInput(currentUser, 0);

            if (HasUser(GetUserId(currentUser))) ShowMessage("User already exist, discarding");
            else ShowMessage("User Saved, Try logging in now");

            Save(currentUser);
        }
        else if (choice == '3') ShowLeaderboard();
        else if (choice == '4') { quit = 1; break; }
        else ShowMessage("Wrong Choice, Try Again");
    }

    if (quit == 1) return 0;

    ProcessGameOver(result, currentUser);
    Save(currentUser);

    return 0;
}
