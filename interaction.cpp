//Name: Jenil Shah

#include<iostream>
#include<occi.h>
#include<string>

using oracle::occi::Environment;
using oracle::occi::Connection;
using namespace oracle::occi;
using namespace std;

struct ShoppingCart //Shopping Cart Structure to store the item
{
	int product_id;
	double price;
	int quantity;
};

int mainMenu(); //Function to display the main menu
int customerLogin(Connection* conn, int customerId);  //Checks if customer exists in Database
int addToCart(Connection* conn, struct ShoppingCart cart[]);  //
double findProduct(Connection* conn, int product_id);  //checks if product exists in Database
void displayProducts(struct ShoppingCart cart[], int productCount); //Display the ordered items
int checkout(Connection* conn, struct ShoppingCart cart[], int customerId, int productCount);  //Function to add order and order items


int main(void)
{
	//OCCi Varaiables
	Environment* env = nullptr;  //Environment Variable
	Connection* conn = nullptr;  //Connection Variable

	//User Variable
	string str;
	string usr = "Enter the User ID";
	string pass = "Enter the Password";
	string srv = "Enter the connection string";

	try
	{
		env = Environment::createEnvironment(Environment::DEFAULT);
		conn = env->createConnection(usr, pass, srv);

		//Starting part of the Application
		while (mainMenu())  //Loop will continue until user exits
		{
			//Prompts the user to enter customer ID
			int custId;
			cout << "Enter the customer ID: ";
			cin >> custId;

			//checks whether customer exists or not
			if (!customerLogin(conn, custId))
				cout << "The customer does not exist." << endl;

			else
			{
				struct ShoppingCart cart[5];  //Declaring array of structure ShoppingCart
				int pCount = addToCart(conn, cart);  //return the product count
				displayProducts(cart, pCount);  //displays the product
				int val = checkout(conn, cart, custId, pCount);

				if (val)
					cout << "The order is successfully completed." << endl;
				else
					cout << "The order is cancelled." << endl;
			}
		}
		cout << "Good bye!..." << endl;
		env->terminateConnection(conn);  //Terminate the Connection
		Environment::terminateEnvironment(env);
	}

	catch (SQLException& sqlExcp)  //Catch if any error occurs with error code and error message
	{
		cout << sqlExcp.getErrorCode() << ": " << sqlExcp.getMessage();
	}

	return 0;
}

int mainMenu()
{
	int option;
	bool check = false;

	//Loop will continue until user enters correct input
	do
	{
		cout << "******************** Main Menu ********************" << endl;
		cout << "1)      Login" << endl;
		cout << "0)      Exit" << endl;

		if (check)
			cout << "You entered a wrong value. ";  //displays the message if user enters value other than 0 or 1
		cout << "Enter an option (0-1): ";
		cin >> option;

		if (option < 0 || option > 1)
			check = true;
	} while (option < 0 || option > 1);

	return option;
}

int customerLogin(Connection* conn, int customerId)
{
	int val = 0;

	try
	{
		Statement* stmt = conn->createStatement();

		stmt->setSQL("BEGIN find_customer(:1, :2); END;");  //call procedure find_customers

		stmt->setNumber(1, customerId);
		stmt->registerOutParam(2, Type::OCCIINT, sizeof(val));

		stmt->executeUpdate();  //Execuiton of the statement
		val = stmt->getInt(2);

		conn->terminateStatement(stmt);  //Termination of the statement
	}

	catch (SQLException & excp)  //catch if any error is there
	{
		throw excp;
	}
		
	return val;
}

