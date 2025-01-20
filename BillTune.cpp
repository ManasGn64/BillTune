#include <iostream>
#include <iomanip>
#include <string>
#include <cstdlib>
#include <cstring>
#include <stdio.h>
#include <conio.h>
#include <fstream>
#include <windows.h>
#include <cstdio>
#include <time.h>

using namespace std;

// Global variables 
int cursorY = 7, unusedR = 0, foundFlag = 0;
ofstream foutStream;
ifstream finStream;

// Track the "top customer" details
char  topCustomerName[50], topCustomerCitizenship[50];
int   topCustomerAge    = 0,
      topCustomerID     = 0;
float topCustomerPoints = 0,
      topCustomerMoney  = 0;

// Forward declarations
void creditCard();
void adminMenu();
void userMenu();

//--------------------------------------------------------------------------------------
//To 'draw' a heading design at the top of the screen.
void drawDesign(int width, char ch)
{
    cout << "\n\n\n";
    system("cls");
    cout << "\t\t\t" << setw(width+5) << setfill(ch);
    cout << "\n\n\t\t\t\t\t" << "SAHUJI";
    cout << "\n\n\t\t\t" << setw(width) << setfill(ch);
    cout << "\n\n";
}

// A function to move the console cursor to a specific X/Y position.
COORD coord = {0, 0};
void gotoxy(int x, int y)
{
    COORD coordLocal;
    coordLocal.X = x;
    coordLocal.Y = y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coordLocal);
}

//--------------------------------------------------------------------------------------
// DATE struct holds basic date fields
struct date
{
    int mm, dd, yy;
};

// Secure password reading function
string getpass(const char *prompt, bool show_asterisk = true)
{
    const char BACKSPACE = 8;
    const char RETURN    = 13;

    string password;
    unsigned char ch = 0;

    cout << prompt << endl;

    DWORD con_mode;
    DWORD dwRead;
    HANDLE hIn = GetStdHandle(STD_INPUT_HANDLE);

    GetConsoleMode(hIn, &con_mode);
    SetConsoleMode(hIn, con_mode & ~(ENABLE_ECHO_INPUT | ENABLE_LINE_INPUT));

    while(ReadConsoleA(hIn, &ch, 1, &dwRead, NULL) && ch != RETURN)
    {
        if(ch == BACKSPACE)
        {
            if(!password.empty())
            {
                if(show_asterisk) cout << "\b \b";
                password.resize(password.length() - 1);
            }
        }
        else
        {
            password += ch;
            if(show_asterisk) cout << '*';
        }
    }
    cout << endl;
    return password;
}

//--------------------------------------------------------------------------------------
// Base class for item; holds fundamental product info and date.
class BaseItem
{
    friend class bill;

protected:
    int  itemNumber;
    char itemName[25];
    date manufactureDate;

public:
    void add()
    {
        cout << "\n\n\tItem identification No: ";
        cin  >> itemNumber;

        cout << "\n\n\tName of the item: ";
        cin  >> itemName;

        cout << "\n\n\tManufacturing Date(dd-mm-yy): ";
        cin  >> manufactureDate.mm >> manufactureDate.dd >> manufactureDate.yy;
    }

    void uadd()
    {
        cout << "\n\n\tName of the item: ";
        cin  >> itemName;

        cout << "\n\n\tManufacturing Date(dd-mm-yy): ";
        cin  >> manufactureDate.mm >> manufactureDate.dd >> manufactureDate.yy;
    }

    void show()
    {
        cout << "\n\tItem identification No: " << itemNumber;
        cout << "\n\n\tName of the item: "       << itemName;
        cout << "\n\n\tDate : " << manufactureDate.mm << "-" 
                                << manufactureDate.dd << "-" 
                                << manufactureDate.yy;
    }

    int getItemNumber()
    {
        return itemNumber;
    }

    void report()
    {
        gotoxy(3, cursorY);
        cout << itemNumber;
        gotoxy(13, cursorY);
        puts(itemName);
    }
} baseItemObject;

//--------------------------------------------------------------------------------------
// Derived class that holds pricing and quantity information, extends BaseItem.
class ProductItem : public BaseItem
{
    friend class bill;

protected:
    float markPrice, costPrice, quantityInStock;
    float taxPercent, grossPrice, discountRate, individualAmount;
    float initialQuantity, totalAmount;

public:
    void add();
    void show();
    void pay();
    void pay1(int, int);
    void refillItem();
    void editItem();
    void report();
    void del(int);
    void report1();
    void surv();

