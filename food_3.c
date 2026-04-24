#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define nmax 100
#define lmax 1024
#define rate 100
#define max_menu 10
#define maxq 10
#define max_order 10

typedef struct
{
	char title[nmax];
	char filename[nmax];
} Menu;

Menu *m;

typedef struct order
{
	int code;
	char name[nmax];
	int quantity;
	double price;
} Order;

Order *ord;

char date[11];

int token = 1;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Date()
{
	time_t t = time(NULL);
	struct tm today = *localtime(&t);
	
	int day = today.tm_mday;
	int month = today.tm_mon + 1;
	int year = today.tm_year + 1900;
	
	sprintf(date, "%02d/%02d/%04d", day, month, year);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Token()
{
	FILE *order = fopen("orders.csv", "r");
	if (order == NULL)
	{
		printf("Error9");
	}
	
	char lastdate[11];
	int lasttoken = 0;

	char line[lmax];
    char lastline[lmax] = "";

    while (fgets (line, sizeof (line), order))
	{
        strcpy(lastline, line);
    }
	
	fclose (order);
	
	if (strlen (lastline) == 0)
	{
		token = 1;
		
		return;
	}
	
	sscanf(lastline, "%10[^,],%d", lastdate, &lasttoken);
	
	if (strcmp (lastdate, date) == 0)
	{
		token = lasttoken + 1;
	}
	else
	{
		token = 1;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int LoadMenu()
{
	char line[lmax];
	int i = 0;
	FILE *file = fopen("menu_all.csv", "r");
	if (file == NULL)
	{
		printf("Error1");
	}

	while (fgets (line, sizeof(line), file))
	{
		sscanf(line, "%99[^,],%99[^\n]", m[i].title, m[i].filename);
		i++;
	}
	
	fclose(file);
	
	return i;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void DisplayMenu(int count)
{	
	char line[lmax];
	int code = 0;
	char name[nmax];
	double price = 0.0;
	
	for (int i = 1; i < count; i++)
	{
		printf("\n%s\n", m[i].title);
		
		FILE *mn;
		mn = fopen(m[i].filename, "r");
		if (mn == NULL)
		{
			printf("Error2");
		}
		
		//skipping header row
		fgets(line, sizeof(line), mn);
		
		while (fgets (line, sizeof (line), mn))
		{
			sscanf(line, "%d,%99[^,],%lf", &code, name, &price);
			printf("%-5d %-50s %-10.2f\n", code, name, price);
		}
		
		fclose(mn);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int Account(int num, int credit)
{
	FILE *acc;
	
	acc = fopen("accounts.csv", "a");
	if (acc == NULL)
	{
		printf("Error3");
	}
	
	fprintf(acc, "%d,%d\n", num, credit);
	
	if (credit == 0)
	{
		printf("\nYou currently have no credit points.\n");
		fclose(acc);
		return 0;
	}
	else
	{
		printf("\nYou have %d credit points.\n", credit);
		fclose(acc);
		return 1;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int Credits(int num, int check1, int bill)
{
	FILE *acc = fopen("accounts.csv", "r");
	FILE *temp = fopen("temp.csv", "w");
	if (acc == NULL || temp == NULL)
	{
		printf("Error4");
		return -1;
	}
	
	char line[lmax];
	int found = 0;
	int n = 0, cred = 0, credit = 0;
	
	while (fgets (line, sizeof (line), acc))
	{
		sscanf(line, "%d,%d", &n, &cred);
		
		if (n == num)
		{
			found = 1;
			
			if (check1 == 0)
			{
				fclose(acc);
				fclose(temp);
				
				return cred;
			}
		}
	}
	
	if (found == 1 && check1 == 1)
	{
		rewind (acc);
		
		while (fgets (line, sizeof (line), acc))
		{
			sscanf (line, "%d,%d", &n, &cred);
			
			if (n != num)
			{
				fputs(line, temp);
			}
		}
		
		credit = (bill / rate);
		fprintf(temp, "%d,%d\n", num, credit);
		
		fclose(acc);
		fclose(temp);

		remove("accounts.csv");
		rename("temp.csv", "accounts.csv");
		
		return 1;
	}
	
	fclose(acc);
	fclose(temp);
	
	if (found == 0)
	{
		acc = fopen("accounts.csv", "a");
		
		if (check1 == 0)
		{
			credit = 0;
			
			if (bill != 0)
			{
				fprintf(acc, "%d,%d\n", num, credit);
			}
			
			fclose(acc);
			
			return credit;
		}
		if (check1 == 1)
		{
			credit = (bill / rate);
			fprintf(acc, "%d,%d\n", num, credit);
			
			fclose(acc);
			
			return 0;
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int AcceptCode(int count)
{
	printf("\nEnter Item Code\n");
	int code = 0;
	scanf("%d", &code);
	
	int c = 0;
	char line[lmax];
	
	for (int i = 1; i < count; i++)
	{	
		FILE *mn;
		mn = fopen(m[i].filename, "r");
		if (mn == NULL)
		{
			printf("Error5");
		}
		
		//skipping header row
		fgets(line, sizeof(line), mn);
		
		while (fgets (line, sizeof (line), mn))
		{
			sscanf(line, "%d", &c);
			
			if (c == code)
			{
				return code;
			}
		}
		
		fclose(mn);
	}
	
	return -1;
}
	
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int Quantity()
{
	int quantity = 0;
	
	do
	{
		printf("\nEnter the quantity. (LIMIT: 25)\n");
		scanf("%d", &quantity);
		while (getchar() != '\n');
		
		if (quantity <= 0 || quantity > 25)
		{
			printf("\nInvalid Input\n");
		}
	}
	while (quantity <= 0 || quantity > 25);
	
	return quantity;
}
	
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int Index(int i, int code)
{
	for (int j = 0; j < i; j++)
	{
		if (ord[j].code == code)
		{
			return j;
		}
	}
	
	return -1;
}

int SaveOrder(int i, int code, int quantity, int count)
{
	char line[lmax];
	int c = 0;
	char name[nmax];
	double price = 0.0;
	
	ord[i].code = code;
	
	for (int j = 1; j < count; j++)
	{	
		FILE *mn;
		mn = fopen(m[j].filename, "r");
		if (mn == NULL)
		{
			printf("Error7");
		}
		
		//skipping header row
		fgets(line, sizeof(line), mn);
	
		while (fgets (line, sizeof (line), mn))
		{
			sscanf(line, "%d,%99[^,],%lf", &c, name, &price);
			
			if (c == code)
			{
				strcpy(ord[i].name, name);
				
				ord[i].price = price;
			}
		}
	
		fclose(mn);
	}
	
	int index = Index(i, code);
	
	if (index == -1)
	{
		ord[i].quantity = quantity;
		return 1;
	}
	else
	{
		ord[index].quantity = ord[index].quantity + quantity;
		return 0;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void DisplayOrder(int i)
{
	printf("\nYour Order\n");
	
	for (int j = 0; j <= i; j++)
	{
		if (ord[j].quantity != 0)
		{
			printf("%-50s %-5dRs. %-10.2f\n", ord[j].name, ord[j].quantity, (ord[j].quantity * ord[j].price));
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ConfirmOrder(int i, int num)
{
	FILE *order = fopen("orders.csv", "a");
	if (order == NULL)
	{
		printf("Error8");
	}
	
	for (int j = 0; j <= i; j++)
	{
		if (ord[j].quantity != 0)
		{
			fprintf(order, "%s,%d,%d,%d,%s,%d,%.2lf\n", 
			date, token, num, ord[j].code, ord[j].name, ord[j].quantity, (ord[j].quantity * ord[j].price));
		}
	}
	
	printf("Order Confirmed");
	
	fclose(order);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CancelOrder(int i)
{
	for (int j = 0; j < i; j++)
		{
			ord[j].quantity = 0;
		}

		printf("Order Canceled");
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Bill(int num)
{
	FILE *order = fopen("orders.csv", "r");
	if (order == NULL)
	{
		printf("Error10");
	}
	
	char d[11], nm[nmax], line[lmax];
	int t, n, c, q;
	double sum = 0.0;
	
	double bill = 0.0;
	
	while (fgets (line, sizeof (line), order))
	{
		sscanf(line, "%10[^,],%d,%d,%d,%99[^,],%d,%lf", d, &t, &n, &c, nm, &q, &sum);
		if (n == num && strcmp(d, date) == 0 && t == token)
		{
			bill = bill + sum;
		}
	}
	
	fclose(order);
	
	printf("\n");
	printf("%-55s Rs. %-10.2lf\n", "CGST 2.5%: ", (0.025 * bill));
	printf("%-55s Rs. %-10.2lf\n", "SGST 2.5%: ", (0.025 * bill));
	printf("%-55s Rs. %-10.2lf\n", "Total Tax: ", (0.05 * bill));
	
	double credit = Credits(num, 0, 0);
	printf("%-55s Rs. %-10.2lf\n", "Credit Applied: ", -credit);
	
	double tbill = bill + (0.05 * bill) - credit;
	printf("%-55s Rs. %-10.2lf\n", "Your Total Bill is: ", tbill);
	
	Credits(num, 1, bill);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int main()
{
	Date();
	
	Token();
	
	while(1)
	{
		printf("\nWELCOME\n");
		
		printf("\nEnter 1 to place order\n");
		printf("Enter 2 to view credit points\n");
		int option = 0;
		scanf("%d", &option);
		
		printf("\nEnter mobile number\n");
		int num = 0;
		scanf("%d", &num);
		
		switch(option)
		{
			case 1:
			{
				m = (Menu *)malloc(sizeof(Menu) * max_menu);
				ord = (Order *)malloc(sizeof(Order) * max_order);
				if (!m || !ord)
				{
					printf("Memory allocation failed");
					return -1;
				}
				
				int count = LoadMenu();
				DisplayMenu(count);
				
				char choice;
				int i = 0;
				
				int code = 0;
				int quantity = 0;
				
				do
				{
					do
					{
						code = AcceptCode(count);
						if (code == -1)
						{
							printf("\nInvalid Input\n");
						}
					}
					while (code == -1);
					
					quantity = Quantity();
					
					int check = SaveOrder(i, code, quantity, count);
					
					do
					{
						printf("\nWould you like to order more? (y/n)\n");
						scanf(" %c", &choice);
						
						if (choice != 'y' && choice != 'n')
						{
							printf("\nInvalid Input\n");
						}
					}
					while (choice != 'y' && choice != 'n');
					
					if (choice == 'y')
					{
						i++;
					}
				}
				while (choice == 'y');
				
				SaveOrder(i, code, quantity, count);
				DisplayOrder(i);
				
				printf("\nEnter 1 to Confirm Order\n");
				printf("Enter 2 to Cancel Order\n");
				int option1 = 0;
				scanf("%d", &option1);
				
				switch(option1)
				{
					case 1: 
					{
						ConfirmOrder(i, num);
						printf("\n\n");
						DisplayOrder(i);
						Bill(num); 
						break;
					}
					
					case 2: CancelOrder(i); break;
				}
				
				free(m);
				free(ord);
				
				break;
			}
			
			case 2:
			{	
				int credit = Credits(num, 0, 0);
				
				int r = Account(num, credit);
				
				break;
			}
		}
	}
	return 0;
}