int addToCart(Connection* conn, struct ShoppingCart cart[])
{
	cout << "-------------- Add Products to Cart --------------" << endl;

	int count = 0;
	bool checkout = false;
	int prodId;
	double price = 0;
	int qty;
	
	//Loop continues till either 5 items are added or user checkout
	do
	{
		//Enter the product ID
		cout << "Enter the product ID: ";
		cin >> prodId;

		try
		{
			price = findProduct(conn, prodId);  //call find product function to check if customer product exists in database

			if (!price)
				cout << "The product does not exists. Try again..." << endl;

			else
			{
				cout << "Product Price: " << price << endl;
				cout << "Enter the product Quantity: ";
				cin >> qty;

				//store item description in cart
				cart[count].product_id = prodId;
				cart[count].price = price;
				cart[count].quantity = qty;

				count++;

				//prompts user to enter more products
				cout << "Enter 1 to add more products or 0 to checkout: ";
				int choose;
				cin >> choose;

				if (!choose)
					checkout = true;
			}

		}

		catch (SQLException & excp)  //catch if any error is there
		{
			throw excp;
		}
	} while (count < 5 && !checkout);
	
	return count;
}

double findProduct(Connection* conn, int product_id)
{
	double price = 0;

	try
	{
		Statement* stmt = conn->createStatement();

		stmt->setSQL("BEGIN find_product(:1, :2); END;");  //call the procedure to check whether product exists or not

		stmt->setNumber(1, product_id);
		stmt->registerOutParam(2, Type::OCCIDOUBLE, sizeof(price));

		stmt->executeUpdate();  //execution of the statement
		price = stmt->getDouble(2);  //store value of the output parameter

		conn->terminateStatement(stmt);
	}

	catch (SQLException & excp)  //Catch any error if occurs
	{
		throw excp;
	}

	return price;
}

void displayProducts(struct ShoppingCart cart[], int productCount)
{
	cout << "------- Ordered Products ---------" << endl;

	double totalPrice = 0;

	//Loop to display the products
	for (int i = 0; i < productCount; i++)
	{
		cout << "---Item " << (i + 1) << endl;
		cout << "Product ID: " << cart[i].product_id << endl;
		cout << "Price: " << cart[i].price << endl;
		cout << "Quantity: " << cart[i].quantity << endl;

		totalPrice += (cart[i].price * cart[i].quantity);  //calculate the total price of the order
	}

	cout << "----------------------------------" << endl;
	cout << "Total: " << totalPrice << endl;
}

int checkout(Connection* conn, struct ShoppingCart cart[], int customerId, int productCount)
{
	string str;
	bool check = false;
	int returningValue = 0;
	do
	{
		cout << "Would you like to checkout? (Y/y or N/n) ";
		cin >> str;
		
		if (str == "y" || str == "Y")
		{
			int newOrderId = 0;

			try
			{
				Statement* stmt = conn->createStatement();

				stmt->setSQL("BEGIN add_order(:1, :2); END;");  //Procedure to insert order in the database

				stmt->setNumber(1, customerId);
				stmt->registerOutParam(2, Type::OCCIINT, sizeof(newOrderId));

				stmt->executeUpdate();
				newOrderId = stmt->getInt(2);  //store value of the output parameter

				conn->terminateStatement(stmt);
			}

			catch (SQLException & excp)  //catch errors
			{
				throw excp;
			}

			for (int i = 0; i < productCount; i++)
			{
				try
				{
					Statement* stmt = conn->createStatement();

					stmt->setSQL("BEGIN add_order_item(:1, :2, :3, :4, :5); END;");  //calling procedure to add order items

					stmt->setNumber(1, newOrderId);
					stmt->setNumber(2, (i + 1));
					stmt->setNumber(3, cart[i].product_id);
					stmt->setNumber(4, cart[i].quantity);
					stmt->setNumber(5, cart[i].price);

					stmt->executeUpdate();

					conn->terminateStatement(stmt);
				}

				catch (SQLException & excp)
				{
					throw excp;
				}
			}

			returningValue = 1;
			check = false;
		}

		else if (str == "n" || str == "N")
		{
			returningValue = 0;
			check = false;
		}

		else
		{
			//prompts error message if user enters wrong value
			cout << "Wrong input. Try again..." << endl;
			check = true;
		}
	} while (check);

	return returningValue;
}