    float getIndividualAmount()
    {
        return individualAmount;
    }
    void setQuantityInStock(int a)
    {
        quantityInStock = a;
    }
    float getTotalAmount()
    {
        return totalAmount;
    }
    int   getNumber()
    {
        return getItemNumber();
    }
} productItem;

//--------------------------------------------------------------------------------------
// Implementation of ProductItem class methods
void ProductItem::add()
{
    BaseItem::add();

    cout << "\n\n\tCost Price: ";
    cin  >> costPrice;

    cout << "\n\n\tMark price: ";
    cin  >> markPrice;

    cout << "\n\n\tQuantity: ";
    cin  >> quantityInStock;
    initialQuantity = quantityInStock;

    cout << "\n\n\tTax percent: ";
    cin  >> taxPercent;

    cout << "\n\n\tDiscount percent: ";
    cin  >> discountRate;

    grossPrice       = markPrice + (markPrice * (taxPercent / 100));
    individualAmount = (grossPrice - (grossPrice * (discountRate / 100)));
    totalAmount      = individualAmount * quantityInStock;

    foutStream.write((char *)&productItem, sizeof(productItem));
    foutStream.close();
}

void ProductItem::editItem()
{
    BaseItem::uadd();

    cout << "\n\n\tCost Price: ";
    cin  >> costPrice;

    cout << "\n\n\tMark Price: ";
    cin  >> markPrice;

    cout << "\n\n\tTax percent: ";
    cin  >> taxPercent;

    cout << "\n\n\tDiscount percent: ";
    cin  >> discountRate;

    grossPrice       = markPrice + (markPrice * (taxPercent / 100));
    individualAmount = (grossPrice - (grossPrice * (discountRate / 100)));
    totalAmount      = individualAmount * quantityInStock;
}

void ProductItem::del(int d)
{
    foundFlag = 0;
    int itemToDelete = d;

    finStream.close();
    cout << "this is a try" << itemToDelete;

    fstream tmp("temp.dat", ios::binary | ios::out);
    cout << "\n\n\tEnter Item Number to be deleted :";
    cin  >> itemToDelete;

    finStream.open("itemstore.dat", ios::binary);
    if(!finStream)
    {
        cout << "\n\nFile Not Found...";
    }
    finStream.seekg(0);
    while(finStream.read((char*)&productItem, sizeof(productItem)))
    {
        int x = productItem.BaseItem::getItemNumber();
        if(x != itemToDelete)
        {
            tmp.write((char*)&productItem, sizeof(productItem));
        }
        else
        {
            foundFlag = 1;
        }
    }
    finStream.close();
    tmp.close();

    foutStream.open("itemstore.dat", ios::trunc | ios::binary);
    foutStream.seekp(0);
    tmp.open("temp.dat", ios::binary | ios::in);

    if(!tmp)
    {
        cout << "Error in File";
    }
    while(tmp.read((char*)&productItem, sizeof(productItem)))
    {
        foutStream.write((char*)&productItem, sizeof(productItem));
    }
    tmp.close();
    foutStream.close();

    if(foundFlag == 1)
        cout << "\n\t\tItem Successfully Deleted";
    else if (foundFlag == 0)
        cout << "\n\t\tItem does not Exist! Please Retry";
    getch();
}

void ProductItem::refillItem()
{
refillStart:
    drawDesign(45, '*');
    int inum, addedQuantity;
    cout << "\t\t\t\n Enter the item identification number:";
    cin  >> inum;

    fstream fio;
    fio.open("itemstore.dat", ios::in | ios::out | ios::binary);
    if(!fio)
    {
        cout << "\n\nFile Not Found...\nProgram Terminated!";
    }
    int ans = 0;
    fio.seekg(0);

    while(fio.read((char*)&productItem, sizeof(productItem)))
    {
        int pos = fio.tellg();

        if(inum == productItem.BaseItem::getItemNumber())
        {
            ans++;
            cout << endl << "Input the quantity to be added" << endl;
            cin  >> addedQuantity;
            fio.seekg(pos - sizeof(productItem));
            quantityInStock += addedQuantity;
            totalAmount      = quantityInStock * individualAmount;
            initialQuantity  = quantityInStock;
            fio.write((char*)&productItem, sizeof(productItem));
        }
    }

    char ans1, ans2;
    if(ans == 0)
    {
        cout << endl << "You entered invalid choice";
        cout << endl << "Do you wish to add quantity (Y/N)";
        cin  >> ans1;
    }
    if(ans1 == 'y' || ans1 == 'Y')
        goto refillStart;

    cout << endl << "Do you want add other products(Y/N)";
    cin  >> ans2;
    if(ans2 == 'y' || ans2 == 'Y')
        goto refillStart;

    fio.close();
}

void ProductItem::show()
{
    finStream.open("itemstore.dat", ios::binary);
    finStream.read((char*)&productItem, sizeof(productItem));
    BaseItem::show();
    cout << "\n\n\tindividual amount: " << individualAmount;
    cout << "\n\n\tquantity : "         << quantityInStock;
    finStream.close();
}

void ProductItem::pay()
{
    show();
    cout << "\n\n\n\t\t*********************************************";
    cout << "\n\t\t                 DETAILS                  ";
    cout << "\n\t\t*********************************************";
    cout << "\n\n\t\tCOST PRICE                        :Rs." << costPrice;
    cout << "\n\t\tMARK PRICE                        :Rs." << markPrice;
    cout << "\n\t\tQUANTITY                          :"       << quantityInStock;
    cout << "\n\t\tTAX PERCENTAGE                    :"       << taxPercent;
    cout << "\n\t\tDISCOUNT PERCENTAGE               :"       << discountRate;
    cout << "\n\t\tSELLING PRICE                     :Rs."    << individualAmount;
    cout << "\n\t\tprofit                            :Rs."    << quantityInStock*(individualAmount - costPrice);
    cout << "\n\t\t----------------------------------------------";
    cout << "\n\t\t*********************************************";
}

void ProductItem::surv()
{
    // If the current stock is below 10% of the initial quantity, you need to add more
    float threshold = 0.1f * initialQuantity;
    if (quantityInStock < threshold)
    {
        cout << endl << endl << "You need to add: ";
        BaseItem::show();
        cout << endl << "\tRemaining: " << quantityInStock;
        cout << endl << endl;
    }
    else
    {
        cout << endl << "You don't need to add";
        BaseItem::show();
        cout << endl << "\tRemaining: " << quantityInStock;
        cout << endl << endl;
    }
}

void ProductItem::report()
{
    BaseItem::report();
    gotoxy(23, cursorY);
    cout << costPrice;
    gotoxy(33, cursorY);
    cout << markPrice;
    gotoxy(44, cursorY);
    cout << quantityInStock;
    gotoxy(52, cursorY);
    cout << taxPercent;
    gotoxy(64, cursorY);
    cout << discountRate;
    gotoxy(74, cursorY);
    cout << individualAmount;

    cursorY = cursorY + 1;
    if(cursorY == 50)
    {
        gotoxy(25, 50);
        cout << "PRESS ANY KEY TO CONTINUE...";
        getch();
        cursorY = 7;
        system("cls");
        gotoxy(30,3);
        cout << " ITEM DETAILS ";
        gotoxy(3,5);
        cout << " ID NUMBER";
        gotoxy(13,5);
        cout << "NAME";
        gotoxy(23,5);
        cout << "PRICE";
        gotoxy(33,5);
        cout << "QUANTITY";
        gotoxy(44,5);
        cout << "TAX";
        gotoxy(52,5);
        cout << "DEDUCTION";
        gotoxy(80,5);
        cout << " SELLING PRICE";
    }
}

//--------------------------------------------------------------------------------------
// MAIN FUNCTION
int main()
{
    int   choice;
    float grandTotal;

    cout << "\n \t\t\t=============================================================\n";
    cout << "\n\t\t\t\t\tWELCOME TO SAHUJI  \n";
    cout << "\n \t\t\t=============================================================\n\n";
    cout << "\n\n\n\n\t\t\t\t\t  Made By:";
    cout << "\n\n\t\t\t\t Name\t\t\t\tSymbol.no";
    cout << "\n\t\t\t\t Prashant Basyal\t\t\t27600431";

    getch();

    while(1)
    {
menu:
        {
            static int  remainingAttempts = 3;
            std::string passwordAttempt, retryAnswer;

            drawDesign(45, '*');
            cout << "\n\t\t\t  1. VIEW/GENERATE CUSTOMER ID ";
            cout << "\n\t\t\t  2. ADMIN ";
            cout << "\n\t\t\t  3. USER ";
            cout << "\n\t\t\t  4. EXIT ";
            cout << "\n\t\t\t Enter Your choice: ";
            cin  >> choice;

            switch(choice)
            {
            case 1:
                creditCard();
                break;

            case 2:
                if(remainingAttempts >= 1)
                {
doneAdminLogin:
                    passwordAttempt = getpass("\n\n\n\t\t\t\tEnter the password for Admin : ", true);

                    if(passwordAttempt == "admin")
                    {
                        adminMenu();
                        getch();
                        break;
                    }
                    else
                    {
                        remainingAttempts--;
                        drawDesign(50, '*');
                        if(remainingAttempts >= 1)
                        {
                            cout << "\n\n\n\t\t\tWrong password you have " << remainingAttempts << " chances left:";
                            cout << "\n\n\n\t\t\t Do you want to try again (y/n) ? ";
                            cin  >> retryAnswer;
                            if(retryAnswer == "y" || retryAnswer == "Y")
                                goto doneAdminLogin;
                        }
                        break;
                    }
                }
                else
                {
                    cout << "\n\n\n\t\t\t Password try limit exceeded.";
                }
                getch();
                break;

            case 3:
                userMenu();
                break;

            case 4:
            {
                drawDesign(45, '*');
                gotoxy(20,15);
                cout << "ARE YOU SURE, YOU WANT TO EXIT (Y/N)?";
                char yn;
                cin >> yn;
                if((yn=='Y')||(yn=='y'))
                {
                    drawDesign(45, '*');
                    gotoxy(20,15);
                    cout <<"|=======================================================================|";
                    gotoxy(20,16);
                    cout<<"|********************************THANKS*********************************|";
                    gotoxy(20,17);
                    cout <<"|=======================================================================|";
                    getch();
                    exit(0);
                }
                else if((yn=='N')||(yn=='n'))
                {
                    goto menu;
                }
                else
                {
                    goto menu;
                }
            }
            break;

            default:
                cout << "\n\n\n\n\t\t\t\t\tEnter valid choice\n";
                getch();
                break;
            }
        }
    }
    return 0;
}

//--------------------------------------------------------------------------------------
// Class to store customer information
class Customer
{
    friend class bill;
protected:
    char  name[50], citizenship[50];
    int   age, creditID;
    float points, moneyUsed;

public:
    void cal()
    {
        // Check if this user overtakes the top customer
        if(topCustomerPoints < points)
        {
            strcpy(topCustomerName,        name);
            strcpy(topCustomerCitizenship, citizenship);
            topCustomerAge    = age;
            topCustomerID     = creditID;
            topCustomerPoints = points;
            topCustomerMoney  = moneyUsed;
        }
    }

    int  getCreditID()
    {
        return creditID;
    }

    void getdata()
    {
        cout << endl << "input name of customer :";
        cin  >> name;

        cout << endl << "input citizenship no.: ";
        cin  >> citizenship;

        cout << endl << "input age:";
        cin  >> age;

        calculation();
    }

    void calculation()
    {
        srand(time(NULL));
        int number = (rand() % 9000) + 1000;
        creditID   = number;
    }

    int retcrad()
    {
        return creditID;
    }

    void display()
    {
        cout << endl << "\t Name: "              << name;
        cout << endl << "\t Citizenship no.: "   << citizenship;
        cout << endl << "\t Age: "               << age;
        cout << endl << "\t Customer ID no. is: " << creditID;
        cout << endl << "\t\t\txxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";
    }

    void calc(int credit, int quantity, int itemChoice);
} customerObject;

//--------------------------------------------------------------------------------------
// Bill class for storing single purchase details
class bill
{
protected:
    int  customerCreditID, productID, totalCost, singlePrice, quantity;
    char customerName[50], customerCitizenship[50], productName[50];

public:
    void disp()
    {
        cout << endl;
        if(customerCreditID != 0)
            cout << endl << "costumer name:    "  << customerName;
        if(customerCreditID != 0)
            cout << endl << "Customer ID no. : "  << customerCitizenship;

        cout << endl << "product name:     "   << productName;

        if(customerCreditID != 0)
            cout << endl << "Customer ID no. : "  << customerCreditID;

        cout << endl << "id of product:    "   << productID;
        cout << endl << "individual price: "   << singlePrice;
        cout << endl << "quantity:         "   << quantity;
        cout << endl << "total price:      "   << totalCost;
        cout << endl;
    }

    void billcalc(int, int, int);
} billObject;

//--------------------------------------------------------------------------------------
// Implementation details
void adminMenu()
{
adminMenuLabel:
    drawDesign(45, '*');
    cout.setf(ios::fixed);
    cout.setf(ios::showpoint);
    cout << setprecision(2);

    fstream tmp("temp.dat", ios::binary | ios::out);
    int    option;
    float  gtotal;
    int    ff;

    cout << "\t\t\t 1. Add product \n ";
    cout << "\t\t\t 2. View desired product details \n ";
    cout << "\t\t\t 3. View product \n ";
    cout << "\t\t\t 4. Delete \n ";
    cout << "\t\t\t 5. Edit \n ";
    cout << "\t\t\t 6. Refill \n ";
    cout << "\t\t\t 7. Survey \n ";
    cout << "\t\t\t 8. View bill \n";
    cout << "\t\t\t 9. Gift hamper \n ";
    cout << "\t\t\t 10. Return to main menu \n ";
    cout << "\t\t\t enter your option :\n ";
    cin  >> option;

    switch(option)
    {
    case 1:
    {
        drawDesign(45, '*');
addProduct:
        foutStream.open("itemstore.dat", ios::binary | ios::app);
        productItem.add();
        cout << "\n\t\tItem Added Successfully!";
        char choice;
        cout << " Do you want to add more item(y/n) ?\n";
        cin  >> choice;
        if(choice == 'y' || choice == 'Y')
        {
            goto addProduct;
        }
        else
            goto adminMenuLabel;
        getch();
        break;
    }

    case 2:
    {
        drawDesign(45, '*');
viewProductDetail:
        foundFlag = 0;
        int desiredID;
        cout << "\n\n\t\tEnter Item identification Number : ";
        cin  >> desiredID;

        finStream.open("itemstore.dat", ios::binary);
        if(!finStream)
        {
            cout << "\n\nFile Not Found...\nProgram Terminated!";
            goto adminMenuLabel;
            break;
        }
        finStream.seekg(0);
        while(finStream.read((char*)&productItem, sizeof(productItem)))
        {
            int x = productItem.BaseItem::getItemNumber();
            if(x == desiredID)
            {
                productItem.pay();
                foundFlag = 1;
                break;
            }
        }
        if(foundFlag == 0)
            cout << "\n\t\tItem does not exist....Please Retry!";
        getch();
        finStream.close();

        char choice;
        cout << "\n\t do you want to view more item (y/n) ?";
        cin  >> choice;
        if(choice == 'y' || choice == 'Y')
        {
            goto viewProductDetail;
        }
        else
            goto adminMenuLabel;
        break;
    }

    case 3:
    {
        drawDesign(45, '*');
        gotoxy(34,3);
        cout << " <<  BILL DETAILS >> ";
        gotoxy(1,5);
        cout << "ITEM ID ";
        gotoxy(13,5);
        cout << "NAME";
        gotoxy(23,5);
        cout << "COST PRICE";
        gotoxy(33,5);
        cout << "MRP";
        gotoxy(44,5);
        cout << "QUANTITY";
        gotoxy(52,5);
        cout << "TAX %";
        gotoxy(64,5);
        cout << "DISCOUNT %";
        gotoxy(74,5);
        cout << "SELLING PRICE";

        finStream.open("itemstore.dat", ios::binary);
        if(!finStream)
        {
            cout << "\n\nFile Not Found...";
            goto adminMenuLabel;
            break;
        }

        finStream.seekg(0);
        gtotal = 0;
        cursorY = 7;

        while(!finStream.eof())
        {
            finStream.read((char*)&productItem, sizeof(productItem));
            if(!finStream.eof())
            {
                productItem.report();
                gtotal += productItem.getTotalAmount();
                ff = 0;
            }
            if(ff != 0) gtotal = 0;
        }
        gotoxy(17, cursorY);
        cout << "\n\n\n\t\t\tGrand Total=" << gtotal;
        getch();
        finStream.close();
        goto adminMenuLabel;
        break;
    }

    case 4:
    {
deleteProduct:
        drawDesign(45, '*');
        foundFlag = 0;
        int itemToDelete;
reEnterDelete:
        cout << "\n\n\tEnter Item Number to be deleted :";
        cin  >> itemToDelete;

        char choice;
        cout << " Do you want to delete this item(y/n) ?\n";
        cin  >> choice;
        if(choice == 'y' || choice == 'Y')
        {
            finStream.open("itemstore.dat", ios::binary);
            if(!finStream)
            {
                cout << "\n\nFile Not Found...";
            }
            finStream.seekg(0);

            while(finStream.read((char*)&productItem, sizeof(productItem)))
            {
                int x = productItem.BaseItem::getItemNumber();
                if(x != itemToDelete)
                    tmp.write((char*)&productItem, sizeof(productItem));
                else
                {
                    foundFlag = 1;
                }
            }
            finStream.close();
            tmp.close();

            foutStream.open("itemstore.dat", ios::trunc | ios::binary);
            foutStream.seekp(0);
            tmp.open("temp.dat", ios::binary | ios::in);

            if(!tmp)
            {
                cout << "Error in File";
            }

            while(tmp.read((char*)&productItem, sizeof(productItem)))
                foutStream.write((char*)&productItem, sizeof(productItem));

            tmp.close();
            foutStream.close();

            if(foundFlag == 1)
                cout << "\n\t\t Item Successfully Deleted";
            else if (foundFlag == 0)
                cout << "\n\t\t Item does not Exist! Please Retry";

            {
                char choice2;
                cout << "\n\t Do you want to delete more item (y/n) ?";
                cin  >> choice2;
                if(choice2 == 'y' || choice2 == 'Y')
                {
                    goto deleteProduct;
                }
                else
                {
                    getch();
                    goto adminMenuLabel;
                }
            }
            break;
        }
        else
            goto reEnterDelete;
    }

    case 5:
    {
editItemLabel:
        drawDesign(45, '*');
        int inum;
        cout << "\n\n\tEnter Item Number to be edited :";
        cin  >> inum;

        fstream fio;
        fio.open("itemstore.dat", ios::in | ios::out | ios::binary);
        if(!fio)
        {
            cout << "\n\nFile Not Found...\nProgram Terminated!";
        }
        int ans = 0;
        fio.seekg(0);

        while(fio.read((char*)&productItem, sizeof(productItem)))
        {
            int pos = fio.tellg();
            if(inum == productItem.BaseItem::getItemNumber())
            {
                ans++;
                fio.seekg(pos - sizeof(productItem));
                productItem.editItem();
                fio.write((char*)&productItem, sizeof(productItem));
            }
        }

        char ans1, ans2;
        if(ans == 0)
        {
            cout << endl << "You entered invalid choice";
            cout << endl << "Do you wish to edit (Y/N)";
            cin  >> ans1;
        }
        if(ans1 == 'y' || ans1 == 'Y')
            goto editItemLabel;

        cout << endl << "Do you want edit other products(Y/N) ?";
        cin  >> ans2;
        if(ans2 == 'y' || ans2 == 'Y')
            goto editItemLabel;

        goto adminMenuLabel;
        break;
    }

    case 6:
    {
        productItem.refillItem();
        goto adminMenuLabel;
    }

    case 7:
    {
        drawDesign(45, '*');
        finStream.open("itemstore.dat", ios::binary);

        while(finStream.read((char*)&productItem, sizeof(productItem)))
        {
            productItem.surv();
        }
        finStream.close();
        getch();
        goto adminMenuLabel;
    }

    case 8:
    {
        drawDesign(45, '*');
        fstream em;
        em.open("bill.txt", ios::app | ios::out | ios::binary | ios::in);
        em.seekg(0);
        while(em.read((char*)&billObject, sizeof(billObject)))
        {
            billObject.disp();
        }
        em.close();

        getch();
        goto adminMenuLabel;
        break;
    }

    case 9:
    {
        fstream em;
        em.open("costumer.txt", ios::app | ios::out | ios::in);
        em.read(reinterpret_cast<char*>(&customerObject), sizeof(customerObject));
        while(!em.eof())
        {
            customerObject.cal();
            em.read(reinterpret_cast<char*>(&customerObject), sizeof(customerObject));
        }
        em.close();
        drawDesign(45, '*');
        cout << endl << "\t\t\t\tCONGRATULATIONS!!!";
        cout << endl << "\n\t\t\t\tCostumer of the year is " << topCustomerName;
        cout << endl << "full detail of customer of the year is: ";
        cout << endl << "\t Mr. " << topCustomerName;
        cout << endl << "\t Citizenship no.: " << topCustomerCitizenship;
        cout << endl << "\t age :" << topCustomerAge;
        cout << endl << "\t Customer ID : " << topCustomerID;

        getch();
        goto adminMenuLabel;
        break;
    }

    case 10:
    {
        break;
    }

    default:
        cout << "\n\n\n\n\t\t\t\t\tEnter valid choice\n";
        getch();
        goto adminMenuLabel;
        break;
    }
}

//--------------------------------------------------------------------------------------
void Customer::calc(int credit, int quantity, int itemChoice)
{
    finStream.open("itemstore.dat", ios::binary);
    while(finStream.read((char*)&productItem, sizeof(productItem)))
    {
        if(itemChoice == productItem.BaseItem::getItemNumber())
        {
            float d = productItem.getIndividualAmount();
            fstream fio;
            fio.open("costumer.txt", ios::in | ios::out | ios::binary);

            while(fio.read((char*)&customerObject, sizeof(customerObject)))
            {
                int pos = fio.tellg();

                if(credit == creditID)
                {
                    fio.seekg(pos - sizeof(customerObject));
                    float buyAmount = d * quantity;
                    moneyUsed += buyAmount;

                    float pointsEarned = 0.02f * d; // 2% of individual item cost
                    float totalPoints  = pointsEarned * quantity;
                    points += totalPoints;

                    fio.write((char*)&customerObject, sizeof(customerObject));
                }
            }
            fio.close();
        }
    }
    finStream.close();
}

//--------------------------------------------------------------------------------------
void creditCard()
{
    fstream em;
    char    createMember;

    drawDesign(45, '*');
    gotoxy(1,5);
    cout << "\n Do you want to take membership (y/n) ? ";
    cin  >> createMember;

    em.open("costumer.txt", ios::app | ios::out | ios::in);
    while(createMember == 'y' || createMember == 'Y')
    {
        customerObject.getdata();
        em.write(reinterpret_cast<char*>(&customerObject), sizeof(customerObject));
        cout << "\n Do you want to create more credit card no. (y/n) ? ";
        cin  >> createMember;
    }
    em.close();

    system("cls");
    cout << "\n\t\t============================================================";
    cout << "\n\t\t\t\t     The Customer ID detail :";
    cout << "\n\t\t============================================================\n";

    em.open("costumer.txt", ios::app | ios::out | ios::in);
    em.read(reinterpret_cast<char*>(&customerObject), sizeof(customerObject));
    while(!em.eof())
    {
        customerObject.display();
        cout << endl << endl;
        em.read(reinterpret_cast<char*>(&customerObject), sizeof(customerObject));
    }
    getch();
    em.close();
}

//--------------------------------------------------------------------------------------
// Bill Calculation
void bill::billcalc(int creditParam, int quantityParam, int itemIDParam)
{
    int crdta    = creditParam;
    int quantity = quantityParam;
    int idno     = itemIDParam;

    fstream em, eima, eimb;
    em.open("bill.txt",        ios::app | ios::out | ios::binary | ios::in);
    eima.open("costumer.txt",  ios::app | ios::out | ios::binary | ios::in);
    eimb.open("itemstore.dat", ios::app | ios::out | ios::binary | ios::in);

    if(!em)
    {
        cout << "\n\nFile Not Found...\nProgram Terminated!";
    }
    while(eimb.read(reinterpret_cast<char*>(&productItem), sizeof(productItem)))
    {
        if(idno == productItem.itemNumber)
        {
            strcpy(productName, productItem.itemName);
            productID     = productItem.itemNumber;
            singlePrice   = productItem.individualAmount;
        }
    }

    while(eima.read(reinterpret_cast<char*>(&customerObject), sizeof(customerObject)))
    {
        if(crdta == customerObject.creditID)
        {
            strcpy(customerName,        customerObject.name);
            strcpy(customerCitizenship, customerObject.citizenship);
            customerCreditID           = customerObject.creditID;
        }
    }

    this->quantity  = quantity;
    totalCost       = singlePrice * quantity;

    em.write(reinterpret_cast<char*>(&billObject), sizeof(billObject));

    em.close();
    eima.close();
    eimb.close();

    drawDesign(45, '*');
    disp();
}

//--------------------------------------------------------------------------------------
void ProductItem::report1()
{
    BaseItem::report();
    gotoxy(23, cursorY);
    cout << markPrice;
    gotoxy(33, cursorY);
    cout << quantityInStock;
    gotoxy(44, cursorY);
    cout << taxPercent;
    gotoxy(52, cursorY);
    cout << discountRate;
    gotoxy(68, cursorY);
    cout << individualAmount;

    cursorY = cursorY + 1;
    if(cursorY == 50)
    {
        gotoxy(25, 50);
        cout << "PRESS ANY KEY TO CONTINUE...";
        getch();
        cursorY = 7;
        system("cls");
        gotoxy(30,3);
        cout << " ITEM DETAILS ";
        gotoxy(3,5);
        cout << " ID NUMBER";
        gotoxy(13,5);
        cout << "NAME";
        gotoxy(23,5);
        cout << "PRICE";
        gotoxy(33,5);
        cout << "QUANTITY";
        gotoxy(44,5);
        cout << "TAX";
        gotoxy(52,5);
        cout << "DEDUCTION";
    }
}

void ProductItem::pay1(int itemChoice, int creditNumber)
{
    drawDesign(45, '*');

    finStream.read((char*)&productItem, sizeof(productItem));
    BaseItem::show();
    cout << "\n\n\n\t\t*********************************************";
    cout << "\n\t\t                 DETAILS                  ";
    cout << "\n\t\t*********************************************";
    cout << "\n\n\t\tPRICE                       :" << markPrice;
    cout << "\n\n\t\tQUANTITY                    :" << quantityInStock;
    cout << "\n\t\tTAX PERCENTAGE              :" << taxPercent;
    cout << "\n\t\tDISCOUNT PERCENTAGE         :" << discountRate;
    cout << "\n\t\tINDIVIDUAL AMOUNT           :Rs." << individualAmount;
    cout << "\n\t\t*********************************************";
    getch();
    cout << "\n\n\t\tinput the quantity you want to buy ";
    int buyQuantity;
    cin >> buyQuantity;

    // Reduce quantity in the file
    fstream fio;
    fio.open("itemstore.dat", ios::in | ios::out | ios::binary);
    while(fio.read((char*)&productItem, sizeof(productItem)))
    {
        int pos = fio.tellg();
        if(itemChoice == productItem.BaseItem::getItemNumber())
        {
            fio.seekg(pos - sizeof(productItem));
            quantityInStock -= buyQuantity;
            fio.write((char*)&productItem, sizeof(productItem));
        }
    }
    fio.close();

    // Update the bill and customer details
    billObject.billcalc(creditNumber, buyQuantity, itemChoice);
    if(creditNumber != 0)
        customerObject.calc(creditNumber, buyQuantity, itemChoice);
}

//--------------------------------------------------------------------------------------
void userMenu()
{
    drawDesign(45, '*');
    int creditID = 0;
    char hasCredit;
    cout << endl << "\n\t\t Do you have credit no.(y/n) ? ";
    cin  >> hasCredit;
    if(hasCredit == 'y' || hasCredit == 'Y')
    {
        cout << endl << " Input your Customer ID number : ";
        cin  >> creditID;
    }

buyMenuStart:
    drawDesign(45, '*');
    gotoxy(39,3);
    cout << " BILL DETAILS ";
    gotoxy(1,5);
    cout << "ITEM ID";
    gotoxy(13,5);
    cout << "NAME";
    gotoxy(23,5);
    cout << "MRP";
    gotoxy(33,5);
    cout << "QUANTITY";
    gotoxy(44,5);
    cout << "TAX %";
    gotoxy(52,5);
    cout << "DISCOUNT %";
    gotoxy(68,5);
    cout << "PRICE";

    finStream.open("itemstore.dat", ios::binary);
    if(!finStream)
    {
        cout << "\n\nFile Not Found...";
    }
    finStream.seekg(0);
    cursorY = 7;
    while(!finStream.eof())
    {
        finStream.read((char*)&productItem, sizeof(productItem));
        if(!finStream.eof())
        {
            productItem.report1();
        }
    }
    gotoxy(17, cursorY);
    getch();
    finStream.close();

    // Prompt user to buy an item
    fstream em("itemstore.dat", ios::binary | ios::app | ios::out | ios::in);
    int productChoice;
    cout << "\n Input the product's identification no. you want to buy :";
    cin  >> productChoice;
    if(!em)
    {
        cout << "\n\nFile Not Found...\nProgram Terminated!";
    }

    em.seekg(0);
    foundFlag = 0;
    while(em.read((char*)&productItem, sizeof(productItem)))
    {
        int x = productItem.BaseItem::getItemNumber();
        if(x == productChoice)
        {
            productItem.pay1(productChoice, creditID);
            foundFlag = 1;
            break;
        }
    }
    if(foundFlag == 0)
    {
        cout << "\n\t\tItem does not exist....Please Retry!";
        getch();
    }

    cout << "\n\n press Y to continue and N to exit ? ";
    cin  >> hasCredit;
    if(hasCredit == 'y' || hasCredit == 'Y')
    {
        goto buyMenuStart;
    }
    else
    {
        em.close();
    }
}
#   B i l l T u n e  
 